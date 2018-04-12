#include "WifiController.hpp"

WifiController::WifiController(unsigned int task_priority, DataProcessor &dp) : // @suppress("Member declaration not found") // @suppress("Type cannot be resolved")
	BaseTask(task_priority),
	DPHandle{dp} // @suppress("Symbol is not resolved")
	{
		mqtt = new MQTTController();
		main_task();
	}

void run_wifi_task(void *args)  {
	WifiController *sTask = static_cast<WifiController*>(args);
	MovementStack MovementSaver; // @suppress("Type cannot be resolved")

	int size = MovementSaver.DataCount(); // @suppress("Method cannot be resolved")
	while(size != 0) {
		double test2 = MovementSaver.GetActivityData(); // @suppress("Method cannot be resolved")
	    ESP_LOGI("WIFI TASK", "%f, %d", test2, size); // @suppress("Symbol is not resolved")
	    size--;
	}

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag | StandbyWifiTaskUnhandled | RBufferReadyFlag), pdTRUE, pdFALSE, portMAX_DELAY);

        if((uxBits & WifiActivateFlag)) {
			ESP_LOGI("WIFI TASK", "Connecting to wifi"); // @suppress("Symbol is not resolved")
			bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);

			// push activity data in its fifo
			MovementSaver.PushData(sTask->DPHandle.GetActivityData()); // @suppress("Method cannot be resolved") // @suppress("Invalid arguments")
			if(enabled == true) {

				ESP_LOGI("WIFI TASK", "Wifi connected"); // @suppress("Symbol is not resolved")

				/*sTask->mqtt->connectMQTT();
				while(MovementSaver.DataCount() != 0) { // @suppress("Method cannot be resolved")
					sTask->mqtt->publish(MovementSaver.GetActivityData()); // @suppress("Invalid arguments") // @suppress("Method cannot be resolved")
				}
				sTask->mqtt->disconnect();*/
				MovementSaver.GetActivityData();

				ESP_LOGI("WIFI TASK", "Wifi done"); // @suppress("Symbol is not resolved")

				WiFiDisconnect();
			}
			else {
				ESP_LOGI("WIFI TASK", "Wifi connection failed"); // @suppress("Symbol is not resolved")
			}
        }

        if(uxBits & RBufferReadyFlag) {
			sTask->DPHandle.CalculateRRInterval();
        }

        if(uxBits & RRBufferReadyFlag) {
			sTask->DPHandle.fasper();
        }

        if(uxBits & HRVBufferReadyFlag) {
			sTask->DPHandle.CalculateHRV();
        }



        if(uxBits & WifiReadyFlag) {
			//pass
        }

        if(uxBits & StandbyWifiTaskUnhandled) {
        	ESP_LOGI("WIFI TASK", "Ready to sleep"); // @suppress("Symbol is not resolved")
        	MovementSaver.WriteStackToFlash(); // @suppress("Method cannot be resolved")
        	xEventGroupClearBits(GlobalEventGroupHandle, StandbyWifiTaskUnhandled);
        	while(1) {
        		vTaskDelay(1000 / portTICK_PERIOD_MS); // @suppress("Invalid arguments") // @suppress("Symbol is not resolved")
        	}
        }
    }
}

void wifi_timers_callback( TimerHandle_t xTimer )  {
	xEventGroupSetBits(GlobalEventGroupHandle, WifiActivateFlag);
}

void WifiController::main_task() {
	TimerHandle_t wifi_poll_timer = NULL;
	wifi_poll_timer = xTimerCreate("wifi_poll_clock",
			WIFI_POLL_FREQUENCY_SEC * 1000,
			pdTRUE,
			WIFITASK_TIMER_ID,
			wifi_timers_callback);
	xTimerStart( wifi_poll_timer, 0 );

	if(wifi_poll_timer == NULL) {
		// Something has failed creating the timer
		ESP_LOGI("WIFI TASK", "Timer creation failed"); // @suppress("Symbol is not resolved")
	} else {
		ESP_LOGI("WIFI TASK", "Timer created"); // @suppress("Symbol is not resolved")
	}

	TaskHandle_t xHandle = NULL;
	void* thisTask = static_cast<void*>(this);
	BaseType_t xReturned = xTaskCreatePinnedToCore(run_wifi_task,
			"wifi_task",
			WIFITASK_STACK_SIZE,
			thisTask,
			task_priority,
			&xHandle,
			WIFITASK_CORE_NUM);

	if(xHandle == NULL) {
		// Handle assignment has failed
		ESP_LOGI("WIFI TASK", "Handle creation failed"); // @suppress("Symbol is not resolved")
	}

	if(xReturned != pdPASS) {
		// xReturned false (something went wrong!)
		ESP_LOGI("WIFI TASK", "Task creation failed"); // @suppress("Symbol is not resolved")
	}

	ESP_LOGI("WIFI TASK", "Task is running"); // @suppress("Symbol is not resolved")
}
