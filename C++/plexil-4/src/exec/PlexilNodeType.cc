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

#include "PlexilNodeType.hh"
#include "Error.hh"

namespace PLEXIL
{
  std::string const ASSIGNMENT = "Assignment";
  std::string const COMMAND = "Command";
  std::string const EMPTY = "Empty";
  std::string const LIBRARYNODECALL = "LibraryNodeCall";
  std::string const LIST = "NodeList";
  std::string const UPDATE = "Update";

  // Must be kept in same order as PlexilNodeType enum
  static std::string const &NODE_TYPE_NAME(size_t t)
  {
    static std::string sl_names[] =
      {"", // 0 == invalid
       LIST,
       COMMAND,
       ASSIGNMENT,
       UPDATE,
       EMPTY,
       LIBRARYNODECALL
      };
    return sl_names[t];
  }

  // Simple linear search
  PlexilNodeType parseNodeType(char const *typeName)
  {
    for (size_t t = NodeType_NodeList; t < NodeType_error; ++t)
      if (NODE_TYPE_NAME(t) == typeName)
        return (PlexilNodeType) t;
    return NodeType_error;
  }

  const std::string& nodeTypeString(PlexilNodeType nodeType)
  {
    assertTrue_2(nodeType > NodeType_uninitialized && nodeType < NodeType_error,
                 "nodeTypeString: Illegal node type");
    return NODE_TYPE_NAME(nodeType);
  }

} // namespace PLEXIL
