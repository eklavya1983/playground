cmake_minimum_required(VERSION 3.2)
project("test")

# Here add_unit_test is coming actor/test/CMakefiles.txt
macro(add_unit_test name)
  add_executable(${name}Test ${name}Test.cpp ${ARGN})
  target_link_libraries(${name}Test
                        infra zookeeper_mt thriftcpp2 thrift folly ${COMMON_TEST_LIBS})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name}Test)
  install (TARGETS ${name}Test DESTINATION test)
  #add_dependencies(${name}Test all_unit_tests)
endmacro()

include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${FBTHRIFT_LIB_DIRS} ${GTEST_LIB_DIRS} ${ZOOKEEPER_LIB_DIRS})


add_unit_test(ZookeeperClient)
add_unit_test(Service)
