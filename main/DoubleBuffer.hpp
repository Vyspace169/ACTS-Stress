/**
* @file doubleBuffer.hpp
* @data 21 september, 2017
*
* \class DoubleBuffer
*
* \brief Combination of two buffers to a ping-pong buffer
*
* This class combines two BinaryBuffer classes into a single
* class and handles all writing and reading. This class acts
* as a ping-pong buffer.
*/
#pragma once

#include "BaseTask.hpp"
#include "BinaryBuffer.hpp"
#include "SDWriter.hpp"

class DoubleBuffer {
public:
	/*!
	 * \brief DoubleBuffer constructor
	 * \param SDWriter class pointer
	 *
	 * This constructor initializes all buffers and
	 * needs an SDWritter class as argument, needed
	 * for writing.
	 */
	DoubleBuffer(SDWriter &wr);

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores the data in the currently
	 * used buffer.
	 */
	void storeData(SampleData in);

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores potential R-peaks in the
	 * currently used Rbuffer.
	 */
	void storeRData(RData in);

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores RR-intervals in the
	 * currently used buffer.
	 */
	void storeRRData(RRSeries in);

	/*!
	 * \brief storeData method
	 * \param in Lomb structure
	 *
	 * This method stores Lomb data in the
	 * currently used buffer
	 */
	void storeLombData(Lomb in);

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores RR-intervals in the
	 * currently used buffer.
	 */
	void storeHRVData(HRVData in);

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swap();

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swapR();

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swapRR();

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swapLomb();

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
	void swapHRV();

	/*!
	 * \brief emptyBuffer method
	 *
	 * This method empties the current buffer.
	 */
	void emptyBuffer();

	/*!
	 * \brief writeToSd method
	 *
	 * This method writes the current buffer to the
	 * sd card using the SDWriter class.
	 *
	 * Warning: file must be opened before calling
	 * this method.
	 */
	void writeToSd();

	/*!
	 * \brief writeToSd method
	 *
	 * This method writes the current RR buffer to the
	 * sd card using the SDWriter class.
	 *
	 * Warning: file must be opened before calling
	 * this method.
	 */
	void writeRToSd();

	/*!
	 * \brief writeToSd method
	 *
	 * This method writes the current RR buffer to the
	 * sd card using the SDWriter class.
	 *
	 * Warning: file must be opened before calling
	 * this method.
	 */
	void writeRRToSd();

	/*!
	 * \brief writeToSd method
	 *
	 * This method writes the current Lomb buffer to the
	 * sd cartd using the SDWriter class.
	 *
	 * Warning: file must be opened before calling
	 * this method.
	 */
	void writeLombToSd();

	/*!
	 * \brief writeToSd method
	 *
	 * This method writes the current HRV buffer to the
	 * sd card using the SDWriter class.
	 *
	 * Warning: file must be opened before calling
	 * this method.
	 */
	void writeHRVToSd();

	/*!
	 * \brief DoubleBuffer deconstructor
	 *
	 * This method deletes all buffers.
	 */
	~DoubleBuffer();

	BinaryBuffer * firstRBuffer;
	BinaryBuffer * secondRBuffer;
	BinaryBuffer * currentR;
	BinaryBuffer * nextR;
	BinaryBuffer * nextRR;
private:
	SDWriter &writer;
	BinaryBuffer * firstBuffer;
	BinaryBuffer * secondBuffer;
	BinaryBuffer * current;
	BinaryBuffer * next;


	//nextR is public

	BinaryBuffer * firstRRBuffer;
	BinaryBuffer * secondRRBuffer;
	BinaryBuffer * currentRR;
	//nextRR is public

	BinaryBuffer * firstLombBuffer;
	BinaryBuffer * secondLombBuffer;
	BinaryBuffer * currentLomb;
	BinaryBuffer * nextLomb;

	BinaryBuffer * firstHRVBuffer;
	BinaryBuffer * secondHRVBuffer;
	BinaryBuffer * currentHRV;
	BinaryBuffer * nextHRV;
};
