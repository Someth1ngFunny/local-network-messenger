#ifndef LNMS_LOGIC_ERRORS_HPP
#define LNMS_LOGIC_ERRORS_HPP

#include <stdexcept>

namespace LogicErrors
{
  class NoPortsAvailable : public std::exception 
  {
    using std::exception::exception;
  };
};

#endif