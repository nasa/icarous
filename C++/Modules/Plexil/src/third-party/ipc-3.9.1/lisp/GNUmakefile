#############################################################################
#       $Id: GNUmakefile,v 2.2 2009/01/12 15:54:55 reids Exp $
# $Revision: 2.2 $
#     $Date: 2009/01/12 15:54:55 $
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
# Revision 2.2  2009/01/12 15:54:55  reids
# Added BSD Open Source license info
#
# Revision 2.1.1.1  1999/11/23 19:07:37  reids
# Putting IPC Version 2.9.0 under local (CMU) CVS control.
#
# Revision 1.3  1996/03/12 03:10:37  reids
# Better support for "install"ing the fasls.
#
# Revision 1.2  1996/03/07 03:18:36  reids
# Don't make lib, obj, etc. subdirectories
#
# Revision 1.1  1996/03/04 22:31:28  reids
# Makefile for LISP version of IPC
#
#############################################################################

.KEEP_STATE:

#############################################################################
# Module configuration.
#############################################################################

INSTALL_DIR	= ..
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
MODULE_DIR	= lisp
PROJECT		= ipc
MODULE		= ipc
SUBDIRS		=
DEPEND_FILE	= $(PWD)/GNUmakefile.$(THIS_MACHINE)
#DEPEND_PREFIX	= $(ODIR)/ $(LODIR)/ $(SODIR)/ $(VODIR)/
VERSION_FILE	= ipcPriv.h
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

# Generated files - installed
PUBLIC_BINS_SunOS	= 
PUBLIC_BINS		= 
#Uncomment the next line if you want to build the lisp library.
PUBLIC_LIBS_LISP	= 
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
PRIVS	= 

IPCOBJS = ipc.fasl ipcForeignCalls.fasl
TCAOBJS = formatters.fasl primFmttrs.fasl
OBJS = $(IPCOBJS) $(TCAOBJS)

# Source files - not installed

IPC_SRCS            = ipc-system.lisp ipc-package.lisp ipcForeignCalls.lisp

TCA_SRCS       = formatters.lisp primFmttrs.lisp \
		 lwMacros.lisp mclMacros.lisp winMacros.lisp

SRCS            = $(IPC_SRCS) $(TCA_SRCS)

PRIVATE_INCS	= 

MISC	= GNUmakefile README INSTALL ipc-load.lisp ipc-compile.lisp

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
	          $(wildcard *.BAK) $(wildcard .*.BAK)

# Files never to be removed, even if Make encounters an error
.PRECIOUS: $(SRCS_ALL) $(SUBDIRS)

##########
# Default Target
##########

all:: privs

##########
# User Defined Targets
##########

LISP_BIN = /usr/misc/.allegro/bin/cl

FASLS = fasl

fasl: $(SRCS)
	$(LISP_BIN) -e "(progn (load \"ipc-compile\") (exit))"

install:: fasl

install_bins:: fasl

##########
# Include standard target definitions and RCS rules
##########

include $(STD_FILE)
