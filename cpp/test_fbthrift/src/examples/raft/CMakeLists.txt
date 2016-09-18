cmake_minimum_required(VERSION 2.8)
project("config")

# External dependency include directories
include_directories(.)

#add_subdirectory(test)

add_executable(configservice ConfigService.cpp)
target_link_libraries(configservice actor ${COMMON_LIBS})
