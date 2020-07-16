#############################################################################
#       $Id: GNUmakefile,v 2.5 2009/01/15 16:09:46 reids Exp $
# $Revision: 2.5 $
#     $Date: 2009/01/15 16:09:46 $
#   $Author: reids $
#    $State: Exp $
#   $Locker:  $
#
# PROJECT:	IPC
#
# FILE:		GNUmakefile
#		Based on Erebus prototypical Makefile
#		Makefile.proto,v 1.1 1992/06/16 22:50:21 jwest Exp 
#
# DESCRIPTION: Makes IPC documentation.
#
# Copyright (c) 2008, Carnegie Mellon University
#     This software is distributed under the terms of the 
#     Simplified BSD License (see ipc/LICENSE.TXT)
#
# HISTORY:
#
# $Log: GNUmakefile,v $
# Revision 2.5  2009/01/15 16:09:46  reids
# No longer creating ps file
#
# Revision 2.4  2009/01/12 15:54:54  reids
# Added BSD Open Source license info
#
# Revision 2.3  2003/02/13 20:34:52  reids
# CStore postscrip file gzip'd
# VS: ----------------------------------------------------------------------
#
# Revision 2.2  2001/03/16 21:02:39  reids
# Documentation about Trey's xdrgen program.
#
# Revision 2.1.1.1  1999/11/23 19:07:38  reids
# Putting IPC Version 2.9.0 under local (CMU) CVS control.
#
#############################################################################

INSTALL_DIR	= ..
ifndef BASE_DIR
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
endif
MODULE_DIR	= doc
PROJECT		= ipc
MODULE		= ipc
SUBDIRS		=

#############################################################################
# Where are the standard files?
#############################################################################

ifndef FAST_COMPILE
PWD 	:= $(shell /bin/pwd)
endif

STD_FILE := $(shell ls $(BASE_DIR)/etc/GNUmakefile.std | grep -v "not found")
DEFS_FILE := $(shell ls $(BASE_DIR)/etc/GNUmakefile.defs | grep -v "not found")

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
PUBLIC_BINS		= 
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

OBJS	= 

# Source files - not installed

SRCS		= 

PRIVATE_INCS	= 

MISC		=  GNUmakefile ipc.book cover.doc ipc.doc ipcIX.doc ipcTOC.doc \
		   IPC_Manual.pdf xdrgen.html

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

##########
# Include standard target definitions and RCS rules
##########

#ipc.ps.gz: ipc.ps
#	gzip -f ipc.ps

include $(STD_FILE)
