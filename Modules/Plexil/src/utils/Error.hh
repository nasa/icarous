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

// !! Two doxygen comments for each #define depending on another #define is
// !!   a very bad idea.  There's no way to keep them in sync and no point
// !!   in doing so if doxygen is not using the C/C++ preprocessor.
// !! --wedgingt 2004 Oct 5

/**
   @file Error.hh
   @author Will Edgington

   @brief Numerous declarations related to error generation and handling.

   @note These macros should be used rather than direct calls to throw
   unless an existing Error is being "re-thrown" from a catch block.

   @note This is presently only the "interface for programmers";
   nothing has been tested.

   @note Think of the first section, #ifdef PLEXIL_FAST, as the
   "prototypes" and documentation as well as the "production" variant.
*/

#ifndef PLEXIL_ERROR_HH
#define PLEXIL_ERROR_HH

#include <iostream>
#include <string>
#include <sstream>
#include <stdexcept>

// Helper macro for telling compiler that error handler won't return
#ifdef __GNUC__
// This version for g++ and clang++
#define PLEXIL_NORETURN __attribute__((__noreturn__))
#else
// TODO: Support other compilers here
// fallback (no-op)
#define PLEXIL_NORETURN
#endif

/**
 * @def DECLARE_ERROR
 * Declare an error as a function that returns a string naming itself.
 * @param error The error to declare.
 */
