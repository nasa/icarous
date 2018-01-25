/* Copyright (c) 2006-2017, Universities Space Research Association (USRA).
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
   @file TestData.hh
   @author Will Edgington

   @brief Numerous declarations related to testing.
*/

//
// *** FIXME ***
// Rename these macros from __[whatever]
// See SEI CERT C++ Coding Standard rule DCL51-CPP.
//

#ifndef PLEXIL_TEST_DATA_HH
#define PLEXIL_TEST_DATA_HH

/* $Id: TestData.hh,v 1.1 2006/08/14 22:05:54 miatauro Exp $ */

#include "Error.hh"

#ifdef PLEXIL_NO_ERROR_EXCEPTIONS
/* Since no exceptions are thrown by this variant, we cannot test for them. */

/**
   @brief Complain about an unexpected exception.
   @note Use only in test programs.  Should not be in this include file,
   but rather in one specific to test programs.
*/
#define __x__(exception)

/**
   @brief Complain about the lack of an expected exception by printing the
   message with location information.
   @note Use only in test programs.  Should not be in this include file,
   but rather in one specific to test programs.
*/
#define __y__(msg)

/**
   @brief Complain about the lack of an expected exception.
   @note Use only in test programs.  Should not be in this include file,
   but rather in one specific to test programs.
*/
#define __y2__(exception)

/**
   @brief Compare an exception that occurred with the expected exception.
   If they match, say so and continue.
   If they don't, complain in a format that Emacs will recognize as an error
   containing location information.
   @param exception The exception thrown during the test.
   @param expectedException The exception that is thrown if the code being tested is correct.
   @param good A bool variable set to false if the expected error does not match the one actually thrown.
   @note Use only in test programs.  Should not be in this include file,
   but rather in one specific to test programs.
 */
#define __z__(exception, expectedException, good)

#else /* _NO_ERROR_EXCEPTIONS */
/* This variant does throw exceptions, so check they are the correct ones. */

#define __x__(exception) { \
  TestData::unexpectedException("__x__", (exception), __FILE__, __LINE__); \
}

#define __y__(msg) { \
  TestData::missingException("__y__", (msg), __FILE__, __LINE__); \
}

#define __y2__(exception) { \
  TestData::missingException("__y2__", (exception), __FILE__, __LINE__); \
}

/* !!This should use Error::matches() rather than operator==() after Error itself is tested. */
#define __z__(exception, expectedException, good) { \
  if ((exception).matches(expectedException)) { \
    TestData::correctException("__z__", (exception)); \
  } else { \
    TestData::wrongException("__z__", (exception), (expectedException), __FILE__, __LINE__); \
    good = false; \
  } \
}


/**
   @class TestData
   @brief Records statistics about tests, including success and failure.
   @note Use only in test programs.  Should not be in this include file,
   but rather in one specific to test programs.
*/
class TestData {
public:

  /**
     @brief Record a failed comparison test; i.e., one should be equal to two but wasn't.
  */
  static void failedCompare(const std::string& macro, const std::string& one, const std::string& two,
                            const std::string& file, const int& line);

  /**
     @brief Record a failed test in that an exception was generated where none was expected.
  */
  static void unexpectedException(const std::string& macro, const PLEXIL::Error& exception,
                                  const std::string& file, const int& line);

  /**
     @brief Record a failed test in that an exception should have been generated but wasn't.
  */
  static void missingException(const std::string& macro, const std::string& msg,
                               const std::string& file, const int& line);

  /**
     @brief Record a failed test in that an exception should have been generated but wasn't.
  */
  static void missingException(const std::string& macro, const PLEXIL::Error& exception,
                               const std::string& file, const int& line);

  /**
     @brief Record a failed test in that the wrong exception was thrown.
  */
  static void wrongException(const std::string& macro,
                             const PLEXIL::Error& caughtException,
                             const PLEXIL::Error& expectedException,
                             const std::string& file, const int& line);

  /**
     @brief Record a successful test in that an expected exception was thrown.
  */
  static void correctException(const std::string& macro,
                               const PLEXIL::Error& caughtException);
};

#endif /* PLEXIL_NO_ERROR_EXCEPTIONS */

#endif /* PLEXIL_TEST_DATA_HH */
