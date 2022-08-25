CC = gcc
CXX =  g++
CXXFLAGS = -fno-stack-protector -O3 -flto -lblkid


dhelper: dhelper.o partition.o benchmark.o
	$(CXX) $(CXXFLAGS) benchmark.o dhelper.o partition.o -o dhelper

benchmark.o: benchmark.cpp benchmark.h
	$(CXX) benchmark.cpp $(CXXFLAGS) -c
dhelper.o: dhelper.c dhelper.h
	$(CC) dhelper.c $(CXXFLAGS) -c 

partition.o: partition.c partition.h 
	$(CC) partition.c -c $(CXXFLAGS)

clean:
	rm -rf *.o

install: dhelper
	cp dhelper /usr/bin/dhelper
	chmod 755 /usr/bin/dhelper
