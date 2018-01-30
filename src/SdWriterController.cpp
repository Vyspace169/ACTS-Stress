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
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (SensorBufferSdReady | StandbyWriterTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorBufferSdReady){
        	ESP_LOGI("WRITER TASK", "Writing data");
        	if(sTask->SDWHandle.Open() == SD_WRITER_OK) {
        		sTask->DBHandle.writeToSd();
        		sTask->SDWHandle.Close();
        	}
        }

        if(uxBits & StandbyWriterTaskUnhandled) {
        	ESP_LOGI("WRITER TASK", "Ready to sleep");
			xEventGroupClearBits(GlobalEventGroupHandle, StandbyWriterTaskUnhandled);
			while(1) {
				vTaskDelay(1000 / portTICK_PERIOD_MS);
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
		  ESP_LOGI("WRITER TASK", "Handle creation failed");
	  }

	  if(xReturned != pdPASS) {
		  // xReturned false (something went wrong!)
		  ESP_LOGI("WRITER TASK", "Task creation failed");
	  }

	  ESP_LOGI("WRITER TASK", "Task is running");
}
