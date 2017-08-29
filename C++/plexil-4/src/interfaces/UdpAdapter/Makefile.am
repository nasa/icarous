# Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

lib_LTLIBRARIES = libUdpUtils.la libUdpAdapter.la
include_HEADERS = MessageQueueMap.hh UdpAdapter.hh udp-utils.hh
libUdpUtils_la_SOURCES = udp-utils.cc
libUdpUtils_la_CPPFLAGS = -I@top_srcdir@/utils

libUdpAdapter_la_SOURCES = MessageQueueMap.cc UdpAdapter.cc
libUdpAdapter_la_LIBADD = libUdpUtils.la
libUdpAdapter_la_CPPFLAGS = -I@top_srcdir@/interfaces/UpdUtils \
 -I@top_srcdir@/third-party/pugixml/src -I@top_srcdir@/app-framework \
 -I@top_srcdir@/exec -I@top_srcdir@/intfc -I@top_srcdir@/expr \
 -I@top_srcdir@/value -I@top_srcdir@/utils

# TODO: libraries to link against (?)

if MODULE_TESTS_OPT
  noinst_PROGRAMS = test/udp-tests
  test_udp_tests_SOURCES = test/udp-tests.cc
  test_udp_tests_CPPFLAGS = -I@top_srcdir@/utils
  test_udp_tests_LDADD = libUdpUtils.la ../../utils/libPlexilUtils.la
endif
