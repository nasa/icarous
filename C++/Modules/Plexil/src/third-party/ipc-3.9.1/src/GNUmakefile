#############################################################################
#       $Id: GNUmakefile,v 2.15 2011/08/17 15:58:06 reids Exp $
# $Revision: 2.15 $
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
# Revision 2.15  2011/08/17 15:58:06  reids
# Updated makefiles to include new Python stuff for tar'ing up
#
# Revision 2.14  2009/02/07 18:36:43  reids
# Updates for making libraries compatible with Java
#
# Revision 2.13  2009/01/12 15:54:55  reids
# Added BSD Open Source license info
#
# Revision 2.12  2003/02/13 20:38:41  reids
# Updated for ACL6.0
#
# Revision 2.11  2002/06/25 16:34:16  reids
# Added multiThread.{h,c} so that it is included in the tar file
#
# Revision 2.10  2002/04/29 22:24:03  trey
# numerous small fixes
#
# Revision 2.9  2002/01/09 19:17:33  reids
# Forgot to remove -DCHECKMALLOC
#
# Revision 2.8  2002/01/03 20:52:09  reids
# Version of IPC now supports multiple threads (Caveat: Currently only
#   tested for Linux).
# Also some minor changes to support Java version of IPC.
#
# Revision 2.7  2001/08/28 13:36:30  reids
# Currently, access control works only for Linux.
#
# Revision 2.6  2001/08/13 13:39:00  reids
# Added support for access control (using /etc/hosts.allow, /etc/hosts.deny)
#
# Revision 2.5  2001/01/10 15:32:49  reids
# Added the function IPC_subscribeData that automatically unmarshalls
#   the data before invoking the handler.
#
# Revision 2.4  2000/12/14 22:12:50  reids
# Added (back) in ability to compile shared libraries (using:
#  make MAKE_SHARED_LIBS=1 ...)
#
# Revision 2.3  2000/08/25 13:46:31  reids
# Now "gmake ftp" includes wsocksrt.c
#
# Revision 2.2  2000/08/14 21:28:32  reids
# Added support for making under Windows.
#
# Revision 2.1.1.1  1999/11/23 19:07:36  reids
# Putting IPC Version 2.9.0 under local (CMU) CVS control.
#
# Revision 1.5.2.1  1996/10/29 22:35:09  udo
# *** empty log message ***
#
# Revision 1.5  1996/05/24 16:45:46  reids
# Removed all (most?) of the task-tree related code from the IPC build.
#
# Revision 1.4  1996/05/09 01:01:08  reids
# Moved all the TCA files over to the IPC directory.
# Fixed problem with sending NULL data.
# Added function IPC_setCapacity.
# VxWorks m68k version released.
#
# Revision 1.3  1996/04/24 19:13:49  reids
# Changes to support vxworks version.
#
# Revision 1.2  1996/03/12 03:33:45  reids
# Support for makeing the LISP Solaris version of IPC
#
# Revision 1.1  1996/03/03 04:36:14  reids
# First release of IPC files.  Corresponds to IPC Specifiction 2.2, except
# that IPC_readData is not yet implemented.  Also contains "cover" functions
# for the xipc interface.
#
#
#############################################################################

.KEEP_STATE:

#############################################################################
# Module configuration.
#############################################################################

INSTALL_DIR	= ..
BASE_DIR	:= $(shell cd $(INSTALL_DIR);/bin/pwd)
MODULE_DIR	= src
PROJECT		= ipc
MODULE		= ipc
SUBDIRS		=
DEPEND_FILE	= $(PWD)/GNUmakefile.$(THIS_MACHINE)
DEPEND_PREFIX	= $(ODIR)/ $(SODIR)/
VERSION_FILE	= ipcPriv.h
VERSION_PREFIX	= IPC

# Uncomment the following line to make the libraries useable by Allegro Common Lisp
#USE_LISP	= 1
# Uncomment the following line to make the libraries useable by Java for 64 bit machines
#USE_JAVA	= 1

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
ifneq ($(THIS_OS),vxworks)
PUBLIC_BINS		= central  $(PUBLIC_BINS_$(THIS_OS))
endif

ifdef USE_LISP
PUBLIC_LIBS_Solaris	= libipc_lisp.so libipc_lisp.sa
PUBLIC_LIBS_Linux	= libipc_lisp.so libipc_lisp.sa
endif

PUBLIC_LIBS_RISC	= 
ifneq ($(THIS_OS),vxworks)
PUBLIC_LIBS		= libipc.a $(PUBLIC_LIBS_$(THIS_OS)) \
			$(PUBLIC_LIBS_$(THIS_MACHINE))
else
PUBLIC_LIBS		= ipcModule.o centralModule.o
endif
ifdef MAKE_SHARED_LIBS
PUBLIC_LIBS	+= libipc.so.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR) \
		   libipc.sa.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR)
