/**
* @file D.hpp
* @Author Jop van Buuren, Ferdie Stoeltie
* @data 21 september, 2017
* @brief
*/

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

class WifiModule {
public:
    WifiModule();
    bool ClientSetStaticIP(char *ip, char *gateway, char *netmask);
    bool ClientConfig(char* SSID, char* PASS);
    bool ClientConnect(int timeout);
    bool ClientDisconnect();
    bool ClientGetConnectionState();
    void ClientDeinit();
    ~WifiModule();
private:
    bool state;
    bool WifiIPSet;
    bool WifiIsInitialized;
    unsigned int IPAddress;
    unsigned int Gateway;
    unsigned int Netmask;
};
