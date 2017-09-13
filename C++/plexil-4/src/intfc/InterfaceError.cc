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

#include "InterfaceError.hh"

#include "Logging.hh"

#include <cassert>

namespace PLEXIL
{

  bool InterfaceError::s_throw = false;

  //
  // Static member functions
  //

  void InterfaceError::doThrowExceptions()
  {
    s_throw = true;
  }

  void InterfaceError::doNotThrowExceptions()
  {
    s_throw = false;
  }

  bool InterfaceError::throwEnabled()
  {
    return s_throw;
  }

  InterfaceError::InterfaceError(const std::string& condition,
                                 const std::string& msg,
                                 const std::string& file,
                                 const int& line)
    : Error(condition, msg, file, line)
  {
  }
    
  InterfaceError::InterfaceError(const InterfaceError &orig)
    : Error(orig)
  {
  }

  InterfaceError &InterfaceError::operator=(const InterfaceError &other)
  {
    Error::operator=(other);
    return *this;
  }

  InterfaceError::~InterfaceError()
    throw ()
  {
  }

  bool InterfaceError::operator==(const InterfaceError &other)
  {
    return Error::operator==(other);
  }

  void InterfaceError::report()
  {
    Logging::handle_message(Logging::LOG_ERROR, m_file.c_str(), m_line, m_msg.c_str());
    if (throwEnabled())
      throw *this;
    else
      assert(false);
  }

}
