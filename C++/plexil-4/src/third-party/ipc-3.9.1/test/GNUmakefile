#############################################################################
#       $Id: GNUmakefile,v 2.11 2011/08/16 16:00:06 reids Exp $
# $Revision: 2.11 $
#     $Date: 2011/08/16 16:00:06 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# PROJECT:	NM-DS1
#
# FILE:		GNUmakefile
#		Based on TCA GNUmakefile (Richard Goodwin)
#
# DESCRIPTION: Makes test programs for the IPC
#
# EXPORTS:
#	The following targets are available as part of GNUmakefile.std:
#		all bins libs privs objs
#		clean install
#		lint depend
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# HISTORY:
#
# $Log: GNUmakefile,v $
# Revision 2.11  2011/08/16 16:00:06  reids
# Adding Python test programs
#
# Revision 2.10  2009/09/04 19:09:26  reids
# IPC Java is now in its own package
#
# Revision 2.9  2009/01/12 15:54:58  reids
# Added BSD Open Source license info
#
# Revision 2.8  2005/12/30 17:01:48  reids
# Support for Mac OSX
#
# Revision 2.7  2003/02/16 16:48:27  trey
# added disconnectedListen test to build, commented out java-related targets that i cannot get to compile at the moment
#
# Revision 2.6  2002/01/11 22:34:25  reids
# Added target for BDIR/multiTest
#
# Revision 2.5  2002/01/04 15:02:37  reids
# Added module.java to list of SRCS
#
# Revision 2.4  2002/01/02 21:39:17  reids
# Changed makefile so that tar file ("gmake ftp") includes java stuff.
#
# Revision 2.3  2002/01/02 21:13:09  reids
# Added test files for the Java version of IPC, corresponding to the
#   C and Lisp test files that already existed.
#
# Revision 2.2  2000/08/14 21:32:11  reids
# Added support for making under Windows.
#
# Revision 2.1.1.1  1999/11/23 19:07:37  reids
# Putting IPC Version 2.9.0 under local (CMU) CVS control.
#
#############################################################################

.KEEP_STATE:

#############################################################################
# Module configuration.
#############################################################################

INSTALL_DIR	= ..
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
MODULE_DIR	= test
PROJECT		= ipc
MODULE		= test
SUBDIRS		=
DEPEND_FILE	= $(PWD)/GNUmakefile.$(THIS_MACHINE)
DEPEND_PREFIX	= $(ODIR)/
VERSION_FILE	= 
VERSION_PREFIX	= 

#############################################################################
# Standard Makefile Includes
#############################################################################

PWD 	:= $(shell /bin/pwd)
STD_FILE := $(shell ls $(INSTALL_DIR)/etc/GNUmakefile.std 2>/dev/null | grep -v "not found" | head -1)
DEFS_FILE := $(shell ls $(INSTALL_DIR)/etc/GNUmakefile.defs 2>/dev/null | grep -v "not found" | head -1)

##########
# Include standard definitions
##########

include $(DEFS_FILE)

##########
# Override defaults here.
##########

##########
# File Definition Macros.  Deletion of generated files assumed
#    to be o.k.  Source files will never be deleted.
##########

# Generated files - installed
PUBLIC_BINS_SunOS	= 
PUBLIC_BINS		= 
PUBLIC_LIBS_Solaris	= 
PUBLIC_LIBS_SunOS	= 
PUBLIC_LIBS_linux	= 
PUBLIC_LIBS_RISC	= 
PUBLIC_LIBS		= 
PUBLIC_LINTS		= 

MODULE_LIBS		= 
MODULE_LINTS		= 

# Source files - installed
PUBLIC_INCS		= 
PUBLIC_MANS		= 
PUBLIC_DOCS		= 

MODULE_INCS		= 
MODULE_DOCS		= 

INCS			= $(MODULE_INCS) $(PUBLIC_INCS)

# Generated files - not installed
PRIVS	= sizesTest ipcTest1 module1 module2 module3 \
	  latency1 latency2 throughput1 throughput2 \
	  checkTest intTest marshallTest priorityTest timerTest \
	   multiTest disconnectedListen # ipcTest1j module1j module2j module3j

