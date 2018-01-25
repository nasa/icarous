/* Copyright (c) 2006-2008, Universities Space Research Association (USRA).
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

#ifndef PARSER_EXCEPTION_H
#define PARSER_EXCEPTION_H

#include <exception>
#include <sstream>

/**
 * @def checkParserException
 * @brief If the condition is false, throw a ParserException
 * @param cond The condition to test; if false, throw the exception
 * @param msg An expression which writes the required message to a stream
 */
#define checkParserException(cond, msg) { \
  if (!(cond)) \
    { \
      std::ostringstream whatstr; \
      whatstr << msg; \
      throw PLEXIL::ParserException(whatstr.str().c_str()); \
    } \
}

namespace PLEXIL
{

  class ParserException : public std::exception
  {
  public:
    ParserException() throw();

    ParserException(const char* msg) throw ();
    ParserException(const char* msg, const char* filename, int offset) throw();
    ParserException(const char* msg, const char* filename, int line, int col) throw();

    ParserException& operator=(const ParserException&) throw();

    virtual ~ParserException() throw();

    virtual const char *what() const throw();

  private:
    std::string m_what;
    std::string m_file; /**<The source file in which the error was detected (__FILE__). */
    int m_line; /**< Line number of the error */
	int m_char; /**< The character offset of the error */
  };

}


#endif // PARSER_EXCEPTION_H
