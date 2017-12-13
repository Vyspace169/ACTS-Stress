
#pragma once
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "bmp280.h"
#include "Sensor.hpp"
#include <array>
#define TAG_BMP280 "BMP280"

#define SCL_PIN GPIO_NUM_26
#define SDA_PIN GPIO_NUM_25

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

#define OUTPUT_SIZE 4

class bmp280_data{
public:
   float temp;
   float hPa;
   bmp280_data(float temp, float hPa) : temp{temp}, hPa{hPa} {}
};


class Bmp280Implementation: public Sensor{
public:
   Bmp280Implementation()  {}
   unsigned short* SensorRead() override;
   ~Bmp280Implementation() {}
   private:
protected:
};

//void i2c_master_init(); // not important for bmp280
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BMP280_delay_msek(u32 msek);
void task_bmp280_normal_mode(void *ignore);
