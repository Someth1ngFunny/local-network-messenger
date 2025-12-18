#include <iostream>
#include <ctime>
#include <string>
#include <array>

#include <asio.hpp>

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>
{
public:
  typedef std::shared_ptr<tcp_connection> pointer;

  static pointer create(
    asio::io_context& io_context, 
    void (*handle_message_func)(pointer, const std::error_code&, size_t)
  )
  {
    return pointer(new tcp_connection(io_context, handle_message_func));
  } 

  asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  void start() 
  {
    socket_.async_read_some(
      asio::buffer(recv_message_),
      std::bind(
        (handle_message_func != nullptr ? 
          handle_message_func 
          : &tcp_connection::default_handle_read),
        shared_from_this(),
        asio::placeholders::error,
        asio::placeholders::bytes_transferred
      )
    );
    /* message_ = "";

    asio::async_write(
      socket_, 
      asio::buffer(message_),
      std::bind(
        &tcp_connection::default_handle_write, 
        shared_from_this(),
        asio::placeholders::error,
        asio::placeholders::bytes_transferred
      )
    ); */
  }

  inline std::array<char, 1024> get_recv_message() {
    return recv_message_;
  }

private:
  tcp_connection(
    asio::io_context& io_context,
    void (*handle_message_func)(pointer, const std::error_code&, size_t)
  )
    : socket_(io_context),
      handle_message_func(handle_message_func)
  {
  }

  static void default_handle_read(
    tcp_connection::pointer self,
    const std::error_code& error,
    size_t bytes_transferred
  )
  {
  }

  void default_handle_write(
    const std::error_code& error,
    size_t bytes_transferred
  )
  {
  }

  asio::ip::tcp::socket socket_;
  std::string message_;
  std::array<char, 1024> recv_message_;
  void (*handle_message_func)(pointer, const std::error_code&, size_t);
};

class tcp_server 
{
public:
  tcp_server(
    asio::io_context& io_context, 
    unsigned int port,
    void (*handle_message_func)(tcp_connection::pointer, const std::error_code&, size_t) = nullptr
  ) 
    : io_context_(io_context),
      handle_message_func(handle_message_func),
      acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) 
  {
    start_accept();
  }

  void start_accept() 
  {
    // create new socket
    tcp_connection::pointer new_connection =
      tcp_connection::create(io_context_, handle_message_func);

    // start listening
    // logger::debug() << "start accept!" << std::endl;
    acceptor_.async_accept(
      new_connection->socket(),
      std::bind(&tcp_server::handle_accept, this, new_connection, asio::placeholders::error)
    );
  }

  void handle_accept(
    tcp_connection::pointer new_connection,
    const std::error_code& error
  )
  {
    // std::cout << "handle accept!" << std::endl;
    if (!error)
    {
      // std::cout << "no error!" << std::endl;
      new_connection->start();
    } else {
      // std::cout << "error: " << error << std::endl;
    }
    start_accept();
  }
    
private:
  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
  void (*handle_message_func)(tcp_connection::pointer, const std::error_code&, size_t);
};

class udp_server
{
public:
  udp_server(asio::io_context& io_context)
    : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), 13))
  {
    start_receive();
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(
        asio::buffer(recv_buffer_), remote_endpoint_,
        std::bind(&udp_server::handle_receive, this,
          asio::placeholders::error));
  }

  void handle_receive(const std::error_code& error)
  {
    if (!error)
    {
      std::shared_ptr<std::string> message(
          new std::string(""));

      socket_.async_send_to(asio::buffer(*message), remote_endpoint_,
          std::bind(&udp_server::handle_send, this, message));

      start_receive();
    }
  }

  void handle_send(std::shared_ptr<std::string> /*message*/)
  {
  }

  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  std::array<char, 1> recv_buffer_;
};