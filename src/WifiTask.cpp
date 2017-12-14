#include "WifiTask.hpp"

WifiTask::WifiTask(unsigned int task_priority, DataProcessor &dp) : BaseTask(task_priority), DPHandle{dp}  { main_task(); }


void run_wifi_task(void *args)  {
	WifiTask *sTask = static_cast<WifiTask*>(args);

	char* ssid = "Allyouare";
	char* pass = "Meulen-2017";
	WifiModule *testmodule = new WifiModule;
	testmodule->ClientConfig(ssid, pass);

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag | StandbyWifiTaskUnhandled), pdTRUE, pdFALSE, portMAX_DELAY);

        if((uxBits & WifiActivateFlag) && testmodule->ClientGetConnectionState() == false) {
			ESP_LOGI("WIFI TASK", "Connecting to wifi");
			bool enabled = testmodule->ClientConnect(WIFI_CONNECT_TIMEOUT);
			if(enabled == false) {
				ESP_LOGI("WIFI TASK", "Wifi not connected");
			}
			else {
				ESP_LOGI("WIFI TASK", "Wifi connected");

				// enable power save mode
				testmodule->ClientSetPowerSave();

				// Sending data example
				ESP_LOGI("WIFI TASK", "Activety: %f", sTask->DPHandle.GetActivityData());
				char ActivityDataString[25];
				sprintf(ActivityDataString, "%.2f\n" ,sTask->DPHandle.GetActivityData());
				testmodule->TCPConnectToServer("192.168.178.38", 3010);
				testmodule->TCPSend(ActivityDataString, strlen(ActivityDataString));
				testmodule->TCPDisconnect();
			}
        }

        if(uxBits & WifiReadyFlag) {
        	// TODO: WifiReadyFlag must be set by something
            // TODO: Send data over wifi to the server
        	// TODO: MQTT frame builder class
        }

        if(uxBits & StandbyWifiTaskUnhandled) {
        	if(testmodule->ClientGetConnectionState() == true) {
        		testmodule->ClientDisconnect();
        	}
        	testmodule->ClientDeinit();
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
