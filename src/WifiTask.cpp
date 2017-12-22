#include "WifiTask.hpp"

WifiTask::WifiTask(unsigned int task_priority, DataProcessor &dp) : BaseTask(task_priority), DPHandle{dp}  { main_task(); }

void run_wifi_task(void *args)  {
	WifiTask *sTask = static_cast<WifiTask*>(args);

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag | StandbyWifiTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if((uxBits & WifiActivateFlag)) {
			ESP_LOGI("WIFI TASK", "Connecting to wifi");

			bool enabled = WiFiConnect(WIFI_CONNECT_TIMEOUT);

			if(enabled == false) {
				ESP_LOGI("WIFI TASK", "Wifi not connected");
			}
			else {
				ESP_LOGI("WIFI TASK", "Wifi connected");

				char send_string[50];
				sprintf(send_string, "%.2f\n", sTask->DPHandle.GetActivityData());
				sTask->DPHandle.ResetActivityData();

				if(TCPConnectToServer("192.168.8.100", 3010) == true) {
					ESP_LOGI("WIFI TASK", "Socket connected, sending to server: %s", send_string);
					TCPSend(send_string, strlen(send_string));
					TCPDisconnect();
				}

				WiFiDisconnect();
			}
        }

        if(uxBits & WifiReadyFlag) {
        	// TODO: WifiReadyFlag must be set by something
            // TODO: Send data over wifi to the server
        	// TODO: MQTT frame builder class
        }

#ifdef DATA_THROUGH_TCP
        if(uxBits & WifiReadyFlag) {
        	if(testmodule->ClientGetConnectionState() == true) {
        		char* sendData = sTask->DPHandle.GetDataString();
        		testmodule->TCPSend(sendData, strlen(sendData));
        	}
        }
#endif

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

void WifiTask::main_task() {
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
