/**
* @file binaryBuffer.hpp
* @data 21 september, 2017
*
* \class BinaryBuffer
*
* \brief Buffer between sensor and SPI SD write
*
* This class holds a finite amount of data and acts
* as a FiFo.
*/
#pragma once
#include <vector>

#include "SystemVariables.hpp"
#include "esp_log.h"

class BinaryBuffer{
public:
	/*!
	 * \brief BinaryBuffer constructor
	 *
	 * Clear the buffer and set state to write only.
	 */
	BinaryBuffer();

	/*!
	 * \brief readOnly method
	 *
	 * This method sets the buffer to read only mode.
	 */
	void readOnly();

	/*!
	 * \brief writeOnly method
	 *
	 * This method set the buffer to write only mode.
	 */
	void writeOnly();

	/*!
	 * \brief clear method
	 *
	 * This method clears the BinaryBuffer.
	 */
	void clear();

	/*!
	 * \brief clear method
	 *
	 * This method clears the BinaryBuffer.
	 */
	void clearR();

	/*!
	 * \brief clear method
	 *
	 * This method clears the BinaryBuffer.
	 */
	void clearRR();

	/*!
	 * \brief clear method
	 *
	 * This method clears the BinaryBuffer.
	 */
	void clearLomb();

	/*!
	 * \brief clear method
	 *
	 * This method clears the BinaryBuffer.
	 */
	void clearHRV();

	/*!
	 * \brief add method
	 * \param in SampleData structure
	 * \return bool returns flase if state is read only
	 *
	 * This method adds a SampleData strcture
	 * to the buffer.
	 */
	bool add( SampleData in );

	/*!
	 * \brief add method
	 * \param in RData structure
	 * \return bool returns false if state is read only
	 *
	 * This method adds an RData strcture
	 * to the buffer.
	 */
	bool addR( RData in );

	/*!
	 * \brief add method
	 * \param in RRSeries structure
	 * \return bool returns false if state is read only
	 *
	 * This method adds an RRSeries strcture
	 * to the buffer.
	 */
	bool addRR( RRSeries in );

	/*!
	 * \brief add method
	 * \param in RRSeries structure
	 * \return bool returns false if state is read only
	 *
	 * This method adds an RRSeries strcture
	 * to the buffer.
	 */
	bool addLomb( Lomb in );

	/*!
	 * \brief add method
	 * \param in SampleData structure
	 * \return bool returns flase if state is read only
	 *
	 * This method adds a SampleData strcture
	 * to the buffer.
	 */
	bool addHRV( HRVData in );

	/*!
	 * \brief get method
	 * \return SampleData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	const std::vector<SampleData>& getSD(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief get method
	 * \return RData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	std::vector<RData>& getR(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief get method
	 * \return RData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	std::vector<RRSeries>& getRR(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief get method
	 * \return RData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	std::vector<Lomb>& getLomb(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief get method
	 * \return SampleData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	std::vector<HRVData>& getHRV(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFull();

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFullR();

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFullRR();

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFullLomb();

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFullHRV();

	/*!
	 * \brief BinaryBuffer deconstructor
	 *
	 * Empty, not implemented.
	 */
	~BinaryBuffer();

private:
	bool readState();
	bool state;
	std::vector<SampleData> buffer;
	std::vector<RData> RBuffer;
	std::vector<RRSeries> RRBuffer;
	std::vector<Lomb> LombBuffer;
	std::vector<HRVData> HRVBuffer;
	const int BufferSize = BINARY_BUFFER_SIZE;
};
