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
      std::string message,
      std::chrono::milliseconds timeout,
      std::function<void(const std::string&, const asio::ip::udp::endpoint&, const std::error_code&)> handler
  ) {
    using udp = asio::ip::udp;

    auto socket   = std::make_shared<udp::socket>(io_context_);
    auto payload  = std::make_shared<std::string>(std::move(message));
    auto recv_buf = std::make_shared<std::array<char, 1024>>();
    auto sender   = std::make_shared<udp::endpoint>();
    auto timer    = std::make_shared<asio::steady_timer>(io_context_);
    auto done     = std::make_shared<std::atomic_bool>(false);

    socket->open(udp::v4());
    socket->set_option(asio::socket_base::broadcast(true));
    socket->bind(udp::endpoint(udp::v4(), 0));

    udp::endpoint bcast(asio::ip::address_v4::broadcast(), port);

    timer->expires_after(timeout);
    timer->async_wait([socket, timer, handler, done](const std::error_code& ec){
      if (ec == asio::error::operation_aborted) return;
      if (done->exchange(true)) return;
      socket->cancel();
      handler("", asio::ip::udp::endpoint{}, make_error_code(asio::error::timed_out));
    });

    socket->async_send_to(asio::buffer(*payload), bcast,
      [socket, payload, recv_buf, sender, timer, handler, done](const std::error_code& se, std::size_t){
        if (se) {
          if (!done->exchange(true)) {
            timer->cancel(); 
            handler("", asio::ip::udp::endpoint{}, se);
          }
          return;
        }
        socket->async_receive_from(asio::buffer(*recv_buf), *sender,
          [socket, recv_buf, sender, timer, handler, done](const std::error_code& re, std::size_t n){
            timer->cancel(); 
            if (re == asio::error::operation_aborted) return;
            if (done->exchange(true)) return;
            if (re) { handler("", asio::ip::udp::endpoint{}, re); return; }
            std::string msg(recv_buf->data(), n);
            handler(msg, *sender, {});
          }
        );
      }
    );
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