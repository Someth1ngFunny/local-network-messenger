#ifndef LNMS_MAIN_ERRORS_HPP
#define LNMS_MAIN_ERRORS_HPP

#include <stdexcept>
#include <vector>
#include <string>
#include <format>

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

  class InvalidFlagArgument : public std::runtime_error 
  {
  private:
    std::string __join_str(const std::vector<std::string> &strs) 
    {
      std::string res;
      for (auto &str : strs) 
      {
        res += str;
      }
      return res;
    }
    
  public:
    InvalidFlagArgument(const std::string &arg, const std::vector<std::string> &possible_ways = {}) 
      : runtime_error(std::format("Invalid flag provided: '{}', avaialbe oprions are: {}", arg, __join_str(possible_ways))) {}
  };
};

#endif