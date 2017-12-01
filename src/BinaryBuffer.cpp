#include "BinaryBuffer.hpp"
BinaryBuffer::BinaryBuffer(){
	buffer.clear();
	state = true;
}

void BinaryBuffer::readOnly(){
	state = false;
}

void BinaryBuffer::writeOnly(){
	state = true;
}

void BinaryBuffer::clear(){
	buffer.clear();
}

bool BinaryBuffer::add( data in ){
	ESP_LOGI("DB", "size: %d", buffer.size());
	if(state){
		buffer.push_back(in);
		return true;
	}else{
		return false;
	}
}
const std::vector<data>& BinaryBuffer::get(){
	return buffer;
}

bool BinaryBuffer::isFull(){
	return buffer.size() >= BufferSize;
}

BinaryBuffer::~BinaryBuffer(){
	//
}
