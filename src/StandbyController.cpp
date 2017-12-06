#include "StandbyController.hpp"

StandbyController::StandbyController(unsigned int task_priority) : BaseTask(task_priority) { main_task(); }


void standbycontroller_handle_task(void *args)  {
	bool BitsSet = false;
	EventBits_t uxBits;

	while(true) {
		uxBits = xEventGroupGetBits(GlobalEventGroupHandle);

		if(BitsSet == true) {
			//if((uxBits & (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
			if((uxBits & (StandbySensorTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
				ESP_LOGI("SLEEP TASK", "Going to sleep");
				esp_sleep_enable_timer_wakeup(TIMEOUT_TIME_SEC * 1000000);
				esp_deep_sleep_start();
			}
		}
		else {
			if(uxBits & MovementTimeoutReached) {
				ESP_LOGI("SLEEP TASK", "Setting bits");
				//xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWriterTaskUnhandled));
				BitsSet = true;
			}
		}

		vTaskDelay(SC_LOOP_DELAY / portTICK_PERIOD_MS);
	}
}


void StandbyController::main_task() {
	ESP_LOGI("STANDBY CONTROLLER", "Task starting...");

	TaskHandle_t xHandle = NULL;
    BaseType_t xReturned = xTaskCreatePinnedToCore(standbycontroller_handle_task, "standby_controller_task", 2048, NULL, 6, &xHandle, 0);

    if(xHandle == NULL) {
    	// Handle assignment has failed
    	ESP_LOGI("STANDBY CONTROLLER", "Handle creation failed");
    } else {
    	ESP_LOGI("STANDBY CONTROLLER", "Handle creation OK");
    }

    if(xReturned != pdPASS) {
    	// xReturned false (something went wrong!)
    	ESP_LOGI("STANDBY CONTROLLER", "Task creation failed");
    } else {
    	ESP_LOGI("STANDBY CONTROLLER", "Task creation OK");
    }

    ESP_LOGI("STANDBY CONTROLLER", "Task is running");
}
