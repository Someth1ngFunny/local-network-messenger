#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <format>
#include <logic.hpp>
#include <lexparser.hpp>

Logic GLOBAL_STATE;

std::unordered_map<std::string, void (*)(const std::vector<std::string>&)> command_handler;

void exit(const std::vector<std::string> &lexems) 
{
  throw std::runtime_error("exit");
}

void handle_uknown_command(const std::string &command) 
{
  std::cout << std::format("Uknown command: \"{}\"", command) << std::endl;
}

int main(int argc, char* argv[]) {

  command_handler["exit"] = exit;

  try
  {
    std::string str_line;
    
    auto welcome = [&str_line]()
    {
      std::cout << ">> ";
      std::getline(std::cin, str_line);
      return true;
    };

    while (welcome()) 
    {
      std::stringstream stream_line(str_line);
      
      std::vector<std::string> lexems;
      std::string lexem;
      while (stream_line >> lexem)
      {
        lexems.push_back(lexem);
      }

      if (lexems.empty()) 
      { 
        continue; 
      }
      if (!command_handler.contains(lexems[0])) 
      {
        handle_uknown_command(lexems[0]);
        continue;
      }

      (command_handler[lexems[0]])(lexems);
    }

  } 
  catch (const std::exception& e) 
  {

  }

  return 0;
}