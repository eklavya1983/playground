cmake_minimum_required(VERSION 3.2)
project("testlib")

include_directories(${FBTHRIFT_INCLUDE_DIRS} ${ZOOKEEPER_INCLUDE_DIRS} ${ARTIFACTS_INCLUDE_DIRS})

# Library
add_library(testlib DatomBringupHelper.cpp)
#install (TARGETS infra DESTINATION lib)

# Tests
#add_subdirectory(test)
