#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <format>

#include <logic.hpp>
#include <lexparser.hpp>
#include <logger.hpp>
#include "errors.hpp"

Logic G_LOGIC;

void send(const std::vector<LexParser::lexem> &lexems);
void help(const std::vector<LexParser::lexem> &lexems);
void exit(const std::vector<LexParser::lexem> &lexems);
void echo(const std::vector<LexParser::lexem> &lexems);

void handle_uknown_command(const LexParser::lexem &command);

int main(int argc, char* argv[]) {
  std::unordered_map<
    std::string, 
    void (*)(const std::vector<LexParser::lexem>&)
  > command_handler;
  command_handler["help"] = help;
  command_handler["HELP"] = help;
  command_handler["exit"] = exit;
  command_handler["EXIT"] = exit;
  command_handler["echo"] = echo;
  command_handler["ECHO"] = echo;
  command_handler["send"] = send;
  command_handler["SEND"] = send;

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
      std::vector<LexParser::lexem> lexems;
      try 
      {
        LexParser::pars(str_line.data(), lexems);
      }
      catch (LexParser::InvalidStringUnclosedQuotes &e)
      {
        std::cerr 
          << "Invalid string, unclosed quotes: " 
          << e.what()
          << std::endl;
        continue;
      }
   
      if (lexems.empty()) { continue; }

      if (!command_handler.contains(lexems[0].str)) 
      {
        handle_uknown_command(lexems[0]);
        continue;
      }
      (command_handler[lexems[0].str])(lexems);
    }

  } 
  catch (const Interpreter::ExitProcess& e) 
  {
  }
  catch (const std::exception& e) 
  {
    logger::debug() << "main loop: " << e.what() << std::endl;
    logger::info() << "Unexpected error occured" << std::endl;
  }

  return 0;
}

void send(const std::vector<LexParser::lexem> &args) {
  if (args.size() == 1 || args.size() > 6) 
  {
    std::cout 
      << "send [(-u)(-t)] [(-v4)(-v6)] <address> <port> <\"message\">"
      << std::endl;
    return;
  }
  
  const int n = args.size();
  bool is_udp = true;
  int options = 0;

  for (int i = 1; i < n-3; ++i) {
    if (args[i].str == "-u")
      is_udp = true;
    else if (args[i].str == "-t")
      is_udp = false;
    else if (args[i].str == "-v4")
      options += Client::SendOptions::ip_v4;
    else if (args[i].str == "-v6")
      options += Client::SendOptions::ip_v6;
    else
      throw Interpreter::InvalidFlagArgument(args[i].str, {"-u", "-t", "-v4", "-v6"});
  }

  std::string addres  = args[n-3].str;
  std::string port    = args[n-2].str;
  std::string message = args[n-1].str;

  if (is_udp)
    G_LOGIC.send_udp_message_sync(addres, port, message, options);
  else
    G_LOGIC.send_tcp_message_sync(addres, port, message, options);
}

void help(const std::vector<LexParser::lexem> &args)
{
  std::cout 
    << "Avaialble commands are:\n" \
    "\techo - print arguments provided\n" \
    "\texit - quit the program" \
    "\thelp - print current message" \
    << std::endl;
}

void exit(const std::vector<LexParser::lexem> &args) 
{
  throw Interpreter::ExitProcess();
}

void echo(const std::vector<LexParser::lexem> &args) 
{
  const int n = args.size();
  for (int i = 1; i < n; ++i)
  {
    if (args[i].type == LexParser::LexType::string) {
      std::cout << std::format("\"{}\"", args[i].str) << " ";
    } else {
      std::cout << args[i].str << " ";
    }
  }
  std::cout << std::endl;
}

void handle_uknown_command(const LexParser::lexem &command) 
{
  std::cout << std::format("Uknown command: \"{}\"", command.str) << std::endl;
}