cmake_minimum_required(VERSION 3.2)
project("configservice")

# External dependency include directories
include_directories(${THRIFT_INCLUDE_DIRS} ${ZOOKEEPER_INCLUDE_DIRS} ./gen-cpp)
link_directories(${THRIFT_LIB_DIRS} ${ZOOKEEPER_LIB_DIRS}) 

#add_subdirectory(test)

# Get generated thrift sources
#file(GLOB_RECURSE thrift_cpp_sources "gen-cpp/*.cpp")
#file(GLOB_RECURSE thrift_ignore_sources "gen-cpp/*.skeleton.cpp")
#list(REMOVE_ITEM thrift_cpp_sources ${thrift_ignore_sources})
message("Thirft sources: ${thrift_cpp_sources}")

# Executable
add_executable(ConfigService
	main.cpp
	${thrift_cpp_sources}
	)
target_link_libraries(ConfigService infra thrift ${COMMON_LIBS})
install (TARGETS ConfigService DESTINATION bin)