OBJS	= sizesTest.o ipcTest1.o module1.o module2.o module3.o \
	  latency1.o latency2.o throughput1.o throughput2.o \
	  checkTest.o intTest.o marshallTest.o priorityTest.o timerTest.o \
	  multiTest.o disconnectedListen.o

# Source files - not installed

SRCS   =  sizesTest.c ipcTest1.c module1.c module2.c module3.c \
	  latency1.c latency2.c throughput1.c throughput2.c \
	  checkTest.c intTest.c marshallTest.c priorityTest.c timerTest.c \
	  ipcTest1.java module.java module1.java module1.java \
	  module2.java module3.java multiTest.c disconnectedListen.c \
	  ipcTest1.py module.py module1.py module2.py module3.py

PRIVATE_INCS	= latency.h module.h throughput.h

ifeq ($(shell uname),CYGWIN32_NT)
DEFINES		+= -D_WINSOCK_
endif

LIBRARIES +=  -lpthread

INC_PATH += -I../src
VX_INC   += -I../src

MISC	= GNUmakefile ds1-msgs.formats marshallTest.tcl \
	  intTest.lisp ipcTest1.lisp latency.lisp latency2.lisp module.lisp \
	  module1.lisp module2.lisp module3.lisp Windows

# All source files
SRCS_ALL	= $(INCS) $(PRIVATE_INCS) $(SRCS) $(MANS) $(DOCS) $(MISC)

LOCAL_LIBS	= $(patsubst %,$(LDIR)/%,$(PUBLIC_LIBS))

LOCAL_BINS	= 

