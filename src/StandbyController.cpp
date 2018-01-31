#include "StandbyController.hpp"

StandbyController::StandbyController(unsigned int task_priority) :
	BaseTask(task_priority)
	{
		main_task();
	}

void standbycontroller_handle_task(void *args)  {
	EventBits_t uxBits;
	bool EventBitsSet = false;

	bool InfinityReset = false;
	bool PinInterruptReset = false;
	bool FastReset = false;

	int OldChargeDetectGPIOState = gpio_get_level(GPIO_CHARGE_DETECT);
	int ChargeDetectGPIOState = gpio_get_level(GPIO_CHARGE_DETECT);

	while(true) {
		vTaskDelay(STANDBYCONT_LOOP_DELAY / portTICK_PERIOD_MS);

		uxBits = xEventGroupGetBits(GlobalEventGroupHandle);

		// bits have been set and task is waiting for bits to reset
		if(EventBitsSet == true) {
			if((uxBits & (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled)) == 0) {
				ESP_LOGI("SLEEP TASK", "Going to sleep");
				gettimeofday(&SleepEnterTime, NULL);
				if(FastReset == true) {
					esp_sleep_enable_timer_wakeup(100000);
				}
				else if(InfinityReset == true) {
					esp_sleep_enable_timer_wakeup(0xFFFFFFFFFFFFFFFF);
				}
				else if(PinInterruptReset == true) {
					vTaskDelay(1000 / portTICK_PERIOD_MS);
					esp_sleep_enable_ext1_wakeup((1<<GPIO_SD_DETECT), ESP_EXT1_WAKEUP_ANY_HIGH);
				}
				else {
					esp_sleep_enable_ext0_wakeup(GPIO_MPU_INT, 1);
				}
				esp_deep_sleep_start();
			}
		}
		else {

			// Battery voltage measurement
			float BatteryVoltage = adc1_get_raw(ADC_BATTERY) / ADC_TO_BAT_VOLTAGE;
			if(gpio_get_level(GPIO_CHARGE_DETECT) == 0 && BatteryVoltage > 4.15) {
				blink_set_led(GPIO_LED_GREEN, 10000, 0);
			}
			else if(gpio_get_level(GPIO_CHARGE_DETECT) == 0 && BatteryVoltage <= 4.15) {
				blink_set_led(GPIO_LED_GREEN, 10, 1000);
			}
			else {
				if(BatteryVoltage < BAT_LED_VALUE_VOLTAGE) {
					blink_set_led(GPIO_LED_RED, 10, 1000);
				}
				else if(BatteryVoltage >= BAT_LED_VALUE_VOLTAGE) {
					blink_set_led(GPIO_LED_GREEN, 10, 5000);
				}
			}
			/*if(BatteryVoltage < TURN_OFF_VOLTAGE) {
				ESP_LOGI("SLEEP TASK", "Setting bits due to a low battery");
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				InfinityReset = true;
				EventBitsSet = true;
			}*/

			// If the card is removed, this loop will be run
			if(gpio_get_level(GPIO_SD_DETECT) == 0) {
				vTaskDelay(SD_DET_DEBOUNCE_MS / portTICK_PERIOD_MS);
				if(gpio_get_level(GPIO_SD_DETECT) == 0) {
					ESP_LOGI("SLEEP TASK", "Setting bits due to SD-card remove");
					xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
					PinInterruptReset = true;
					EventBitsSet = true;
				}
			}

			// charge detect rising edge detector
			ChargeDetectGPIOState = gpio_get_level(GPIO_CHARGE_DETECT);
			if(ChargeDetectGPIOState == 1 && OldChargeDetectGPIOState == 0) {
				ESP_LOGI("SLEEP TASK", "Setting bits due to charge released");
				xEventGroupSetBits(GlobalEventGroupHandle, (StandbySensorTaskUnhandled | StandbyWifiTaskUnhandled | StandbyWriterTaskUnhandled));
				FastReset = true;
				EventBitsSet = true;
			}
			else {
				OldChargeDetectGPIOState = ChargeDetectGPIOState;
			}

			// movement timeout sleep
			if(uxBits & MovementTimeoutReached) {
				ESP_LOGI("SLEEP TASK", "Setting bits due to movement timeout");
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
