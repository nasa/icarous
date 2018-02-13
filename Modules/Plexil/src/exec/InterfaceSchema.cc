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

#include "InterfaceSchema.hh"
#include <cstring>

namespace PLEXIL
{

  /**
   * Extract comma separated arguments from a character string.
   * @return pointer to vector of strings
   * @note Caller is responsible for disposing of the vector.
   */

  std::vector<std::string> * 
  InterfaceSchema::parseCommaSeparatedArgs(const char * argString)
  {
    static const char * whitespace = " \t\n";
    std::vector<std::string> * result = new std::vector<std::string>();
    const char * next = argString;
    while ((next != NULL) && (strlen(next) > 0))
      {
	// skip leading whitespace
	unsigned int offset = strspn(next, whitespace);
	if (offset == strlen(next))
	  break; // at end of string
	next += offset;

	// look for comma or end of string
	const char * comma = strchr(next, ',');
	unsigned int len = 
	  (comma == NULL) ? 
	  strcspn(next, whitespace) /* strip trailing whitespace */ :
	  comma - next;

	// construct result string
	result->push_back(std::string(next, len));
	next =
	  (comma == NULL) ? NULL : comma + 1;
      }
    return result;
  }

}
