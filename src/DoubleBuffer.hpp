/**
* @file doubleBuffer.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief switching buffer
*/
#pragma once

#include "BaseTask.hpp"
#include "BinaryBuffer.hpp"
#include "SDWriter.hpp"

class DoubleBuffer {
public:
<<<<<<< HEAD
	DoubleBuffer(SDWriter&);
	void storeData(SampleData in );
=======
	/*!
	 * \brief DoubleBuffer constructor
	 * \param SDWriter class pointer
	 *
	 * This constructor initializes all buffers and
	 * needs an SDWritter class as argument, needed
	 * for writing.
	 */
	DoubleBuffer(SDWriter&);

	/*!
	 * \brief storeData method
	 * \param in SampleData structure
	 *
	 * This method stores the data in the currently
	 * used buffer.
	 */
	void storeData(SampleData in);

	/*!
	 * \brief swap method
	 *
	 * This method swaps the ping pong buffer to the
	 * next one.
	 */
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
	void swap();

	/*!
	 * \brief emptyBuffer method
	 *
	 * This method empties the current buffer.
	 */
	void emptyBuffer();
<<<<<<< HEAD
	void writeToSd();
=======

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
	 * \brief DoubleBuffer deconstructor
	 *
	 * This method delets all buffers.
	 */
>>>>>>> 6445be6125a08d7cdadab28488266c732473b202
	~DoubleBuffer();
private:
	SDWriter &writer;
	BinaryBuffer * firstBuffer;
	BinaryBuffer * secondBuffer;
	BinaryBuffer * current;
	BinaryBuffer * next;
};
