cmake_minimum_required(VERSION 3.16)
project(stb_image)

set(CMAKE_CXX_STANDARD 17)

add_library(stb_image stb_image.cpp stb_image.h)
target_include_directories(stb_image PUBLIC .)

