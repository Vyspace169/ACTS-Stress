#include "Mpu9250Implementation.hpp"


esp_err_t i2c_mpu9250_read_array(unsigned char address, unsigned char* read_data, int size)
{
    int ret;
    uint8_t read_data_l;
    uint8_t read_data_h;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_READ_BIT, ACK_CHECK_EN);
    while(size-- != 0) {
    	i2c_master_read_byte(cmd, read_data++, ACK_VAL);
    }
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_mpu9250_read_short(unsigned char address, unsigned short* read_data)
{
    int ret;
    uint8_t read_data_l;
    uint8_t read_data_h;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, &read_data_h, ACK_VAL);
    i2c_master_read_byte(cmd, &read_data_l, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    *read_data = read_data_h;
    *read_data <<= 8;
    *read_data |= read_data_l;

    return ret;
}

esp_err_t i2c_mpu9250_read_byte(unsigned char address, unsigned char* read_data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, read_data, ACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_mpu9250_write_word(unsigned char address, unsigned short write_data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, (write_data >> 8), ACK_CHECK_EN);
    i2c_master_write_byte(cmd, (write_data & 0xFF), ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_mpu9250_write_byte(unsigned char address, unsigned char write_data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU9250_I2C_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, write_data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_AK8936_read_byte(unsigned char address, unsigned char* read_data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AK8936_MAG_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AK8936_MAG_ADDRESS << 1 | I2C_READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, read_data, ACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_AK8936_write_byte(unsigned char address, unsigned char write_data)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, AK8936_MAG_ADDRESS << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, write_data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}



Mpu9250Implementation::Mpu9250Implementation() {
	//i2c_mpu9250_write_byte(GYRO_CONFIG_REG, GYROSCALE_500_DPS);
	//i2c_mpu9250_write_byte(ACCEL_CONFIG_REG, ACCELSCALE_4G);


	uint8_t ak8936_id;
	uint8_t mpu9250_id;

	i2c_mpu9250_read_byte(117, &mpu9250_id);

	i2c_mpu9250_write_byte(MPU9250_BYPASS_REG, MPU9250_BYPASS_BIT);

	i2c_AK8936_read_byte(0, &ak8936_id);

	i2c_AK8936_write_byte(AK8936_MAG_CNTL, AK8936_CONT_MODE_1);

	i2c_mpu9250_write_byte(MPU9250_BYPASS_REG, 0);


	ESP_LOGI("I2C TASK", "MPU9250 ID: %i    AK8936 ID: %i", mpu9250_id, ak8936_id);
}

mpu9250_data Mpu9250Implementation::GetMpu9250Data() {

	mpu9250_data localdata;
	unsigned char localbyte;


	/*i2c_mpu9250_read_short(ACCEL_X_REG, &localdata.AccelerometerX);
	i2c_mpu9250_read_short(ACCEL_Y_REG, &localdata.AccelerometerY);
	i2c_mpu9250_read_short(ACCEL_Z_REG, &localdata.AccelerometerZ);
	i2c_mpu9250_read_short(GYRO_X_REG, &localdata.GyroscopeX);
	i2c_mpu9250_read_short(GYRO_Y_REG, &localdata.GyroscopeY);
	i2c_mpu9250_read_short(GYRO_Z_REG, &localdata.GyroscopeZ);

	ESP_LOGI("I2C TASK", "Value: \t %i, \t %i, \t %i, \t %i, \t %i, \t %i",
			localdata.AccelerometerX/100,
			localdata.AccelerometerY/100,
			localdata.AccelerometerZ/100,
			localdata.GyroscopeX/100,
			localdata.GyroscopeY/100,
			localdata.GyroscopeZ/100);
			*/

	uint8_t data_array[12];
	i2c_mpu9250_read_array(ACCEL_X_REG, (unsigned char*)&localdata, 12);

	ESP_LOGI("I2C TASK", "Value: \t %i, \t %i, \t %i, \t %i, \t %i, \t %i",
				localdata.AccelerometerX/100,
				localdata.AccelerometerY/100,
				localdata.AccelerometerZ/100,
				localdata.GyroscopeX/100,
				localdata.GyroscopeY/100,
				localdata.GyroscopeZ/100);

	/*
	i2c_AK8936_read_byte(AK8936_MAG_XOUT_L, &localbyte);
	localdata.MagnetoX = localbyte;
	localdata.MagnetoX <<= 8;
	i2c_AK8936_read_byte(AK8936_MAG_XOUT_H, &localbyte);
	localdata.MagnetoX |= localbyte;
	i2c_AK8936_read_byte(AK8936_MAG_YOUT_L, &localbyte);
	localdata.MagnetoY = localbyte;
	localdata.MagnetoY <<= 8;
	i2c_AK8936_read_byte(AK8936_MAG_YOUT_H, &localbyte);
	localdata.MagnetoY |= localbyte;
	i2c_AK8936_read_byte(AK8936_MAG_ZOUT_L, &localbyte);
	localdata.MagnetoZ = localbyte;
	localdata.MagnetoZ <<= 8;
	i2c_AK8936_read_byte(AK8936_MAG_ZOUT_H, &localbyte);
	localdata.MagnetoZ |= localbyte;

	ESP_LOGI("I2C TASK", "Value:  %i,  %i,  %i",
				localdata.MagnetoX,
				localdata.MagnetoY,
				localdata.MagnetoZ);

	i2c_mpu9250_write_byte(MPU9250_BYPASS_REG, 0x00);*/

	return localdata;
}

Mpu9250Implementation::~Mpu9250Implementation() {

}
