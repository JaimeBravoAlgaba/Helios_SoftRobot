import os
import pandas as pd

# Path to the 'datasets' folder
folder_path = './dataset'

# Get a list of all CSV files in the folder
csv_files = [file for file in os.listdir(folder_path) if file.endswith('.csv')]
n_files = len(csv_files)

# Initialize an empty list to store the data
combined_data = list()

# Iterate over each CSV file and append its data to the combined DataFrame
count = 0
for file in csv_files:
    file_path = os.path.join(folder_path, file)
    data = pd.read_csv(file_path)
    for row in data.values:
        combined_data.append(row)
    count += 1
    print(f'Processed file: {file} ({100*count/n_files:.2f}%)')

# Save the combined data to a new CSV file
combined_data = pd.DataFrame(combined_data)
combined_data.to_csv('combined_data.csv', index=False)