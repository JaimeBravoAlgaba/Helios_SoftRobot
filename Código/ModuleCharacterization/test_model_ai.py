import numpy as np
from numpy import cos, sin, pi, arctan2
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plt
import json

# This script tests a pretrained model to predict the output of a given dataset
model_file = 'models/neural_network/model_4A.keras'
test_data = 'dataset/240823/0x4A_240823_3.csv'
normalization_file = 'models/normalization_params.json'

def denormalize(data, min, max):
    return data * (max - min) + min

# Load data form csv file
data = pd.read_csv(test_data, header=None, delimiter=';')

# Split data into input and output:
# CSV file format: theta | phi | h0 | h1

# Outputs
qy = data.iloc[:, 0]
qz = data.iloc[:, 1]

# Inputs
h_mean = data.iloc[:, 6]
h0 = data.iloc[:, 2] - h_mean
h1 = data.iloc[:, 3] - h_mean
h2 = data.iloc[:, 4] - h_mean
h3 = data.iloc[:, 5] - h_mean

x = np.column_stack((h0, h1, h2, h3))
y = np.column_stack((qy, qz))

# Load the model and predict the output
model = tf.keras.models.load_model(model_file)
predicted_output = pd.DataFrame(model.predict(x))

# Denormalize the real output
qy = denormalize(qy, -60, 60)
qz = denormalize(qz, -60, 60)

# Denormalize the predicted output
qy_pred = denormalize(predicted_output.iloc[:, 0], -60, 60)
qz_pred = denormalize(predicted_output.iloc[:, 1], -60, 60)

# Calculate the error
error = np.sqrt((qy - qy_pred)**2 + (qz - qz_pred)**2)

# Plot the error
plt.plot(error)
plt.show()

# Compare the predicted output with the real output
# Subplot 1
plt.subplot(2, 1, 1)
plt.plot(qy, label='IMU qy', color='blue')
plt.plot(qy_pred, label='Predicted qy', color='red')
plt.legend()

# Subplot 2
plt.subplot(2, 1, 2)
plt.plot(qz, label='IMU qz', color='blue')
plt.plot(qz_pred, label='Predicted qz', color='red')
plt.legend()

plt.show()
# compute the RMSE
rmse = np.sqrt(np.mean(error**2))
print('RMSE: ', rmse)

