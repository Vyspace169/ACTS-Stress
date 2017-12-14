#include "SensorTask.hpp"

SensorTask::SensorTask(unsigned int task_priority, DoubleBuffer &db, DataProcessor &dp) : 
    BaseTask(task_priority), 
    DBHandle{db}, 
    DataHandler{dp},     
    Sensor_MPU{new Mpu9250Implementation()},
    Sensor_BMP{new Bmp280Implementation()}
    { 
        main_task(); 
    }

void sensor_handle_task(void *args)  {
	SensorTask *sTask = static_cast<SensorTask*>(args);
	SampleData SensorData;
	EventBits_t uxBits;
	short MPUData[sTask->Sensor_MPU->DataSize() / sizeof(short)];
	int BMPData[sTask->Sensor_BMP->DataSize() / sizeof(int)];

    while(1) {
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (SensorMeasurementFlag | StandbySensorTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorMeasurementFlag)  {
        	memcpy(MPUData, sTask->Sensor_MPU->SensorRead(), sTask->Sensor_MPU->DataSize());
        	memcpy(BMPData, sTask->Sensor_BMP->SensorRead(), sTask->Sensor_BMP->DataSize());

        	SensorData.accelX = 	MPUData[sTask->DATA_OFFSET_ACCELERO_X];
        	SensorData.accelY = 	MPUData[sTask->DATA_OFFSET_ACCELERO_Y];
        	SensorData.accelZ = 	MPUData[sTask->DATA_OFFSET_ACCELERO_Z];
        	SensorData.gyroX = 		MPUData[sTask->DATA_OFFSET_GYRO_X];
        	SensorData.gyroY = 		MPUData[sTask->DATA_OFFSET_GYRO_Y];
        	SensorData.gyroZ = 		MPUData[sTask->DATA_OFFSET_GYRO_Z];
        	SensorData.magnetoX = 	MPUData[sTask->DATA_OFFSET_MAGNETO_X];
        	SensorData.magnetoY = 	MPUData[sTask->DATA_OFFSET_MAGNETO_Y];
        	SensorData.magnetoZ = 	MPUData[sTask->DATA_OFFSET_MAGNETO_Z];
        	SensorData.temp = 		BMPData[sTask->DATA_OFFSET_TEMPERATURE];
        	SensorData.pressure = 	BMPData[sTask->DATA_OFFSET_PRESSURE];
        	SensorData.microTime =  xTaskGetTickCount();

        	sTask->DataHandler.HandleData(SensorData);
        	sTask->DBHandle.storeData(SensorData);
        }

        if(uxBits & StandbySensorTaskUnhandled) {
        	sTask->Sensor_BMP->Sleep();
        	sTask->Sensor_MPU->Sleep();
        	ESP_LOGI("SENSOR TASK", "Ready to sleep");
        	xEventGroupClearBits(GlobalEventGroupHandle, StandbySensorTaskUnhandled);
        	while(1) {
        		vTaskDelay(1000 / portTICK_PERIOD_MS);
        	}
        }
    }
}

void print_struct(SampleData SensorData) {
	ESP_LOGI("SENSOR TASK", "Value: \t %llu, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d \t",
			SensorData.microTime,
			SensorData.accelX,
   			SensorData.accelY,
   			SensorData.accelZ,
   			SensorData.gyroX,
   			SensorData.gyroY,
   			SensorData.gyroZ,
   			SensorData.magnetoX,
   			SensorData.magnetoY,
   			SensorData.magnetoZ,
   			SensorData.temp,
   			SensorData.pressure);
}


void set_sensor_measurement_bit( TimerHandle_t xTimer )  {
    xEventGroupSetBits(GlobalEventGroupHandle, SensorMeasurementFlag);
}

void SensorTask::main_task() {
	ESP_LOGI("SENSOR TASK", "Task starting...");

	TimerHandle_t sample_poll_timer = NULL;
	sample_poll_timer = xTimerCreate("sensor_poll_clock",
			SAMPLE_TIME_MS,
			pdTRUE,
			SENSORTASK_TIMER_ID,
			set_sensor_measurement_bit);
    xTimerStart(sample_poll_timer, 0 );

    if(sample_poll_timer == NULL) {
    	// Something has failed creating the timer
    	ESP_LOGI("SENSOR TASK", "Timer creation failed");
    } else {
    	ESP_LOGI("SENSOR TASK", "Timer created");
    }

    TaskHandle_t xHandle = NULL;
    void* thisTask = static_cast<void*>(this);
    BaseType_t xReturned = xTaskCreatePinnedToCore(sensor_handle_task,
    												"sensor_task",
													SENSORTASK_STACK_SIZE,
													thisTask,
													task_priority,
													&xHandle,
													SENSORTASK_CORE_NUM);

    if(xHandle == NULL) {
    	// Handle assignment has failed
    	ESP_LOGI("SENSOR TASK", "Handle creation failed");
    }

    if(xReturned != pdPASS) {
    	// xReturned false (something went wrong!)
    	ESP_LOGI("SENSOR TASK", "Task creation failed");
    }

    ESP_LOGI("SENSOR TASK", "Task is running");
}
