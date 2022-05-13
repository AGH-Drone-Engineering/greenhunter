cmake_minimum_required(VERSION 3.12)
project(greenhunter)

set(CMAKE_CXX_STANDARD 17)

find_package(Boost REQUIRED)
message(STATUS "Boost version: ${Boost_VERSION}")

find_package(OpenCV REQUIRED)
message(STATUS "OpenCV version: ${OpenCV_VERSION}")

add_subdirectory(src)
