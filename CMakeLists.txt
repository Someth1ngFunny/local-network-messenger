cmake_minimum_required(VERSION 3.23)
project(lnms LANGUAGES CXX)

find_package(asio QUIET)

set(CMAKE_CXX_STANDARD 20)

# Add commands file for C++ intellisense in IDE 
# stored at "${workspaceFolder}/build/compile_commands.json"
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
# Make full paths
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_INCLUDES OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_OBJECTS  OFF)
set(CMAKE_CXX_USE_RESPONSE_FILE_FOR_LIBRARIES OFF)

# Single-config generator
if (NOT DEFINED CMAKE_CONFIGURATION_TYPES)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE)
  elseif(NOT CMAKE_BUILD_TYPE MATCHES "^(Debug|Release|RelWithDebInfo|MinSizeRel)$")
    message(FATAL_ERROR "Invalid CMAKE_BUILD_TYPE=\"${CMAKE_BUILD_TYPE}\". Valid: Debug, Release, RelWithDebInfo, MinSizeRel")
  endif()
endif()

include(FetchContent)

# link to asio whenever ptoject 
# build by conan or cmake
add_library(asio_lib INTERFACE)

if(asio_FOUND)
  # conan install
  target_link_libraries(asio_lib INTERFACE asio::asio)
else()
  # build without conan

  FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG asio-1-36-0
  )
  FetchContent_MakeAvailable(asio)

  target_include_directories(asio_lib
    INTERFACE
      ${asio_SOURCE_DIR}/asio/include
  )
  target_compile_definitions(asio_lib
    INTERFACE
      ASIO_STANDALONE
  )

  if (WIN32)
    target_link_libraries(asio_lib INTERFACE ws2_32)
  endif()
endif()

add_subdirectory(lib)
add_subdirectory(src)