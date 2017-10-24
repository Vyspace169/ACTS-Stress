#include "DoubleBuffer.hpp"

DoubleBuffer::DoubleBuffer(SDWriter wr){
	this->writer = wr;
	this->firstBuffer = new BinaryBuffer();
	this->secondBuffer = new BinaryBuffer();
}

void DoubleBuffer::storeData(data in){
	if (!current->isFull()){
		current->add(in);
	}
	else if(!next->isFull()){
		this->swap();
	}
	else{
		throw();
	}
}

void DoubleBuffer::swap(){
	BinaryBuffer * tmp = this->current;
	this->current = this->next;
	this->current->writeOnly();
	this->next = tmp;
	this->next->readOnly();
}

void DoubleBuffer::emptyBuffer(){
	next->get();
}

DoubleBuffer::~DoubleBuffer(){
	delete firstBuffer;
	delete secondBuffer;
	delete current;
	delete next;
}
