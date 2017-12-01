/**
* @file binaryBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief Buffer between sensor and SPI SD write
*/
#pragma once
#include <vector>

#include "esp_log.h"

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
	const std::vector<data>& get(); // should perhaps be a pointer, copy could be too slow on large scale operations?
	bool isFull();
	~BinaryBuffer();
private:
	bool readState();
	bool state;
	std::vector<data> buffer;
	const int BufferSize = 1000;
};
