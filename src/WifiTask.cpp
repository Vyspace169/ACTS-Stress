#include "WifiTask.hpp"

WifiTask::WifiTask(unsigned int task_priority) : BaseTask(task_priority)  { main_task(); }

void run_wifi_task(void *args)  {

	char* ssid = "Allyouare";
	char* pass = "Meulen-2017";

    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, (WifiActivateFlag | WifiReadyFlag), pdTRUE, pdFALSE, 0);

        if(uxBits & WifiActivateFlag) {
        	ESP_LOGI("WIFI TASK", "Connecting to wifi");
        	/*testmodule.ClientConfig(ssid, pass);
        	bool enabled = testmodule.ClientConnect(WIFI_CONNECT_TIMEOUT);
        	if(enabled == false){
        		ESP_LOGI("WIFI TASK", "Wifi not connected");
        	} else {
        		ESP_LOGI("WIFI TASK", "Wifi connected");
        	}
        	testmodule.ClientDeinit();*/
        }

        if(uxBits & WifiReadyFlag) {
            // Todo Send data over wifi to the server
        }
    }
}

void wifi_timers_callback( TimerHandle_t xTimer )  {
	ESP_LOGI("WIFI TASK", "tick");
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
	BaseType_t xReturned = xTaskCreatePinnedToCore(run_wifi_task,
			"wifi_task",
			WIFITASK_STACK_SIZE,
			NULL,
			task_priority,
			&xHandle,
			WIFITASK_CORE_NUM);

	if(xHandle == NULL) {
		// Handle assignment has failed
		ESP_LOGI("WIFI TASK", "Handle creation failed");
	} else {
		ESP_LOGI("WIFI TASK", "Handle creation OK");
	}

	if(xReturned != pdPASS) {
		// xReturned false (something went wrong!)
		ESP_LOGI("WIFI TASK", "Task creation failed");
	} else {
		ESP_LOGI("WIFI TASK", "Task creation OK");
	}

	ESP_LOGI("WIFI TASK", "Task is running");
}
