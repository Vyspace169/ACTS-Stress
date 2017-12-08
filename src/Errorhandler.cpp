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
void Errorhandler::push_error(BaseError err)   {
   //errors.push_back(err);
   error = err;
   xEventGroupSetBits(GlobalEventGroupHandle, SystemErrorFlag);
}
void Errorhandler::ReportError(BaseError err)   {
   //Todo determine error lvl and compare with report lvl and Error_lvl to stop system
   //Todo if Error_lvl to stop system, stop task management system, write to flash or sd, show status lights?
   if((unsigned int) err.errLvl >= (unsigned int) sysErrReportLvl)   {
      // Report errors (show the msg)
      //ESP_LOGE(ERROR, err.Msg.c_str());
      err.LogMsg(ERROR);
   }
   if((unsigned int) err.errLvl >= (unsigned int) sysErrLvl)  {
      // Is critical error
      //ESP_LOGE(ERROR, err.Msg.c_str());
      SystemFailed = true;
      push_error(err);
   }
}
void Errorhandler::AddError(BaseError* error)   {
/* ERROR_TYPE err_type = error->err_type;
   switch(err_type)   {
      case ERROR_TYPE::ERROR_INIT:
      #ifdef ERROR_INIT_REPORT_ONLY 
         ESP_LOGD(DEV_TAG, error->Msg());
         return;
         #else
         ESP_LOGE(ERROR, error->Msg().c_str());
         push_error(error);
         #endif
      break;
      case ERROR_TYPE::ERROR_GENERIC:
      #ifdef ERROR_GENERIC_REPORT_ONLY 
         ESP_LOGD(DEV_TAG, error->Msg());
         return;
         #else
         ESP_LOGE(ERROR, error->Msg());
         push_error(error);
         #endif
      break;
      case ERROR_TYPE::ERROR_CRITICAL:
         #ifdef ERROR_CRITICAL_REPORT_ONLY 
         ESP_LOGE(DEV_TAG, error->Msg());
         return;
         #else
         ESP_LOGE(ERROR, error->Msg());
         push_error(error);
         #endif
      break;
   }*/
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
/*void Errorhandler::AddTaskHandle(TaskHandle_t* th)   {
   for(auto& handle : taskHandles)  {
      if(handle == th)  {
         ESP_LOGV(ERROR, "Errorhandler already contains the taskhandle");
         return; // Task already added
      }
   }
   taskHandles.push_back(th);
}*/

void Errorhandler::main_task()   {
   EventBits_t uxBits;
   const TickType_t xDelay = 1000 / portTICK_PERIOD_MS;
   while(1) {
      // wait for flag to be set
      uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, SystemErrorFlag, pdTRUE, pdFALSE, portMAX_DELAY);
      xEventGroupSetBits(GlobalEventGroupHandle, SystemErrorBit);
      vTaskDelay(xDelay);

      esp_err_t err;
      //ESP_ERROR_CHECK( err );
      nvs_handle my_handle;
      err = nvs_open("storage", NVS_READWRITE, &my_handle);
      if (err != ESP_OK) {
        printf("Error (%d) opening NVS handle!\n", err);
      } else {
         int32_t error_code = (int32_t) error.errCode; // value will default to 0, if not set yet in NVS
         err = nvs_set_i32(my_handle, "error_value", error_code);
         err = nvs_commit(my_handle);
         nvs_close(my_handle);
      

      //for(auto& err : errors) {
         //err->HandleError();
         //if(err->err_type == ERROR_TYPE::ERROR_CRITICAL) {
            // Shutdown system
            // Remove all tasks (set shutdown bits first)
            // Set shutdown bit?
            
            // Get bits back
            // Write to flash
            vTaskDelay(xDelay);
            // Shutdown tasks if still running
            ESP_LOGE(ERROR, "Error handling complete!");
            // Remove this task
            //vTaskDelete( xHandle );
         //}     
      //}
      }
   }
}
/*void Errorhandler::AddError(BaseError* error)   {

}*/
/*
void Errorhandler::log(BaseError& error)   {
   if(error.error_code & ERROR_INIT_TYPE) {
      //ESP_LOGE("INIT_ERROR", "size: %d", buffer.size());
   }
   
}*/
