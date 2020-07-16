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

#ifndef NODE_FACTORY_HH
#define NODE_FACTORY_HH

#include "NodeConstants.hh"
#include "PlexilNodeType.hh"

namespace PLEXIL
{

  class Node;

  /**
   * @brief Abstract factory class for Node instances.
   */

  class NodeFactory
  {
  public:
    virtual ~NodeFactory();

    /**
     * @brief Primary factory method.
     */
    static Node *createNode(char const *name,
                            PlexilNodeType type,
                            Node *parent = NULL);

    /**
     * @brief Alternate factory method.  Used only by Exec test module.
     */
    static Node *createNode(const std::string& type, 
                            const std::string& name, 
                            NodeState state,
                            Node *parent = NULL);

  protected:

    // Base class constructor only available to derived classes.
    NodeFactory();

    /**
     * @brief Primary factory method delegated to derived classes.
     */
    virtual Node *create(char const *name, 
                         Node *parent = NULL) const = 0;

    /**
     * @brief Alternate factory method.  Used only by Exec test module.
     */
    virtual Node *create(const std::string& type,
                         const std::string& name,
                         NodeState state,
                         Node *parent = NULL) const = 0;

  private:
    // Deliberately unimplemented
    NodeFactory(const NodeFactory&);
    NodeFactory& operator=(const NodeFactory&);
  };

}

#endif // NODE_FACTORY_HH
