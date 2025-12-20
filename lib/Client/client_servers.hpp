#include <iostream>
#include <ctime>
#include <string>
#include <array>
#include <functional>

#include <asio.hpp>

#define READ_RECV_BUFFER 1024

class tcp_connection
  : public std::enable_shared_from_this<tcp_connection>
{
public:
  using pointer = std::shared_ptr<tcp_connection>;
  using read_handler_func = std::function<void(pointer, const std::error_code&, size_t)>;

  static pointer create(
    asio::io_context& io_context, 
    read_handler_func handle_read_func = nullptr
  )
  {
    return pointer(new tcp_connection(io_context, handle_read_func));
  } 

  asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  void start() 
  {
    socket_.async_read_some(
      asio::buffer(recv_buffer_),
      std::bind(
        handle_read,
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

  inline std::array<char, READ_RECV_BUFFER> get_recv_message() {
    return recv_buffer_;
  }

  inline asio::ip::tcp::endpoint get_remote_endpoint() {
    return socket_.remote_endpoint();
  }

  inline asio::ip::tcp::endpoint get_local_endpoint() {
    return socket_.local_endpoint();
  }

private:
  tcp_connection(
    asio::io_context& io_context,
    read_handler_func handle_read_func
  )
    : socket_(io_context),
      handle_read_func(handle_read_func)
  {
  }

  static void handle_read(
    tcp_connection::pointer self,
    const std::error_code& error,
    size_t bytes_transferred
  )
  {
    if (bytes_transferred != sizeof(recv_buffer_) / sizeof(recv_buffer_[0]))
      self->recv_buffer_[bytes_transferred] = '\0';
    else
      self->recv_buffer_[bytes_transferred-1] = '\0';

    if (self->handle_read_func != nullptr) {
      self->handle_read_func(self, error, bytes_transferred);
    }
  }

  void default_handle_write(
    const std::error_code& error,
    size_t bytes_transferred
  )
  {
  }

  asio::ip::tcp::socket socket_;
  std::string message_;
  std::array<char, READ_RECV_BUFFER> recv_buffer_;
  read_handler_func handle_read_func = nullptr;
};

class tcp_server 
{
public:
  tcp_server(
    asio::io_context& io_context, 
    unsigned int port,
    tcp_connection::read_handler_func handle_read_func = nullptr
  ) 
    : io_context_(io_context),
      handle_read_func(handle_read_func),
      acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) 
  {
    start_accept();
  }

  void start_accept() 
  {
    // create new socket
    tcp_connection::pointer new_connection =
      tcp_connection::create(io_context_, handle_read_func);

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

  inline asio::ip::tcp::endpoint get_local_endpoint() {
    return acceptor_.local_endpoint();
  }
    
private:
  asio::io_context& io_context_;
  asio::ip::tcp::acceptor acceptor_;
  tcp_connection::read_handler_func handle_read_func = nullptr;
};

class udp_server
{
public:

  using read_handler_func = std::function<void(udp_server*, const std::error_code&, size_t)>;

  udp_server(
    asio::io_context& io_context, 
    unsigned int port,
    udp_server::read_handler_func handle_read_func
  )
    : socket_(io_context, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
      handle_read_func(handle_read_func)
  {
    start_receive();
  }

  void send_back(const std::string &message) {
    socket_.async_send_to(
      asio::buffer(message),
      remote_endpoint_,
      std::bind(
        &udp_server::handle_send,
        this,
        asio::placeholders::error,
        asio::placeholders::bytes_transferred
      )
    ); 
    // socket_.send_to(asio::buffer(message), remote_endpoint_);
  }

  inline asio::ip::udp::endpoint get_remote_endpoint() {
    return remote_endpoint_;
  }

  inline asio::ip::udp::endpoint get_local_endpoint() {
    return socket_.local_endpoint();
  }

  inline std::array<char, READ_RECV_BUFFER> get_recv_message() {
    return recv_buffer_;
  }

private:
  void start_receive()
  {
    socket_.async_receive_from(
      asio::buffer(recv_buffer_), 
      remote_endpoint_,
      std::bind(
        &udp_server::handle_read,
        this,
        asio::placeholders::error,
        asio::placeholders::bytes_transferred
      )
    );
  }

  static void handle_read(udp_server* self, const std::error_code& error, size_t bytes_transferred)
  {
    if (bytes_transferred != sizeof(recv_buffer_) / sizeof(recv_buffer_[0]))
      self->recv_buffer_[bytes_transferred] = '\0';
    else
      self->recv_buffer_[bytes_transferred-1] = '\0';
    
    if (self->handle_read_func != nullptr) {
      self->handle_read_func(self, error, bytes_transferred);
    } else {
      self->send_back("");
    }

    self->start_receive();
  }

  static void handle_send(udp_server* self, const std::error_code& error, size_t bytes_transferred)
  {
    if (!error)
    {
      if (self->handle_send_func != nullptr)
      {
        self->handle_send_func(self, error, bytes_transferred);
      }
    }
  }

  asio::ip::udp::socket socket_;
  asio::ip::udp::endpoint remote_endpoint_;
  udp_server::read_handler_func handle_read_func = nullptr;
  udp_server::read_handler_func handle_send_func = nullptr;
  std::string message_;
  std::array<char, READ_RECV_BUFFER> recv_buffer_;
};