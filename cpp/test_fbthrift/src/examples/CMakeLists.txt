cmake_minimum_required(VERSION 2.8)
project("examples")

# External dependency include directories
include_directories(${GTEST_INCLUDE_DIRS} ${ZOOKEEPER_INCLUDE_DIRS} .)
link_directories(${GTEST_LIB_DIRS} ${ZOOKEEPER_LIB_DIRS}) 

#add_executable(zk ZookeeperClient.cpp zk.cpp)
#target_link_libraries(zk zookeeper_mt ${COMMON_LIBS})

add_executable(future_gtest future_test.cpp)
target_link_libraries(future_gtest ${COMMON_TEST_LIBS})
