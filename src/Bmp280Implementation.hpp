#pragma once
#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"
#include "esp_log.h"
#include "freertos/task.h"
#include "bmp280.h"
#include "Sensor.hpp"
#include "Systemerrors.hpp"
#include "Errorhandler.hpp"
#define TAG_BMP280 "BMP280"

#define SCL_PIN GPIO_NUM_26
#define SDA_PIN GPIO_NUM_25

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1
enum class BMP280ErrTypes{
   NO_ERROR,
   INIT_ERROR_1,
   INIT_ERROR_2,
   INIT_ERROR_3,
   INIT_ERROR_4,
   INIT_ERROR_5,
   INIT_ERROR_6,
};
class Bmp280Implementation: public Sensor{
public:
   Bmp280Implementation();
   int DataSize() override;
   unsigned short* SensorRead() override;
   //void HandleError() override;
   //std::string Msg() override;
   ~Bmp280Implementation() {}
private:
   struct bmp280_t bmp280_com_functions;
   unsigned short BMPData[4];
   unsigned short BackupBMPData[4];
   bool IsInitialized;
   BMP280ErrTypes error_type = BMP280ErrTypes::NO_ERROR;
protected:
};

//void i2c_master_init(); // not important for bmp280
s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt);
void BMP280_delay_msek(u32 msek);
void task_bmp280_normal_mode(void *ignore);
