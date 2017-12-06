/**
* @file StandbyController.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/

#pragma once

#include "esp_sleep.h"
#include "BaseTask.hpp"

#define TIMEOUT_TIME_SEC 	60
#define SC_LOOP_DELAY		250

class  StandbyController : BaseTask {
public:
    StandbyController(unsigned int task_priority);
    friend void sensor_handle_task(void *args);
private:

protected:
    void main_task();
};
