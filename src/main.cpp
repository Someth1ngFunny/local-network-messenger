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
bool G_is_command_input_open = false;

void send(const std::vector<LexParser::lexem> &lexems);
void help(const std::vector<LexParser::lexem> &lexems);
void exit(const std::vector<LexParser::lexem> &lexems);
void echo(const std::vector<LexParser::lexem> &lexems);
void close(const std::vector<LexParser::lexem> &lexems);
void find(const std::vector<LexParser::lexem> &lexems);

void handle_uknown_command(const LexParser::lexem &command);

int main(int argc, char* argv[]) {
  std::unordered_map<
    std::string, 
    void (*)(const std::vector<LexParser::lexem>&)
  > command_handler;
  command_handler["close"] = close;
  command_handler["CLOSE"] = close;
  command_handler["help"] = help;
  command_handler["HELP"] = help;
  command_handler["exit"] = exit;
  command_handler["EXIT"] = exit;
  command_handler["echo"] = echo;
  command_handler["ECHO"] = echo;
  command_handler["send"] = send;
  command_handler["SEND"] = send;
  command_handler["find"] = find;
  command_handler["FIND"] = find;

  try
  {
    std::string str_line;
    
    bool is_not_first_time = false;
    auto welcome = [&str_line, &is_not_first_time]()
    {
      // get \n for command input activation if not app first cin
      if (!G_is_command_input_open && is_not_first_time)
        std::getline(std::cin, str_line);

      if (!G_is_command_input_open) {
        G_LOGIC.handle_user_input_start();
        G_is_command_input_open = true;
      }
      std::cout << ">> ";
      std::getline(std::cin, str_line);
      if (!is_not_first_time)
        is_not_first_time = true;
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
    logger::info() << "Unexpected error occured" << std::endl;
    logger::debug() << "main loop: " << e.what() << std::endl;
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

  std::string address = args[n-3].str;
  std::string port    = args[n-2].str;
  std::string message = args[n-1].str;

  try 
  {
    if (is_udp)
      G_LOGIC.send_udp_message_sync(address, port, message, options);
    else
      G_LOGIC.send_tcp_message_sync(address, port, message, options);
  }
  catch (const asio::system_error &e)
  {
    if (e.code() == asio::error::host_not_found)
      std::cout << std::format("host ({}:{}) not found", address, port) << std::endl;
    else if (e.code() == asio::error::connection_refused)
      std::cout << std::format("host ({}:{}) refused connection", address, port) << std::endl;
    else if (e.code() == asio::error::connection_aborted)
      std::cout << std::format("host ({}:{}) aborted connection", address, port) << std::endl;
    else if (e.code() == asio::error::already_connected)
      std::cout << std::format("host ({}:{}) already connected", address, port) << std::endl;
    else
      throw e;
  }
}

void find(const std::vector<LexParser::lexem> &args)
{
  G_LOGIC.find_hosts();
}

void close(const std::vector<LexParser::lexem> &args)
{
  if (G_is_command_input_open) {
    G_LOGIC.handle_user_input_end();
    G_is_command_input_open = false;
  }
}

void help(const std::vector<LexParser::lexem> &args)
{
  std::cout 
    << "Avaialble commands are:\n" \
    "\tsend - send a message to provided host and port\n" \
    "\tclose - close command input menu (reopen via \\n)\n" \
    "\techo - print arguments provided\n" \
    "\thelp - print current message\n" \
    "\texit - quit the program" \
    << std::endl;
}

void exit(const std::vector<LexParser::lexem> &args) 
{
  throw Interpreter::ExitProcess();
}

void echo(const std::vector<LexParser::lexem> &args) 
{
  if (args.size() == 1 || args.size() > 6) 
  {
    std::cout 
      << "echo <word | string> [<word | string>]*"
      << std::endl;
    return;
  }

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