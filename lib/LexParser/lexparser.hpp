#ifndef COMMAND_PARSER_HPP
#define COMMAND_PARSER_HPP

#include <vector>
#include <string>

#include "errors.hpp"

namespace LexParser 
{
  enum LexType 
  {
    string,
    flag,
    word
  };
  
  struct lexem 
  {
    std::string str;
    LexType type;
  };

  void pars(const char*, std::vector<lexem>&);
};

#endif