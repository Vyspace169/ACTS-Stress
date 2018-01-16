/**
* @file StandbyController.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/

#pragma once

#include "esp_sleep.h"
#include "esp_log.h"

#include "BaseTask.hpp"

class  StandbyController : BaseTask {
public:
    StandbyController(unsigned int task_priority);
    friend void sensor_handle_task(void *args);
private:

protected:
    void main_task();
};
