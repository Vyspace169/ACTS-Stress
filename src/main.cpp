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

	/*gpio_config_t io_conf;
	//disable interrupt
	io_conf.intr_type = GPIO_INTR_DISABLE;
	//set as output mode
	io_conf.mode = GPIO_MODE_INPUT;
	//bit mask of the pins that you want to set,e.g.GPIO18/19
	io_conf.pin_bit_mask = GPIO_PW_GOOD;
	//disable pull-down mode
	io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
	//disable pull-up mode
	io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
	//configure GPIO with the given settings
	gpio_config(&io_conf);

	while(1){
		int test = gpio_get_level(GPIO_PW_GOOD);
		ESP_LOGI("MAIN", "level: %d", test);
		vTaskDelay(500 / portTICK_PERIOD_MS);
	}*/
}

static void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = PIN_NUM_SDA;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = PIN_NUM_SCL;
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

    GlobalEventGroupHandle = xEventGroupCreate();

    SDWriter *GlobalSDWriter = new SDWriter;
    GlobalSDWriter->InitSDMMC(SDMMC_INIT_RETRIES);
    GlobalSDWriter->SetFileName("filename.bin");

    DataProcessor *GlobalDataHandler = new DataProcessor;
    GlobalDataHandler->SetTimeoutValue(TIMEOUT_TIME_SEC * 1000);
    GlobalDataHandler->SetTrigger(TRIGGER_VALUE_X, TRIGGER_VALUE_Y, TRIGGER_VALUE_Z);

    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);

    SensorTask *st = new SensorTask(SENSORTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalDataHandler);

    SdWriterTask *sdw = new SdWriterTask(WRITERTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalSDWriter);

    StandbyController *sbc = new StandbyController(STANDBYCONT_PRIORITY);

    WifiTask *wt = new WifiTask(WIFITASK_PRIORITY, *GlobalDataHandler);

    // Start blink task
    xTaskCreate(&blink_task, "blink_task", 2048, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Init done");
}
