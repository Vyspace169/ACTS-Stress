/*
 * HRVController.hpp
 *
 *  Created on: 15 apr. 2018
 *      Author: caspe
 */

#ifndef HRVCONTROLLER_HPP_
#define HRVCONTROLLER_HPP_

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

class HRVController : BaseTask  {
public:
  HRVController(unsigned int task_priority, DoubleBuffer &db, SDWriter &sdw, DataProcessor &dp);
  ~HRVController() = delete; //Should be deleted?
  friend void run_hrv_task(void *args);
private:
  DoubleBuffer &DBHandle;
  SDWriter &SDWHandle;
  DataProcessor &DPHandle;
protected:
  void main_task();
};
#endif /* MAIN_HRVCONTROLLER_HPP_ */
