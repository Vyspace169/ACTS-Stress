/**
* @file SDReader.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/

#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

const char *SD_READER_TAG = "SD Reader";
const gpio_num_t BLINK_GPIO = GPIO_NUM_13;
const gpio_num_t PIN_NUM_MISO = GPIO_NUM_23;
const gpio_num_t PIN_NUM_MOSI = GPIO_NUM_19;
const gpio_num_t PIN_NUM_CLK = GPIO_NUM_18;
const gpio_num_t PIN_NUM_CS = GPIO_NUM_5;
const int SDCardSPISpeedkHz = 3000;
const int SDCardCMDTimeoutms = 0;

#pragma once
class  SDReader{
    public:
        SDReader();
        bool InitSDMMC();
        int  Read(unsigned char* p_Data, int DataLength);
        bool Status();
        ~SDReader();
    private:
        bool CardIsReading;
        bool CardIsInitialized;
        sdmmc_card_t* SDCardReader;
};
