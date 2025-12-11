add_library(Logic INTERFACE)

target_sources(Logic
  INTERFACE
    FILE_SET HEADERS
    FILES logic.hpp
)