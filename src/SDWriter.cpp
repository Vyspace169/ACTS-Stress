#include "SDWriter.hpp"

SDWriter::SDWriter() {
	FileIsOpen = false;
	CardIsWriting = false;
	CardIsInitialized = false;
	FileNameIsSet = false;
	SDCardWriter = NULL;
	FileForData = NULL;
	memset(FileNameCharArray, 0, 256);
}

SDWriterErrorCodes SDWriter::InitSDMMC() {
	ESP_LOGI("SD WRITER", "Initializing SD card using SPI peripheral");

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
		ESP_LOGI("SD WRITER", "Card already initialized");
	}
	else if (ret != ESP_OK) {
	    if (ret == ESP_FAIL) {
	        ESP_LOGI("SD WRITER", "Failed to mount filesystem. ");
	    }
	    else {
	        ESP_LOGI("SD WRITER", "Failed to initialize the card, code: %d", ret);
	    }
	    CardIsInitialized = false;
	    return CARD_NOT_INITIALIZED;
	}

	//sdmmc_card_print_info(stdout, card);
	ESP_LOGI("SD WRITER", "Card init OK");
	CardIsInitialized = true;
	return SD_WRITER_OK;
}

void SDWriter::SetFileName(time_t TimeReceivedFromWifi) {
	strcpy(FileNameCharArray,"/sdcard/testfile.bin");
	FileNameIsSet = true;
	//strcpy(FileNameCharArray,"/sdcard/");
	//strcat(FileNameCharArray, itoa(TimeReceivedFromWifi));
}

SDWriterErrorCodes SDWriter::Open() {
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
	ESP_LOGI("SD WRITER", "Opening file");

	// Write data
	FileForData = fopen(FileNameCharArray, "a");

	// check if file open succeeded
	if (FileForData == NULL) {
		// feedback
		ESP_LOGE("SD WRITER", "Failed to open file for writing");
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

SDWriterErrorCodes SDWriter::Write(void *data, int length) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	int BytesWritten = fwrite(data, length, 1, FileForData);

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

	// feedback
	ESP_LOGI("SD WRITER", "File closed");

	FileIsOpen = false;
	return SD_WRITER_OK;
}

bool SDWriter::WriteStatus() {
	return CardIsWriting;
}

SDWriter::~SDWriter(){

}
