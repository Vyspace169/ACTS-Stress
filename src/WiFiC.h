#pragma once

#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "freertos/event_groups.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_wifi.h"

#include "apps/sntp/sntp.h"

#include "SystemVariables.hpp"

#ifdef __cplusplus
extern "C" {
#endif

extern void WiFiInitialize(void);

extern bool WiFiConnect(char* SSID, char* PASS, int timeout);

extern void WiFiSetPowerSave(void);

extern void WiFiDisconnect(void);

extern time_t WiFiGetTime(int retries);

extern bool WiFiGetConnectionStatus(void);

extern bool TCPConnectToServer(char *IPAddress, int port);

extern bool TCPSend(char *Data, int size);

extern void TCPDisconnect(void);

#ifdef __cplusplus
}
#endif
