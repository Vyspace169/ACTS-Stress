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
	ESP_LOGW("DataProcessor"," Calculate RR Interval")
	it = this->DBHandler.nextR->getR().begin();
	end = this->DBHandler.nextR->getR().end();

	while(it != end){
		while(PeakHasBeenFound == false && it != end){
			if(it->potentialRPeak > CurrentRValue){ 		//loop through buffer until first local max is found
				//ESP_LOGW("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGW("DataProcessor"," it->potentialRPeak %d", it->potentialRPeak);
				CurrentRValue = it->potentialRPeak;
				it++;
			}else{											//found peak
				PeakHasBeenFound = true;
				FirstRPeak = (it->sampleNr-1);
				//ESP_LOGW("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGW("DataProcessor"," it->potentialRPeak %d", it->potentialRPeak);
				//ESP_LOGW("DataProcessor"," First Peak found, peak is: %d at sample %d", CurrentRValue, FirstRPeak);
				CurrentRValue = it->potentialRPeak;		//
				it++;
			}
		}

		while(PeakHasBeenFound == true && it != end){
			if(it->potentialRPeak > CurrentRValue){
				//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				CurrentRValue = it->potentialRPeak;
				it++;
			}else{
				SecondRPeak = (it->sampleNr-1);
				//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				//ESP_LOGW("DataProcessor"," Second Peak found, peak is: %d at sample %d", CurrentRValue, SecondRPeak);

				if(SecondRPeak > FirstRPeak){
					RRInterval = ((SecondRPeak - FirstRPeak) * 1000) / SAMPLE_RATE_H; //calculate RR-interval is milliseconds
					//ESP_LOGI("DataProcessor"," FirstRPeak %d < SecondRPeak %d, ", FirstRPeak, SecondRPeak);
				}else{
					RRInterval = ((BINARY_BUFFER_SIZE - FirstRPeak + SecondRPeak) * 1000) / SAMPLE_RATE_H;
					//ESP_LOGI("DataProcessor"," FirstRPeak %d > SecondRPeak %d, ", FirstRPeak, SecondRPeak);
				}

				//ESP_LOGW("DataProcessor"," RR-interval found, RR-interval is: %d", RRInterval);
				RRData.RRInterval = RRInterval;
				RRData.RRTotal += RRInterval;
				this->DBHandler.storeRRData(RRData);
				FirstRPeak = SecondRPeak;
				while(it->potentialRPeak <= CurrentRValue && it != end){
					CurrentRValue = it->potentialRPeak;
					it++;
					//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
					//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				}

				if(RRData.RRTotal > 3000){
					xEventGroupSetBits(GlobalEventGroupHandle, RRBufferReadyFlag);
					ESP_LOGI("DataProcessor","RRBufferReadyFlag set");
				}

				if(it != end){
					CurrentRValue = it->potentialRPeak;
					it++;
				}
			}
		}
	}
	this->DBHandler.nextR->clearR();
	PeakHasBeenFound = false;
	xEventGroupClearBits(GlobalEventGroupHandle, RBufferReadyFlag);
	ESP_LOGI("DataProcessor","RBufferReadyFlag cleared");
}

void DataProcessor::CalculateHRV(){
	ESP_LOGI("DataProcessor","CalculateHRV");
	xEventGroupClearBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	ESP_LOGI("DataProcessor","RRBufferReadyFlag cleared");


}

/*
void LombScargle(Vec_I_DP &x, Vec_I_DP &y, const DP ofac, const DP hifac,
	Vec_O_DP &wk1, Vec_O_DP &wk2, int &nout, int &jmax, DP &prob)
{
	const int MACC=4;
	int j,k,ndim,nfreq,nfreqt;
	DP ave,ck,ckk,cterm,cwt,den,df,effm,expy,fac,fndim,hc2wt,hs2wt,
		hypo,pmax,sterm,swt,var,xdif,xmax,xmin;

	int n=x.size();
	int nwk=wk1.size();
	nout=0.5*ofac*hifac*n;
	nfreqt=ofac*hifac*n*MACC;
	nfreq=64;
	while (nfreq < nfreqt) nfreq <<= 1;
	ndim=nfreq << 1;
	if (ndim > nwk) ESP_LOGI("LombScargle", "workspaces too small in fasper");
	avevar(y,ave,var);
	if (var == 0.0) ESP_LOGI("LombScargle", "zero variance in fasper");
	xmin=x[0];
	xmax=xmin;
	for (j=1;j<n;j++) {
		if (x[j] < xmin) xmin=x[j];
		if (x[j] > xmax) xmax=x[j];
	}
	xdif=xmax-xmin;
	Vec_DP wk1_t(0.0,ndim);
	Vec_DP wk2_t(0.0,ndim);
	fac=ndim/(xdif*ofac);
	fndim=ndim;
	for (j=0;j<n;j++) {
		ck=fmod((x[j]-xmin)*fac,fndim);
		ckk=2.0*(ck++);
		ckk=fmod(ckk,fndim);
		++ckk;
		spread(y[j]-ave,wk1_t,ck,MACC);
		spread(1.0,wk2_t,ckk,MACC);
	}
	realft(wk1_t,1);
	realft(wk2_t,1);
	df=1.0/(xdif*ofac);
	pmax = -1.0;
	for (k=2,j=0;j<nout;j++,k+=2) {
		hypo=sqrt(wk2_t[k]*wk2_t[k]+wk2_t[k+1]*wk2_t[k+1]);
		hc2wt=0.5*wk2_t[k]/hypo;
		hs2wt=0.5*wk2_t[k+1]/hypo;
		cwt=sqrt(0.5+hc2wt);
		swt=SIGN(sqrt(0.5-hc2wt),hs2wt);
		den=0.5*n+hc2wt*wk2_t[k]+hs2wt*wk2_t[k+1];
		cterm=SQR(cwt*wk1_t[k]+swt*wk1_t[k+1])/den;
		sterm=SQR(cwt*wk1_t[k+1]-swt*wk1_t[k])/(n-den);
		wk1[j]=(j+1)*df;
		wk2[j]=(cterm+sterm)/(2.0*var);
		if (wk2[j] > pmax) pmax=wk2[jmax=j];
	}
	expy=exp(-pmax);
	effm=2.0*nout/ofac;
	prob=effm*expy;
	if (prob > 0.01) prob=1.0-pow(1.0-expy,effm);
}
*/

DataProcessor::~DataProcessor(){

}

