cmake_minimum_required(VERSION 2.7)
project(giordano_speranza CXX)

set(ENABLE_OMP On CACHE BOOL "Enable OpenMP")

if(ENABLE_OMP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")
endif()

add_executable(giordano_speranza_omp main_omp.cpp)




