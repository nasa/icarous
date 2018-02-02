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

#ifndef PLEXIL_ARRAY_VARIABLE_HH
#define PLEXIL_ARRAY_VARIABLE_HH

#include "Assignable.hh"
#include "GetValueImpl.hh"
#include "NotifierImpl.hh"

namespace PLEXIL
{

  /**
   * @class ArrayVariable
   * @brief An abstract class similar to UserVariable, which adds accessors required
   *        by the ArrayReference and MutableArrayReference expression classes.
   *        ArrayVariable methods implement the type-invariant operations;
   *        type-specific operations are delegated to a derived class.
   */

  class ArrayVariable :
    public Assignable,
    public NotifierImpl
  {
  public:

    virtual ~ArrayVariable();

    //
    // Essential Expression API
    //

    virtual bool isAssignable() const;

    virtual Assignable const *asAssignable() const;
    virtual Assignable *asAssignable();

    virtual char const *getName() const;

    virtual char const *exprName() const;

    virtual bool isKnown() const;

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    virtual bool getValuePointer(Array const *&ptr) const;

    virtual void saveCurrentValue();

    // Provided by derived class
    // virtual void restoreSavedValue() = 0;

    virtual Value getSavedValue() const;

    virtual NodeConnector const *getNode() const;
    virtual NodeConnector *getNode();

    virtual Expression *getBaseVariable();
    virtual Expression const *getBaseVariable() const;

    /**
     * @brief Set the expression from which this object gets its initial value.
     * @param expr Pointer to an Expression.
     * @param garbage True if the expression should be deleted with this object, false otherwise.
     */
    void setInitializer(Expression *expr, bool garbage);

    /**
     * @brief Set the value for this object.
     * @param val The new value for this object.
     */
    virtual void setValue(Value const &val);

    /**
     * @brief Set the value for this object.
     * @param val The expression with the new value for this object.
     */
    virtual void setValue(Expression const &val);

    /**
     * @brief Set the current value unknown.
     */
    virtual void setUnknown();

    virtual void handleActivate();

    virtual void handleDeactivate();

    virtual void printSpecialized(std::ostream &s) const;

    //
    // Access needed by ArrayReference
    //

    bool elementIsKnown(size_t idx) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result variable.
     * @return True if the value is known, false otherwise.
     * @note Default methods throw a PlanError.
     */
    virtual bool getElement(size_t idx, Boolean &result) const; 
    virtual bool getElement(size_t idx, Integer &result) const;
    virtual bool getElement(size_t idx, Real &result) const;
    virtual bool getElement(size_t idx, String &result) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result pointer variable.
     * @return True if the value is known, false otherwise.
     * @note Default methods throw a PlanError.
     */
    virtual bool getElementPointer(size_t idx, String const *&ptr) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @return The value; may be unknown.
     */
    virtual Value getElementValue(size_t idx) const;

    //
    // Access needed by MutableArrayReference
    //

    virtual void setElement(size_t idx, Value const &value) = 0;

    void setElementUnknown(size_t idx);

  protected:

    //
    // API to derived classes
    //

    /**
     * @brief Default constructor.
     */
    ArrayVariable();

    /**
     * @brief Constructor for plan loading.
     * @param node The node to which this variable belongs (default none).
     * @param name The name of this variable in the parent node.
     * @param size Expression whose value is the initial size of the array.
     * @param sizeIsGarbage True if the size expression should be deleted 
     *                      with the array variable.
     */
    ArrayVariable(NodeConnector *node,
                  char const *name = "",
                  Expression *size = NULL,
                  bool sizeIsGarbage = false);

    /**
     * @brief Copy from a generic array.
     * @param a Pointer to array whose contents are to be copied.
     */
    virtual void setValueImpl(Array const *a) = 0;

    //
    // API provided by derived classes
    //

    /**
     * @brief Compare the given array with the current value.
     * @return True if equal, false if not.
     */

    virtual bool equals(Array const *ary) const = 0;

    /**
     * @brief Construct an array of the appropriate type and requested size.
     * @return Pointer to an Array.
     */

    virtual Array *makeArray(size_t n) const = 0;

    //
    // Member variables
    //

    Array* m_value;
    Array* m_savedValue;   // for undoing assignment 

    Expression *m_size;
    Expression *m_initializer;
    char const *m_name;
    size_t m_maxSize;
    
    // Only used by LuvListener at present. Eliminate?
    NodeConnector *m_node;

    bool m_known;
    bool m_savedKnown;
    bool m_sizeIsGarbage;
    bool m_initializerIsGarbage;
    bool m_sizeIsConstant;
    bool m_initializerIsConstant;
  };

