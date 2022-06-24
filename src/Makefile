
TARGET=pro
SHAREDLIB_TARGET=libparaordenar_core.so
DYLIB_TARGET=libparaordenar_core.dylib
STATICLIB_TARGET=libparaordenar_core.a
CXX=clang++

CXXFLAGS= -Wall -g -I./ -I/Users/marcclasca/Documents/Programació/boost/include -std=c++20
LDFLAGS= -fuse-ld=lld -L/Users/marcclasca/Documents/Programació/boost/lib -lboost_regex -lboost_filesystem

LIB_OBJECTS := storage.cc project.cc pugixml.cpp 

.PHONY:
all: pro

pro: paraordenar.cc $(DYLIB_TARGET)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $^ $(LDFLAGS)

$(SHAREDLIB_TARGET): $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) -shared -undefined dynamic_lookup -o $@ $^

$(DYLIB_TARGET): $(LIB_OBJECTS)
	$(CXX) $(CXXFLAGS) -shared -undefined dynamic_lookup -fuse-ld=lld -o $@ $^

$(STATICLIB_TARGET): $(LIB_OBJECTS)
	llvm-ar cr $@ $^

lib: $(SHAREDLIB_TARGET) $(DYLIB_TARGET) $(STATICLIB_TARGET)

clean:
	rm *.o $(TARGET) $(SHAREDLIB_TARGET) $(STATICLIB_TARGET) $(DYLIB_TARGET)
