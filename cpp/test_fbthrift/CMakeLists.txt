cmake_minimum_required(VERSION 2.8)
project("store")

# Common flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++11")

# Common variables
set(TOPDIR ${CMAKE_SOURCE_DIR})
message("topdir is: ${TOPDIR}")

# fbthrift related common variables
set(FBTHRIFT_INCLUDE_DIRS
	${TOPDIR}/fbthrift
	${TOPDIR}/fbthrift/thrift/folly)
set(FBTHRIFT_LIB_DIRS
	${TOPDIR}/fbthrift/thrift/lib/cpp/.libs
	${TOPDIR}/fbthrift/thrift/lib/cpp2/.libs
	${TOPDIR}/fbthrift/thrift/folly/folly/.libs)

# gtest related common variables
set(GTEST_INCLUDE_DIRS ${TOPDIR}/gtest-1.7.0/include)
set(GTEST_LIB_DIRS ${TOPDIR}/gtest-1.7.0/lib/.libs)

# Common set of libraries to link for binaries
set(COMMON_LIBS actor thriftcpp2 thrift folly glog gflags pthread boost_system double-conversion z)

# Common set of libraries to link for tests 
set(COMMON_TEST_LIBS gtest ${COMMON_LIBS})

# common include directories
include_directories(${FBTHRIFT_INCLUDE_DIRS}
	${TOPDIR}/src)

# common library directories
link_directories(${FBTHRIFT_LIB_DIRS})

add_subdirectory(src)
#add_subdirectory(config)
