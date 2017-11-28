#include "Bmp280Implementation.hpp"

//#define TAG_BMP280 "BMP280"

#define SCL_PIN GPIO_NUM_26
#define SDA_PIN GPIO_NUM_25

#define I2C_MASTER_ACK 0
#define I2C_MASTER_NACK 1

s8 BMP280_I2C_bus_write(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
   s32 iError = BMP280_INIT_VALUE;

   esp_err_t espRc;
   i2c_cmd_handle_t cmd = i2c_cmd_link_create();

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);

   i2c_master_write_byte(cmd, reg_addr, true);
   i2c_master_write(cmd, reg_data, cnt, true);
   i2c_master_stop(cmd);

   espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
   if (espRc == ESP_OK) {
      iError = SUCCESS;
   } else {
      iError = FAIL;
   }
   i2c_cmd_link_delete(cmd);

   return (s8)iError;
}

s8 BMP280_I2C_bus_read(u8 dev_addr, u8 reg_addr, u8 *reg_data, u8 cnt)
{
   s32 iError = BMP280_INIT_VALUE;
   esp_err_t espRc;

   i2c_cmd_handle_t cmd = i2c_cmd_link_create();

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_WRITE, true);
   i2c_master_write_byte(cmd, reg_addr, true);

   i2c_master_start(cmd);
   i2c_master_write_byte(cmd, (dev_addr << 1) | I2C_MASTER_READ, true);

   if (cnt > 1) {
      i2c_master_read(cmd, reg_data, cnt-1, I2C_MASTER_ACK);
   }
   i2c_master_read_byte(cmd, reg_data+cnt-1, I2C_MASTER_NACK);
   i2c_master_stop(cmd);

   espRc = i2c_master_cmd_begin(I2C_NUM_0, cmd, 10/portTICK_PERIOD_MS);
   if (espRc == ESP_OK) {
      iError = SUCCESS;
   } else {
      iError = FAIL;
   }

   i2c_cmd_link_delete(cmd);

   return (s8)iError;
}
void BMP280_delay_msek(u32 msek)
{
   vTaskDelay(msek/portTICK_PERIOD_MS);
}

bmp280_data Bmp280Implementation::GetBmp280Data()   {
/*   struct bmp280_t bmp280 = {
      .bus_write = BMP280_I2C_bus_write,
      .bus_read = BMP280_I2C_bus_read,
      .dev_addr = BMP280_I2C_ADDRESS2,
      .delay_msec = BMP280_delay_msek
   };*/
   
   struct bmp280_t bmp280;
   bmp280.bus_write = BMP280_I2C_bus_write;
   bmp280.bus_read = BMP280_I2C_bus_read;
   bmp280.dev_addr = BMP280_I2C_ADDRESS2;
   bmp280.delay_msec = BMP280_delay_msek;

   s32 com_rslt;
   s32 v_uncomp_pressure_s32;
   s32 v_uncomp_temperature_s32;
   //s32 v_uncomp_humidity_s32;

   com_rslt = bmp280_init(&bmp280);

   com_rslt += bmp280_set_oversamp_pressure(BMP280_OVERSAMP_16X);
   com_rslt += bmp280_set_oversamp_temperature(BMP280_OVERSAMP_2X);
   //com_rslt += bmp280_set_oversamp_humidity(BMP280_OVERSAMP_1X);

   com_rslt += bmp280_set_standby_durn(BMP280_STANDBY_TIME_1_MS);
   com_rslt += bmp280_set_filter(BMP280_FILTER_COEFF_16);

   com_rslt += bmp280_set_power_mode(BMP280_NORMAL_MODE);
   if (com_rslt == SUCCESS) {
      vTaskDelay(40/portTICK_PERIOD_MS);
      com_rslt = bmp280_read_uncomp_pressure_temperature(&v_uncomp_pressure_s32, &v_uncomp_temperature_s32);
      // com_rslt = bme280_read_uncomp_pressure_temperature_humidity(
      //    &v_uncomp_pressure_s32, &v_uncomp_temperature_s32, &v_uncomp_humidity_s32);

      if (com_rslt == SUCCESS) {
         ESP_LOGI(TAG_BMP280, "%.2f degrees Celsius / %.3f hPa / ",
            bmp280_compensate_temperature_double(v_uncomp_temperature_s32),
            bmp280_compensate_pressure_double(v_uncomp_pressure_s32)/100); // Pa -> hPa
         return bmp280_data(bmp280_compensate_temperature_double(v_uncomp_temperature_s32), 
            bmp280_compensate_pressure_double(v_uncomp_pressure_s32)/100);
            //bme280_compensate_humidity_double(v_uncomp_humidity_s32));
      } else {
         ESP_LOGE(TAG_BMP280, "measure error. code: %d", (int)com_rslt);
      }
      // while(true) {

      // }
   } else {
      ESP_LOGE(TAG_BMP280, "init or setting error. code: %d", (int)com_rslt);
   }
   return bmp280_data(0, 0);
   //vTaskDelete(NULL);
}
