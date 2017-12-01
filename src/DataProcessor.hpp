/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "esp_log.h"

#include "DoubleBuffer.hpp"

class  DataProcessor{
public:
    DataProcessor();
    void SetTrigger(float triggerx, float triggery, float triggerz);
    void ResetActivityData();
    void HandleData(data NewData);
    double GetActivityData();
    ~DataProcessor();
private:
    data OldData;
    double ActivityData;
    float TriggerValueX;
    float TriggerValueY;
    float TriggerValueZ;
};
