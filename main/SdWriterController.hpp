/**
* @file SdWriterController.hpp
* @data 21 september, 2017
*
* \class SdWriterController
*
* \brief Task that handles writing to SD-Card
*
* This task will write all data to the SDCard when one of the
* DoubleBuffers is full. When this happens a bit will be set and
* this task will open, write and close the used file. The filename
* is set in the main loop.
*
* If the sleepbit has been set this task will go into an infinite loop.
*
*/

#ifndef SD_WRITER_TASK_HPP
#define SD_WRITER_TASK_HPP

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "BaseTask.hpp"
#include "DoubleBuffer.hpp"
#include "DataProcessor.hpp"

#include "SystemVariables.hpp"

#include <array>

class SdWriterController : BaseTask  {
public:
  SdWriterController(unsigned int task_priority, DoubleBuffer &db, SDWriter &sdw, DataProcessor &dp);
  ~SdWriterController() = delete; //Should be deleted?
  friend void run_sd_task(void *args);
  void *HRV_1_TIMER_ID = 0;
private:
  DoubleBuffer &DBHandle;
  SDWriter &SDWHandle;
  DataProcessor &DPHandle;
protected:
  void main_task();
};
#endif //SD_WRITER_TASK_HPP
