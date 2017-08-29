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

#include "plexil-config.h"

#include "Debug.hh"
#include "Error.hh"
#include "Expressions.hh"
#include "Node.hh"
#include "lifecycle-utils.h"
#include "parsePlan.hh"
#include "planLibrary.hh"
#include "pugixml.hpp"
#include "test/TransitionExternalInterface.hh"

#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <string>
#include <cstring>

using PLEXIL::Expression;
using PLEXIL::Node;
using PLEXIL::NodeType_error;
using PLEXIL::NodeType_NodeList;
using PLEXIL::NodeType_LibraryNodeCall;
using PLEXIL::nodeTypeString;
using PLEXIL::NotifierImpl;
using PLEXIL::PlexilNodeType;

static size_t g_nodeCount = 0;

static size_t g_nodeTypeCounts[NodeType_error];
static size_t g_conditionCounts[Node::conditionIndexMax];
static size_t g_conditionCountNodes[Node::conditionIndexMax];
static size_t g_conditionCountListNodes[Node::conditionIndexMax];

// Initialize to semi-sane values
static std::vector<size_t> g_nodeChildCounts(16, 0);
static std::vector<size_t> g_nodeVariableCounts(16, 0);
#ifdef RECORD_EXPRESSION_STATS
static std::map<size_t, size_t> g_expressionListenerCounts;
static std::map<size_t, std::vector<NotifierImpl const *> > g_expressions;
static size_t g_expressionCount = 0;
static size_t g_listenerHighWater = 0;
static NotifierImpl const *g_highWaterExpression = NULL;
static bool g_expressionDetailedReport = false;
#endif

typedef std::map<std::string, size_t> LibraryCallMap;
typedef std::pair<std::string, size_t> LibraryCallEntry;
LibraryCallMap g_calledLibs;

static size_t g_currentCallDepth = 0;
static size_t g_maxCallDepth = 0;

static void incrementNodeChildCount(size_t nKids)
{
  if (nKids >= g_nodeChildCounts.size())
    g_nodeChildCounts.resize(nKids * 2, 0); // grow rapidly to minimize resizing
  ++g_nodeChildCounts[nKids];
}

static void incrementNodeVariableCount(size_t nVars)
{
  if (nVars >= g_nodeVariableCounts.size())
    g_nodeVariableCounts.resize(nVars * 2, 0); // grow rapidly to minimize resizing
  ++g_nodeVariableCounts[nVars];
}

static void incrementLibraryCallCount(std::string const &name)
{
  LibraryCallMap::iterator it = g_calledLibs.find(name);
  if (it == g_calledLibs.end())
    g_calledLibs.insert(LibraryCallEntry(name, 1));
  else
    ++it->second;
}

static void initializeStatistics()
{
  g_nodeCount = 0;
  g_currentCallDepth = g_maxCallDepth = 0;
  for (size_t i = 0; i < NodeType_error; ++i)
    g_nodeTypeCounts[i] = 0;
  g_nodeChildCounts.clear();
  g_nodeVariableCounts.clear();
#ifdef RECORD_EXPRESSION_STATS
  g_expressionCount = 0;
  g_expressionListenerCounts.clear();
  g_expressions.clear();
#endif
  for (size_t i = 0; i < Node::conditionIndexMax; ++i)
    g_conditionCounts[i] = 0;
  for (size_t i = 0; i < Node::conditionIndexMax; ++i)
    g_conditionCountNodes[i] = 0;
  for (size_t i = 0; i < Node::conditionIndexMax; ++i)
    g_conditionCountListNodes[i] = 0;
  g_calledLibs.clear();
}

static Expression const *getNodeCondition(Node const *node, size_t idx)
{
  switch (idx) {
  case Node::ancestorExitIdx:
  case Node::ancestorInvariantIdx:
  case Node::ancestorEndIdx: {
    std::vector<Node *> const &kids = node->getChildren();
    if (kids.empty())
      return NULL;
    return (const_cast<Node const *>(kids.front()))->getCondition(idx);
  }

  default:
    return node->getCondition(idx);
  }

}

static void getConditionStatistics(Node const *node)
{
  // Count total conditions on this node
  size_t nConds = 0;
  for (size_t i = 0 ; i < Node::conditionIndexMax; ++i) 
    if (getNodeCondition(node, i)) {
      ++nConds;
      ++g_conditionCounts[i];
    }
  ++g_conditionCountNodes[nConds];
  if (node->getType() == NodeType_NodeList)
    ++g_conditionCountListNodes[nConds];
}

