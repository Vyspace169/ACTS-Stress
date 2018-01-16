#include "StandbyController.hpp"

StandbyController::StandbyController(unsigned int task_priority) : BaseTask(task_priority) { main_task(); }


void standbycontroller_handle_task(void *args)  {
	bool EventBitsSet = false;
	EventBits_t uxBits;

	bool InfinityReset = false;
	bool FastReset = false;

	gpio_pad_select_gpio(GPIO_CHARGE_DETECT);
	gpio_set_direction(GPIO_CHARGE_DETECT, GPIO_MODE_INPUT);
	int old_charge_detect_gpio_state = 0;
	int charge_detect_gpio_state = 0;

	gpio_pad_select_gpio(GPIO_SD_DETECT);
	gpio_set_direction(GPIO_SD_DETECT, GPIO_MODE_INPUT);
	int old_card_detect_gpio_state = 0;
	int card_detect_gpio_state = 0;

	while(true) {
		vTaskDelay(STANDBYCONT_LOOP_DELAY / portTICK_PERIOD_MS);

		uxBits = xEventGroupGetBits(GlobalEventGroupHandle);

		// bits have been set and task is waiting for bits to reset
		if(EventBitsSet == true) {
			if((uxBits & (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
				ESP_LOGI("SLEEP TASK", "Going to sleep");
				if(FastReset == true) {
					esp_sleep_enable_timer_wakeup(100000);
				}
				else if(InfinityReset == true) {
					esp_sleep_enable_ext1_wakeup((1<<GPIO_SD_DETECT), ESP_EXT1_WAKEUP_ANY_HIGH);
				}
				else {
					esp_sleep_enable_timer_wakeup(SLEEP_TIME_SEC * 1000000);
				}
				esp_deep_sleep_start();
			}
		}
		else {
			// card detect falling edge detector
			card_detect_gpio_state = gpio_get_level(GPIO_SD_DETECT);
			if(card_detect_gpio_state == 0 && old_card_detect_gpio_state == 1) {
				ESP_LOGI("SLEEP TASK", "Setting bits due to SD-card remove");
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				InfinityReset = true;
				EventBitsSet = true;
			}
			else {
				old_card_detect_gpio_state = card_detect_gpio_state;
			}

			// charge detect rising edge detector
			charge_detect_gpio_state = gpio_get_level(GPIO_CHARGE_DETECT);
			if(charge_detect_gpio_state == 0 && old_charge_detect_gpio_state == 1) {
				ESP_LOGI("SLEEP TASK", "Setting bits due to charge released");
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				FastReset = true;
				EventBitsSet = true;
			}
			else {
				old_charge_detect_gpio_state = charge_detect_gpio_state;
			}

			// movement timeout sleep
			if(uxBits & MovementTimeoutReached) {
				ESP_LOGI("SLEEP TASK", "Setting bits");
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				EventBitsSet = true;
			}
		}
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
    }

    if(xReturned != pdPASS) {
    	// xReturned false (something went wrong!)
    	ESP_LOGI("STANDBY CONTROLLER", "Task creation failed");
    }

    ESP_LOGI("STANDBY CONTROLLER", "Task is running");
}
