#include "SensorTask.hpp"

SensorTask::SensorTask(unsigned int task_priority, DoubleBuffer &db) : BaseTask(task_priority), DBHandle{db}  {main_task();}

void sensor_handle_task(void *args)  {
	SensorTask *sTask = static_cast<SensorTask*>(args);
	Sensor *TestMPU = new Mpu9250Implementation();
	Sensor *TestBMP = new Bmp280Implementation();
	data SensorData;
	EventBits_t uxBits;
	short MPUData[9];
	int BMPData[2];

    while(1) {

    	// wait for flag to be set
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, SensorMeasurementFlag, pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorMeasurementFlag)  {
            //SensorMeasurementFlag has been set
        	memcpy(MPUData, TestMPU->SensorRead(), sizeof(unsigned short) * 9);
        	memcpy(BMPData, TestBMP->SensorRead(), sizeof(int) * 2);

        	SensorData.accelX = (float)MPUData[0];
        	SensorData.accelY = (float)MPUData[1];
        	SensorData.accelZ = (float)MPUData[2];
        	SensorData.gyroX = (float)MPUData[3];
        	SensorData.gyroY = (float)MPUData[4];
        	SensorData.gyroZ = (float)MPUData[5];
        	SensorData.magnetoX = (float)MPUData[6];
        	SensorData.magnetoY = (float)MPUData[7];
        	SensorData.magnetoZ = (float)MPUData[8];
        	SensorData.temp = (float)BMPData[0];
        	SensorData.pressure = (float)BMPData[1];

        	sTask->DBHandle.storeData(SensorData);

        	/*ESP_LOGI("I2C TASK", "Value: \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f, \t %.0f \t",
        			SensorData.accelX/100,
        			SensorData.accelY/100,
        			SensorData.accelZ/100,
        			SensorData.gyroX/100,
        			SensorData.gyroY/100,
        			SensorData.gyroZ/100,
        			SensorData.magnetoX/100,
        			SensorData.magnetoY/100,
        			SensorData.magnetoZ/100,
        			SensorData.temp,
        			SensorData.pressure);*/
        }
    }
}

void set_sensor_measurement_bit( TimerHandle_t xTimer )  {
    //Todo set bit 4
    xEventGroupSetBits(GlobalEventGroupHandle, SensorMeasurementFlag);
}

void SensorTask::main_task() {
	ESP_LOGI("SENSOR TASK", "Task starting...");

	TimerHandle_t wifi_poll_timer = NULL;
    wifi_poll_timer = xTimerCreate("sensor_poll_clock", Timer_100hz, pdTRUE, 0, set_sensor_measurement_bit);
    xTimerStart( wifi_poll_timer, 0 );

    if(wifi_poll_timer == NULL) {
    	// Something has failed creating the timer
    	ESP_LOGI("SENSOR TASK", "Timer creation failed");
    } else {
    	ESP_LOGI("SENSOR TASK", "Timer created");
    }

    TaskHandle_t xHandle = NULL;
    void* thisTask = static_cast<void*>(this);
    BaseType_t xReturned = xTaskCreatePinnedToCore(sensor_handle_task, "sensor_task", 2048, thisTask, 2, &xHandle, 1);

    if(xHandle == NULL) {
    	// Handle assignment has failed
    	ESP_LOGI("SENSOR TASK", "Handle creation failed");
    } else {
    	ESP_LOGI("SENSOR TASK", "Handle creation OK");
    }

    if(xReturned != pdPASS) {
    	// xReturned false (something went wrong!)
    	ESP_LOGI("SENSOR TASK", "Task creation failed");
    } else {
    	ESP_LOGI("SENSOR TASK", "Task creation OK");
    }

    ESP_LOGI("SENSOR TASK", "Task is running");
}
