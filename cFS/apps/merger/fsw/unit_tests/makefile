##############################################################################
## File:
##   $Id: makefile 1.1 2016/06/14 16:47:23EDT czogby Exp  $
##
## Purpose:
##   Makefile for building the UT-Assert example unit tests
##
##   $Log: makefile  $
##   Revision 1.1 2016/06/14 16:47:23EDT czogby 
##   Initial revision
##   Member added to project /FSW-TOOLs-REPOSITORY/ut-assert/example/fsw/unit_test/project.pj
##
##############################################################################
## GNU Makefile for building UT unit tests

#
# Supported MAKEFILE targets:
#   clean - deletes object files, executables, output files, and gcov files
#   all   - makes example_testrunner.exe
#   run   - runs example_testrunner.exe
#   gcov  - prints a GCOV coverage report (make all, make run, make gcov)
#
# GCOV is disabled by default.  If you are using the source level debugger you will want to 
# disable GCOV.  To enable GCOV you can override the ENABLE_GCOV variable on the command line 
# by setting it to TRUE.  For example "make ENABLE_GCOV=TRUE".
#

#
# VPATH specifies the search paths for source files outside of the current directory.  Note that
# all object files will be created in the current directory even if the source file is not in the 
# current directory.
#

APP_PATH=../../../
CFE_PATH=../../../../cFE/cfe/fsw/cfe-core
OSAL_PATH=../../../../cFE/osal
PSP_PATH=../../../../cFE/psp
UTASSERT_PATH=../../../../cFE/tools/ut_assert
MODULES_PATH=../../../../../Modules

VPATH := .
VPATH += ../src
VPATH += $(UTASSERT_PATH)/src

#
# INCLUDES specifies the search paths for include files outside of the current directory.  
# Note that the -I is required. 
#
INCLUDES := -I.
INCLUDES += -I$(UTASSERT_PATH)/inc
INCLUDES += -I$(CFE_PATH)/os/inc
INCLUDES += -I$(CFE_PATH)/src/inc
INCLUDES += -I$(CFE_PATH)/src/time
INCLUDES += -I$(CFE_PATH)/src/sb
INCLUDES += -I$(CFE_PATH)/src/es
INCLUDES += -I$(CFE_PATH)/src/evs
INCLUDES += -I$(CFE_PATH)/src/fs
INCLUDES += -I$(CFE_PATH)/src/tbl
INCLUDES += -I$(CFE_PATH)/../mission_inc
INCLUDES += -I$(CFE_PATH)/../platform_inc/cpu1
INCLUDES += -I$(OSAL_PATH)/src/os/inc
INCLUDES += -I$(OSAL_PATH)/src/bsp/pc-linux/config
INCLUDES += -I$(OSAL_PATH)/build/inc
INCLUDES += -I$(PSP_PATH)/fsw/inc
INCLUDES += -I$(PSP_PATH)/fsw/pc-linux/inc
INCLUDES += -I$(PSP_PATH)/fsw/pc-linux/inc
INCLUDES += -I$(APP_PATH)/inc
INCLUDES += -I../src
INCLUDES += -I$(MODULES_PATH)/Utils

#
# FLIGHT_OBJS specifies flight software object files.
#
FLIGHT_OBJS := IntersectionScheduler.o

#
# UT_OBJS specifies unit test object files.
#
UT_OBJS := ut_osapi_stubs.o
UT_OBJS += ut_osfileapi_stubs.o
UT_OBJS += ut_cfe_sb_stubs.o
UT_OBJS += ut_cfe_es_stubs.o
UT_OBJS += ut_cfe_evs_stubs.o
UT_OBJS += ut_cfe_time_stubs.o
UT_OBJS += ut_cfe_fs_stubs.o
UT_OBJS += ut_cfe_psp_memutils_stubs.o
UT_OBJS += ut_cfe_psp_timer_stubs.o
UT_OBJS += ut_cfe_tbl_stubs.o
UT_OBJS += ut_cfe_sb_hooks.o
UT_OBJS += ut_cfe_es_hooks.o
UT_OBJS += ut_cfe_evs_hooks.o
UT_OBJS += ut_cfe_tbl_hooks.o
UT_OBJS += ut_cfe_time_hooks.o
UT_OBJS += utassert.o
UT_OBJS += utlist.o
UT_OBJS += uttest.o
UT_OBJS += uttools.o
UT_OBJS += IntersectionScheduler.o
UT_OBJS += merger_test.o



#
# UT_TEST_RUNNER specifies the name of the test runner.
#
UT_TEST_RUNNER = TestRunner.exe

###############################################################################

COMPILER=gcc
LINKER=g++

#
# Compiler and Linker Options
#
#GCOV_COPT = -fprofile-arcs -ftest-coverage -pg -p
#GCOV_LOPT = -pg -p -fprofile-arcs -ftest-coverage -lgcov

#WARNINGS = -Wall -W -ansi -Werror -Wstrict-prototypes -Wundef
WARNINGS = -Wall -Wstrict-prototypes -Wundef
DEBUGGER = -g

COPT = $(WARNINGS) $(DEBUGGER) $(GCOV_COPT) -DSOFTWARE_LITTLE_BIT_ORDER -D_EL -D__ix86_64__ -DUT_VERBOSE -m32

LOPT = $(GCOV_LOPT) -lm -lUtils -m32 -L$(MODULES_PATH)/lib

###############################################################################
## Rule to make the specified TARGET
##
%.exe: %.o
	$(LINKER) $^ -o $*.exe $(LOPT)

###############################################################################
##  "C" COMPILER RULE
##
%.o: %.c
	$(COMPILER) -c $(COPT) $(INCLUDES) $<

##############################################################################
##

all:$(UT_TEST_RUNNER)

$(UT_TEST_RUNNER): $(UT_OBJS) $(FLIGHT_OBJS)

clean ::
	rm -f *.o *.exe *.gcda *.gcno *.gcov gmon.out

run ::
	./$(UT_TEST_RUNNER)

gcov ::
	@echo
	@gcov $(FLIGHT_OBJS:.o=.gcda) | sed 'N;s/\n/ /' | \
		sed -n '/File/p' | sed '/ads/d' | sed -e '/\.h/d'  | \
		sed 's/ Lines executed:/ /; s/File/gcov:/; s/of// '
	@rm -f *.gcda *.gcno
	@echo

# end of file
