cmake_minimum_required(VERSION 2.8)
project("zk")

include_directories(${ZOOKEEPER_INCLUDE_DIRS} .)
# Library
add_library(zk thread.cc zkadapter.cc)
