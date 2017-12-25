/* Copyright (c) 2006-2016, Universities Space Research Association (USRA).
*  All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are met:
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in the
*       documentation and/or other materials provided with the distribution.
*     * Neither the name of the Universities Space Research Association nor the
*       names of its contributors may be used to endorse or promote products
*       derived from this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY USRA ``AS IS'' AND ANY EXPRESS OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED. IN NO EVENT SHALL USRA BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
* TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
* USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//  Copyright Notices

//  This software was developed for use by the U.S. Government as
//  represented by the Administrator of the National Aeronautics and
//  Space Administration. No copyright is claimed in the United States
//  under 17 U.S.C. 105.

//  This software may be used, copied, and provided to others only as
//  permitted under the terms of the contract or other agreement under
//  which it was acquired from the U.S. Government.  Neither title to nor
//  ownership of the software is hereby transferred.  This notice shall
//  remain on all copies of the software.

/**
   @file util-test-module.cc
   @author Will Edgington

   @brief A small test of classes Error and TestData and the related
   macros.
*/

#include "util-test-module.hh"
#include "ConstantMacros.hh"
#include "Debug.hh"
#include "Error.hh"
#include "lifecycle-utils.h"
#include "stricmp.h"
#include "TestData.hh"
#include "TestSupport.hh"
#include "ThreadMutex.hh"
#include "timespec-utils.hh"
#include "timeval-utils.hh"

#include <cassert>
#include <cfloat>
#include <cstring> // for strcmp
#include <iomanip>
#include <iostream>
#include <fstream>
#include <list>
#include <sstream>
#include <typeinfo>

#ifdef HAVE_SYS_TIME_H 
#include <sys/time.h>
#elif defined(__VXWORKS__)
#include <time.h>
#include <sys/times.h>
#include <sysLib.h> /* for sysClkRateGet() */
#endif

#if S950
// apparently needed for sys950lib
#include <types/vxTypesOld.h>
#include <sys950Lib.h>
#endif

// Tests not in this source file

extern bool LinkedQueueTest();
extern bool SimpleMapTest();
extern bool bitsetUtilsTest();

/**
 * @def assertFalse
 * @brief Test a condition and create an error if true.
 * @param cond Expression that yields a true/false result.
 */
#define assertFalse(cond) { \
  if (cond) { \
    Error(#cond, __FILE__, __LINE__).handleAssert(); \
  } \
}

/**
 * @def handle_error
 * Create an error instance for throwing, printing, etc., per class Error.
 * @param cond Condition that failed (was false), implying an error has occurred.
 * @param msg String describing the error.
 */
#define handle_error(cond, msg) {         \
  Error(#cond, #msg, __FILE__, __LINE__);        \
}

using namespace PLEXIL;

class TestError {
public:
  DECLARE_STATIC_CLASS_CONST(char*, TEST_CONST, "TestData");
  DECLARE_ERROR(BadThing);
};

