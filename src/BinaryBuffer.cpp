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

bool BinaryBuffer::add( SampleData in ){
	if(state){
		buffer.push_back(in);
		return true;
	}else{
		return false;
	}
}
const std::vector<SampleData>& BinaryBuffer::get(){
	return buffer;
}

bool BinaryBuffer::isFull(){
	return buffer.size() >= BufferSize;
}

BinaryBuffer::~BinaryBuffer(){
	//
}
