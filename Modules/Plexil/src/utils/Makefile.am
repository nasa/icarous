# Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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

lib_LTLIBRARIES = libPlexilUtils.la

libPlexilUtils_la_CPPFLAGS =

include_HEADERS = ConstantMacros.hh Debug.hh Error.hh \
 LinkedQueue.hh Logging.hh ParserException.hh PlanError.hh SimpleMap.hh \
 TestSupport.hh bitsetUtils.hh lifecycle-utils.h map-utils.hh \
 stricmp.h timespec-utils.hh timeval-utils.hh utils_main_page.hh

libPlexilUtils_la_SOURCES = Error.cc Logging.cc ParserException.cc \
 PlanError.cc bitsetUtils.cc lifecycle-utils.c stricmp.c timespec-utils.cc \
 timeval-utils.cc

if JNI_OPT
  include_HEADERS += JNIHelper.hh JNIUtils.hh ScopedOstreamRedirect.hh
  libPlexilUtils_la_SOURCES += JNIHelper.cc JNIUtils.cc
endif

if THREADS_OPT
  include_HEADERS += RecursiveThreadMutex.hh ThreadMutex.hh ThreadSemaphore.hh \
 ThreadSpawn.hh
  libPlexilUtils_la_SOURCES += RecursiveThreadMutex.cc ThreadMutex.cc ThreadSemaphore.cc \
 ThreadSpawn.cc
endif

if PLEXIL_DLFCN_H
  include_HEADERS += DynamicLoader.h
  libPlexilUtils_la_SOURCES += DynamicLoader.cc
endif

if DEBUG_LOGGING_OPT
  include_HEADERS += DebugMessage.hh
  libPlexilUtils_la_SOURCES += DebugMessage.cc
endif

if MODULE_TESTS_OPT
  bin_PROGRAMS = test/utils-module-tests
  noinst_HEADERS = test/TestData.hh test/util-test-module.hh
  test_utils_module_tests_SOURCES = test/bitsetUtilsTest.cc test/LinkedQueueTest.cc \
 test/SimpleMapTest.cc test/TestData.cc test/util-test-module.cc test/module-tests.cc
  test_utils_module_tests_LDADD = libPlexilUtils.la
if JNI_OPT
    noinst_HEADERS += test/jni-adapter.hh
    test_utils_module_tests_SOURCES += test/jni-adapter.cc
endif
endif

