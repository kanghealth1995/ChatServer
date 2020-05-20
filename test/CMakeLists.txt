cmake_minimum_required(VERSION 3.0)
project(main)

#配置编译选项
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -g)
ADD_DEFINITIONS(-std=c++11)
#指定搜索子目录
add_subdirectory(muduoCStest)