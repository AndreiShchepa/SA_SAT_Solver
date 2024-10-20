cmake_minimum_required(VERSION 3.22)
project(NI-KOP)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

file(GLOB SOURCE_FILES "src/*.cpp")

add_executable(sasat++ ${SOURCE_FILES}
        src/solver.cpp
        src/solver.h)
