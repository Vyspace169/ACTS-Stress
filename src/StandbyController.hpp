/**
* @file StandbyController.hpp
* @data 21 september, 2017
*
* \class StandbyController
*
* \brief Class that controlls the devices sleepmodes
*
* This class will take care of every sleep and reset condition.
* Currently the system knows the following:
*  - Reset when the device gets taken of the charger;
*  - Sleep until a pin interrupt when SD card is removed;
*  - Sleep SLEEP_TIME_SEC after TIMEOUT_TIME_SEC with no movement.
*
* If one of the states requests a deep sleep, bits for all running tasks
* (WiFi, Sensor & SDWriter) will be set. Every task will go
* into a ready to sleep routine and reset these bits. This task will notice
* all three bits have been reset and will enable the ESP32's deep sleep.
*
*/

#pragma once

#include "esp_sleep.h"
#include "esp_log.h"

#include "driver/adc.h"

#include <time.h>
#include <sys/time.h>

#include "BaseTask.hpp"

#include "Setup.hpp"

class  StandbyController : BaseTask {
public:
    StandbyController(unsigned int task_priority);
    friend void sensor_handle_task(void *args);
private:

protected:
    void main_task();
};
