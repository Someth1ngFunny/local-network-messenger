add_library(LexParser)

target_sources(LexParser
  PRIVATE
    parser.cpp
  
  PUBLIC
    FILE_SET HEADERS
    FILES parser.hpp
)