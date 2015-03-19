cmake_minimum_required(VERSION 2.8)
project("mytest")

# flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -std=c++11")

add_subdirectory(actor)

# External dependency include directories
include_directories(/home/nbayyana/playground/cpp/test_fbthrift/fbthrift /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/folly .)

# External dependency library directories
LINK_DIRECTORIES(/home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/lib/cpp/.libs /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/lib/cpp2/.libs /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/folly/folly/.libs)

add_executable(actortest ActorTest.cpp)
TARGET_LINK_LIBRARIES(actortest actor thriftcpp2 thrift folly glog gflags pthread boost_system double-conversion z) 

# Executable
#add_executable(mytest TestServer.cpp gen-cpp2/Service_constants.cpp gen-cpp2/Service_types.cpp gen-cpp2/TestService_client.cpp gen-cpp2/TestService.cpp gen-cpp2/TestService_processmap_binary.cpp gen-cpp2/TestService_processmap_compact.cpp gen-cpp2/TestService_processmap_simple_json.cpp)

#TARGET_LINK_LIBRARIES(mytest thriftcpp2 thrift folly glog gflags pthread boost_system double-conversion z) 
#TARGET_LINK_LIBRARIES(mytest libthriftcpp2.a libthrift.a folly glog gflags pthread boost_system double-conversion z snappy krb5 gssapi_krb5 k5crypto sasl2 numa ssl crypto)
