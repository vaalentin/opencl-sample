project(opencl CXX C)

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -std=c++11")

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake)

find_package(OPENCL REQUIRED)

add_library(core
  ./src/App.cpp)

add_executable(main
  ./src/main.cpp)

include_directories(
  ${OPENCL_INCLUDE_DIR})

target_link_libraries(main
  PUBLIC core
  PUBLIC ${OPENCL_LIBRARIES})

add_custom_target(run
  COMMAND main
  DEPENDS main
  WORKING_DIRECTORY ${CMAKE_PROJECT_DIR})