class ErrorTest {
public:
  static bool test() {
    runTest(testExceptions);
    return true;
  }
private:
  static bool testExceptions() {
    assertTrue_1(strcmp(TestError::TEST_CONST(), "TestData") == 0);
    bool success = true;
    Error::doThrowExceptions();
    int var = 1;
    assertTrue_1(var == 1);
    assertTrue_1(Error::printingErrors());
    assertTrue_1(Error::displayWarnings());
    assertTrue_1(Error::throwEnabled());
    assertTrue_1(var == 1);
    assertTrue_1(Error::printingErrors());
    assertTrue_1(Error::displayWarnings());
    assertTrue_1(Error::throwEnabled());
    try {
      // These are tests of check_error() and should therefore not be changed
      //   to assertTrue() despite the usual rule for test programs.
      // --wedgingt@email.arc.nasa.gov 2005 Feb 9
      check_error_2(Error::printingErrors(), "not printing errors by default!");
      check_error_2(Error::displayWarnings(), "display warnings off by default!");
      check_error_1(var == 1);
      check_error_2(var == 1, "check_error_2(var == 1)");
      check_error_2(var == 1, Error("check_error_2(var == 1)"));
      checkError(var == 1, "Can add " << 1.09 << " and " << 2.81 << " to get " << 1.09 +2.81);
      std::cout << std::endl;
      Error::setStream(std::cout);
      warn("Warning messages working");
      Error::setStream(std::cerr);
    } 
    catch (Error &e) {
      __x__(e);
      success = false;
    }
    // check_error will not throw the errors for PLEXIL_FAST
#if !defined(PLEXIL_FAST) && !defined(__CYGWIN__)
    assertTrue_1(Error::throwEnabled());
    /* Do not print errors that we are provoking on purpose to ensure they are noticed. */
    try {
      Error::doNotDisplayErrors();
      check_error_1(var == 2);
      __y__("check_error_1(var == 2) did not throw an exception");
      success = false;
    } 
    catch (Error &e) {
      Error::doDisplayErrors();
      __z__(e, Error("var == 2", __FILE__, __LINE__ - 5), success);
    }
    try {
      Error::doNotDisplayErrors();
      check_error_2(var == 2, "check_error_2(var == 2)");
      __y__("check_error_2(var == 2, blah) did not throw an exception");
      success = false;
    } 
    catch (Error &e) {
      Error::doDisplayErrors();
      __z__(e, Error("var == 2", "check_error_2(var == 2)", __FILE__, __LINE__ - 5), success);
    }
    try {
      Error::doNotDisplayErrors();
      check_error_2(var == 2, Error("check_error_1(var == 2)"));
      __y__("check_error_2(var == 2, Error(blah)) did not throw an exception");
      success = false;
    } 
    catch (Error &e) {
      Error::doDisplayErrors();
      __z__(e, Error("var == 2", "check_error_1(var == 2)", __FILE__, __LINE__ - 5), success);
    }
#endif
    return(success);
  }
};

class DebugTest {
public:
  static bool test() {
    runTest(testDebugError);
    runTest(testDebugFiles);
    return true;
  }
private:

  static bool testDebugError() {
    bool success = true;
    // check_error will not throw the errors for PLEXIL_FAST
#if !defined(PLEXIL_FAST) && !defined(NO_DEBUG_MESSAGE_SUPPORT)
    Error::doThrowExceptions();
    assertTrue_1(Error::throwEnabled());
    //!!Add a test of DebugMessage that should throw an error here.
    //!!  Skipped for lack of time presently. --wedgingt@email.arc.nasa.gov
    Error::doNotThrowExceptions();
    assertTrue_1(!Error::throwEnabled());
#endif
    return(success);
  }

  static bool testDebugFiles() {
    for (int i = 1; i < 7; i++)
      runDebugTest(i);
    return(true);
  }

  static void runDebugTest(int cfgNum) {
#if !defined(PLEXIL_FAST) && !defined(NO_DEBUG_MESSAGE_SUPPORT)
    std::stringstream cfgName;
    cfgName << "debug" << cfgNum << ".cfg";
    std::string cfgFile(cfgName.str());
    cfgName << ".output";
    std::string cfgOut(cfgName.str());

    Error::doNotThrowExceptions();
    Error::doNotDisplayErrors();
    std::ofstream debugOutput(cfgOut.c_str());
    assertTrue_2(debugOutput.good(), "could not open debug output file");
    setDebugOutputStream(debugOutput);
    std::ifstream debugStream(cfgFile.c_str());
    assertTrue_2(debugStream.good(), "could not open debug config file");
    if (!PLEXIL::readDebugConfigStream(debugStream))
      handle_error(!PLEXIL::readDebugConfigStream(debugStream),
                   "problems reading debug config file");
    debugMsg("main1", "done opening files");
    condDebugMsg(std::cout.good(), "main1a", "std::cout is good");
    debugStmt("main2a", int s = 0; for (int i = 0; i < 5; i++) { s += i; } debugOutput << "Sum is " << s << '\n'; );
    debugMsg("main2", "primary testing done");
    Error::doThrowExceptions();
    Error::doDisplayErrors();
    setDebugOutputStream(std::cerr);
#endif
  }
};

