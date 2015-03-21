cmake_minimum_required(VERSION 2.8)
project("data")

# External dependency include directories
include_directories(.)

add_executable(dataacessservice DataAcessService.cpp)
target_link_libraries(dataacessservice actor ${COMMON_LIBS})
