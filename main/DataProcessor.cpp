#include "DataProcessor.hpp"

DataProcessor::DataProcessor(DoubleBuffer &db) :
DBHandler{db}
{

	OldAcceleroXValue = 0;
	OldAcceleroYValue = 0;
	OldAcceleroZValue = 0;
	TimeoutCounter = 0;
	TimeoutTrigger = 60000;
	LastTriggerOn = false;
	ActivityData = 0;
	TriggerValueX = 2000;
	TriggerValueY = 2000;
	TriggerValueZ = 2000;
	Magnitude= 0;

	FirstRPeak = 0;
	SecondRPeak = 0;
	RRInterval = 0;
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

	Magnitude += sqrt(pow(NewData.accelX, 2) + pow(NewData.accelY, 2) + pow(NewData.accelZ, 2));

	LastTriggerOn = TriggerOn;
	OldAcceleroXValue = NewData.accelX;
	OldAcceleroYValue = NewData.accelY;
	OldAcceleroZValue = NewData.accelZ;
}

movement_type_t DataProcessor::GetActivityData() {
	return ActivityData;
}

void DataProcessor::ResetActivityData() {
	ActivityData = 0;
}

void DataProcessor::CalculateRRInterval()
{
	CurrentR = this->DBHandler.nextR->getR().begin();
	EndR = this->DBHandler.nextR->getR().end();

	//Loop through Rbuffer until first peak is found
	while(PeakHasBeenFound == false && CurrentR != EndR){
		//Check if current value if higher than last value. If so, keep looping
		if(CurrentR->potentialRPeak > LastRValue){
			LastRValue = CurrentR->potentialRPeak;
			CurrentR++;
		}else{
			//Peak has been found, assign previous sample number to FirstRPeak
			FirstRPeak = (CurrentR->sampleNr-1);
			PeakHasBeenFound = true;
			LastRValue = CurrentR->potentialRPeak;
		}
	}

	//Loop through RBuffer until valley is found
	while(CurrentR->potentialRPeak <= CurrentRValue && CurrentR != EndR){
		CurrentRValue = CurrentR->potentialRPeak;
		CurrentR++;
	}

		while(PeakHasBeenFound == true && CurrentR != EndR){
			if(CurrentR->potentialRPeak > CurrentRValue){
				//ESP_LOGI("DataProcessor"," Samplevalue : %d : Samplenumber : %d", CurrentR->potentialRPeak, CurrentR->sampleNr);
				CurrentRValue = CurrentR->potentialRPeak;
				CurrentR++;
			}else{
				NextRPeak = (CurrentR->sampleNr-1);
				//ESP_LOGI("DataProcessor"," Next Peak found, peak is: %d at sample %d", CurrentRValue, SecondRPeak);

				//Check if RBuffer has been swapped
				if(NextRPeak > FirstRPeak){
					//Regular RR-interval calculation
					RRInterval = ((SecondRPeak - FirstRPeak) * 1000) / SAMPLE_RATE_H;
				}else{
					//Alternative RR-interval calculation
					RRInterval = ((BINARY_BUFFER_SIZE - FirstRPeak + NextRPeak) * 1000) / SAMPLE_RATE_H;
				}

				//Check for ectopic beat
				if(RRInterval <= 0.8*LastRRInterval && LastBeatWasEctopic == false){
					//Invalid RR-interval
					FirstRPeak = SecondRPeak;
					LastBeatWasEctopic = true;
				} else{
					//Valid RR-interval
					if(LastBeatWasEctopic == false){
						//Last RR-interval was also valid, store in RRBuffer
						RRData.RRInterval = RRInterval;
						this->DBHandler.storeRRData(RRData);
						LastRRInterval = RRInterval;
						NextRPeak = SecondRPeak;
					} else{
						//Last RR-interval wasn't valid, don't store
						FirstRPeak = SecondRPeak;
						LastBeatWasEctopic = false;
					}
				}

				LastRRInterval = RRInterval;

				while(CurrentR->potentialRPeak <= CurrentRValue && CurrentR != EndR){
					//ESP_LOGI("DataProcessor"," Samplevalue : %d : Samplenumber : %d", CurrentR->potentialRPeak, CurrentR->sampleNr);
					CurrentRValue = CurrentR->potentialRPeak;
					CurrentR++;
					//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
					//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				}

				if(RRTotal > 30000){
					this->DBHandler.swapRR();
					xEventGroupSetBits(GlobalEventGroupHandle, RRBufferReadyFlag);
					ESP_LOGI("DataProcessor","RRBufferReadyFlag set");
					RRTotal = 0;
				}

				if(CurrentR != EndR){
					CurrentRValue = CurrentR->potentialRPeak;
					CurrentR++;
				}
			}
		}
	}
	this->DBHandler.nextR->clearR();
	PeakHasBeenFound = false;
	xEventGroupClearBits(GlobalEventGroupHandle, RBufferReadyFlag);
	ESP_LOGI("DataProcessor","RBufferReadyFlag cleared");
}

