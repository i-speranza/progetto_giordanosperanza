cmake_minimum_required(VERSION 2.7)
project(giordano_speranza CXX)
list(APPEND CMAKE_CXX_FLAGS "-O3 ${CMAKE_CXX_FLAGS}" )
add_executable(giordano_speranza main_seq.cpp)


