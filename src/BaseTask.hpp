#ifndef BASE_TASK_HPP
#define BASE_TASK_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "SystemVariables.hpp"
#include "Systemerrors.hpp"

#define BIT_0	( 1 << 0 )
#define BIT_4	( 1 << 4 )

#define WIFI_PRIO 1 // priority is lower than sd writer because sd writer could possibly otherwise not write correctly
#define SD_PRIO 0
#define SENSOR_PRIO // sensor task is prio 0

/// @brief The base class that contains the requirements that all tasks require to operate
/// All Controllers that use a task should inherit from this sexy base class.
class BaseTask{
public:
   /// @brief Basetask that assigns priority
   /// param[unsigned int] task_priority Priority of task used in task scheduler 
   BaseTask(unsigned int task_priority) : task_priority{task_priority} {} ;
private:
protected:
  ~BaseTask();
  unsigned int task_priority;
  // main_task that runs in the freertos task function
  virtual void main_task() = 0;
};
#endif //BASE_TASK_HPP
