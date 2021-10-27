
TARGET=pro
CXX=g++

CXXFLAGS= -Wall -g -I./

pro: paraordenar.cc
	$(CXX) $(CXXFLAGS) -o $(TARGET) paraordenar.cc -L/usr/local/lib -static -lboost_filesystem


clean:
	rm *.o $(TARGET)
