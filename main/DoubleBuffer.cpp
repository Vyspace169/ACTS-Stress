#include "DoubleBuffer.hpp"

DoubleBuffer::DoubleBuffer(SDWriter& wr) : writer{wr} {
	this->firstBuffer = new BinaryBuffer();
	this->secondBuffer = new BinaryBuffer();
	this->current = this->firstBuffer;
	this->next = this->secondBuffer;

	this->firstRBuffer = new BinaryBuffer();
	this->secondRBuffer = new BinaryBuffer();
	this->currentR = this->firstBuffer;
	this->nextR = this->secondBuffer;

	this->firstRRBuffer = new BinaryBuffer();
	this->secondRRBuffer = new BinaryBuffer();
	this->currentRR = this->firstBuffer;
	this->nextRR = this->secondBuffer;
}

void DoubleBuffer::storeData(SampleData in){
	if (!current->isFull()){
		current->add(in);
	}
	else if(!next->isFull()){
		this->swap();
		xEventGroupSetBits(GlobalEventGroupHandle, SensorBufferSdReady);
	}
	//else { critical error }
}



void DoubleBuffer::storeRRData(){
	if (!currentRR->isFull()){
		currentRR->add(in);
	}
	else if(!nextRR->isFull()){
		this->swap();
		xEventGroupSetBits(GlobalEventGroupHandle, SensorBufferSdReady);
	}
	//else { critical error }
}

void DoubleBuffer::swap(){
	BinaryBuffer * tmp = this->current;
	this->current = this->next;
	this->current->writeOnly();
	this->next = tmp;
	this->next->readOnly();

	BinaryBuffer * tmp = this->currentR;
	this->currentR = this->nextR;
	this->currentR->writeOnly();
	this->nextR = tmp;
	this->nextR->readOnly();

	BinaryBuffer * tmp = this->currentRR;
	this->currentRR = this->nextRR;
	this->currentRR->writeOnly();
	this->nextRR = tmp;
	this->nextRR->readOnly();
}

void DoubleBuffer::writeToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->next->get().data(), this->next->get().size() * sizeof(SampleData));

	// Clear the buffer for the next swap
	this->next->clear();

	// write all elements to the file using the SdWriter
	writer.Write(this->nextRR->get().data(), this->nextRR->get().size() * sizeof(SampleData));

	// Clear the buffer for the next swap
	this->nextRR->clear();



}

void DoubleBuffer::emptyBuffer(){
	next->get();
	nextR->get();
	nextRR->get();
}

DoubleBuffer::~DoubleBuffer(){
	delete firstBuffer;
	delete secondBuffer;
	delete current;
	delete next;

	delete firstRBuffer;
	delete secondRBuffer;
	delete currentR;
	delete nextR;

	delete firstRRBuffer;
	delete secondRRBuffer;
	delete currentRR;
	delete nextRR;
}
