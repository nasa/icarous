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

#include "ExprVec.hh"
#include "Error.hh"
#include "Expression.hh"
#include "ParserException.hh"

namespace PLEXIL
{

  //
  // FixedExprVec
  //
  // General cases - optimized cases below
  //

  /**
   * @class FixedExprVec
   * @brief Concrete class template for small expression vectors.
   */

  template <unsigned N>
  class FixedExprVec : public ExprVec
  {
  public:
    FixedExprVec()
      : ExprVec()
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i] = NULL;
      for (size_t i = 0; i < N; ++i)
        garbage[i] = false;
    }

    ~FixedExprVec()
    {
      for (size_t i = 0; i < N; ++i)
        if (exprs[i] && garbage[i])
          delete exprs[i];
    }

    size_t size() const 
    {
      return N; 
    }

    Expression const *operator[](size_t n) const
    {
      check_error_1(n < N);
      return exprs[n]; 
    }

    Expression *operator[](size_t n)
    {
      check_error_1(n < N);
      return exprs[n]; 
    }

    void setArgument(size_t i, Expression *exp, bool isGarbage)
    {
      assertTrue_2(i < N, "setArgument(): too many args");
      exprs[i] = exp;
      garbage[i] = isGarbage;
    }

    void activate()
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i]->activate();
    }

    void deactivate()
    {
      for (size_t i = 0; i < N; ++i)
        exprs[i]->deactivate();
    }

    virtual void doSubexprs(ExprUnaryOperator const &f)
    {
      for (size_t i = 0; i < N; ++i)
        (f)(exprs[i]);
    }

    void print(std::ostream & s) const
    {
      for (size_t i = 0; i < N; ++i) {
        s << ' ';
        exprs[i]->print(s);
      }
    }

  private:

    // Not implemented
    FixedExprVec(FixedExprVec const &);
    FixedExprVec &operator=(FixedExprVec const &);

    Expression *exprs[N];
    bool garbage[N];
  };


  //
  // GeneralExprVec
  //

  /**
   * @class GeneralExprVec
   * @brief Concrete variable-length variant of ExprVec which uses dynamically allocated arrays.
   */
  class GeneralExprVec : public ExprVec
  {
  public:
    GeneralExprVec(size_t n)
      : ExprVec(),
        m_size(n),
        exprs(new Expression*[n]()),
        garbage(new bool[n]())
    {
    }

    ~GeneralExprVec()
    {
      for (size_t i = 0; i < m_size; ++i)
        if (exprs[i] && garbage[i])
          delete exprs[i];
      delete[] garbage;
      delete[] exprs;
    }

    size_t size() const
    {
      return m_size; 
    }

    Expression const *operator[](size_t n) const
    {
      check_error_1(n < m_size);
      return exprs[n]; 
    }

    Expression *operator[](size_t n)
    {
      check_error_1(n < m_size);
      return exprs[n]; 
    }

    void setArgument(size_t i, Expression *exp, bool isGarbage)
    {
      assertTrue_2(i < m_size, "setArgument(): too many args");
      exprs[i] = exp;
      garbage[i] = isGarbage;
    }

    void activate()
    {
      for (size_t i = 0; i < m_size; ++i)
        exprs[i]->activate();
    }
      
    void deactivate()
    {
      for (size_t i = 0; i < m_size; ++i)
        exprs[i]->deactivate();
    }

    void print(std::ostream & s) const
    {
      for (size_t i = 0; i < m_size; ++i) {
        s << ' ';
        exprs[i]->print(s);
      }
    }

    virtual void doSubexprs(ExprUnaryOperator const &f)
    {
      for (size_t i = 0; i < m_size; ++i)
        (f)(exprs[i]);
    }

  private:

    // Not implemented
    GeneralExprVec();
    GeneralExprVec(GeneralExprVec const &);
    GeneralExprVec &operator=(GeneralExprVec const &);

    size_t m_size;
    Expression **exprs;
    bool *garbage;
  };

  //
  // Factory function
  //

  ExprVec *makeExprVec(size_t n)
  {
    switch (n) {
    case 0:
      assertTrue_2(false, "makeExprVec: zero-length vector not implemented");
      return NULL;
      
    case 1:
      return static_cast<ExprVec *>(new FixedExprVec<1>());

    case 2:
      return static_cast<ExprVec *>(new FixedExprVec<2>());

    case 3:
      return static_cast<ExprVec *>(new FixedExprVec<3>());

    case 4:
      return static_cast<ExprVec *>(new FixedExprVec<4>());

    default: // anything greater than 4
      return static_cast<ExprVec *>(new GeneralExprVec(n));
    }
  }

  template class FixedExprVec<1>;
  template class FixedExprVec<2>;
  template class FixedExprVec<3>;
  template class FixedExprVec<4>;

} // namespace PLEXIL
