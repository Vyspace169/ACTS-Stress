/**
* @file Constants.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#pragma once
class  MovementStack{
    public:
        static const char OFFSET_ACCELX = 0;
        static const char OFFSET_ACCELY = 1;
        static const char OFFSET_ACCELZ = 2;

        static const char OFFSET_GYROX = 3;
        static const char OFFSET_GYROY = 4;
        static const char OFFSET_GYROZ = 5;

        static const char OFFSET_MAGX = 6;
        static const char OFFSET_MAGY = 7;
        static const char OFFSET_MAGZ = 8;

        static const char OFFSET_PRESSURE = 0;
        static const char OFFSET_TEMP = 1;
};
