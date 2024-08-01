from numpy import cos, sin, pi, arctan2
import pandas as pd
import tensorflow as tf
import matplotlib.pyplot as plt
import json

# This script tests a pretrained model to predict the output of a given dataset
model_file = 'models/neural_network/model_44_V2.h5'
test_data = 'dataset/0x44_240423_2.csv'
normalization_file = 'models/normalization_params.json'


def normalize(data, min, max):
    return (data - min) / (max - min)
# Load the trained network
model = tf.keras.models.load_model(model_file)

def denormalize(data, min, max):
    return data * (max - min) + min

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

# Normalize the data and concatenate
theta0 = normalize(theta0, y_min, y_max)
theta1 = normalize(theta1, y_min, y_max)
h0 = normalize(h0, x_min, x_max)
h1 = normalize(h1, x_min, x_max)

x = pd.concat([h0, h1], axis=1)
y = pd.concat([theta0, theta1], axis=1)

# Predict the output using the trained network
predicted_output = pd.DataFrame(model.predict(x))

# Denormalize the predicted output
theta0 = denormalize(predicted_output.iloc[:, 0], y_min, y_max)
theta1 = denormalize(predicted_output.iloc[:, 1], y_min, y_max)

phi_pred = arctan2(theta1, theta0)
theta_pred= (theta0/cos(phi_pred) + theta1/sin(phi_pred))/2

phi_pred = phi_pred*180/pi
for index in range(len(phi_pred)):
    if phi_pred[index]<0:
        phi_pred[index] += 360

y = pd.concat([theta, phi], axis=1)
y_pred = pd.concat([theta_pred, phi_pred], axis=1)

# Calculate the error between the predicted and the measured output
error = y - y_pred
error = error.abs()
# Correct the error for the angle wrapping around 360 degrees
error.iloc[:, 1] = error.iloc[:, 1].apply(lambda x: min(x, 360-x))

# Plot the error
fig1, (ax1, ax2) = plt.subplots(2, 1, figsize=(8, 6))

ax1.scatter(theta, error.iloc[:, 0], c='r', label='Theta error')
ax1.set_xlabel('Theta')
ax1.set_ylabel('Error (Theta)')
ax1.legend()

ax2.scatter(theta, error.iloc[:, 1], c='b', label='Phi error')
ax2.set_xlabel('Theta')
ax2.set_ylabel('Error (Phi)')
ax2.legend()

plt.tight_layout()
plt.show(block=False)

# Plot the predicted and measured output
fig2 = plt.figure()
ax2 = fig2.add_subplot(111)
ax2.plot(y, label='Measured (theta,phi)')
ax2.plot(y_pred, label='Predicted (theta,phi)')

plt.xlim(0, len(y))
plt.ylim(0, 360)

plt.legend()
plt.show(block=False)

# 3D plot of the data, representing phi vs theta vs measurement index (time)
rx = theta*cos(phi*pi/180.0)
ry = theta*sin(phi*pi/180.0)
rx_pred = theta_pred*cos(phi_pred*pi/180.0)
ry_pred = theta_pred*sin(phi_pred*pi/180.0)

fig3 = plt.figure()
ax = fig3.add_subplot(111, projection='3d')
ax.plot(rx, ry, range(len(rx)), c='r', label='Measured')
ax.plot(rx_pred, ry_pred, range(len(rx_pred)), c='b', label='Predicted')
ax.set_xlabel('X')
ax.set_ylabel('Y')
ax.set_zlabel('Measurement index')

plt.xlim(-60, 60)
plt.ylim(-60, 60)

plt.legend()
plt.show(block=False)

input("Press [enter] to close.")
plt.close('all')