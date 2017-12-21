#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "nvs_flash.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"

#include "nvs.h"

#include "driver/gpio.h"
#include "driver/adc.h"

#include "SystemVariables.hpp"

#include "Sensor.hpp"
#include "SDWriter.hpp"
#include "WifiModule.hpp"
#include "DoubleBuffer.hpp"
#include "Mpu9250Implementation.hpp"
#include "Bmp280Implementation.hpp"

#include "BaseTask.hpp"
#include "SensorTask.hpp"

#include "SdWriterTask.hpp"
#include "StandbyController.hpp"
#include "WifiTask.hpp"

#include "Systemerrors.hpp"

#define SNTP_BIT   ( 1 << 0 )
EventGroupHandle_t GlobalEventGroupHandle;

static void gpio_init_leds() {
	// Select gipo
	gpio_pad_select_gpio(GPIO_LED_RED);
	gpio_pad_select_gpio(GPIO_LED_GREEN);
	gpio_pad_select_gpio(GPIO_LED_BLUE);
	// Set the GPIO as a push/pull output
	gpio_set_direction(GPIO_LED_RED, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_LED_GREEN, GPIO_MODE_OUTPUT);
	gpio_set_direction(GPIO_LED_BLUE, GPIO_MODE_OUTPUT);
	// Set initial state
	gpio_set_level(GPIO_LED_RED, 1);
	gpio_set_level(GPIO_LED_GREEN, 1);
	gpio_set_level(GPIO_LED_BLUE, 1);
}

void blink_task(void *pvParameter)
{
    while(1) {
    	gpio_set_level(GPIO_LED_GREEN, 1);
    	vTaskDelay(5000 / portTICK_PERIOD_MS);
    	gpio_set_level(GPIO_LED_GREEN, 0);
    	vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

static void i2c_master_init()
{
	// give scl pin some clocks
	gpio_pad_select_gpio(GPIO_SCL);
	gpio_set_direction(GPIO_SCL, GPIO_MODE_OUTPUT);
	for(int i=0;i<100;i++) {
		gpio_set_level(GPIO_SCL, 1);
		vTaskDelay(1);
		gpio_set_level(GPIO_SCL, 0);
		vTaskDelay(1);
	}

	// initialize bus
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_SCL;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_SPEED;

	if(i2c_param_config(I2C_NUM_0, &conf) != ESP_OK)  {
		ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c param config failed!", ERROR_I2C_PARAM_CONFIG);
	}

	if(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0) != ESP_OK) {
		ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c driver install failed!", ERROR_I2C_DRIVER_INSTALL);
	}
}


void error_flash_init() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES) {
        // NVS partition was truncated and needs to be erased
        // Retry nvs_flash_init
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    nvs_handle my_handle;
    err = nvs_open("storage", NVS_READWRITE, &my_handle);
    int32_t error_value = (int32_t) ErrorCode::NO_ERROR; // value will default to 0, if not set yet in NVS
    err = nvs_get_i32(my_handle, "error_value", &error_value);
    bool failed = false;
    switch (err) {
        case ESP_OK:
        	ESP_LOGI("INIT", "Done");
        	ESP_LOGI("INIT", "Restart counter = %d", error_value);
            break;
        case ESP_ERR_NVS_NOT_FOUND:
            failed = true;
            break;
        default :
        	ESP_LOGI("INIT", "Error (%d) reading!", err);
    }
    if(failed)  {
        //int32_t error_code = (int32_t) error::errCode; // value will default to 0, if not set yet in NVS
        err = nvs_set_i32(my_handle, "error_value", (int32_t) ErrorCode::NO_ERROR);
        err = nvs_commit(my_handle);
        nvs_close(my_handle);
    }
    else    {
        nvs_close(my_handle);
    }
    if(error_value != (int32_t) ErrorCode::NO_ERROR) {
        ESP_LOGI("INIT", "Last ran ended with error code: %i\n", error_value);
    }
    else    {
        ESP_LOGI("INIT", "NO EXCEPTION FOUND IN LAST RUN");
    }
}
void init_task(void *param) {

}
EventGroupHandle_t sntp_evt_grp;
void sntp_task(void* param) {
    ESP_LOGI("SNTP_TASK", "ENTERING SNTP TASK");
	WiFiInitialize(WIFI_SSID, WIFI_PASSWORD);
	bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);
	time_t this_moment = WiFiGetTime(20);
	WiFiDisconnect();
    
	struct tm timeinfo;
	localtime_r(&this_moment, &timeinfo);
	char strftime_buf[64];
	strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	ESP_LOGI("SNTP TASK", "Central time:  %s  ", strftime_buf);

	while(1) {
	   	vTaskDelay(5000 / portTICK_PERIOD_MS);
        /* Set bit 0 and bit 4 in xEventGroup. */
    xEventGroupSetBits(
                              sntp_evt_grp,    /* The event group being updated. */
                              SNTP_BIT );/* The bits being set. */

	}
}

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Booting completed");

    int WakeUpCause = esp_sleep_get_wakeup_cause();
    if(WakeUpCause == ESP_SLEEP_WAKEUP_TIMER) {
    	ESP_LOGI("MAIN", "Woke up from a timer reset");
    }
    else {
    	ESP_LOGI("MAIN", "Woke up from reset");
    }

    if(nvs_flash_init() != ESP_OK)   {
    	ESP_LOGE("INIT_ERROR", "Error code: %i , message: nvs_flash_init failed!", ERROR_NVS_FLASH_INIT);
    }

    error_flash_init();

    i2c_master_init();

    gpio_init_leds();

    // sntp
    /* Declare a variable to hold the created event group. */
    

    /* Attempt to create the event group. */
    sntp_evt_grp = xEventGroupCreate();
    

      /* Wait a maximum of 100ms for either bit 0 or bit 4 to be set within
  the event group.  Clear the bits before exiting. */
    BaseType_t xReturned = xTaskCreatePinnedToCore(sntp_task,
                                                    "sntp_task",
                                                    16000,
                                                    NULL,
                                                    1,
                                                    NULL,
                                                    0);

    xEventGroupWaitBits(
            sntp_evt_grp,   /* The event group being tested. */
            SNTP_BIT, /* The bits within the event group to wait for. */
            pdTRUE,        /* BIT_0 & BIT_4 should be cleared before returning. */
            pdFALSE,       /* Don't wait for both bits, either bit will do. */
            portMAX_DELAY );/* Wait a maximum of 100ms for either bit to be set. */
    ESP_LOGI("SNTP", "SNTP DONE");
    // end_sntp

    GlobalEventGroupHandle = xEventGroupCreate();

    SDWriter *GlobalSDWriter = new SDWriter;
    GlobalSDWriter->InitSDMMC(SDMMC_INIT_RETRIES);
    GlobalSDWriter->SetFileName("filename.bin");

    DataProcessor *GlobalDataHandler = new DataProcessor;
    GlobalDataHandler->SetTimeoutValue(TIMEOUT_TIME_SEC * 1000);
    GlobalDataHandler->SetTrigger(TRIGGER_VALUE_X, TRIGGER_VALUE_Y, TRIGGER_VALUE_Z);

    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);

    SensorTask *st = new SensorTask(SENSORTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalDataHandler);

    //SdWriterTask *sdw = new SdWriterTask(WRITERTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalSDWriter);

    StandbyController *sbc = new StandbyController(STANDBYCONT_PRIORITY);

    WifiTask *wt = new WifiTask(WIFITASK_PRIORITY, *GlobalDataHandler);

    // Start blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Init done");
}
