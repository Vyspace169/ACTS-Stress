/** \mainpage
*
****************************************************************************
* Made for the DotDotFactory, by the Hogeschool Utrecht.
*
* Copyright The DotDotFactory ( 2018 - 2019 )
*
* Date : 26/01/2018
*
****************************************************************************
*
* \section License
*
* TODOOOOO 8===>
**************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <algorithm>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs.h"
#include "nvs_flash.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_adc_cal.h"

#include "rom/rtc.h"

#include "driver/rtc_io.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#include "SystemVariables.hpp"
#include "Systemerrors.hpp"

#include "Setup.hpp"

#include "SdWriterController.hpp"
#include "SensorController.hpp"
#include "StandbyController.hpp"
#include "WifiController.hpp"

RTC_DATA_ATTR struct timeval SleepEnterTime;
EventGroupHandle_t GlobalEventGroupHandle;
struct timeval GlobalTimeValNow;
time_t GlobalStartTime;

extern "C" void app_main(void)
{
	ESP_LOGI("MAIN", "Booting completed"); // @suppress("Symbol is not resolved")
	
	// Print wakeup cause
	int WakeUpCause = esp_sleep_get_wakeup_cause();
	switch(WakeUpCause) {
	case ESP_SLEEP_WAKEUP_TIMER: ESP_LOGI("MAIN", "Woke up from a timer reset"); break; // @suppress("Symbol is not resolved")
	case ESP_SLEEP_WAKEUP_EXT1: ESP_LOGI("MAIN", "Woke up from SD Card"); break; // @suppress("Symbol is not resolved")
	case ESP_SLEEP_WAKEUP_EXT0: ESP_LOGI("MAIN", "Woke up from Motion Interrupt"); break; // @suppress("Symbol is not resolved")
	default: ESP_LOGI("MAIN", "Woke up from normal reset"); break; // @suppress("Symbol is not resolved")
	}

	// Initialize GPIO
	gpio_init_all();
	// Initialize I2C
	i2c_master_init();
	// Check if sd card is present, else sleep
	CheckForSdcard();
	// Initialize flash
	nvs_flash_init();
	// Read errors from last run
	error_flash_init();
	// Initialize global event handle group
	GlobalEventGroupHandle = xEventGroupCreate();

	// Check if BOD was enabled, if so go to sleep again
	int ResetCause = rtc_get_reset_reason(0);
	ESP_LOGI("MAIN", "Reset reason: %d", ResetCause); // @suppress("Symbol is not resolved")
	if(ResetCause == 15 && gpio_get_level(GPIO_CHARGE_DETECT) == 0) {
		ESP_LOGI("MAIN", "Reset reason was BOD and no charger detecte, going to sleep"); // @suppress("Symbol is not resolved")
		esp_sleep_enable_timer_wakeup(600 * 1000000);
		esp_deep_sleep_start();
	}

	// Set blink color and frequency
	blink_set_led(GPIO_LED_GREEN, 10, 5000);
	// Start blink task
	xTaskCreatePinnedToCore(&blink_task, "blink_task", BLINKTASK_STACK_SIZE, NULL, BLINKTASK_PRIORITY, NULL, BLINKTASK_CORE_NUM);

	// Feedback
    ESP_LOGI("MAIN", "Creating SNTP task"); // @suppress("Symbol is not resolved")
    // Taskhandle for sntp task
    TaskHandle_t SNTPTaskHandle;
    // Start SNTP task. Wifi is also initialized here
    xTaskCreatePinnedToCore(sntp_task, "sntp_task", SNTPTASK_STACK_SIZE, NULL, SNTPTASK_PRIORITY, &SNTPTaskHandle, SNTPTASK_CORE_NUM);
    // Wait for task to be done
    xEventGroupWaitBits(GlobalEventGroupHandle, SNTPTaskDoneFlag, pdTRUE, pdFALSE, portMAX_DELAY);
    // Delete task
    vTaskDelete(SNTPTaskHandle);

    // Calculate and print sleep time
    int sleep_time_ms = (GlobalTimeValNow.tv_sec - SleepEnterTime.tv_sec) * 1000 + (GlobalTimeValNow.tv_usec - SleepEnterTime.tv_usec) / 1000;
    ESP_LOGI("MAIN", "Time spent in deep sleep: %d ms", sleep_time_ms); // @suppress("Symbol is not resolved")

    // Build filename
    char name[64];
    BuildFileName(name, sizeof(name));
    // Create SDWriter object
    SDWriter *GlobalSDWriter = new SDWriter;
    // Initialize card
    GlobalSDWriter->InitSDMMC(SDMMC_INIT_RETRIES);
    // Set filename used for writing movement data
    GlobalSDWriter->SetFileName(name);
    // Set filename used for writing RR data
    GlobalSDWriter->SetFileName("RR_intervals");


    // Create DoubleBuffer object
    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);

    // Create DoubleBuffer object
    BinaryBuffer *GlobalBinaryBuffer = new BinaryBuffer();

    // Create DataProcessor object
    DataProcessor *GlobalDataHandler = new DataProcessor(*GlobalDoubleBuffer);
    // Set timeout value for DataProcessor
    GlobalDataHandler->SetTimeoutValue(TIMEOUT_TIME_SEC * 1000);
    // Set triggers for data DataProcessor
    GlobalDataHandler->SetTrigger(DP_SLEEP_THRESHOLD, DP_SLEEP_THRESHOLD, DP_SLEEP_THRESHOLD);



    // Create and run Standby task
    StandbyController *sbc = new StandbyController(STANDBYCONT_PRIORITY);

    // Create and run Sensor task
    SensorController *st = new SensorController(SENSORTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalDataHandler);

    // Create and run SDWriter task
    SdWriterController *sdw = new SdWriterController(WRITERTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalSDWriter, *GlobalDataHandler);

    // Create and run Wifi task
    WifiController *wt = new WifiController(WIFITASK_PRIORITY, *GlobalDataHandler);

    // Initialization done
    ESP_LOGI("MAIN", "Init done"); // @suppress("Symbol is not resolved")
}
