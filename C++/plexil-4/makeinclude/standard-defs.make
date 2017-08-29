# Copyright (c) 2006-2016, Universities Space Research Association (USRA).
#  All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are met:
#     * Redistributions of source code must retain the above copyright
#       notice, this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright
#       notice, this list of conditions and the following disclaimer in the
#       documentation and/or other materials provided with the distribution.
#     * Neither the name of the Universities Space Research Association nor the
#       names of its contributors may be used to endorse or promote products
#       derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
# WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
# OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
# ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
# USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

# Standard macro definitions for Plexil make files.
# Note that these are defaults -
# overrides (if any) are defined in the platform-*.make files in this directory.

ifeq ($(PLEXIL_HOME),)
$(error The environment variable PLEXIL_HOME is not set. Exiting.)
endif

# The location we're building into - may be overridden
TOP_DIR			?= $(PLEXIL_HOME)

# Where to install product files. 
# May be overridden, e.g. for cross-compilation.
PREFIX			?= $(PLEXIL_HOME)

# Which variant(s) to build by default
# These can be overridden at the command line or in the shell environment
PLEXIL_DEBUG		?= 1
PLEXIL_OPTIMIZED	?=
PLEXIL_STATIC		?=
ifeq ($(PLEXIL_STATIC),)
PLEXIL_SHARED		?= 1
else
PLEXIL_SHARED		?=
endif

##### Basic utilities and Unix commands

SHELL           = /bin/sh

# Delete files (recursively, forced)
RM		= /bin/rm -fr
# File system link
LN		= /bin/ln -s
# Directory list
LS              = /bin/ls
TAG		= etags -t
# Move a file
MV              = /bin/mv
# Make a directory
MKDIR           = /bin/mkdir
# Copy a file
CP              = /bin/cp -p

##### C/C++ compiler options.

##### *** FIXME: Most of these presume gcc,
##### *** but clang is default on OS X and the BSDs,
##### *** and cross-compilers could be anything.
##### *** Fortunately clang emulates gcc's option parsing.

# Compiler options

# Generating include file dependencies
DEPEND		= $(CXX) -MM

# Defines
DEFINES			:=

STANDARD_CFLAGS		:=
STANDARD_CXXFLAGS	:=

# Include path

SYSTEM_INC_DIRS	=
INC_DIRS	= . $(PLEXIL_HOME)/include
INCLUDES	= $(addprefix -isystem,$(SYSTEM_INC_DIRS)) $(addprefix -I,$(INC_DIRS))

# Compiler flags for shared libraries
POSITION_INDEPENDENT_CODE_FLAG	:= -fPIC

# Compiler flags for debug builds
#DEBUG_FLAGS	:= -ggdb # Not appropriate for OS X
DEBUG_FLAGS	:= -g
WARNING_FLAGS	:= -Wall

# Compiler flags for optimized builds
OPTIMIZE_FLAGS	:= -O3 -DPLEXIL_FAST

VARIANT_CFLAGS	=
ifneq ($(PLEXIL_DEBUG),)
VARIANT_CFLAGS	+= $(DEBUG_FLAGS) $(WARNING_FLAGS)
endif
ifneq ($(PLEXIL_OPTIMIZED),)
VARIANT_CFLAGS	+= $(OPTIMIZE_FLAGS)
endif

CFLAGS		+= $(DEFINES) $(STANDARD_CFLAGS) $(VARIANT_CFLAGS) $(INCLUDES)
CXXFLAGS	+= $(DEFINES) $(STANDARD_CXXFLAGS) $(VARIANT_CFLAGS) $(INCLUDES)

##### Library support

# Names the library that will be the product of this make.
# User must set this to be useful.
LIBRARY		=

# Where to put the new libraries
LIB_DIR		?= $(PREFIX)/lib

# Where to find previously built libraries
LIB_PATH	:= $(PREFIX)/lib
LIB_PATH_FLAGS	= $(foreach libdir,$(LIB_PATH),$(LIBRARY_PATH_SEARCH_FLAG)$(libdir))

LIBS		=
LIB_FLAGS	= $(foreach lib,$(LIBS),-l$(lib))

##### Executable support

# Names the executable that will be the product of this make.
# User must set this to be useful.
EXECUTABLE	=

# Where to store the resulting executable
BIN_DIR		?= $(PREFIX)/bin

#
# Linker
#

# Linker program

# KMD: Command for building archive libraries, if applicable.
AR		= ar

# Command for building shared libraries, if applicable.
LD		= $(CXX) $(CXXFLAGS) $(foreach flag,$(EXE_FLAGS),$(LINKER_PASSTHROUGH_FLAG)$(flag))

# Compiler flag to pass an argument to the linker
LINKER_PASSTHROUGH_FLAG			:= -Wl,
# Linker flag for link-time library search path
LIBRARY_PATH_SEARCH_FLAG		:= -L
LINKTIME_SHARED_LIBRARY_PATH_FLAG	:= -L
# Linker flag for static link-time library search path
STATIC_LIBRARY_PATH_FLAG		:= -L
# Linker flag for run-time library search path
RUNTIME_SHARED_LIBRARY_PATH_FLAG	:= -rpath
# Linker flag to construct shared library
SHARED_FLAGS				:= -shared
# Extension for shared library
SUFSHARE				:= .so
# Linker flag to construct statically linked executable
STATIC_EXE_FLAG				:= -Bstatic

EXE_FLAGS				=
ifneq ($(PLEXIL_STATIC),)
EXE_FLAGS				+= $(STATIC_EXE_FLAG)
endif

##### Java

# Choose appropriate default version of Java
ifeq ($(JAVA_HOME),)
JAVA	?= java
JAVAC	?= javac
JAR	?= jar
else
JAVA	?= $(JAVA_HOME)/bin/java
JAVAC	?= $(JAVA_HOME)/bin/javac
JAR	?= $(JAVA_HOME)/bin/jar
endif


##### Conveniences

# Default for CVS targets in svn.make; should be shadowed to be more useful.
SVN_FILES       = *

##### A pre-emptive strike against some 3rd party platform include files

all: plexil-default

include $(PLEXIL_HOME)/makeinclude/platform-defs.make

# Check here in case some platform include file (re)defines these

ifneq ($(PLEXIL_SHARED),)
ifneq ($(PLEXIL_STATIC),)
$(error PLEXIL_STATIC and PLEXIL_SHARED cannot both be true. Exiting.)
endif
endif

ifneq ($(PLEXIL_SHARED),)
VARIANT_CFLAGS	+= $(POSITION_INDEPENDENT_CODE_FLAG)
endif
