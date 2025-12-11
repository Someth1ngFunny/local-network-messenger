add_executable(Client)

target_sources(Client
  PRIVATE
    client.cpp

  PUBLIC
    FILE_SET HEADERS
    FILES client.hpp
)