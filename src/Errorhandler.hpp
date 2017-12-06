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
enum class ERROR_TYPE{
   ERROR_INIT,
   ERROR_GENERIC,
   ERROR_CRITICAL,
};
class BaseError{
public:
   BaseError(unsigned int errorCode) : errorCode{errorCode}{};
   ~BaseError(){};
   //friend class Errorhandler;
   virtual void HandleError() = 0;
   virtual std::string Msg() = 0;
   //std::string log();
   //void log();
   
   unsigned int GetError()   {
      return errorCode;
   }
   ERROR_TYPE err_type = ERROR_TYPE::ERROR_GENERIC;
protected:
   unsigned int errorCode = 0;
private:
   
};
class Errorhandler : public BaseTask  {
public:
   static Errorhandler& getInstance();
   // Errors that occur during initialization of the system.
   void ErrorInit(BaseError* error);
   // Errors that occur during runtime and are non critical.
   void Error(BaseError* error);
   // Errors that are of critical nature and cause system malfunction.
   void ErrorCritical(BaseError* error);
   void AddError(BaseError* error);
   friend void ErrorhandlerTask(void* args);

   Errorhandler(Errorhandler const&)    = delete;
   void operator=(Errorhandler const&)  = delete;
protected:
   void main_task() override;
private:
   Errorhandler(unsigned int task_priority = 0) : BaseTask{task_priority} {}
   //bool attempt_fix_mpu();
   ~Errorhandler() {}
   //std::array<BaseError*, 10> InitErrors;
   std::vector<BaseError*> InitErrors;
   std::vector<BaseError*> GenericErrors;
   std::vector<BaseError*> CriticalErrors;
   std::vector<BaseError*> errors;

   int index = 0;
   //void add_error(Base_Error& error);
   void log(BaseError& error);

   //Errorhandler(Errorhandler const&);   // Don't Implement
   //void operator=(Errorhandler const&); // Don't implement

};