/**
* @file SDWriter.hpp
* @data 21 september, 2017
*
* \class SDWriter
*
* This class handles writing data to
* the SD card. The transactions are all
* done in SPI mode. The class uses the
* built in functions from ESP-IDF to
* initialize the card, mount the FAT32 lib
* and write data to the card.
*
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
	/*!
	 * \brief SDWriter constructor
	 *
	 * This method initializes all states,
	 * variables and GPIO's used in this class.
	 */
	SDWriter();

	/*!
	 * \brief WaitForCard method
	 * \param timeout in milliseconds
	 * \return bool if the card is pressent (true) or not (false)
	 *
	 * This method waits for a card to be inserted. If the
	 * timeout is reached, false will be returned.
	 */
	bool WaitForCard(int timeout);

	/*!
	 * \brief InitSDMMC method
	 * \param retries The retry count for the card init
	 * \return SDWriterErrorCodes
	 *
	 * This function will call all underlying functions
	 * in the ESP-IDF to initialize an SD card in SPI mode
	 * and mount the FAT32 library.
	 */
	SDWriterErrorCodes InitSDMMC(int retries);

	/*!
	 * \brief SetFileName method
	 * \param name Char array with name
	 *
	 * Sets the used file for data to be written to.
	 */
	void SetFileName(const char* name);

	/*!
	 * \brief Open method
	 * \return SDWriterErrorCodes
	 *
	 * This method opens the used file. This must
	 * be called before writing data.
	 */
	SDWriterErrorCodes Open();

	/*!
	 * \brief Write method
	 * \return SDWriterErrorCodes
	 * \param in A single SampleData structure
	 *
	 * This method writes a single SampleData structure
	 * to the chosen and opened file.
	 */
	SDWriterErrorCodes Write(SampleData in);

	/*!
	 * \brief Write method
	 * \return SDWriterErrorCodes
	 * \param in A pointer to a SampleData array
	 * \param The size of the given array
	 *
	 * This method writes an array of SampleData
	 * to the chosen and opened file.
	 */
	SDWriterErrorCodes Write(const SampleData *in, int size);

	/*!
	 * \brief Write methode
	 * \return SDWriterErrorCodes
	 * \param data A char array
	 *
	 * This method writes an array of characters
	 * to the cosen and opened file.
	 */
	SDWriterErrorCodes Write(char *data);

	/*!
	 * \brief Close method
	 * \return SDWriterErrorCodes
	 *
	 * This method closes the opened file.
	 */
	SDWriterErrorCodes Close();

	/*!
	 * \brief WriteStatus method
	 * \return Class is writing (true) or not (false)
	 *
	 * This method returns the current write status.
	 */
	bool WriteStatus();

	/*!
	 * \brief SDWriter deconstructor
	 *
	 * Empty, not implemented.
	 */
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
