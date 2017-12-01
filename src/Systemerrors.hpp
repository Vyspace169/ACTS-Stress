#pragma once

extern const unsigned short ERROR_NVS_FLASH_INIT = 0x0001;  // Error codes 0x01 (nvs_flash_init failed)

// 0x1xxx = i2c errors
extern const unsigned short ERROR_I2C_PARAM_CONFIG      = 0x1001;  // Error codes 0x02 (i2c_master_init failed)
extern const unsigned short ERROR_I2C_DRIVER_INSTALL    = 0x1002;  // Error codes 0x02 (i2c_master_init failed)

extern const unsigned short MPU_ERROR   =  (0x0001) << 1; // 0x0001 MPU ERROR
extern const unsigned short AK_ERROR    =  (0x0001) << 2; // 0x0002 AK_ERROR
extern const unsigned short BMP_ERROR   =  (0x0001) << 3; // 0X0004 BMP_ERROR

extern unsigned short SystemErrorState = 0x0000;

extern const unsigned short SYS_OK      = 0x0000;
