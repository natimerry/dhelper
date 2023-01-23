CC = clang
CXX =  clang++
CFLAGS = -DFORTIFY_SOURCE=2 -fstack-protector -O3 -flto=thin -lblkid 
CXXFLAGS = -std=c++17 -DFORTIFY_SOURCE=2 -fstack-protector -O3 -flto=thin -lblkid 
STRIP=llvm-strip 
dhelper: dhelper.o partition.o benchmark.o
	$(CXX) $(CFLAGS) benchmark.o dhelper.o partition.o -o dhelper 
	@echo "Stripping Binaries"
	$(STRIP) dhelper

benchmark.o: benchmark.cpp benchmark.h
	$(CXX) benchmark.cpp $(CXXFLAGS) -c
dhelper.o: dhelper.c dhelper.h
	$(CC) dhelper.c $(CFLAGS) -c 

partition.o: partition.c partition.h 
	$(CC) partition.c -c $(CFLAGS)

clean:
	rm -rf *.o

install: dhelper
	cp dhelper /usr/bin/dhelper
	chmod 755 /usr/bin/dhelper
