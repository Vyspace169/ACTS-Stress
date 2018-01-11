/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief 
*/

#pragma once

#include <string.h>
#include <stdio.h>

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "driver/gpio.h"
#include "sdmmc_cmd.h"
#include "BinaryBuffer.hpp"
#include "SystemVariables.hpp"

/**
 * Enum return type for the SDWriter class
 */
enum SDWriterErrorCodes
{
	SD_WRITER_OK, 			/**< There were no errors detected */
	CARD_NOT_INITIALIZED, 	/**< The SDMMC peripheral and card were not initialized */
	FILE_NOT_OPEN, 			/**< The file was not opened */
	WRITE_ERROR, 			/**< The write gave an error */
	FILE_NAME_NOT_SET,		/**< The file name was not yet set */
	CARD_NOT_IN_SLOT		/**< The card detect pin is asserted low, not high */
};

class SDWriter{
public:
	/// Constructer for the SDWriter class.
	SDWriter();

	bool WaitForCard(int timeout);
	bool CardDetectState();

	/**
	 * Initialization for the SDMMC peripheral on the ESP32.
	 * This function will call all underlying functions in the ESP-IDF to initialize the SDMMC peripheral in SPI mode.
	 * \return a SDWriterErrorCodes enum
	 */
	SDWriterErrorCodes InitSDMMC(int retries);


	void SetFileName(const char* name);
	SDWriterErrorCodes Open();
	SDWriterErrorCodes Write(SampleData in);
	SDWriterErrorCodes Write(const SampleData *in, int size);
	SDWriterErrorCodes Write(char *data);
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
