/*
 * HRVController.hpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#pragma once

#include "esp_sleep.h"
#include "esp_log.h"

#include <time.h>
#include <sys/time.h>

#include "BaseTask.hpp"

#include "Setup.hpp"

class  HRVController : BaseTask {
public:
    HRVController(unsigned int task_priority);
    friend void sensor_handle_task(void *args);
private:

protected:
    void main_task();
};






