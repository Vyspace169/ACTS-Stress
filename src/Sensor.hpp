/**
* @file Sensor.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#pragma once
#define OUTPUT_SIZE 4
#include "DoubleBuffer.hpp"
<<<<<<< HEAD

=======
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
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
<<<<<<< HEAD
=======
protected:
        // int frequency;
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
};
