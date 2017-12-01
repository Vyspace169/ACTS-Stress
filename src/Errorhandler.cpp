#include "Errorhandler.hpp"

Errorhandler& Errorhandler::getInstance() {
   static Errorhandler errorhandler;
   return errorhandler;
}
bool Errorhandler::attempt_fix_mpu()  {

}
void Errorhandler::setHardwareError(unsigned short error) {
   // Determine hardware error type
   PeripheralErrors |= error;
   if(error & MPU_ERROR)   {
      if(!attempt_fix_mpu())  {
         ESP_LOGE("ERROR", "Failed at attempting to fix MPU issues...");
      }
   }
}
void Errorhandler::setSoftwareError(unsigned short error)  {
   // Determine software error type
   SystemErrors |= error;
   if(error & SBUFFER_SWAP_ERROR) {
      // Handle software buffer error
      SystemErrorState |= CRITICAL_ERROR_RANGE;
   }

   if(SystemErrorState &= CRITICAL_ERROR_RANGE) {
      // send interrupt
   }
}