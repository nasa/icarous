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

#ifndef COMMAND_NODE_HH
#define COMMAND_NODE_HH

#include "Node.hh"
#include "Constant.hh"

namespace PLEXIL
{
  // Forward reference
  class Command;

  class CommandNode : public Node
  {
  public:

    /**
     * @brief The constructor.
     * @param nodeId The name of this node.
     * @param parent The parent of this node (used for the ancestor conditions and variable lookup).
     */
    CommandNode(char const *nodeId, Node *parent = NULL);

    /**
     * @brief Alternate constructor.  Used only by Exec test module.
     */
    CommandNode(const std::string& type,
                const std::string& name,
                NodeState state,
                Node *parent = NULL);

    /**
     * @brief Destructor.  Cleans up this entire part of the node tree.
     */
    virtual ~CommandNode();

    /**
     * @brief Gets the type of this node.
     * @return The type of this node.
     */
    virtual PlexilNodeType getType() const
    {
      return NodeType_Command;
    }

    // Called from the transition handler
    virtual void abort();

    /**
     * @brief Get the node's command.
     */
    Command const *getCommand() const { return m_command; }
    Command *getCommand() { return m_command; }

    /**
     * @brief Set the command.
     * @param cmd The command.
     * @note Should only be used by plan parser and unit test.
     */
    void setCommand(Command *cmd);

  protected:

    // Specific behaviors for derived classes
    virtual void specializedCreateConditionWrappers();
    virtual void specializedHandleExecution();
    virtual void specializedDeactivateExecutable();

    virtual bool getDestStateFromExecuting();
    virtual bool getDestStateFromFinishing();
    virtual bool getDestStateFromFailing();

    virtual void transitionToExecuting();
    virtual void transitionToFinishing();
    virtual void transitionToFailing();

    virtual void transitionFromExecuting();
    virtual void transitionFromFinishing();
    virtual void transitionFromFailing();

    virtual void printCommandHandle(std::ostream& stream, const unsigned int indent) const;

    virtual void cleanUpNodeBody();

    // Node state limit
    virtual NodeState nodeStateMax() const { return FINISHING_STATE; }

  private:

    // Unit test support
    void createDummyCommand(); // unit test variant

    Command *m_command; /*<! The command to be performed. */
  };

}

#endif // COMMAND_NODE_HH
