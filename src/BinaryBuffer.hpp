/**
* @file binaryBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief Buffer between sensor and SPI SD write
*/
#pragma once
#include <vector>

#include "SystemVariables.hpp"
#include "esp_log.h"

class BinaryBuffer{
public:
	BinaryBuffer();
	void readOnly();
	void writeOnly();
	void clear();
	bool add( SampleData in );
	const std::vector<SampleData>& get(); // should perhaps be a pointer, copy could be too slow on large scale operations?
	bool isFull();
	~BinaryBuffer();
private:
	bool readState();
	bool state;
	std::vector<SampleData> buffer;
	const int BufferSize = BINARY_BUFFER_SIZE;
};
