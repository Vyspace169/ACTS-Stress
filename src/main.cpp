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

void wifi_task(void *pvParameter) {

	char* ip = "192.168.43.140";
	char* subnet = "192.168.43.1";
	char* mask = "255.255.255.0";
	char* ssid = "Allyouare";
	char* pass = "Meulen-2017";
	WifiModule wificonnection;
	wificonnection.ClientSetStaticIP(ip, subnet, mask);
	wificonnection.ClientConfig(ssid, pass);
	wificonnection.ClientConnect(10000);

	while(1){
		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}


static void i2c_example_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_25;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_NUM_26;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0);
}

void blink_task(void *pvParameter)
{
    while(1) {
    	/*gpio_set_level(GPIO_LED_RED, 0);
    	gpio_set_level(GPIO_LED_BLUE, 1);
        vTaskDelay(800 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_LED_GREEN, 0);
        gpio_set_level(GPIO_LED_RED, 1);
        vTaskDelay(800 / portTICK_PERIOD_MS);
        gpio_set_level(GPIO_LED_BLUE, 0);
        gpio_set_level(GPIO_LED_GREEN, 1);
        vTaskDelay(800 / portTICK_PERIOD_MS);*/

    	gpio_set_level(GPIO_LED_GREEN, 1);
    	vTaskDelay(5000 / portTICK_PERIOD_MS);
    	gpio_set_level(GPIO_LED_GREEN, 0);
    	vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}

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

void to_test() {
	//adc2_config_width(ADC_WIDTH_12Bit);
	//adc2_config_channel_atten(ADC2_CHANNEL_3, ADC_ATTEN_11db);
	//int val = adc2_get_raw(ADC2_CHANNEL_3);
	//ESP_LOGI("MAIN", "%d ", val);
}

EventGroupHandle_t GlobalEventGroupHandle;

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

    nvs_flash_init();

    gpio_init_leds();

    i2c_example_master_init();

    GlobalEventGroupHandle = xEventGroupCreate();

    SDWriter *GlobalSDWriter = new SDWriter;
    GlobalSDWriter->InitSDMMC(4);
    GlobalSDWriter->SetFileName("filename.bin");

    DataProcessor *GlobalDataHandler = new DataProcessor;
    GlobalDataHandler->SetTimeoutValue(TIMEOUT_TIME_MS);
    GlobalDataHandler->SetTrigger(TRIGGER_VALUE_X, TRIGGER_VALUE_Y, TRIGGER_VALUE_Z);

    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);

    //WifiModule *testmodule = new WifiModule;

    SensorTask *st = new SensorTask(SENSORTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalDataHandler);

    SdWriterTask *sdw = new SdWriterTask(WRITERTASK_PRIORITY, *GlobalDoubleBuffer, *GlobalSDWriter);

    StandbyController *sbc = new StandbyController(STANDBYCONT_PRIORITY);

    //WifiTask *wt = new WifiTask(WIFITASK_PRIORITY);

    // Start blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    ESP_LOGI("MAIN", "Init done");
}
