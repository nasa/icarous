# Copyright (c) 2006-2009, Universities Space Research Association (USRA).
# All rights reserved.
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

# Peforms batch translations of:
#  - Standard Plexil into Core Plexil
#  - Extended Plexil into Core Plexil
#  - 

SHELL = /bin/sh

CORE       = SimpleAssignment SimpleDrive DriveToTarget SafeDrive
CORE_PLE   = $(CORE:=.ple) 
CORE_PLX   = $(CORE_PLE:.ple=.plx) 
EXTENDED   = DriveToSchool

ALL = $(EXTENDED) $(CORE)

# Temporary file
DIFF = /tmp/plexil-$$$$

default: standard extended

# Compile all Standard Plexil (.ple) files.
standard: $(CORE_PLX)

extended:
	@ echo Translating Extended Plexil examples...
	@ for i in $(EXTENDED); do \
	        eplexil $$i.epx $$i.plx ; \
	  done
	@ echo Finished.

.SUFFIXES : .ple .plx

.ple.plx:
	../bin/plexil $<