#define DECLARE_ERROR(error) \
  static const std::string& error() { \
    static const std::string sl_lblStr(#error); \
    return(sl_lblStr); \
  }

namespace PLEXIL
{

  /**
     @class Error
     @brief Used whenever a C++ exception is thrown.
  */
  class Error : public std::exception
  {
  public:

    /**
       @brief Build an Error object from the usual information: the false/failed
       condition and the source file and line where it was tested/detected.
    */

    Error(const std::string& condition, const std::string& file, const int& line);

    /**
       @brief Build an Error object from the information given, including an extra message.
    */
    Error(const std::string& condition, const std::string& msg, const std::string& file, const int& line);

    /**
       @brief Build an Error object from the information given, including another Error object's info.
    */
    Error(const std::string& condition, const Error& exception, const std::string& file, const int& line);

    /**
       @brief Build an Error object from only a message.

       @note Should only be used when setCause() will be called before
       the Error is thrown.

       @note Never prints anything, unlike the other constructors.
       @see Error::setCause()
    */
    Error(const std::string& msg);

    /**
       @brief Copy constructor.
    */
    Error(const Error& err);

    /**
     * The Error destructor.
     * @note Should only be used implicitly.
     */
    virtual ~Error() throw ();

    /**
       @brief Assignment operator.
    */
    Error& operator=(const Error& err);

    /**
       @brief Return the message as a character string.
    */
    virtual char const *what() const
      throw ();

    /**
       @brief Return whether all error information should be printed when detected.
    */
    static bool printingErrors();

    /**
       @brief Indicate that error information should be printed at detection.
    */
    static void doDisplayErrors();

    /**
       @brief Indicate that nothing should be printed when an error is detected.
    */
    static void doNotDisplayErrors();

    /**
       @brief Return the output stream to which error information should be sent.
    */
    static std::ostream& getStream();

    /**
       @brief Indicate where output related to errors should be directed.
    */
    static void setStream(std::ostream& os);

    /**
     * Display in "error format" (for Emacs, e.g.) on the stream (getStream()).
     */
    void display();

    /**
     * Print as if calling an Error constructor.
     */
    void print(std::ostream& os = std::cerr) const;

    /**
       @brief Compare two Errors.
    */
    bool operator==(const Error& err) const;

    /**
       @brief Return true iff (if and only if) the two Errors
       "match": are the same except for possibly the line numbers.
    */
    bool matches(const Error& err) const;

    /**
       @brief Print a warning in the same format used by Error::display()
       unless printing warnings has been disabled.
    */
    static void printWarning(const std::string& msg,
                             const std::string& file,
                             const int& line);

    /**
       @brief Return true if printing warnings and false if not.
    */
    static bool displayWarnings();

    /**
     * Indicate that warnings should be printed when detected.
     */
    static void doDisplayWarnings();

    /**
     * Indicate that warnings should not be printed.
     */
    static void doNotDisplayWarnings();

    /**
     * Indicate that errors should throw exceptions rather than
     * complaining and aborting.
     */
    static void doThrowExceptions();

    /**
     * Indicate that errors should complain and abort rather than throw
     * exceptions.
     */
    static void doNotThrowExceptions();

    /**
     * Are errors set to throw exceptions?
     * @return true if so; false if errors will complain and abort.
     */
    static bool throwEnabled();

    DECLARE_ERROR(GeneralMemoryError);
    DECLARE_ERROR(GeneralUnknownError);

    /**
     * Actually throw the exception or complain and abort.
     * @note Which is done depends on throwEnabled().
     * @see throwEnabled
     */
    void handleAssert() PLEXIL_NORETURN;

  protected:

    // Data members for use by derived classes
    std::string m_condition; /**<The condition that, being false, implies the error has occurred. */
    std::string m_msg; /**<Additional info about the error. */
    std::string m_file; /**<The source file in which the error was detected (__FILE__). */
    int m_line; /**<The source line on which the error detected (__LINE__). */

  private:

    static bool s_throw; /**<Set to throw exception. */
    static bool s_printErrors; /**<Set to print errors when detetected */
    static bool s_printWarnings; /**<Set to print warnings */
    static std::ostream *s_os; /**<The stream to write all error data to. */
    Error(); /**<The zero argument constructor should not be used. */
  };

  std::ostream& operator<<(std::ostream& os, const Error& err);

} // namespace PLEXIL

//
// Error macros
//

/**
 * @def ALWAYS_FAIL
 * False.
 * @note Used as argument to assert() and similar functions to
 * make clear that the assertion will fail, throw an error, etc.
 */
#define ALWAYS_FAIL (false)

//
// assertTrue & friends
//

/**
 * @def assertTrue_1
 * @brief Test a condition and create an error if false.
 * @param cond Expression that yields a true/false result.
 */
#define assertTrue_1(cond) { \
  if (!(cond)) { \
    PLEXIL::Error(#cond, __FILE__, __LINE__).handleAssert(); \
  } \
}

/**
 * @def assertTrue_2
 * @brief Test a condition and create an error if false.
 * @param cond Expression that yields a true/false result.
 * @param msg A string or Error instance.
 */
#define assertTrue_2(cond, msg) { \
  if (!(cond)) { \
    PLEXIL::Error(#cond, msg, __FILE__, __LINE__).handleAssert(); \
  } \
}

/**
 * @def assertTrueMsg
 * @brief Test a condition and create an error if false.
 * @param cond Expression that yields a true/false result.
 * @param msg Anything suitable as the right-hand side of <<.
 */
#define assertTrueMsg(cond, msg) { \
  if (!(cond)) { \
    std::ostringstream sstr; \
    sstr << msg; \
    PLEXIL::Error(#cond, sstr.str(), __FILE__, __LINE__).handleAssert(); \
  } \
}

#ifdef PLEXIL_FAST

#define check_error_1(cond)
#define check_error_2(cond, msg)
#define check_error_1(cond, msg1, msg2)

#define checkError(cond, msg)

/**
 * @def warn
 * Print a warning if such is enabled.
 * @param msg The information to print.
 * @note When PLEXIL_FAST is defined, these are ignored
 */
#define warn(msg)

#else

//
// check_error implementation
//

/**
 * @def check_error_1
 * @brief If the condition is false, throw an exception.
 * @param cond The condition to test.
 */
#define check_error_1(cond) { \
  if (!(cond)) { \
    PLEXIL::Error(#cond, __FILE__, __LINE__).handleAssert(); \
  } \
}

/**
 * @def check_error_2
 * @brief If the condition is false, throw an exception.
 * @param cond The condition to test.
 * @param msg A string or Error instance.
 */
#define check_error_2(cond, msg) { \
  if (!(cond)) { \
    PLEXIL::Error(#cond, msg, __FILE__, __LINE__).handleAssert(); \
  } \
}

#define checkError(cond, msg) { \
  if (!(cond)) { \
    std::ostringstream sstr; \
    sstr << msg; \
    PLEXIL::Error(#cond, sstr.str(), __FILE__, __LINE__).handleAssert(); \
  } \
}

#define warn(msg) {                                             \
      std::ostringstream sstr;                                   \
      sstr << msg;                                              \
      PLEXIL::Error::printWarning(sstr.str(), __FILE__, __LINE__);  \
}

#endif /* PLEXIL_FAST */

#endif /* PLEXIL_ERROR_HH */
