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

#ifndef PLEXIL_CONSTANT_HH
#define PLEXIL_CONSTANT_HH

#include "ArrayImpl.hh"
#include "GetValueImpl.hh"

namespace PLEXIL {

  /**
   * @class Constant
   * @brief Templatized class for expressions whose value cannot change.
   *        Expression listeners are therefore not required.
   */

  template <typename T>
  class Constant :
    public GetValueImpl<T>
  {
  public:

    /**
     * @brief Default constructor.
     */
    Constant();

    /**
     * @brief Copy constructor.
     */
    Constant(const Constant &other);

    /**
     * @brief Constructor from value type.
     */
    Constant(const T &value);

    /**
     * @brief Constructor from char *.
     * @note Unimplemented conversions will cause a link time error.
     */
    Constant(const char * value);

    /**
     * @brief Destructor.
     */
    virtual ~Constant();

    /**
     * @brief Return a print name for the expression type.
     * @return A constant character string.
     */
    const char *exprName() const;

    /**
     * @brief Retrieve the value of this Expression in its native type.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown.
     */
    bool getValue(T &result) const;

    /**
     * @brief Query whether the expression's value is known.
     * @return True if known, false otherwise.
     */
    bool isKnown() const;

    /**
     * @brief Query whether this expression is constant, i.e. incapable of change.
     * @return True if constant, false otherwise.
     */
    bool isConstant() const;

    /**
     * @brief Query whether this expression is a source of change events.
     * @return True if the value may change independently of any subexpressions, false otherwise.
     */
    virtual bool isPropagationSource() const;

    /**
     * @brief Is this expression active (i.e. propagating value changes?)
     * @return true if this Expression is active, false if it is not.
     */
    virtual bool isActive() const;

    /**
     * @brief Make this expression active.
     * @note No-op for constants.
     */
    virtual void activate();

    /**
     * @brief Make this expression inactive.
     * @note No-op for constants.
     */
    virtual void deactivate();

    /**
     * @brief Add a listener for changes to this Expression's value.
     * @param ptr The pointer to the listener to add.
     * @note No-op for constants.
     */
    virtual void addListener(ExpressionListener * /* ptr */);

    /**
     * @brief Remove a listener from this Expression.
     * @param ptr The pointer to the listener to remove.
     * @note No-op for constants.
     */
    virtual void removeListener(ExpressionListener * /* ptr */);

  protected:

    T m_value;
    bool m_known;

  private:

    // Disallow assignment
    Constant &operator=(const Constant &);
  };

  // String is different
  template <>
  class Constant<String> :
    public GetValueImpl<String>
  {
  public:

    /**
     * @brief Default constructor.
     */
    Constant();

    /**
     * @brief Copy constructor.
     */
    Constant(const Constant &other);

    /**
     * @brief Constructor from value type.
     */
    Constant(const String &value);

    /**
     * @brief Constructor from char *.
     * @note Unimplemented conversions will cause a link time error.
     */
    Constant(const char * value);

    /**
     * @brief Destructor.
     */
    virtual ~Constant();

    /**
     * @brief Return a print name for the expression type.
     * @return A constant character string.
     */
    const char *exprName() const;

    /**
     * @brief Retrieve the value of this Expression in its native type.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown.
     */
    bool getValue(String &result) const;

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    bool getValuePointer(String const *& ptr) const;

    /**
     * @brief Query whether the expression's value is known.
     * @return True if known, false otherwise.
     */
    bool isKnown() const;

    /**
     * @brief Query whether this expression is constant, i.e. incapable of change.
     * @return True if constant, false otherwise.
     */
    bool isConstant() const;

    /**
     * @brief Query whether this expression is a source of change events.
     * @return True if the value may change independently of any subexpressions, false otherwise.
     */
    virtual bool isPropagationSource() const;

    /**
     * @brief Is this expression active (i.e. propagating value changes?)
     * @return true if this Expression is active, false if it is not.
     */
    virtual bool isActive() const;

    /**
     * @brief Make this expression active.
     * @note No-op for constants.
     */
    virtual void activate();

    /**
     * @brief Make this expression inactive.
     * @note No-op for constants.
     */
    virtual void deactivate();

    /**
     * @brief Add a listener for changes to this Expression's value.
     * @param ptr The pointer to the listener to add.
     * @note No-op for constants.
     */
    virtual void addListener(ExpressionListener * /* ptr */);

    /**
     * @brief Remove a listener from this Expression.
     * @param ptr The pointer to the listener to remove.
     * @note No-op for constants.
     */
    virtual void removeListener(ExpressionListener * /* ptr */);

  protected:

    String m_value;
    bool m_known;

  private:

    // Disallow assignment
    Constant &operator=(const Constant &);
  };

  // Array types
  template <typename T>
  class Constant<ArrayImpl<T> > :
    public GetValueImpl<ArrayImpl<T> >
  {
  public:

    /**
     * @brief Default constructor.
     */
    Constant();

    /**
     * @brief Copy constructor.
     */
    Constant(const Constant &other);

    /**
     * @brief Constructor from value type.
     */
    Constant(const ArrayImpl<T> &value);

    /**
     * @brief Destructor.
     */
    virtual ~Constant();

    /**
     * @brief Return a print name for the expression type.
     * @return A constant character string.
     */
    const char *exprName() const;

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    bool getValuePointer(ArrayImpl<T> const *& ptr) const;

    /**
     * @brief Query whether the expression's value is known.
     * @return True if known, false otherwise.
     */
    bool isKnown() const;

    /**
     * @brief Query whether this expression is constant, i.e. incapable of change.
     * @return True if constant, false otherwise.
     */
    bool isConstant() const;

    /**
     * @brief Query whether this expression is a source of change events.
     * @return True if the value may change independently of any subexpressions, false otherwise.
     */
    virtual bool isPropagationSource() const;

    /**
     * @brief Is this expression active (i.e. propagating value changes?)
     * @return true if this Expression is active, false if it is not.
     */
    virtual bool isActive() const;

    /**
     * @brief Make this expression active.
     * @note No-op for constants.
     */
    virtual void activate();

    /**
     * @brief Make this expression inactive.
     * @note No-op for constants.
     */
    virtual void deactivate();

    /**
     * @brief Add a listener for changes to this Expression's value.
     * @param ptr The pointer to the listener to notify.
     * @note No-op for constants.
     */
    virtual void addListener(ExpressionListener * /* ptr */);

    /**
     * @brief Remove a listener from this Expression.
     * @param ptr The pointer to the listener to remove.
     * @note No-op for constants.
     */
    virtual void removeListener(ExpressionListener * /* ptr */);

  protected:

    ArrayImpl<T> m_value;
    bool m_known;

  private:

    // Disallow assignment
    Constant &operator=(const Constant &);
  };


  //
  // Convenience typedefs
  //

  typedef Constant<Boolean>   BooleanConstant;
  typedef Constant<Integer>   IntegerConstant;
  typedef Constant<Real>      RealConstant;
  typedef Constant<String>    StringConstant;

  typedef Constant<BooleanArray>   BooleanArrayConstant;
  typedef Constant<IntegerArray>   IntegerArrayConstant;
  typedef Constant<RealArray>      RealArrayConstant;
  typedef Constant<StringArray>    StringArrayConstant;
  
} // namespace PLEXIL

#endif // PLEXIL_CONSTANT_HH
