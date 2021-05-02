cmake_minimum_required(VERSION 3.5)

project(csv2obj LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(csv2obj main.cpp csv.h)
