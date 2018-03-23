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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <queue>

#include "esp_log.h"

#include "MovementStack.hpp"
#include "DoubleBuffer.hpp"
#include "SystemVariables.hpp"

class  DataProcessor{
public:

	/*!
	 * \brief DataProcessor constructor
	 *
	 * This method initializes all states and
	 * variables used in this class.
	 */
    DataProcessor();

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
     * \brief HandleData method
     * \param NewData Structure with measured data
     *
     * The ECG data is passed to this method.
     * This method handles all R-peak thresholding
     */
    void HandleECGData(SampleData NewData);

    /*!
     * \brief CalculateRRInterval method
     * \param RData structure with potential R-peaks
     *
     * Potential R-peaks are passed to this method.
     * This method determines
     */
    void CalculateRRInterval(RData RPeaks);

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
     * \brief DataProcessor deconstructor
     *
     * Empty, not implemented.
     */
    ~DataProcessor();
private:
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
};
