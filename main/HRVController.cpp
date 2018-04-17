/*
 * HRVController.cpp
 *
 *  Created on: 15 apr. 2018
 *      Author: caspe
 */

#include "HRVController.hpp"

HRVController::HRVController(unsigned int TaskPriority, DoubleBuffer &db, SDWriter &sdw, DataProcessor &dp) :
	BaseTask(TaskPriority),
	DBHandle(db),
	SDWHandle{sdw},
	DPHandle(dp)
	{
		main_task();
	}

void run_hrv_task(void *args) {
	HRVController *sTask = static_cast<HRVController*>(args);

	EventBits_t uxBits;

    while(1)  {
    	uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (StandbyWriterTaskUnhandled | RRBufferSDReadyFlag), pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & RRBufferSDReadyFlag){
        	ESP_LOGI("HRV TASK", "Calculating HRV RR data"); // @suppress("Symbol is not resolved")
        	sTask->DPHandle.fasper();
        }
    }
}
void HRVController::main_task() {
	  TaskHandle_t xHandle = NULL;
	  void* thisTask = static_cast<void*>(this);

	  BaseType_t xReturned = xTaskCreatePinnedToCore(run_hrv_task,
			  "run_hrv_task",
			  HRVTASK_STACK_SIZE,
			  thisTask,
			  task_priority,
			  &xHandle,
			  HRVTASK_CORE_NUM);


	  if(xHandle == NULL) {
		  // Handle assignment has failed
		  ESP_LOGI("HRV TASK", "Handle creation failed"); // @suppress("Symbol is not resolved")
	  }

	  if(xReturned != pdPASS) {
		  // xReturned false (something went wrong!)
		  ESP_LOGI("HRV TASK", "Task creation failed"); // @suppress("Symbol is not resolved")
	  }

	  ESP_LOGI("HRV TASK", "Task is running"); // @suppress("Symbol is not resolved")
}
