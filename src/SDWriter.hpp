/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief 
*/

#pragma once

#include <string.h>
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "BinaryBuffer.hpp"

const gpio_num_t PIN_NUM_MISO = GPIO_NUM_23;
const gpio_num_t PIN_NUM_MOSI = GPIO_NUM_19;
const gpio_num_t PIN_NUM_CLK = GPIO_NUM_18;
const gpio_num_t PIN_NUM_CS = GPIO_NUM_5;
const int SDCardSPISpeedkHz = 1000;
const int SDCardCMDTimeoutms = 0;

/**
 * Enum return type for the SDWriter class
 */
enum SDWriterErrorCodes
{
	SD_WRITER_OK, 			/**< There were no errors detected */
	CARD_NOT_INITIALIZED, 	/**< The SDMMC peripheral and card were not initialized */
	FILE_NOT_OPEN, 			/**< The file was not opened */
	WRITE_ERROR, 			/**< The write gave an error */
	FILE_NAME_NOT_SET		/**< The file name was not yet set */
};

class SDWriter{
public:
	/// Constructer for the SDWriter class.
	SDWriter();

	/**
	 * Initialization for the SDMMC peripheral on the ESP32.
	 * This function will call all underlying functions in the ESP-IDF to initialize the SDMMC peripheral in SPI mode.
	 * \return a SDWriterErrorCodes enum
	 */
	SDWriterErrorCodes InitSDMMC();


	void SetFileName(time_t TimeReceivedFromWifi);
	SDWriterErrorCodes Open();
	SDWriterErrorCodes Write(data in);
	SDWriterErrorCodes Write(void *data, int length);
	SDWriterErrorCodes Close();
	bool WriteStatus();
	~SDWriter();

private:
	bool FileIsOpen;
	bool CardIsWriting;
    bool CardIsInitialized;
    bool FileNameIsSet;
    char FileNameCharArray[256];
    sdmmc_card_t* SDCardWriter;
    FILE* FileForData;
};
