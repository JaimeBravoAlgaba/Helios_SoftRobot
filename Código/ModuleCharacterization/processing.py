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

def plot_csv_file(file_path):
    # Read the data
    data = pd.read_csv(file_path, delimiter=';')
    
    # Get data as arrays
    qy = data.iloc[:, 0] # Euler angle Y
    qz = data.iloc[:, 1] # Euler angle Z

    h0 = data.iloc[:, 2] # Helios measurement 0
    h1 = data.iloc[:, 3] # Helios measurement 1
    h2 = data.iloc[:, 4] # Helios measurement 2
    h3 = data.iloc[:, 5] # Helios measurement 3

    h_avg = data.iloc[:, 6] # Helios average

    # PLOT 1: Euler angles Y and Z, Helios measurements, Helios average
    fig, axs = plt.subplots(3, 1, figsize=(10, 10), facecolor='white')
    
    # Set the font to arial
    plt.rcParams['font.sans-serif'] = 'Arial'
    plt.rcParams.update({'font.size': 16})

    # Plot Euler angles Y and Z
    axs[0].plot((qy-0.5)*120, label='qY')
    axs[0].plot((qz-0.5)*120, label='qZ')
    axs[0].set_ylabel('Ángulo [grados]', fontsize=16)
    axs[0].legend()   

    # Plot Helios measurements
    axs[2].plot(h0-h_avg, label='h0')
    axs[2].plot(h1-h_avg, label='h1')
    axs[2].plot(h2-h_avg, label='h2')
    axs[2].plot(h3-h_avg, label='h3')
    axs[2].set_ylabel('Medición', fontsize=16)
    axs[2].legend()   

    # Plot Helios measurements
    axs[1].plot(h1-h3, label='h1-h3')
    axs[1].plot(h0-h2, label='h0-h2')
    axs[2].set_xlabel('Muestra', fontsize=16)
    axs[1].set_ylabel('Medición diferencial', fontsize=16)
    axs[1].legend()   

    # Plot Helios average
    #axs[2].plot(h_avg, label='h_avg}')
    #axs[2].set_xlabel('Muestra', fontsize=16)
    #axs[2].set_ylabel('Valor promedio', fontsize=16)

    for ax in axs:  
        ax.grid(True)  
        ax.tick_params(axis='both', which='major', labelsize=14)

    # 3D plot of h0, h2 vs qy
    fig = plt.figure()
    ax = fig.add_subplot(121, projection='3d')
    ax.scatter(h0- h2, h1-h3, (qy-0.5)*120, c=h_avg, cmap='inferno')
    ax.set_xlabel('h0-h2')
    ax.set_ylabel('h1-h3')
    ax.set_zlabel('qy')

    # 3D plot of h1, h3 vs qz
    ax = fig.add_subplot(122, projection='3d')
    ax.scatter(h0- h2, h1-h3, (qz-0.5)*120, c=h_avg, cmap='inferno')
    ax.set_xlabel('h0-h2')
    ax.set_ylabel('h1-h3')
    ax.set_zlabel('qz')
    plt.show()

    # export a MATLAB .mat file with the following variables: qy, qz, h0, h1, h2, h3, h_avg
    data_dict = {
        'qy': qy,
        'qz': qz,
        'h0': h0,
        'h1': h1,
        'h2': h2,
        'h3': h3,
        'h_avg': h_avg
    }

    # Save the data to a .mat file
    file_path = file_path.with_suffix('.mat')
    scipy.io.savemat(file_path, data_dict)

if __name__ == '__main__':
    # File path
    file_path = Path('./dataset/240823/0x40_240823_1.csv')

    # Plot the data
    plot_csv_file(file_path)
    # Show the plot
    plt.show()