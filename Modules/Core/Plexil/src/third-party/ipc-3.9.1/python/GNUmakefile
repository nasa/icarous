#############################################################################
#       $Id: GNUmakefile,v 1.2 2011/08/17 15:58:06 reids Exp $
# $Revision: 1.2 $
#     $Date: 2011/08/17 15:58:06 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# PROJECT:	NM-DS1
#
# FILE:		GNUmakefile
#		Based on TCA GNUmakefile (Richard Goodwin)
#
# DESCRIPTION: Makes the PYTHON version of IPC.
#              Make sure to set PYTHONPATH to include paths to
#              both ipc/python and ipc/lib/<THIS_OS_BOTH>
#              e.g. setenv PYTHONPATH $IPC_DIR/python:$IPC_DIR/lib/Linux-2.6
#
# EXPORTS:
#	The following targets are available as part of GNUmakefile.std:
#		all bins libs privs objs
#		clean install
#		lint depend
#
#
# HISTORY:
#
# $Log: GNUmakefile,v $
# Revision 1.2  2011/08/17 15:58:06  reids
# Updated makefiles to include new Python stuff for tar'ing up
#
# Revision 1.1  2011/08/16 16:00:36  reids
# Adding Python interface to IPC
#
#############################################################################

.KEEP_STATE:

#############################################################################
# Module configuration.
#############################################################################

INSTALL_DIR	= ..
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
MODULE_DIR	= python
PROJECT		= ipc
MODULE		= ipc
SUBDIRS		=
DEPEND_FILE	= $(PWD)/GNUmakefile.$(THIS_MACHINE)
DEPEND_PREFIX	= $(ODIR)/ $(SODIR)/
VERSION_FILE	= ../src/ipcPriv.h
VERSION_PREFIX	= IPC

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

PYTHONLIB_PREFIX	= _IPC
ifeq ($(THIS_OS),wnt)
PYTHONLIB_POSTFIX	= dll
else
PYTHONLIB_POSTFIX	= so
endif

# Generated files - installed
PUBLIC_BINS_SunOS	= 
PUBLIC_BINS		= 

PYTHONLIB		= $(PYTHONLIB_PREFIX).$(PYTHONLIB_POSTFIX)

PUBLIC_LIBS_RISC	= 
PUBLIC_LIBS	     	= $(PYTHONLIB)

PUBLIC_LINTS		= 

MODULE_LIBS		= 
MODULE_LINTS		= 

# Source files - installed
PUBLIC_INCS		= 
PUBLIC_MANS		= 
PUBLIC_DOCS		= 

MODULE_INCS		= 
MODULE_DOCS		= 

# To eliminate warnings (from SWIG-generated code) about type-punning
CFLAGS			+= -fno-strict-aliasing

#install_libs		:: formatters.class primFmttrs.class IPC.class
CFLAGS			+= -fPIC -O2
ifndef PYTHON_INC_DIR
PYTHON_INC_DIR		= /usr/include/python2.5
endif
ifndef PYTHON_LIB_DIR
PYTHON_LIB_DIR		= /usr/lib/python2.5
endif
INC_PATH		+= -I$(PYTHON_INC_DIR)

# Needed for jni_md.h
ifeq ($(THIS_OS),Linux)
#INC_PATH		+= -I/usr/java/jdk1.3.1/include/linux
else
ifeq ($(THIS_OS),Solaris)
INC_PATH		+= -I/usr/local/include
endif
endif
INC_PATH		+= -I../src

INCS			= $(MODULE_INCS) $(PUBLIC_INCS)

# Generated files - not installed
PRIVS	= 

_OBJS = IPC_wrap.o

OBJS = $(patsubst %,$(ODIR)/%,$(_OBJS))

# Source files - not installed

SRCS            = IPC.i formatters.py primFmttrs.py ipcPython.py test.py \
		  ipcPython.h ipcPython.c

PRIVATE_INCS	= 

MISC	= GNUmakefile README

# All source files
SRCS_ALL	= $(INCS) $(PRIVATE_INCS) $(SRCS) $(MANS) $(DOCS) $(MISC)

LOCAL_LIBS	= $(patsubst %,$(LDIR)/%,$(PUBLIC_LIBS))

LOCAL_BINS	= 

# Files to be removed by 'make clean' and 'make realclean'
DELETES		= core $(DEPEND_FILE) \
	  	  $(LOCAL_BINS) $(LOCAL_LIBS) $(PUBLIC_LINTS) \
		  $(MODULE_LIBS) $(MODULE_LINTS) $(PRIVS) $(OBJS) make.state \
		  $(wildcard .nse_dep*) $(wildcard *.CKP) $(wildcard \#*\#) \
		  $(wildcard *~) $(wildcard .*~) $(wildcard *.CKP)\
	          $(wildcard *.BAK) $(wildcard .*.BAK) *.class $(GEN_HEADERS) \
		  IPC.py *.pyc IPC_wrap.c

# Files never to be removed, even if Make encounters an error
.PRECIOUS: $(SRCS_ALL) $(SUBDIRS)

##########
# Default Target
##########

all:: srcs libs bins privs

# The vx works compiler constants are in etc/GNUmakefile.defs.
#vx:  $(VODIR)/.force $(LDIR)/.force \
#	$(PUBLIC_LIB_DIR)/ipcModule.o $(PUBLIC_LIB_DIR)/centralModule.o

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

IPC_wrap.c : IPC.i ../src/ipcFFI.h ../src/ipcFFI.c \
	     ipcPython.h ipcPython.c ipcPython.py
	swig -python $(INC_PATH) -outcurrentdir $<

ifeq ($(THIS_OS),wnt)
$(LDIR)/$(PYTHONLIB): $(OBJS)
	$(RM) $@
	$(LD_SHARED) -o $@ $(OBJS) -L$(PYTHON_LIB_DIR)/config -lpython2.5 \
	  $(LIB_PATH) -lipc
else
$(LDIR)/$(PYTHONLIB): $(OBJS)
	$(RM) $@
	cd ../src; $(MAKE) MAKE_SHARED_LIBS=1 install; cd ../python
	$(LD_SHARED) -o $@ $(OBJS) \
	  $(PUBLIC_LIB_DIR)/libipc.so.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR)
endif

# this dependency is used to update the version information.
# $(ODIR)/ipc.o: $(SRCS_ALL)

# $(LODIR)/ipc.o: $(SRCS_ALL)

#
# Targets for building a release
#

##########
# Include standard target definitions and RCS rules
##########

include $(STD_FILE)
