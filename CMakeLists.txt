cmake_minimum_required(VERSION 3.29)

project(raytrace VERSION 1.0)
# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(bitmap bitmap.cpp)
add_library(vector3 vector3.cpp)
add_executable(raytrace main.cpp)
target_link_libraries(raytrace bitmap
                               vector3)