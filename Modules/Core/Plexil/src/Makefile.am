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

ACLOCAL_AMFLAGS = -I m4

include_HEADERS = plexil-config.h

if GANTT_OPT
  MAYBE_GANTT_SUBDIRS = interfaces/GanttListener
endif

if IPC_OPT
  MAYBE_IPC_SUBDIRS = third-party/ipc interfaces/IpcUtils interfaces/IpcAdapter
endif

if SAS_OPT
  MAYBE_SAS_SUBDIRS = apps/StandAloneSimulator
endif

if TEST_EXEC_OPT
  MAYBE_TEST_EXEC_SUBDIRS = apps/TestExec
endif

if UDP_OPT
  MAYBE_UDP_SUBDIRS = interfaces/UdpAdapter
endif

if UNIVERSAL_EXEC_OPT
  MAYBE_UNIVERSAL_EXEC_SUBDIRS = universal-exec
endif

if VIEWER_OPT
  MAYBE_VIEWER_SUBDIRS = interfaces/Sockets interfaces/LuvListener
endif

SUBDIRS = utils value expr intfc exec \
 third-party/pugixml/src xml-parser app-framework \
 interfaces/Launcher interfaces/PlanDebugListener \
 $(MAYBE_IPC_SUBDIRS) $(MAYBE_VIEWER_SUBDIRS) $(MAYBE_SAS_SUBDIRS) \
 $(MAYBE_TEST_EXEC_SUBDIRS) $(MAYBE_UDP_SUBDIRS) $(MAYBE_UNIVERSAL_EXEC_SUBDIRS) \
 $(MAYBE_GANTT_SUBDIRS)

