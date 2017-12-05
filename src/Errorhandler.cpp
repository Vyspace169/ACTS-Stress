#include "Errorhandler.hpp"

Errorhandler& Errorhandler::getInstance() {
   static Errorhandler errorhandler;
   return errorhandler;
}
// Errors that occur during initialization of the system.
void InitError(Base_Error& error)   {
   add_error(error);

}
// Errors that occur during runtime and are non critical.
void Error(Base_Error& error)   {
   add_error(error);
}
// Errors that are of critical nature and cause system malfunction.
void CriticalError(Base_Error& error)  {
   add_error(error);
}

void add_error(Base_Error& error)   {
   if(index >= 9) {
      // Handle exception, otherwise array index out of bounds!
   }  
   else  {
      errors[index++] = error;
   }
   
}

void log(Base_Error& error)   {
   if(error.error_code & ERROR_INIT_TYPE) {
      ESP_LOGE("INIT_ERROR", "size: %d", buffer.size());
   }
   
}
