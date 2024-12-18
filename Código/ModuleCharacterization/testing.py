import numpy as np
import serial
import time

from matplotlib import pyplot as plt
import pandas as pd
from keras import models as km

from processing_tof import get_data
from test_model_ai import denormalize

def tofs2pcc(l, D):
    theta = np.zeros((len(l), 1))
    phi = np.zeros((len(l), 1))
    length = np.zeros((len(l), 1))

    for i in range(len(l)):
        thetaX = np.arctan2(l[i][2]-l[i][0], D)
        thetaY = np.arctan2(l[i][3]-l[i][1], D)
        theta[i] = np.sqrt(thetaX**2 + thetaY**2)
        phi[i] = np.arctan2(thetaY, thetaX)
        if theta[i] < 1e-6:
            length[i] = np.mean(l[i])
        else:
            length[i] = theta[i] * np.mean(l[i]) * np.tan(np.pi/2 - theta[i])
    
    return theta, phi, length

def iKine(coords):
    coords['phi'] = coords['phi'] + np.pi if coords['theta'] < 0 else coords['phi']
    coords['theta'] = 0.0001 if abs(coords['theta']) < 1E-4 else abs(coords['theta'])
    coords['length'] = 0.065 if coords['length'] > 0.065 else coords['length']

    SEGMENTS_NUM = 1  # Example value, replace with actual
    SEGMENTS_RC = 0.0225    # Example value, replace with actual
    cableOffsets = [0, np.pi/2, np.pi, -np.pi/2]  # Example values, replace with actual

    cable_lengths = []
    for i in range(len(cableOffsets)):
        length = 2 * np.sin(coords['theta'] / (2.0 * SEGMENTS_NUM)) * (
            coords['length'] * SEGMENTS_NUM / coords['theta'] - SEGMENTS_RC * np.sin(coords['phi'] + cableOffsets[i])
        )
        cable_lengths.append(length)
    
    return cable_lengths

def wait_confirm(ser, expected_response="OK", max_iterations=1000):
    iterations = 0
    while iterations < max_iterations:
        if ser.in_waiting > 0:
            response = ser.readline().decode().strip()
            print(f">> {response}")
            if response == expected_response:
                return True
        time.sleep(0.1)
        iterations += 1
    return False

# List of PCC coordinates to loop over
pcc_coordinates_ref= [
    {'theta': np.pi/4, 'phi': 0, 'length': 0.0445},
    {'theta': np.pi/4, 'phi': 0, 'length': 0.0440},
    {'theta': 0, 'phi':0, 'length': 0.0440},
    {'theta': 0, 'phi':0, 'length': 0.035},
    {'theta': np.pi/4, 'phi': 0, 'length': 0.035},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.035},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.035},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.040},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.040},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.035},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.045}
]

# Increase pcc_coordinates_ref list with intermediary points when there is an increment in theta bigger than np.pi/36
n_int_points = 9
for i in range(len(pcc_coordinates_ref)-1):
    if pcc_coordinates_ref[i]['theta'] != pcc_coordinates_ref[i+1]['theta']:
        theta_step = (pcc_coordinates_ref[i+1]['theta'] - pcc_coordinates_ref[i]['theta']) / n_int_points
        phi_step = (pcc_coordinates_ref[i+1]['phi'] - pcc_coordinates_ref[i]['phi']) / n_int_points
        #length_step = (pcc_coordinates_ref[i+1]['length'] - pcc_coordinates_ref[i]['length']) / n_int_points
        length_step = 0

        for j in range(1, n_int_points):
            pcc_coordinates_ref.append({
                'theta': pcc_coordinates_ref[i]['theta'] + j * theta_step,
                'phi': pcc_coordinates_ref[i]['phi'] + j * phi_step,
                'length': pcc_coordinates_ref[i]['length'] + j * length_step
            })

# This script tests an existing model with a given dataset
model_file = 'models/nn/nn_0x48_V2.keras'
test_data = './dataset/241207/0x48_241207_5.csv'

h, l, h_avg, l_avg, h0, l0 = get_data(test_data)

# Load the model
model = km.load_model(model_file)

# Open serial port
ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

wait_confirm(ser, expected_response="START")

# Wait until the module is initialized
while True:
    try:

        # Send the "calibrate" command via serial
        ser.write("CALIBRATE".encode())
        print(f"<< CALIBRATE")
        if not wait_confirm(ser):
            raise TimeoutError("Failed to receive expected response 'OK' from the module.")

        # Initialize current cable lengths
        current_cable_lengths = [0.0445, 0.0445, 0.0445, 0.0445]

        for pcc_coordinates in pcc_coordinates_ref:            
            # Send the cable lengths via serial
            cable_lengths_str = ",".join([str(pcc_coordinates['theta']), str(pcc_coordinates['phi']), str(pcc_coordinates['length'])])
            cable_lengths_str = "REF_PCC:" + cable_lengths_str
            ser.write(cable_lengths_str.encode())
            time.sleep(0.1)
            print(f"<< {cable_lengths_str}")
            if not wait_confirm(ser):
                raise TimeoutError("Failed to receive expected response 'OK' from the module.")
            
            # Wait before sending the next set of coordinates
            time.sleep(1)

    finally:
        # Close the serial port
        ser.close()