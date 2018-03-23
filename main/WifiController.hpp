/**
* @file WifiController.hpp
* @data 21 september, 2017
*
* \class WifiController
*
* \brief Task that handles all wifi communication
*
* This class handles all the wifi communication.
* An xTimer will run and set the WifiActivateFlag.
* If the WifiActivateFlag is set, the wifi will
* be enabled and try to connect to the set WIFI_SSID
* using the set WIFI_PASSWORD.
*
* When connected, the activity data will be read
* and sent to the server.
*
* if the StandbyWifiTaskUnhandled is set, the task
* will disable wifi and go into an infinite loop.
*
*/

#pragma once

#include "esp_log.h"

#include "WiFiC.h"
#include "BaseTask.hpp"
#include "DataProcessor.hpp"
#include "MQTTController.hpp"

class WifiController : BaseTask {
public:
	WifiController(unsigned int TaskPriority, DataProcessor &dp);
	friend void run_wifi_task(void *args);
	MQTTController *mqtt;
private:
	DataProcessor &DPHandle;
    void *WIFITASK_TIMER_ID = 0;
protected:
	void main_task();
};
