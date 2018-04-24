#include "DoubleBuffer.hpp"

	std::vector<float> DoubleBuffer::Workspace1;
	std::vector<float> DoubleBuffer::Workspace2;

DoubleBuffer::DoubleBuffer(SDWriter &wr) :
writer{wr}
{
	this->firstBuffer = new BinaryBuffer();
	this->secondBuffer = new BinaryBuffer();
	this->current = this->firstBuffer;
	this->next = this->secondBuffer;
	this->firstBuffer->getSD().reserve(BINARY_BUFFER_SIZE);
	this->secondBuffer->getSD().reserve(BINARY_BUFFER_SIZE);

	this->firstRBuffer = new BinaryBuffer();
	this->secondRBuffer = new BinaryBuffer();
	this->currentR = this->firstRBuffer;
	this->nextR = this->secondRBuffer;
	this->firstRBuffer->getR().reserve(RBUFFER_SIZE);
	this->secondRBuffer->getR().reserve(RBUFFER_SIZE);

	this->firstRRBuffer = new BinaryBuffer();
	this->secondRRBuffer = new BinaryBuffer();
	this->currentRR = this->firstRRBuffer;
	this->nextRR = this->secondRRBuffer;
	this->firstRRBuffer->getRR().reserve(RRBUFFER_SIZE);
	this->secondRRBuffer->getRR().reserve(RRBUFFER_SIZE);

	this->firstLombBuffer = new BinaryBuffer();
	this->secondLombBuffer = new BinaryBuffer();
	this->currentLomb = this->firstLombBuffer;
	this->nextLomb = this->secondLombBuffer;
	this->firstLombBuffer->getLomb().reserve(LOMBBUFFER_SIZE);
	this->secondLombBuffer->getLomb().reserve(LOMBBUFFER_SIZE);

	this->firstHRVBuffer = new BinaryBuffer();
	this->secondHRVBuffer = new BinaryBuffer();
	this->currentHRV = this->firstHRVBuffer;
	this->nextHRV = this->secondHRVBuffer;
	this->firstHRVBuffer->getHRV().reserve(HRVBUFFER_SIZE);
	this->secondHRVBuffer->getHRV().reserve(HRVBUFFER_SIZE);

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
	if (!currentR->isFullR()){
		currentR->addR(in);
	}
	else if(!nextR->isFullR()){
		this->swapR();
		ESP_LOGI("DoubleBuffer", "RBufferReadyFlag set");
		xEventGroupSetBits(GlobalEventGroupHandle, RBufferReadyFlag);

	}
	//else { critical error }
}

void DoubleBuffer::storeRRData(RRSeries in){
	if (!currentRR->isFullRR()){
		currentRR->addRR(in);
	}
	else if(!nextRR->isFullRR()){
		this->swapRR();
	}
	//else { critical error }
}

void DoubleBuffer::storeLombData(Lomb in){
	if (!currentLomb->isFullLomb()){
		currentLomb->addLomb(in);
	}
	else if(!nextRR->isFullLomb()){
		this->swapLomb();
		xEventGroupSetBits(GlobalEventGroupHandle, LombBufferReadyFlag);
	}
	//else { critical error }
}

void DoubleBuffer::storeHRVData(HRVData in){
	if (!currentHRV->isFullHRV()){
		currentHRV->addHRV(in);
	}
	else if(!nextHRV->isFullHRV()){
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
	xEventGroupSetBits(GlobalEventGroupHandle, SensorBufferSdReady);
}

void DoubleBuffer::swapR(){
	ESP_LOGI("DoubleBuffer", "Swap R buffer"); // @suppress("Symbol is not resolved")
	BinaryBuffer * tmpR = this->currentR;
	this->currentR = this->nextR;
	this->currentR->writeOnly();
	this->nextR = tmpR;
	this->nextR->readOnly();
}

void DoubleBuffer::swapRR(){
	ESP_LOGI("DoubleBuffer", "Swap RR buffer"); // @suppress("Symbol is not resolved")
	BinaryBuffer * tmpRR = this->currentRR;
	this->currentRR = this->nextRR;
	this->currentRR->writeOnly();
	this->nextRR = tmpRR;
	this->nextRR->readOnly();
}

void DoubleBuffer::swapLomb(){
	ESP_LOGI("DoubleBuffer", "Swap Lomb buffer"); // @suppress("Symbol is not resolved")
	BinaryBuffer * tmpLomb = this->currentLomb;
	this->currentLomb = this->nextLomb;
	this->currentLomb->writeOnly();
	this->nextLomb = tmpLomb;
	this->nextLomb->readOnly();
}

void DoubleBuffer::swapHRV(){
	BinaryBuffer * tmpHRV = this->currentHRV;
	this->currentHRV = this->nextHRV;
	this->currentRR->writeOnly();
	this->nextHRV = tmpHRV;
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
	this->nextRR->clearRR();

}

void DoubleBuffer::writeLombToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->nextLomb->getLomb().data(), this->nextLomb->getLomb().size() * sizeof(Lomb));

	// Clear the buffer for the next swap
	this->nextLomb->clearLomb();

}

void DoubleBuffer::writeHRVToSd(){
	// write all elements to the file using the SdWriter
	writer.Write(this->nextHRV->getHRV().data(), this->nextHRV->getHRV().size() * sizeof(HRVData));

	// Clear the buffer for the next swap
	this->nextHRV->clearHRV();

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
