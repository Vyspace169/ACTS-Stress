#include "Mpu9250Implementation.hpp"

esp_err_t i2c_sensor_read_array(unsigned char dev_address, unsigned char address, unsigned char count, unsigned char* read_data) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();

	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, dev_address << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	if (ret != ESP_OK) {
	    return ret;
	}

	cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, dev_address << 1 | I2C_READ_BIT, ACK_CHECK_EN);
	for(int i = 0; i < count - 1; i++) {
		i2c_master_read_byte(cmd, read_data++, ACK_VAL);
	}
	i2c_master_read_byte(cmd, read_data, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);

	return ret;
}

esp_err_t i2c_sensor_read_byte(unsigned char dev_address, unsigned char address, unsigned char* read_data) {
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, read_data, NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

esp_err_t i2c_sensor_write_byte(unsigned char dev_address, unsigned char address, unsigned char write_data) {
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, dev_address << 1 | I2C_WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, address, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, write_data, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, I2C_TIMEOUT / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);

    return ret;
}

unsigned short switch_short(unsigned short value) {
	unsigned char upper = value >> 8;
	unsigned char lower = value & 0xFF;
	unsigned short returnvalue = lower;
	returnvalue <<= 8;
	returnvalue |= upper;
	return returnvalue;
}

Mpu9250Implementation::Mpu9250Implementation(){
	memset(&BackupMPUData, 0, sizeof(unsigned short) * 9);
	memset(&MPUData, 0, sizeof(unsigned short) * 9);
	MPUIsInitialized = false;
	AKIsInitialized = false;
	SensIsInitialized = false;
	AK8936SenseX = 0;
	AK8936SenseY = 0;
	AK8936SenseZ = 0;

	uint8_t AK8936ID = 0;
	uint8_t MPU9250ID = 0;

	// reset the MPU9250
	esp_err_t error = i2c_sensor_write_byte(MPU9250_I2C_ADDRESS, MPU9250_REG_PWR_MGMNT_1, MPU9250_SET_PWR_RESET);
	if(error != ESP_OK) {
		ESP_LOGI("I2C TASK", "MPU9250 address gave an error: %d", error);
	}
	else {
		vTaskDelay(10);
		i2c_sensor_read_byte(MPU9250_I2C_ADDRESS, MPU9250_REG_WHOAMI, &MPU9250ID);
		if(MPU9250ID == 113) {
			MPUIsInitialized = true;
		}
		// enable bypass mode
		i2c_sensor_write_byte(MPU9250_I2C_ADDRESS, MPU9250_REG_BYPASS, MPU9250_SET_BYPASS);
	}
	// reset the ak8936
	error = i2c_sensor_write_byte(AK8936_ADDRESS, AK8963_REG_CNTL2, AK8963_SET_RESET);
	if(error != ESP_OK) {
		ESP_LOGI("I2C TASK", "AK8963 address gave an error: %d", error);
	}
	else {
		vTaskDelay(10);
		i2c_sensor_read_byte(AK8936_ADDRESS, AK8936_REG_WHOAMI, &AK8936ID);
		if(AK8936ID == 72) {
			AKIsInitialized = true;
		}

		// Get sense values from magnetometer
		i2c_sensor_write_byte(AK8936_ADDRESS, AK8936_REG_CNTL1, AK8936_SET_FUSE_MODE);
		if( i2c_sensor_read_byte(AK8936_ADDRESS, AK8963_REG_SENSE_X, &AK8936SenseX) == ESP_OK) {
			if( i2c_sensor_read_byte(AK8936_ADDRESS, AK8963_REG_SENSE_Y, &AK8936SenseY) == ESP_OK) {
				if(i2c_sensor_read_byte(AK8936_ADDRESS, AK8963_REG_SENSE_Z, &AK8936SenseZ) == ESP_OK) {
					SensIsInitialized = true;
				}
			}
		}

		// trigger a single sample
		i2c_sensor_write_byte(AK8936_ADDRESS, AK8936_REG_CNTL1, AK8936_SET_16BIT | AK8936_SET_SING_SAMP);
	}

	ESP_LOGI("I2C TASK", "MPU9250 ID: %i    AK8936 ID: %i", MPU9250ID, AK8936ID);
}

unsigned short* Mpu9250Implementation::SensorRead() {

	if(MPUIsInitialized) {
		// read the data if device is initialized
		if(i2c_sensor_read_array(MPU9250_I2C_ADDRESS, MPU9250_REG_ACCEL_XL, 6, (unsigned char*)&MPUData[0]) == ESP_OK) {
			MPUData[0] = switch_short(MPUData[0]);
			MPUData[1] = switch_short(MPUData[1]);
			MPUData[2] = switch_short(MPUData[2]);
			memcpy((void*)&BackupMPUData[0], (void*)&MPUData, 6);
		}
		else {
			// if the read fails, copy the last read into the data
			memcpy((void*)&MPUData, (void*)&BackupMPUData, 6);
		}

		// read the data if device is initialized
		if(i2c_sensor_read_array(MPU9250_I2C_ADDRESS, MPU9250_REG_GYRO_XL, 6, (unsigned char*)&MPUData[3]) == ESP_OK) {
			MPUData[3] = switch_short(MPUData[3]);
			MPUData[4] = switch_short(MPUData[4]);
			MPUData[5] = switch_short(MPUData[5]);
			memcpy((void*)&BackupMPUData[3], (void*)&MPUData[3], 6);
		}
		else {
			// if the read fails, copy the last read into the data
			memcpy((void*)&BackupMPUData[3], (void*)&MPUData[3], 6);
		}
	}

	if(AKIsInitialized) {
		// Check if there is data available
		unsigned char status_reg_1 = 0;
		i2c_sensor_read_byte(AK8936_ADDRESS, AK8936_REG_STATUS1, &status_reg_1);
		if(status_reg_1)
		{
			// read the data if device is initialized
			if(i2c_sensor_read_array(AK8936_ADDRESS, AK8936_REG_XOUT_L, 6, (unsigned char*)&MPUData[6]) == ESP_OK) {
				MPUData[6] = switch_short(MPUData[6]);
				MPUData[7] = switch_short(MPUData[7]);
				MPUData[8] = switch_short(MPUData[8]);
				if(SensIsInitialized == true) {
					MPUData[6] *= ((((AK8936SenseX - 128) * 0.5) / 128) + 1);
					MPUData[7] *= ((((AK8936SenseY - 128) * 0.5) / 128) + 1);
					MPUData[8] *= ((((AK8936SenseZ - 128) * 0.5) / 128) + 1);
				}
				memcpy((void*)&BackupMPUData[6], (void*)&MPUData[6], 6);
			}
			else {
				// if the read fails, copy the last read into the data
				memcpy((void*)&BackupMPUData[6], (void*)&MPUData[6], 6);
			}
		}
		else {
			// if the data is not yet ready, copy the last read into the data
			memcpy((void*)&BackupMPUData[6], (void*)&MPUData[6], 6);
		}

		i2c_sensor_write_byte(AK8936_ADDRESS, AK8936_REG_CNTL1, AK8936_SET_16BIT | AK8936_SET_SING_SAMP);
	}

	/*ESP_LOGI("I2C TASK", "Value: \t %i, \t %i, \t %i, \t %i, \t %i, \t %i, \t %d, \t %d, \t %d \t",
			MPUData[0]/100,
			MPUData[1]/100,
			MPUData[2]/100,
			MPUData[3]/100,
			MPUData[4]/100,
			MPUData[5]/100,
			MPUData[6]/100,
			MPUData[7]/100,
			MPUData[8]/100);*/

	return &MPUData[0];
}
