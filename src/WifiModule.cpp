#include "WifiModule.hpp"

// hanteert het schrijven en lezen van data, onderhoudt ook de connectie tussen esp32 en pc
static EventGroupHandle_t wifi_event_group;
const static unsigned int WIFI_EVENT_BIT = BIT0;
const char *WIFI_CLASS_TAG = "WIFI MODULE";

esp_err_t local_wifi_Event_handler(void *ctx, system_event_t *event)
{
    switch(event->event_id) {
    	case SYSTEM_EVENT_STA_START:
            esp_wifi_connect();
            ESP_LOGI(WIFI_CLASS_TAG, "Starting wifi...");
            break;
        case SYSTEM_EVENT_STA_GOT_IP:
            xEventGroupSetBits(wifi_event_group, WIFI_EVENT_BIT);
            ESP_LOGI(WIFI_CLASS_TAG, "Got IP: %s\n", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip));
            break;
        case SYSTEM_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(wifi_event_group, WIFI_EVENT_BIT);
            esp_wifi_connect();
            ESP_LOGI(WIFI_CLASS_TAG, "WiFi disconnected, restarting wifi...");
            break;
        default:
        	ESP_LOGI(WIFI_CLASS_TAG, "Different event_id: 0x%08x", event->event_id);
            break;
    }
    return ESP_OK;
}

WifiModule::WifiModule() {
	WifiIPSet = false;
	WifiIsInitialized = false;
	IPAddress = 0;
	Gateway = 0;
	Netmask = 0;
	SocketHandle = -1;
	memset(&tcpServerAddr, 0, sizeof(sockaddr_in));
}

bool WifiModule::ClientSetStaticIP(char *ip, char *gateway, char *netmask) {
	IPAddress = ipaddr_addr(ip);
	Gateway = ipaddr_addr(gateway);
	Netmask = ipaddr_addr(netmask);

	WifiIPSet = true;

	return true;
}

bool WifiModule::ClientConfig(char* SSID, char* PASS) {

	tcpip_adapter_init();

	if(WifiIPSet == true) {
		if(tcpip_adapter_dhcpc_stop(TCPIP_ADAPTER_IF_STA) != ESP_OK) {
			return false;
		}

		tcpip_adapter_ip_info_t ipInfo;
		ipInfo.ip.addr = IPAddress;
		ipInfo.gw.addr = Gateway;
		ipInfo.netmask.addr = Netmask;

		if(tcpip_adapter_set_ip_info(TCPIP_ADAPTER_IF_STA, &ipInfo) != ESP_OK) {
			return false;
		}
	}

	wifi_event_group = xEventGroupCreate();
	if(esp_event_loop_init(local_wifi_Event_handler, NULL) != ESP_OK) {
		return false;
	}

	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
	if(esp_wifi_init(&cfg) != ESP_OK) {
		return false;
	}

	if(esp_wifi_set_storage(WIFI_STORAGE_RAM) != ESP_OK) {
		return false;
	}

	wifi_config_t wifi_config;
	strcpy((char*)wifi_config.sta.ssid, SSID);
	strcpy((char*)wifi_config.sta.password, PASS);

	if(esp_wifi_set_mode(WIFI_MODE_STA) != ESP_OK) {
		return false;
	}

	if(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config) != ESP_OK) {
		return false;
	}

	ESP_LOGI(WIFI_CLASS_TAG, "Setting WiFi configuration SSID %s...", wifi_config.sta.ssid);

	WifiIsInitialized = true;
	return true;
}

