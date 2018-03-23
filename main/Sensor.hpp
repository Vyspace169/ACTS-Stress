/**
* @file Sensor.hpp
* @data 21 september, 2017
*
* \class Sensor
*
* \brief Parent class for all sensors
*
* This class is a parent class for all sensor
* which are used in this project.
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
