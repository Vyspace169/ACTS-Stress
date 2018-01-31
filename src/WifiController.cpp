#include "WifiController.hpp"

WifiController::WifiController(unsigned int task_priority, DataProcessor &dp) :
	BaseTask(task_priority),
	DPHandle{dp}
	{
		mqtt = new MQTTController();
		main_task();
	}

void run_wifi_task(void *args)  {
	WifiController *sTask = static_cast<WifiController*>(args);
	MovementStack MovementSaver;

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag | StandbyWifiTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);
		ESP_LOGI("WIFI TASK", "Connecting to wifi");
        if((uxBits & WifiActivateFlag)) {
			ESP_LOGI("WIFI TASK", "Connecting to wifi");
			bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);
			ESP_LOGI("WIFI TASK", "geil");
			// push activity data in its fifo
			MovementSaver.PushData(sTask->DPHandle.GetActivityData());
			if(enabled == true) {
				ESP_LOGI("WIFI TASK", "Wifi connected");
				sTask->mqtt->connectMQTT();
				while(MovementSaver.DataCount() != 0) {
					sTask->mqtt->publish(MovementSaver.GetActivityData());
					MovementSaver.PopData();
				}
				sTask->mqtt->disconnect();
				ESP_LOGI("WIFI TASK", "Wifi done");

				WiFiDisconnect();
			}
			else {
				ESP_LOGI("WIFI TASK", "Wifi connection failed");
			}
        }

        if(uxBits & WifiReadyFlag) {
			//pass
        }

        if(uxBits & StandbyWifiTaskUnhandled) {
        	ESP_LOGI("WIFI TASK", "Ready to sleep");
        	xEventGroupClearBits(GlobalEventGroupHandle, StandbyWifiTaskUnhandled);
        	while(1) {
        		vTaskDelay(1000 / portTICK_PERIOD_MS);
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
		ESP_LOGI("WIFI TASK", "Timer creation failed");
	} else {
		ESP_LOGI("WIFI TASK", "Timer created");
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
		ESP_LOGI("WIFI TASK", "Handle creation failed");
	}

	if(xReturned != pdPASS) {
		// xReturned false (something went wrong!)
		ESP_LOGI("WIFI TASK", "Task creation failed");
	}

	ESP_LOGI("WIFI TASK", "Task is running");
}
