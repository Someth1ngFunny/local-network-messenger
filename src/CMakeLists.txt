add_executable(${PROJECT_NAME})

target_sources(${PROJECT_NAME}
  PRIVATE
    main.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    LexParser
    Logic
)

target_link_libraries(${PROJECT_NAME} PRIVATE asio_lib)

# Single-config generator
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
  set_target_properties(${PROJECT_NAME}
    PROPERTIES
      RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
  )
endif()