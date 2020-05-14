##################################################################
#
# Coverage Unit Test build recipe
#
# This CMake file contains the recipe for building the zmq_iface unit tests.
# It is invoked from the parent directory.
#
##################################################################

#
#
# NOTE on the subdirectory structures here:
#
# - "inc" provides local header files shared between the coveragetest,
#    wrappers, and overrides source code units
# - "coveragetest" contains source code for the actual unit test cases
#    The primary objective is to get line/path coverage on the FSW 
#    code units.
# - "wrappers" contains wrappers for the FSW code.  The wrapper adds
#    any UT-specific scaffolding to facilitate the coverage test, and
#    includes the unmodified FSW source file.
#
 
set(UT_NAME zmq_iface)

# Use the UT assert public API, and allow direct
# inclusion of source files that are normally private
include_directories(${osal_MISSION_DIR}/ut_assert/inc)
include_directories(${PROJECT_SOURCE_DIR}/fsw/src)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/inc)

# Generate a dedicated "testrunner" executable that executes the tests for each FSW code unit
# Although sample_app has only one source file, this is done in a loop such that 
# the general pattern should work for several files as well.
foreach(SRCFILE utils.c)
    get_filename_component(UNITNAME "${SRCFILE}" NAME_WE)
    
    set(TESTNAME "${UT_NAME}-${UNITNAME}")
    set(UNIT_SOURCE_FILE        "${ZMQ_IFACE_SOURCE_DIR}/fsw/src/${UNITNAME}.c")
    set(TESTCASE_SOURCE_FILE    "coveragetest/coveragetest_${UNITNAME}.c")
    
    # Compile the source unit under test as a OBJECT
    add_library(ut_${TESTNAME}_object OBJECT
        ${UNIT_SOURCE_FILE}
    )    
    
    # Apply the UT_COVERAGE_COMPILE_FLAGS to the units under test
    # This should enable coverage analysis on platforms that support this
    target_compile_options(ut_${TESTNAME}_object PRIVATE ${UT_COVERAGE_COMPILE_FLAGS})
        
    # Compile a test runner application, which contains the
    # actual coverage test code (test cases) and the unit under test
    add_executable(${TESTNAME}-testrunner
        ${TESTCASE_SOURCE_FILE}
        $<TARGET_OBJECTS:ut_${TESTNAME}_object>
    )
    
    # This also needs to be linked with UT_COVERAGE_LINK_FLAGS (for coverage)
    # This is also linked with any other stub libraries needed,
    # as well as the UT assert framework    
    target_link_libraries(${TESTNAME}-testrunner
        ${UT_COVERAGE_LINK_FLAGS}
        ut_cfe-core_stubs
        ut_assert
		json-c
    )
    
    # Add it to the set of tests to run as part of "make test"
    add_test(${TESTNAME} ${TESTNAME}-testrunner)
    
endforeach()


