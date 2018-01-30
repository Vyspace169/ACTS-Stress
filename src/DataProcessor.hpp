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
#include <queue>

#include "esp_log.h"

#include "DoubleBuffer.hpp"
#include "SystemVariables.hpp"

class  DataProcessor{
public:
    DataProcessor();
    void SetTimeoutValue(int NewTriggerms);
    void SetTrigger(int triggerx, int triggery, int triggerz);
    void HandleData(SampleData NewData);
    double GetActivityData();
    void PopData();
    void PushData();
    int DataCount();
    ~DataProcessor();
private:
    int TimeoutCounter;
    int TimeoutTrigger;
    bool LastTriggerOn;
    int OldAcceleroXValue;
    int OldAcceleroYValue;
    int OldAcceleroZValue;
    double ActivityData;
    int TriggerValueX;
    int TriggerValueY;
    int TriggerValueZ;
    std::queue<double> ActivityDataQueue;
};
