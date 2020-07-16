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

AUTOMAKE_OPTIONS = subdir-objects

lib_LTLIBRARIES = libstandalonesimulator.la

libstandalonesimulator_la_CPPFLAGS = -I@top_srcdir@/interfaces/IpcUtils \
 -I@top_srcdir@/third-party/ipc/src -I@top_srcdir@/value -I@top_srcdir@/utils

include_HEADERS = CommRelayBase.hh GenericResponse.hh IpcCommRelay.hh \
 ResponseBase.hh ResponseFactory.hh ResponseMessage.hh ResponseMessageManager.hh Simulator.hh \
 SimulatorScriptReader.hh TelemetryResponseManager.hh TimingService.hh simdefs.hh

libstandalonesimulator_la_SOURCES = IpcCommRelay.cc \
 ResponseBase.cc ResponseFactory.cc ResponseMessage.cc ResponseMessageManager.cc \
 Simulator.cc SimulatorScriptReader.cc TelemetryResponseManager.cc TimingService.cc

bin_PROGRAMS = simulator

noinst_HEADERS = PlexilSimulator/PlexilSimResponseFactory.hh

simulator_SOURCES = PlexilSimulator/PlexilSimResponseFactory.cc \
 PlexilSimulator/PlexilSimulator.cc
simulator_CPPFLAGS = -I@top_srcdir@/interfaces/IpcUtils \
 -I@top_srcdir@/third-party/ipc/src -I@top_srcdir@/value -I@top_srcdir@/utils

simulator_LDADD = libstandalonesimulator.la \
 ../../interfaces/IpcUtils/libIpcUtils.la \
 ../../value/libPlexilValue.la ../../utils/libPlexilUtils.la

simulator_LDFLAGS = -L../../../lib -lipc
