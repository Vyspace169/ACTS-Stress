/**
* @file MovementStack.hpp
* @data 21 september, 2017
*
* \class MovementStack
*
* \brief FiFo for sending data
*
* This class puts all activity data in
* a FIFO. Whenever the user is ready to read
* all data, the FIFO can be read using the
* GetActivityData an PopData method.
*/

#pragma once

#include "esp_log.h"
#include "SystemVariables.hpp"
#include <queue>

class  MovementStack{
public:

	/*!
	 * \brief MovementStack constructor
	 *
	 * Empty, not implemented.
	 */
    MovementStack() {}

    /*!
     * \brief GetActivityData method
     * \return Activity data from FIFO
     *
     * This method returns the activitey data from
     * the current FIFO position. It does not delete
     * this data, this must be done with the PopData
     * method.
     */
    double GetActivityData();

    /*!
     * \brief PopData method
     *
     * This method removes a data sample from the
     * FIFO.
     */
    void PopData();

    /*!
     * \brief PushData method
     * \param AcvitityData The data to be pushed in the FIFO
     *
     * The data sent as a parameter is written in
     * the FIFO.
     */
    void PushData(double ActivityData);

    /*!
     * \brief DataCount method
     * \return Data samples in FIFO
     *
     * This method returns the number of samples
     * currently available in the FIFO.
     */
    int DataCount();

    /*!
     * \brief MovementStack deconstructor
     *
     * Empty, not implemented.
     */
    ~MovementStack() {}
private:
    std::queue<double> ActivityDataQueue;
};
