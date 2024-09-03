import json
import pandas as pd
import numpy as np
from math import sqrt
import matplotlib.pyplot as plt
import tensorflow as tf
from hwcounter import count, count_end
import time

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

def test_models(mod, helios_names, model='p11', data_index=1):
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

    clk_cycles = 0
    elapsed_time = 0

    # Test the model by computing the RMSE
    # Compute the predictions depending on the model type
    if model == 'p11':
        # Load the model
        models = load_models(f'models/{model}/{model}.json')
        qy_model = models['qy'][mod]
        qz_model = models['qz'][mod]

        # Predict the output
        elapsed_time = time.time()
        clk_cycles = count()
        qy_pred = p11(h02, h13, qy_model)
        qz_pred = p11(h02, h13, qz_model)
        clk_cycles = count_end() - clk_cycles
        elapsed_time = time.time() - elapsed_time

    elif model == 'p33':
        # Load the model
        models = load_models(f'models/{model}/{model}.json')
        qy_model = models['qy'][mod]
        qz_model = models['qz'][mod]

        # Predict the output
        elapsed_time = time.time()
        clk_cycles = count()
        qy_pred = p33(h02, h13, qy_model)
        qz_pred = p33(h02, h13, qz_model)
        clk_cycles = count_end() - clk_cycles
        elapsed_time = time.time() - elapsed_time

    elif model == 'nn':
        # load the model
        model = tf.keras.models.load_model(f'models/{model}/nn_{helios_name}.keras')

        # Predict the output
        elapsed_time = time.time()
        clk_cycles = count()

        x = np.column_stack((h02, h13))
        y = np.column_stack((qy, qz))

        predicted_output = pd.DataFrame(model(x))

        qy_pred = predicted_output.iloc[:, 0]
        qz_pred = predicted_output.iloc[:, 1]
        qy_pred = denormalize(qy_pred, -60, 60)
        qz_pred = denormalize(qz_pred, -60, 60)

        clk_cycles = count_end() - clk_cycles
        elapsed_time = time.time() - elapsed_time

    else:
        raise ValueError('Non valid model')

    # Compute the RMSE
    rmse_qy = sqrt(np.mean((qy - qy_pred)**2))
    rmse_qz = sqrt(np.mean((qz - qz_pred)**2))

    return rmse_qy, rmse_qz, clk_cycles, elapsed_time

def rmse_test(helios_names, model):
    rmse_training = []
    rmse_test = []

    for mod in range(6):
        rmse_training.append(test_models(mod, helios_names, model=model, data_index=1))
        rmse_test.append(test_models(mod, helios_names, model=model, data_index=2))
        print('')

    # First, calculate RMSE for qy
    rmse_qy_training = [rmse[0] for rmse in rmse_training]
    rmse_qy_test = [rmse[0] for rmse in rmse_test]

    # Second, calculate RMSE for qz
    rmse_qz_training = [rmse[1] for rmse in rmse_training]
    rmse_qz_test = [rmse[1] for rmse in rmse_test]

    # Third, calculate the average number of clock cycles and elapsed time
    clk_cycles = [rmse[2] for rmse in rmse_test]
    clk_cycles = np.mean(clk_cycles)

    elapsed_time = [rmse[3] for rmse in rmse_test]
    elapsed_time = np.mean(elapsed_time)

    return rmse_qy_training, rmse_qy_test, rmse_qz_training, rmse_qz_test, clk_cycles, elapsed_time
    
def rmse_analysis(rmse_qy_training, rmse_qy_test, rmse_qz_training, rmse_qz_test, helios_names, title='RMSE: Entrenamiento vs Validación'):
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

    plt.suptitle(title, fontsize=24)

