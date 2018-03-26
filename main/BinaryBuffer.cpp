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

void BinaryBuffer::clearR(){
	RBuffer.clear();
}

void BinaryBuffer::clearRR(){
	RRBuffer.clear();
}

void BinaryBuffer::clearHRV(){
	HRVBuffer.clear();
}

bool BinaryBuffer::add( SampleData in ){
	if(state){
		buffer.push_back(in);
		return true;
	}else{
		return false;
	}
}

bool BinaryBuffer::addR( RData in ){
	if(state){
		RBuffer.push_back(in);
		return true;
	}else{
		return false;
	}
}

bool BinaryBuffer::addRR( RRSeries in ){
	if(state){
		RRBuffer.push_back(in);

		ESP_LOGW("DoubleBuffer", "RR buffer size %d", RRBuffer.size());
		return true;
	}else{
		return false;
	}
}

bool BinaryBuffer::addHRV( HRVData in ){
	if(state){
		HRVBuffer.push_back(in);
		return true;
	}else{
		return false;
	}
}

const std::vector<SampleData>& BinaryBuffer::getSD() {
	return buffer;
}

std::vector<RData>& BinaryBuffer::getR() {
	return RBuffer;
}

std::vector<RRSeries>& BinaryBuffer::getRR() {
	return RRBuffer;
}

std::vector<HRVData>& BinaryBuffer::getHRV() {
	return HRVBuffer;
}

bool BinaryBuffer::isFull(){
	return buffer.size() >= BufferSize;
}

bool BinaryBuffer::isFullR(){
	return RBuffer.size() >= BufferSize;
}

bool BinaryBuffer::isFullRR(){
	return RRBuffer.size() >= BufferSize;
}

bool BinaryBuffer::isFullHRV(){
	return HRVBuffer.size() >= BufferSize;
}

BinaryBuffer::~BinaryBuffer(){
	//
}
