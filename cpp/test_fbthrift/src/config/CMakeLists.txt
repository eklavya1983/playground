cmake_minimum_required(VERSION 2.8)
project("config")

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++11")

# External dependency include directories
include_directories(/home/nbayyana/playground/cpp/test_fbthrift/fbthrift
	/home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/folly
	/home/nbayyana/playground/cpp/test_fbthrift/src
	.)

# External dependency library directories
LINK_DIRECTORIES(/home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/lib/cpp/.libs /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/lib/cpp2/.libs /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/folly/folly/.libs)

add_executable(configservice ConfigService.cpp)
TARGET_LINK_LIBRARIES(configservice actor thriftcpp2 thrift folly glog gflags pthread boost_system double-conversion z) 
