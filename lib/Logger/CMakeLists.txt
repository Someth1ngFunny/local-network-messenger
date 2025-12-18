add_library(Logger INTERFACE)

target_sources(Logger
  INTERFACE
    FILE_SET HEADERS
)

if (BUILD_LOGGER_SMALL)
  target_compile_definitions(Logger
    INTERFACE
      BUILD_LOGGER_SMALL
  )
endif()