add_library(Logger)

option(BUILD_LOGGER_SMALL "Build light version logger" ON)

target_sources(Logger
  PRIVATE
    logger.cpp
  PUBLIC
    FILE_SET HEADERS
    FILES logger.hpp
)

target_compile_definitions(Logger
  PRIVATE
    BUILD_LOGGER_SMALL ${BUILD_LOGGER_SMALL}
)