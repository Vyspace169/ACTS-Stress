/**
* @file D.hpp
* @Author Jop van Buuren.
* @data 21 september, 2017
* @brief
*/
#ifndef BBEXCEPTION_HPP
#define BBEXCEPTION_HPP
#include <exception>
#include <iostream>
#include <string>

class DBException : public std::exception {
  virtual const char* what() const throw(){
    return "Couldn't write to buffer from DoubleBuffer";
  }
};
#endif
