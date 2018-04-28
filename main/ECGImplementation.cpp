/*
 * ECGImplementation.cpp
 *
 *  Created on: 22 mrt. 2018
 *      Author: casper.rebholz
 */

#include "ECGImplementation.hpp"

ECGImplementation::ECGImplementation(){
	memset(ECGData, 0, sizeof(short) * 2 );
}


int ECGImplementation::DataSize() {
	return sizeof(int) * 2;
}

unsigned short* ECGImplementation::SensorRead() {
	//sample_value[i] = adc1_get_raw(ADC_ECG);
	sample_value[i] = sine[sample_number];

	//Check for buffer overflow
	if(SampleNumber > BINARY_BUFFER_SIZE-1){
		SampleNumber = 0;
	}

	//Check kernel content
	if(KernelFilled == true){
		//Moving Average filter
		for(int j = 0; j < KernelSize; j++){
			ECGValue += SampleValue[j];
		}
		ECGValue = ECGValue/KernelSize;
	}else {
		//No filter
		ECGValue = SampleValue[i];
	}


	sample_number++;
	i++;

	if(i == kernel_size){
		i = 0;
	}


	//Copy ECGValue and SampleNumber to ECGData array
	memcpy(ECGData, &ECGValue, sizeof(short));
	memcpy(&ECGData[1], &SampleNumber, sizeof(short));

	//Reset ECGValue for next filter calculation
	ECGValue = 0;

	return &ECGData[0];

}

void ECGImplementation::Sleep() {

}
