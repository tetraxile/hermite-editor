cmake_minimum_required(VERSION 3.16)
project(hermite-editor VERSION 1.0 LANGUAGES ASM C CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Werror")

include_directories(lib/raylib/src)
include_directories(src)

set(SOURCES src/main.cpp src/utils.cpp src/graph.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} raylib)
