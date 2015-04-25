cmake_minimum_required(VERSION 2.8)
project("test")

#macro(add_unit_test name)
#  add_executable(test_${name} test_${name}.cpp test.cpp ${ARGN})
#  target_link_libraries(test_${name}
#                        ${LD_FLAGS}
#                        ${LIBCAF_LIBRARIES}
#                        ${PTHREAD_LIBRARIES})
#  add_test(${name} ${EXECUTABLE_OUTPUT_PATH}/test_${name})
#  add_dependencies(test_${name} all_unit_tests)
#endmacro()

include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIB_DIRS})

add_executable(actorsystem_test ActorSystemTest.cpp)
target_link_libraries(actorsystem_test ${COMMON_TEST_LIBS})

add_executable(actor_test ActorTest.cpp)
target_link_libraries(actor_test ${COMMON_TEST_LIBS})

add_executable(actormsg_test ActorMsgTest.cpp)
target_link_libraries(actormsg_test ${COMMON_TEST_LIBS})

add_executable(behavior_test BehaviorTest.cpp)
target_link_libraries(behavior_test ${COMMON_TEST_LIBS})
