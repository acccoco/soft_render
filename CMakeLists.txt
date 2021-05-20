cmake_minimum_required(VERSION 3.17)
project(render)

set(CMAKE_CXX_STANDARD 14)

# 头文件
include_directories(include)

# .cpp 文件
file(GLOB SRC src/*.cpp)

# 编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb")


add_executable(main main.cpp ${SRC})

add_executable(test_cube test_cube.cpp ${SRC})
