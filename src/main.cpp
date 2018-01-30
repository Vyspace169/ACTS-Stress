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

RTC_DATA_ATTR struct timeval SleepEnterTime;
EventGroupHandle_t GlobalEventGroupHandle;
struct timeval GlobalTimeValNow;
time_t GlobalStartTime;

extern "C" void app_main(void)
{
	ESP_LOGI("MAIN", "Booting completed");

	// Print wakeup cause
	int WakeUpCause = esp_sleep_get_wakeup_cause();
	switch(WakeUpCause) {
	case ESP_SLEEP_WAKEUP_TIMER:
	 	ESP_LOGI("MAIN", "Woke up from a timer reset");
		break;
	case ESP_SLEEP_WAKEUP_EXT1:
		ESP_LOGI("MAIN", "Woke up from SD Card");
		break;
	case ESP_SLEEP_WAKEUP_EXT0:
		ESP_LOGI("MAIN", "Woke up from Motion Interrupt");
		break;
	default:
		ESP_LOGI("MAIN", "Woke up from normal reset");
		break;
	}

	// Initialize couple of stuff
	gpio_init_all();
	i2c_master_init();
	CheckForSdcard();
	nvs_flash_init();
	error_flash_init();
	GlobalEventGroupHandle = xEventGroupCreate();

	// Start blink task
	blink_set_led(GPIO_LED_GREEN, 10, 5000);
	xTaskCreatePinnedToCore(&blink_task, "blink_task", BLINKTASK_STACK_SIZE, NULL, BLINKTASK_PRIORITY, NULL, BLINKTASK_CORE_NUM);

	// Check if BOD was enabled, if so go to sleep
	int ResetCause = rtc_get_reset_reason(0);
	ESP_LOGI("MAIN", "Reset reason: %d", ResetCause);
	if(ResetCause == 15 && gpio_get_level(GPIO_CHARGE_DETECT) == 0) {
		ESP_LOGI("MAIN", "Reset reason was BOD and no charger detecte, going to sleep");
		esp_sleep_enable_timer_wakeup(600 * 1000000);
		esp_deep_sleep_start();
	}

	// Start SNTP task. Wifi is also initialized here
    ESP_LOGI("MAIN", "Creating SNTP task");
    TaskHandle_t SNTPTaskHandle;
    xTaskCreatePinnedToCore(sntp_task, "sntp_task", SNTPTASK_STACK_SIZE, NULL, SNTPTASK_PRIORITY, &SNTPTaskHandle, SNTPTASK_CORE_NUM);
    xEventGroupWaitBits(GlobalEventGroupHandle, SNTPTaskDoneFlag, pdTRUE, pdFALSE, portMAX_DELAY);
    vTaskDelete(SNTPTaskHandle);

    // Print sleep time
    int sleep_time_ms = (GlobalTimeValNow.tv_sec - SleepEnterTime.tv_sec) * 1000 + (GlobalTimeValNow.tv_usec - SleepEnterTime.tv_usec) / 1000;
    ESP_LOGI("MAIN", "Time spent in deep sleep: %d ms", sleep_time_ms);

    char name[64];
    BuildFileName(name, sizeof(name));
    SDWriter *GlobalSDWriter = new SDWriter;
    GlobalSDWriter->InitSDMMC(SDMMC_INIT_RETRIES);
    GlobalSDWriter->SetFileName(name);

    DataProcessor *GlobalDataHandler = new DataProcessor;
    GlobalDataHandler->SetTimeoutValue(TIMEOUT_TIME_SEC * 1000);
    GlobalDataHandler->SetTrigger(TRIGGER_VALUE_X, TRIGGER_VALUE_Y, TRIGGER_VALUE_Z);

    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);

    SensorTask *st = new SensorTask(SENSORTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalDataHandler);

    SdWriterTask *sdw = new SdWriterTask(WRITERTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalSDWriter);

    StandbyController *sbc = new StandbyController(STANDBYCONT_PRIORITY);

    WifiTask *wt = new WifiTask(WIFITASK_PRIORITY, *GlobalDataHandler);

	ESP_LOGI("MAIN", "Init done");
}