void DataProcessor::period() {
	const double TWOPI=6.283185307179586476;
	int i,j, RRTotalTMP = 0;
	int n=this->DBHandler.nextRR->getRR().size();
	int np = this->DBHandler.currentLomb->getLomb().size();
	double AverageRR,c,cc,cwtau,effm,expy,pnow = 0, prob,pymax = 0,s,ss,sumc,sumcy,sums,sumsh,
		sumsy,swtau,VarianceRR = 0, VarianceTMP = 0,wtau,xave,xdif,xmax,xmin,yy,arg,wtemp;
	wi.resize(n), wpi.resize(n), wpr.resize(n),wr.resize(n);
	int nout=int(0.5*OversamplingFactor*HighestFrequency*n);

	//iterators for RRBuffer
	RRBegin = this->DBHandler.nextRR->getRR().begin();
	RREnd = this->DBHandler.nextRR->getRR().end();

	if (np < nout) {
		this->DBHandler.currentLomb->getLomb().resize(nout);
	}

	//Loop through RRBuffer to calculate abscissas
	for(RRBegin = this->DBHandler.nextRR->getRR().begin(); RRBegin < RREnd; RRBegin++){
		RRTotalTMP += RRBegin->RRInterval;
		RRBegin->RRTotal = RRTotalTMP;
		//Create overlap
		if(RRBegin->RRTotal >= OneMinute && OverlapCreated == false){
			NextHRVMarker = RRBegin-1;
			this->DBHandler.currentRR->getRR().insert(this->DBHandler.currentRR->getRR().begin(),
					NextHRVMarker, RREnd);
			OverlapCreated = true;
		}
	}

	//Reset iterator to first element in vector
	RRBegin = this->DBHandler.nextRR->getRR().begin();

	AverageRR = (RREnd-1)->RRTotal/n;

	ESP_LOGI("DataProcessor", "Total RR %ld", (RREnd-1)->RRTotal);
	ESP_LOGI("DataProcessor", "NRofRR %d", n);
	ESP_LOGI("DataProcessor", "Average RR %f", AverageRR);

	//Calculate variance
	while(RRBegin != RREnd){
		VarianceTMP += pow((RRBegin->RRInterval-AverageRR),2);
		RRBegin++;
	}
	VarianceRR = VarianceTMP/n;
	ESP_LOGI("DataProcessor", "Variance RR %f", VarianceRR);

	//Reset iterator to first element in vector
	RRBegin = this->DBHandler.nextRR->getRR().begin();

	if (VarianceRR == 0.0){
		ESP_LOGE("DataProcessor::period", "zero variance in period");
	}

	xmin = RRBegin->RRTotal;
	xmax = (RREnd-1)->RRTotal;
	xdif = xmax - xmin;
	xave = 0.5 * (xmax + xmin);

	for (j=0; j<n; j++, RRBegin++) {
			arg=TWOPI*((RRBegin->RRTotal-xave)*pnow);
			wpr.at(j)= -2.0*SQR(sin(0.5*arg));
			wpi.at(j)=sin(arg);
			wr.at(j)=cos(arg);
			wi.at(j)=wpi.at(j);
	}

	//Reset iterator to first element in vector
	RRBegin = this->DBHandler.nextRR->getRR().begin();

	pnow = 1.0/(OversamplingFactor*xdif);
	for (i=0;i<nout;i++) {
		LombData.Frequency=pnow;
		ESP_LOGI("DataProcessor", "pnow %f", pnow);
		sumsh=sumc=0.0;
		for (j=0;j<n;j++) {
			c=wr.at(j);
			ESP_LOGI("DataProcessor", "c %f", c);
			s=wi.at(j);
			ESP_LOGI("DataProcessor", "s %f", s);
			sumsh += s*c;
			ESP_LOGI("DataProcessor", "sumsh %f", sumsh);
			sumc += (c-s)*(c+s);
			ESP_LOGI("DataProcessor", "sumc %f", sumc);
		}
		wtau=0.5*atan2(2.0*sumsh,sumc);
				swtau=sin(wtau);
				ESP_LOGI("DataProcessor", "swtau %f", swtau);
				cwtau=cos(wtau);
				ESP_LOGI("DataProcessor", "cwtau %f", cwtau);
				sums=sumc=sumsy=sumcy=0.0;
				for (j=0;j<n;j++, RRBegin++) {
					s=wi.at(j);
					ESP_LOGI("DataProcessor", "s %f", s);
					c=wr.at(j);
					ESP_LOGI("DataProcessor", "c %f", c);
					ss=s*cwtau-c*swtau;
					ESP_LOGI("DataProcessor", "ss %f", ss);
					cc=c*cwtau+s*swtau;
					ESP_LOGI("DataProcessor", "cc %f", cc);
					sums += ss*ss;
					ESP_LOGI("DataProcessor", "sums %f", sums);
					sumc += cc*cc;
					ESP_LOGI("DataProcessor", "sumc %f", sumc);
					yy=RRBegin->RRInterval-AverageRR;
					ESP_LOGI("DataProcessor", "yy %f", yy);
					sumsy += yy*ss;
					ESP_LOGI("DataProcessor", "sumsy %f", sumsy);
					sumcy += yy*cc;
					ESP_LOGI("DataProcessor", "sumcy %f", sumcy);
					wr.at(j)=((wtemp=wr.at(j))*wpr.at(j)-wi.at(j)*wpi.at(j))+wr.at(j);
					ESP_LOGI("DataProcessor", "wr.at(j) %f", wr.at(j));
					wi.at(j)=(wi.at(j)*wpr.at(j)+wtemp*wpi.at(j))+wi.at(j);
					ESP_LOGI("DataProcessor", "wi.at(j) %f", wi.at(j));
				}
				LombData.LombValue=0.5*((sumcy*sumcy/sumc)+(sumsy*sumsy/sums))/VarianceRR;
				if (LombData.LombValue >= pymax){
					pymax=LombData.LombValue;
				}
				this->DBHandler.storeLombData(LombData);
				ESP_LOGI("DataProcessor::period","Frequency: %f: Lombvalue : %f", LombData.Frequency, LombData.LombValue);
				pnow += 1.0/(OversamplingFactor*xdif);
	}
	expy=exp(-pymax);
	effm=2.0*nout/OversamplingFactor;
	prob=effm*expy;
	if (prob > 0.01) prob=1.0-pow(1.0-expy,effm);

	this->DBHandler.swapLomb();
	xEventGroupClearBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	ESP_LOGI("DataProcessor::period","RRBufferReadyFlag cleared");
	xEventGroupSetBits(GlobalEventGroupHandle, LombBufferReadyFlag);
	ESP_LOGI("DataProcessor::period","LombBufferReadyFlag Ready");
	CalculateHRV();
}

