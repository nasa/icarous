CC = g++
CFLAGS = -Wall -g -Wno-write-strings
INCLUDES = -I./libs/mavlink -I./include/ACCoRD -I./include/ICAROUS
LIBS = -lpthread -lm
SRCS = $(wildcard ./src/ICAROUS/*.cpp)
OBJS = $(patsubst ./src/ICAROUS/%.cpp,./obj/ICAROUS/%.o,$(SRCS))
MAIN = icarous

all: $(MAIN)

test:
	@echo $(SRCS)
	@echo $(OBJS)

$(MAIN): PRE $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJS) $(LFLAGS) $(LIBS)

./obj/ICAROUS/%.o:./src/ICAROUS/%.cpp	
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

PRE:
	@test -d obj/ICAROUS || (mkdir obj && mkdir obj/ICAROUS)

clean:
	rm obj/ICAROUS/*