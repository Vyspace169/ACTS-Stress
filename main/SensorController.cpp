#include "SensorController.hpp"

SensorController::SensorController(unsigned int task_priority, DoubleBuffer &db, DataProcessor &dp) :
    BaseTask(task_priority),
    DBHandle{db},
    DataHandler{dp},
    Sensor_MPU{new Mpu9250Implementation()},
    Sensor_BMP{new Bmp280Implementation()},
	Sensor_ECG{new ECGImplementation()}
    {
        main_task();
    }

void sensor_handle_task(void *args)  {
	SensorController *sTask = static_cast<SensorController*>(args);
	SampleData SensorData;
	RData Potential_R;
	EventBits_t uxBits;
	short MPUData[sTask->Sensor_MPU->DataSize() / sizeof(short)];
	int BMPData[sTask->Sensor_BMP->DataSize() / sizeof(int)];
	short ECGData[sTask->Sensor_ECG->DataSize() / sizeof(short)];

    while(1) {
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (SensorMeasurementFlag | StandbySensorTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorMeasurementFlag)  {
        	memcpy(MPUData, sTask->Sensor_MPU->SensorRead(), sTask->Sensor_MPU->DataSize());
        	memcpy(BMPData, sTask->Sensor_BMP->SensorRead(), sTask->Sensor_BMP->DataSize());
        	memcpy(ECGData, sTask->Sensor_ECG->SensorRead(), sTask->Sensor_ECG->DataSize());

        	SensorData.accelX = 			MPUData[sTask->DATA_OFFSET_ACCELERO_X];
        	SensorData.accelY = 			MPUData[sTask->DATA_OFFSET_ACCELERO_Y];
        	SensorData.accelZ = 			MPUData[sTask->DATA_OFFSET_ACCELERO_Z];
        	SensorData.gyroX = 				MPUData[sTask->DATA_OFFSET_GYRO_X];
        	SensorData.gyroY = 				MPUData[sTask->DATA_OFFSET_GYRO_Y];
        	SensorData.gyroZ = 				MPUData[sTask->DATA_OFFSET_GYRO_Z];
        	SensorData.magnetoX = 			MPUData[sTask->DATA_OFFSET_MAGNETO_X];
        	SensorData.magnetoY = 			MPUData[sTask->DATA_OFFSET_MAGNETO_Y];
        	SensorData.magnetoZ = 			MPUData[sTask->DATA_OFFSET_MAGNETO_Z];
        	SensorData.temp = 				BMPData[sTask->DATA_OFFSET_TEMPERATURE];
        	SensorData.pressure = 			BMPData[sTask->DATA_OFFSET_PRESSURE];
        	SensorData.microTime =  		xTaskGetTickCount();
        	SensorData.ECGSampleValue = 	ECGData[sTask->DATA_OFFSET_SAMPLE_VALUE];
        	SensorData.ECGSampleNumber = 	ECGData[sTask->DATA_OFFSET_SAMPLE_VALUE];

        	sTask->DataHandler.HandleData(SensorData);
        	sTask->DBHandle.storeData(SensorData);

        	if(SensorData.ECGSampleValue > R_PEAK_THRESHOLD){
        		Potential_R.potentialRPeak = 	SensorData.ECGSampleValue;
        		Potential_R.sampleNr = 			SensorData.ECGSampleNumber;
        		sTask->DBHandle.storeRData(Potential_R);
        		//ESP_LOGI("SensorController", "Value: %d", SensorData.ECGSampleValue); // @suppress("Symbol is not resolved")
        		//ESP_LOGI("SensorController", "Value above threshold."); // @suppress("Symbol is not resolved")

        	}
        }

        if(uxBits & StandbySensorTaskUnhandled) {
        	sTask->Sensor_BMP->Sleep();
        	sTask->Sensor_MPU->Sleep();
        	sTask->Sensor_ECG->Sleep();
        	ESP_LOGI("SENSOR TASK", "Ready to sleep"); // @suppress("Symbol is not resolved")
        	xEventGroupClearBits(GlobalEventGroupHandle, StandbySensorTaskUnhandled);
        	while(1) {
        		vTaskDelay(1000 / portTICK_PERIOD_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
        	}
        }
    }
}

void print_struct(SampleData SensorData) {
	ESP_LOGI("SENSOR TASK", "Value: \t %llu, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d, \t %d \t", // @suppress("Symbol is not resolved")
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
   			SensorData.pressure,
			SensorData.ECGSampleValue);
}


void set_sensor_measurement_bit( TimerHandle_t xTimer )  {
    xEventGroupSetBits(GlobalEventGroupHandle, SensorMeasurementFlag);
}

void SensorController::main_task() {
	ESP_LOGI("SENSOR TASK", "Task starting..."); // @suppress("Symbol is not resolved")

	TimerHandle_t sample_poll_timer = NULL;
	sample_poll_timer = xTimerCreate("sensor_poll_clock",
			1,
			pdTRUE,
			SENSORTASK_TIMER_ID,
			set_sensor_measurement_bit);
    xTimerStart(sample_poll_timer, 0 );

    if(sample_poll_timer == NULL) {
    	// Something has failed creating the timer
    	ESP_LOGI("SENSOR TASK", "Timer creation failed"); // @suppress("Symbol is not resolved")
    } else {
    	ESP_LOGI("SENSOR TASK", "Timer created"); // @suppress("Symbol is not resolved")
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
    	ESP_LOGI("SENSOR TASK", "Handle creation failed"); // @suppress("Symbol is not resolved")
    }

    if(xReturned != pdPASS) {
    	// xReturned false (something went wrong!)
    	ESP_LOGI("SENSOR TASK", "Task creation failed"); // @suppress("Symbol is not resolved")
    }

    ESP_LOGI("SENSOR TASK", "Task is running"); // @suppress("Symbol is not resolved")
}
