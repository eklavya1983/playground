cmake_minimum_required(VERSION 2.8)
project("cluster")

include_directories(. ${ZOOKEEPER_INCLUDE_DIRS})

# Library
add_library(cluster ZookeeperClient.cpp)

# Tests
add_subdirectory(test)
