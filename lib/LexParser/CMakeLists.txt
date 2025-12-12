add_library(LexParser)

target_sources(LexParser
  PRIVATE
    lexparser.cpp

  PUBLIC
    FILE_SET HEADERS
    FILES 
      lexparser.hpp
      errors.hpp
)