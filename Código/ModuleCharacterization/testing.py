import numpy as np
import serial
import time

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

# List of PCC coordinates to loop over
pcc_coordinates_ref= [
    {'theta': np.pi/4, 'phi': 0, 'length': 0.065},
    {'theta': 0, 'phi':0, 'length': 0.065},
    {'theta': np.pi/4, 'phi': np.pi/2, 'length': 0.065},
    {'theta': 0, 'phi': np.pi/2, 'length': 0.065}
]

# Open serial port
ser = serial.Serial('COM3', 115200, timeout=1)
time.sleep(2)  # Wait for the serial connection to initialize

try:

    # Send the "calibrate" command via serial
    ser.write("calibrate".encode())
    while ser.in_waiting == 0:
        time.sleep(0.1)
    response = ser.readline().decode().strip()
    print(f"Sent: calibrate, Received: {response}")

    # Initialize current cable lengths
    current_cable_lengths = [0.065, 0.065, 0.065, 0.065]

    for pcc_coordinates in pcc_coordinates_ref:
        # Compute the needed increment of cable lengths
        cable_lengths = iKine(pcc_coordinates)
        delta_cable_lengths = [new - old for new, old in zip(cable_lengths, current_cable_lengths)]
        
        # Send the cable lengths via serial
        cable_lengths_str = ",".join([str(length) for length in delta_cable_lengths])
        ser.write(cable_lengths_str.encode())
        
        # Capture the readings answered by the module
        response = ser.readline().decode().strip()
        while ser.in_waiting == 0:
            time.sleep(0.1)
        response = ser.readline().decode().strip()
        print(f"Sent: {cable_lengths_str}, Received: {response}")

        current_cable_lengths = cable_lengths
        
        # Wait before sending the next set of coordinates
        time.sleep(1)

finally:
    # Close the serial port
    ser.close()