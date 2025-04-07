import numpy as np
from numpy import arange, sin, cos, sqrt, pi
import pandas as pd
import matplotlib.pyplot as plt

def normalize(*args):
    x_avg = sum(args[0]) / len(args[0])

    x_norm = [(x - x_avg) / x_avg for x in args]
    x0 = [x[0] for x in x_norm]
    x_norm = [x - x[0] for x in x_norm]

    return x_norm, x_avg, x0

def denormalize(x, x0, x_avg):
    for i in range(len(x)):
        x[i] = [(x[i][j] + x0[i] + 1) * x_avg for j in range(len(x[i]))]    
    return x

def get_data(file_path):
    # Read the data
    data = pd.read_csv(file_path, delimiter=',')
    
    # Get data as arrays
    l0 = data.iloc[:, 0] # TOF distance l0
    l1 = data.iloc[:, 1] # TOF distance l1
    l2 = data.iloc[:, 2] # TOF distance l2
    l3 = data.iloc[:, 3] # TOF distance l3

    # Aplly a moving average filter to smooth the length data. Use a window size of 5.
    #window_size = 5
    #l0 = pd.Series(l0).rolling(window=window_size, min_periods=1).mean()
    #l1 = pd.Series(l1).rolling(window=window_size, min_periods=1).mean()
    #l2 = pd.Series(l2).rolling(window=window_size, min_periods=1).mean()
    #l3 = pd.Series(l3).rolling(window=window_size, min_periods=1).mean()

    h0 = data.iloc[:, 4] # Helios measurement 0
    h1 = data.iloc[:, 5] # Helios measurement 1
    h2 = data.iloc[:, 6] # Helios measurement 2
    h3 = data.iloc[:, 7] # Helios measurement 3

    # Apply a moving average filter to smooth the Helios data. Use a window size of 5.
    #window_size = 5
    #h0 = pd.Series(h0).rolling(window=window_size, min_periods=1).mean()
    #h1 = pd.Series(h1).rolling(window=window_size, min_periods=1).mean()
    #h2 = pd.Series(h2).rolling(window=window_size, min_periods=1).mean()
    #h3 = pd.Series(h3).rolling(window=window_size, min_periods=1).mean()

    h, h_avg, h0 = normalize(h0, h1, h2, h3)
    l, l_avg, l0 = normalize(l0, l1, l2, l3)

    return h, l, h_avg, l_avg, h0, l0

def parametric_arc(L=1, theta=pi/2, phi=0, N = 10):
    R = L/theta
    theta_i = arange(0, theta, theta/N)
    z = [R*sin(theta_i[i]) for i in range(len(theta_i))]
    x = [R - R*cos(theta_i[i]) for i in range(len(theta_i))]
    y = x*sin(phi)
    x = x*cos(phi)
    return x, y, z

def view_dataset(dataset):
    # Open the file "dataset" and read the data
    with open(dataset, 'r') as file:
        data = file.readlines()

    # Split the data into lines and remove the header
    data = [line.strip().split(',') for line in data[1:]]
    
    # Convert the data to a numpy array
    data = np.array(data, dtype=float)

    # Extract the columns
    l0 = data[:, 0]  # TOF distance l0
    l1 = data[:, 1]  # TOF distance l1
    l2 = data[:, 2]  # TOF distance l2
    l3 = data[:, 3]  # TOF distance l3
    h0 = data[:, 4]  # Helios measurement 0
    h1 = data[:, 5]  # Helios measurement 1
    h2 = data[:, 6]  # Helios measurement 2
    h3 = data[:, 7]  # Helios measurement 3

    # Plot the data
    plt.figure(figsize=(12, 10))

    # Subplot for TOF distances
    plt.subplot(3, 1, 1)
    plt.plot(l0, label='TOF distance l0')
    plt.plot(l1, label='TOF distance l1')
    plt.plot(l2, label='TOF distance l2')
    plt.plot(l3, label='TOF distance l3')
    plt.xlabel('Sample Index')
    plt.ylabel('TOF Distance')
    plt.title('Evolution of TOF Distances')
    plt.legend()
    plt.grid(True)

    # Subplot for Helios measurements
    plt.subplot(3, 1, 2)
    plt.plot(h0, label='Helios measurement h0')
    plt.plot(h1, label='Helios measurement h1')
    plt.plot(h2, label='Helios measurement h2')
    plt.plot(h3, label='Helios measurement h3')
    plt.xlabel('Sample Index')
    plt.ylabel('Helios Measurement')
    plt.title('Evolution of Helios Measurements')
    plt.legend()
    plt.grid(True)

    # Subplot for average length
    plt.subplot(3, 1, 3)
    avg_length = (l0 + l1 + l2 + l3) / 4
    plt.plot(avg_length, label='Average Length', color='purple')
    plt.xlabel('Sample Index')
    plt.ylabel('Average Length')
    plt.title('Evolution of Average Length')
    plt.legend()
    plt.grid(True)

    plt.tight_layout()
    plt.show()

def __main__():
    # File path
    file_path = './dataset/250109/0x4A_250109_test.csv'

    # View dataset
    view_dataset(file_path)

if __name__ == "__main__":
    __main__()
