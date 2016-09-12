# Simple Makefile to build ICAROUS library and example application

SRCS   = $(wildcard src/*.cpp)
OBJS   = $(SRCS:.cpp=.o)
INCLUDEFLAGS = -Iinclude 
CXXFLAGS = $(INCLUDEFLAGS) -Wall -O 

all: lib example

lib: $(OBJS)
	@echo 
	@echo "** Building ICAROUS binary library"
	ar -cr lib/ICAROUS.a $(OBJS) && ranlib lib/ICAROUS.a #&& rm *.o
	@echo 

example:
	@echo
	@echo "** Building DaidalusExample application"
	$(CXX) -o IcarousExample $(CXXFLAGS) IcarousExample.cpp lib/ICAROUS.a
	@echo 
	@echo "** To run IcarousExample type:"
	@echo "./IcarousExample"

clean:
	rm -f IcarousExample $(OBJS) lib/ICAROUS.a

.PHONY: all lib example
