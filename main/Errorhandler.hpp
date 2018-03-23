#pragma once
/*
   Class takes care of errors/issues that come up in the system
   Should know the entire system to issue fixes to it or shutdown correctly.
*/
#include "Systemerrors.hpp"
#include <array>
#include <string>
#include <vector>
#include "esp_system.h"
#include "esp_err.h"
#include "esp_log.h"
#include "BaseTask.hpp"
#include "SystemVariables.hpp"

#include "FreeRTOS/task.h"

#include "nvs_flash.h"
#include "esp_spi_flash.h"
#include "nvs.h"

#define ERROR_REPORT_LVL 0
#define ERROR_LVL 4
enum class ERROR_TYPE : unsigned int {
   ERROR_INIT,
   ERROR_GENERIC,
   ERROR_CRITICAL,
};
enum class ErrorLevel : unsigned int {
   Warning        = 0,
   Generic        = 1,
   Init           = 2,
   Non_Continues  = 3,
   Critical       = 4,
};
enum class ErrorCode : unsigned int {
   NO_ERROR = 0x00,
};
class BaseError{
public:
   BaseError() {}
   BaseError(ErrorCode errCode, ErrorLevel errLvl, std::string errMsg) : errCode{errCode}, errLvl{errLvl}, errMsg{errMsg}  {}
   ~BaseError() {}
   ErrorCode errCode;   
   ErrorLevel errLvl;   
   std::string errMsg;  
   void LogMsg(const char* tag) {}
protected:
private:
   
};

class Errorhandler : public BaseTask  {
public:
   static Errorhandler& getInstance();
   void ReportError(BaseError err);
   // Errors that occur during initialization of the system.
   void ErrorInit(BaseError* error);
   // Errors that occur during runtime and are non critical.
   void Error(BaseError* error);
   // Errors that are of critical nature and cause system malfunction.
   void ErrorCritical(BaseError* error);
   friend void ErrorhandlerTask(void* args);

   Errorhandler(Errorhandler const&)    = delete;
   void operator=(Errorhandler const&)  = delete;
   bool SystemFailed = false;
protected:
   void main_task() override;
private:
   void push_error(BaseError err);
   Errorhandler(unsigned int task_priority = 0) : BaseTask{task_priority} {}
   ~Errorhandler() {}
   BaseError error;
   ErrorLevel sysErrReportLvl;
   ErrorLevel sysErrLvl; 

   int index = 0;
};
