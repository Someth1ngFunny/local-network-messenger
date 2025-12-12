#ifndef COMMAND_PARSER_ERRORS_HPP
#define COMMAND_PARSER_ERRORS_HPP

#include <stdexcept>

namespace LexParser 
{
  class InvalidStringUnclosedQuotes : public std::runtime_error 
  {
    using std::runtime_error::runtime_error;
  };
};

#endif