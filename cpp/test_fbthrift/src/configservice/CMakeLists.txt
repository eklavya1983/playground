cmake_minimum_required(VERSION 3.2)
project("configservice")

include_directories(${FBTHRIFT_INCLUDE_DIRS} ${ZOOKEEPER_INCLUDE_DIRS} ${ARTIFACTS_INCLUDE_DIRS})
link_directories(${FBTHRIFT_LIB_DIRS} ${GTEST_LIB_DIRS} ${ZOOKEEPER_LIB_DIRS} ${ARTIFACTS_LIB_DIRS})

# Library
add_library(config ConfigService.cpp)
install (TARGETS config DESTINATION lib)

add_executable(configservice ConfigServiceMain.cpp)
target_link_libraries(configservice config infra zookeeper_mt rdkafka++ thriftcpp2 thrift folly ${COMMON_LIBS})

