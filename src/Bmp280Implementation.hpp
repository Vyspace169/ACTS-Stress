
#pragma once
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "bmp280.h"
#include "Sensor.hpp"
#define TAG_BMP280 "BMP280"

#define SCL_PIN GPIO_NUM_26
#define SDA_PIN GPIO_NUM_25

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

class Bmp280Implementation: public Sensor{
public:
   Bmp280Implementation();
   int DataSize() override;
   unsigned short* SensorRead() override;
   ~Bmp280Implementation() {}
private:
   struct bmp280_t bmp280_com_functions;
   unsigned short BMPData[4];
   unsigned short BackupBMPData[4];
   bool IsInitialized;
protected:
};

//void i2c_master_init(); // not important for bmp280
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BMP280_delay_msek(u32 msek);
void task_bmp280_normal_mode(void *ignore);
