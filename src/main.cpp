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

#include "SystemVariables.hpp"

#include "Sensor.hpp"
#include "SDWriter.hpp"
#include "WifiModule.hpp"
#include "DoubleBuffer.hpp"
#include "Mpu9250Implementation.hpp"
#include "Bmp280Implementation.hpp"

#include "BaseTask.hpp"
#include "SensorTask.hpp"

// Define led pin
#define BLINK_GPIO  GPIO_NUM_13

void blink_task(void *pvParameter)
{
    gpio_pad_select_gpio(BLINK_GPIO);
    // Set the GPIO as a push/pull output
    gpio_set_direction(BLINK_GPIO, (gpio_mode_t)GPIO_MODE_OUTPUT);
    while(1) {
        // Blink off (output low)
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
        // Blink on (output high)
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

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

EventGroupHandle_t GlobalEventGroupHandle;

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Booting completed\n");

    nvs_flash_init();

    i2c_example_master_init();

    SDWriter writer;
    writer.InitSDMMC();

    GlobalEventGroupHandle = xEventGroupCreate();

    SDWriter *GlobalSDWriter = new SDWriter;
    DataProcessor *GlobalDataHandler = new DataProcessor;
    DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);
    SensorTask *st = new SensorTask(1, *GlobalDoubleBuffer, *GlobalDataHandler);



    // Start blink task
    xTaskCreate(&blink_task, "blink_task", configMINIMAL_STACK_SIZE, NULL, 5, NULL);

    // start sample task
    //xTaskCreatePinnedToCore(&sample_task, "sample_task", 8192, NULL, 5, NULL, 0);

    // start writer task
    //xTaskCreatePinnedToCore(&writer_task, "writer_task", 4092, NULL, 5, NULL, 0);


    //xTaskCreatePinnedToCore(&wifi_task, "wifi_task", 10000, NULL, 0, NULL, 0);




    ESP_LOGI("MAIN", "Init done");
}
