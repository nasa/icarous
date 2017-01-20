# Simple Makefile to build ICAROUS library and example application

SRCS   = $(wildcard src/*.cpp)
OBJS   = $(SRCS:.cpp=.o)
INCLUDEFLAGS = -Iinclude 
CXXFLAGS = $(INCLUDEFLAGS) -Wall -O 

all: lib example

lib: $(OBJS)
	@echo 
	@echo "** Building ICAROUS binary library"
	ar -cr lib/icarous.a $(OBJS) && ranlib lib/icarous.a #&& rm *.o
	@echo 

example:
	@echo
	@echo "** Building DaidalusExample application"
	$(CXX) -o DAAGeofencingExample $(CXXFLAGS) DAAGeofencingExample.cpp lib/ICAROUS.a
	@echo 
	@echo "** To run DAAGeofencingExample type:"
	@echo "./DAAGeofencingExample"

clean:
	rm -f DAAGeofencingExample $(OBJS) lib/ICAROUS.a

.PHONY: all lib example
