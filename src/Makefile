
TARGET=pro
CXX=g++

CXXFLAGS= -Wall -g -I./

pro: paraordenar.cc storage.o
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ -L/usr/local/lib -static -lboost_filesystem

storage.o: storage.cc
	$(CXX) $(CXXFLAGS) -o $@ -c $^

clean:
	rm *.o $(TARGET)
