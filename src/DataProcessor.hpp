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
    void SetTimeoutValue(int NewTriggerms);
    void SetTrigger(float triggerx, float triggery, float triggerz);
    void ResetActivityData();
    void HandleData(SampleData NewData);
    double GetActivityData();
    ~DataProcessor();
private:
    SampleData OldData;
    double ActivityData;
    float TriggerValueX;
    float TriggerValueY;
    float TriggerValueZ;
    int TimeoutCounter;
    int TimeoutTrigger;
    const int SampleTimems = 10;
    const int TimeoutInitValue = 60000;
    const int TriggerValueXInit = 500;
    const int TriggerValueYInit = 500;
    const int TriggerValueZInit = 15000;
};
