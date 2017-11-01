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

void BinaryBuffer::add( data in ){
	(state) ? buffer.push_back(in) : throw BOException();
}

bool BinaryBuffer::isFull(){
	return buffer.size()>=100;
}

BinaryBuffer::~BinaryBuffer(){
	//
}
