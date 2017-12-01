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

class SensorTask : BaseTask {
public:
    SensorTask(unsigned int task_priority, DoubleBuffer &db);
    friend void sensor_handle_task(void *args);
private:
    DoubleBuffer &DBHandle;
    const int SENSORTASK_CORE_NUM = 1;
    const int SENSORTASK_PRIORITY = 2;
    const int SENSORTASK_STACK_SIZE = 2048;
    void *SENSORTASK_TIMER_ID = 0;
    const int SENSORTASK_FREQ_100HZ = pdMS_TO_TICKS( 10 );

    const int DATA_SIZE_MPU = 9;
    const int DATA_OFFSET_ACCELERO_X = 0;
    const int DATA_OFFSET_ACCELERO_Y = 1;
    const int DATA_OFFSET_ACCELERO_Z = 2;
    const int DATA_OFFSET_GYRO_X = 3;
    const int DATA_OFFSET_GYRO_Y = 4;
    const int DATA_OFFSET_GYRO_Z = 5;
    const int DATA_OFFSET_MAGNETO_X = 6;
    const int DATA_OFFSET_MAGNETO_Y = 7;
    const int DATA_OFFSET_MAGNETO_Z = 8;
    const int DATA_SIZE_BMP = 2;
    const int DATA_OFFSET_TEMPERATURE = 0;
    const int DATA_OFFSET_PRESSURE = 1;
protected:
    void main_task();
};
#endif //SENSOR_TASK_HPP
