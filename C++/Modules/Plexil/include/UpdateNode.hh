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

#ifndef UPDATE_NODE_HH
#define UPDATE_NODE_HH

#include "Node.hh"

namespace PLEXIL
{
  class Update;

  class UpdateNode : public Node
  {
  public:

    /**
     * @brief The constructor.
     * @param nodeId The name of this node.
     * @param parent The parent of this node (used for the ancestor conditions and variable lookup).
     */
    UpdateNode(char const *nodeId, Node *parent = NULL);

    /**
     * @brief Alternate constructor.  Used only by Exec test module.
     */
    UpdateNode(const std::string& type,
               const std::string& name,
               NodeState state,
               Node *parent = NULL);

    /**
     * @brief Destructor.  Cleans up this entire part of the node tree.
     */
    virtual ~UpdateNode();

    /**
     * @brief Gets the type of this node.
     * @return The type of this node.
     */
    virtual PlexilNodeType getType() const
    {
      return NodeType_Update;
    }

    /**
     * @brief Get the node's update structure.
     */
    Update *getUpdate()
    {
      return m_update; 
    }

    /**
     * @brief Set the update.
     * @param upd The update.
     * @note Should only be used by plan parser and unit tests.
     */
    void setUpdate(Update *upd);

  protected:

    // Specific behaviors for derived classes
    virtual void specializedCreateConditionWrappers();
    virtual void specializedHandleExecution();
    virtual void specializedDeactivateExecutable();

    virtual bool getDestStateFromExecuting();
    virtual bool getDestStateFromFailing();

    virtual void transitionToFailing();

    virtual void transitionFromExecuting();
    virtual void transitionFromFailing();

    virtual void abort();

    virtual void cleanUpNodeBody();

    // Node state limit
    virtual NodeState nodeStateMax() const { return FAILING_STATE; }

  private:
    void createDummyUpdate(); // for unit test

    Update *m_update;
  };

}

#endif // UPDATE_NODE_HH
