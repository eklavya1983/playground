TOPDIR=/home/nbayyana/playground/cpp/test_fbthrift
THRIFT=$(TOPDIR)/thrift-0.9.3/compiler/cpp/thrift
FBTHRIFT_CPP=$(TOPDIR)/fbthrift/thrift/compiler/thrift1

infra: commontypes.thrift status.thrift configtree.thrift
	$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen commontypes.thrift
	$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen status.thrift
	$(FBTHRIFT_CPP) --gen cpp -out $(TOPDIR)/src/infra/gen configtree.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java commontypes.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java status.thrift
	$(THRIFT) --gen java -out $(TOPDIR)/src/java/src/main/java configtree.thrift

infra-clean:
	rm -rf $(TOPDIR)/src/infra/gen/*
	rm -rf $(TOPDIR)/src/java/src/main/java/infra/gen/*

clean: infra-clean
