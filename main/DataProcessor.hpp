/**
* @file DataProcessor.hpp
* @data 21 september, 2017
*
* \class DataProcessor
*
* \brief Processes data received from SensorController
*
* This class handles the data received from the sensors.
* This data is sent throug a SampleData structure. With
* the received samples a activity index will be calculated.
* This index can be read and reset through this class.
*
* This class will set the MovementTimeoutReached bit if
* the movement was lower than the set triggervalues. The
* StandbyController will handle the next actions.
*
* The movement index is calculated using the Gyro data from
* the MPU9250. The differential from this data is compared
* to a trigger. When this value is higher, the movement index
* is increased with the measued data divided by the trigger value.
* If the measured data is lower than the trigger, the timeout
* counter will be incremented.
*
*/

#pragma once


#include "nrtypes_nr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>
#include <vector>
#include <cmath>

#include "math.h"

#include "esp_log.h"

#include "MovementStack.hpp"
#include "DoubleBuffer.hpp"
#include "SystemVariables.hpp"

class  DataProcessor{
public:
	DataProcessor(DoubleBuffer & db);
	/*!
	 * \brief DataProcessor constructor
	 *
	 * This method initializes all states and
	 * variables used in this class.
	 */



    /*!
     * \brief SetTimeoutValue method
     * \param Timeout in milliseconds
     *
     * This method sets the timeout time in
     * milliseconds. The timeout counter is
     * incremented with SAMPE_TIME_MS each time
     * the HandleData method is called. When
     * this counter reaches the set value, the
     * MovementTimeoutReached bit will be set.
     */
    void SetTimeoutValue(int NewTriggerms);

    /*!
     * \brief SetTrigger method
     * \param triggerx Trigger value for x axis
     * \param triggery Trigger value for y axis
     * \param triggerz Trigger value for z axis
     *
     * This method sets the gyro trigger values. If
     * the measured gyro movment is higher than these
     * triggers the timeout will be reset.
     */
    void SetTrigger(int triggerx, int triggery, int triggerz);

    /*!
     * \brief HandleData method
     * \param NewData Structure with measured data
     *
     * The new movement data is passed to this method.
     * This method handles all data and counters.
     */
    void HandleData(SampleData NewData);

    /*!
     * \brief GetActivityData method
     * \return Current activity data value
     *
     * The measured activitydata is returned.
     */
    movement_type_t GetActivityData();

    /*!
     * \brief ResetActivityData method
     *
     * This method resets the activitydata.
     * Often this is called after reading the
     * activity data.
     */
    void ResetActivityData();

    /*!
     * \brief CalculateRRInterval method
     * \param RData structure with potential R-peaks
     *
     * Potential R-peaks are passed to this method.
     * This method determines
     */
    int DigitalFilter();

    /*!
     * \brief CalculateRRInterval method
     * \param RData structure with potential R-peaks
     *
     * Potential R-peaks are passed to this method.
     * This method determines
     */
    void CalculateRRInterval();

    void period();

    /*!
     * \brief Extirpolate data
     * \param y value
     * \param yy destination
     * \param x vector element
     * \param m actual vector elements
     *
     * Used by fasper() to extirpolate RR-intervals
     */

    void spread(float y, std::vector<float> &yy, float x, int m);
    //void spread(double y, std::vector<double> &yy, double x, int m);


    void four1(std::vector<float> &workspace, const int isign);
    //void four1(std::vector<double> &workspace, const int isign);

    void realft(std::vector<float> &workspace, const int isign);
    //void realft(std::vector<double> &workspace, const int isign);

    /*!
     * \brief FASt computation of lomb PERiodogram
     * \param RRData RRSeries structure with RR-intervals
     * \param pLomb iterator of Lomb data type
     *
     * Potential R-peaks are passed to this method.
     * This method determines
     */
    void fasper();
    void fasper2();

    void CalculateHRV();

    void CalculateStress();

    /*!
     * \brief DataProcessor deconstructor
     *
     * Empty, not implemented.
     */
    ~DataProcessor();


private:
    DoubleBuffer & DBHandler;
    int TimeoutCounter;
    int TimeoutTrigger;
    bool LastTriggerOn;
    int OldAcceleroXValue;
    int OldAcceleroYValue;
    int OldAcceleroZValue;
    movement_type_t ActivityData;
    int TriggerValueX;
    int TriggerValueY;
    int TriggerValueZ;
    float Magnitude;

    int CurrentRValue = 0;
    int FirstRPeak;
    int SecondRPeak;
    bool PeakHasBeenFound = false;

    RRSeries RRData;
    int RRInterval = 0, LastRRInterval = 0;
    int RRTotal = 0;
	std::vector<RData>::iterator CurrentR, EndR;
	bool LastBeatWasEctopic = false;

	//periode.h
	std::vector<double> wi, wpi, wpr, wr;
	std::vector<RRSeries>::iterator RRBegin, RREnd;
	Lomb LombData;

	//spread
	float y, x;
	int m;
	std::vector<float> yy;
	//std::vector<double> yy;

	//realft
	std::vector<float> workspace;
	//std::vector<double> workspace;

	//fasper
	std::vector<Lomb>::iterator pLomb, CurrentLomb, EndLomb;
	std::vector<RRSeries>::iterator CurrentRR, EndRR , NextHRVMarker, CurrentRRHRVMarker;
	std::vector<RRSeries>::iterator FirstRRi, LastRRi, FirstRRt, LastRRt;
	std::vector<float>::iterator pWorkspace1, pWorkspace2;
	//std::vector<double>::iterator pWorkspace1, pWorkspace2;
	bool OverlapCreated = false;
	//Lomb LombData;
	//HRV
	HRVData HRVSeries;

};
