add_library(Logic)

target_sources(Logic
  PUBLIC
    FILE_SET HEADERS
    FILES logic.hpp
)

target_link_libraries(Logic
  PUBLIC
    Client
)