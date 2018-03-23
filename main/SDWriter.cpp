#include "SDWriter.hpp"

SDWriter::SDWriter() {
	// set variables initial state
	FileIsOpen = false;
	CardIsWriting = false;
	CardIsInitialized = false;
	FileNameIsSet = false;
	SDCardWriter = NULL;
	FileForData = NULL;
	memset(FileNameCharArray, 0, 256);
}

bool SDWriter::WaitForCard(int timeout) {
	int counter = 0;
	int gpio_state = 0;
	while(true) {
		gpio_state = gpio_get_level(GPIO_SD_DETECT);
		if(gpio_state == 1) {
			break;
		}
		else if(counter >= timeout) {
			break;
		}
		else {
			counter += 100;
		}
		vTaskDelay(100 / portTICK_PERIOD_MS); // @suppress("Symbol is not resolved") // @suppress("Invalid arguments")
	}

	if(counter >= timeout) {
		return false;
	}
	else {
		return true;
	}
}

SDWriterErrorCodes SDWriter::InitSDMMC(int retries) {
	ESP_LOGI("SD WRITER", "Initializing SD card using SPI peripheral"); // @suppress("Symbol is not resolved")

	sdmmc_host_t host = SDSPI_HOST_DEFAULT();

	host.flags = SDMMC_HOST_FLAG_SPI;
	host.slot = HSPI_HOST;
	host.max_freq_khz = SDMMC_FREQ_DEFAULT;
	host.io_voltage = 3.3f;
	host.set_bus_width = NULL;
	host.command_timeout_ms = 0;
	host.init = &sdspi_host_init;
	host.set_card_clk = &sdspi_host_set_card_clk;
	host.do_transaction = &sdspi_host_do_transaction;
	host.deinit = &sdspi_host_deinit;
	host.max_freq_khz = SD_CARD_SPI_SPEED_KHZ;

	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_MISO;
	slot_config.gpio_mosi = PIN_NUM_MOSI;
	slot_config.gpio_sck  = PIN_NUM_CLK;
	slot_config.gpio_cs   = PIN_NUM_CS;

	esp_vfs_fat_sdmmc_mount_config_t mount_config;
	mount_config.format_if_mount_failed = false;
	mount_config.max_files = 5;

	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		return CARD_NOT_IN_SLOT;
	}

	int i = 0;
	for(i=0;i<retries;i++) {
		// Use settings defined above to initialize SD card and mount FAT filesystem
		esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config, &mount_config, &SDCardWriter);

		if (ret == ESP_OK) {
			CardIsInitialized = true;
			break;
		}
		else {
			if (ret == ESP_FAIL) {
				ESP_LOGI("SD WRITER", "Failed to mount filesystem. "); // @suppress("Symbol is not resolved")
			}
			else {
				ESP_LOGI("SD WRITER", "Failed to initialize the card, code: %d", ret); // @suppress("Symbol is not resolved")
			}
			CardIsInitialized = false;
		}
	}

	//sdmmc_card_print_info(stdout, card);

	if(CardIsInitialized == false) {
		ESP_LOGI("SD WRITER", "Card init failed. Retries: %d", i + 1); // @suppress("Symbol is not resolved")
		return CARD_NOT_INITIALIZED;
	}
	else {

		// Create directory for measurement data
		struct stat st;
		char *DirStr = new char[256];
		strcpy(DirStr, "/sdcard/");
		strcat(DirStr, DIRECTORY_NAME);
		if(stat(DirStr, &st) == -1) {
		    if(mkdir(DirStr, ACCESSPERMS) == -1){
		    	ESP_LOGI("SD WRITTER", "Creating directory %s failed", DirStr); // @suppress("Symbol is not resolved")
		    	perror("mkdri: ");
		    }
		    else {
		    	ESP_LOGI("SD WRITTER", "Creating directory succeded"); // @suppress("Symbol is not resolved")
		    }
		}
		delete(DirStr);


		ESP_LOGI("SD WRITER", "Card init OK. Retries: %d", i + 1); // @suppress("Symbol is not resolved")
		return SD_WRITER_OK;
	}
}

void SDWriter::SetFileName(const char* name) {
	if(name == NULL) {
		return;
	}

	strcpy(FileNameCharArray, "/sdcard/");
	strcat(FileNameCharArray, DIRECTORY_NAME);
	strcat(FileNameCharArray, "/");
	strcat(FileNameCharArray, name);

	ESP_LOGI("SD WRITER", "File name set: %s", FileNameCharArray); // @suppress("Symbol is not resolved")

	FileNameIsSet = true;
}

SDWriterErrorCodes SDWriter::Open() {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		return CARD_NOT_IN_SLOT;
	}

	if(FileIsOpen == true) {
		return SD_WRITER_OK;
	}

	if(FileNameIsSet == false) {
		return FILE_NAME_NOT_SET;
	}

	// Open file
	FileForData = fopen(FileNameCharArray, "a");

	// check if file open succeeded
	if (FileForData == NULL) {
		// feedback
		ESP_LOGE("SD WRITER", "Failed to open file for writing"); // @suppress("Symbol is not resolved")
		return FILE_NOT_OPEN;
	}

	FileIsOpen = true;
	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Write(SampleData in) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		return CARD_NOT_IN_SLOT;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	//int BytesWritten = fprintf(FileForData, "%llu,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n",in.microTime,in.accelX,in.accelY,in.accelZ,in.gyroX,in.gyroY,in.gyroZ,in.magnetoX,in.magnetoY,in.magnetoZ,in.temp,in.pressure);

	int BytesWritten = 0;
	BytesWritten += fwrite(&in.microTime, sizeof(long long), 1, FileForData);
	BytesWritten += fwrite(&in.accelX, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.accelY, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.accelZ, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.gyroX, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.gyroY, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.gyroZ, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.magnetoX, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.magnetoY, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.magnetoZ, sizeof(short), 1, FileForData);
	BytesWritten += fwrite(&in.temp, sizeof(int), 1, FileForData);
	BytesWritten += fwrite(&in.pressure, sizeof(int), 1, FileForData);
	BytesWritten += fwrite(&in.ECGSampleValue, sizeof(short), 1, FileForData);


	CardIsWriting = false;

	if(BytesWritten != 32) {
		return WRITE_ERROR;
	}

	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Write(const SampleData *in, int size) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		return CARD_NOT_IN_SLOT;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	int BytesWritten = fwrite(in, size, 1, FileForData);

	CardIsWriting = false;

	if(BytesWritten != 1) {
		ESP_LOGI("SD WRITER", "Write failed"); // @suppress("Symbol is not resolved")
		return WRITE_ERROR;
	}

	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Write(char *data) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(gpio_get_level(GPIO_SD_DETECT) == 0) {
		return CARD_NOT_IN_SLOT;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	int BytesWritten = fprintf(FileForData, data);

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
