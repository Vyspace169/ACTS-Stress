/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief 
*/
#pragma once
#include "BinaryBuffer.hpp"

class SDWriter{
public:
	SDWriter();
	void write(data in);
	void status();
	~SDWriter();
private:
	bool state;
};