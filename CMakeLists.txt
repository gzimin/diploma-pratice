cmake_minimum_required(VERSION 3.15)
project(test C)

set(CMAKE_C_FLAGS " ${CMAKE_C_FLAGS} -lm -fopenmp")


add_executable(test main.c)

find_package(GSL REQUIRED)    # See below (2)
target_link_libraries(test  m  GSL::gsl GSL::gslcblas)