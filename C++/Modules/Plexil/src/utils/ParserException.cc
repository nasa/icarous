/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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


#include "ParserException.hh"
#include "Logging.hh"
#include <cstring>
#include <iostream>

namespace PLEXIL
{

  ParserException::ParserException()
    throw ()
    : std::exception(), 
      m_what("Unspecified parser exception"),
      m_file(),
      m_line(0),
      m_char(0)
  {
  }

  // Must copy the message as it may be stack or dynamically allocated.
  ParserException::ParserException(const char * msg)
    throw()
    : std::exception(),
      m_what(),
      m_file(),
      m_line(0),
      m_char(0)
  {
    if (msg)
      m_what = msg;
    else
      m_what = "Message not specified";
    Logging::handle_message(Logging::LOG_ERROR, m_what.c_str());
  }
  
  // Used to report (e.g.) pugixml errors.
  ParserException::ParserException(const char * msg, const char * file, int offset)
    throw()
    : std::exception(),
      m_what(),
      m_file(),
      m_line(0),
      m_char(offset)
  {
    if (msg)
      m_what = msg;
    else
      m_what = "Message not specified";
    if (file)
      m_file = file;
    Logging::handle_message(Logging::LOG_ERROR, file, offset, m_what.c_str());
  }
  
  // When we have complete information about the location.
  ParserException::ParserException(const char * msg, const char * file, int line, int col)
    throw()
    : std::exception(),
      m_what(),
      m_file(),
      m_line(line),
      m_char(col)
  {
    if (msg)
      m_what = msg;
    else
      m_what = "Message not specified";
    if (file)
      m_file = file;
    Logging::handle_message(Logging::LOG_ERROR, file, line, col, m_what.c_str());
  }
  
  ParserException& ParserException::operator=(const ParserException& other)
    throw()
  {
    this->std::exception::operator=(other);
    m_what = other.m_what;
    m_file = other.m_file;
    m_line = other.m_line;
    m_char = other.m_char;
    return *this;
  }

  ParserException::~ParserException()
  throw()
  {
  }

  const char* ParserException::what() const
    throw()
  {
    return m_what.c_str();
  }

}
