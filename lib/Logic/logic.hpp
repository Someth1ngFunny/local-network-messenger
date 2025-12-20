#ifndef LNMS_LOGIC_HPP
#define LNMS_LOGIC_HPP

#include <map>
#include <string>
#include <thread>
#include <memory>
#include <mutex>

#include <asio.hpp>

#include <logger.hpp>
#include <client.hpp>
#include "errors.hpp"
#include "logic_write_buffer.hpp"

class Logic 
{
private:
  asio::io_context io_context_;
  asio::executor_work_guard<asio::io_context::executor_type> work_guard_;
  std::thread io_thread_;
  Client client;

  std::shared_ptr<tcp_server> tcp_server__ = nullptr;
  std::shared_ptr<udp_server> udp_server__ = nullptr;
  std::shared_ptr<udp_server> udp_broadcast_server__ = nullptr;

  std::mutex write_mutex;
  WriteBufferAsync main_writer;
  WriteBufferAsync broadcast_echo_writer;

public:
  static constexpr const unsigned int possible_ports[10] = {
    3070, 3326, 3572, 3730, 4169, 4238, 5221, 6526, 6641, 7816
  };
  static constexpr const unsigned int possible_broadcast_ports[10] = {
    10190, 10648, 10891, 10910, 11027, 11508, 12190, 12208, 13125, 14640
  };

  Logic()
    : io_context_(),
      work_guard_(asio::make_work_guard(io_context_)),
      client(io_context_),
      main_writer(write_mutex, std::cout),
      broadcast_echo_writer(write_mutex, std::cout)
  {
    for (auto port : possible_ports) {
      try {
        tcp_server__ = client.start_listening_tcp(
            port, 
            [this](tcp_connection::pointer p, const std::error_code& e, size_t s){
              this->default_handle_tcp_message_read(p, e, s);
            }
          );
        logger::info() << "Listenning on port: " << port << std::endl;
        break;
      } catch (const asio::system_error &e) {
        if (e.code() != asio::error::address_in_use)
          throw e;
      }
    }

    for (auto port : possible_ports) {
      try {
        udp_server__ = client.start_listening_udp(
            port,
            [this](udp_server* p, const std::error_code& e, size_t s){
              this->default_handle_udp_message_read(p, e, s);
            }
          );
        logger::debug() << "UDP Listenning on port: " << port << std::endl;
        break;
      } catch (const asio::system_error &e) {
        if (e.code() != asio::error::address_in_use)
          throw e;
      }
    }

    for (auto port : possible_broadcast_ports) {
      try {
        udp_broadcast_server__ = client.start_listening_udp(
            port,
            [this](udp_server* p, const std::error_code& e, size_t s){
              this->handle_udp_broadscast_read(p, e, s);
            }
          );
        logger::debug() << "UDP Listenning on port: " << port << std::endl;
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

  void handle_udp_broadscast_read(
    udp_server* self,
    const std::error_code& error,
    size_t bytes_transferred
  ) {
    asio::ip::tcp::endpoint tcp_self_endpoint = tcp_server__->get_local_endpoint();
    asio::ip::udp::endpoint udp_self_endpoint = udp_server__->get_local_endpoint();
    
    self->send_back(std::format(
      "t:({}:{})u:({}:{})",
      tcp_self_endpoint.address().to_string(),
      tcp_self_endpoint.port(),
      tcp_self_endpoint.address().to_string(),
      tcp_self_endpoint.port()
    ));
  }

  /* void handle_udp_broadscast_send(
    udp_server* self,
    const std::error_code& error,
    size_t bytes_transferred
  ) {
    std::ostringstream oss;
    oss << std::format(
      "Found: {}\n", 
      self->get_recv_message().data()
    );
    broadcast_echo_writer.buf_write(oss.str());
  } */

  void default_handle_tcp_message_read(
    tcp_connection::pointer self,
    const std::error_code& error,
    size_t bytes_transferred
  ) 
  {
    asio::ip::tcp::endpoint sender_endpoint = self->get_remote_endpoint();

    std::ostringstream oss;
    oss << std::format(
      "{}:{}: {}\n", 
      sender_endpoint.address().to_string(), 
      sender_endpoint.port(), 
      self->get_recv_message().data()
    );
    main_writer.buf_write(oss.str());
  }

  void default_handle_udp_message_read(
    udp_server* self,
    const std::error_code& error,
    size_t bytes_transferred
  ) 
  {
    asio::ip::udp::endpoint sender_endpoint = self->get_remote_endpoint();

    std::ostringstream oss;
    oss << std::format(
      "{}:{}: {}\n", 
      sender_endpoint.address().to_string(), 
      sender_endpoint.port(), 
      self->get_recv_message().data()
    );
    main_writer.buf_write(oss.str());
  }

  void handle_user_input_start() {
    // std::cout << "handle_user_input_start" << std::endl;
    main_writer.close_write();
  }

  void handle_user_input_end() {
    // std::cout << "handle_user_input_end" << std::endl;
    main_writer.open_write();
  }

  void find_hosts() {
    for (auto port : possible_broadcast_ports) {
      client.send_broadcast_async(
        port,
        "",
        std::chrono::milliseconds(500),
        [this, port](const std::string& msg,
                    const asio::ip::udp::endpoint& from,
                    const std::error_code& ec) {
          if (ec) {
            if (ec == asio::error::timed_out) {
              
            } else {
              
            }
            return;
          }

          std::ostringstream oss;
          oss << std::format(
            "found: {} from {}:{}\n", 
            msg,
            from.address().to_string(), 
            from.port()
          );
          this->main_writer.buf_write(oss.str());
        }
      );
    }
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