  //
  // Implementation classes
  //

  template <typename T>
  class ArrayVariableImpl :
    public GetValueImpl<ArrayImpl<T> >,
    public ArrayVariable
  {
  public:

    /**
     * @brief Default constructor.
     */
    ArrayVariableImpl();

    /**
     * @brief Constructor for plan loading.
     * @param node The node to which this variable belongs (default none).
     * @param name The name of this variable in the parent node.
     * @param size Expression whose value is the initial size of the array.
     * @param sizeIsGarbage True if the size expression should be deleted 
     *                      with the array variable.
     */
    ArrayVariableImpl(NodeConnector *node,
                      char const *name = "",
                      Expression *size = NULL,
                      bool sizeIsGarbage = false);

    virtual ~ArrayVariableImpl() {}

    /**
     * @brief Return the value type.
     * @return A constant enumeration.
     */
    virtual ValueType valueType() const;

    /**
     * @brief Determine whether the value is known or unknown.
     * @return True if known, false otherwise.
     * @note Just defer to ArrayVariable method.
     */
    virtual bool isKnown() const
    {
      return ArrayVariable::isKnown();
    }

    /**
     * @brief Assign a new value.
     * @param value The value to assign.
     */
    virtual void setValueImpl(Array const *a);

    virtual void restoreSavedValue();

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     * @note Simply delegates to ArrayVariable method.
     */
    virtual bool getValuePointer(Array const *& ptr) const
    {
      return ArrayVariable::getValuePointer(ptr);
    }
    
    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    virtual bool getValuePointer(ArrayImpl<T> const *&ptr) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result variable.
     * @return True if the value is known, false otherwise.
     */
    virtual bool getElement(size_t idx, T &result) const; 

    virtual void setElement(size_t idx, Value const &value);

    //
    // API to base class
    //

    /**
     * @brief Compare the given array with the current value.
     * @return True if equal, false if not.
     */

    virtual bool equals(Array const *ary) const;

    /**
     * @brief Construct an array of the appropriate type and requested size.
     * @return Pointer to an Array.
     */

    virtual Array *makeArray(size_t n) const;

  private:

    //
    // Internal utilities
    //

    ArrayImpl<T> const *typedArrayPointer() const;
    ArrayImpl<T> *typedArrayPointer();

  };

  template <>
  class ArrayVariableImpl<Integer> :
    public GetValueImpl<ArrayImpl<Integer> >,
    public ArrayVariable
  {
  public:

    /**
     * @brief Default constructor.
     */
    ArrayVariableImpl();

    /**
     * @brief Constructor for plan loading.
     * @param node The node to which this variable belongs (default none).
     * @param name The name of this variable in the parent node.
     * @param size Expression whose value is the initial size of the array.
     * @param sizeIsGarbage True if the size expression should be deleted 
     *                      with the array variable.
     */
    ArrayVariableImpl(NodeConnector *node,
                      char const *name = "",
                      Expression *size = NULL,
                      bool sizeIsGarbage = false);

    virtual ~ArrayVariableImpl() {}

    /**
     * @brief Return the value type.
     * @return A constant enumeration.
     */
    virtual ValueType valueType() const;

    /**
     * @brief Determine whether the value is known or unknown.
     * @return True if known, false otherwise.
     * @note Just defer to ArrayVariable method.
     */
    virtual bool isKnown() const
    {
      return ArrayVariable::isKnown();
    }

    /**
     * @brief Assign a new value.
     * @param value The value to assign.
     */
    virtual void setValueImpl(Array const *a);

    virtual void restoreSavedValue();

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     * @note Simply delegates to ArrayVariable method.
     */
    virtual bool getValuePointer(Array const *& ptr) const
    {
      return ArrayVariable::getValuePointer(ptr);
    }
    
    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    virtual bool getValuePointer(ArrayImpl<Integer> const *&ptr) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result variable.
     * @return True if the value is known, false otherwise.
     */
    virtual bool getElement(size_t idx, Integer &result) const; 

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result variable.
     * @return True if the value is known, false otherwise.
     * @note Conversion method.
     */
    virtual bool getElement(size_t idx, Real &result) const; 

    virtual void setElement(size_t idx, Value const &value);

    /**
     * @brief Compare the given array with the current value.
     * @return True if equal, false if not.
     */

    virtual bool equals(Array const *ary) const;

    /**
     * @brief Construct an array of the appropriate type and requested size.
     * @return Pointer to an Array.
     */

    virtual Array *makeArray(size_t n) const;

  private:

    //
    // Internal utilities
    //

    ArrayImpl<Integer> const *typedArrayPointer() const;
    ArrayImpl<Integer> *typedArrayPointer();

  };

