cmake_minimum_required(VERSION 3.2)
project("test")

add_custom_target(all_unit_tests)

macro(add_unit_test name)
  add_executable(${name}Test ${name}Test.cpp ${ARGN})
  target_link_libraries(${name}Test
                        ${COMMON_TEST_LIBS})
  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/${name}Test)
  add_dependencies(${name}Test all_unit_tests)
endmacro()

include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIB_DIRS})


add_unit_test(ActorSystem)
add_unit_test(Actor)
add_unit_test(ActorMsg)
add_unit_test(Behavior)
add_unit_test(ActorRequest)
add_unit_test(Future)
