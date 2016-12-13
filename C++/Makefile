CC = g++
CFLAGS = -g -Wno-write-strings -std=c++11
INCLUDES = -I./libs/mavlink -I./include/ACCoRD -I./include/ICAROUS
LIBS = -lpthread -lm
SRCS = $(wildcard ./src/ICAROUS/*.cpp)
OBJS = $(patsubst ./src/ICAROUS/%.cpp,./obj/ICAROUS/%.o,$(SRCS))
ASRCS = $(wildcard ./src/ACCoRD/*.cpp)
AOBJS = $(patsubst ./src/ACCoRD/%.cpp,./obj/ACCoRD/%.o,$(ASRCS))
MAIN = icarous

all: $(MAIN)

$(MAIN): PRE $(AOBJS) $(OBJS) 
	@$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(AOBJS) $(OBJS) $(LFLAGS) $(LIBS)

./obj/ICAROUS/%.o:./src/ICAROUS/%.cpp	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

./obj/ACCoRD/%.o:./src/ACCoRD/%.cpp	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

PRE:
	@test -d obj/ICAROUS || (mkdir obj && mkdir obj/ICAROUS)
	@test -d obj/ACCoRD || (mkdir obj/ACCoRD)

clean:
	rm obj/ICAROUS/*