// Recursive function for plan traversal
static void getNodeStatistics(Node const *node)
{
  ++g_nodeCount;

  // Count node type
  PlexilNodeType typ = node->getType();
  ++g_nodeTypeCounts[typ];

  // Count variables
  if (node->getLocalVariables())
    incrementNodeVariableCount(node->getLocalVariables()->size());

  getConditionStatistics(node);

  // Count children
  std::vector<Node *> const &kids = node->getChildren();
  incrementNodeChildCount(kids.size());

  switch (typ) {
  case NodeType_LibraryNodeCall:
    incrementLibraryCallCount(kids.front()->getNodeId());
    ++g_currentCallDepth;
    if (g_currentCallDepth > g_maxCallDepth)
      g_maxCallDepth = g_currentCallDepth;

    // Recurse on child
    getNodeStatistics(kids.front());

    --g_currentCallDepth;
    break;

  default:
  for (std::vector<Node *>::const_iterator it = kids.begin();
       it != kids.end();
       ++it)
    getNodeStatistics(*it);
    break;
  }
}

#ifdef RECORD_EXPRESSION_STATS
static void recordExpression(NotifierImpl const *exp)
{
  ++g_expressionCount;
  size_t nListeners = exp->getListenerCount();
  if (nListeners > g_listenerHighWater) {
    g_listenerHighWater = nListeners;
    g_highWaterExpression = exp;
  }
  ++g_expressionListenerCounts[nListeners];
  if (g_expressionDetailedReport)
    g_expressions[nListeners].push_back(exp);
}

static void getExpressionStatistics()
{
  for (NotifierImpl const *exp = NotifierImpl::getInstanceList(); exp; exp = exp->next())
    recordExpression(exp);
}
#endif

//
// Reporting
//

static void reportLibraryStatistics()
{
  if (!g_calledLibs.empty()) {
    std::cout << "\n--- Libraries ---\n\n";
    std::cout << "Maximum library call depth: " << g_maxCallDepth << "\n\n";
    std::cout << g_calledLibs.size() << " libraries called:\n\n";
    for (LibraryCallMap::const_iterator it = g_calledLibs.begin();
         it != g_calledLibs.end();
         ++it)
      std::cout << it->first << " called "
                << it->second << " time"
                << (it->second == 1 ? "" : "s")
                << '\n';
    std::cout << '\n';
  }
}

#ifdef RECORD_EXPRESSION_STATS
static void reportExpressionStatistics()
{
  std::cout << "\n--- Expression Listener Counts --- \n\n";
  std::cout << g_expressionCount << " expressions with listeners\n\n";
  std::cout << "Expression " << *g_highWaterExpression
            << "\n has largest count of listeners, " << g_listenerHighWater << "\n\n";

  for (std::map<size_t, size_t>::const_iterator it = g_expressionListenerCounts.begin();
       it != g_expressionListenerCounts.end();
       ++it) {
    std::cout << it->second << " expression"
              << (it->second == 1 ? "" : "s")
              << " with " << it->first << " listener"
              << (it->first == 1 ? "" : "s")
              << '\n';
    if (g_expressionDetailedReport) {
      std::vector<NotifierImpl const *> &exprs = g_expressions[it->first];
      for (size_t j = 0; j < exprs.size(); ++j) {
        std::cout << "  " << *exprs[j] << '\n';
      }
      std::cout << '\n';
    }
  }
  std::cout << '\n';
}
#endif

