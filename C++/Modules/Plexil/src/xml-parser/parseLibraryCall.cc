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

#include "Debug.hh"
#include "Error.hh"
#include "ExpressionFactory.hh"
#include "LibraryCallNode.hh"
#include "parseNode.hh"
#include "ParserException.hh"
#include "parser-utils.hh"
#include "planLibrary.hh"
#include "PlexilSchema.hh"

#include "pugixml.hpp"
#include <cstring>

using pugi::xml_node;
using pugi::node_element;
using pugi::node_pcdata;

namespace PLEXIL
{

  //
  // First pass
  //

  static void checkLibraryCall(std::string const &callerId, xml_node const callXml)
    throw (ParserException)
  {
    checkTag(LIBRARYNODECALL_TAG, callXml);
    xml_node temp = callXml.first_child();
    checkTag(NODEID_TAG, temp);
    char const *name = temp.child_value();
    checkParserExceptionWithLocation(*name,
                                     temp,
                                     "Empty NodeId for called library in LibraryNodeCall node "
                                     << callerId);
  }

  static void checkAlias(LibraryCallNode *node, xml_node const aliasXml)
    throw (ParserException)
  {
    checkTag(ALIAS_TAG, aliasXml);
    xml_node nameXml = aliasXml.first_child();
    checkTag(NODE_PARAMETER_TAG, nameXml);
    char const *name = nameXml.child_value();
    checkParserExceptionWithLocation(*name,
                                     nameXml,
                                     "NodeParameter element is empty in LibraryNodeCall node "
                                     << node->getNodeId());

    // Check for duplicate parameter names here
    for (xml_node sib = aliasXml.previous_sibling(); sib; sib = sib.previous_sibling()) {
      checkParserExceptionWithLocation(strcmp(name, sib.first_child().child_value()),
                                       aliasXml,
                                       "Multiple aliases for \""
                                       << name
                                       << "\" in LibraryNodeCall node "
                                       << node->getNodeId());
    }
    
    // Basic checks to see that we have something that could be an expression
    xml_node temp = nameXml.next_sibling();
    checkParserExceptionWithLocation(temp,
                                     aliasXml,
                                     "Alias for \"" << name
                                     << "\" without value expression in LibraryNodeCall node "
                                     << node->getNodeId());
    checkParserExceptionWithLocation(temp.type() == node_element && temp.first_child(),
                                     temp,
                                     "Alias for \"" << name
                                     << "\" has malformed value expression in LibraryNodeCall node " 
                                     << node->getNodeId());
  }

  static void allocateAliases(LibraryCallNode *node, xml_node const callXml)
    throw (ParserException)
  {
    // Check, preallocate, but don't populate, aliases
    xml_node temp = callXml.first_child();
    if (!temp)
      return; // no aliases in this call

    size_t nAliases = 0;
    while ((temp = temp.next_sibling())) {
      checkAlias(node, temp);
      ++nAliases;
    }
    node->allocateAliasMap(nAliases);
  }

  void constructLibraryCall(LibraryCallNode *node, xml_node const callXml)
    throw (ParserException)
  {
    assertTrue_1(node);
    debugMsg("constructLibraryCall", " caller " << node->getNodeId());

    checkLibraryCall(node->getNodeId(), callXml);

    allocateAliases(node, callXml);

    // Construct call
    xml_node const templ = getLibraryNode(callXml.first_child().child_value());
    checkParserExceptionWithLocation(templ,
                                     callXml,
                                     "Library node "
                                     << callXml.first_child().child_value()
                                     << " not found while expanding LibraryNodeCall node "
                                     << node->getNodeId());
    node->addChild(parseNode(templ, node));
  }

  // Second pass
  static void finalizeAliases(LibraryCallNode *node, xml_node const callXml)
    throw (ParserException)
  {
    debugMsg("finalizeAliases", " caller " << node->getNodeId());
    // Skip over NodeId
    xml_node aliasXml = callXml.first_child();
    while ((aliasXml = aliasXml.next_sibling())) {
      xml_node const nameXml = aliasXml.first_child();
      debugMsg("finalizeAliases", " constructing alias " << nameXml.child_value());
             
      // Add the alias
      bool isGarbage = false;
      Expression *exp = createExpression(nameXml.next_sibling(), node, isGarbage);
      node->addAlias(nameXml.child_value(), exp, isGarbage);
    }
  }

  // Second pass
  void finalizeLibraryCall(LibraryCallNode *node, xml_node const callXml)
    throw (ParserException)
  {
    assertTrue_1(node);
    debugMsg("finalizeLibraryCall", " caller " << node->getNodeId());

    finalizeAliases(node, callXml);

    xml_node const calleeXml =
      getLibraryNode(callXml.first_child().child_value());

    // These should never happen, but...
    assertTrue_2(calleeXml,
                 "Internal error: LibraryNodeCall can't find XML for called node");
    assertTrue_2(!node->getChildren().empty(),
                 "Internal error: LibraryNodeCall node missing called node");

    finalizeNode(node->getChildren().front(), calleeXml);
  }

} // namespace PLEXIL
