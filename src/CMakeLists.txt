add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE
    main.cpp
  PRIVATE
    FILE_SET HEADERS
    FILES 
      errors.hpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    LexParser
    Logic
    Logger
)

# Single-config generator
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
  )
else()
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
  )
endif()

# TODO (think about include for IDE)
# target_include_directories(lnms PRIVATE
#   ${CMAKE_SOURCE_DIR}/lib/LexParser
#   ${CMAKE_SOURCE_DIR}/lib/Logic
#   ${CMAKE_SOURCE_DIR}/lib/Client
#   ${CMAKE_SOURCE_DIR}/lib/Logger
# )