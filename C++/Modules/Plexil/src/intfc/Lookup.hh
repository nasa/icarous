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

#ifndef PLEXIL_LOOKUP_HH
#define PLEXIL_LOOKUP_HH

#include "NotifierImpl.hh"
#include "State.hh"

namespace PLEXIL
{

  // Forward references
  class CachedValue;
  class ExprVec;
  class StateCacheEntry;
  class ThresholdCache; // local to LookupOnChange

  // OPEN QUESTIONS -
  // - Registry for Lookup, Command param/return types
  //
  // FORMERLY OPEN QUESTIONS -
  // - Local cache for last value? (Yes, for now; revisit when we can profile)
  // - Access to Exec "globals":
  //  StateCacheMap instance - via singleton pattern
  //  cycle count (timestamp) - via g_exec global var

  //
  // Lookup use cases
  //
  // LookupNow
  //  - external i/f queried on demand synchronously
  //  - may be active for more than one Exec cycle, so updates possible
  //
  // LookupOnChange
  //  - grab from external i/f or state cache at initial activation
  //  - data updates triggered by interface
  //  - frequently active for many Exec cycles
  //

  class Lookup : public NotifierImpl
  {
  public:
    Lookup(Expression *stateName,
           bool stateNameIsGarbage,
           ValueType declaredType,
           ExprVec *paramVec = NULL);

    virtual ~Lookup();

    // Standard Expression API
    virtual bool isAssignable() const;
    virtual const char *exprName() const;
    virtual void printValue(std::ostream &s) const;
    virtual void printSubexpressions(std::ostream &s) const;

    /**
     * @brief Query whether this expression is a source of change events.
     * @return True if the value may change independently of any subexpressions, false otherwise.
     */
    virtual bool isPropagationSource() const;

    //
    // Value access
    //

    virtual ValueType valueType() const;

    // Delegated to the StateCacheEntry in every case
    virtual bool isKnown() const;

    /**
     * @brief Retrieve the value of this Expression.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown or invalid.
     * @note The expression value is not copied if the return value is false.
     */

    // Local macro
#define DECLARE_LOOKUP_GET_VALUE_METHOD(_rtype_) \
    virtual bool getValue(_rtype_ &result) const;

    DECLARE_LOOKUP_GET_VALUE_METHOD(Boolean)
    DECLARE_LOOKUP_GET_VALUE_METHOD(Integer)
    DECLARE_LOOKUP_GET_VALUE_METHOD(Real)
    DECLARE_LOOKUP_GET_VALUE_METHOD(String)

#undef DECLARE_LOOKUP_GET_VALUE_METHOD

    // Uncomment if ever required
    // Falls back to Expression::getValue(_rtype_) methods
    // virtual bool getValue(uint16_t &result) const;

    /**
     * @brief Retrieve a pointer to the (const) value of this Expression.
     * @param ptr Reference to the pointer variable to receive the result.
     * @return True if known, false if unknown or invalid.
     * @note The pointer is not copied if the return value is false.
     */

    // Local macro
#define DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(_rtype_) \
    virtual bool getValuePointer(_rtype_ const *&ptr) const;

    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(String)
    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(Array)
    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(BooleanArray)
    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(IntegerArray)
    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(RealArray)
    DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD(StringArray)

#undef DECLARE_LOOKUP_GET_VALUE_POINTER_METHOD

    /**
     * @brief Get the value of this expression as a Value instance.
     * @return The Value instance.
     */
    virtual Value toValue() const;

    //
    // API to external interface
    //

    /**
     * @brief Notify this Lookup that its value has been updated.
     */
    virtual void valueChanged();

    /**
     * @brief Get this lookup's high and low thresholds.
     * @param high Place to store the high threshold value.
     * @param low Place to store the low threshold value.
     * @return True if this lookup has active thresholds, false otherwise.
     * @note The base class method always returns false.
     */
    virtual bool getThresholds(Integer &high, Integer &low);
    virtual bool getThresholds(Real &high, Real &low);

    // Utility

    /**
     * @brief Get the state for this Lookup, if known.
     * @param result The place to store the State.
     * @return True if fully known, false if not.
     */
    bool getState(State &result) const; 

  protected:

    //
    // NotifierImpl API
    // 

    virtual void handleActivate();
    virtual void handleDeactivate();
    virtual void handleChange();

    virtual void doSubexprs(ExprUnaryOperator const &f);

    // Behavior that needs to be augmented for LookupOnChange
    virtual void invalidateOldState(); // called before updating state to new value

    // Shared behavior needed by LookupOnChange
    bool handleChangeInternal();
    void ensureRegistered();
    void unregister();
    
    // Member variables shared with implementation classes
    State m_cachedState;
    Expression *m_stateName;
    ExprVec *m_paramVec;
    StateCacheEntry* m_entry; // TODO opportunity to use refcounted ptr?
    ValueType m_declaredType;
    bool m_known;
    bool m_stateKnown;
    bool m_stateIsConstant; // allows early caching of state value
    bool m_stateNameIsGarbage;
    bool m_isRegistered;

  private:
    // Unimplemented
    Lookup();
    Lookup(Lookup const &);
    Lookup &operator=(Lookup const &);
  };

  class LookupOnChange : public Lookup
  {
  public:
    LookupOnChange(Expression *stateName,
                   bool stateNameIsGarbage,
                   ValueType declaredType,
                   Expression *tolerance,
                   bool toleranceIsGarbage = false,
                   ExprVec *paramVec = NULL);

    ~LookupOnChange();

    virtual const char *exprName() const;

    virtual void valueChanged();

    virtual bool getThresholds(Integer &high, Integer &low);
    virtual bool getThresholds(Real &high, Real &low);

    /**
     * @brief Retrieve the value of this Expression.
     * @param The appropriately typed place to put the result.
     * @return True if known, false if unknown or invalid.
     * @note The expression value is not copied if the return value is false.
     */

    // Local macro
#define DECLARE_CHANGE_LOOKUP_GET_VALUE_METHOD(_rtype_)  \
    virtual bool getValue(_rtype_ &result) const;

    DECLARE_CHANGE_LOOKUP_GET_VALUE_METHOD(Integer)
    DECLARE_CHANGE_LOOKUP_GET_VALUE_METHOD(Real)

#undef DECLARE_CHANGE_LOOKUP_GET_VALUE_METHOD

    /**
     * @brief Get the value of this expression as a Value instance.
     * @return The Value instance.
     */
    Value toValue() const;

  protected:

    //
    // NotifierImpl API
    //

    virtual void handleActivate();
    virtual void handleDeactivate();
    virtual void handleChange();

    virtual void doSubexprs(ExprUnaryOperator const &f);

  private:
    // Prohibit default constructor, copy, assign
    LookupOnChange();
    LookupOnChange(const LookupOnChange &);
    LookupOnChange &operator=(const LookupOnChange &);

    // Wrapper for base class method
    virtual void invalidateOldState();

    // Internal helper
    bool updateInternal(bool valueChanged);

    // Unique member data
    ThresholdCache *m_thresholds;
    CachedValue *m_cachedValue;
    Expression *m_tolerance;
    bool m_toleranceIsGarbage;
  };

} // namespace PLEXIL

#endif // PLEXIL_LOOKUP_HH
