

#The targets
TARGET=pro
SHAREDLIB_TARGET=libparaordenar_core.so
DYLIB_TARGET=libparaordenar_core.dylib
STATICLIB_TARGET=libparaordenar_core.a

#Compiler
CXX=clang++

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := inc
BUILDDIR    := build
TARGETDIR   := bin
LIBDIR      := lib
RESDIR      := res
DOCSDIR     := doc
SRCEXT      := cc
DEPEXT      := d
OBJEXT      := o

#Flags, libraries and includes
CXXFLAGS= -Wall -g -I$(INCDIR) -I/Users/marcclasca/Documents/Programació/boost/include -std=c++20
LDFLAGS= -fuse-ld=lld -L/Users/marcclasca/Documents/Programació/boost/lib -lboost_regex -lboost_filesystem

LIB_SOURCES := $(SRCDIR)/storage.cc $(SRCDIR)/project.cc $(SRCDIR)/pugixml.cc
LIB_OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(LIB_SOURCES:.$(SRCEXT)=.$(OBJEXT)))

all: $(TARGET) lib doc

#Remake
remake: cleaner all

#Make the Directories
directories:
	@mkdir -p $(TARGETDIR)
	@mkdir -p $(BUILDDIR)
	@mkdir -p $(LIBDIR)

#Clean only Objecst
clean:
	@$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	@$(RM) -rf $(TARGETDIR) $(LIBDIR) $(DOCSDIR)

#Main program link
$(TARGET): $(BUILDDIR)/paraordenar.o $(LIBDIR)/$(DYLIB_TARGET)
	@mkdir -p $(TARGETDIR)
	$(CXX) -o $(TARGETDIR)/$(TARGET) $^ $(LDFLAGS)

#Link libraries
$(LIBDIR)/$(SHAREDLIB_TARGET): $(LIB_OBJECTS)
	@mkdir -p $(LIBDIR)
	$(CXX) -shared -undefined dynamic_lookup -o $@ $^

$(LIBDIR)/$(DYLIB_TARGET): $(LIB_OBJECTS)
	@mkdir -p $(LIBDIR)
	$(CXX) -shared -undefined dynamic_lookup -fuse-ld=lld -o $@ $^

$(LIBDIR)/$(STATICLIB_TARGET): $(LIB_OBJECTS)
	@mkdir -p $(LIBDIR)
	llvm-ar cr $(LIBDIR)/$@ $^


#Compile all sources
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

lib: $(SHAREDLIB_TARGET) $(DYLIB_TARGET) $(STATICLIB_TARGET)

doc: Doxyfile
	@mkdir -p $(DOCSDIR)
	doxygen Doxyfile

.PHONY: all remake clean cleaner doc