#ifndef LOGGER_HPP
#define LOGGER_HPP

#include <string>
#include <iostream>

namespace logger 
{

  #ifdef BUILD_LOGGER_SMALL
    #define logger::debug() std::cout << "[LIGHT_DEBUG]"
    #define logger::error() std::cout << "[ERROR]"
    #define logger::info()  std::cout << "[INFO]"
  #else
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
      debug& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::cout << manip;
        return *this;
      }
    };

    class error
    {
    public:
      error()
      {
        std::cout << "[ERROR] ";
      }

      template <class T>
      error &operator<<(const T &v)
      {
        std::cout << v;
        return *this;
      }
      error& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::cout << manip;
        return *this;
      }
    };

    class info
    {
    public:
      info()
      {
        std::cout << "[INFO] ";
      }

      template <class T>
      info &operator<<(const T &v)
      {
        std::cout << v;
        return *this;
      }
      info& operator<<(std::ostream& (*manip)(std::ostream&)) {
        std::cout << manip;
        return *this;
      }
    };
  #endif
};

#endif