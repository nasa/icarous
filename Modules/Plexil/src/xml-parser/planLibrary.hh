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

#ifndef PLEXIL_PLAN_LIBRARY_HH
#define PLEXIL_PLAN_LIBRARY_HH

#include <string>
#include <vector>

#include "ParserException.hh"

#include "pugixml.hpp"

namespace PLEXIL
{

  std::vector<std::string> const & getLibraryPaths();
  extern void appendLibraryPath(std::string const &dirname);
  extern void prependLibraryPath(std::string const &dirname);
  extern void setLibraryPaths(std::vector<std::string> const &paths);

  extern pugi::xml_node getLibraryNode(char const *name,
                                       bool loadIfNotFound = true);
  extern void addLibraryNode(char const *name, pugi::xml_document *doc)
    throw (ParserException);
  extern pugi::xml_document *loadLibraryFile(std::string const &filename);
  extern pugi::xml_node loadLibraryNode(char const *nodeName);

} // namespace PLEXIL

#endif // PLEXIL_PLAN_LIBRARY_HH