  template <>
  class ArrayVariableImpl<String> :
    public GetValueImpl<ArrayImpl<String> >,
    public ArrayVariable
  {
  public:

    /**
     * @brief Default constructor.
     */
    ArrayVariableImpl();

    /**
     * @brief Constructor for plan loading.
     * @param node The node to which this variable belongs (default none).
     * @param name The name of this variable in the parent node.
     * @param size Expression whose value is the initial size of the array.
     * @param sizeIsGarbage True if the size expression should be deleted 
     *                      with the array variable.
     */
    ArrayVariableImpl(NodeConnector *node,
                      char const *name = "",
                      Expression *size = NULL,
                      bool sizeIsGarbage = false);

    virtual ~ArrayVariableImpl() {}

    /**
     * @brief Return the value type.
     * @return A constant enumeration.
     */
    virtual ValueType valueType() const;

    /**
     * @brief Determine whether the value is known or unknown.
     * @return True if known, false otherwise.
     * @note Just defer to ArrayVariable method.
     */
    virtual bool isKnown() const
    {
      return ArrayVariable::isKnown();
    }

    /**
     * @brief Assign a new value.
     * @param value The value to assign.
     */
    virtual void setValueImpl(Array const *a);

    virtual void restoreSavedValue();

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     * @note Simply delegates to ArrayVariable method.
     */
    virtual bool getValuePointer(Array const *& ptr) const
    {
      return ArrayVariable::getValuePointer(ptr);
    }
    
    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown.
     */
    virtual bool getValuePointer(ArrayImpl<String> const *&ptr) const;

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result variable.
     * @return True if the value is known, false otherwise.
     */
    virtual bool getElement(size_t idx, String &result) const; 

    /**
     * @brief Get the value of the element of the array.
     * @param idx The index.
     * @param result Reference to the result pointer variable.
     * @return True if the value is known, false otherwise.
     */
    virtual bool getElementPointer(size_t idx, String const *&ptr) const;

    virtual void setElement(size_t idx, Value const &value);

    /**
     * @brief Compare the given array with the current value.
     * @return True if equal, false if not.
     */

    virtual bool equals(Array const *ary) const;

    /**
     * @brief Construct an array of the appropriate type and requested size.
     * @return Pointer to an Array.
     */

    virtual Array *makeArray(size_t n) const;

  private:

    //
    // Internal utilities
    //

    ArrayImpl<String> const *typedArrayPointer() const;
    ArrayImpl<String> *typedArrayPointer();

  };

  typedef ArrayVariableImpl<Boolean> BooleanArrayVariable;
  typedef ArrayVariableImpl<Integer> IntegerArrayVariable;
  typedef ArrayVariableImpl<Real>    RealArrayVariable;
  typedef ArrayVariableImpl<String>  StringArrayVariable;

} // namespace PLEXIL

#endif // PLEXIL_ARRAY_VARIABLE_HH
