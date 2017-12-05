#pragma once
/*
   Class takes care of errors/issues that come up in the system
   Should know the entire system to issue fixes to it or shutdown correctly.

*/
#include "Systemerrors.hpp"
#include <array>
class Base_Error{
public:
   Base_Error(){};
   ~Base_Error(){};
   //friend class Errorhandler;
   handle_error() = 0;
   unsigned int get_error()   {
      return error_code;
   }
protected:
   
private:
   unsigned int error_code;
};
class Errorhandler   {
public:
   &Errorhandler getInstance();
   // Errors that occur during initialization of the system.
   void InitError(Base_Error& error);
   // Errors that occur during runtime and are non critical.
   void Error(Base_Error& error);
   // Errors that are of critical nature and cause system malfunction.
   void CriticalError(Base_Error& error);
protected:
private:
   Errorhandler();
   bool attempt_fix_mpu();
   ~Errorhandler();

   std::array<Base_Error, 10> errors;
   int index = 0;
   void add_error(Base_Error& error);
   void log(Base_Error& error);

};