/*
void DataProcessor::period() {
	int NrOfRR 	= this->DBHandler.nextRR->getRR().size();
	int np 		= this->DBHandler.currentLomb->getLomb().size();
	const double TWOPI=6.283185307179586476;
	int i, j, RRTotalFront, RRTotalBack, RRIntervalFront, RRIntervalBack, RRRange, RRTotalTMP= 0, RRTMP;
	double c,cc,cwtau,effm,expy,CurrentFrequency,prob,pymax,s,ss,sumc,sumcy,sums,sumsh,
		sumsy,swtau,wtau,xave,yy,arg,wtemp;
	double AverageRR, VarianceRR, VarianceTMP = 0;
	wi.resize(NrOfRR);
	wpi.resize(NrOfRR);
	wpr.resize(NrOfRR);
	wr.resize(NrOfRR);
	int nout=int(0.5*OversamplingFactor*HighestFrequency*NrOfRR);

	//iterators for RRBuffer
	RRBegin = this->DBHandler.nextRR->getRR().begin();
	RREnd = this->DBHandler.nextRR->getRR().end();
	EndRR = this->DBHandler.nextRR->getRR().end();

	//Resize LombBuffer if needed
	if (np < nout) {
		this->DBHandler.currentLomb->getLomb().resize(nout);
	}

	//Loop through RRBuffer to calculate abscissas and create overlap
	for(RRBegin = this->DBHandler.nextRR->getRR().begin(); RRBegin < RREnd; RRBegin++){
		RRTotalTMP += RRBegin->RRInterval;
		RRBegin->RRTotal= RRTotalTMP;
		if(RRBegin->RRTotal >= OneMinute && OverlapCreated == false){
			NextHRVMarker = RRBegin-1;
			this->DBHandler.currentRR->getRR().insert(this->DBHandler.currentRR->getRR().begin(), NextHRVMarker, RREnd);
			ESP_LOGI("DataProcessor", "insert last four minutes in other buffer");
			OverlapCreated = true;
		}
	}

	RRBegin = this->DBHandler.nextRR->getRR().begin();

	RRIntervalFront = this->DBHandler.nextRR->getRR().front().RRInterval;
	RRIntervalBack = this->DBHandler.nextRR->getRR().back().RRInterval;
	RRTotalFront =	this->DBHandler.nextRR->getRR().front().RRTotal;
	RRTotalBack = 	this->DBHandler.nextRR->getRR().back().RRTotal;

	//Get range of abscissas
	RRRange = (RRTotalBack - RRTotalFront);

	RRBegin = this->DBHandler.nextRR->getRR().begin();
	AverageRR = RRTotalBack/NrOfRR;
	ESP_LOGI("DataProcessor", "Total RR %d", RRTotalBack);
	ESP_LOGI("DataProcessor", "NRofRR RR %d", NrOfRR);
	ESP_LOGI("DataProcessor", "Average RR %f", AverageRR);

	//Calculate variance
	while(RRBegin != RREnd){
		VarianceTMP += pow((RRBegin->RRInterval-AverageRR),2);
		RRBegin++;
	}
	VarianceRR = VarianceTMP/NrOfRR;
	ESP_LOGI("DataProcessor", "Variance RR %f", VarianceRR);

	if (VarianceRR == 0.0){
		ESP_LOGE("DataProcessor::period", "zero variance in period");
	}
	RRBegin = this->DBHandler.nextRR->getRR().begin();

	xave=0.5*(RRTotalBack+RRTotalFront);
	pymax=0.0;
	CurrentFrequency=1.0/(RRRange*OversamplingFactor);
	for (j=0;j<NrOfRR;j++, RRBegin++) {
		arg=TWOPI*((RRBegin->RRTotal-xave)*CurrentFrequency);
		wpr.at(j)= -2.0*SQR(sin(0.5*arg));
		wpi.at(j)=sin(arg);
		wr.at(j)=cos(arg);
		wi.at(j)=wpi.at(j);
	}

	RRBegin = this->DBHandler.nextRR->getRR().begin();

	for (i=0;i<nout;i++) {
		LombData.Frequency=CurrentFrequency;
		sumsh=sumc=0.0;
		for (j=0;j<NrOfRR;j++) {
			c=wr.at(j);
			s=wi.at(j);
			sumsh += s*c;
			sumc += (c-s)*(c+s);
		}
		wtau=0.5*atan2(2.0*sumsh,sumc);
		swtau=sin(wtau);
		cwtau=cos(wtau);
		sums=sumc=sumsy=sumcy=0.0;
		for (j=0;j<NrOfRR;j++, RRBegin++) {
			s=wi.at(j);
			c=wr.at(j);
			ss=s*cwtau-c*swtau;
			cc=c*cwtau+s*swtau;
			sums += ss*ss;
			sumc += cc*cc;
			yy=RRBegin->RRInterval-AverageRR;
			sumsy += yy*ss;
			sumcy += yy*cc;
			wr.at(j)=((wtemp=wr.at(j))*wpr.at(j)-wi.at(j)*wpi.at(j))+wr.at(j);
			wi.at(j)=(wi.at(j)*wpr.at(j)+wtemp*wpi.at(j))+wi.at(j);
		}
		LombData.LombValue=0.5*(sumcy*sumcy/sumc+sumsy*sumsy/sums)/VarianceRR;
		this->DBHandler.storeLombData(LombData);
		ESP_LOGI("DataProcessor::period","Frequency/Lomb : %f : %f", LombData.Frequency, LombData.LombValue);
		if (LombData.LombValue >= pymax) pymax=LombData.LombValue;
		CurrentFrequency += 1.0/(OversamplingFactor*RRRange);
	}
	expy=exp(-pymax);
	effm=2.0*nout/OversamplingFactor;
	prob=effm*expy;
	if (prob > 0.01) prob=1.0-pow(1.0-expy,effm);

	this->DBHandler.swapLomb();
	xEventGroupClearBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	ESP_LOGI("DataProcessor::period","RRBufferReadyFlag cleared");
	xEventGroupSetBits(GlobalEventGroupHandle, LombBufferReadyFlag);
	ESP_LOGI("DataProcessor::period","LombBufferReadyFlag Ready");
	CalculateHRV();
}
*/


