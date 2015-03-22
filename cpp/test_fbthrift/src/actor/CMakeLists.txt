cmake_minimum_required(VERSION 2.8)
project("actor")

# Generate thrift files
#set(PY_LOCAL_PATH /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/compiler/py/build/lib.linux-x86_64-2.7) 
#add_custom_command(
#	OUTPUT
#		gen-cpp2/Service_client.cpp
#		gen-cpp2/Service_constants.cpp
#		gen-cpp2/Service.cpp
#		gen-cpp2/Service_processmap_binary.cpp
#		gen-cpp2/Service_processmap_compact.cpp
#		gen-cpp2/Service_processmap_simple_json.cpp
#		gen-cpp2/Service_types.cpp
#		gen-cpp2/TestService_client.cpp
#		gen-cpp2/TestService.cpp
#		gen-cpp2/TestService_processmap_binary.cpp
#		gen-cpp2/TestService_processmap_compact.cpp
#		gen-cpp2/TestService_processmap_simple_json.cpp
#	COMMAND
#		PYTHONPATH=${PY_LOCAL_PATH} python -mthrift_compiler.main --gen cpp2:json Service.thrift
#	DEPENDS Service.thrift
#	)

# External dependency include directories
include_directories(./gen-cpp2)

add_subdirectory(test)

# Get generated thrift sources
file(GLOB_RECURSE thrift_gen_sources "gen-cpp2/*.cpp")
message("Thirft sources: ${thrift_gen_sources}")

# Executable
add_library(actor
	ActorMsg.cpp
	Actor.cpp
	ActorSystem.cpp
	ServiceHandle.cpp
	${thrift_gen_sources}
	ActorServer.cpp
	RemoteActor.cpp
	)
