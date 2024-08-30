import numpy as np
from numpy import cos, sin, pi
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
import pandas as pd
import json

# This script trains a new model with a given dataset
model_file = 'models/neural_network/model_0x4A_V2.keras'
training_data = 'dataset/240823/0x4A_240823_1.csv'
normalization_file = 'models/normalization_params.json'

def denormalize(data, min, max):
    return data * (max - min) + min

# Load data form csv file
data = pd.read_csv(training_data, header=None, delimiter=';')

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

# Compute the differential measurements
h02 = h0 - h2
h13 = h1 - h3

# Create a model
model = Sequential()
model.add(Dense(16, input_dim=2, activation='relu'))
model.add(Dense(8, activation='relu'))
model.add(Dense(2, activation='tanh'))

# Compile the model
model.compile(loss='mean_squared_error', optimizer='adam')

# Train the model
x = np.column_stack((h02, h13))
y = np.column_stack((qy, qz))
model.fit(x, y, epochs=100, batch_size=10)

# Predict the output
predicted_output = pd.DataFrame(model.predict(x))

# Denormalize the real output
qy = denormalize(qy, -60, 60)
qz = denormalize(qz, -60, 60)

# Denormalize the predicted output
qy_pred = denormalize(predicted_output.iloc[:, 0], -60, 60)
qz_pred = denormalize(predicted_output.iloc[:, 1], -60, 60)

# Calculate the error
error = np.sqrt((qy - qy_pred)**2 + (qz - qz_pred)**2)

# Compute the RMSE
rmse = np.sqrt(np.mean(error**2))
print('RMSE: ', rmse)

# Model summary
model.summary()

# Save the model
model.save(model_file)
