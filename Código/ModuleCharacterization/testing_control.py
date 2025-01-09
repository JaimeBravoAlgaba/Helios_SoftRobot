import json
import numpy as np
import serial
import time

from matplotlib import pyplot as plt

import platform
import csv

if platform.system() == 'Linux':
    com_port = '/dev/ttyUSB0'
elif platform.system() == 'Windows':
    com_port = 'COM5'
else:
    raise EnvironmentError('Unsupported platform')
baud_rate = 115200

# Open a CSV file to write the readings
csv_file = open('data.csv', mode='w', newline='')
csv_writer = csv.writer(csv_file)

def tofs2pcc(l, D=100):
    thetaX = np.arctan2(l[2]-l[0], D)
    thetaY = np.arctan2(l[3]-l[1], D)

    theta = np.sqrt(thetaX**2 + thetaY**2)
    phi = np.arctan2(thetaY, thetaX)

    if theta < 1e-6:
        length = np.mean(l[i])/1000
    else:
        length = theta * np.mean(l)/1000 * np.tan(np.pi/2 - theta)

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
    {'theta': 0, 'phi': 0, 'length': 0.0445},
    {'theta': np.pi/4, 'phi': 0, 'length': 0.0445},
    {'theta': np.pi/4, 'phi': 0, 'length': 0.040},
    {'theta': 0, 'phi':0, 'length': 0.040},
    {'theta': 0, 'phi':0, 'length': 0.035},
    {'theta': np.pi/4, 'phi': 0, 'length': 0.035},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.035},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.035},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.040},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.040},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.045},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.045}
]

# Increase pcc_coordinates_ref list with intermediary points when there is an increment in theta bigger than np.pi/36
n_int_points = 9
pcc_coordinates_points = []
for i in range(len(pcc_coordinates_ref)-1):
    if pcc_coordinates_ref[i]['theta'] != pcc_coordinates_ref[i+1]['theta'] or pcc_coordinates_ref[i]['phi'] != pcc_coordinates_ref[i+1]['phi'] or pcc_coordinates_ref[i]['length'] != pcc_coordinates_ref[i+1]['length']:
        theta_step = (pcc_coordinates_ref[i+1]['theta'] - pcc_coordinates_ref[i]['theta']) / n_int_points
        phi_step = (pcc_coordinates_ref[i+1]['phi'] - pcc_coordinates_ref[i]['phi']) / n_int_points
        #length_step = (pcc_coordinates_ref[i+1]['length'] - pcc_coordinates_ref[i]['length']) / n_int_points
        length_step = 0

        if pcc_coordinates_ref[i]['length'] != pcc_coordinates_ref[i+1]['length']:
            pcc_coordinates_points.append({
                'theta': pcc_coordinates_ref[i]['theta'],
                'phi': pcc_coordinates_ref[i]['phi'],
                'length': pcc_coordinates_ref[i]['length']
            })
            pcc_coordinates_points.append({
                'theta': pcc_coordinates_ref[i+1]['theta'],
                'phi': pcc_coordinates_ref[i+1]['phi'],
                'length': pcc_coordinates_ref[i+1]['length']
            })
            continue

        
        for j in range(1, n_int_points):
            pcc_coordinates_points.append({
                'theta': pcc_coordinates_ref[i]['theta'] + j * theta_step,
                'phi': pcc_coordinates_ref[i]['phi'] + j * phi_step,
                'length': pcc_coordinates_ref[i]['length'] + j * length_step
            })

# This script tests an existing model with a given dataset
#model_file = 'models/nn/nn_0x48_V2.keras'
#test_data = './dataset/241207/0x48_241207_5.csv'
#
#h, l, h_avg, l_avg, h0, l0 = get_data(test_data)
#
## Load the model
#model = km.load_model(model_file)

# Open serial port
ser = serial.Serial(com_port, baud_rate, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

wait_confirm(ser, expected_response="START")

try:
    # Send the "calibrate" command via serial
    ser.write("CALIBRATE".encode())
    print(f"<< CALIBRATE")
    if not wait_confirm(ser):
        raise TimeoutError("Failed to receive expected response 'OK' from the module.")

    pcc_coordinates = {'theta': 0, 'phi': 0, 'length': 0.0430}

    for p in np.arange(0, 2 * np.pi, np.pi / 4):
        pcc_coordinates['phi'] = p

        for t in np.arange(0, np.pi/4, np.pi / 32):
            pcc_coordinates['theta'] = t

            # Send the cable lengths via serial
            pcc_ref_str = ",".join([str(pcc_coordinates['theta']), str(pcc_coordinates['phi']), str(pcc_coordinates['length'])])
            pcc_ref_str= "REF_PCC:" + pcc_ref_str
            ser.write(pcc_ref_str.encode())
            print(f"<< {pcc_ref_str}")
            
            if not wait_confirm(ser):
                print("Failed to confirm. Retrying...")
                ser.write(pcc_ref_str.encode())
            else:
                # Capture the readings answered by the module
                response = ser.readline().decode().strip()

                # Parse the response 
                try:
                    response_dict = json.loads(response)
                except json.JSONDecodeError:
                    print("Failed to decode JSON response:" + response)
                    continue

                tof_values = response_dict['TOFS']
                helios_values = response_dict['HELIOS']

                print(f">> TOF: {tof_values}, HELIOS: {helios_values}")
                csv_writer.writerow([tof_values, helios_values])

        for t in np.arange(0, np.pi/4, np.pi / 32):
            pcc_coordinates['theta'] = np.pi/4 - t

            # Send the cable lengths via serial
            pcc_ref_str = ",".join([str(pcc_coordinates['theta']), str(pcc_coordinates['phi']), str(pcc_coordinates['length'])])
            pcc_ref_str= "REF_PCC:" + pcc_ref_str
            ser.write(pcc_ref_str.encode())
            print(f"<< {pcc_ref_str}")
            
            if not wait_confirm(ser):
                print("Failed to confirm. Retrying...")
                ser.write(pcc_ref_str.encode())
            else:
                # Capture the readings answered by the module
                response = ser.readline().decode().strip()

                # Parse the response 
                try:
                    response_dict = json.loads(response)
                except json.JSONDecodeError:
                    print("Failed to decode JSON response:" + response)
                    continue

                tof_values = response_dict['TOFS']
                helios_values = response_dict['HELIOS']

                print(f">> TOF: {tof_values}, HELIOS: {helios_values}")
                csv_writer.writerow([tof_values, helios_values])

finally:
    csv_file.close()
    print("Finished!")
    ser.close()