cmake_minimum_required(VERSION 3.23)
project(lnms LANGUAGES CXX)

find_package(asio)

add_executable(${PROJECT_NAME} src/main.cpp)

# Single-config generator
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
  if (NOT CMAKE_BUILD_TYPE)
    
    set(CMAKE_BUILD_TYPE Release)
  elseif(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release|RelWithDebInfo|MinSizeRel)$")
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\". Valid: Debug, Release, RelWithDebInfo, MinSizeRel")
  endif()
endif()

if(asio_FOUND)
  target_link_libraries(${PROJECT_NAME} asio::asio)
  
  message(STATUS "Found asio")
else()
  include(FetchContent)

  FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-36-0
  )
  FetchContent_MakeAvailable(asio)

  if (NOT TARGET asio_lib) 
    add_library(asio_lib INTERFACE)
    target_include_directories(asio_lib
      INTERFACE
        ${asio_SOURCE_DIR}/asio/include
    )
  endif()

  # Single-config generator
  if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
    set_target_properties(${PROJECT_NAME}
      PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}"
    )
  endif()
  
  message(STATUS "asio installed via CMake")
endif()

add_subdirectory(lib)