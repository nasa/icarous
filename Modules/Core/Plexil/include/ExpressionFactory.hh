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

#ifndef EXPRESSION_FACTORY_HH
#define EXPRESSION_FACTORY_HH

#include "Expression.hh"
#include "ParserException.hh"

// Forward declaration
namespace pugi
{
  class xml_node;
}

namespace PLEXIL
{
  // Forward declarations
  class NodeConnector;

  /**
   * @class ExpressionFactory
   * @brief Virtual base class establishing the API for concrete factories for Expression subtypes.
   */

  class ExpressionFactory
  {
  public:
    ExpressionFactory(const std::string& name);
    virtual ~ExpressionFactory();

    virtual Expression *allocate(pugi::xml_node const expr,
                                 NodeConnector *node,
                                 bool & wasCreated,
                                 ValueType returnType = UNKNOWN_TYPE) const = 0;

  private:
    // Default, copy, assign all prohibited
    ExpressionFactory();
    ExpressionFactory(const ExpressionFactory &);
    ExpressionFactory &operator=(const ExpressionFactory &);

  protected:
    const std::string m_name; /*!< Name used for lookup and error reporting */
  };

  /**
   * @brief Creates a new Expression instance with the type associated with the
   *        given expression specification.
   * @param expr The expression specification.
   * @param node Node for name lookup.
   * @return Pointer to the new Expression. May not be unique.
   * @note Convenience wrapper.
   */

  extern Expression *createExpression(pugi::xml_node const expr,
                                      NodeConnector *node = NULL)
    throw (ParserException);

  /**
   * @brief Creates a new Expression instance with the type associated with the
   *        given expression prototype.
   * @param expr The expression spec.
   * @param node Node for name lookup.
   * @return Pointer to the new Expression. May not be unique.
   * @param wasCreated Reference to a boolean variable;
   *                   variable will be set to true if new object created, false otherwise.
   */
  // Used in AssignmentNode, CommandNode, LibraryCallNode, Node::createConditions
  extern Expression *createExpression(pugi::xml_node const expr,
                                      NodeConnector *node,
                                      bool& wasCreated,
                                      ValueType returnType = UNKNOWN_TYPE)
    throw (ParserException);

  // Used in AssignmentNode, CommandNode
  extern Expression *createAssignable(pugi::xml_node const expr,
                                      NodeConnector *node,
                                      bool& wasCreated)
    throw (ParserException);

  /**
   * @brief Deallocate all factories
   */
  extern void purgeExpressionFactories();

} // namespace PLEXIL

#endif // EXPRESSION_FACTORY_HH
