#include "DataProcessor.hpp"

DataProcessor::DataProcessor() {
	OldAcceleroXValue = 0;
	OldAcceleroYValue = 0;
	OldAcceleroZValue = 0;
	TimeoutCounter = 0;
	TimeoutTrigger = TIMEOUT_TIME_SEC * 1000;
	LastTriggerOn = false;
	ActivityData = 0;
	TriggerValueX = TRIGGER_VALUE_X;
	TriggerValueY = TRIGGER_VALUE_Y;
	TriggerValueZ = TRIGGER_VALUE_Z;
}

void DataProcessor::SetTimeoutValue(int NewTriggerms) {
	TimeoutTrigger = NewTriggerms;
}

void DataProcessor::SetTrigger(int triggerx, int triggery, int triggerz) {
	TriggerValueX = triggerx;
	TriggerValueY = triggery;
	TriggerValueZ = triggerz;
}

void DataProcessor::ResetActivityData() {
	ActivityData = 0;
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

#ifdef DATA_THROUGH_TCP
	sprintf(DataStringBuffer, "%llu,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,\r\n",
			NewData.microTime,
			NewData.accelX,
			NewData.accelY,
			NewData.accelZ,
			NewData.gyroX,
			NewData.gyroY,
			NewData.gyroZ,
			NewData.magnetoX,
			NewData.magnetoY,
			NewData.magnetoZ,
			NewData.temp,
			NewData.pressure);
	xEventGroupSetBits(GlobalEventGroupHandle, WifiReadyFlag);
#endif
}

double DataProcessor::GetActivityData() {
	return ActivityData;
}

#ifdef DATA_THROUGH_TCP
char* DataProcessor::GetDataString() {
	return DataStringBuffer;
}
#endif

DataProcessor::~DataProcessor(){

}
