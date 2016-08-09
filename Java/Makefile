MAVLinkSRC = $(wildcard src/com/MAVLink/*.java src/com/MAVLink/common/*.java \
			src/com/MAVLink/icarous/*.java \
			src/com/MAVLink/ardupilotmega/*.java \
			src/com/MAVLink/enums/*.java \
			src/com/MAVLink/Messages/*.java)

ICAROUSSRC = $(wildcard src/gov/nasa/larcfm/ICAROUS/*.java)

MISSIONSRC = $(wildcard src/gov/nasa/larcfm/MISSION/*.java)

SITLSRC    = $(wildcard src/gov/nasa/larcfm/SITL/*.java)

PX4SRC     = $(wildcard src/gov/nasa/larcfm/PX4/*.java)

LAUNCHSRC  = $(wildcard src/gov/nasa/larcfm/*.java)

JAR        = :lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar

CLASSPATH  = bin

JAVAC      = javac


ALL: MAVLINK ICAROUS MISSION LAUNCH SITL PX4 

PRE:
	test -d bin || mkdir bin

MAVLINK: PRE $(MAVLinkSRC)
	@echo
	@echo "Compiling MAVLink messages"
	@$(JAVAC) $(MAVLinkSRC) -d $(CLASSPATH)

ICAROUS: PRE $(ICAROUSSRC)
	@echo
	@echo "Compiling ICAROUS package"
	@$(JAVAC) -cp $(CLASSPATH)$(JAR) $(ICAROUSSRC) -d $(CLASSPATH)

MISSION: PRE $(MISSIONSRC)
	@echo
	@echo "Compiling Mission files"
	@$(JAVAC) -cp $(CLASSPATH)$(JAR) $(MISSIONSRC) -d $(CLASSPATH)

SITL: PRE $(SITLSRC)
	@echo
	@echo "Compiling SITL files"
	@$(JAVAC) -cp $(CLASSPATH)$(JAR) $(SITLSRC) -d $(CLASSPATH)

PX4: PRE $(PX4SRC)
	@echo
	@echo "Compiling PX4 files"
	@$(JAVAC) -cp $(CLASSPATH)$(JAR) $(PX4SRC) -d $(CLASSPATH)

LAUNCH: PRE $(LAUNCHSRC)
	@echo
	@echo "Compiling launch module"
	@$(JAVAC) -cp $(CLASSPATH)$(JAR) $(LAUNCHSRC) -d $(CLASSPATH)
