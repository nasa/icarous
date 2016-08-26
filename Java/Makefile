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

LIB        = :lib/jssc-2.8.0.jar:lib/FormalATMj/FormalATM.jar

CLASSPATH  = bin

JAVAC      = javac


ALL: MAVLINK ICAROUS MISSION LAUNCH JAR

PRE:
	test -d bin || mkdir bin

MAVLINK: PRE $(MAVLinkSRC)
	@echo
	@echo "Compiling MAVLink messages"
	@$(JAVAC) $(MAVLinkSRC) -d $(CLASSPATH)

ICAROUS: PRE $(ICAROUSSRC)
	@echo
	@echo "Compiling ICAROUS package"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(ICAROUSSRC) -d $(CLASSPATH)

MISSION: PRE $(MISSIONSRC)
	@echo
	@echo "Compiling Mission files"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(MISSIONSRC) -d $(CLASSPATH)

LAUNCH: PRE $(LAUNCHSRC)
	@echo
	@echo "Compiling launch module"
	@$(JAVAC) -cp $(CLASSPATH)$(LIB) $(LAUNCHSRC) -d $(CLASSPATH)


JAR:
	@cd bin && jar -cvfm icarous.jar ../MANIFEST.mf gov com *.class
	@mv bin/icarous.jar ./
