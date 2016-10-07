TOPDIR=/home/nbayyana/playground/cpp/test_fbthrift
THRIFT=$(TOPDIR)/thrift-0.9.3/compiler/cpp/thrift
FBTHRIFT_CPP=$(TOPDIR)/artifacts/bin/thrift1
FBTHRIFT_CPP2_GEN_PATH = $(TOPDIR)/artifacts/bin

#PYTHONPATH=$(FBTHRIFT_CPP2_GEN_PATH) python -mthrift_compiler.main --gen cpp2:json -o $(TOPDIR)/src/infra/gen service.thrift

infra: commontypes.thrift status.thrift configtree.thrift service.thrift
	$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen commontypes.thrift
	$(FBTHRIFT_CPP) --gen cpp:enum_strict -out $(TOPDIR)/src/infra/gen status.thrift
	$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen configtree.thrift
	PYTHONPATH=$(FBTHRIFT_CPP2_GEN_PATH) python -mthrift_compiler.main --gen cpp2:process_in_event_base,json -o $(TOPDIR)/src/infra/gen service.thrift
	#$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen service.thrift

infra-java: commontypes.thrift status.thrift configtree.thrift service.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java commontypes.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java status.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java configtree.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java service.thrift

infra-clean:
	rm -rf $(TOPDIR)/src/infra/gen/*
	rm -rf $(TOPDIR)/src/java/src/main/java/infra/gen/*

clean: infra-clean
