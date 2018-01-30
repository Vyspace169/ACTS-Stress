<<<<<<< HEAD
=======

>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
#pragma once

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

extern void blink_task(void *pvParameter);

extern void sntp_task(void* param);

extern void gpio_init_all();

extern void blink_set_led(gpio_num_t led, int time_on, int time_off);

extern void i2c_master_init();

extern void error_flash_init();

extern void BuildFileName(char *TimeStringBuffer, int str_len);

extern void CheckForSdcard();
