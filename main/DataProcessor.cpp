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
	CurrentR = this->DBHandler.nextR->getR().begin();
	EndR = this->DBHandler.nextR->getR().end();

	while(CurrentR != EndR){
		while(PeakHasBeenFound == false && CurrentR != EndR){
			if(CurrentR->potentialRPeak > CurrentRValue){ 		//loop through buffer until first local max is found
				//ESP_LOGW("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGW("DataProcessor"," it->potentialRPeak %d", it->potentialRPeak);
				CurrentRValue = CurrentR->potentialRPeak;
				CurrentR++;
			}else{											//found peak
				PeakHasBeenFound = true;
				FirstRPeak = (CurrentR->sampleNr-1);
				//ESP_LOGW("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGW("DataProcessor"," it->potentialRPeak %d", it->potentialRPeak);
				//ESP_LOGW("DataProcessor"," First Peak found, peak is: %d at sample %d", CurrentRValue, FirstRPeak);
				CurrentRValue = CurrentR->potentialRPeak;		//
				CurrentR++;
			}
		}

		while(PeakHasBeenFound == true && CurrentR != EndR){
			if(CurrentR->potentialRPeak > CurrentRValue){
				//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
				//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				CurrentRValue = CurrentR->potentialRPeak;
				CurrentR++;
			}else{
				SecondRPeak = (CurrentR->sampleNr-1);
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
				RRTotal += RRInterval;
				this->DBHandler.storeRRData(RRData);
				FirstRPeak = SecondRPeak;
				while(CurrentR->potentialRPeak <= CurrentRValue && CurrentR != EndR){
					CurrentRValue = CurrentR->potentialRPeak;
					CurrentR++;
					//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
					//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
				}

				if(RRTotal > FiveMinutes){
					this->DBHandler.swapRR();
					xEventGroupSetBits(GlobalEventGroupHandle, RRBufferReadyFlag);
					ESP_LOGI("DataProcessor","RRBufferReadyFlag set");
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

void DataProcessor::fasper(){
	ESP_LOGI("DataProcessor", "Calculate Lomd Periodogram");

	pLomb = this->DBHandler.currentLomb;

	const int MACC=4;
	int NrOfRR 	= this->DBHandler.nextRR->getRR().size();
	int np 		= this->DBHandler.currentLomb->getLomb().size();
	int nout 	= int(0.5 * OversamplingFactor * HighestFrequency * NrOfRR);
	int nfreqt 	= int(OversamplingFactor * HighestFrequency * NrOfRR * MACC);
	int nfreq	= 64;
	int j, k, nwk;
	double AverageRR,ck,ckk,cterm,cwt,den,df,effm,expy,fac,fndim,hc2wt,hs2wt,
		hypo,pmax,sterm,swt,VarianceTMP,VarianceRR,RRRange,LastRR,FirstRR;

	CurrentRR = this->DBHandler.nextRR->getRR().begin();
	EndRR = this->DBHandler.nextRR->getRR().end();

	while (nfreq < nfreqt){
		nfreq <<= 1;
	}

	nwk = nfreq << 1;

	if (np < nout) {
		this->DBHandler.currentLomb->getLomb().resize(nout);
	}

	CurrentRRHRVMarker = this->DBHandler.currentRR->getRR().begin();

	//Calculate ordinates and place marker before 1 minuten mark
	while(CurrentRR != EndRR){
		CurrentRR->RRTotal += CurrentRR->RRInterval;
		if(CurrentRR->RRTotal >= OneMinute){
			NextHRVMarker = CurrentRR-1;
			this->DBHandler.currentRR->getRR().insert(CurrentRRHRVMarker, NextHRVMarker, EndRR);
		}
		CurrentRR++;
	}
	CurrentRR = this->DBHandler.nextRR->getRR().begin();

	AverageRR = this->DBHandler.nextRR->getRR().back() / NrOfRR;

	//Calculate variance
	while(CurrentRR != EndRR){
		VarianceTMP += (CurrentRR->RRInterval-AverageRR)^2;
		CurrentRR++;
	}
	CurrentRR = this->DBHandler.nextRR->getRR().begin();
	VarianceRR = VarianceTMP/NrOfRR;

	if (VarianceRR == 0.0){
		ESP_LOGE("DataProcessor::fasper", "zero variance in fasper");
	}

	FirstRR = *this->DBHandler.nextRR->getRR().begin();
	LastRR = this->DBHandler.nextRR->getRR().back();
	RRRange = this->DBHandler.nextRR->getRR().size();

	Workspace1.clear();
	Workspace2.clear();

	fac=nwk/(NrOfRR*OversamplingFactor);
	fndim=nwk;

	while(CurrentRR != EndRR){
		ck=fmod((CurrentRR->RRTotal-FirstRR)*fac,fndim);
		ckk=2.0*(ck++);
		ckk=fmod(ckk,fndim);
		++ckk;
		spread(CurrentRR->RRInterval-AverageRR, Workspace1, ck, MACC);
		spread(1.0, Workspace2, ckk, MACC);
		CurrentRR++;
	}

	realft(Workspace1, 1);
	realft(Workspace2, 1);

	df=1.0/(RRRange*OversamplingFactor);
	pmax =-1.0;

	for (pWorkspace1, pWorkspace2, j=0; j<nout; j++, pLomb++, pWorkspace1 += 2, pWorkspace2 += 2) {
		hypo=sqrt((*pWorkspace2*(*pWorkspace2))+(*pWorkspace2+1)*(*pWorkspace2+1));
		hc2wt=0.5*(*pWorkspace2/hypo);
		hs2wt=0.5*(*pWorkspace2+1)/hypo;
		cwt=sqrt(0.5+hc2wt);
		swt=SIGN(sqrt(0.5-hc2wt),hs2wt);
		den=0.5*NrOfRR+hc2wt*(*pWorkspace2)+hs2wt*(*pWorkspace2+1);

		cterm=SQR(cwt*(*pWorkspace1)+swt*(*pWorkspace2+1))/den;
		sterm=SQR(cwt**pWorkspace2+1)-swt*(*pWorkspace1)/(NrOfRR-den);
		pLomb->Frequency = (j+1)*df;
		pLomb->LombValue = (cterm+sterm)/(2.0*VarianceRR);
	}

	this->DBHandler.swapLomb();

	xEventGroupClearBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	ESP_LOGI("DataProcessor","RRBufferReadyFlag cleared");
	xEventGroupSetBits(GlobalEventGroupHandle, LombBufferReadyFlag);
	ESP_LOGI("DataProcessor","LombBufferReadyFlag Ready");
}

void DataProcessor::spread(double y, std::vector<double> &yy, double x, int m) {
	static int nfac[11]={0,1,1,2,6,24,120,720,5040,40320,362880};
	int ihi, ilo, ix, j, nden, n=yy.size();
	double fac;

		if (m > 10){
			ESP_LOGE("DataProccesor::Spread", "factorial table too small in spread"); // @suppress("Symbol is not resolved")
		}

		ix=int(x);

		if (x == double(ix)){
			yy[ix-1] += y;
		}
		else {
			ilo=MIN(MAX(int(x-0.5*m),0),int(n-m));
			ihi=ilo+m;
			nden=nfac[m];
			fac=x-ilo-1;

		for (j=ilo+1;j<ihi;j++){
			fac *= (x-j-1);
		}

		yy[ihi-1] += y*fac/(nden*(x-ihi));

		for (j=ihi-1;j>ilo;j--) {
			nden=(nden/(j-ilo))*(j-ihi);
			yy[j-1] += y*fac/(nden*(x-j));
		}
	}
}

void DataProcessor::realft(std::vector<double> &workspace, const int isign){
	int i,i1,i2,i3,i4;
	double c1=0.5,c2,h1r,h1i,h2r,h2i,wr,wi,wpr,wpi,wtemp,theta;

		int n = workspace.size();
		theta=3.141592653589793238/DP(n>>1);
		if (isign == 1) {
			c2 = -0.5;
			four1(workspace,1);
		} else {
			c2=0.5;
			theta = -theta;
		}
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0+wpr;
		wi=wpi;
		for (i=1;i<(n>>2);i++) {
			i2=1+(i1=i+i);
			i4=1+(i3=n-i1);
			h1r=c1*(workspace.at(i1)+workspace.at(i3));
			h1i=c1*(workspace.at(i2)-workspace.at(i4));
			h2r= -c2*(workspace.at(i2)+workspace.at(i4));
			h2i=c2*(workspace.at(i1)-workspace.at(i3));
			workspace.at(i1)=h1r+wr*h2r-wi*h2i;
			workspace.at(i2)=h1i+wr*h2i+wi*h2r;
			workspace.at(i3)=h1r-wr*h2r+wi*h2i;
			workspace.at(i4)= -h1i+wr*h2i+wi*h2r;
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		if (isign == 1) {
			workspace.at(0) = (h1r=workspace.at(0))+workspace.at(1);
			workspace.at(1) = h1r-workspace.at(1);
		} else {
			workspace.at(0)=c1*((h1r=workspace.at(0))+workspace.at(1));
			workspace.at(1)=c1*(h1r-workspace.at(1));
			four1(workspace,-1);
		}
}

void DataProcessor::four1(std::vector<double> &workspace, const int isign)
{
	int n,mmax,m,j,istep,i;
	DP wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	int nn=workspace.size()/2;
	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(&workspace.at(j-1), &workspace.at(i-1));
			SWAP(&workspace.at(j), &workspace.at(i));
		}
		m=nn;
		while (m >= 2 && j > m) {
			j -= m;
			m >>= 1;
		}
		j += m;
	}
	mmax=2;
	while (n > mmax) {
		istep=mmax << 1;
		theta=isign*(6.28318530717959/mmax);
		wtemp=sin(0.5*theta);
		wpr = -2.0*wtemp*wtemp;
		wpi=sin(theta);
		wr=1.0;
		wi=0.0;
		for (m=1;m<mmax;m+=2) {
			for (i=m;i<=n;i+=istep) {
				j=i+mmax;
				tempr=wr*workspace.at(j-1)-wi*workspace.at(j);
				tempi=wr*workspace.at(j)+wi*workspace.at(j-1);
				workspace.at(j-1)=workspace.at(i-1)-tempr;
				workspace.at(j)=workspace.at(i)-tempi;
				workspace.at(i-1) += tempr;
				workspace.at(i) += tempi;
			}
			wr=(wtemp=wr)*wpr-wi*wpi+wr;
			wi=wi*wpr+wtemp*wpi+wi;
		}
		mmax=istep;
	}
}

void DataProcessor::CalculateHRV(){
	ESP_LOGI("DataProcessor","CalculateHRV");
	CurrentLomb = this->DBHandler.nextLomb->getLomb().begin();
	EndLomb = this->DBHandler.nextLomb->getLomb().end();

	for(*CurrentLomb ){

	}


}


DataProcessor::~DataProcessor(){

}

