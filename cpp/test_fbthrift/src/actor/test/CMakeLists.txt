cmake_minimum_required(VERSION 2.8)
project("test")

include_directories(${GTEST_INCLUDE_DIRS})
link_directories(${GTEST_LIB_DIRS})

add_executable(actorsystem_test ActorSystemTest.cpp)
target_link_libraries(actorsystem_test ${COMMON_TEST_LIBS})

add_executable(actormsg_test ActorMsgTest.cpp)
target_link_libraries(actormsg_test ${COMMON_TEST_LIBS})

add_executable(behavior_test BehaviorTest.cpp)
target_link_libraries(behavior_test ${COMMON_TEST_LIBS})
