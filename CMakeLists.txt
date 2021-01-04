cmake_minimum_required(VERSION 3.17)
project(bedrock_format)

set(CMAKE_CXX_STANDARD 17)

add_library(bedrock_format src/format.cpp include/format.h)