####
# This GNUmakefile is in charge of building the C++17 version of the NanoLog
# library, its gtest unit tests, and some internal performance benchmarks.
# To build the Preprocessor version of the library, see ../NanoLogMakeFrag
###

# Common Sources
SRCS=Cycles.cc Util.cc NanoLog.cc ConfigFileMonitor.cc Log.cc RuntimeLogger.cc LogManager.cc TimeTrace.cc LogSplitter.cc
OBJECTS:=$(SRCS:.cc=.o)

LIBS=-L. -lrt -pthread
# CXX_ARGS=-std=c++17 -g -O3 -DENABLE_DEBUG_PRINTING=1 
CXX_ARGS=-std=c++17 -g -O2 
CXX?=g++

all: libNanoLog.a

%.o: %.cc
	$(CXX) $(CXX_ARGS) $(EXTRA_NANOLOG_FLAGS) $(INCLUDES) -c $< -o $@

# Builds a C++17 version of the library; the generated OBJ is only necessary for error checking
libNanoLog.a: $(OBJECTS) 
	ar -cr libNanoLog.a $(OBJECTS) 

clean:
	rm -f  *.o *.gch *.log ./.depend

clean-all: clean
	rm -f libgtest.a 

