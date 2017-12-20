#pragma once

#include "esp_log.h"

#include "apps/sntp/sntp.h"

#include "BaseTask.hpp"
#include "WifiModule.hpp"
#include "DataProcessor.hpp"

class WifiTask : BaseTask {
public:
	WifiTask(unsigned int task_priority, DataProcessor &dp);
	friend void run_wifi_task(void *args);
private:
	DataProcessor &DPHandle;
    void *WIFITASK_TIMER_ID = 0;
protected:
	void main_task();
};

