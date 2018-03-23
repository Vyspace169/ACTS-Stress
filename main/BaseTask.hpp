/**
* @file BaseTask.hpp
* @data 21 september, 2017
*
* \class BaseTask
*
* \brief Parent class for all Tasks/Controllers
*
* This class describes which methods and functions
* the tasks need to have. A Task will inherit from
* this class.
*
*/

#ifndef BASE_TASK_HPP
#define BASE_TASK_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "SystemVariables.hpp"
#include "Systemerrors.hpp"

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
