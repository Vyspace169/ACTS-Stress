#pragma once

#include "esp_log.h"

#include "apps/sntp/sntp.h"

#include "BaseTask.hpp"
#include "WifiModule.hpp"
#include "DataProcessor.hpp"

class WifiTask : BaseTask {
public:
   /// @brief Wifi task that takes care of handling the wifi stack,
   /// currently it is used for sending the performance index over wifi
	WifiTask(unsigned int task_priority, DataProcessor &dp);
   /// friend task that is the task that executes this object
	friend void run_wifi_task(void *args);
private:
	// Dataprocessor that can process the data
   DataProcessor &DPHandle;
   void *WIFITASK_TIMER_ID = 0;
protected:
	void main_task();
};

