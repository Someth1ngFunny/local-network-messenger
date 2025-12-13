#ifndef LNMS_CLIENT_HPP
#define LNMS_CLIENT_HPP

#include <asio.hpp>

class Client 
{
private:
  asio::io_context& io_context;
public:
  Client(asio::io_context& io_context)
    : io_context(io_context)
  {
  }

  void send_udp_message(const std::string& message, const std::string& receiver)
  {
    using asio::ip::udp;
    
    udp::resolver resolver(io_context);
    udp::endpoint receiver_endpoint =
      *resolver.resolve(udp::v4(), "0.0.0.0", "daytime").begin();

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

    /* udp::resolver(*io_context);
    udp::receiver receiver_endpoints = 
      *resolver.resolve(udp::v4(), "0.0.0.0", receiver).begin();
    
    udp::socket socket((*io_context));
    socket.open(udp::v4());

    socket.send_to(
      asio::buffer(message),
      receiver_endpoints
    );

    std::string recv_buffer;
    udp::endpoint sender_endpoint;
    size_t len = socket.receive_from(
      asio::buffer(recv_buf.data()),
      sender_endpoint
    );

    std::cout.write(recv_buffer); */
  }

};


#endif