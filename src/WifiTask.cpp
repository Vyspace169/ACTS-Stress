#include "WifiTask.hpp"

WifiTask::WifiTask(unsigned int task_priority, DataProcessor &dp) : BaseTask(task_priority), DPHandle{dp}  { main_task(); }


void run_wifi_task(void *args)  {
	WifiTask *sTask = static_cast<WifiTask*>(args);
	WifiModule *testmodule = new WifiModule;
	testmodule->ClientConfig(WIFI_SSID, WIFI_PASSWORD);

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
				//testmodule->ClientSetPowerSave();

				//testmodule->TCPConnectToServer("192.168.178.38", 3010);
				//testmodule->TCPSend(ActivityDataString, strlen(ActivityDataString));
				//testmodule->TCPDisconnect();

				sntp_setoperatingmode(SNTP_OPMODE_POLL);
				sntp_setservername(0, "pool.ntp.org");
				sntp_init();

				// wait for time to be set
				time_t now = 0;
				struct tm timeinfo;
				int retry = 0;
				const int retry_count = 10;
				while(timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
				    ESP_LOGI("WIFI TASK", "Waiting for system time to be set... (%d/%d)", retry, retry_count);
				    vTaskDelay(1000 / portTICK_PERIOD_MS);
				    time(&now);
				    localtime_r(&now, &timeinfo);
				}

				char strftime_buf[64];
				strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
				ESP_LOGI("WIFI TASK", "Central time: %s", strftime_buf);
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
