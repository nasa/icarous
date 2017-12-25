# Platform definitions for cross-compilation for VxWorks RTOS

# Copyright (c) 2006-2015, Universities Space Research Association (USRA).
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

ifndef WIND_BASE
$(error Required environment variable WIND_BASE is not set. Exiting.)
endif

ifndef CPU
$(error Required environment variable CPU is not set. Exiting.)
endif


# Default to the Gnu toolchain.
TOOL_FAMILY ?= gnu
TOOL ?= gnu

# work around an ordering bug in defs.library
TGT_DIR=$(WIND_BASE)/target
include $(WIND_BASE)/target/h/make/defs.library

##
## Plexil-specific customizations
##

#
# C/C++ compiler flag overrides
#

# Header file path for targets
INC_DIRS	+= $(WIND_BASE)/target/usr/h $(WIND_BASE)/target/usr/h/wrn/coreip $(WIND_BASE)/target/h/wrn/coreip

# Define this as a Real Time Process project

DEFINES		+= -DPLEXIL_VXWORKS -D__RTP__ -DCPU=$(CPU) -DTOOL=$(TOOL) -DTOOL_FAMILY=$(TOOL_FAMILY)

STANDARD_CFLAGS		+= $(CC_ARCH_SPEC)
STANDARD_CXXFLAGS	+= $(CC_ARCH_SPEC)

ifneq ($(PLEXIL_OPTIMIZE),)
VARIANT_CFLAGS		+= $(CC_OPTIM_NORMAL)
endif

# Kludge around some things the Wind River Workbench includes but the standard make includes don't
ifeq ($(CPU),PPC604)
CPU_FAMILY := PPC32
endif

ifeq ($(CPU),PPC32)
CPU_FAMILY := PPC32
endif

ifeq ($(CPU_FAMILY),PPC32)
STANDARD_CFLAGS		+= -mhard-float -mstrict-align -mregnames -ansi -mrtp -Wall  -MD -MP
STANDARD_CXXFLAGS	+= -mhard-float -mstrict-align -mregnames -ansi -mrtp -Wall  -MD -MP

# Compiler flags for shared libraries
POSITION_INDEPENDENT_CODE_FLAG	:= -fpic
endif

#
# Compiler/linker option overrides
#

# Compiler flag to pass an argument to the linker
LINKER_PASSTHROUGH_FLAG			:= -Wl,
# Linker flag for run-time library search path
RUNTIME_SHARED_LIBRARY_PATH_FLAG	:= -rpath
# Linker flag to construct shared library
SHARED_FLAGS				:= -shared
# Extension for shared library
SUFSHARE				:= .so
# Name of the library with the pthreads API
PTHREAD_LIB   	      	       		:= pthreadLib