CFLAGS += -fPIC
endif

PUBLIC_LINTS		= 

MODULE_LIBS		= 
MODULE_LINTS		= 

# Source files - installed
PUBLIC_INCS		= ipc.h
PUBLIC_MANS		= 
PUBLIC_DOCS		= 

MODULE_INCS		= 
MODULE_DOCS		= 

INCS			= $(MODULE_INCS) $(PUBLIC_INCS)

# Generated files - not installed
PRIVS	= 

LIBOBJS = ipc.o queryResponse.o marshall.o timer.o \
	  list.o behaviors.o tcModError.o \
	  datamsg.o formatters.o hash.o idtable.o key.o \
	  primFmttrs.o reg.o sendMsg.o tcaMem.o tcaRef.o \
	  comModule.o com.o globalM.o globalMUtil.o strList.o \
	  modLogging.o modVar.o resMod.o \
	  parseFmttrs.o lex.o printData.o 
ifdef USE_LISP
LIBOBJS += ipcLisp.o
endif

VXLIBOBJS = $(patsubst %,$(VODIR)/%,$(LIBOBJS))

MODDATA = globalM.o

MODULEOBJS = $(patsubst %,$(ODIR)/%,$(LIBOBJS))
ifneq ($(THIS_OS),vxworks)
SHAREDOBJS = $(patsubst %,$(SODIR)/%,$(LIBOBJS))
endif
MODULEDATA = $(patsubst %,$(ODIR)/%,$(MODDATA))

SERVOBJS = comServer.o dispatch.o msgTap.o \
	recvMsg.o res.o \
	tcerror.o logging.o \
	globalS.o centralIO.o globalVar.o centralMain.o

SERVEROBJS = $(patsubst %,$(ODIR)/%,$(SERVOBJS))

VXSERVEROBJS = $(patsubst %,$(VODIR)/%,$(SERVOBJS))

OBJS	= $(MODULEOBJS) $(SHAREDOBJS) $(SERVEROBJS) \
	  $(VXSERVEROBJS) $(VXLIBOBJS)

# Source files - not installed

IPC_SRCS            = ipc.c queryResponse.c marshall.c timer.c multiThread.c

ifdef THREADED
DEFINES  += -DTHREADED
LIBOBJS  += multiThread.o
LIBRARIES += -lpthread
endif

TCA_SRCS        = list.c behaviors.c tcModError.c \
		  datamsg.c formatters.c hash.c idtable.c key.c \
		  primFmttrs.c reg.c sendMsg.c tcaMem.c tcaRef.c \
		  comModule.c com.c globalM.c globalMUtil.c strList.c \
		  modLogging.c modVar.c resMod.c \
		  parseFmttrs.c lex.c printData.c ipcLisp.c ipcFFI.c
ifeq ($(shell uname),CYGWIN32_NT)
TCA_SRCS	+= wsocksrt.c
LIBOBJS		+= wsocksrt.o
DEFINES		+= -D_WINSOCK_
endif

SERVSRCS = comServer.c dispatch.c recvMsg.c res.c \
        tcerror.c logging.c \
        msgTap.c globalS.c centralIO.c globalVar.c centralMain.c 

SRCS            = $(IPC_SRCS) $(TCA_SRCS) $(SERVSRCS)

PRIVATE_INCS	= ipcPriv.h multiThread.h ipcFFI.h \
	basics.h libc.h tca.h tcaInternal.h behaviors.h com.h comModule.h \
	comServer.h datamsg.h dispatch.h formatters.h \
	globalM.h globalS.h hash.h idtable.h key.h lex.h \
	logging.h msgTap.h parseFmttrs.h primFmttrs.h \
	printData.h recvMsg.h reg.h res.h resMod.h \
	sendMsg.h tcaMem.h tcaRef.h tcerror.h centralIO.h \
	globalVar.h modVar.h modLogging.h list.h strList.h centralMsg.h 

MISC	= GNUmakefile README INSTALL Windows wsocksrt.c

# All source files
SRCS_ALL	= $(INCS) $(PRIVATE_INCS) $(SRCS) $(MANS) $(DOCS) $(MISC)

LOCAL_LIBS	= $(patsubst %,$(LDIR)/%,$(PUBLIC_LIBS))

LOCAL_BINS	= 

# Files to be removed by 'make clean' and 'make realclean'
DELETES		= core $(DEPEND_FILE) \
	  	  $(LOCAL_BINS) $(LOCAL_LIBS) $(PUBLIC_LINTS) \
		  $(MODULE_LIBS) $(MODULE_LINTS) $(PRIVS) make.state \
                  $(ODIR) $(BDIR) $(LDIR) $(SODIR)

