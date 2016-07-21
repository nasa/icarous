MAVLinkSRC = $(wildcard src/com/MAVLink/*.java src/com/MAVLink/common/*.java \
			src/com/MAVLink/icarous/*.java \
	                src/com/MAVLink/enums/*.java \
			src/com/MAVLink/Messages/*.java)

ICAROUSSRC = $(wildcard src/gov/nasa/larcfm/ICAROUS/*.java)

SITLSRC    = $(wildcard src/gov/nasa/larcfm/SITL/*.java)

PX4SRC     = $(wildcard src/gov/nasa/larcfm/PX4/*.java)

JAR        = :lib/jssc-2.8.0.jar:lib/FormalATM.jar

CLASSPATH  = bin

JAVAC      = javac


MAVLINK: $(MAVLinkSRC)
	@echo
	@echo "Compiling MAVLink messages"
	$(JAVAC) $(MAVLinkSRC) -d $(CLASSPATH)

ICAROUS: $(ICAROUSSRC)
	@echo
	@echo
	$(JAVAC) -cp $(CLASSPATH)$(JAR) $(ICAROUSSRC) -d $(CLASSPATH)

SITL: $(SITLSRC)
	@echo
	@echo
	$(JAVAC) -cp $(CLASSPATH)$(JAR) $(SITLSRC) -d $(CLASSPATH)

PX4: $(PX4SRC)
	@echo
	@echo
	$(JAVAC) -cp $(CLASSPATH)$(JAR) $(PX4SRC) -d $(CLASSPATH)

