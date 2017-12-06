#include "Errorhandler.hpp"
#define ERROR_INIT_REPORT_ONLY
//#define ERROR_GENERIC_REPORT_ONLY
//#define ERROR_CRITICAL_REPORT_ONLY
#define DEV_TAG "DEV_ERROR"
#define ERROR "ERROR"
Errorhandler& Errorhandler::getInstance() {
   static Errorhandler eh(0);
   return eh;
}

void Errorhandler::AddError(BaseError* error)   {
   ERROR_TYPE err_type = error->err_type;
   switch(err_type)   {
      case ERROR_TYPE::ERROR_INIT:
      #ifdef ERROR_INIT_REPORT_ONLY 
         //ESP_LOGD(DEV_TAG, error->Msg());
         return;
         #else
         
         ESP_LOGE(ERROR, error->Msg().c_str());

         #endif
      break;
      case ERROR_TYPE::ERROR_GENERIC:
      #ifdef ERROR_GENERIC_REPORT_ONLY 
         //ESP_LOGD(DEV_TAG, error->Msg());
         return;
         #else
         //ESP_LOGE(ERROR, error->Msg());
         #endif
      break;
      case ERROR_TYPE::ERROR_CRITICAL:
         #ifdef ERROR_CRITICAL_REPORT_ONLY 
         //ESP_LOGE(DEV_TAG, error->Msg());
         return;
         #else
         //ESP_LOGE(ERROR, error->Msg());
         #endif
      break;
   }
   //Todo set bit that task waits for
}
// Errors that occur during initialization of the system.
/*void Errorhandler::ErrorInit(BaseError* initError)   {
   #ifdef DEV_MODE_NO_BLOCK 
   ESP_LOGD(DEV_TAG, initError->Msg());
   #else
   if(InitErrors.size() >= 5) {
      // Handle exception, otherwise array index out of bounds!
   }  
   else  {
      InitErrors[InitErrors.size()] = initError;
   }
   ESP_LOGE(DEV_TAG, initError->Msg());
   #endif
}*/
// Errors that occur during runtime and are non critical.
/*void Errorhandler::Error(BaseError* error)   {
   //AddError(error);
   // Report error but do nothing special
   if(GenericErrors.size() >= 5) {
      // Handle exception, otherwise array index out of bounds!
   }  
   else  {
      GenericErrors[GenericErrors.size()] = error;
   }
}*/
// Errors that are of critical nature and cause system malfunction.
/*void Errorhandler::ErrorCritical(BaseError* criticalError)  {
   if(CriticalErrors.size() >= 5) {
      // Handle exception, otherwise array index out of bounds!
   }  
   else  {
      CriticalErrors[CriticalErrors.size()] = error;
   }
}*/

void ErrorhandlerTask(void *args)  {
   Errorhandler *eTask = static_cast<Errorhandler*>(args);
   eTask->main_task();
}
void Errorhandler::main_task()   {
      EventBits_t uxBits;
      while(1) {
         // wait for flag to be set
         uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, SystemErrorFlag, pdTRUE, pdFALSE, portMAX_DELAY);
         for(auto& err : errors) {
            err->HandleError();
            if(err->err_type == ERROR_TYPE::ERROR_CRITICAL) {
               // Shutdown system
            }
            
         }
      }

}
/*void Errorhandler::AddError(BaseError* error)   {

}*/

void Errorhandler::log(BaseError& error)   {
/*   if(error.error_code & ERROR_INIT_TYPE) {
      //ESP_LOGE("INIT_ERROR", "size: %d", buffer.size());
   }*/
   
}
