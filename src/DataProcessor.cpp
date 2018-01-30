#include "DataProcessor.hpp"

DataProcessor::DataProcessor() {
	OldAcceleroXValue = 0;
	OldAcceleroYValue = 0;
	OldAcceleroZValue = 0;
	TimeoutCounter = 0;
<<<<<<< HEAD
	TimeoutTrigger = TIMEOUT_TIME_SEC * 1000;
	LastTriggerOn = false;
	ActivityData = 0;
	TriggerValueX = TRIGGER_VALUE_X;
	TriggerValueY = TRIGGER_VALUE_Y;
	TriggerValueZ = TRIGGER_VALUE_Z;
=======
	TimeoutTrigger = 60000;
	LastTriggerOn = false;
	ActivityData = 0;
	TriggerValueX = 2000;
	TriggerValueY = 2000;
	TriggerValueZ = 2000;
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
}

void DataProcessor::SetTimeoutValue(int NewTriggerms) {
	TimeoutTrigger = NewTriggerms;
}

void DataProcessor::SetTrigger(int triggerx, int triggery, int triggerz) {
	TriggerValueX = triggerx;
	TriggerValueY = triggery;
	TriggerValueZ = triggerz;
}

void DataProcessor::HandleData(SampleData NewData) {
	int DifferentialValue = 0;
	bool TriggerOn = false;

	DifferentialValue = abs(NewData.accelX - OldAcceleroXValue);
	if(DifferentialValue > TriggerValueX) {
		ActivityData += DifferentialValue / TriggerValueX;
		TriggerOn = true;
	}

	DifferentialValue = abs(NewData.accelY - OldAcceleroYValue);
	if(DifferentialValue > TriggerValueY) {
		ActivityData += DifferentialValue / TriggerValueY;
		TriggerOn = true;
	}

	DifferentialValue = abs(NewData.accelZ - OldAcceleroZValue);
	if(DifferentialValue > TriggerValueZ) {
		ActivityData += DifferentialValue / TriggerValueZ;
		TriggerOn = true;
	}

	if(TriggerOn == true && LastTriggerOn == true) {
		TimeoutCounter = 0;
	}
	else {
		TimeoutCounter += SAMPE_TIME_MS;
		if(TimeoutCounter >= TimeoutTrigger) {
			xEventGroupSetBits(GlobalEventGroupHandle, MovementTimeoutReached);
		}
	}

	LastTriggerOn = TriggerOn;
	OldAcceleroXValue = NewData.accelX;
	OldAcceleroYValue = NewData.accelY;
	OldAcceleroZValue = NewData.accelZ;
}

double DataProcessor::GetActivityData() {
<<<<<<< HEAD
	if(!ActivityDataQueue.empty()) {
		return ActivityDataQueue.front();
	}
	else {
		return 0;
	}
}

void DataProcessor::PopData() {
	ActivityDataQueue.pop();
}

void DataProcessor::PushData() {
	ActivityDataQueue.push(ActivityData);
	ActivityData = 0;
}

int DataProcessor::DataCount() {
	return ActivityDataQueue.size();
=======
	return ActivityData;
}

void DataProcessor::ResetActivityData() {
	ActivityData = 0;
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
}

DataProcessor::~DataProcessor(){

}