# Files never to be removed, even if Make encounters an error
.PRECIOUS: $(SRCS_ALL) $(SUBDIRS)

##########
# Default Target
##########

.PHONY: all
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

ifneq ($(THIS_OS),vxworks)
$(BDIR)/central:: $(SERVEROBJS) $(LIBDEP)
	@[ -d $(BDIR) ] || $(MKDIR) $(BDIR)
	$(LINK.c) -o $@ $(SERVEROBJS) $(LIBRARIES) $(DYN_LIBRARIES)
else
$(LDIR)/centralModule.o: $(VXSERVEROBJS)
	$(LINK.vx) -o $@ $(VXSERVEROBJS)
$(PUBLIC_LIB_DIR)/centralModule.o: $(LDIR)/centralModule.o
endif

$(PUBLIC_BIN_DIR)/central: $(SERVEROBJS) $(LIBDEP)

ifneq ($(THIS_OS),vxworks)
$(LDIR)/libipc.a: $(MODULEOBJS)
	@[ -d $(LDIR) ] || $(MKDIR) $(LDIR)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $(MODULEOBJS)
	$(RANLIB) $@
else
$(LDIR)/ipcModule.o: $(VXLIBOBJS)
	rm -f $(LDIR)ipcModule.o
	$(VX_AR) rvu $(LDIR)/ipcModule.o $(VXLIBOBJS)
	$(VX_RANLIB) $(LDIR)/ipcModule.o

$(PUBLIC_LIB_DIR)/ipcModule.o: $(LDIR)/ipcModule.o
endif

$(PUBLIC_LIB_DIR)/libipc.a: $(MODULEOBJS)

$(LDIR)/libipc.so.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR): $(SHAREDOBJS)
	$(LD_SHARED) -o \
	$(LDIR)/libipc.so.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR) \
		$(SHAREDOBJS) $(LIB_PATH) $(LIBS_$(THIS_OS))

$(PUBLIC_LIB_DIR)/libipc.so.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR): $(SHAREDOBJS)

$(LDIR)/libipc.sa.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR): $(MODULEDATA)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $(MODULEDATA)
	$(RANLIB) $@

$(PUBLIC_LIB_DIR)/libipc.sa.$(IPC_VERSION_MAJOR).$(IPC_VERSION_MINOR): $(MODULEDATA)

ifeq ($(THIS_MACHINE),sun4u)
LIBACL	= /usr/local/lib/acl60/libacl601.so
endif

$(LDIR)/libipc_lisp.so: $(SHAREDOBJS)
	$(LD_SHARED) -o $@ $(SHAREDOBJS) $(LIB_PATH) $(LIBACL)

$(PUBLIC_LIB_DIR)/libipc_lisp.so : $(SHAREDOBJS)

$(LDIR)/libipc_lisp.sa: $(MODULEDATA)
	$(RM) $@
	$(AR) $(ARFLAGS) $@ $(MODULEDATA)
	$(RANLIB) $@

$(PUBLIC_LIB_DIR)/libipc_lisp.sa: $(MODULEDATA)

# this dependency is used to update the version information.
# $(ODIR)/ipc.o: $(SRCS_ALL)

# $(LODIR)/ipc.o: $(SRCS_ALL)

#
# Targets for building a release
#

ifdef USE_JAVA
CFLAGS += -fPIC
endif

ifdef USE_LISP
DEFINES		+= -DLISP -DALLEGRO
ifeq ($(THIS_OS),SunOS)
LISP_INC_DIR	= /usr/misc/.allegro/lib/misc
else
ifeq ($(THIS_OS),Solaris)
LISP_INC_DIR	= /afs/cs/local/allegro/sun4_41/beta/lib/misc
else
LISP_INC_DIR	= .
endif
endif
FOUND_LISP := $(shell ls $(LISP_INC_DIR)/lisp.h 2>/dev/null | grep -v "not found" | head -1)
INC_PATH += -I$(LISP_INC_DIR)

ifeq ($(FOUND_LISP),)
$(ODIR)/ipcLisp.o::
	echo "error: CANNOT FIND lisp.h IN $(LISP_INC_DIR)"
	echo "  Need to change LISP_INC_DIR in GNUmakefile"
	foo # Use this to cause gmake to exit
endif

ifeq ($(THIS_OS),wnt)
DEFINES += -DPC_ALLEGRO
endif

endif

ifdef ACCESS_CONTROL
ifeq ($(THIS_OS),Linux)
DEFINES += -DACCESS_CONTROL
LIBRARIES += -lwrap -lnsl
else # Currently, access control only works for Linux
$(ODIR)/comServer.o::
	@echo ERROR: Access control available only for Linux
	@exit 1
endif
endif

##########
# Include standard target definitions and RCS rules
##########

include $(STD_FILE)
