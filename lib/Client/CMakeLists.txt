add_library(Client)

target_sources(Client
  PRIVATE
    client.cpp

  PUBLIC
    FILE_SET HEADERS
    FILES 
      client.hpp
      client_servers.hpp
)

target_link_libraries(Client
  PRIVATE 
    asio_lib
)