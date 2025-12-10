#include <iostream>
#include <asio.hpp>

int main(int argc, char* argv[]) 
{
  try
  {
    asio::io_context io_context;
  }
  catch (const std::exception& error)
  {
    std::cout << error.what() << std::endl;
  }
}