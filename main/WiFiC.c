#include "WiFiC.h"

static bool WifiEnabled = false;
static bool connected = false;
static EventGroupHandle_t wifi_event_group_1;

esp_err_t local_wifi_Event_handler(void *ctx, system_event_t *event)
{
	if(WifiEnabled == true) {
		switch(event->event_id) {
			case SYSTEM_EVENT_STA_START:
				esp_wifi_connect();
				ESP_LOGI("WIFI EVENT", "Starting wifi..."); // @suppress("Symbol is not resolved")
				break;
			case SYSTEM_EVENT_STA_GOT_IP:
				xEventGroupSetBits(wifi_event_group_1, WIFI_EVENT_BIT);
				ESP_LOGI("WIFI EVENT", "Got IP: %s", ip4addr_ntoa(&event->event_info.got_ip.ip_info.ip)); // @suppress("Symbol is not resolved")
				break;
			case SYSTEM_EVENT_STA_DISCONNECTED:
				xEventGroupClearBits(wifi_event_group_1, WIFI_EVENT_BIT);
				esp_wifi_connect();
				ESP_LOGI("WIFI EVENT", "WiFi disconnected, restarting wifi..."); // @suppress("Symbol is not resolved")
				break;
			default:
				ESP_LOGI("WIFI EVENT", "Different event_id: 0x%08x", event->event_id); // @suppress("Symbol is not resolved")
				break;
		}
	}

    return ESP_OK;
}

void WiFiInitialize(char* SSID, char* PASS) {
	tcpip_adapter_init();
	wifi_event_group_1 = xEventGroupCreate();
	esp_event_loop_init(local_wifi_Event_handler, NULL);
	wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // @suppress("Symbol is not resolved")
	esp_wifi_init(&cfg);
	esp_wifi_set_storage(WIFI_STORAGE_FLASH);
	wifi_config_t wifi_config;
	strcpy((char*) wifi_config.sta.ssid, SSID);
	strcpy((char*) wifi_config.sta.password, PASS);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config);
}

bool WiFiConnect(int timeout) {
	WifiEnabled = true;
	esp_wifi_start();
	xEventGroupWaitBits(wifi_event_group_1, WIFI_EVENT_BIT, false, true, timeout / portTICK_PERIOD_MS); // @suppress("Symbol is not resolved")

	if (xEventGroupGetBits(wifi_event_group_1) & WIFI_EVENT_BIT) {
		connected = true;
		return true;
	} else {
		connected = false;
		WifiEnabled = false;
		xEventGroupClearBits(wifi_event_group_1, WIFI_EVENT_BIT);
		esp_wifi_disconnect();
		esp_wifi_stop();
		return false;
	}
}

void WiFiSetPowerSave(void) {
	if (connected == false) {
		return;
	}
	esp_wifi_set_ps(WIFI_PS_MODEM);
}

void WiFiDisconnect(void) {
	connected = false;
	WifiEnabled = false;
	xEventGroupClearBits(wifi_event_group_1, WIFI_EVENT_BIT);
	esp_wifi_disconnect();
	esp_wifi_stop();
}

time_t WiFiGetTime(int retries) {
	if (connected == false) {
		return 0;
	}
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_init();
	time_t now = 0;
	int retry = 0;
	while (now < 948000000 && ++retry < retries) {
		vTaskDelay(100 / portTICK_PERIOD_MS); // @suppress("Symbol is not resolved")
		time(&now);
	}
	sntp_stop();
	if (now < 948000000) {
		return 0;
	}
	return now;
}

bool WiFiGetConnectionStatus(void) {
	return connected;
}
