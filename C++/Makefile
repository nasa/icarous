CFLAGS = -Wall -g -Wno-write-strings -std=c++11
INCLUDES = -I./libs/mavlink -I./include/ACCoRD -I./include/ICAROUS -I./include/INTERFACE
PLEXIL_INCLUDES = -I./plexil-4/include -I./include/PLEXIL
LIBS  = -lpthread -lm
PLIBS = -L./plexil-4/lib -lpugixml -lIpcUtils -lipc -lPlexilExpr -lPlexilUtils -lPlexilValue -lpthread
SRCS = $(wildcard ./src/ICAROUS/*.cpp)
OBJS = $(patsubst ./src/ICAROUS/%.cpp,./obj/ICAROUS/%.o,$(SRCS))
ISRCS = $(wildcard ./src/INTERFACE/*.cpp)
IOBJS = $(patsubst ./src/INTERFACE/%.cpp,./obj/INTERFACE/%.o,$(ISRCS))
ASRCS = $(wildcard ./src/ACCoRD/*.cpp)
AOBJS = $(patsubst ./src/ACCoRD/%.cpp,./obj/ACCoRD/%.o,$(ASRCS))
PSRCS = ./src/PLEXIL/IpcIcarousAdapter.cpp
POBJS = $(patsubst ./src/PLEXIL/%.cpp,./obj/PLEXIL/%.o,$(PSRCS))
PLAIN = icarous
ICPLEXIL = icarousPlexil

all: $(PLAIN) $(ICPLEXIL) DAAGeofencing

$(PLAIN): LIB
	@$(CXX) $(CFLAGS) $(INCLUDES) -o $(PLAIN) src/LaunchIcarous.cpp libs/icarous.a $(LFLAGS) $(LIBS)

$(ICPLEXIL): LIB $(POBJS)
	@$(CXX) $(CFLAGS) $(PLEXIL_INCLUDES) $(INCLUDES) -o $(ICPLEXIL) src/LaunchIcarousPlexil.cpp libs/icarous.a $(POBJS) $(LIBS) $(PLIBS)

LIB: PRE $(AOBJS) $(IOBJS) $(OBJS)
	ar -cr libs/icarous.a $(OBJS) $(IOBJS) $(AOBJS) && ranlib libs/icarous.a

DAAGeofencing:
	$(CXX) -o DAAGeofencingExample $(CFLAGS) $(INCLUDES) src/DAAGeofencingExample.cpp libs/icarous.a $(LIBS)

./obj/ICAROUS/%.o:./src/ICAROUS/%.cpp	
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/INTERFACE/%.o:./src/INTERFACE/%.cpp
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/ACCoRD/%.o:./src/ACCoRD/%.cpp	
	$(CXX) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/PLEXIL/%.o:./src/PLEXIL/%.cpp
	$(CXX) $(CFLAGS) $(PLEXIL_INCLUDES) $(INCLUDES) -c $< -o $@ $(PLIBS)

PRE:
	@test -d obj/ICAROUS || (mkdir obj && mkdir obj/ICAROUS)
	@test -d obj/INTERFACE || (mkdir obj/INTERFACE)
	@test -d obj/ACCoRD || (mkdir obj/ACCoRD)
	@test -d obj/PLEXIL || (mkdir obj/PLEXIL)

cleanica:
	-@rm obj/ICAROUS/* obj/INTERFACE/* libs/icarous.a

cleanacc: 
	-@rm obj/ACCoRD/*

clean: cleanica cleanacc
