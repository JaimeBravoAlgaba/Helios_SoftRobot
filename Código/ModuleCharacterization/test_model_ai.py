from matplotlib import pyplot as plt
import numpy as np
import pandas as pd
import os
os.environ['TF_ENABLE_ONEDNN_OPTS'] = '0'
import tensorflow as tf
from keras import models as km

from utils import get_data, denormalize
from helios_kine import fKine

# Change font of plots
plt.rcParams["font.family"] = "Times New Roman"
plt.rcParams.update({'font.size': 25})

# This script tests an existing model with a given dataset
model_file = 'models/nn/nn_0x4A_V3.keras'
test_data = './dataset/250109/0x4A_250109_test.csv'

h, l, h_avg, l_avg, h0, l0 = get_data(test_data)

# Load the model
model = km.load_model(model_file)

# Prepare the test data
x = np.array(h).T
y = np.array(l).T

# Predict the output
predicted_output = pd.DataFrame(model(x))
predicted_output = np.array([predicted_output[i].tolist() for i in range(len(predicted_output.columns))])

# Denormalize the output and the test data
expected_output = denormalize(l, l0, l_avg)
predicted_output = denormalize(predicted_output, l0, l_avg)

# Plot the predicted output
fig, axs = plt.subplots(2, 1, figsize=(10, 12))

# Plot the predicted output
axs[0].plot(predicted_output[0], label='Predicted TOF distance l0')
axs[0].plot(predicted_output[1], label='Predicted TOF distance l1')
axs[0].plot(predicted_output[2], label='Predicted TOF distance l2')
axs[0].plot(predicted_output[3], label='Predicted TOF distance l3')
axs[0].set_ylabel('TOF Distance')
axs[0].set_title('Predicted TOF Distances')
axs[0].legend()
axs[0].grid(True)

# Plot the test data
axs[1].plot(expected_output[0], label='Test TOF distance l0')
axs[1].plot(expected_output[1], label='Test TOF distance l1')
axs[1].plot(expected_output[2], label='Test TOF distance l2')
axs[1].plot(expected_output[3], label='Test TOF distance l3')
axs[1].set_ylabel('TOF Distance')
axs[1].set_title('Test TOF Distances')
axs[1].legend()
axs[1].grid(True)

plt.tight_layout()

# Create a 4x1 grid of subplots to compare predicted and test TOF distances
fig, axs = plt.subplots(4, 1)

for i in range(4):
    axs[i].plot(expected_output[i], label=f'Current TOF distance', linewidth=2)
    axs[i].plot(predicted_output[i], label=f'Predicted TOF distance', linewidth=2)
    axs[i].set_xlim([0, len(predicted_output[i])])
    axs[i].set_ylabel(f'd{i+1} (mm)', fontsize=25)
    if i == 3:
        axs[i].tick_params(axis='both', which='major', labelsize=25)
        axs[i].set_xlabel(f'Sample', fontsize=25)
        fig.legend(['Expected', 'Predicted'], loc='upper center', ncol=2, fontsize=25)
    else:
        axs[i].tick_params(axis='y', which='major', labelsize=25)
        axs[i].tick_params(axis='x', which='both', bottom=False, top=False, labelbottom=False)
    axs[i].grid(True)

    # Compute the RMSE
    rmse = np.sqrt(np.mean((predicted_output[i] - expected_output[i]) ** 2))
    print(f'RMSE for Length {i+1}:', rmse)

    # Compute the median error
    median_error = np.median(np.abs(predicted_output[i] - expected_output[i]))
    print(f'Median Error for Length {i+1}:', median_error)

    # Compute the standard deviation of the error
    std_error = np.std(predicted_output[i] - expected_output[i])
    print(f'Standard Deviation of Error for Length {i+1}:', std_error)

    # Compute the MAE
    mae = np.mean(np.abs(predicted_output[i] - expected_output[i]))
    print(f'Mean Absolute Error for Length {i+1}:', mae)

fig.tight_layout()

# Create a box plot with the differences between predicted_output and expected_output
differences = np.abs(predicted_output - expected_output)

fig, ax = plt.subplots(figsize=(10, 6))
ax.boxplot(differences.T, labels=[f'd{i+1}' for i in range(differences.shape[0])], patch_artist=True, boxprops=dict(linewidth=2))
#ax.set_title('Current vs Predicted TOF Distances', fontsize=25)
ax.set_ylabel('Error (mm)', fontsize=25)
ax.grid(True)

plt.tight_layout()

# Compute the RMSE, median error, and standard deviation of the error
rmse = np.sqrt(np.mean((predicted_output - expected_output) ** 2))
print('RMSE:', rmse)
median_error = np.median(np.abs(predicted_output - expected_output))    
print('Median Error:', median_error)
std_error = np.std(predicted_output - expected_output)
print('Standard Deviation of Error:', std_error)
mae = np.mean(np.abs(predicted_output - expected_output))
print('Mean Absolute Error:', mae)


# Model summary
model.summary()

theta_expected, phi_expected, length_expected = fKine(np.transpose(expected_output), 100)
theta_predicted, phi_predicted, length_predicted = fKine(np.transpose(predicted_output), 100)

