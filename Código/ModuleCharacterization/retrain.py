from matplotlib import pyplot as plt
import numpy as np
from numpy import cos, sin, pi
import tensorflow as tf
from keras import models as km
import pandas as pd
import json

from processing_tof import get_data, normalize

# This script retrains an existing model with a given dataset
model_file = 'models/nn/nn_0x48_V2.keras'
model_file_new = 'models/nn/nn_0x48_V2.keras'
training_data = './dataset/241207/0x48_241207_2.csv'

h, l, h_avg, l_avg, h0, l0 = get_data(training_data)

# Load the existing model
model = km.load_model(model_file)

# Compile the model
model.compile(loss='mean_squared_error', optimizer='adam')

# Retrain the model
x = np.array(h).T
y = np.array(l).T
model.fit(x, y, epochs=100, batch_size=10)

# Predict the output
predicted_output = pd.DataFrame(model.predict(x))

# Plot the predicted output

fig, axs = plt.subplots(2, 1, figsize=(10, 12))

# Plot the predicted output
axs[0].plot(predicted_output[0], label='Predicted TOF distance l0')
axs[0].plot(predicted_output[1], label='Predicted TOF distance l1')
axs[0].plot(predicted_output[2], label='Predicted TOF distance l2')
axs[0].plot(predicted_output[3], label='Predicted TOF distance l3')
axs[0].set_xlabel('Sample Index')
axs[0].set_ylabel('TOF Distance')
axs[0].set_title('Predicted TOF Distances')
axs[0].legend()
axs[0].grid(True)

# Plot the training data
axs[1].plot(l[0], label='Training TOF distance l0')
axs[1].plot(l[1], label='Training TOF distance l1')
axs[1].plot(l[2], label='Training TOF distance l2')
axs[1].plot(l[3], label='Training TOF distance l3')
axs[1].set_xlabel('Sample Index')
axs[1].set_ylabel('TOF Distance')
axs[1].set_title('Training TOF Distances')
axs[1].legend()
axs[1].grid(True)

plt.tight_layout()
plt.show()

# Compute the RMSE
rmse = np.sqrt(np.mean((predicted_output - y) ** 2))
print('RMSE:', rmse)

# Model summary
model.summary()

# Save the retrained model
model.save(model_file_new)
