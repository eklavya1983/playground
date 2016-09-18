cmake_minimum_required(VERSION 2.8)
project("configservice")

# External dependency include directories
include_directories(./gen-cpp ${ZOOKEEPER_INCLUDE_DIRS})
link_directories(${ZOOKEEPER_LIB_DIRS}) 

#add_subdirectory(test)

# Get generated thrift sources
file(GLOB_RECURSE thrift_cpp_sources "gen-cpp/*.cpp")
file(GLOB_RECURSE thrift_ignore_sources "gen-cpp/*.skeleton.cpp")
list(REMOVE_ITEM thrift_cpp_sources ${thrift_ignore_sources})
message("Thirft sources: ${thrift_cpp_sources}")

# Executable
add_executable(ConfigService
	main.cpp
	${thrift_cpp_sources}
	)
target_link_libraries(ConfigService cluster ${COMMON_LIBS})
