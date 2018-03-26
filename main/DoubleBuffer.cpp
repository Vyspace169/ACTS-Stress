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

void DoubleBuffer::storeRData(RData in){
	if (!currentR->isFull()){
		currentR->addR(in);
	}
	else if(!nextR->isFull()){
		this->swapR();
		xEventGroupSetBits(GlobalEventGroupHandle, RBufferReadyFlag);  //is deze nodig?
	}
	//else { critical error }
}

void DoubleBuffer::storeRRData(RRSeries in){
	if (!currentRR->isFull()){
		currentRR->addRR(in);
	}
	else if(!nextRR->isFull()){
		this->swapRR();
		xEventGroupSetBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	}
	//else { critical error }
}

void DoubleBuffer::storeHRVData(HRVData in){
	if (!currentHRV->isFull()){
		currentHRV->addHRV(in);
	}
	else if(!nextHRV->isFull()){
		this->swapHRV();
		xEventGroupSetBits(GlobalEventGroupHandle, HRVBufferReadyFlag);
	}
	//else { critical error }
}

void DoubleBuffer::swap(){
	BinaryBuffer * tmp = this->current;
	this->current = this->next;
	this->current->writeOnly();
	this->next = tmp;
	this->next->readOnly();
}

void DoubleBuffer::swapR(){
	BinaryBuffer * tmp = this->currentR;
	this->currentR = this->nextR;
	this->currentR->writeOnly();
	this->nextR = tmp;
	this->nextR->readOnly();
}

void DoubleBuffer::swapRR(){
	BinaryBuffer * tmp = this->currentRR;
	this->currentRR = this->nextRR;
	this->currentRR->writeOnly();
	this->nextRR = tmp;
	this->nextRR->readOnly();
}
void DoubleBuffer::swapHRV(){
	BinaryBuffer * tmp = this->currentHRV;
	this->currentHRV = this->nextHRV;
	this->currentRR->writeOnly();
	this->nextHRV = tmp;
	this->nextHRV->readOnly();
}

void DoubleBuffer::writeToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->next->getSD().data(), this->next->getSD().size() * sizeof(SampleData));

	// Clear the buffer for the next swap
	this->next->clear();

}

void DoubleBuffer::writeRRToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->nextRR->getRR().data(), this->nextRR->getRR().size() * sizeof(RRSeries));

	// Clear the buffer for the next swap
	this->nextRR->clear();

}

void DoubleBuffer::writeHRVToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->nextHRV->getHRV().data(), this->nextHRV->getHRV().size() * sizeof(HRVData));

	// Clear the buffer for the next swap
	this->nextHRV->clear();

}

void DoubleBuffer::emptyBuffer(){
	next->getSD();
	nextR->getR();
	nextRR->getRR();
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
