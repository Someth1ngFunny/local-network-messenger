#ifndef LNMS_CLIENT_HPP
#define LNMS_CLIENT_HPP

#include <iostream>
#include <array>
#include <memory>

#include <asio.hpp>

#include "client_servers.hpp"

class Client 
{
private:
  asio::io_context& io_context;

public:
  enum SendOptions {
    ip_v4 = 1,
    ip_v6 = 2
  };


  Client(asio::io_context& io_context)
    : io_context(io_context)
  {
  }

  std::shared_ptr<tcp_server> start_listening_tcp(
    unsigned int port, 
    void (*handle_message_func)(tcp_connection::pointer, const std::error_code&, size_t)
  ) 
  {
    return std::make_shared<tcp_server>(io_context, port, handle_message_func);
  }

  void send_udp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = SendOptions::ip_v4 | SendOptions::ip_v6
  )
  {
    using asio::ip::udp;
    
    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint =
      *resolver.resolve(udp::v4(), receiver_address, receiver_port).begin();

    udp::socket socket(io_context);
    socket.open(udp::v4());

    std::array<char, 1> send_buf = {{ 0 }};
    socket.send_to(asio::buffer(send_buf), receiver_endpoint);
    
    std::array<char, 128> recv_buf;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(
      asio::buffer(recv_buf), 
      sender_endpoint
    );

    std::cout.write(recv_buf.data(), len);
  }

  void send_tcp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = SendOptions::ip_v4 | SendOptions::ip_v6
  )
  {
    using asio::ip::tcp;

    tcp::resolver resolver(io_context);
    tcp::resolver::results_type endpoints;
    
    if ((options & (SendOptions::ip_v4 | SendOptions::ip_v6)) == (SendOptions::ip_v4 | SendOptions::ip_v6)) {
      endpoints = resolver.resolve(receiver_address, receiver_port);
    } else if (options & SendOptions::ip_v6) {
      endpoints = resolver.resolve(tcp::v6(), receiver_address, receiver_port);
    } else {
      endpoints = resolver.resolve(tcp::v4(), receiver_address, receiver_port);
    }
    
    tcp::socket socket(io_context);
    asio::connect(socket, endpoints);

    std::error_code send_error;
    std::cout << "write start!" << std::endl;
    size_t send_len = socket.write_some(asio::buffer(message), send_error);
    if (send_error && send_error != asio::error::eof)
      throw std::system_error(send_error);
    std::cout << "write end!" << std::endl;

    std::array<char, 128> recv_buf;
    std::error_code error;
    size_t len = socket.read_some(asio::buffer(recv_buf), error);

    if (error && error != asio::error::eof)
      throw std::system_error(error);

    std::cout.write(recv_buf.data(), len);
  }

};


#endif