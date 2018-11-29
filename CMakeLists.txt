cmake_minimum_required(VERSION 2.8.12)
project(VideoCapturingTool)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} ${CMAKE_CURRENT_SOURCE_DIR}/include/)

add_executable(VideoCapturingTool include/MotionDetector.hpp sources/MotionDetector.cpp sources/main.cpp)

target_link_libraries(VideoCapturingTool ${OpenCV_LIBS})