# Files to be removed by 'make clean' and 'make realclean'
DELETES		= core $(DEPEND_FILE) \
	  	  $(LOCAL_BINS) $(LOCAL_LIBS) $(PUBLIC_LINTS) \
		  $(MODULE_LIBS) $(MODULE_LINTS) $(PRIVS) \
		  $(patsubst %,$(BDIR)/%,$(PRIVS)) \
		  $(patsubst %,$(ODIR)/%,$(OBJS)) make.state \
		  $(patsubst %,$(VODIR)/%,$(OBJS)) \
		  $(wildcard .nse_dep*) $(wildcard *.CKP) $(wildcard \#*\#) \
		  $(wildcard *~) $(wildcard .*~) $(wildcard *.CKP)\
	          $(wildcard *.BAK) $(wildcard .*.BAK) *.class *.pyc

# Files never to be removed, even if Make encounters an error
.PRECIOUS: $(SRCS_ALL) $(SUBDIRS)

##########
# Default Target
##########

all:: srcs libs bins privs

##########
# User Defined Targets
##########

# Define rules for all files listed in BINS, LIBS, and LINTS in this section.
# Example executable, object, library, and combined objects are shown below:
#
#  $(BDIR)/sample: file1.o file2.o $(LIBDEP)
#	$(LINK.c) -o $@ file1.o file2.o $(LIBRARIES)
#
#  $(ODIR)/alternateName.o: file1.c
#	$(COMPILE.c) -o $@ file1.c
#
#  $(LDIR)/libsample.a: file1.o file2.o
#	-$(RM) $@
#	$(AR) $(ARFLAGS) $@ file1.o file2.o
#	$(RANLIB) $@
#
#  $(ODIR)/combined.o: $(ODIR)/part1.o $(ODIR)/part2.o
#	$(LD) -r -o $@ $(ODIR)/part1.o $(ODIR)/part2.o; chmod a-x $@

ifdef THREADED
LIBRARIES += -lpthread
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/sizesTest: $(ODIR)/sizesTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/sizesTest.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/sizesTest: $(VODIR)/sizesTest.o
	$(LINK.vx) -o $@ $(VODIR)/sizesTest.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/ipcTest1: $(ODIR)/ipcTest1.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/ipcTest1.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/ipcTest1: $(VODIR)/ipcTest1.o
	$(LINK.vx) -o $@ $(VODIR)/ipcTest1.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/module1: $(ODIR)/module1.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/module1.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/module1: $(VODIR)/module1.o
	$(LINK.vx) -o $@ $(VODIR)/module1.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/module2: $(ODIR)/module2.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/module2.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/module2: $(VODIR)/module2.o
	$(LINK.vx) -o $@ $(VODIR)/module2.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/module3: $(ODIR)/module3.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/module3.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/module3: $(VODIR)/module3.o
	$(LINK.vx) -o $@ $(VODIR)/module3.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/latency1: $(ODIR)/latency1.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/latency1.o $(LIBRARIES) $(DYN_LIBRARIES) -lm
else
$(BDIR)/latency1: $(VODIR)/latency1.o
	$(LINK.vx) -o $@ $(VODIR)/latency1.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/latency2: $(ODIR)/latency2.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/latency2.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/latency2: $(VODIR)/latency2.o
	$(LINK.vx) -o $@ $(VODIR)/latency2.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/throughput1: $(ODIR)/throughput1.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/throughput1.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/throughput1: $(VODIR)/throughput1.o
	$(LINK.vx) -o $@ $(VODIR)/throughput1.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/throughput2: $(ODIR)/throughput2.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/throughput2.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/throughput2: $(VODIR)/throughput2.o
	$(LINK.vx) -o $@ $(VODIR)/throughput2.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/checkTest: $(ODIR)/checkTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/checkTest.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/checkTest: $(VODIR)/checkTest.o
	$(LINK.vx) -o $@ $(VODIR)/checkTest.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/intTest: $(ODIR)/intTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/intTest.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/intTest: $(VODIR)/intTest.o
	$(LINK.vx) -o $@ $(VODIR)/intTest.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/marshallTest: $(ODIR)/marshallTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/marshallTest.o $(LIBRARIES) $(DYN_LIBRARIES) -lm
else
$(BDIR)/marshallTest: $(VODIR)/marshallTest.o
	$(LINK.vx) -o $@ $(VODIR)/marshallTest.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/priorityTest: $(ODIR)/priorityTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/priorityTest.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/priorityTest: $(VODIR)/priorityTest.o
	$(LINK.vx) -o $@ $(VODIR)/priorityTest.o
endif

ifneq ($(THIS_OS),vxworks)
$(BDIR)/timerTest: $(ODIR)/timerTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/timerTest.o $(LIBRARIES) $(DYN_LIBRARIES)
else
$(BDIR)/timerTest: $(VODIR)/timerTest.o
	$(LINK.vx) -o $@ $(VODIR)/timerTest.o
endif

multiTest: $(ODIR)/multiTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/multiTest.o $(LIBRARIES) -lpthread \
	$(DYN_LIBRARIES)

lita.bug: $(ODIR)/lita.bug.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/lita.bug.o $(LIBRARIES) -lpthread \
	$(DYN_LIBRARIES)

$(BDIR)/multiTest: $(ODIR)/multiTest.o $(LIBDEP)
	$(LINK.c) -o $@ $(ODIR)/multiTest.o $(LIBRARIES) -lpthread \
	$(DYN_LIBRARIES)

IPC_CLASS_ROOT = ../java/build
IPC_CLASS_FILE = $(IPC_CLASS_ROOT)/ipc/java/IPC.class

ipcTest1j : ipcTest1.java $(IPC_CLASS_FILE) ../java/ipcjava.c
	javac -classpath .:$(IPC_CLASS_ROOT) ipcTest1.java

module1j : module.java module1.java $(IPC_CLASS_FILE) ../java/ipcjava.c
	javac -classpath .:$(IPC_CLASS_ROOT) module1.java

module2j : module.java module2.java $(IPC_CLASS_FILE) ../java/ipcjava.c
	javac -classpath .:$(IPC_CLASS_ROOT) module2.java

module3j : module.java module3.java $(IPC_CLASS_FILE) ../java/ipcjava.c
	javac -classpath .:$(IPC_CLASS_ROOT) module3.java

$(BDIR)/ipcTest1j : ipcTest1j
$(BDIR)/module1j : module1j
$(BDIR)/module2j : module2j
$(BDIR)/module3j : module3j

##########
# Include standard target definitions and RCS rules
##########

include $(STD_FILE)