#ifdef PLEXIL_WITH_THREADS
class MutexTest
{
public:
  static bool test()
  {
    runTest(testGuard);
    return true;
  }

  static bool testGuard()
  {
    bool result = true;
    ThreadMutex m;
    try {
      ThreadMutexGuard mg(m);
      Error::doThrowExceptions();
      assertTrue_2(0 == 1, "This assertion is supposed to fail");
      std::cout << "ERROR: Failed to throw exception" << std::endl;
      result = false;
    }
    catch (Error& e) {
      std::cout << "Caught expected exception" << std::endl;
      if (m.trylock())
    result = result && true;
      else {
    std::cout << "Throwing failed to run guard destructor" << std::endl;
    result = false;
      }
      m.unlock();
    }
    return result;
  }

};
#endif /* PLEXIL_WITH_THREADS */

class TimespecTests
{
public:
  static bool test()
  {
    runTest(testTimespecComparisons);
    runTest(testTimespecArithmetic);
    runTest(testTimespecConversions);
    return true;
  }

private:
  static bool testTimespecComparisons()
  {
    struct timespec a = {1, 0};
    struct timespec a1 = {1, 0};
    struct timespec b = {2, 0};
    struct timespec c = {1, 1};

    assertTrue_2(a < b, "Timespec operator< failed");
    assertTrue_2(a < c, "Timespec operator< failed");
    assertTrue_2(c < b, "Timespec operator< failed");
    assertTrue_2(!(b < a), "Timespec operator< failed");
    assertTrue_2(!(c < a), "Timespec operator< failed");
    assertTrue_2(!(b < c), "Timespec operator< failed");
    assertTrue_2(!(a1 < a), "Timespec operator< failed");
    assertTrue_2(!(a < a1), "Timespec operator< failed");

    assertTrue_2(b > a, "Timespec operator> failed");
    assertTrue_2(b > c, "Timespec operator> failed");
    assertTrue_2(c > a, "Timespec operator> failed");
    assertTrue_2(!(a > b), "Timespec operator> failed");
    assertTrue_2(!(a > c), "Timespec operator> failed");
    assertTrue_2(!(c > b), "Timespec operator> failed");
    assertTrue_2(!(a1 > a), "Timespec operator> failed");
    assertTrue_2(!(a > a1), "Timespec operator> failed");

    assertTrue_2(a == a, "Timespec operator== failed - identity");
    assertTrue_2(a == a1, "Timespec operator== failed - equality");
    assertTrue_2(!(a == b), "Timespec operator== failed - tv_sec");
    assertTrue_2(!(a == c), "Timespec operator== failed - tv_nsec");

    return true;
  }

  static bool testTimespecArithmetic()
  {
    struct timespec tsminus1 = {-1, 0};
    struct timespec ts0 = {0, 0};
    struct timespec ts1 = {1, 0};
    struct timespec ts1pt1 = {1, 1};
    struct timespec ts0pt9 = {0, 999999999};
    struct timespec ts2 = {2, 0};

    assertTrue_2(ts0 == ts0 + ts0, "Timespec operator+ failed - 0 + 0");
    assertTrue_2(ts1 == ts0 + ts1, "Timespec operator+ failed - 0 + 1");
    assertTrue_2(ts0 == ts1 + tsminus1, "Timespec operator+ failed - 1 + -1");
    assertTrue_2(ts0 == tsminus1 + ts1, "Timespec operator+ failed - -1 + 1");
    assertTrue_2(ts1pt1 == ts0 + ts1pt1, "Timespec operator+ failed - 0 + 1.000000001");
    assertTrue_2(ts1 == ts1 + ts0, "Timespec operator+ failed - 1 + 0");
    assertTrue_2(ts2 == ts1 + ts1, "Timespec operator+ failed - 1 + 1");
    assertTrue_2(ts2 == ts1pt1 + ts0pt9, "Timespec operator+ failed - 1.00000001 + 0.999999999");

    assertTrue_2(ts0 == ts0 - ts0, "Timespec operator- failed - 0 - 0");
    assertTrue_2(ts0 == ts1 - ts1, "Timespec operator- failed - 1 - 1");
    assertTrue_2(ts0 == tsminus1 - tsminus1, "Timespec operator- failed - -1 - -1");
    assertTrue_2(ts1 == ts1 - ts0, "Timespec operator- failed - 1 - 0");
    assertTrue_2(tsminus1 == ts0 - ts1, "Timespec operator- failed - 0 - 1");
    assertTrue_2(ts1 == ts0 - tsminus1, "Timespec operator- failed - 0 - -1");
    assertTrue_2(ts1pt1 == ts2 - ts0pt9, "Timespec operator- failed - 2 - 0.999999999");
    assertTrue_2(ts0pt9 == ts2 - ts1pt1, "Timespec operator- failed - 2 - 1.000000001");

    return true;
  }

