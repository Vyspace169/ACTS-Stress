#include "Errorhandler.hpp"
#define ERROR_INIT_REPORT_ONLY
#define DEV_TAG "DEV_ERROR"
#define ERROR "ERROR"
Errorhandler& Errorhandler::getInstance() {
   static Errorhandler eh(0);
   return eh;
}
void Errorhandler::push_error(BaseError err)   {
   error = err;
   xEventGroupSetBits(GlobalEventGroupHandle, SystemErrorFlag);
}
void Errorhandler::ReportError(BaseError err)   {
   //Todo determine error lvl and compare with report lvl and Error_lvl to stop system
   //Todo if Error_lvl to stop system, stop task management system, write to flash or sd, show status lights?
   if((unsigned int) err.errLvl >= (unsigned int) sysErrReportLvl)   {
      // Report errors (show the msg)
      err.LogMsg(ERROR);
   }
   if((unsigned int) err.errLvl >= (unsigned int) sysErrLvl)  {
      // Is critical error
      SystemFailed = true;
      push_error(err);
   }
}

void ErrorhandlerTask(void *args)  {
   Errorhandler *eTask = static_cast<Errorhandler*>(args);
   eTask->main_task();
}

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

         // Shutdown system
         // Remove all tasks (set shutdown bits first)
         // Set shutdown bit?
         
         // Get bits back
         // Write to flash
         vTaskDelay(xDelay);
         // Shutdown tasks if still running
         ESP_LOGE(ERROR, "Error handling complete!");
      }
   }
}
