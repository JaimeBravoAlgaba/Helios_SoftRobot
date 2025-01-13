# Description: This script reads a CSV file with the following columns:	qy, qz, h0, h1, h2, h3, h_avg
#               and plots the data in 3 subplots: Euler angles Y and Z, Helios measurements, Helios average
#               It also plots the differential measurements h1-h3 and h0-h2 vs the Euler angles Y and Z
#               Finally, it exports the data to a MATLAB .mat file with the following variables: qy, qz, h0, h1, h2, h3, h_avg
#

import pandas as pd
import matplotlib.pyplot as plt
from math import pi, sqrt, atan2
from pathlib import Path
import scipy.io

def normalize(*args):
    x_avg = sum(args[0]) / len(args[0])

    x_norm = [(x - x_avg) / x_avg for x in args]
    x0 = [x[0] for x in x_norm]
    x_norm = [x - x[0] for x in x_norm]

    return x_norm, x_avg, x0

def get_data(file_path):
    # Read the data
    data = pd.read_csv(file_path, delimiter=',')
    
    # Get data as arrays
    l0 = data.iloc[:, 0] # TOF distance l0
    l1 = data.iloc[:, 1] # TOF distance l1
    l2 = data.iloc[:, 2] # TOF distance l2
    l3 = data.iloc[:, 3] # TOF distance l3    

    h0 = data.iloc[:, 4] # Helios measurement 0
    h1 = data.iloc[:, 5] # Helios measurement 1
    h2 = data.iloc[:, 6] # Helios measurement 2
    h3 = data.iloc[:, 7] # Helios measurement 3

    h, h_avg, h0 = normalize(h0, h1, h2, h3)
    l, l_avg, l0 = normalize(l0, l1, l2, l3)

    return h, l, h_avg, l_avg, h0, l0

def plot_data(l, h, l_avg, h_avg):
    # Plot the TOF distances
    plt.figure(figsize=(10, 6))
    plt.plot(l[0], label='TOF distance l0')
    plt.plot(l[1], label='TOF distance l1')
    plt.plot(l[2], label='TOF distance l2')
    plt.plot(l[3], label='TOF distance l3')
    
    plt.xlabel('Sample Index')
    plt.ylabel('TOF Distance')
    plt.title('Evolution of TOF Distances')
    plt.legend()
    plt.grid(True)

    # Plot the Helios measurements
    plt.figure(figsize=(10, 6))
    plt.plot(h[0], label='Helios measurement 0')
    plt.plot(h[1], label='Helios measurement 1')
    plt.plot(h[2], label='Helios measurement 2')
    plt.plot(h[3], label='Helios measurement 3')

    plt.xlabel('Sample Index')
    plt.ylabel('Helios Measurement')
    plt.title('Evolution of Helios Measurements')
    plt.legend()
    plt.grid(True)

    # Calculate the differential measurements
    diff_l2_l0 = l[2] - l[0]
    diff_l3_l1 = l[3] - l[1]

    # Plot the differential measurements
    plt.figure(figsize=(10, 6))
    plt.plot(diff_l2_l0, label='Differential l2 - l0')
    plt.plot(diff_l3_l1, label='Differential l3 - l1')

    plt.xlabel('Sample Index')
    plt.ylabel('Differential TOF Distance')
    plt.title('Differential TOF Distances')
    plt.legend()
    plt.grid(True)

    # Calculate the differential measurements for Helios
    diff_h2_h0 = h[2] - h[0]
    diff_h3_h1 = h[3] - h[1]

    # Plot the differential measurements for Helios
    plt.figure(figsize=(10, 6))
    plt.plot(diff_h2_h0, label='Differential h2 - h0')
    plt.plot(diff_h3_h1, label='Differential h3 - h1')
    
    plt.xlabel('Sample Index')
    plt.ylabel('Differential Helios Measurement')
    plt.title('Differential Helios Measurements')
    plt.legend()
    plt.grid(True)

    # Plot the average Helios measurement and TOF distance in the same figure
    plt.figure(figsize=(10, 12))

    # Subplot for average Helios measurement
    plt.subplot(2, 1, 1)
    plt.plot(h_avg, label='Average Helios measurement')
    plt.xlabel('Sample Index')
    plt.ylabel('Average Measurement')
    plt.title('Average Helios Measurement')
    plt.legend()
    plt.grid(True)

    # Subplot for average TOF distance
    plt.subplot(2, 1, 2)
    plt.plot(l_avg, label='Average TOF distance')
    plt.xlabel('Sample Index')
    plt.ylabel('Average TOF Distance')
    plt.title('Average TOF Distance')
    plt.legend()
    plt.grid(True)

if __name__ == '__main__':
    # File path
    file_path = Path('./dataset/250109/0x4A_250109_445_1.csv')

    # Plot the data
    h, l, h_avg, l_avg, h0, l0 = get_data(file_path)
    plot_data(l, h, l_avg, h_avg)
    plt.show()

    print(h0)
    print(l0)