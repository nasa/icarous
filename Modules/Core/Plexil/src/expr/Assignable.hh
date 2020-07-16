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

#ifndef PLEXIL_ASSIGNABLE_HH
#define PLEXIL_ASSIGNABLE_HH

namespace PLEXIL {
  
  // Forward declarations
  class Array;
  class Expression;
  class NodeConnector;
  class Value;

  /**
   * @class Assignable
   * @brief Pure virtual mixin class for all expressions which can be assigned to by a plan.
   * @note Examples include variables, array references, aliases for InOut variables, etc.
   * @note This class has no state of its own.
   */
  class Assignable
  {
  protected:
    Assignable();

  public:
    virtual ~Assignable();

    //
    // Core Assignable API
    // Every Assignable must implement these behaviors
    //

    /**
     * @brief Temporarily stores the previous value of this variable.
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void saveCurrentValue() = 0;

    /**
     * @brief Restore the value set aside by saveCurrentValue().
     * @note Used to implement recovery from failed Assignment nodes.
     */
    virtual void restoreSavedValue() = 0;

    /**
     * @brief Read the saved value of this variable.
     * @return The saved value.
     */
    virtual Value getSavedValue() const = 0;

    /**
     * @brief Get the node that owns this expression.
     * @return The parent node; may be NULL.
     * @note Used by LuvFormat::formatAssignment().  
     * @note Default method returns NULL.
     */
    virtual NodeConnector *getNode();
    virtual NodeConnector const *getNode() const;

    /**
     * @brief Get the real variable for which this may be a proxy.
     * @return Pointer to the base variable.
     * @note Used by the assignment node conflict resolution logic.
     */
    virtual Expression *getBaseVariable() = 0;
    virtual Expression const *getBaseVariable() const = 0;

    /**
     * @brief Set the expression from which this object gets its initial value.
     * @param expr Pointer to an Expression.
     * @param garbage True if the expression should be deleted with this object, false otherwise.
     * @note Default method throws an exception.
     */
    virtual void setInitializer(Expression *expr, bool garbage);

    //
    // SetValue API subset actually used in Exec and test suites
    //

    /**
     * @brief Set the value of this expression to unknown.
     */
    virtual void setUnknown() = 0;

    /**
     * @brief Set the value for this object.
     * @param val The new value.
     */
    virtual void setValue(Value const &val) = 0;

  private:
    // Not implemented
    Assignable(Assignable const &);
    Assignable &operator=(Assignable const &);
  };

} // namespace PLEXIL

#endif // PLEXIL_ASSIGNABLE_HH
