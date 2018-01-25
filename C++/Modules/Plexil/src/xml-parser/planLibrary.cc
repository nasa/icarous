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

#include "planLibrary.hh"

#include "Error.hh"
#include "lifecycle-utils.h"
#include "map-utils.hh"
#include "parsePlan.hh"
#include "parser-utils.hh"
#include "PlexilSchema.hh"
#include "SimpleMap.hh"

using pugi::xml_document;
using pugi::xml_node;
using std::string;
using std::vector;

namespace PLEXIL
{
  //
  // Static variables local to this file
  //

  // List of library directories to search
  static vector<string> librarySearchPaths;

  // Place to store library nodes
  typedef SimpleMap<string, xml_document *> LibraryMap;
  static LibraryMap libraryMap;

  vector<string> const &getLibraryPaths()
  {
    return librarySearchPaths;
  }

  void appendLibraryPath(string const &dirname)
  {
    librarySearchPaths.push_back(dirname);
  }

  void prependLibraryPath(string const &dirname)
  {
    librarySearchPaths.insert(librarySearchPaths.begin(), dirname);
  }

  void setLibraryPaths(std::vector<std::string> const &paths)
  {
    librarySearchPaths = paths;
  }

  static void cleanLibraryMap()
  {
    for (LibraryMap::iterator it = libraryMap.begin(); it != libraryMap.end(); ++it) {
      xml_document *temp = it->second;
      it->second = NULL;
      delete temp;
    }
  }

  void addLibraryNode(char const *name, xml_document *doc)
    throw (ParserException)
  {
    static bool sl_inited = false;
    if (!sl_inited) {
      plexilAddFinalizer(&cleanLibraryMap);
      sl_inited = true;
    }
    assertTrue_2(doc, "addLibraryNode: Null document");
    assertTrue_2(*name, "addLibraryNode: Empty name");

    // *** TODO: Check library is well formed ***
    // *** TODO: handle global decls ***

    // Check whether we already have a library by the same name
    LibraryMap::iterator it = libraryMap.find<char const *, CStringComparator>(name);
    if (it != libraryMap.end()) {
      if (it->second == doc)
        return; // same document, no need to do anything else

      // If there is an existing entry, delete its document.
      delete it->second;
    }

    // Insert it
    libraryMap[name] = doc;
  }

  xml_document *loadLibraryFile(string const &filename)
  {
    // Check current working directory first
    xml_document *result = loadXmlFile(filename);
    if (result)
      return result;

    // Find the first occurrence of the library in this path
    vector<string>::const_iterator it = librarySearchPaths.begin();
    while (!result && it != librarySearchPaths.end()) {
      string candidateFile = *it + "/" + filename;
      result = loadXmlFile(candidateFile);
      if (result)
        return result;
      ++it;
    }
    return NULL;
  }

  // name could be node name, file name w/ or w/o directory, w/ w/o .plx
  xml_node loadLibraryNode(char const *name)
  {
    string nodeName = name;
    string fname = name;
    size_t pos = fname.rfind(".plx");
    if (pos == string::npos)
      fname += ".plx";
    else
      nodeName = nodeName.substr(0, pos);
    pos = nodeName.find_last_of("/\\");
    if (pos != string::npos)
      nodeName = nodeName.substr(++pos);

    xml_document *doc = loadLibraryFile(fname);
    if (!doc)
      return xml_node();
    xml_node theNode = doc->document_element().child(NODE_TAG);
    xml_node nodeIdXml = theNode.child(NODEID_TAG);
    checkParserExceptionWithLocation(nodeIdXml,
                                     doc->document_element(),
                                     "No " << NODEID_TAG << " element in library node, or not a PLEXIL plan");
    char const *nodeId = nodeIdXml.child_value();
    checkParserExceptionWithLocation(nodeName == nodeId,
                                     nodeIdXml,
                                     "loadLibraryNode: Requested " << nodeName
                                     << " but file contains " << nodeId);
    addLibraryNode(nodeName.c_str(), doc);
    return theNode;
  }

  xml_node getLibraryNode(char const *name, bool loadIfNotFound)
  {
    LibraryMap::iterator it = libraryMap.find<char const *, CStringComparator>(name);
    if (it != libraryMap.end())
      return it->second->document_element().child(NODE_TAG);
    else if (loadIfNotFound)
      return loadLibraryNode(name);
    else
      return xml_node();
  }

} // namespace PLEXIL
