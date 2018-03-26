#include "SdWriterController.hpp"

SdWriterController::SdWriterController(unsigned int TaskPriority, DoubleBuffer &db, SDWriter &sdw) :
	BaseTask(TaskPriority),
	DBHandle(db),
	SDWHandle(sdw)
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
        	if(sTask->SDWHandle.Open() == SD_WRITER_OK) {
        		sTask->DBHandle.writeRRToSd();
        		sTask->SDWHandle.Close();
        	}
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
}
