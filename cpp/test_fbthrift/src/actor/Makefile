PY_LOCAL_PATH = /home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/compiler/py/build/lib.linux-x86_64-2.7 

gen: Service

Service: Service.thrift
	PYTHONPATH=$(PY_LOCAL_PATH) python -mthrift_compiler.main --gen cpp2:json Service.thrift
clean:
	rm -rf gen-cpp2/*
