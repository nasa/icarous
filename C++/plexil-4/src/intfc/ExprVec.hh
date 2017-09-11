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

#ifndef PLEXIL_EXPR_VEC_HH
#define PLEXIL_EXPR_VEC_HH

#include "Expression.hh"
#include "ValueType.hh"

#include <vector>

namespace PLEXIL
{
  class ExpressionListener;

  /**
   * @class ExprVec
   * @brief Virtual base class for a family of expression vector classes,
   * whose representations vary by size.
   */

  class ExprVec
  {
  public:
    virtual ~ExprVec() {}

    virtual size_t size() const = 0;
    virtual Expression const *operator[](size_t n) const = 0;
    virtual Expression *operator[](size_t n) = 0;
    virtual void setArgument(size_t i, Expression *exp, bool garbage) = 0;
    virtual void doSubexprs(ExprUnaryOperator const &f) = 0;
    virtual void print(std::ostream &s) const = 0;

    // These are in critical path of exec inner loop, 
    // so should be optimized for each representation
    virtual void activate() = 0;
    virtual void deactivate() = 0;

  protected:

    // Only available to derived classes
    ExprVec() {}

  private:

    // Not implemented
    ExprVec(ExprVec const &);
    ExprVec &operator=(ExprVec const &);

  };

  // Factory function
  extern ExprVec *makeExprVec(size_t nargs);

} // namespace PLEXIL

#endif // PLEXIL_EXPR_VEC_HH
