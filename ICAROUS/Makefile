MAVLinkSRC = $(wildcard src/com/MAVLink/*.java src/com/MAVLink/common/*.java \
	                src/com/MAVLink/enums/*.java \
			src/com/MAVLink/Messages/*.java)

ICAROUSSRC = $(wildcard src/gov/nasa/larc/ICAROUS/*.java)

SITLSRC    = $(wildcard src/gov/nasa/larc/SITL/*.java)

JAVAC      = javac

MAVLINK: $(MAVLinkSRC)
	@echo
	@echo "Compiling MAVLink messages"
	$(JAVAC) $(MAVLinkSRC) -d ./bin

ICAROUS: $(ICAROUSSRC)
	@echo
	@echo
	$(JAVAC) -cp ./bin $(ICAROUSSRC) -d ./bin

SITL: $(SITLSRC)
	@echo
	@echo
	$(JAVAC) -cp ./bin $(SITLSRC) -d ./bin
