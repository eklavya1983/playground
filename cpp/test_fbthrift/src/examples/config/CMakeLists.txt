cmake_minimum_required(VERSION 3.2)
project("config")

# External dependency include directories
include_directories(.)

#add_subdirectory(test)

add_executable(configservice ConfigService.cpp)
target_link_libraries(configservice actor ${COMMON_LIBS})
install(TARGETS configservice DESTINATION bin)
