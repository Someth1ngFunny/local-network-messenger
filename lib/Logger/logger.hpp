#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

namespace logger 
{
  class debug
  {
  public:
    debug()
    {
      #ifndef NDEBUG
        std::cout << "[DEBUG] ";
      #endif
    }

    template <class T>
    debug &operator<<(const T &v)
    {
      #ifndef NDEBUG
        std::cout << v;
      #endif
      return *this;
    }
  };
};

#endif