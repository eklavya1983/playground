cmake_minimum_required(VERSION 2.8)
project("data")

# External dependency include directories
include_directories(.)

add_executable(dataacess DataAcessService.cpp)
target_link_libraries(dataacess actor ${COMMON_LIBS})
