#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

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

AC_PREREQ(2.59)
AC_INIT(Plexil, 4.5.0a1, plexil-support@sourceforge.net, plexil, https://plexil.wiki.sourceforge.net/)
AC_LANG(C++)
AC_CONFIG_SRCDIR([app-framework/AdapterConfiguration.cc])
AC_CONFIG_HEADER([plexil-config.h])
AC_CONFIG_MACRO_DIR([m4])

#LT_INIT
AC_PROG_LIBTOOL
AC_SUBST([LIBTOOL_DEPS])
AM_INIT_AUTOMAKE

# Configuration options
AC_ARG_WITH([jni],
        AS_HELP_STRING([--with-jni], [Include utilties for Java Native Interface (default=no)]))
AC_ARG_WITH([unix-time],
        AS_HELP_STRING([--with-unix-time], [Use Unix time facilities (default=yes)]))
AC_ARG_WITH([threads],
        AS_HELP_STRING([--with-threads], [Build for multithreaded use (default=yes)]))
AC_ARG_ENABLE([debug-listener],
        AS_HELP_STRING([--enable-debug-listener], [Build PlanDebugListener interface (default=yes)]))
AC_ARG_ENABLE([debug-logging],
        AS_HELP_STRING([--enable-debug-logging], [Allow debug output (default=yes)]))
AC_ARG_ENABLE([gantt],
        AS_HELP_STRING([--enable-gantt], [Build GanttListener interface (default=no)]))
AC_ARG_ENABLE([ipc],
        AS_HELP_STRING([--enable-ipc], [Build IPC and IpcAdapter library (default=no)]))
AC_ARG_ENABLE([module-tests],
        AS_HELP_STRING([--enable-module-tests], [Build module tests (default=no)]))
AC_ARG_ENABLE([viewer],
        AS_HELP_STRING([--enable-viewer], [Build Plexil Viewer interface (default=yes)]))
AC_ARG_ENABLE([sas],
        AS_HELP_STRING([--enable-sas], [Build StandAloneSimulator - requires IPC (default=no)]))
AC_ARG_ENABLE([test-exec],
        AS_HELP_STRING([--enable-test-exec], [Build TestExec executable (default=no)]))
AC_ARG_ENABLE([udp],
        AS_HELP_STRING([--enable-udp], [Build UdpAdapter interface (default=no)]))
AC_ARG_ENABLE([universal-exec],
        AS_HELP_STRING([--enable-universal-exec], [Build universalExec executable (default=yes)]))

# Conditionals for makefiles
# These default to enabled (with)
AM_CONDITIONAL([DEBUG_LOGGING_OPT], [test "x$enable_debug_logging" != "xno"])
AM_CONDITIONAL([DEBUG_LISTENER_OPT], [test "x$enable_debug_listener" != "xno"])
AM_CONDITIONAL([THREADS_OPT], [test "x$with_threads" != "xno"])
AM_CONDITIONAL([UNIX_TIME_OPT], [test "x$with_unix_time" != "xno"])
AM_CONDITIONAL([VIEWER_OPT], [test "x$enable_viewer" != "xno"])
AM_CONDITIONAL([UNIVERSAL_EXEC_OPT], [test "x$enable_universal_exec" != "xno"])

# These default to disabled (without)
AM_CONDITIONAL([GANTT_OPT], [test "x$enable_gantt" = "xyes"])
AM_CONDITIONAL([IPC_OPT], [test "x$enable_ipc" = "xyes"])
AM_CONDITIONAL([JNI_OPT], [test "x$with_jni" = "xyes"])
AM_CONDITIONAL([MODULE_TESTS_OPT], [test "x$enable_module_tests" = "xyes"])
AM_CONDITIONAL([SAS_OPT], [test "x$enable_sas" = "xyes"])
AM_CONDITIONAL([TEST_EXEC_OPT], [test "x$enable_test_exec" = "xyes"])
AM_CONDITIONAL([UDP_OPT], [test "x$enable_udp" = "xyes"])

# Helpers for headers
AS_IF([test "x$enable_debug_logging" = "xno"],[
  AC_DEFINE([NO_DEBUG_MESSAGE_SUPPORT],[1],[Define to 1 if debug logging is disabled.])
])
AS_IF([test "x$with_threads" != "xno"],[
  AC_DEFINE([PLEXIL_WITH_THREADS],[1],[Define to 1 if multithreading is enabled.])
])
AS_IF([test "x$with_unix_time" != "xno"],[
  AC_DEFINE([PLEXIL_WITH_UNIX_TIME],[1],[Define to 1 if Unix native time facilities are enabled.])
])

AC_DEFINE_UNQUOTED([HAVE_DEBUG_LISTENER],
  [`if test "x$enable_debug_listener" != "xno" 
    then 
      echo 1
    else
      echo 0
    fi`],
  [Define to 1 if PlanDebugListener is enabled in the build.])
AC_DEFINE_UNQUOTED([HAVE_GANTT_LISTENER], 
  [`if test "x$enable_gantt" = "xyes" 
    then 
      echo 1
    else
      echo 0
    fi`],
  [Define to 1 if GanttListener is enabled in the build.])
AC_DEFINE_UNQUOTED([HAVE_LUV_LISTENER], 
  [`if test "x$enable_viewer" != "xno" 
    then 
      echo 1
    else
      echo 0
    fi`],
  [Define to 1 if LuvListener is enabled in the build.])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_LIBTOOL
# Required by IPC
AS_IF([test "x$enable_ipc" = "xyes"], [
AC_PROG_LEX
AC_PROG_YACC
])
#AC_PROG_INSTALL
AC_PROG_MAKE_SET

# Checks for libraries.

AC_CHECK_LIB([dl], [dlopen])
AC_CHECK_LIB([pthread], [pthread_create])
AC_CHECK_LIB([rt], [timer_create])
AC_CHECK_LIB([m], [sqrt])
AC_CHECK_LIB([execinfo], [backtrace])

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([arpa/inet.h dlfcn.h execinfo.h float.h limits.h netdb.h netinet/in.h stddef.h stdint.h sys/socket.h sys/time.h unistd.h wchar.h])

AM_CONDITIONAL([PLEXIL_DLFCN_H], [test "x$ac_cv_header_dlfcn_h" = "xyes"])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
# These types have special macros in newer autoconf versions,
# which aren't present in older autoconf versions
AC_CHECK_TYPES([int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t])
AC_CHECK_TYPES([pid_t, size_t, ssize_t])
AC_HEADER_TIME
AC_STRUCT_TM
AC_C_VOLATILE
AC_CHECK_TYPES([ptrdiff_t, suseconds_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_PROG_GCC_TRADITIONAL
AC_FUNC_MKTIME
AC_FUNC_SELECT_ARGTYPES
AC_TYPE_SIGNAL
AC_FUNC_STRFTIME
AC_FUNC_STRTOD
AC_CHECK_FUNCS([clock_gettime ctime_r floor gettimeofday localtime_r memmove memset modf pow setitimer socket sqrt])
AC_CHECK_FUNCS([strchr strcspn strspn strstr strtol strtoll strtoul strtoull])
# Used by UdpAdapter (and IPC)
AC_CHECK_FUNCS([gethostbyname inet_ntoa])

# Only needed by threads (and IPC)
AS_IF([test "x$with_threads" != "xno"],[
AC_CHECK_FUNCS([pthread_mutexattr_settype])
])

# FIXME: define PLEXIL_POSIX_TIME_FUNCS macro if unistd.h contains a suitable value of _POSIX_TIMERS

# Used only in GanttListener
AS_IF([test "x$enable_gantt" = "xyes"], [
AC_CHECK_FUNCS([getcwd])
])

# Used only in IPC
AS_IF([test "x$enable_ipc" = "xyes"], [
AC_CHECK_LIB([nsl], [inet_ntoa])
AC_CHECK_LIB([wrap], [hosts_ctl])
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([fcntl.h malloc.h memory.h strings.h sys/file.h sys/ioctl.h sys/param.h sys/timeb.h termios.h])
AC_FUNC_FORK
AC_FUNC_SETPGRP
AC_FUNC_SETVBUF_REVERSED
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([bzero dup2 gethostbyaddr gethostname mkdir select strdup strpbrk])
])

AC_CONFIG_FILES([Makefile
                 utils/Makefile
                 value/Makefile
                 expr/Makefile
                 intfc/Makefile
                 exec/Makefile
                 third-party/pugixml/src/Makefile
                 xml-parser/Makefile
                 app-framework/Makefile
                 apps/TestExec/Makefile
                 interfaces/GanttListener/Makefile
		 interfaces/Launcher/Makefile
                 interfaces/LuvListener/Makefile
                 interfaces/PlanDebugListener/Makefile
                 interfaces/Sockets/Makefile
                 interfaces/UdpAdapter/Makefile
                 universal-exec/Makefile])

AS_IF([test "x$enable_ipc" = "xyes"], [
AC_CONFIG_FILES([third-party/ipc/Makefile
                 interfaces/IpcAdapter/Makefile
                 interfaces/IpcUtils/Makefile])
])

AS_IF([test "x$enable_sas" = "xyes"], [
AC_CONFIG_FILES([apps/StandAloneSimulator/Makefile])
])

AC_OUTPUT
