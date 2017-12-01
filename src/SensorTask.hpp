#ifndef SENSOR_TASK_HPP
#define SENSOR_TASK_HPP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_event_loop.h"

#include "BaseTask.hpp"
#include "DoubleBuffer.hpp"
#include "Mpu9250Implementation.hpp"
#include "Bmp280Implementation.hpp"

#define Timer_100hz        pdMS_TO_TICKS( 10 )

class SensorTask : BaseTask {
public:
    SensorTask(unsigned int task_priority, DoubleBuffer &db);
    friend void sensor_handle_task(void *args);
private:
    DoubleBuffer &DBHandle;
protected:
    void main_task();
};
#endif //SENSOR_TASK_HPP
