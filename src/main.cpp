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
#include "Systemerrors.hpp"
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
/*static void error_msg(char* err_type, char* c, err_code ec)  {
   ESP_LOGE("INIT_ERROR", "Error code:%i , msg_detail: nvs_flash_init failed!")
}*/

static void i2c_master_init()
{
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = GPIO_NUM_25;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = GPIO_NUM_26;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = 100000;
   if(i2c_param_config(I2C_NUM_0, &conf) != ESP_OK)  {
      ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c param config failed!", ERROR_I2C_PARAM_CONFIG);
   }
   if(i2c_driver_install(I2C_NUM_0, conf.mode, 0, 0, 0) != ESP_OK) {
      ESP_LOGE("INIT_ERROR", "Error code:%i , message: i2c driver install failed!", ERROR_I2C_DRIVER_INSTALL);
   }
}

void app_init()  {

   if(nvs_flash_init() != ESP_OK)   {
      ESP_LOGE("INIT_ERROR", "Error code:%i , message: nvs_flash_init failed!", ERROR_NVS_FLASH_INIT);
   }
   i2c_master_init();
   /*if(i2c_master_init() != ESP_OK)  {
      //ESP_LOGE(err_type, "Error code:%i , message: nvs_flash_init failed!", I2C_MASTER_INIT);
   }*/

}

EventGroupHandle_t GlobalEventGroupHandle;

extern "C" void app_main(void)
{
    ESP_LOGI("MAIN", "Booting completed\n");

    app_init();

    SDWriter writer;
    writer.InitSDMMC();

    GlobalEventGroupHandle = xEventGroupCreate();

    SDWriter *GlobalSDWriter = new SDWriter;

    auto *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);
    auto *st = new SensorTask(1, *GlobalDoubleBuffer);
    
    //DataProcessor *GlobalDataHandler = new DataProcessor;
    //DoubleBuffer *GlobalDoubleBuffer = new DoubleBuffer(*GlobalSDWriter);
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
