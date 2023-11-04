#include <HeliosModule.h>

SoftWire sw(SW_SDA, SW_SCL);

char swTxBuffer[14];
char swRxBuffer[14];

char tmp_str[7]; // temporary variable used in convert function

char* convert_int16_to_str(int16_t i)
{ // converts int16 to string. Moreover, resulting strings will have the same length in the debug monitor.
    sprintf(tmp_str, "%6d", i);
    return tmp_str;
}

void initIMU()
{
    sw.setTxBuffer(swTxBuffer, sizeof(swTxBuffer));
    sw.setRxBuffer(swRxBuffer, sizeof(swRxBuffer));
    sw.setDelay_us(5);
    sw.setTimeout(1000);
    sw.begin();

    sw.beginTransmission(IMU_ADDR); // Begins a transmission to the I2C slave (GY-521 board)
    sw.write(0x6B); // PWR_MGMT_1 register
    sw.write(0); // set to zero (wakes up the MPU-6050)
    sw.endTransmission(true);
}

void readIMU(ImuData* imu_data)
{
    sw.beginTransmission(IMU_ADDR);
    sw.write(0x3B); // starting with register 0x3B (ACCEL_XOUT_H) [MPU-6000 and MPU-6050 Register Map and Descriptions Revision 4.2, p.40]
    sw.endTransmission(); // the parameter indicates that the Arduino will send a restart. As a result, the connection is kept active.
    sw.requestFrom(IMU_ADDR, 7*2); // request a total of 7*2=14 registers
    
    // "Wire.read()<<8 | Wire.read();" means two registers are read and stored in the same variable
    imu_data->acc_x = sw.read()<<8 | sw.read(); // reading registers: 0x3B (ACCEL_XOUT_H) and 0x3C (ACCEL_XOUT_L)
    imu_data->acc_y = sw.read()<<8 | sw.read(); // reading registers: 0x3D (ACCEL_YOUT_H) and 0x3E (ACCEL_YOUT_L)
    imu_data->acc_z = sw.read()<<8 | sw.read(); // reading registers: 0x3F (ACCEL_ZOUT_H) and 0x40 (ACCEL_ZOUT_L)
    imu_data->temp  = sw.read()<<8 | sw.read(); // reading registers: 0x41 (TEMP_OUT_H)   and 0x42 (TEMP_OUT_L)
    imu_data->gyr_x = sw.read()<<8 | sw.read(); // reading registers: 0x43 (GYRO_XOUT_H)  and 0x44 (GYRO_XOUT_L)
    imu_data->gyr_y = sw.read()<<8 | sw.read(); // reading registers: 0x45 (GYRO_YOUT_H)  and 0x46 (GYRO_YOUT_L)
    imu_data->gyr_z = sw.read()<<8 | sw.read(); // reading registers: 0x47 (GYRO_ZOUT_H)  and 0x48 (GYRO_ZOUT_L)
}

void printIMU(ImuData* imu_data)
{
    // print out data
    Serial.print("aX = "); Serial.print(convert_int16_to_str(imu_data->acc_x));
    Serial.print(" | aY = "); Serial.print(convert_int16_to_str(imu_data->acc_y));
    Serial.print(" | aZ = "); Serial.print(convert_int16_to_str(imu_data->acc_z));

    // the following equation was taken from the documentation [MPU-6000/MPU-6050 Register Map and Description, p.30]
    Serial.print(" | tmp = "); Serial.print(imu_data->temp/340.00+36.53);
    Serial.print(" | gX = "); Serial.print(convert_int16_to_str(imu_data->gyr_x));
    Serial.print(" | gY = "); Serial.print(convert_int16_to_str(imu_data->gyr_y));
    Serial.print(" | gZ = "); Serial.print(convert_int16_to_str(imu_data->gyr_z));
    Serial.println();
}