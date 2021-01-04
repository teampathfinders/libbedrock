cmake_minimum_required(VERSION 3.17)
project(bedrock_format)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(leveldb)
add_library(bedrock_format src/format.cpp)

target_include_directories(bedrock_format PRIVATE include leveldb/Projects/leveldb-mcpe/include)
target_link_libraries(bedrock_format PRIVATE LevelDB-MCPE)