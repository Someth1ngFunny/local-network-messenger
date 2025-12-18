#include <cstring>
#include <format>
#include <vector>
#include <iostream>

#include "lexparser.hpp"
#include "errors.hpp"

enum State {
  SEARCH,
  STRING,
  FLAG,
  WORD
};

void LexParser::pars(const char* inp, std::vector<lexem> &output) {
  State state = State::SEARCH;

  std::string cur;
  const int end = std::strlen(inp);
  for (int i = 0; i <= end; ++i) {
    switch(state) 
    {
    case State::SEARCH:
      if (std::isspace(inp[i]) || inp[i] == '\0') { continue; } 
      if (inp[i] == '"')
      {
        state = State::STRING;
        continue;
      }
      state = State::WORD;
      cur += inp[i];
      break;
    case State::WORD:
      if (std::isspace(inp[i]) || inp[i] == '\0' || inp[i] == '"')
      { 
        output.push_back(lexem{cur, LexType::word});
        cur.clear();
        if (inp[i] == '"') {
          state = State::STRING;
        } else {
          state = State::SEARCH;
        }
        continue;
      }
      cur += inp[i];
      break;
    case State::STRING:
      if (inp[i] == '"')
      {
        output.push_back(lexem{cur, string});
        cur.clear();
        state = State::SEARCH;
        continue;
      }
      if (inp[i] == '\0')
      {
        throw InvalidStringUnclosedQuotes(std::format("\"{}", cur));
      }
      cur += inp[i];
      break;
    }
  }
}