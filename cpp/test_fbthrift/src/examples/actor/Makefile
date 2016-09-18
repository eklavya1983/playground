PY_LOCAL_PATH = /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/compiler/py/build/lib.linux-x86_64-2.7 

THRIFT=/home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/compiler/thrift1


gen: Service

Service: Service.thrift
	PYTHONPATH=$(PY_LOCAL_PATH) python -mthrift_compiler.main --gen cpp2:json Service.thrift

genjava: ServiceJava
ServiceJava: Service.thrift
	$(THRIFT) --gen java Service.thrift

clean:
	rm -rf gen-cpp2/*
