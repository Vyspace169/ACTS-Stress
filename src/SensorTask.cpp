#include "SensorTask.hpp"

//<<<<<<< HEAD
/*SensorTask::SensorTask(unsigned int task_priority, DoubleBuffer &db) : 
    BaseTask(task_priority), 
    DBHandle{db}, 
    Sensor_MPU{new Mpu9250Implementation()},
    Sensor_BMP{new Bmp280Implementation()}  {
        
    main_task();
}*/
//=======
SensorTask::SensorTask(unsigned int task_priority, DoubleBuffer &db, DataProcessor &dp) : BaseTask(task_priority), DBHandle{db}, DataHandler{dp} { main_task(); }
//>>>>>>> 206602684acdfac9c52ab87ab4b993e28466331d

void sensor_handle_task(void *args)  {
	SensorTask *sTask = static_cast<SensorTask*>(args);

	data SensorData;
	EventBits_t uxBits;
	short MPUData[sTask->DATA_SIZE_MPU];
	int BMPData[sTask->DATA_SIZE_BMP];

    // Should be changed?
    Sensor *TestMPU = sTask->Sensor_MPU;
    //Sensor *TestMPU = new Mpu9250Implementation();
    //Sensor *TestBMP = new Mpu9250Implementation();
    Sensor *TestBMP = sTask->Sensor_BMP;

    while(1) {

    	// wait for flag to be set
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, SensorMeasurementFlag, pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorMeasurementFlag)  {
            //SensorMeasurementFlag has been set
        	memcpy(MPUData, TestMPU->SensorRead(), TestMPU->DataSize());
        	memcpy(BMPData, TestBMP->SensorRead(), TestBMP->DataSize());

        	SensorData.accelX = 	(float)MPUData[sTask->DATA_OFFSET_ACCELERO_X];
        	SensorData.accelY = 	(float)MPUData[sTask->DATA_OFFSET_ACCELERO_Y];
        	SensorData.accelZ = 	(float)MPUData[sTask->DATA_OFFSET_ACCELERO_Z];
        	SensorData.gyroX = 		(float)MPUData[sTask->DATA_OFFSET_GYRO_X];
        	SensorData.gyroY = 		(float)MPUData[sTask->DATA_OFFSET_GYRO_Y];
        	SensorData.gyroZ = 		(float)MPUData[sTask->DATA_OFFSET_GYRO_Z];
        	SensorData.magnetoX = 	(float)MPUData[sTask->DATA_OFFSET_MAGNETO_X];
        	SensorData.magnetoY = 	(float)MPUData[sTask->DATA_OFFSET_MAGNETO_Y];
        	SensorData.magnetoZ = 	(float)MPUData[sTask->DATA_OFFSET_MAGNETO_Z];
        	SensorData.temp = 		(float)BMPData[sTask->DATA_OFFSET_TEMPERATURE];
        	SensorData.pressure = 	(float)BMPData[sTask->DATA_OFFSET_PRESSURE];

        	sTask->DataHandler.HandleData(SensorData);
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
    wifi_poll_timer = xTimerCreate("sensor_poll_clock",
    								SENSORTASK_FREQ_100HZ,
									pdTRUE,
									SENSORTASK_TIMER_ID,
									set_sensor_measurement_bit);
    xTimerStart( wifi_poll_timer, 0 );

    if(wifi_poll_timer == NULL) {
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
													SENSORTASK_PRIORITY,
													&xHandle,
													SENSORTASK_CORE_NUM);


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