bool WifiModule::ClientConnect(int timeout) {
	if(WifiIsInitialized == false) {
		return false;
	}

	unsigned int TickCountStart = xTaskGetTickCount();

    if( esp_wifi_start() != ESP_OK) {
        return false;
    }

    xEventGroupWaitBits(wifi_event_group, WIFI_EVENT_BIT, false, true, timeout / portTICK_PERIOD_MS);

    bool WifiIsConnected = (bool)xEventGroupGetBits(wifi_event_group) & WIFI_EVENT_BIT;

    if (!WifiIsConnected) {
        esp_wifi_stop();
    }
    else {
    	ESP_LOGI(WIFI_CLASS_TAG, "Wifi is connected");
    	unsigned int TickCountStop = xTaskGetTickCount();
    	TickCountStop -= TickCountStart;
    	ESP_LOGI(WIFI_CLASS_TAG, "Connection took %d ticks", TickCountStop);
    }

    return WifiIsConnected;
}

bool WifiModule::ClientSetPowerSave() {
	if(ClientGetConnectionState() == false) {
		return false;
	}

	if(esp_wifi_set_ps(WIFI_PS_MODEM) != ESP_OK) {
		return false;
	}

	return true;
}

bool WifiModule::ClientDisconnect() {
    if( esp_wifi_stop() != ESP_OK) {
        return false;
    }

    return true;
}

bool WifiModule::ClientGetConnectionState() {
	if( xEventGroupGetBits(wifi_event_group) & WIFI_EVENT_BIT) {
		return true;
	}

	return false;
}

void WifiModule::ClientDeinit() {
	esp_wifi_set_mode(WIFI_MODE_NULL);
	esp_wifi_stop();
	esp_wifi_deinit();
	vEventGroupDelete(wifi_event_group);
}

bool WifiModule::TCPConnectToServer(char *IPAddress, int port) {
	if(ClientGetConnectionState() == false) {
		return false;
	}

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

bool WifiModule::TCPSend(char *Data, int size) {
	if(ClientGetConnectionState() == false) {
		return false;
	}

	if(SocketHandle < 0) {
		return false;
	}

	if( write(SocketHandle , Data , size) < 0)
	{
		ESP_LOGI("TCP CLIENT", "Send failed");
		close(SocketHandle);
		return false;
    }

	return true;
}

void WifiModule::TCPDisconnect() {
	close(SocketHandle);
	SocketHandle = -1;
	memset(&tcpServerAddr, 0, sizeof(sockaddr_in));
}

WifiModule::~WifiModule(){

}

/*
void tcp_client(void *pvParam){
    ESP_LOGI("TCP CLIENT","tcp_client task started \n");
    struct sockaddr_in tcpServerAddr;
    tcpServerAddr.sin_addr.s_addr = inet_addr("192.168.43.28");
    tcpServerAddr.sin_family = AF_INET;
    tcpServerAddr.sin_port = htons(3010);
    int s, r;
    char recv_buf[64];

    while(1) {
        s = socket(AF_INET, SOCK_STREAM, 0);
        if(s < 0) {
        	ESP_LOGI("TCP CLIENT", "... Failed to allocate socket.\n");
            vTaskDelay(1000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI("TCP CLIENT", "... allocated socket\n");
         if(connect(s, (struct sockaddr *)&tcpServerAddr, sizeof(tcpServerAddr)) != 0) {
        	 ESP_LOGI("TCP CLIENT", "... socket connect failed errno=%d \n", errno);
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI("TCP CLIENT", "... connected \n");
        if( write(s , "TESTING" , 7) < 0)
        {
        	ESP_LOGI("TCP CLIENT", "... Send failed \n");
            close(s);
            vTaskDelay(4000 / portTICK_PERIOD_MS);
            continue;
        }
        ESP_LOGI("TCP CLIENT", "... socket send success");
        do {
            bzero(recv_buf, sizeof(recv_buf));
            r = read(s, recv_buf, sizeof(recv_buf)-1);
            for(int i = 0; i < r; i++) {
                putchar(recv_buf[i]);
            }
        } while(r > 0);
        ESP_LOGI("TCP CLIENT", "... done reading from socket. Last read return=%d errno=%d\r\n", r, errno);
        close(s);
        ESP_LOGI("TCP CLIENT", "... new request in 5 seconds");
        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
    ESP_LOGI("TCP CLIENT", "...tcp_client task closed\n");
}
 */
