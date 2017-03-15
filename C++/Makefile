dry=--dry-run
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

sync:
	rsync -avz $(dry) --exclude '*Test*' --exclude '*Stress*' --exclude 'CDBatch*' --exclude 'bBands*' --exclude 'bCRSS*' ~/src/Util/C++/src/*.cpp ~/src/ACCoRD/C++/src/*.cpp src/ACCoRD/
	rsync -avz $(dry) ~/src/Util/C++/src/*.h ~/src/ACCoRD/C++/src/*.h include/ACCoRD/
	@echo "Try make sync -e dry="

clean:
	-@rm obj/ICAROUS/* libs/icarous.a

cleanall: clean
	-@rm obj/ACCoRD/*
