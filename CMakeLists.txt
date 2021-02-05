cmake_minimum_required(VERSION 3.17)
project(bedrock_format)

set(CMAKE_CXX_STANDARD 17)
set(BEDROCK_FORMAT_ENABLE_TESTING TRUE)

add_subdirectory(libraries/leveldb)
add_library(
        bedrock_format
        include/format.h
        src/format.cpp
        include/chunk.h
        src/chunk.c
        include/binary.h
        src/binary.c
        include/nbt.h
        src/nbt.c
        include/hashmap.h
)

target_include_directories(
        bedrock_format PRIVATE
        include
        libraries/leveldb/Projects/leveldb-mcpe/include
)
target_link_libraries(
        bedrock_format PRIVATE
        LevelDB-MCPE
)

if(MSVC)
        target_compile_options(bedrock_format PRIVATE /W4 /WX /wd4505)
endif()

if(BEDROCK_FORMAT_ENABLE_TESTING)
        add_custom_command(
                TARGET bedrock_format POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:LevelDB-MCPE> "${PROJECT_BINARY_DIR}"
                COMMENT "Copied DLL to test directory"
        )

        add_executable(test test/test.cpp)
        target_include_directories(test PRIVATE include)
        target_link_libraries(test PRIVATE bedrock_format)
endif()
