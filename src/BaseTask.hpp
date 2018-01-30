#ifndef BASE_TASK_HPP
#define BASE_TASK_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "SystemVariables.hpp"
#include "Systemerrors.hpp"

/// @brief The base class that contains the requirements that all tasks require to operate
class BaseTask{
public:
  BaseTask(unsigned int task_priority) : task_priority{task_priority} {} ;
private:
protected:
  ~BaseTask();
  unsigned int task_priority;
  virtual void main_task() = 0;
};
#endif //BASE_TASK_HPP
