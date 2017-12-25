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
   @file Error.cc
   @author Will Edgington

   @brief Numerous declarations related to error generation and handling.
*/

#ifndef PLEXIL_NO_ERROR_EXCEPTIONS
/* Contains the rest of this file */

#include "Error.hh"
#include "Logging.hh"
#include <cassert>

namespace PLEXIL
{

  std::ostream *Error::s_os = 0;
  bool Error::s_throw = false;
  bool Error::s_printErrors = true;
  bool Error::s_printWarnings = true;

  Error::Error(const Error& err)
    : std::exception(err),
      m_condition(err.m_condition), m_msg(err.m_msg), m_file(err.m_file), m_line(err.m_line) 
  {
  }

  Error::Error(const std::string& msg)
    : std::exception(),
      m_msg(msg), m_line(0) 
  {
  }

  Error::Error(const std::string& condition, const std::string& file, const int& line)
    : std::exception(),
      m_condition(condition), m_file(file), m_line(line) {
    if (s_os == 0)
      s_os = &(std::cerr);
  }

  Error::Error(const std::string& condition, const std::string& msg,
               const std::string& file, const int& line)
    : std::exception(),
      m_condition(condition), m_msg(msg), m_file(file), m_line(line) {
    if (s_os == 0)
      s_os = &(std::cerr);
  }

  Error::Error(const std::string& condition, const Error& exception,
               const std::string& file, const int& line)
    : std::exception(),
      m_condition(condition), m_msg(exception.m_msg), m_file(file), m_line(line) {
    if (s_os == 0)
      s_os = &(std::cerr);
  }

  Error::~Error()
    throw ()
  {
  }

  Error& Error::operator=(const Error& err) 
  {
    std::exception::operator=(err);
    m_condition = err.m_condition;
    m_msg = err.m_msg;
    m_file = err.m_file;
    m_line = err.m_line;
    return *this;
  }

  char const *Error::what() const
    throw ()
  {
    return m_msg.c_str();
  }

  /**
     @brief Compare two Errors.
  */
  bool Error::operator==(const Error& err) const 
  {
    return(m_condition == err.m_condition &&
           m_msg == err.m_msg &&
           m_file == err.m_file &&
           m_line == err.m_line);
  }

  /**
     @brief Return true iff (if and only if) the two Errors
     "match": are the same except for possibly the line numbers.
  */
  bool Error::matches(const Error& err) const {
    return(m_condition == err.m_condition &&
           m_msg == err.m_msg &&
           m_file == err.m_file);
  }

  void Error::handleAssert() {
    Logging::handle_message(Logging::LOG_ERROR, m_file.c_str(), m_line, m_msg.c_str());
    if (throwEnabled())
      throw *this;
    assert(false); // Need the stack to work backwards and look at state in the debugger
  }

  void Error::display() {
    if (!printingErrors())
      return;
    std::cout.flush();
    std::cerr.flush();
    getStream() << '\n' << m_file << ':' << m_line << ": Error: " << m_condition << " is false";
    if (!m_msg.empty())
      getStream() << "\n\t" << m_msg;
    getStream() << std::endl;
  }

  void Error::printWarning(const std::string& msg,
                           const std::string& file,
                           const int& line) {
    if (!displayWarnings())
      return;
    Logging::handle_message(Logging::WARNING, file.c_str(), line, msg.c_str());
  }

  void Error::print(std::ostream& os) const {
    os << "Error(\"" << m_condition << "\", \"";
    if (!m_msg.empty())
      os << m_msg << "\", \"";
    os << m_file << "\", " << m_line << ")";
  }

  //
  // Static member functions
  // 

  /**
     @brief Return true if printing warnings and false if not.
  */
  bool Error::displayWarnings()
  {
    return s_printWarnings;
  }

  /**
   * Indicate that warnings should be printed when detected.
   */
  void Error::doDisplayWarnings()
  {
    s_printWarnings = true;
  }

  /**
   * Indicate that warnings should not be printed.
   */
  void Error::doNotDisplayWarnings()
  {
    s_printWarnings = false;
  }

  /**
   * Indicate that errors should throw exceptions rather than
   * complaining and aborting.
   */
  void Error::doThrowExceptions()
  {
    s_throw = true;
  }

  /**
   * Indicate that errors should complain and abort rather than throw
   * exceptions.
   */
  void Error::doNotThrowExceptions()
  {
    s_throw = false;
  }


  /**
   * Are errors set to throw exceptions?
   * @return true if so; false if errors will complain and abort.
   */
  bool Error::throwEnabled()
  {
    return s_throw;
  }


  /**
     @brief Return whether all error information should be printed when detected.
  */
  bool Error::printingErrors() {
    return(s_printErrors);
  }

  /**
     @brief Indicate that error information should be printed at detection.
  */
  void Error::doDisplayErrors() {
    s_printErrors = true;
  }

  /**
     @brief Indicate that nothing should be printed when an error is detected.
  */
  void Error::doNotDisplayErrors() {
    s_printErrors = false;
  }

  /**
     @brief Return the output stream to which error information should be sent.
  */
  std::ostream& Error::getStream() {
    if (s_os == 0)
      s_os = &(std::cerr);
    return(*s_os);
  }

  /**
     @brief Indicate where output related to errors should be directed.
  */
  void Error::setStream(std::ostream& os) {
    s_os = &os;
  }

  std::ostream& operator<<(std::ostream& os, const Error& err) {
    err.print(os);
    return(os);
  }

#endif /* PLEXIL_NO_ERROR_EXCEPTIONS */

} // namespace PLEXIL
