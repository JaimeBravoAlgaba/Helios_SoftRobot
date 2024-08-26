import numpy as np
from numpy import cos, sin, pi
import tensorflow as tf
from keras.models import Sequential
from keras.layers import Dense
import pandas as pd
import json

# This script trains a new model with a given dataset
model_file = 'models/neural_network/model_40.keras'
training_data = 'dataset/0x40_240823_4.csv'
normalization_file = 'models/normalization_params.json'

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

# Load a pretrained model
model = tf.keras.models.load_model(model_file)

x = np.column_stack((h0, h1, h2, h3, h_mean))
y = np.column_stack((qy, qz))

# Retrain the model with the new dataset, using the pretrained model as a starting point
model.fit(x, y, epochs=100, batch_size=10)

# Model summary
model.summary()
model.save(model_file)