#include "DataProcessor.hpp"

DataProcessor::DataProcessor() {
	memset(&OldData, 0, sizeof(data));
	ActivityData = 0;
	TriggerValueX = 500;
	TriggerValueY = 500;
	TriggerValueZ = 15000;
}

void DataProcessor::SetTrigger(float triggerx, float triggery, float triggerz) {
	TriggerValueX = triggerx;
	TriggerValueY = triggery;
	TriggerValueZ = triggerz;
}

void DataProcessor::ResetActivityData() {
	ActivityData = 0;
}

void DataProcessor::HandleData(data NewData) {
	float DifferentialValue = 0;

	DifferentialValue = NewData.accelX - OldData.accelX;
	if(DifferentialValue > TriggerValueX) {
		ActivityData += (DifferentialValue / TriggerValueX);
	}

	DifferentialValue = NewData.accelY - OldData.accelY;
	if(DifferentialValue > TriggerValueY) {
		ActivityData += (DifferentialValue / TriggerValueY);
	}

	DifferentialValue = NewData.accelZ - OldData.accelZ;
	if(DifferentialValue > TriggerValueZ) {
		ActivityData += (DifferentialValue / TriggerValueZ);
	}

	memcpy((void*)&OldData, (void*)&NewData, sizeof(data));
}

double DataProcessor::GetActivityData() {
	return ActivityData;
}

DataProcessor::~DataProcessor(){

}
