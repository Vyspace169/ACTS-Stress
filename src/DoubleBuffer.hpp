/**
* @file doubleBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief switching buffer
*/
#pragma once
#include "BinaryBuffer.hpp"
#include "SDWriter.hpp"

class DoubleBuffer{
public:
	DoubleBuffer(SDWriter);
	void storeData( data in );
	void swap();
	void emptyBuffer();
	~DoubleBuffer();
private:
	SDWriter writer;
	BinaryBuffer * firstBuffer;
	BinaryBuffer * secondBuffer;
	BinaryBuffer * current;
	BinaryBuffer * next;
};
