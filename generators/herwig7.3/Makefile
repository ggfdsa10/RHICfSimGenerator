# -*- Makefile -*- (for emacs)
#
# This Makefile is intended for compiling Herwig++ plugins
#
# This Makefile received very little testing, 
# any bug reports are very welcome!
#
# Location of include files

THEPEGINCLUDE=-I/home/public/herwig/include
HERWIGINCLUDE=-I/home/public/herwig/include/
GSLINCLUDE=-I/home/public/herwig/include 

ROOTINCLUDE = -I/home/public/root-v6.24/root_install/include
ROOTCFLAGS  := -m64
ROOTGLIBS   := -L/home/public/root-v6.24/root_install/lib -lGui -lCore -lImt -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lROOTVecOps -lTree -lTreePlayer -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -lMultiProc -lROOTDataFrame -Wl,-rpath,/home/public/root-v6.24/root_install/lib -pthread -lm -ldl -rdynamic -lGenVector

RHICFSIMINCLUDE=-I/home/shlee/workspace/rhicf/source
RHICFSIM_LIB=/home/shlee/workspace/rhicf/build
RHICFSIM=-L$(RHICFSIM_LIB) -lRHICfSim -Wl,-rpath,$(RHICFSIM_LIB)


LDFLAGS= $(ROOTCFLAGS) $(ROOTGLIBS)
SHARED_FLAG=-shared  
INCLUDE = $(THEPEGINCLUDE) $(HERWIGINCLUDE) $(GSLINCLUDE) $(ROOTINCLUDE) $(RHICFSIMINCLUDE)

# C++ flags
CXX=g++ -std=c++14 -std=c++14 
CXXFLAGS=-O2 -DBOOST_UBLAS_NDEBUG

ALLCCFILES=$(shell echo *.cc)

default : HwRHICfHandler.so

%.o : %.cc %.h
	$(CXX) -fPIC $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $(SHARED_FLAG) $(LDFLAGS) -c -shared $< -o $@
	
HwRHICfHandler.so : HwRHICfHandler.o
	$(CXX) -o HwRHICfHandler.so HwRHICfHandler.o -fPIC $(CPPFLAGS) $(INCLUDE) $(CXXFLAGS) $(RHICFSIM) $(SHARED_FLAG) $(LDFLAGS) \
	-Wl,-undefined -Wl,dynamic_lookup\

clean:
	rm -f $(ALLCCFILES:.cc=.o) \
		HwRHICfHandler.so