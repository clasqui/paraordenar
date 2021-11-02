
TARGET=pro
CXX=g++

CXXFLAGS= -Wall -g -I./

all: pro

pro: paraordenar.cc storage.o pugixml.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ -L/usr/local/lib -static -lboost_filesystem

storage.o: storage.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

pugixml.o: pugixml.cpp
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	rm *.o $(TARGET)
