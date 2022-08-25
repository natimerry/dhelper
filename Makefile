CC = gcc
CXX =  g++
CXXFLAGS = -fno-stack-protector -O3 -flto -lblkid

dhelper: dhelper.o partition.o benchmark.o
	$(CXX) $(CXXFLAGS) benchmark.o dhelper.o partition.o -o dhelper

benchmark.o: benchmark.cpp benchmark.h
	$(CXX) benchmark.cpp $(CXXFLAGS) -c
dhelper.o: dhelper.cpp dhelper.h
	$(CC) dhelper.cpp $(CXXFLAGS) -c 

partition.o: partition.cpp partition.h 
	$(CC) partition.cpp -c $(CXXFLAGS)

clean:
	rm -rf *.o
