/**
* @file binaryBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief Buffer between sensor and SPI SD write
*/
#pragma once
#include <vector>
const int BUFFER_SIZE = 50000/(12*2); // 50000 bytes / 12 values * 2 bytes (float size) 
typedef struct {
	long long microTime;
	float accelX;
	float accelY;
	float accelZ;
	float gyroX;
	float gyroY;
	float gyroZ;
	float magnetoX;
	float magnetoY;
	float magnetoZ;
	float temp;
	float pressure;
} data;

class BinaryBuffer{
public:
	BinaryBuffer();
	void readOnly();
	void writeOnly();
	void clear();
	bool add( data in );
	data get();
	bool isFull();
	~BinaryBuffer();
private:
	bool readState();
	bool state;
	std::vector<data> buffer;
};
