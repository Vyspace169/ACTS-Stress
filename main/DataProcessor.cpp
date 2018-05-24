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
	ESP_LOGI("DataProcessor"," Calculate RR Interval")
	CurrentR = this->DBHandler.nextR->getR().begin();
	EndR = this->DBHandler.nextR->getR().end();

	while(CurrentR != EndR){
		while(PeakHasBeenFound == false && CurrentR != EndR){
			if(CurrentR->potentialRPeak > CurrentRValue){ 		//loop through buffer until first local max is found
				//ESP_LOGI("DataProcessor"," Samplevalue : %d : Samplenumber : %d", CurrentR->potentialRPeak, CurrentR->sampleNr);
				CurrentRValue = CurrentR->potentialRPeak;
				CurrentR++;
			}else{											//found peak
				PeakHasBeenFound = true;
				FirstRPeak = (CurrentR->sampleNr-1);
				//ESP_LOGI("DataProcessor"," First Peak found, peak is: %d at sample %d", CurrentRValue, FirstRPeak);
				CurrentRValue = CurrentR->potentialRPeak;
			}
		}

		while(CurrentR->potentialRPeak <= CurrentRValue && CurrentR != EndR){
			//ESP_LOGI("DataProcessor"," Samplevalue : %d : Samplenumber : %d", CurrentR->potentialRPeak, CurrentR->sampleNr);
			CurrentRValue = CurrentR->potentialRPeak;
			CurrentR++;
			//ESP_LOGI("DataProcessor"," CurrentRValue %d", CurrentRValue);
			//ESP_LOGI("DataProcessor"," it->potentialRPeak %d at sample %d", it->potentialRPeak, it->sampleNr);
		}

		while(PeakHasBeenFound == true && CurrentR != EndR){
			if(CurrentR->potentialRPeak > CurrentRValue){
				//ESP_LOGI("DataProcessor"," Samplevalue : %d : Samplenumber : %d", CurrentR->potentialRPeak, CurrentR->sampleNr);
				CurrentRValue = CurrentR->potentialRPeak;
				CurrentR++;
			}else{
				SecondRPeak = (CurrentR->sampleNr-1);
				//ESP_LOGI("DataProcessor"," Next Peak found, peak is: %d at sample %d", CurrentRValue, SecondRPeak);

				if(SecondRPeak > FirstRPeak){
					RRInterval = ((SecondRPeak - FirstRPeak) * 1000) / SAMPLE_RATE_H; //calculate RR-interval is milliseconds
					//ESP_LOGI("DataProcessor","RBuffer hasn't been swapped: regular RR-interval calculation");
				}else{
					RRInterval = ((RBUFFER_SIZE - FirstRPeak + SecondRPeak) * 1000) / SAMPLE_RATE_H;
					//ESP_LOGI("DataProcessor","RBuffer has been swapped, alternative RR-interval calculation");
				}

				if(RRInterval <= 0.8*LastRRInterval && LastBeatWasEctopic == false){
					//ESP_LOGI("DataProcessor"," RR-interval found, RR-interval is: %d", RRInterval); // @suppress("Symbol is not resolved")
					//ESP_LOGI("DataProcessor"," Ectopic beat found, ignore this RRInterval"); // @suppress("Symbol is not resolved")
					FirstRPeak = SecondRPeak;
					LastBeatWasEctopic = true;
				} else{
					if(LastBeatWasEctopic == false){
						//ESP_LOGI("DataProcessor"," RR-interval found, RR-interval is: %d", RRInterval); // @suppress("Symbol is not resolved")
						RRData.RRInterval = RRInterval;
						LastRRInterval = RRInterval;
						RRTotal += RRInterval;
						this->DBHandler.storeRRData(RRData);
						FirstRPeak = SecondRPeak;
					} else{
						//ESP_LOGI("DataProcessor"," RR-interval found, RR-interval is: %d", RRInterval); // @suppress("Symbol is not resolved")
						//ESP_LOGI("DataProcessor"," Ectopic beat found in last RRInterval, also ignore this RRInterval"); // @suppress("Symbol is not resolved")
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

				ESP_LOGI("DataProcessor","RRTotal %d", RRTotal);

				if(RRTotal > 225000){
					this->DBHandler.swapRR();
					xEventGroupSetBits(GlobalEventGroupHandle, RRBufferReadyFlag);
					ESP_LOGI("DataProcessor","RRBufferReadyFlag set");
					RRTotal = 0;
					//fasper();
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
	//xTimerStart(HRV_1_TIMER_ID, 0 );

	ESP_LOGI("DataProcessor", "Calculating Lomb periodogram...");
	pLomb = this->DBHandler.currentLomb->getLomb().begin();
	int MACC=4;
	int NrOfRR 	= this->DBHandler.nextRR->getRR().size();
	int np 		= this->DBHandler.currentLomb->getLomb().size();
	int nout 	= int(0.5 * OversamplingFactor * HighestFrequency * NrOfRR);
	int nfreqt 	= int(OversamplingFactor * HighestFrequency * NrOfRR * MACC);
	int nfreq	= 64;
	int j, k, nwk;
	float ck,ckk = 0,cterm,cwt,den,df,effm,expy,fac,fndim,hc2wt,hs2wt,
		hypo,pmax,sterm,swt,VarianceTMP = 0,VarianceRR = 0,RRRange, AverageRR = 0;
	//iterators
	CurrentRR = this->DBHandler.nextRR->getRR().begin();
	EndRR = this->DBHandler.nextRR->getRR().end();
	//pointers
	FirstRRi = this->DBHandler.nextRR->getRR().begin();
	LastRRi = this->DBHandler.nextRR->getRR().end();
	FirstRRt = this->DBHandler.nextRR->getRR().begin();
	LastRRt = this->DBHandler.nextRR->getRR().end()-1;
	RRRange = this->DBHandler.nextRR->getRR().size();
	while (nfreq < nfreqt){
		nfreq <<= 1;
	}
	nwk = nfreq << 1;
	if (np < nout) {
		this->DBHandler.currentLomb->getLomb().resize(nout);
	}
	//CurrentRRHRVMarker = this->DBHandler.currentRR->getRR().begin();
	//Calculate ordinates and place marker before 1 minute mark
	for(CurrentRR = this->DBHandler.nextRR->getRR().begin(); CurrentRR < EndRR; CurrentRR++){
		CurrentRR->RRTotal = CurrentRR->RRInterval + (CurrentRR-1)->RRTotal;
		if(CurrentRR->RRTotal >= OneMinute && OverlapCreated == false){
			NextHRVMarker = CurrentRR-1;
			this->DBHandler.currentRR->getRR().insert(this->DBHandler.currentRR->getRR().begin(), NextHRVMarker, EndRR);
			ESP_LOGI("DataProcessor", "insert last four minutes in other buffer");
			OverlapCreated = true;
		}
	}
	CurrentRR = this->DBHandler.nextRR->getRR().begin();
	AverageRR = LastRRt->RRTotal/NrOfRR;
	ESP_LOGI("DataProcessor", "nwk %d", nwk);
	//ESP_LOGI("DataProcessor", "Total RR %f", LastRRt->RRTotal);
	//ESP_LOGI("DataProcessor", "NRofRR RR %d", NrOfRR);
	//ESP_LOGI("DataProcessor", "Average RR %f", AverageRR);
	//Calculate variance
	while(CurrentRR != EndRR){
		VarianceTMP += pow((CurrentRR->RRInterval-AverageRR),2);
		CurrentRR++;
	}
	CurrentRR = this->DBHandler.nextRR->getRR().begin();
	VarianceRR = VarianceTMP/NrOfRR;
	//ESP_LOGI("DataProcessor", "Variance RR %f", VarianceRR);
	if (VarianceRR == 0.0){
		ESP_LOGE("DataProcessor::fasper", "zero variance in fasper");
	}

	//DoubleBuffer::Workspace1.assign(nwk,0);
	//DoubleBuffer::Workspace2.assign(nwk,0);
	this->DBHandler.Workspace1.assign(nwk,0);
	this->DBHandler.Workspace2.assign(nwk,0);

	pWorkspace1 = this->DBHandler.Workspace1.begin();
	pWorkspace2 = this->DBHandler.Workspace2.begin()+2;
	fac=nwk/(NrOfRR*OversamplingFactor);
	fndim=nwk;
	while(CurrentRR != EndRR){
		ck=fmod((CurrentRR->RRTotal-FirstRRt->RRTotal)*fac,fndim);
		ckk=2.0*(ck++);
		ckk=fmod(ckk,fndim);
		++ckk;
		spread(CurrentRR->RRInterval-AverageRR, this->DBHandler.Workspace1, ck, MACC);
		spread(1.0, this->DBHandler.Workspace2, ckk, MACC);
		CurrentRR++;
	}
	realft(this->DBHandler.Workspace1, 1);
	realft(this->DBHandler.Workspace2, 1);
	df=1.0/(RRRange*OversamplingFactor);
	pmax =-1.0;
	for(j=0; j<nout; j++, pLomb++, pWorkspace1 += 2, pWorkspace2 += 2) {
		hypo=sqrt((*pWorkspace2*(*pWorkspace2))+(*pWorkspace2+1)*(*pWorkspace2+1));
		hc2wt=0.5*(*pWorkspace2/hypo);
		hs2wt=0.5*(*pWorkspace2+1)/hypo;
		cwt=sqrt(0.5+hc2wt);
		swt=SIGN(sqrt(0.5-hc2wt),hs2wt);
		den=0.5*NrOfRR+hc2wt*(*pWorkspace2)+hs2wt*(*pWorkspace2+1);
		cterm=SQR(cwt*(*pWorkspace1)+swt*(*pWorkspace2+1))/den;
		sterm=SQR(cwt**pWorkspace2+1)-swt*(*pWorkspace1)/(NrOfRR-den);
		LombData.Frequency = (j+1)*df;
		LombData.LombValue = (cterm+sterm)/(2.0*VarianceRR);
		this->DBHandler.storeLombData(LombData);
		ESP_LOGI("DataProcessorfasper","Frequency/Lomb : %f : %f", LombData.Frequency, LombData.LombValue);
	}
	this->DBHandler.swapLomb();
	xEventGroupClearBits(GlobalEventGroupHandle, RRBufferReadyFlag);
	ESP_LOGI("DataProcessor","RRBufferReadyFlag cleared");
	xEventGroupSetBits(GlobalEventGroupHandle, LombBufferReadyFlag);
	ESP_LOGI("DataProcessor","LombBufferReadyFlag Ready");
	CalculateHRV();
}

void DataProcessor::spread(float y, std::vector<float> &yy, float x, int m) {
	//ESP_LOGI("DataProcessor","Spreading data");
	static int nfac[11]={0,1,1,2,6,24,120,720,5040,40320,362880};
	int ihi, ilo, ix, j, nden, n=yy.size();
	float fac;

	//ESP_LOGI("DataProcessor:spread","yy.size %d", n);

	if (m > 10){
			ESP_LOGE("DataProccesor::Spread", "factorial table too small in spread"); // @suppress("Symbol is not resolved")
		}

		ix=static_cast<int>(x);

		if (x == static_cast<float>(ix)){
			yy.at(ix-1) += y;
		}
		else {
			ilo=MIN(MAX(int(x-0.5*m),0),int(n-m));
			ihi=ilo+m;
			nden=nfac[m];
			fac=x-ilo-1;
		for (j=ilo+1;j<ihi;j++){
			fac *= (x-j-1);
		}

		yy.at(ihi-1) += y*fac/(nden*(x-ihi));
		for (j=ihi-1;j>ilo;j--) {
			nden=(nden/(j-ilo))*(j-ihi);
			yy.at(j-1) += y*fac/(nden*(x-j));
		}
	}
}

void DataProcessor::realft(std::vector<float> &workspace, const int isign){
	int i,i1,i2,i3,i4;
	float c1=0.5,c2,h1r,h1i,h2r,h2i,wr,wi,wpr,wpi,wtemp,theta;

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

void DataProcessor::four1(std::vector<float> &workspace, const int isign)
{
	int n,mmax,m,j,istep,i;
	DP wtemp,wr,wpr,wpi,wi,theta,tempr,tempi;

	int nn=workspace.size()/2;
	n=nn << 1;
	j=1;
	for (i=1;i<n;i+=2) {
		if (j > i) {
			SWAP(workspace.at(j-1), workspace.at(i-1));
			SWAP(workspace.at(j), workspace.at(i));
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
