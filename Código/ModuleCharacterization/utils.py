import pandas as pd

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

    h0 = data.iloc[:, 4] # Helios measurement 0
    h1 = data.iloc[:, 5] # Helios measurement 1
    h2 = data.iloc[:, 6] # Helios measurement 2
    h3 = data.iloc[:, 7] # Helios measurement 3

    h, h_avg, h0 = normalize(h0, h1, h2, h3)
    l, l_avg, l0 = normalize(l0, l1, l2, l3)

    return h, l, h_avg, l_avg, h0, l0