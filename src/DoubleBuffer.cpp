#include "DoubleBuffer.hpp"

DoubleBuffer::DoubleBuffer(SDWriter& wr) : writer{wr}{
	//this->writer = wr;
	this->firstBuffer = new BinaryBuffer();
	this->secondBuffer = new BinaryBuffer();
	this->current = this->firstBuffer;
	this->next = this->secondBuffer;
}

void DoubleBuffer::storeData(data in){
	if (!current->isFull()){
		current->add(in);
	}
	else if(!next->isFull()){
		this->swap();
		xEventGroupSetBits(GlobalEventGroupHandle, SensorBufferSdReady);
	}
}

void DoubleBuffer::swap(){
	BinaryBuffer * tmp = this->current;
	this->current = this->next;
	this->current->writeOnly();
	this->next = tmp;
	this->next->readOnly();
}

void DoubleBuffer::writeToSd(){
	// standard default name for now
	writer.SetFileName(0);
	writer.Open();
	// write all elements to the file using the SdWriter
	for(auto it = this->next->get().begin(); it != this->next->get().end(); ++it) {
		writer.Write(*it);
		ESP_LOGI("writer", "writing elements...");
	}
	writer.Close();
	// Clear the buffer for the next swap
	this->next->clear();
	
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