if __name__ == '__main__':
    helios_names = ['0x40', '0x41', '0x44', '0x45', '0x48', '0x4A']
    
    # Test each model
    rmse_qy_training_p11, rmse_qy_test_p11, rmse_qz_training_p11, rmse_qz_test_p11, clk_p11, time_p11 = rmse_test(helios_names, model='p11')
    rmse_qy_training_p33, rmse_qy_test_p33, rmse_qz_training_p33, rmse_qz_test_p33, clk_p33, time_p33 = rmse_test(helios_names, model='p33')
    rmse_qy_training_nn, rmse_qy_test_nn, rmse_qz_training_nn, rmse_qz_test_nn, clk_nn, time_nn = rmse_test(helios_names, model='nn')

    rmse_analysis(rmse_qy_training_p11, rmse_qy_test_p11, rmse_qz_training_p11, rmse_qz_test_p11, helios_names, "Comparación RMSE - p11")
    rmse_analysis(rmse_qy_training_p33, rmse_qy_test_p33, rmse_qz_training_p33, rmse_qz_test_p33, helios_names, "Comparación RMSE - p33")
    rmse_analysis(rmse_qy_training_nn, rmse_qy_test_nn, rmse_qz_training_nn, rmse_qz_test_nn, helios_names, "Comparación RMSE - Red Neuronal")

    # Compare the validation RMSE of each model
    fig, axs = plt.subplots(1, 2, figsize=(10, 10), facecolor='white')
    plt.rcParams['font.family'] = 'Arial'
    bar_width = 0.25
    ylim = [0, 20]

    # Plot RMSE for qy
    x = np.arange(len(helios_names))
    axs[0].bar(x - bar_width, rmse_qy_test_p11, label='p11', color='blue', width=bar_width)
    axs[0].bar(x, rmse_qy_test_p33, label='p33', color='red', width=bar_width)
    axs[0].bar(x + bar_width, rmse_qy_test_nn, label='NN', color='green', width=bar_width)
    axs[0].set_ylabel('RMSE qy [grados]', fontsize=20)
    axs[0].set_xticks(x)
    axs[0].set_xticklabels(helios_names, fontsize=20)
    axs[0].legend(fontsize=20)
    axs[0].grid(True)
    axs[0].set_ylim(ylim)
    axs[0].set_title('RMSE - qy', fontsize=20)

    # Plot RMSE for qz
    axs[1].bar(x - bar_width, rmse_qz_test_p11, label='p11', color='blue', width=bar_width)
    axs[1].bar(x, rmse_qz_test_p33, label='p33', color='red', width=bar_width)
    axs[1].bar(x + bar_width, rmse_qz_test_nn, label='NN', color='green', width=bar_width)
    axs[1].set_ylabel('RMSE qz [grados]', fontsize=20)
    axs[1].set_xticks(x)
    axs[1].set_xticklabels(helios_names, fontsize=20)
    axs[1].legend(fontsize=20)
    axs[1].grid(True)
    axs[1].set_ylim(ylim)
    axs[1].set_title('RMSE - qz', fontsize=20)

    for ax in axs:
        ax.set_xlabel('Módulo', fontsize=20)
        ax.set_ylim(ylim)
        ax.tick_params(axis='both', which='major', labelsize=20)

    plt.suptitle('Comparativa RMSE - Validación', fontsize=24)

    # Compare the average number of clock cycles and elapsed time
    fig, axs = plt.subplots(1, 2, figsize=(15, 10), facecolor='white')
    plt.rcParams['font.family'] = 'Arial'
    bar_width = 0.25
    ylim_clk = [0, max([clk_p11, clk_p33, clk_nn])*1.1/1000000]
    ylim_time = [0, 1000*max([time_p11, time_p33, time_nn])*1.1]

    # Plot clk cycles
    x = np.arange(3)
    axs[0].bar(x, [clk_p11/1000000, clk_p33/1000000, clk_nn/1000000], color='blue', width=bar_width)
    axs[0].set_ylabel('Millones de Ciclos', fontsize=20)
    axs[0].set_xticks(x)
    axs[0].set_xticklabels(['p11', 'p33', 'NN'], fontsize=20)
    axs[0].grid(True)
    axs[0].set_ylim(ylim_clk)
    axs[0].set_title('Ciclos de reloj promedio', fontsize=20)

    for i, v in enumerate([clk_p11, clk_p33, clk_nn]):
        axs[0].text(i, v *1.05/1000000, f"{v/1000000:.2}", ha='center', va='bottom', fontsize=16)

    axs[0].set_xlabel('Modelo', fontsize=20)
    axs[0].set_ylim(ylim_clk)
    axs[0].tick_params(axis='both', which='major', labelsize=20)

    # Plot elapsed time
    axs[1].bar(x, [time_p11*1000, time_p33*1000, time_nn*1000], color='blue', width=bar_width)
    axs[1].set_ylabel('Tiempo [ms]', fontsize=20)
    axs[1].set_xticks(x)
    axs[1].set_xticklabels(['p11', 'p33', 'NN'], fontsize=20)
    axs[1].grid(True)
    axs[1].set_ylim(ylim_time)
    axs[1].set_title('Tiempo de ejecución promedio', fontsize=20)

    for i, v in enumerate([time_p11*1000, time_p33*1000, time_nn*1000]):
        axs[1].text(i, v + 0.1, str(round(v, 2)), ha='center', va='bottom', fontsize=16)

    axs[1].set_xlabel('Modelo', fontsize=20)
    axs[1].set_ylim(ylim_time)
    axs[1].tick_params(axis='both', which='major', labelsize=20)

    plt.suptitle('Comparativa Ciclos de Reloj y Tiempo Transcurrido', fontsize=24)

    plt.show()

    # Improvement percentage vs p11; RMSE, CLK and Time
    clk_p11 = clk_p11.mean()
    time_p11 = time_p11.mean()

    clk_p33 = clk_p33.mean()
    time_p33 = time_p33.mean()

    clk_nn = clk_nn.mean()
    time_nn = time_nn.mean()

    # Polynomial p33
    improvement_rmse_qy_p33 = [(rmse_qy_p11 - rmse_qy_p33)/rmse_qy_p11*100 for rmse_qy_p11, rmse_qy_p33 in zip(rmse_qy_test_p11, rmse_qy_test_p33)]
    improvement_rmse_qz_p33 = [(rmse_qz_p11 - rmse_qz_p33)/rmse_qz_p11*100 for rmse_qz_p11, rmse_qz_p33 in zip(rmse_qz_test_p11, rmse_qz_test_p33)]

    improvement_rmse_p33 = np.mean([np.mean(improvement_rmse_qy_p33), np.mean(improvement_rmse_qz_p33)])

    improvement_clk_p33 = (clk_p11 - clk_p33)/clk_p11*100
    improvement_time_p33 = (time_p11 - time_p33)/time_p11*100

    # Neural network
    improvement_rmse_qy_nn = [(rmse_qy_p11 - rmse_qy_nn)/rmse_qy_p11*100 for rmse_qy_p11, rmse_qy_nn in zip(rmse_qy_test_p11, rmse_qy_test_nn)]
    improvement_rmse_qz_nn = [(rmse_qz_p11 - rmse_qz_nn)/rmse_qz_p11*100 for rmse_qz_p11, rmse_qz_nn in zip(rmse_qz_test_p11, rmse_qz_test_nn)]
    improvement_rmse_nn = np.mean([np.mean(improvement_rmse_qy_nn), np.mean(improvement_rmse_qz_nn)])

    improvement_clk_nn = (clk_p11 - clk_nn)/clk_p11*100
    improvement_time_nn = (time_p11 - time_nn)/time_p11*100

    # Print the results as a table
    print('Improvement percentage vs p11')
    print('Model\t\t\tRMSE [%]\tCLK [%]\tTime [%]')
    print(f'p33\t\t\t{improvement_rmse_p33:.2f}\t\t{improvement_clk_p33:.2f}\t{improvement_time_p33:.2f}')
    print(f'Neural Network\t\t{improvement_rmse_nn:.2f}\t\t{improvement_clk_nn:.2f}\t{improvement_time_nn:.2f}')
    

    



