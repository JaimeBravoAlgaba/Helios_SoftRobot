import numpy as np
from numpy import cos, sin, pi
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
import pandas as pd
import json

# This script trains a new model with a given dataset
model_file = 'models/neural_network/model_40.keras'
test_data = 'dataset/0x40_240823_3.csv'
normalization_file = 'models/normalization_params.json'

# Load data form csv file
data = pd.read_csv(test_data, header=None, delimiter=';')

# Split data into input and output:
# CSV file format: theta | phi | h0 | h1

# Outputs
qy = data.iloc[:, 0]
qz = data.iloc[:, 1]

# Inputs
h0 = data.iloc[:, 2]
h1 = data.iloc[:, 3]
h2 = data.iloc[:, 4]
h3 = data.iloc[:, 5]
h_mean = data.iloc[:, 6]

# Create a model
model = Sequential()
model.add(Dense(10, input_dim=5, activation='relu'))
model.add(Dense(5, activation='relu'))
model.add(Dense(2, activation='tanh'))

# Compile the model
model.compile(loss='mean_squared_error', optimizer='adam')

# Train the model
x = np.column_stack((h0, h1, h2, h3, h_mean))
y = np.column_stack((qy, qz))
model.fit(x, y, epochs=100, batch_size=10)

# Model summary
model.summary()

# Save the model
model.save(model_file)
