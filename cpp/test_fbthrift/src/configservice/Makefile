THRIFT=/home/nbayyana/playground/cpp/test_fbthrift/fbthrift/thrift/compiler/thrift1

gen: gencpp genjava

gencpp: configapi.thrift
	$(THRIFT) --gen cpp configapi.thrift
genjava: configapi.thrift
	$(THRIFT) --gen java -out java/src/main/java configapi.thrift
clean:
	rm -rf gen-cpp java/src/main/java/genapi/*
