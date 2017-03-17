CFLAGS = -Wall -g -Wno-write-strings -std=c++11
INCLUDES = -I./libs/mavlink -I./include/ACCoRD -I./include/ICAROUS
LIBS = -lpthread -lm
SRCS = $(wildcard ./src/ICAROUS/*.cpp)
OBJS = $(patsubst ./src/ICAROUS/%.cpp,./obj/ICAROUS/%.o,$(SRCS))
ASRCS = $(wildcard ./src/ACCoRD/*.cpp)
AOBJS = $(patsubst ./src/ACCoRD/%.cpp,./obj/ACCoRD/%.o,$(ASRCS))
MAIN = icarous

all: $(MAIN) DAAGeofencing

$(MAIN): LIB
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(MAIN) libs/icarous.a $(LFLAGS) $(LIBS)

LIB: PRE $(AOBJS) $(OBJS)
	ar -cr libs/icarous.a $(OBJS) $(AOBJS) && ranlib libs/icarous.a 

DAAGeofencing:
	$(CXX) -o DAAGeofencingExample $(CFLAGS) $(INCLUDES) src/DAAGeofencingExample.cpp libs/icarous.a

./obj/ICAROUS/%.o:./src/ICAROUS/%.cpp	
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/ACCoRD/%.o:./src/ACCoRD/%.cpp	
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

PRE:
	@test -d obj/ICAROUS || (mkdir obj && mkdir obj/ICAROUS)
	@test -d obj/ACCoRD || (mkdir obj/ACCoRD)

cleanica:
	-@rm obj/ICAROUS/* libs/icarous.a

cleanacc: 
	-@rm obj/ACCoRD/*

clean: cleanica cleanacc
