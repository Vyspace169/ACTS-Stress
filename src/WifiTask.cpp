#include "WifiTask.hpp"

WifiTask::WifiTask(unsigned int task_priority, DataProcessor &dp) : BaseTask(task_priority), DPHandle{dp}  { main_task(); }

/*static EventGroupHandle_t wifi_event_group_1;

esp_err_t local_wifi_Event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    	case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            ESP_LOGI("WIFI EVENT", "Starting wifi...");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group_1, WIFI_EVENT_BIT);
            ESP_LOGI("WIFI EVENT", "Got IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(wifi_event_group_1, WIFI_EVENT_BIT);
            esp_wifi_connect();
            ESP_LOGI("WIFI EVENT", "WiFi disconnected, restarting wifi...");
            break;
        default:
        	ESP_LOGI("WIFI EVENT", "Different event_id: 0x%08x", event->event_id);
            break;
    }

    return ESP_OK;
}

class TestClass{
public:
	TestClass() {
		connected = false;
		SocketHandle = -1;
		memset(&tcpServerAddr, 0, sizeof(sockaddr_in));
	}
	bool Connect(char* SSID, char* PASS, int timeout) {
		tcpip_adapter_init();
		wifi_event_group_1 = xEventGroupCreate();
		esp_event_loop_init(local_wifi_Event_handler, NULL);
		wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
		esp_wifi_init(&cfg);
		esp_wifi_set_storage(WIFI_STORAGE_FLASH);
		wifi_config_t wifi_config;
		strcpy((char*)wifi_config.sta.ssid, SSID);
		strcpy((char*)wifi_config.sta.password, PASS);
		esp_wifi_set_mode(WIFI_MODE_STA);
		esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
		esp_wifi_start();
		xEventGroupWaitBits(wifi_event_group_1, WIFI_EVENT_BIT, false, true, timeout / portTICK_PERIOD_MS);

		if( xEventGroupGetBits(wifi_event_group_1) & WIFI_EVENT_BIT) {
			connected = true;
			return true;
		} else {
			vEventGroupDelete(wifi_event_group_1);
			esp_wifi_stop();
			esp_wifi_set_mode(WIFI_MODE_NULL);
			esp_wifi_deinit();
			connected = false;
			return false;
		}
	}
	void SetPowerSave() {
		if(connected == false) {
			return;
		}
		esp_wifi_set_ps(WIFI_PS_MODEM);
	}
	void Disconnect() {
		vEventGroupDelete(wifi_event_group_1);
		esp_wifi_stop();
		esp_wifi_set_mode(WIFI_MODE_NULL);
		esp_wifi_deinit();
		connected = false;
	}
	time_t GetTime(int retries) {
		if(connected == false) {
			return 0;
		}
		sntp_setoperatingmode(SNTP_OPMODE_POLL);
		sntp_setservername(0, "pool.ntp.org");
		sntp_init();
		time_t now = 0;
		int retry = 0;
		while(now < 948000000 && ++retry < retries) {
		    vTaskDelay(100 / portTICK_PERIOD_MS);
		    time(&now);
		}
		sntp_stop();
		if(now < 948000000) {
			return 0;
		}
		return now;
	}
	bool GetConnectionStatus() {
		return connected;
	}
	bool TCPConnectToServer(char *IPAddress, int port) {
		//if(connected == false) {
		//	return false;
		//}
	    tcpServerAddr.sin_addr.s_addr = inet_addr(IPAddress);
	    tcpServerAddr.sin_family = AF_INET;
	    tcpServerAddr.sin_port = htons(port);
	    SocketHandle = socket(AF_INET, SOCK_STREAM, 0);

	    if(SocketHandle < 0) {
	       	ESP_LOGI("TCP CLIENT", "Failed to allocate socket");
	       	return false;
	    }
	    if(connect(SocketHandle, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
	    	ESP_LOGI("TCP CLIENT", "Socket connect failed errno = %d", errno);
	    	close(SocketHandle);
	    	SocketHandle = -1;
	    	return false;
	    }
	    return true;
	}
	bool TCPSend(char *Data, int size) {
		//if(connected == false) {
		//	return false;
		//}
		if(SocketHandle < 0) {
			return false;
		}
		if(write(SocketHandle , Data , size) < 0)
		{
			ESP_LOGI("TCP CLIENT", "Send failed");
			close(SocketHandle);
			return false;
	    }
		return true;
	}
	void TCPDisconnect() {
		close(SocketHandle);
		SocketHandle = -1;
		memset(&tcpServerAddr, 0, sizeof(sockaddr_in));
	}
	~TestClass() {}
private:
	bool connected;
	int SocketHandle;
	struct sockaddr_in tcpServerAddr;
};*/

void run_wifi_task(void *args)  {
	WifiTask *sTask = static_cast<WifiTask*>(args);

	ESP_LOGI("WIFI TASK", "Initializing wifi");
	WiFiInitialize(WIFI_SSID, WIFI_PASSWORD);

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