void DataProcessor::CalculateHRV(){
	ESP_LOGI("DataProcessor","Calculating HRV..");
	//CurrentLomb = this->DBHandler.nextLomb->getLomb().begin();
	EndLomb = this->DBHandler.nextLomb->getLomb().end();

	for(CurrentLomb = this->DBHandler.nextLomb->getLomb().begin(); CurrentLomb < EndLomb; CurrentLomb++ ){
		if(CurrentLomb->Frequency >= 0.04 && CurrentLomb->Frequency < 0.15){
			//ESP_LOGI("DataProcessor::CalculateHRV"," Power is %f at frequency %f", CurrentLomb->LombValue, CurrentLomb->Frequency);
			HRVSeries.LFPower += CurrentLomb->LombValue;
			//ESP_LOGI("DataProcessor::CalculateHRV"," LFPower is %f", HRVSeries.LFPower);
		}else if(CurrentLomb->Frequency >= 0.15 && CurrentLomb->Frequency <= 0.4 ){
			//ESP_LOGI("DataProcessor::CalculateHRV"," Power is %f at frequency %f", CurrentLomb->LombValue, CurrentLomb->Frequency);
			HRVSeries.HFPower += CurrentLomb->LombValue;
			//ESP_LOGI("DataProcessor::CalculateHRV"," HFPower is %f", HRVSeries.HFPower);
		}
	}
	this->DBHandler.nextLomb->clearLomb();

	HRVSeries.LFHFRatio = HRVSeries.LFPower/HRVSeries.HFPower;

	ESP_LOGI("DataProcessor","LF/HF-ratio = %f", HRVSeries.LFHFRatio);
}

void DataProcessor::CalculateStress(){
	Magnitude = 0;

}

DataProcessor::~DataProcessor(){

}
