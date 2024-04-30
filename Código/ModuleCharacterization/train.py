import numpy as np
from numpy import cos, sin, pi
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
import pandas as pd
import json

# This script trains a new model with a given dataset
model_file = 'models/neural_network/model_44_V2.h5'
test_data = 'dataset/0x44_240423_1.csv'
normalization_file = 'models/normalization_params.json'


def normalize(data, min, max):
    return (data - min) / (max - min)

def moving_average(x, w):
    return np.convolve(x, np.ones(w), 'valid') / w

# Load data form csv file
data = pd.read_csv(test_data, header=None)

# Load normalization parameters from json file
x_min = 0
x_max = 0
y_min = 0
y_max = 0
with open(normalization_file) as f:
    norm = json.load(f)
    x_min = norm['x_min']
    x_max = norm['x_max']
    y_min = norm['y_min']
    y_max = norm['y_max']

# Split data into input and output:
# CSV file format: theta | phi | h0 | h1
theta = data.iloc[:, 0]
phi = data.iloc[:, 1]
h0 = data.iloc[:, 2]
h1 = data.iloc[:, 3]

# Postprocess the input data, projecting the bending angle into x and y components.
# Thus, the discontinuity caused by the angle wrapping around 360 degrees is removed,
# smoothing the input data.
theta0 = theta*cos(phi*pi/180.0)    
theta1 = theta*sin(phi*pi/180.0)

# Moving average filter
theta0 = moving_average(theta0, 10)
theta1 = moving_average(theta1, 10)
h0 = moving_average(h0, 10)
h1 = moving_average(h1, 10)

# Normalize the data and concatenate
theta0 = normalize(theta0, y_min, y_max)
theta1 = normalize(theta1, y_min, y_max)
h0 = normalize(h0, x_min, x_max)
h1 = normalize(h1, x_min, x_max)

h0 = pd.DataFrame(h0)
h1 = pd.DataFrame(h1)
theta0 = pd.DataFrame(theta0)
theta1 = pd.DataFrame(theta1)

x = pd.concat([h0, h1], axis=1)
y = pd.concat([theta0, theta1], axis=1)

# Create a model
model = Sequential()
model.add(Dense(8, input_dim=2, activation='relu'))
model.add(Dense(4, activation='relu'))
model.add(Dense(2, activation='tanh'))

# Compile the model
model.compile(loss='mean_squared_error', optimizer='adam')

# Train the model
model.fit(x, y, epochs=100, batch_size=10)

# Model summary
model.summary()

# Save the model
model.save(model_file)
