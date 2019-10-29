cmake_minimum_required(VERSION 3.15)
project(mpi1)


set(CMAKE_CXX_STANDARD 14)
include_directories(/usr/include/mpi/)


add_executable(mpi1 main.cpp)

