/**
* @file Setup.hpp
* @data 21 september, 2017
*
* \brief This file contains all setup functions
*
* In this file are a couple of functions defined
* which are used during the startup procedure
* of the system.
*
*/

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

#include "SdWriterController.hpp"
#include "SensorController.hpp"
#include "StandbyController.hpp"
#include "WifiController.hpp"

/*!
 * \brief task used for blinking the status LED
 * \param parameters
 *
 * This function defines a loop that blinks the
 * status LEd (RGB). using the blink_set_led
 * function the color, time on and time off can
 * be set.
 */
extern void blink_task(void *pvParameter);

/*!
 * \brief task that set the global time
 * \param parameters
 *
 * This function defines a loop which reads
 * the boot cause, and depending on which cause
 * was set the time will be read from wifi.
 *
 * Note: this function is used to initialize
 * the wifi and should always be started before
 * using wifi. This task should always be run
 * on the wifi core (core 0).
 */
extern void sntp_task(void* param);

/*!
 * \brief This initializes all used GPIO
 *
 * Function used to initialize all GPIO used by
 * tasks.
 */
extern void gpio_init_all();

/*!
 * \brief function used to set the status led
 * \param led that should blink
 * \param time_on in milliseconds
 * \param time_off in milliseconds
 *
 * This function sets the LED color and time on / off
 * in milliseconds.
 */
extern void blink_set_led(gpio_num_t led, int time_on, int time_off);

/*!
 * \brief initialize the I2C bus
 *
 * This is used to initialize the I2C bus on the
 * ESP32 (I2C_NUM_0). In the SystemVariables the
 * SDA pin, SCL pin and frequency are declared.
 */
extern void i2c_master_init();

/*!
 * \brief function that reads any set error variables
 */
extern void error_flash_init();

/*!
 * \brief Function that builds the used file name
 * \param TimeStringBuffer for string
 * \param str_len maximum
 *
 * This function builds a filename for the SDWriter.
 * If the SNTP task has set the global time, this is used.
 * else the internal flash is used to read the last filename.
 *
 */
extern void BuildFileName(char *TimeStringBuffer, int str_len);

/*!
 * \brief function that checks if the SD card is in its slot
 *
 * This function lets the device go into sleep if there is no
 * SD-Card detected. Otherwise it will just return.
 */
extern void CheckForSdcard();
