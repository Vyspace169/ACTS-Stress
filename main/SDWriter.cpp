#include "SDWriter.hpp"

SDWriter::SDWriter() {
	FileIsOpen = false;
	CardIsWriting = false;
	CardIsInitialized = false;
	FileNameIsSet = false;
	SDCardWriter = NULL;
	FileForData = NULL;
	memset(FileCharArray, 0, 256);
}

SDWriterErrorCodes SDWriter::InitSDMMC() {
	ESP_LOGI(SD_READER_TAG, "Initializing SD card using SPI peripheral");

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	host.max_freq_khz = SDCardSPISpeedkHz;

	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_MISO;
	slot_config.gpio_mosi = PIN_NUM_MOSI;
	slot_config.gpio_sck  = PIN_NUM_CLK;
	slot_config.gpio_cs   = PIN_NUM_CS;

	esp_vfs_fat_sdmmc_mount_config_t mount_config;
	mount_config.format_if_mount_failed = false;
	mount_config.max_files = 5;

	// Use settings defined above to initialize SD card and mount FAT filesystem
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &SDCardWriter);

	if (ret == ESP_ERR_INVALID_STATE) {
		ESP_LOGI(SD_READER_TAG, "Card already initialized");
	}
	else if (ret != ESP_OK) {
	    if (ret == ESP_FAIL) {
	        ESP_LOGI(SD_READER_TAG, "Failed to mount filesystem. ");
	    }
	    else {
	        ESP_LOGI(SD_READER_TAG, "Failed to initialize the card, code: %d", ret);
	    }
	    CardIsInitialized = false;
	    return CARD_NOT_INITIALIZED;
	}

	// Card has been initialized, print its properties
	//sdmmc_card_print_info(stdout, card);
	CardIsInitialized = true;
	return SD_WRITER_OK;
}

void SDWriter::SetFileName(time_t TimeReceivedFromWifi) {
	strcpy(FileCharArray,"/sdcard/testfile.bin");
	FileNameIsSet = true;
	//strcpy(FileCharArray,"/sdcard/");
	//strcat(FileCharArray, itoa(TimeReceivedFromWifi));
}

SDWriterErrorCodes SDWriter::Open(char *filename) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == true) {
		return SD_WRITER_OK;
	}

	if(FileNameIsSet == false) {
		return FILE_NAME_NOT_SET;
	}

	// feedback
	ESP_LOGI(SD_READER_TAG, "Opening file");

	// Write data
	FileForData = fopen(FileCharArray, "a");

	// check if file open succeeded
	if (FileForData == NULL) {
		// feedback
		ESP_LOGE(SD_READER_TAG, "Failed to open file for writing");
		return FILE_NOT_OPEN;
	}

	FileIsOpen = true;
	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Write(data in) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	int BytesWritten = fwrite(&in, sizeof(data), 1, FileForData);

	CardIsWriting = false;

	if(BytesWritten != sizeof(data)) {
		return WRITE_ERROR;
	}

	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Close() {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == false) {
		return SD_WRITER_OK;
	}

	// close the file
	fclose(FileForData);

	FileIsOpen = false;
	return SD_WRITER_OK;
}

bool SDWriter::WriteStatus() {
	return CardIsWriting;
}

SDWriter::~SDWriter(){

}