static void reportStatistics()
{
  std::cout << '\n';
  std::cout << g_nodeCount << " total nodes\n";

  std::cout << "\n--- Node Type Counts --- \n\n";
  for (size_t i = NodeType_NodeList; i < NodeType_error; ++i)
    if (g_nodeTypeCounts[i])
      std::cout << nodeTypeString((PlexilNodeType) i) << ": " << g_nodeTypeCounts[i] << '\n';
  std::cout << '\n';

  std::cout << "\n--- Node Variable Counts --- \n\n";
  for (size_t i = 0; i < g_nodeVariableCounts.size(); ++i)
    if (g_nodeVariableCounts[i])
      std::cout << g_nodeVariableCounts[i] << " nodes with " << i << " variables\n";
  std::cout << '\n';

  std::cout << "\n--- Node Child Counts --- \n\n";
  std::cout << g_nodeChildCounts[0] << " leaf nodes\n";
  std::cout << g_nodeChildCounts[1] << " nodes with 1 child (includes library calls)\n";
  for (size_t i = 2; i < g_nodeChildCounts.size(); ++i)
    if (g_nodeChildCounts[i])
      std::cout << g_nodeChildCounts[i] << " nodes with " << i << " children\n";
  std::cout << '\n';

  std::cout << "\n--- Nodes With Specific Conditions --- \n\n";
  for (size_t i = 0; i < Node::conditionIndexMax; ++i) {
    std::cout << g_conditionCounts[i] << " nodes with "
	      << Node::getConditionName(i) << '\n';
  }
  std::cout << '\n';

  std::cout << "\n--- Total Node Condition Counts --- \n\n";
  for (size_t i = 0; i < Node::conditionIndexMax; ++i) {
    if (g_conditionCountNodes[i])  
      std::cout << g_conditionCountNodes[i] << " nodes with " << i << " conditions\n";
  }
  std::cout << '\n';

  std::cout << "\n--- List Node Condition Counts --- \n\n";
  for (size_t i = 0; i < Node::conditionIndexMax; ++i) {
    if (g_conditionCountListNodes[i])  
      std::cout << g_conditionCountListNodes[i] << " nodes with " << i << " conditions\n";
  }
  std::cout << '\n';

  reportLibraryStatistics();

#ifdef RECORD_EXPRESSION_STATS
  reportExpressionStatistics();
#endif
}

static void loadAndAnalyzePlan(std::string const &planFile)
{
  // Load the XML
  pugi::xml_document *doc = PLEXIL::loadXmlFile(planFile);
  checkParserException(doc, "File " << planFile << " not found");
  
  Node *root = PLEXIL::parsePlan(doc->document_element());
  checkParserException(root, "parsePlan returned NULL");

  // Analyze plan
  initializeStatistics();
  getNodeStatistics(root);
#ifdef RECORD_EXPRESSION_STATS
  getExpressionStatistics();
#endif

  reportStatistics();

  // Clean up
  delete root;
  delete doc;
}

static void usage()
{
  std::cout << "Usage: analyzePlan [options] <plan file>\n"
            << " Options:\n"
            << "  -h               Display this message and exit\n"
            << "  -d <debug file>  Use debug-file as debug message config (default Debug.cfg)\n"
#ifdef RECORD_EXPRESSION_STATS
            << "  -D               Display detailed expression statistics\n"
#endif
            << "  -L <dir>         Add <dir> to library path\n"
            << std::endl;
}

extern "C"
int main(int argc, char *argv[])
{
  std::string debugConfig("Debug.cfg");
  std::string planFile;

  for (int i = 1; i < argc; ++i) {
    if (!strcmp(argv[i], "-d"))
      debugConfig = std::string(argv[++i]);
    else if (!strcmp(argv[i], "-h")) {
      usage();
      return 0;
    }
    else if (!strcmp(argv[i], "-L")) {
      PLEXIL::appendLibraryPath(argv[++i]);
    }
#ifdef RECORD_EXPRESSION_STATS
    else if (!strcmp(argv[i], "-D")) {
      g_expressionDetailedReport = true;
    }
#endif
    else {
      if (!planFile.empty()) {
        std::cerr << "Multiple plan files specified" << std::endl;
        usage();
        return 1;
      }
      planFile = argv[i];
    }
  }

  if (planFile.empty()) {
    std::cerr << "No plan file specified" << std::endl;
    usage();
    return 1;
  }
  
  std::ifstream config(debugConfig.c_str());
  if (config.good()) {
    PLEXIL::readDebugConfigStream(config);
     std::cout << "Reading configuration file " << debugConfig.c_str() << "\n";
  }
  
  std::cout << "Loading plan file " << planFile << " for analysis." << std::endl;

  try {
    // Initialize infrastructure
    PLEXIL::Error::doThrowExceptions();
    PLEXIL::initializeExpressions();
    PLEXIL::TransitionExternalInterface intfc;
    PLEXIL::g_interface = &intfc;

    loadAndAnalyzePlan(planFile);

    PLEXIL::g_interface = NULL;
    plexilRunFinalizers();
  }
  catch (PLEXIL::ParserException const &e) {
    std::cerr << "Aborting due to parser exception:\n" << e.what() << std::endl;
    std::cout << "Aborted." << std::endl;
    return 1;
  }
  catch (PLEXIL::Error const &e) {
    std::cerr << "Aborting due to error:\n" << e << std::endl;
    std::cout << "Aborted." << std::endl;
    return 1;
  }
  std::cout << "Done." << std::endl;
  return 0;
}
