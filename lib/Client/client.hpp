#ifndef LNMS_CLIENT_HPP
#define LNMS_CLIENT_HPP

#include <iostream>
#include <array>
#include <memory>

#include <asio.hpp>

#include "client_servers.hpp"

#define SEND_RECV_BUFFER 1024

class Client 
{
private:
  asio::io_context& io_context_;

public:
  enum SendOptions {
    ip_v4 = 1,
    ip_v6 = 2
  };

  Client(asio::io_context& io_context)
    : io_context_(io_context)
  {
  }

  std::shared_ptr<tcp_server> start_listening_tcp(
    unsigned int port, 
    tcp_connection::read_handler_func handle_read_func
  ) 
  {
    return std::make_shared<tcp_server>(io_context_, port, handle_read_func);
  }

  std::shared_ptr<udp_server> start_listening_udp(
    unsigned int port, 
    udp_server::read_handler_func handle_read_func
  ) 
  {
    return std::make_shared<udp_server>(io_context_, port, handle_read_func);
  }

  void send_broadcast_async(
    unsigned int port, 
    const std::string &message
  ) {
    using udp = asio::ip::udp;

    auto socket = std::make_shared<udp::socket>(io_context_);
    
    socket->open(udp::v4());
    socket->set_option(asio::socket_base::broadcast(true));

    socket->bind(udp::endpoint(udp::v4(), 0));

    udp::endpoint endpoint(asio::ip::address_v4::broadcast(), port);
    std::shared_ptr<std::string> payload = std::make_shared<std::string>(message);

    auto recv_buf = std::make_shared<std::array<char, 128>>();
    auto sender_ep = std::make_shared<udp::endpoint>();
    
    socket->async_send_to(
      asio::buffer(*payload),
      endpoint,
      [socket, payload, recv_buf, sender_ep](const std::error_code& e, size_t s){
        socket->async_receive_from(
          asio::buffer(*recv_buf),
          *sender_ep,
          [socket, payload, recv_buf, sender_ep](const std::error_code& e, size_t s){
            if (s < 3) return;
            (*recv_buf)[s] = '\0';
            std::cout << "br_receive_from: " << recv_buf->data() << std::endl;
          }
        );
      }
    );

    // std::array<char, 128> recv_buffer;
    // socket.async_receive_from(
    //   asio::buffer(recv_buffer),
    //   endpoint,
    //   [&recv_buffer](const std::error_code& e, size_t s){
    //     if (s < 3) return;
    //     recv_buffer[s] = '\0';
    //     std::cout << "br_receive_from: " << recv_buffer.data() << std::endl;
    //   }
    // );
  }

  void send_udp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = SendOptions::ip_v4 | SendOptions::ip_v6
  )
  {
    using asio::ip::udp;

    udp protocol = (options & SendOptions::ip_v6) ? udp::v6() : udp::v4();
    
    udp::resolver resolver(io_context_);
    udp::endpoint receiver_endpoint =
      *resolver.resolve(
        protocol, 
        receiver_address, 
        receiver_port
      ).begin();

    udp::socket socket(io_context_);
    socket.open(protocol);

    socket.send_to(asio::buffer(message), receiver_endpoint);
  }

  void send_tcp_message_sync(
    const std::string& receiver_address, 
    const std::string& receiver_port,
    const std::string& message,
    int options = SendOptions::ip_v4 | SendOptions::ip_v6
  )
  {
    using asio::ip::tcp;

    tcp::resolver resolver(io_context_);
    tcp::resolver::results_type endpoints;
    
    if ((options & (SendOptions::ip_v4 | SendOptions::ip_v6)) == (SendOptions::ip_v4 | SendOptions::ip_v6)) {
      endpoints = resolver.resolve(receiver_address, receiver_port);
    } else if (options & SendOptions::ip_v6) {
      endpoints = resolver.resolve(tcp::v6(), receiver_address, receiver_port);
    } else {
      endpoints = resolver.resolve(tcp::v4(), receiver_address, receiver_port);
    }
    
    tcp::socket socket(io_context_);
    asio::connect(socket, endpoints);

    std::error_code send_error;
    size_t send_len = socket.write_some(asio::buffer(message), send_error);
    if (send_error) 
    {
      if (send_error == asio::error::eof)
        return;
      throw std::system_error(send_error);
    }
  }

};


#endif