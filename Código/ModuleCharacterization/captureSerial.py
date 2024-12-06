import serial
import csv

com_port = '/dev/ttyUSB0'
baud_rate = 115200

# Open the COM port
print('Starting test...')
ser = serial.Serial(com_port, baud_rate)  # Replace 'COM1' with the appropriate COM port and baud rate

# Create a CSV file
csv_file = open('data.csv', 'w', newline='')
csv_writer = csv.writer(csv_file)

try:
    counter = 0

    while True:
        # Read data from the COM port
        data = ser.readline().decode().strip()

        # Split the data into individual values
        values = data.split(',')

        # Write the values to the CSV file if none of the values are 'nan'
        if 'nan' not in values:
            csv_writer.writerow(values)

            # Flush the CSV file to ensure data is written immediately
            csv_file.flush()

            # Increase the counter and print the values
            counter += 1
            print(f'[{counter}] >> {values}')

except KeyboardInterrupt:
    # Close the CSV file and COM port on keyboard interrupt
    csv_file.close()
    ser.close()
    print('Test complete!')
