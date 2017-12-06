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

void SDWriter::SetFileName(char* name) {
	if(name == NULL) {
		return;
	}

	if(*name == '/') {
		strcpy(FileNameCharArray, "/sdcard");
		strcpy(&FileNameCharArray[7], name);
	}
	else {
		strcpy(FileNameCharArray, "/sdcard/");
		strcpy(&FileNameCharArray[8], name);
	}

	FileNameIsSet = true;
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

SDWriterErrorCodes SDWriter::Write(SampleData in) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	// write the data to the file
	unsigned char *array = new unsigned char[32];
	int ArrayPointer = 0;
	memcpy(&array[ArrayPointer], &in.microTime, sizeof(long long));
	ArrayPointer += sizeof(long long);
	memcpy(&array[ArrayPointer], &in.accelX, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.accelY, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.accelZ, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.gyroX, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.gyroY, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.gyroZ, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.magnetoX, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.magnetoY, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.magnetoZ, sizeof(short));
	ArrayPointer += sizeof(short);
	memcpy(&array[ArrayPointer], &in.temp, sizeof(short));
	ArrayPointer += sizeof(int);
	memcpy(&array[ArrayPointer], &in.pressure, sizeof(short));

	int BytesWritten = fwrite(array, 32, 1, FileForData);

	//int BytesWritten = fprintf(FileForData, "%llu,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n",in.microTime,in.accelX,in.accelY,in.accelZ,in.gyroX,in.gyroY,in.gyroZ,in.magnetoX,in.magnetoY,in.magnetoZ,in.temp,in.pressure);

	/*int BytesWritten = 0;
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
	BytesWritten += fwrite(&in.pressure, sizeof(int), 1, FileForData);*/

	CardIsWriting = false;

	if(BytesWritten != 32) {
		return WRITE_ERROR;
	}

	return SD_WRITER_OK;
}

SDWriterErrorCodes SDWriter::Write(char *data) {
	if(CardIsInitialized == false) {
		return CARD_NOT_INITIALIZED;
	}

	if(FileIsOpen == false) {
		return FILE_NOT_OPEN;
	}

	CardIsWriting = true;

	ESP_LOGI("SD WRITER", "Writing data");

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
