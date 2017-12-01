#pragma once
/*
   Class takes care of errors/issues that come up in the system
   Should know the entire system to issue fixes to it or shutdown correctly.
*/
#include "Systemerrors.hpp"
class Errorhandler   {
public:
   &Errorhandler getInstance();
   
   void setHardwareError(unsigned short error);
   void setSoftwareError(unsigned short error);
   // Todo Remove assignment operators because errorhandler is singleton
protected:
private:
   Errorhandler();
   bool attempt_fix_mpu();
   ~Errorhandler();
};