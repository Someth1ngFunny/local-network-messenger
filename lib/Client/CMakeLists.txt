add_executable(Client)

target_sources(Client
  PRIVATE
    main.cpp
)

target_link_libraries(Client
  PRIVATE
    asio_lib
)

target_link_libraries(Client
  PRIVATE
    ws2_32
)