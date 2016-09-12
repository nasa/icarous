# Simple Makefile to build ICAROUS library and example application

SRCS  = $(wildcard src/gov/nasa/larcfm/IO/*.java src/gov/nasa/larcfm/Util/*.java src/gov/nasa/larcfm/ACCoRD/*.java)
OBJS  = $(SRCS:.java=.class)
JAVAC = javac
JAR   = jar

all: example

lib: $(SRCS)
	@echo 
	@echo "** Building ICAROUS jar library"
	$(JAVAC) -cp src src/gov/nasa/larcfm/IO/*.java src/gov/nasa/larcfm/Util/*.java src/gov/nasa/larcfm/ACCoRD/*.java
	cd src; $(JAR) cvf ../lib/ICAROUS.jar gov/nasa/larcfm/IO/*.class gov/nasa/larcfm/Util/*.class gov/nasa/larcfm/ACCoRD/*.class

example:
	@echo 
	@echo "** Building IcarousExample application"
	$(JAVAC) -cp lib/ICAROUS.jar IcarousExample.java
	@echo 
	@echo "** To run IcarousExample type:"
	@echo "java -cp lib/ICAROUS.jar:. IcarousExample"

clean:
	rm -f IcarousExample.class $(OBJS) lib/ICAROUS.jar

.PHONY: all lib example
