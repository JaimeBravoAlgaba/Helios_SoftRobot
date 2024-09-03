# Write a pyhon script that reads the parameters of a json file

import json
import pandas as pd
import numpy as np
from math import sqrt
import matplotlib.pyplot as plt
import tensorflow as tf

def read_json_file(file_path):
    with open(file_path, 'r') as file:
        data = json.load(file)
    return data

def load_models(file_path):
    data = read_json_file(file_path)

    # Get the models
    qy_models = data['data']['qy']
    qz_models = data['data']['qz']
    q_keys = qy_models.keys()

    # Convert to list
    qy_models = [qy_models[key] for key in q_keys]
    qz_models = [qz_models[key] for key in q_keys]

    return {"qy": qy_models, "qz": qz_models}

def p11(x, y, params):
    return params[0] + params[1]*x + params[2]*y

def p33(x, y, params):
    return params[0] + params[1]*x + params[2]*y + params[3]*x**2 + params[4]*x*y + params[5]*y**2 + params[6]*x**3 + params[7]*x**2*y + params[8]*x*y**2 + params[9]*y**3

def denormalize(data, min, max):
    return data * (max - min) + min

def test_models(mod, helios_names, model='poly_1', data_index=1):
    helios_name = helios_names[mod]
    training_data = pd.read_csv(f'dataset/240823/{helios_name}_240823_{data_index}.csv', delimiter=';')

    # Get the data as arrays
    # CSV file format: theta | phi | h0 | h1

    # Outputs
    qy = (training_data.iloc[:, 0]-0.5)*120
    qz = (training_data.iloc[:, 1]-0.5)*120

    # Inputs
    h0 = training_data.iloc[:, 2]
    h1 = training_data.iloc[:, 3]
    h2 = training_data.iloc[:, 4]
    h3 = training_data.iloc[:, 5]
    h_mean = training_data.iloc[:, 6]

    # Compute the differential measurements
    h02 = h0 - h2
    h13 = h1 - h3

    # Test the model by computing the RMSE
    # Compute the predictions depending on the model type
    if model == 'poly1':
        # Load the model
        models = load_models(f'models/{helios_name}_240823.json')
        qy_model = models['qy'][0]
        qz_model = models['qz'][0]

        # Predict the output
        qy_pred = p11(h02, h13, qy_model)
        qz_pred = p11(h02, h13, qz_model)

    elif model == 'poly3':
        # Load the model
        models = load_models(f'models/{helios_name}_240823.json')
        qy_model = models['qy'][3]
        qz_model = models['qz'][3]

        # Predict the output
        qy_pred = p33(h02, h13, qy_model)
        qz_pred = p33(h02, h13, qz_model)

    elif model == 'neural_network':
        # load the model
        model = tf.keras.models.load_model(f'models/neural_network/model_{helios_name}.keras')

        # Predict the output
        x = np.column_stack((h02, h13))
        y = np.column_stack((qy, qz))
        predicted_output = pd.DataFrame(model.predict(x))

        qy_pred = predicted_output.iloc[:, 0]
        qz_pred = predicted_output.iloc[:, 1]

        # Denormalize the real output
        qy_pred = denormalize(qy_pred, -60, 60)
        qz_pred = denormalize(qz_pred, -60, 60)

    else:
        raise ValueError('Non valid model')

    # Compute the RMSE
    rmse_qy = sqrt(np.mean((qy - qy_pred)**2))
    rmse_qz = sqrt(np.mean((qz - qz_pred)**2))

    return rmse_qy, rmse_qz

if __name__ == '__main__':
    helios_names = ['0x40', '0x41', '0x44', '0x45', '0x48', '0x4A']
    model = 'neural_network'
    rmse_training = []
    rmse_test = []

    # Iterate over the models
    for mod in range(6):
        rmse_training.append(test_models(mod, helios_names, model=model, data_index=1))
        rmse_test.append(test_models(mod, helios_names, model=model, data_index=2))
        print('')
    
    # Barplot of the RMSE, comparing the two degrees
    # First, plot RMSE for qy
    rmse_qy_training = [rmse[0] for rmse in rmse_training]
    rmse_qy_test = [rmse[0] for rmse in rmse_test]

    # Second, plot RMSE for qz
    rmse_qz_training = [rmse[1] for rmse in rmse_training]
    rmse_qz_test = [rmse[1] for rmse in rmse_test]

    # Plot the RMSE
    fig, axs = plt.subplots(1, 2, figsize=(10, 10), facecolor='white')
    plt.rcParams['font.family'] = 'Arial'
    bar_width = 0.35
    ylim = [0, 20]

    # Plot RMSE for qy
    x = np.arange(len(helios_names))
    axs[0].bar(x - bar_width/2, rmse_qy_training, label='Entrenamiento', color='blue', width=bar_width)
    axs[0].bar(x + bar_width/2, rmse_qy_test, label='Validación', color='red', width=bar_width)
    axs[0].set_ylabel('RMSE qy [grados]', fontsize=20)
    axs[0].set_xticks(x)
    axs[0].set_xticklabels(helios_names, fontsize=20)
    axs[0].legend(fontsize=20)
    axs[0].grid(True)
    axs[0].set_ylim(ylim)
    axs[0].set_title('RMSE - qy', fontsize=20)

    # Plot RMSE for qz
    axs[1].bar(x - bar_width/2, rmse_qz_training, label='Entrenamiento', color='blue', width=bar_width)
    axs[1].bar(x + bar_width/2, rmse_qz_test, label='Validación', color='red', width=bar_width)
    axs[1].set_ylabel('RMSE qz [grados]', fontsize=20)
    axs[1].set_xticks(x)
    axs[1].set_xticklabels(helios_names, fontsize=20)
    axs[1].legend(fontsize=20)
    axs[1].grid(True)
    axs[1].set_ylim(ylim)
    axs[1].set_title('RMSE - qz', fontsize=20)
   
    # Plot a text box next to each bar indicating its value
    for i, v in enumerate(rmse_qy_training):
        axs[0].text(i, v + 0.5, str(round(v, 2)), ha='right', va='bottom', fontsize=16)
        axs[0].text(i, rmse_qy_test[i] + 0.5, str(round(rmse_qy_test[i], 2)), ha='left', va='bottom', fontsize=16)

    for i, v in enumerate(rmse_qz_training):
        axs[1].text(i, v + 0.5, str(round(v, 2)), ha='right', va='bottom', fontsize=16)
        axs[1].text(i, rmse_qz_test[i] + 0.5, str(round(rmse_qz_test[i], 2)), ha='left', va='bottom', fontsize=16)

    for ax in axs:
        ax.set_xlabel('Módulo', fontsize=20)
        ax.set_ylim(ylim)
        ax.tick_params(axis='both', which='major', labelsize=20)

    plt.show()    