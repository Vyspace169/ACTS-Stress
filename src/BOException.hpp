/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#ifndef BOEXCEPTION_HPP
#define BOEXCEPTION_HPP
#include <exception>
#include <iostream>
#include <string>

class BOException: public std::exception {
  virtual const char* what() const throw()
  {
    return "A Buffer overflow exception overflow occured over one of the binaryBuffers";
  }
};
#endif
