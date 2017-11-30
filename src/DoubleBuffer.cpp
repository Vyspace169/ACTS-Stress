#include "DoubleBuffer.hpp"

DoubleBuffer::DoubleBuffer(SDWriter& wr) : writer{wr}{
	//this->writer = wr;
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
}

void DoubleBuffer::swap(){
	BinaryBuffer * tmp = this->current;
	this->current = this->next;
	this->current->writeOnly();
	this->next = tmp;
	this->next->readOnly();
}
void DoubleBuffer::WriteToSd()	{

	// standard default name for now
	writer.SetFileName(0);
	// write all elements to the file using the SdWriter
	for(std::vector<data>::iterator it = this->next.buffer.begin(); it != this->next.buffer.end(); ++it) {
   	writer.Write(*it);
	}
	// Clear the buffer for the next swap
	this->next.buffer.clear();
	
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
