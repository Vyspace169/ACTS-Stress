#include "SdWriterTask.hpp"

SdWriterTask::SdWriterTask(unsigned int task_priority, DoubleBuffer &db, SDWriter &sdw) : BaseTask(task_priority), DBHandle(db), SDWHandle(sdw)  {main_task();}

void run_sd_task(void *args) {
	SdWriterTask *sTask = static_cast<SdWriterTask*>(args);

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (SensorBufferSdReady | StandbyWriterTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorBufferSdReady){
        	ESP_LOGI("WRITER TASK", "Writing data");
        	if(sTask->SDWHandle.Open() != SD_WRITER_OK) {
        		ESP_LOGI("WRITER TASK", "Opening file gave an error");
        	}

        	sTask->DBHandle.writeToSd();

        	sTask->SDWHandle.Close();
        }

        if(uxBits & StandbyWriterTaskUnhandled) {
			xEventGroupClearBits(GlobalEventGroupHandle, StandbyWriterTaskUnhandled);
			while(1) {
				vTaskDelay(1000 / portTICK_PERIOD_MS);
			}
        }
    }
}

void SdWriterTask::main_task() {
  TaskHandle_t xHandle = NULL;
  void* thisTask = static_cast<void*>(this);
//<<<<<<< HEAD
  BaseType_t xReturned = xTaskCreatePinnedToCore(run_sd_task,
		  "run_sd_task",
		  WRITERTASK_STACK_SIZE,
		  thisTask,
		  task_priority,
		  &xHandle,
		  WRITERTASK_CORE_NUM);
/*=======
  BaseType_t xReturned = xTaskCreatePinnedToCore(run_sd_task, "run_sd_task", 2048, thisTask, 1, &xHandle, 0);
>>>>>>> combining_sensors_card_refactor*/

  if(xHandle == NULL) {
   	// Handle assignment has failed
   	ESP_LOGI("WRITER TASK", "Handle creation failed");
  } else {
   	ESP_LOGI("WRITER TASK", "Handle creation OK");
  }

  if(xReturned != pdPASS) {
  	// xReturned false (something went wrong!)
   	ESP_LOGI("WRITER TASK", "Task creation failed");
  } else {
   	ESP_LOGI("WRITER TASK", "Task creation OK");
  }

  ESP_LOGI("WRITER TASK", "Task is running");
}
