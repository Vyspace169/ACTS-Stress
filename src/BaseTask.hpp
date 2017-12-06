#ifndef BASE_TASK_HPP
#define BASE_TASK_HPP

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "SystemVariables.hpp"

// -------Flags for Tasks----------------------------
// Flag for SensorTask to act upon
const EventBits_t SensorMeasurementFlag  = ( 1 << 0 );

// Flags for WifiTask to act upon
const EventBits_t WifiActivateFlag       = ( 1 << 1 );
const EventBits_t WifiReadyFlag          = ( 1 << 2 );

// Flag for SdWriterTask to act upon
const EventBits_t SensorBufferSdReady    = ( 1 << 4 );

// Flag to signal movement timeout count has been reached
const EventBits_t MovementTimeoutReached = ( 1 << 5 );

// Flags from tasks stating they are ready to sleep
const EventBits_t StandbySensorTaskUnhandled = ( 1 << 6 );
const EventBits_t StandbyWifiTaskUnhandled   = ( 1 << 7 );
const EventBits_t StandbyWriterTaskUnhandled = ( 1 << 8 );

//-------------------------------------------------

#define WIFI_PRIO 1 // priority is lower than sd writer because sd writer could possibly otherwise not write correctly
#define SD_PRIO 0
#define SENSOR_PRIO // sensor task is prio 0

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