  static bool testTimespecConversions()
  {
    return true;
  }

};

class TimevalTests
{
public:
  static bool test()
  {
    runTest(testTimevalComparisons);
    runTest(testTimevalArithmetic);
    runTest(testTimevalConversions);
    return true;
  }

private:
  static bool testTimevalComparisons()
  {
    struct timeval a = {1, 0};
    struct timeval a1 = {1, 0};
    struct timeval b = {2, 0};
    struct timeval c = {1, 1};

    assertTrue_2(a < b, "Timeval operator< failed");
    assertTrue_2(a < c, "Timeval operator< failed");
    assertTrue_2(c < b, "Timeval operator< failed");
    assertTrue_2(!(b < a), "Timeval operator< failed");
    assertTrue_2(!(c < a), "Timeval operator< failed");
    assertTrue_2(!(b < c), "Timeval operator< failed");
    assertTrue_2(!(a1 < a), "Timeval operator< failed");
    assertTrue_2(!(a < a1), "Timeval operator< failed");

    assertTrue_2(b > a, "Timeval operator> failed");
    assertTrue_2(b > c, "Timeval operator> failed");
    assertTrue_2(c > a, "Timeval operator> failed");
    assertTrue_2(!(a > b), "Timeval operator> failed");
    assertTrue_2(!(a > c), "Timeval operator> failed");
    assertTrue_2(!(c > b), "Timeval operator> failed");
    assertTrue_2(!(a1 > a), "Timeval operator> failed");
    assertTrue_2(!(a > a1), "Timeval operator> failed");

    assertTrue_2(a == a, "Timeval operator== failed - identity");
    assertTrue_2(a == a1, "Timeval operator== failed - equality");
    assertTrue_2(!(a == b), "Timeval operator== failed - tv_sec");
    assertTrue_2(!(a == c), "Timeval operator== failed - tv_nsec");

    return true;
  }

  static bool testTimevalArithmetic()
  {
    struct timeval tsminus1 = {-1, 0};
    struct timeval ts0 = {0, 0};
    struct timeval ts1 = {1, 0};
    struct timeval ts1pt1 = {1, 1};
    struct timeval ts0pt9 = {0, 999999};
    struct timeval ts2 = {2, 0};

    assertTrue_2(ts0 == ts0 + ts0, "Timeval operator+ failed - 0 + 0");
    assertTrue_2(ts1 == ts0 + ts1, "Timeval operator+ failed - 0 + 1");
    assertTrue_2(ts0 == ts1 + tsminus1, "Timeval operator+ failed - 1 + -1");
    assertTrue_2(ts0 == tsminus1 + ts1, "Timeval operator+ failed - -1 + 1");
    assertTrue_2(ts1pt1 == ts0 + ts1pt1, "Timeval operator+ failed - 0 + 1.000001");
    assertTrue_2(ts1 == ts1 + ts0, "Timeval operator+ failed - 1 + 0");
    assertTrue_2(ts2 == ts1 + ts1, "Timeval operator+ failed - 1 + 1");
    assertTrue_2(ts2 == ts1pt1 + ts0pt9, "Timeval operator+ failed - 1.000001 + 0.999999");

    assertTrue_2(ts0 == ts0 - ts0, "Timeval operator- failed - 0 - 0");
    assertTrue_2(ts0 == ts1 - ts1, "Timeval operator- failed - 1 - 1");
    assertTrue_2(ts0 == tsminus1 - tsminus1, "Timeval operator- failed - -1 - -1");
    assertTrue_2(ts1 == ts1 - ts0, "Timeval operator- failed - 1 - 0");
    assertTrue_2(tsminus1 == ts0 - ts1, "Timeval operator- failed - 0 - 1");
    assertTrue_2(ts1 == ts0 - tsminus1, "Timeval operator- failed - 0 - -1");
    assertTrue_2(ts1pt1 == ts2 - ts0pt9, "Timeval operator- failed - 2 - 0.999999");
    assertTrue_2(ts0pt9 == ts2 - ts1pt1, "Timeval operator- failed - 2 - 1.000001");

    return true;
  }

