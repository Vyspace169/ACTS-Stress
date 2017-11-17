#include "SDReader.hpp"

SDReader::SDReader() {
	CardIsInitialized = false;
}

bool SDReader::InitSDMMC() {
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
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &SDCardReader);

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
	    return false;
	}

	// Card has been initialized, print its properties
	//sdmmc_card_print_info(stdout, card);
	CardIsInitialized = true;
	return true;
}

int SDReader::Read(unsigned char* p_Data, int DataLength) {
	if(!CardIsInitialized) {
		return 0;
	}

	// feedback
	ESP_LOGI(SD_READER_TAG, "Opening file");

	FILE* FileForData = fopen("/sdcard/ACTS sensor data.bin", "r");

	//check if file open succeeded
	if (FileForData == NULL) {
		// feedback
		ESP_LOGE(SD_READER_TAG, "Failed to open file for writing");
		return 0;
	}

	CardIsReading = true;

	// write the data to the file
	int BytesRead = fread(p_Data, DataLength, 1, FileForData);

	// close the file
	fclose(FileForData);

	CardIsReading = false;

	// feedback
	ESP_LOGI(SD_READER_TAG, "File written");

	return BytesRead;
}

bool SDReader::Status() {
	return CardIsReading;
}

SDReader::~SDReader() {

}
