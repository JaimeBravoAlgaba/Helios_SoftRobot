from numpy import cos, sin, pi
import tensorflow as tf
import pandas as pd
import json

# This script retrains a pretrained model with new data
model_file = 'models/neural_network/model_44_V2.h5'
model_file_new = 'models/neural_network/model_44_V2.h5'
test_data = 'dataset/0x44_240423_2.csv'
normalization_file = 'models/normalization_params.json'


def normalize(data, min, max):
    return (data - min) / (max - min)
# Load the trained network
model = tf.keras.models.load_model(model_file)


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

# Entrenar el modelo con los nuevos datos
model.fit(x, y, epochs=100, batch_size=32)

# Guardar el modelo reentrenado
model.save(model_file_new)