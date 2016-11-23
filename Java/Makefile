ACCORDSRC  = $(wildcard src/gov/nasa/larcfm/IO/*.java \
			src/gov/nasa/larcfm/Util/*.java \
			src/gov/nasa/larcfm/ACCoRD/*.java)
MAVLinkSRC = $(wildcard src/com/MAVLink/*.java src/com/MAVLink/common/*.java \
			src/com/MAVLink/icarous/*.java \
			src/com/MAVLink/ardupilotmega/*.java \
			src/com/MAVLink/enums/*.java \
			src/com/MAVLink/Messages/*.java)
ICAROUSSRC = $(wildcard src/gov/nasa/larcfm/ICAROUS/*.java)
MISSIONSRC = $(wildcard src/gov/nasa/larcfm/MISSION/*.java)
OTHERSRC   = $(wildcard src/*.java)
LIB        = :lib/jssc-2.8.0.jar
CLASSPATH  = bin
JAVAC      = javac
OBJ        = $(SRCS:.java=.class)


ALL: MAVLINK ACCORD ICAROUS MISSION OTHERS JAR EXAMPLE

PRE:
	test -d bin || mkdir bin

ACCORD: PRE $(ACCORDSRC)
	@echo
	@echo "Compiling package: ACCoRD"
	@$(JAVAC) $(ACCORDSRC) -d $(CLASSPATH)	

MAVLINK: PRE $(MAVLinkSRC)
	@echo
	@echo "Compiling package: MAVLink"
	@$(JAVAC) $(MAVLinkSRC) -d $(CLASSPATH)

ICAROUS: PRE $(ICAROUSSRC)
	@echo
	@echo "Compiling package: ICAROUS"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(ICAROUSSRC) -d $(CLASSPATH)

MISSION: PRE $(MISSIONSRC)
	@echo
	@echo "Compiling package: Mission"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(MISSIONSRC) -d $(CLASSPATH)

OTHERS: PRE $(OTHERSRC)
	@echo
	@echo "Compiling remaining files in src/"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(OTHERSRC) -d $(CLASSPATH)

JAR:
	@cd bin && jar -cvf icarous.jar gov com *.class
	@mv bin/icarous.jar ./lib/
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(OTHERSRC) -d $(CLASSPATH)

EXAMPLE:
	@echo
	@echo "Compiling IcarousExample"
	@$(JAVAC) -cp lib/icarous.jar IcarousExample.java
	@echo "** To run IcarousExample type:"
	@echo "./IcarousExample"

clean:
	rm -rf bin/* lib/icarous.jar
