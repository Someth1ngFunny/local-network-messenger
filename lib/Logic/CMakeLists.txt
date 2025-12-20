add_library(Logic)

target_sources(Logic
  PUBLIC
    FILE_SET HEADERS
    FILES 
      logic.hpp
      errors.hpp
      logic_write_buffer.hpp
)

target_link_libraries(Logic
  PUBLIC
    asio_lib
    Client
    Logger
)