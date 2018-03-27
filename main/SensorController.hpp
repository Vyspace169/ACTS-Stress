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
#include "DataProcessor.hpp"
#include "Mpu9250Implementation.hpp"
#include "Bmp280Implementation.hpp"
#include "ECGImplementation.hpp"

/**
* @file SensorController.hpp
* @data 21 september, 2017
*
* \class SensorController
*
* \brief Task that handles all sensor communication
*
* This task will sample at a frequency of SAMPE_RATE_H data
* from all sensors (in this case BMP280 and MPU9250). A xTimer
* will run at that frequency and set a bit. The task will then
* read data from the sensors (which are connected through I2C)
* and send this data to the DoubleBuffer and DataProcessor.
*
* If the sleepbit has been set this task will go into an infinite loop.
*
* Added ecg sensor
*
*/
class SensorController : BaseTask {
public:
	SensorController(unsigned int task_priority, DoubleBuffer &db, DataProcessor &dp);
    friend void sensor_handle_task(void *args);
private:
    DoubleBuffer &DBHandle;
    DataProcessor &DataHandler;
    void *SENSORTASK_TIMER_ID = 0;

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
    const int DATA_SIZE_ECG = 2;
    const int DATA_OFFSET_SAMPLE_VALUE = 0;
    const int DATA_OFFSET_SAMPLE_NR = 1;

   Sensor *Sensor_MPU;
   Sensor *Sensor_BMP;
   Sensor *Sensor_ECG;

protected:
    void main_task();
};
#endif //SENSOR_TASK_HPP
