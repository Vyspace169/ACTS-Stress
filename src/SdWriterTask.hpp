#ifndef SD_WRITER_TASK_HPP
#define SD_WRITER_TASK_HPP

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "BaseTask.hpp"
#include "DoubleBuffer.hpp"

#include "SystemVariables.hpp"

#include <array>

/// @brief SdWriterTask that handles the writing to the sd.
class SdWriterTask : BaseTask  {
// ToDo Create two buffers of 50.000 bytes each (total is ~20% of system memory)
// Buffer system should be created and ready to be used
public:
  SdWriterTask(unsigned int task_priority, DoubleBuffer &db, SDWriter &sdw);
  ~SdWriterTask() = delete; //Should be deleted?
  friend void run_sd_task(void *args);
private:
  DoubleBuffer &DBHandle;
  SDWriter &SDWHandle;
protected:
  void main_task();
};
#endif //SD_WRITER_TASK_HPP
