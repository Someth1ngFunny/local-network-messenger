#include <iostream>
#include <asio.hpp>

int main() {

  try
  {
    asio::io_context io_context;
  } 
  catch (const std::exception& e) 
  {

  }

  return 0;
}