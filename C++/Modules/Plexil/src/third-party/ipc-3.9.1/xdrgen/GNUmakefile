#############################################################################
#       $Id: GNUmakefile,v 1.7 2011/08/17 01:11:43 reids Exp $
# $Revision: 1.7 $
#     $Date: 2011/08/17 01:11:43 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# PROJECT:	NM-DS1
#
# FILE:		GNUmakefile
#		Based on TCA GNUmakefile (Richard Goodwin)
#
# DESCRIPTION: Makes the C-Lanaguage IPC API for NMP DS-1.
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
# Revision 1.7  2011/08/17 01:11:43  reids
# Took out unused and LISP-specific functions
#
# Revision 1.6  2009/01/12 15:54:59  reids
# Added BSD Open Source license info
#
# Revision 1.5  2004/04/06 15:06:08  trey
# updated for more recent bison and g++
#
# Revision 1.4  2002/05/09 01:46:24  trey
# added --lang=lisp option for xdr
#
# Revision 1.3  2002/04/29 22:23:06  trey
# minor makefile improvements
#
# Revision 1.2  2001/03/16 21:09:20  reids
# Forgot to add the GNUmakefile itself for the ftp make
#
# Revision 1.1  2001/03/16 17:56:00  reids
# Release of Trey's code to generate IPC format strings from XDR definitions.
#
#
#############################################################################

.KEEP_STATE:

#############################################################################
# Module configuration.
#############################################################################

INSTALL_DIR	= ..
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
MODULE_DIR	= xdrgen
PROJECT		= ipc
MODULE		= xdrgen
SUBDIRS		=
DEPEND_FILE	= $(PWD)/GNUmakefile.$(THIS_MACHINE)
DEPEND_PREFIX	= $(ODIR)/
VERSION_FILE	= 
VERSION_PREFIX	= 

#############################################################################
# Standard Makefile Includes
#############################################################################

ifndef FAST_COMPILE
PWD 	:= $(shell /bin/pwd)
endif

ifndef STD_FILE
STD_FILE := $(shell ls $(BASE_DIR)/etc/GNUmakefile.std | grep -v "not found")
ifeq ($(STD_FILE),)
STD_FILE := $(PROJECT_DIR)/etc/GNUmakefile.std
endif
DEFS_FILE := $(shell ls $(BASE_DIR)/etc/GNUmakefile.defs | grep -v "not found")
ifeq ($(DEFS_FILE),)
DEFS_FILE := $(PROJECT_DIR)/etc/GNUmakefile.defs
endif
endif

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
PUBLIC_BINS		= xdrgen
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
PRIVS	= 

_OBJECTS = XDR.yy.o XDR.tab.o strxcpy.o runCPP.o XDRTree.o XDRParser.o \
	   ParsePos.o ParseError.o XDROutputHeader.o XDROutputLisp.o \
	   XDRIterator.o XDROutputSpec.o XDRMapNames.o XDRFormatIPC.o xdrgen.o

OBJS            = $(patsubst %,$(ODIR)/%,$(_OBJECTS))

# Source files - not installed

SRCS            = strxcpy.c runCPP.cc XDRTree.cc \
		XDRParser.cc ParsePos.cc ParseError.cc XDROutputHeader.cc \
                XDROutputLisp.cc \
	   	XDRIterator.cc XDROutputSpec.cc XDRMapNames.cc XDRFormatIPC.cc \
		XDR.l XDR.y xdrgen.cc

PRIVATE_INCS	= ParseError.h ParsePos.h RefCounter.h XDRFormatIPC.h \
		XDRInternal.h XDRIterator.h XDRMapNames.h XDROutputHeader.h \
                XDROutputLisp.h \
		XDROutputSpec.h XDRParseNode.h XDRParser.h XDRTree.h \
		runCPP.h strxcpy.h xdrgen.h

MISC	= example.xdr xdrgen.html GNUmakefile

# All source files
SRCS_ALL	= $(INCS) $(PRIVATE_INCS) $(SRCS) $(MANS) $(DOCS) $(MISC)

LOCAL_LIBS	= $(patsubst %,$(LDIR)/%,$(PUBLIC_LIBS))

LOCAL_BINS	= 

# Files to be removed by 'make clean' and 'make realclean'
DELETES	+= gen/*

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

LEX = flex
YACC = bison

gen/XDR.yy.cc: XDR.l gen/XDR.tab.cc
	@[ -d gen ] || $(MKDIR) gen
	$(LEX) -f -Ca -ogen/XDR.yy.cc XDR.l

gen/XDR.tab.cc gen/XDR.tab.hh: XDR.y
	@[ -d gen ] || $(MKDIR) gen
	$(YACC) --defines XDR.y -o gen/XDR.tab.cc
	@[ -e gen/XDR.tab.cc.h ] && cp gen/XDR.tab.cc.h gen/XDR.tab.hh

$(ODIR)/XDR.yy.o: gen/XDR.yy.cc gen/XDR.tab.hh
	@[ -d $(ODIR) ] || $(MKDIR) $(ODIR)
	$(COMPILE.cc) -o $@ $<

$(ODIR)/XDR.tab.o: gen/XDR.tab.cc
	@[ -d $(ODIR) ] || $(MKDIR) $(ODIR)
	$(COMPILE.cc) -o $@ $<

LIBRARIES = 

$(BDIR)/xdrgen: $(OBJS) $(LIBDEP)
	@[ -d $(BDIR) ] || $(MKDIR) $(BDIR)
	$(LINK.cc) -o $@ $(OBJS) $(LIBRARIES)

##########
# Include standard target definitions and RCS rules
##########

include $(STD_FILE)
