#pragma once

extern const unsigned short ERROR_NVS_FLASH_INIT;  // Error codes 0x01 (nvs_flash_init failed)

// 0x1xxx = i2c errors
extern const unsigned short ERROR_I2C_PARAM_CONFIG;  // Error codes 0x02 (i2c_master_init failed)
extern const unsigned short ERROR_I2C_DRIVER_INSTALL;  // Error codes 0x02 (i2c_master_init failed)

extern const unsigned short MPU_ERROR; // 0x0001 MPU9250 ERROR
extern const unsigned short AK_ERROR; // 0x0002 AK_ERROR
extern const unsigned short BMP_ERROR; // 0X0004 BMP280_ERROR

extern unsigned short PeripheralErrors = 0x0000;  // Contains the errors the system peripherals have produced

extern unsigned short SBUFFER_SWAP_ERROR = 0x0001;

extern unsigned short SystemErrors = 0x0000; // Errors the software has produced

extern const unsigned short SYSTEM_PERIPHERAL_ERROR_RANGE = (0x01) << 1
extern const unsigned short CRITICAL_ERROR_RANGE = (0x01) << 2;
extern unsigned short SystemErrorState = 0x0000; // Contains the error states the system has such as critical level

extern const unsigned short SYS_OK;
