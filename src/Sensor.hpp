/**
* @file Sensor.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#pragma once
#define OUTPUT_SIZE 4
#include "DoubleBuffer.hpp"
class Sensor{
public:
     Sensor();
     virtual unsigned short* /*[OUTPUT_SIZE]*/ SensorRead() = 0;
/*     void setFrequency(int freq);
     int getFrequency();*/
     ~Sensor();
private:
protected:
        // int frequency;
};