# Wrap phi to 2*pi
phi_expected = np.mod(phi_expected, 2 * np.pi)
phi_predicted = np.mod(phi_predicted, 2 * np.pi)

# Plot theta expected and predicted
fig, axs = plt.subplots(3, 1, figsize=(10, 18))

# Convert theta and phi to degrees
theta_expected_deg = np.degrees(theta_expected)
theta_predicted_deg = np.degrees(theta_predicted)
phi_expected_deg = np.degrees(phi_expected)
phi_predicted_deg = np.degrees(phi_predicted)

# Plot theta expected and predicted
axs[0].plot(theta_expected_deg, label='Expected $\\theta$')
axs[0].plot(theta_predicted_deg, label='Predicted $\\theta$')
axs[0].set_xlim([0, len(theta_expected_deg)])
axs[0].set_ylabel('$\\theta$ (º)')
#axs[0].set_title('Theta Expected and Predicted')
axs[0].legend()
axs[0].grid(True)

# Plot phi expected and predicted
axs[1].plot(phi_expected_deg, label='Expected $\\phi$')
axs[1].plot(phi_predicted_deg, label='Predicted $\\phi$')
axs[1].set_xlim([0, len(phi_expected_deg)])
axs[1].set_ylabel('$\\phi$ (º)')
#axs[1].set_title('Phi Expected and Predicted')
axs[1].legend()
axs[1].grid(True)

# Plot length expected and predicted
axs[2].plot(length_expected, label='Expected Length')
axs[2].plot(length_predicted, label='Predicted Length')
axs[2].set_xlim([0, len(length_expected)])
axs[2].set_ylabel('Length (mm)')
#axs[2].set_title('Length Expected and Predicted')
axs[2].legend()
axs[2].grid(True)

plt.tight_layout()

# Plot theta vs phi expected and predicted
# Create two subplots: one for theta vs phi and one for length
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(8, 16))

# Plot theta vs phi in a polar plot. Expected and predicted
ax1 = plt.subplot(121, projection='polar')
ax1.plot(phi_expected, np.degrees(theta_expected), label='Expected')
ax1.plot(phi_predicted, np.degrees(theta_predicted), label='Predicted')
ax1.set_title('Theta vs Phi Expected and Predicted')
ax1.grid(True)
ax1.legend()

# Plot length expected and predicted
ax2 = plt.subplot(122)
ax2.plot(length_expected, label='Length Expected')
ax2.plot(length_predicted, label='Length Predicted')
ax2.set_ylabel('Length (mm)')
ax2.set_title('Length Expected and Predicted')
ax2.legend()
ax2.grid(True)
ax2.set_ylim([50, 70])

abs_error_theta = np.abs(theta_predicted - theta_expected)
abs_error_phi = np.abs(phi_predicted - phi_expected)
# Remove errors greater than pi
abs_error_phi = np.minimum(abs_error_phi, 2 * np.pi - abs_error_phi)
abs_error_length = np.abs(length_predicted - length_expected)

print('Absolute Error for Theta:', np.degrees(np.mean(abs_error_theta)))
print('Absolute Error for Phi:', np.degrees(np.mean(abs_error_phi)))
print('Absolute Error for Length:', np.mean(abs_error_length))

# Compute the standard deviations
std_theta = np.std(theta_predicted - theta_expected)
std_phi = np.std(phi_predicted - phi_expected)
std_length = np.std(length_predicted - length_expected)

print('Standard Deviation for Theta:', np.degrees(std_theta))
print('Standard Deviation for Phi:', np.degrees(std_phi))
print('Standard Deviation for Length:', std_length)

# Plot absolute error for theta, phi, and length
fig, axs = plt.subplots(3, 1, figsize=(10, 18))

# Plot absolute error for theta
axs[0].plot(np.degrees(abs_error_theta), label='Absolute Error Theta')
axs[0].set_ylabel('Absolute Error (Theta) [degrees]')
axs[0].set_title('Absolute Error for Theta')
axs[0].legend()
axs[0].grid(True)

# Plot absolute error for phi
axs[1].plot(np.degrees(abs_error_phi), label='Absolute Error Phi')
axs[1].set_ylabel('Absolute Error (Phi) [degrees]')
axs[1].set_title('Absolute Error for Phi')
axs[1].legend()
axs[1].grid(True)

# Plot absolute error for length
axs[2].plot(abs_error_length, label='Absolute Error Length')
axs[2].set_ylabel('Absolute Error (Length) [mm]')

axs[2].set_title('Absolute Error for Length')

axs[2].legend()

axs[2].set_title('Absolute Error for Length')
axs[2].legend()
axs[2].grid(True)

plt.tight_layout()

plt.tight_layout()

# Plot length vs theta
fig, ax = plt.subplots(figsize=(10, 6))

ax.plot(np.degrees(theta_expected), length_expected, label='Expected')
ax.plot(np.degrees(theta_predicted), length_predicted, label='Predicted')
ax.set_xlabel('Theta (degrees)')
ax.set_ylabel('Length (mm)')
ax.set_title('Length vs Theta')
ax.legend()
ax.grid(True)

plt.tight_layout()




plt.show()