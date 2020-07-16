# Conditionalizing make variables by platform

# Get the relevant system info
OSNAME		:= $(shell uname -s)
OSVERSION	:= $(shell uname -r)
ARCH		:= $(shell uname -p)

#$(info OSNAME is $(OSNAME), OSVERSION is $(OSVERSION), ARCH is $(ARCH))

ifdef TARGET_OS
# Cross compiling for some other platform
include $(PLEXIL_HOME)/makeinclude/platform-$(TARGET_OS).make
else
# Native compilation
include $(PLEXIL_HOME)/makeinclude/platform-$(OSNAME).make
endif
