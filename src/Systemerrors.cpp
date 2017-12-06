#include "Systemerrors.hpp"
// 0x1xxx = i2c errors
const unsigned short ERROR_I2C_PARAM_CONFIG      = 0x1001;  // Error codes 0x02 (i2c_master_init failed)
const unsigned short ERROR_I2C_DRIVER_INSTALL    = 0x1002;  // Error codes 0x02 (i2c_master_init failed)
const unsigned short ERROR_NVS_FLASH_INIT = 0x0001;  // Error codes 0x01 (nvs_flash_init failed)
/*#include "Systemerrors.hpp"

const unsigned short ERROR_NVS_FLASH_INIT = 0x0001;  // Error codes 0x01 (nvs_flash_init failed)



const unsigned short MPU_ERROR   =  (0x0001) << 1; // 0x0001 MPU9250 ERROR
const unsigned short AK_ERROR    =  (0x0001) << 2; // 0x0002 AK_ERROR

const unsigned short ERROR_BMP280_INIT       =  (0x0011);  // 0X0011 BMP280_ERROR
const unsigned short ERROR_BMP280_PRES       =  (0x0012);  // 0X0011 BMP280_ERROR
const unsigned short ERROR_BMP280_TEMP       =  (0x0013);  // 0X0011 BMP280_ERROR
const unsigned short ERROR_BMP280_STANDY     =  (0x0014);  // 0X0011 BMP280_ERROR
const unsigned short ERROR_BMP280_FILTER     =  (0x0015);  // 0X0011 BMP280_ERROR
const unsigned short ERROR_BMP280_POWER      =  (0x0016);  // 0X0011 BMP280_ERROR

unsigned short PeripheralErrors = 0x0000;  // Contains the errors the system peripherals have produced

unsigned short SystemErrors = 0x0000; // Errors the software has produced

const unsigned short ERROR_INIT_TYPE = 0x1000;
const unsigned short ERROR_GENERAL_TYPE = 0x2000;
const unsigned short ERROR_CRITICAL_TYPE = 0x4000;
//unsigned short SystemErrorState = 0x0000; // Contains the error states the system has such as critical level

const unsigned short SYS_OK      = 0x0000;*/
