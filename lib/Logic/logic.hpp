#ifndef LNMS_LOGIC_HPP
#define LNMS_LOGIC_HPP

#include <map>
#include <string>
#include <thread>
#include <memory>

#include <asio.hpp>

#include <logger.hpp>
#include <client.hpp>
#include "errors.hpp"

class Logic 
{
private:
  asio::io_context io_context_;
  asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
  std::thread io_thread_;
  Client client;

  std::shared_ptr<tcp_server> tcp_server__ = nullptr;

public:
  static constexpr const unsigned int possible_ports[10] = {
    3070, 3326, 3572, 3730, 4169, 4238, 5221, 6526, 6641, 7816
  };

  Logic()
    : io_context_(),
      work_guard_(asio::make_work_guard(io_context_)),
      client(io_context_)
  {
   /*  try {
    io_thread_ = std::thread([&]{
      tcp_server tcp_server__(io_context_, possible_ports[1], &Logic::default_handle_message_read);
      logger::info() << "Listenning on port: " << possible_ports[1] << std::endl;
      this->io_context_.run(); 
    });
    } catch (const std::exception &e) {
      std::cout << "port taked!" << std::endl;
    } */

    for (auto port : possible_ports) {
      try {
        tcp_server__ = client.start_listening_tcp(
            port, 
            &Logic::default_handle_message_read
          );
        logger::info() << "Listenning on port: " << port << std::endl;
        break;
      } catch (const asio::system_error &e) {
        if (e.code() != asio::error::address_in_use)
          throw e;
      }
    }
 
   if (tcp_server__ != nullptr) {
    io_thread_ = std::thread([&]{ 
      this->io_context_.run(); 
    });
   }
  }

  ~Logic() {
    work_guard_.reset();
    io_context_.stop();
    if (io_thread_.joinable()) io_thread_.join();
  }

  static void default_handle_message_read(
    tcp_connection::pointer self,
    const std::error_code& error,
    size_t bytes_transferred
  ) 
  {
    std::cout << "recv_message: " << self->get_recv_message().data() << std::endl;
  }

  void send_tcp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = Client::SendOptions::ip_v4 | Client::SendOptions::ip_v6
  ) 
  {
    client.send_tcp_message_sync(receiver_address, receiver_port, message, options);
  }

  void send_udp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = Client::SendOptions::ip_v4 | Client::SendOptions::ip_v6
  ) 
  {
    client.send_udp_message_sync(receiver_address, receiver_port, message, options);
  }

};

#endif