  static bool testTimevalConversions()
  {
    return true;
  }

};

class StricmpTests
{
public:
  static bool test()
  {
    runTest(testNullArgs);
    runTest(testSameCase);
    runTest(testDifferentCases);
    return true;
  }

private:
  static bool testNullArgs()
  {
    assertTrue_1(0 == stricmp(NULL, NULL));
    assertTrue_1(0 == stricmp(NULL, ""));
    assertTrue_1(-1 == stricmp(NULL, " "));
    assertTrue_1(0 == stricmp("", NULL));
    assertTrue_1(1 == stricmp(" ", NULL));
    assertTrue_1(0 == stricmp("", ""));
    return true;
  }

  static bool testSameCase()
  {
    assertTrue_1(0 == stricmp(" ", " "));
    assertTrue_1(1 == stricmp("  ", " "));
    assertTrue_1(-1 == stricmp(" ", "  "));

    assertTrue_1(0 == stricmp("123", "123"));
    assertTrue_1(1 == stricmp("234", "123"));
    assertTrue_1(1 == stricmp("1234", "123"));
    assertTrue_1(-1 == stricmp("123", "1234"));
    assertTrue_1(-1 == stricmp("123", "234"));

    assertTrue_1(0 == stricmp("foo", "foo"));
    assertTrue_1(0 == stricmp("FOO", "FOO"));
    assertTrue_1(-1 == stricmp("foo", "fool"));
    assertTrue_1(1 == stricmp("fool", "foo"));
    assertTrue_1(1 == stricmp("fou", "foo"));
    assertTrue_1(-1 == stricmp("foo", "fou"));

    return true;
  }

  static bool testDifferentCases()
  {
    assertTrue_1(0 == stricmp("foo", "FOO"));
    assertTrue_1(0 == stricmp("FOO", "foo"));

    assertTrue_1(-1 == stricmp("FOO", "fool"));
    assertTrue_1(-1 == stricmp("foo", "FOOL"));

    assertTrue_1(1 == stricmp("fool", "FOO"));
    assertTrue_1(1 == stricmp("FOOL", "foo"));

    assertTrue_1(1 == stricmp("fou", "FOO"));
    assertTrue_1(1 == stricmp("FOU", "foo"));

    assertTrue_1(-1 == stricmp("foo", "FOU"));
    assertTrue_1(-1 == stricmp("FOO", "fou"));

    return true;
  }
};

void UtilModuleTests::runTests(std::string /* path */) 
{
  runTestSuite(ErrorTest::test);
  runTestSuite(DebugTest::test);
  runTestSuite(TimespecTests::test);
  runTestSuite(TimevalTests::test);
#ifdef PLEXIL_WITH_THREADS
  runTestSuite(MutexTest::test);
#endif
  runTestSuite(StricmpTests::test);

  runTestSuite(SimpleMapTest);
  runTestSuite(LinkedQueueTest);
  runTestSuite(bitsetUtilsTest);

  // Do cleanup
  plexilRunFinalizers();

  std::cout << "Finished" << std::endl;
}
