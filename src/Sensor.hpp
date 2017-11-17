/**
* @file Sensor.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#pragma once
#define OUTPUT_SIZE 4
#include<DoubleDuffer>
class  Sensor{
    public:
        Sensor(int frequency): frequency{frequency} {}
        virtual unsigned short[OUTPUT_SIZE] read();
        void setFrequency(int freq);
        int getFrequency();
        ~Sensor();
    private:
        int frequency;
};
