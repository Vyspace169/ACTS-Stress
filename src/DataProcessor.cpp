#include "DataProcessor.hpp"

DataProcessor::DataProcessor() {
	memset(&OldData, 0, sizeof(SampleData));
	TimeoutCounter = 0;
	TimeoutTrigger = TimeoutInitValue;
	ActivityData = 0;
	TriggerValueX = TriggerValueXInit;
	TriggerValueY = TriggerValueYInit;
	TriggerValueZ = TriggerValueZInit;
}

void DataProcessor::SetTimeoutValue(int NewTriggerms) {
	TimeoutTrigger = NewTriggerms;
}

void DataProcessor::SetTrigger(float triggerx, float triggery, float triggerz) {
	TriggerValueX = triggerx;
	TriggerValueY = triggery;
	TriggerValueZ = triggerz;
}

void DataProcessor::ResetActivityData() {
	ActivityData = 0;
}

void DataProcessor::HandleData(SampleData NewData) {
	float DifferentialValue = 0;

	DifferentialValue = NewData.accelX - OldData.accelX;
	if(DifferentialValue > TriggerValueX) {
		ActivityData += (DifferentialValue / TriggerValueX);
		TimeoutCounter = 0;
	}

	DifferentialValue = NewData.accelY - OldData.accelY;
	if(DifferentialValue > TriggerValueY) {
		ActivityData += (DifferentialValue / TriggerValueY);
		TimeoutCounter = 0;
	}

	DifferentialValue = NewData.accelZ - OldData.accelZ;
	if(DifferentialValue > TriggerValueZ) {
		ActivityData += (DifferentialValue / TriggerValueZ);
		TimeoutCounter = 0;
	}

	TimeoutCounter += SampleTimems;
	if(TimeoutCounter >= TimeoutTrigger) {
		xEventGroupSetBits(GlobalEventGroupHandle, MovementTimeoutReached);
	}

	memcpy((void*)&OldData, (void*)&NewData, sizeof(SampleData));
}

double DataProcessor::GetActivityData() {
	return ActivityData;
}

DataProcessor::~DataProcessor(){

}
