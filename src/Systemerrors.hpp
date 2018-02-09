#pragma once
// 0x1xxx = i2c errors
extern const unsigned short ERROR_I2C_PARAM_CONFIG;  // Error codes 0x02 (i2c_master_init failed)
extern const unsigned short ERROR_I2C_DRIVER_INSTALL;  // Error codes 0x02 (i2c_master_init failed)
extern const unsigned short ERROR_NVS_FLASH_INIT;  // Error codes 0x01 (nvs_flash_init failed)
/*extern const unsigned short ERROR_NVS_FLASH_INIT;  // Error codes 0x01 (nvs_flash_init failed)

extern const unsigned short MPU_ERROR; // 0x0001 MPU9250 ERROR
extern const unsigned short AK_ERROR; // 0x0002 AK_ERROR
extern const unsigned short BMP_ERROR; // 0X0004 BMP280_ERROR

extern const unsigned short ERROR_BMP280_INIT;  // 0X0011 BMP280_ERROR
extern const unsigned short ERROR_BMP280_PRES;  // 0X0011 BMP280_ERROR
extern const unsigned short ERROR_BMP280_TEMP;  // 0X0011 BMP280_ERROR
extern const unsigned short ERROR_BMP280_STANDY;  // 0X0011 BMP280_ERROR
extern const unsigned short ERROR_BMP280_FILTER;  // 0X0011 BMP280_ERROR
extern const unsigned short ERROR_BMP280_POWER;  // 0X0011 BMP280_ERROR

extern unsigned short PeripheralErrors = 0x0000;  // Contains the errors the system peripherals have produced

extern unsigned short SBUFFER_SWAP_ERROR = 0x0001;

extern unsigned short SystemErrors = 0x0000; // Errors the software has produced

const unsigned short ERROR_INIT_TYPE = 0x1000;
const unsigned short ERROR_GENERAL_TYPE = 0x2000;
const unsigned short ERROR_CRITICAL_TYPE = 0x4000;
//extern unsigned short SystemErrorState = 0x0000; // Contains the error states the system has such as critical level

extern const unsigned short SYS_OK;
*/