#ifndef LNMS_MAIN_ERRORS_HPP
#define LNMS_MAIN_ERRORS_HPP

#include <stdexcept>

namespace Interpreter
{
  class ExitProcess : public std::exception 
  {
    using std::exception::exception;
  };

  class InvalidNumberOfArguments : public std::runtime_error 
  {
    using std::runtime_error::runtime_error;
  };

  class InvalidArgument : public std::runtime_error 
  {
    using std::runtime_error::runtime_error;
  };
};

#endif