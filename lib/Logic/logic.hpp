#ifndef LNMS_LOGIC_HPP
#define LNMS_LOGIC_HPP

#include <map>
#include <string>

#include <asio.hpp>
#include <client.hpp>

class Logic 
{
private:
  asio::io_context io_context;
  Client client;

public:
  Logic()
    : io_context(),
      client(io_context)
  {
  }

  void send_udp_message(const std::string& message, const std::string& receiver) {
    client.send_udp_message(message, receiver);
  }
};

#endif