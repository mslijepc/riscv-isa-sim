
# Extracted from Spike Makefile.in - minimum for building shared library

# Basic setup from Makefile.in
.SUFFIXES:
% : %,v
% : RCS/%,v
% : RCS/%
% : s.%
% : SCCS/s.%

# Programs and flags (from Makefile.in)
default-CFLAGS   := -Wall -Wno-nonportable-include-path -g -O2 -fPIC
default-CXXFLAGS := $(default-CFLAGS) -std=c++2a

CXX           := g++
COMPILE       := $(CXX) -MMD -MP $(default-CXXFLAGS) -iquote . -I../riscv -I../fesvr -I../disasm -I../softfloat
LD            := $(CXX)
LINK          := $(LD) -shared

# Extension definition (mimicking customext)
xrockettest_srcs := xrockettest.cc
xrockettest_objs := $(patsubst %.cc, %.o, $(xrockettest_srcs))
xrockettest_deps := $(patsubst %.o, %.d, $(xrockettest_objs))

# Build rules (from subproject template)
$(xrockettest_objs) : %.o : %.cc
	$(COMPILE) -c $<

libxrockettest.so : $(xrockettest_objs)
	$(CXX) -shared -o $@ $^

# Targets
all: libxrockettest.so

clean:
	rm -f $(xrockettest_objs) $(xrockettest_deps) libxrockettest.so

install: libxrockettest.so
	cp $< ../

-include $(xrockettest_deps)

.PHONY: all clean install
