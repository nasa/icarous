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

#include "Assignable.hh"
#include "Error.hh"

namespace PLEXIL {

  //
  // Assignable
  //

  Assignable::Assignable()
  {
  }

  Assignable::~Assignable()
  {
  }

  /**
   * @brief Get the node that owns this expression.
   * @return The parent node; may be NULL.
   * @note Used by LuvFormat::formatAssignment().  
   * @note Default method.
   */
  NodeConnector *Assignable::getNode()
  {
    return NULL;
  }

  NodeConnector const *Assignable::getNode() const
  {
    return NULL;
  }

  /**
   * @brief Set the expression from which this object gets its initial value.
   * @param expr Pointer to an Expression.
   * @param garbage True if the expression should be deleted with this object, false otherwise.
   * @note Default method.
   */
  void Assignable::setInitializer(Expression * /* expr */, bool /* garbage */)
  {
    assertTrue_2(ALWAYS_FAIL,
                 "setInitializer() called on Assignable that doesn't have an initializer");
  }

} // namespace PLEXIL
