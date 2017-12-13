#pragma once

#include "esp_log.h"

#include "BaseTask.hpp"
#include "WifiModule.hpp"

class WifiTask : BaseTask {
public:
	WifiTask(unsigned int task_priority);
private:
    void *WIFITASK_TIMER_ID = 0;
protected:
	void main_task();
};

