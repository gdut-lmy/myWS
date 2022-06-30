cmake_minimum_required(VERSION 3.15)
project(myWS)

set(CMAKE_VERBOSE_MAKEFILE ON)
add_definitions("-O0 -g -ggdb -Wno-unused-variable")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build)
set(LIBRARY_OUT PUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(include include/sync)
# 加载子目录
add_subdirectory(src)
add_subdirectory(tests)