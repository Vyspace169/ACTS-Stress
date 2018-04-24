#include "SdWriterController.hpp"

SdWriterController::SdWriterController(unsigned int TaskPriority, DoubleBuffer &db, SDWriter &sdw, DataProcessor &dp) :
	BaseTask(TaskPriority),
	DBHandle(db),
	SDWHandle(sdw),
	DPHandle(dp)
	{
		main_task();
	}

void run_sd_task(void *args) {
	SdWriterController *sTask = static_cast<SdWriterController*>(args);

	EventBits_t uxBits;

    while(1)  {
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (SensorBufferSdReady | StandbyWriterTaskUnhandled | RRBufferReadyFlag | HRVBufferReadyFlag), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorBufferSdReady){
        	ESP_LOGI("WRITER TASK", "Writing sensor data"); // @suppress("Symbol is not resolved")
        	if(sTask->SDWHandle.Open() == SD_WRITER_OK) {
        		sTask->DBHandle.writeToSd();
        		sTask->SDWHandle.Close();
        	}
        }

        if(uxBits & StandbyWriterTaskUnhandled) {
        	ESP_LOGI("WRITER TASK", "Ready to sleep"); // @suppress("Symbol is not resolved")
			xEventGroupClearBits(GlobalEventGroupHandle, StandbyWriterTaskUnhandled);
			while(1) {
				vTaskDelay(1000 / portTICK_PERIOD_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
			}
        }

        if(uxBits & RRBufferReadyFlag){
        	ESP_LOGI("WRITER TASK", "Writing RR data"); // @suppress("Symbol is not resolved")
        	sTask->DPHandle.period();
        	if(sTask->SDWHandle.Open() == SD_WRITER_OK) {
        		sTask->DBHandle.writeRRToSd();
        		sTask->SDWHandle.Close();
        	}
        	//xEventGroupSetBits(GlobalEventGroupHandle, RRBufferSDReadyFlag);
        }

        if(uxBits & HRVBufferReadyFlag){
        	ESP_LOGI("WRITER TASK", "Writing HRV data"); // @suppress("Symbol is not resolved")
        	if(sTask->SDWHandle.Open() == SD_WRITER_OK) {
        		sTask->DBHandle.writeHRVToSd();
        		sTask->SDWHandle.Close();
        	}
        }


    }
}

void set_HRV_calculation_bit( TimerHandle_t xTimer )  {
    xEventGroupSetBits(GlobalEventGroupHandle, HRVCalculationFlag);
}

void SdWriterController::main_task() {
	  TaskHandle_t xHandle = NULL;
	  void* thisTask = static_cast<void*>(this);

	  BaseType_t xReturned = xTaskCreatePinnedToCore(run_sd_task,
			  "run_sd_task",
			  WRITERTASK_STACK_SIZE,
			  thisTask,
			  task_priority,
			  &xHandle,
			  WRITERTASK_CORE_NUM);


	  if(xHandle == NULL) {
		  // Handle assignment has failed
		  ESP_LOGI("WRITER TASK", "Handle creation failed"); // @suppress("Symbol is not resolved")
	  }

	  if(xReturned != pdPASS) {
		  // xReturned false (something went wrong!)
		  ESP_LOGI("WRITER TASK", "Task creation failed"); // @suppress("Symbol is not resolved")
	  }

	  ESP_LOGI("WRITER TASK", "Task is running"); // @suppress("Symbol is not resolved")

	  /*
		TimerHandle_t HRVTimer_1 = NULL;
		HRVTimer_1 = xTimerCreate("HRVClock_1",
				OneMinute,
				pdTRUE,
				HRV_1_TIMER_ID,
				set_HRV_calculation_bit);

		if(HRVTimer_1 == NULL) {
			// Something has failed creating the timer
			ESP_LOGI("SENSOR TASK", "HRV 1 Timer creation failed"); // @suppress("Symbol is not resolved")
		} else {
			ESP_LOGI("SENSOR TASK", "HRV 1 Timer created"); // @suppress("Symbol is not resolved")
		}
		*/

}
