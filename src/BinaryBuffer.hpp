/**
* @file binaryBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief Buffer between sensor and SPI SD write
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
	 * \brief add method
	 * \param in SampleData structure
	 * \return bool returns flase if state is read only
	 *
	 * This method adds a SampleData strcture
	 * to the buffer.
	 */
	bool add( SampleData in );

	/*!
	 * \brief get method
	 * \return SampleData buffer pointer
	 *
	 * This method returns a pointer to the
	 * first sample of the buffer.
	 */
	const std::vector<SampleData>& get(); // should perhaps be a pointer, copy could be too slow on large scale operations?

	/*!
	 * \brief isFull method
	 * \return bool (true) if the buffer is full
	 *
	 * Returns true if the buffer is full
	 */
	bool isFull();

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
	const int BufferSize = BINARY_BUFFER_SIZE;
};
