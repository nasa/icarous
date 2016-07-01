MAVLinkSRC = $(wildcard src/com/MAVLink/*.java src/com/MAVLink/common/*.java \
			src/com/MAVLink/icarous/*.java \
	                src/com/MAVLink/enums/*.java \
			src/com/MAVLink/Messages/*.java)

ICAROUSSRC = $(wildcard src/gov/nasa/larc/ICAROUS/*.java)

SITLSRC    = $(wildcard src/gov/nasa/larc/SITL/*.java)

JAR        = :lib/jssc-2.8.0.jar

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
	$(JAVAC) -cp $(CLASSPATH) $(SITLSRC) -d $(CLASSPATH)
