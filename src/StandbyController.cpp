#include "StandbyController.hpp"

StandbyController::StandbyController(unsigned int task_priority) : BaseTask(task_priority) { main_task(); }


void standbycontroller_handle_task(void *args)  {
	bool BitsSet = false;
	int gpio_state = 0;
	int old_gpio_state = 0;
	bool reset_cause_charge = false;
	EventBits_t uxBits;

	gpio_pad_select_gpio(GPIO_CHARGE_DETECT);
	gpio_set_direction(GPIO_CHARGE_DETECT, GPIO_MODE_INPUT);

	while(true) {
		uxBits = xEventGroupGetBits(GlobalEventGroupHandle);

		gpio_state = gpio_get_level(GPIO_CHARGE_DETECT);
		if(gpio_state == 0 && old_gpio_state == 1) {
			ESP_LOGI("SLEEP TASK", "Setting bits due to charge released");
			//xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
			xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWriterTaskUnhandled));
			BitsSet = true;
		}
		else {
			old_gpio_state = gpio_state;
		}

		if(BitsSet == true) {
			//if((uxBits & (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
			if((uxBits & (StandbySensorTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
				ESP_LOGI("SLEEP TASK", "Going to sleep");
				if(reset_cause_charge == true) {
					esp_sleep_enable_timer_wakeup(100000);
					esp_deep_sleep_start();
				}
				else {
					esp_sleep_enable_timer_wakeup(SLEEP_TIME_SEC * 1000000);
					esp_deep_sleep_start();
				}
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

		vTaskDelay(STANDBYCONT_LOOP_DELAY / portTICK_PERIOD_MS);
	}
}


void StandbyController::main_task() {
	ESP_LOGI("STANDBY CONTROLLER", "Task starting...");

	TaskHandle_t xHandle = NULL;
    BaseType_t xReturned = xTaskCreatePinnedToCore(standbycontroller_handle_task,
    		"standby_controller_task",
			STANDBYCONT_STACK_SIZE,
			NULL,
			task_priority,
			&xHandle,
			STANDBYCONT_CORE_NUM);

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
