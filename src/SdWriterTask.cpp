#include "SdWriterTask.hpp"
//typedef  void (SdWriterTask::*run_sd_task)(void *args);

SdWriterTask::SdWriterTask(unsigned int task_priority, DoubleBuffer &db) : BaseTask(task_priority), DBHandle(db)  {main_task();}

void run_sd_task(void *args) {
    //SdWriterTask& sdt = (SdWriterTask& )args;
    SdWriterTask *sTask = static_cast<SdWriterTask*>(args);
    while(1)  {
        EventBits_t uxBits;
        uxBits = xEventGroupWaitBits(GlobalEventGroupHandle, SensorBufferSdReady, pdTRUE, pdFALSE, portMAX_DELAY);

        if(uxBits & SensorBufferSdReady){
            // Todo handle writing of buffer to sd card...
        	//DBHandle.writeToSd();
          sTask->DBHandle.writeToSd();

        }
        else {
            // Should not occur (only SensorBufferSdReady bit has been set)
        }
    }
}

void SdWriterTask::main_task() {
  TaskHandle_t xHandle = NULL;
  //void* args[1];
  //args[0] = (void *)&this;
  void* thisTask = static_cast<void*>(this);
  BaseType_t xReturned = xTaskCreatePinnedToCore(run_sd_task, "run_sd_task", 2048, NULL, 1, &xHandle, 0);

  if(xHandle != NULL) {
    // xHandle works
  } else {
    // Handle assignment has failed
  }

  if(xReturned == pdPASS) {
    // Task creation succesful
  }
  else  {
    // xReturned false (something went wrong!)
  }
}
