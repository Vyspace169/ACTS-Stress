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
     Sensor() {}
     virtual int DataSize();
     virtual unsigned short* SensorRead();
     virtual void Sleep();
     ~Sensor() {}

     /*
     void setFrequency(int freq);
     int getFrequency();
     */
private:
};
