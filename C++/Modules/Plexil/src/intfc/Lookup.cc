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

#include "Lookup.hh"

#include "ArrayImpl.hh"
#include "CachedValue.hh"
#include "Debug.hh"
#include "ExprVec.hh"
#include "ExternalInterface.hh" // for timestamp access
#include "PlanError.hh"
#include "StateCacheEntry.hh"
#include "StateCacheMap.hh"

#include <cmath> // for abs()

namespace PLEXIL
{
  Lookup::Lookup(Expression *stateName,
                 bool stateNameIsGarbage,
                 ValueType declaredType,
                 ExprVec *paramVec)
    : m_stateName(stateName),
      m_paramVec(paramVec),
      m_entry(NULL),
      m_declaredType(declaredType),
      m_known(false),
      m_stateKnown(false),
      m_stateIsConstant(true),
      m_stateNameIsGarbage(stateNameIsGarbage),
      m_isRegistered(false)
  {
    if (!m_stateName->isConstant())
      m_stateIsConstant = false;

    if (m_paramVec) {
      bool parmsAreConstant = true;
      for (size_t i = 0; i < m_paramVec->size(); ++i)
        if (!(*m_paramVec)[i]->isConstant())
          parmsAreConstant = false;
      if (!parmsAreConstant)
        m_stateIsConstant = false;
    }
    
    // If all expressions are constants, cache state now
    if (m_stateIsConstant) {
      checkPlanError(getState(m_cachedState),
                     "Error in Lookup: State is constant but state name or some parameter is unknown");
      m_stateKnown = true;
    }
  }

  Lookup::~Lookup()
  {
    if (m_entry) {
      unregister();
      m_entry = NULL;
    }
    delete m_paramVec;
    if (m_stateNameIsGarbage)
      delete m_stateName;
  }

  bool Lookup::isAssignable() const
  {
    return false;
  }

  char const *Lookup::exprName() const
  {
    return "LookupNow";
  }

  // punt
  void Lookup::printValue(std::ostream &s) const
  {
    s << this->toValue();
  }

  void Lookup::printSubexpressions(std::ostream &s) const
  {
    s << " name " << *m_stateName;
    if (m_paramVec) {
      s << " params";
      for (size_t i = 0; i < m_paramVec->size(); ++i)
        s << ' ' << *(*m_paramVec)[i];
    }
    s << ' ';
  }

  void Lookup::handleActivate()
  {
    debugMsg("Lookup:handleActivate", " called");
    // Activate all subexpressions
    m_stateName->activate();
    if (m_paramVec)
      m_paramVec->activate();

    // Compute current state and cache it
    if (!m_stateIsConstant)
      m_stateKnown = getState(m_cachedState);
    if (!m_entry && m_stateKnown) {
      m_entry =
        StateCacheMap::instance().ensureStateCacheEntry(m_cachedState);
      assertTrue_2(m_entry != NULL, "Lookup::handleActivate: Failed to get state cache entry");
    }
    if (m_entry)
      ensureRegistered();
  }

  void Lookup::handleDeactivate()
  {
    // Dectivate all subexpressions
    m_stateName->deactivate();
    if (m_paramVec)
      m_paramVec->deactivate();

    if (m_stateKnown)
      unregister();

    // Preserve cache entry if state is known constant
    if (!m_stateIsConstant)
      m_entry = NULL;
  }

  // Called whenever state name or parameter changes
  void Lookup::handleChange()
  {
    debugMsg("Lookup:handleChange", ' ' << *this);
    if (handleChangeInternal())
      publishChange();
  }

  // Return true if state changed, false otherwise
  bool Lookup::handleChangeInternal()
  {
    State newState;
    bool oldKnown = m_stateKnown;
    m_stateKnown = getState(newState);
    bool stateChanged = (oldKnown != m_stateKnown);
    if (!m_stateKnown) {
      if (oldKnown)
        // state used to be known, isn't any longer
        this->invalidateOldState();
    }
    else { // state known
      if (oldKnown && newState != m_cachedState) {
        this->invalidateOldState();
        stateChanged = true;
      }
      m_cachedState = newState;
      m_entry =
        StateCacheMap::instance().ensureStateCacheEntry(m_cachedState);
      assertTrue_2(m_entry != NULL, "Lookup::handleChange: Failed to get state cache entry");
      ensureRegistered();
    }
    return stateChanged;
  }

  void Lookup::ensureRegistered()
  {
    if (m_isRegistered)
      return;
    debugMsg("Lookup:register", ' ' << m_cachedState);
    m_entry->registerLookup(m_cachedState, this);
    m_isRegistered = true;
  }

  void Lookup::unregister()
  {
    if (!m_isRegistered)
      return;
    debugMsg("Lookup:unregister", ' ' << m_cachedState);
    m_entry->unregisterLookup(m_cachedState, this);
    m_isRegistered = false;
  }

  void Lookup::invalidateOldState()
  {
    unregister();
    m_entry = NULL;
  }

  ValueType Lookup::valueType() const
  {
    if (m_entry && m_entry->valueType() != UNKNOWN_TYPE)
      return m_entry->valueType();
    else
      return m_declaredType;
  }

  bool Lookup::getState(State &result) const
  {
    std::string name;
    if (!m_stateName->getValue(name))
      return false;
    result.setName(name);
    if (m_paramVec) {
      size_t n = m_paramVec->size();
      result.setParameterCount(n);
      for (size_t i = 0; i < n; ++i) {
        Value temp = (*m_paramVec)[i]->toValue();
        if (!temp.isKnown())
          return false;
        result.setParameter(i, temp);
      }
    }
    return true;
  }

  bool Lookup::isKnown() const
  {
    if (!this->isActive() || !m_entry)
      return false;
    else
      return m_entry->isKnown();
  }


#define DEFINE_LOOKUP_GET_VALUE_METHOD(_rtype_) \
  bool Lookup::getValue(_rtype_ &result) const \
  { \
    if (!isActive() || !m_entry || !m_entry->cachedValue()) \
      return false; \
    else \
      return m_entry->cachedValue()->getValue(result); \
  }

    DEFINE_LOOKUP_GET_VALUE_METHOD(Boolean)
    DEFINE_LOOKUP_GET_VALUE_METHOD(Integer)
    DEFINE_LOOKUP_GET_VALUE_METHOD(Real)
    DEFINE_LOOKUP_GET_VALUE_METHOD(String)

    // Uncomment if this is ever required
    // Falls back to Expression::getValue(_rtype_) methods
    // DEFINE_LOOKUP_GET_VALUE_METHOD(uint16_t)

#undef DEFINE_LOOKUP_GET_VALUE_METHOD

#define DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(_rtype_) \
  bool Lookup::getValuePointer(_rtype_ const *&ptr) const \
  { \
    if (!isActive() || !m_entry || !m_entry->cachedValue()) \
      return false; \
    else \
      return m_entry->cachedValue()->getValuePointer(ptr); \
  }

  // Explicit instantiations

  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(String)
  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(Array)
  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(BooleanArray)
  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(IntegerArray)
  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(RealArray)
  DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD(StringArray)

#undef DEFINE_LOOKUP_GET_VALUE_POINTER_METHOD

  /**
   * @brief Get the value of this expression as a Value instance.
   * @return The Value instance.
   */
  Value Lookup::toValue() const
  {
    if (!this->isActive() || !m_entry || !m_entry->cachedValue())
      return Value();
    else
      return m_entry->cachedValue()->toValue();
  }

  // Callback from external interface
  void Lookup::valueChanged()
  {
    publishChange();
  }

  bool Lookup::getThresholds(Integer &high, Integer &low)
  {
    return false;
  }

  bool Lookup::getThresholds(Real &high, Real &low)
  {
    return false;
  }

  //
  // LookupOnChange
  //

  // Internal class used only by LookupOnChange
  // Presumes both lookup value and tolerance value are known.
  class ThresholdCache
  {
  public:
    ThresholdCache() {}
    virtual ~ThresholdCache() {}

    /**
     * @brief Check whether the threshold value itself has changed.
     * @param tolerance Tolerance expression.
     * @return True if changed.
     */
    virtual bool toleranceChanged(Expression const *tolerance) const = 0;

    /**
     * @brief Check whether the current value is beyond the thresholds.
     * @param entry Pointer to the value.
     * @return True if exceeded, false otherwise.
     */
    virtual bool thresholdsExceeded(CachedValue const *value) const = 0;

    /**
     * @brief Set the thresholds based on the given cached value.
     * @param entry Pointer to state cache entry.
     */
    virtual void setThresholds(CachedValue const *value, Expression const *tolerance) = 0;


    /**
     * @brief Get the current thresholds.
     * @param high Place to store the current high threshold.
     * @param low Place to store the current low threshold.
     * @note Default methods.
     */

    virtual void getThresholds(Integer &high, Integer &low)
    {
      assertTrueMsg(ALWAYS_FAIL,
                    "LookupOnChange:getTolerance: attempt to get Integer thresholds from non-Integer");
    }

    virtual void getThresholds(Real &high, Real &low)
    {
      assertTrueMsg(ALWAYS_FAIL,
                    "LookupOnChange:getTolerance: attempt to get Real thresholds from non-Real");
    }

  };

  template <typename NUM>
  class ThresholdCacheImpl : public ThresholdCache
  {
  public:
    ThresholdCacheImpl()
      : ThresholdCache(),
      m_wasKnown(false)
    {
    }

    ~ThresholdCacheImpl() {}

    virtual bool toleranceChanged(Expression const *tolerance) const
    {
      check_error_1(tolerance); // paranoid check
      NUM newTol;
      if (tolerance->getValue(newTol)) {
        if (newTol < 0)
          newTol = -newTol;
        return newTol != m_tolerance;
      }
      // Tolerance is unknown, default to 0
      newTol = 0;
      return true;
    }

    // See below for implementations of this method.
    virtual bool thresholdsExceeded(CachedValue const *value) const;

    virtual void setThresholds(CachedValue const *value, Expression const *tolerance)
    {
      debugMsg("LookupOnChange:setThresholds", " entered");
      check_error_1(value); // paranoid check
      NUM base, tol;
      if (tolerance->getValue(tol)) {
        if (tol < 0)
          tol = -tol;
      }
      else {
        tol = 0;
      }
      if (value->getValue(base)) {
        m_tolerance = tol;
        m_low = base - tol;
        m_high = base + tol;
        m_wasKnown = true;
      }
      else {
        m_wasKnown = false;
      }
    }

    virtual void getThresholds(NUM &high, NUM &low)
    {
      high = m_high;
      low = m_low;
    }

  private:
    NUM m_low;
    NUM m_high;
    NUM m_tolerance;
    bool m_wasKnown;
  };

  // Threshold check for Integer-valued lookups
  template <>
  bool ThresholdCacheImpl<Integer>::thresholdsExceeded(CachedValue const *value) const
  {
    Integer currentValue;
    if (value->getValue(currentValue))
      return (currentValue >= m_high) || (currentValue <= m_low);
    // Current value is unknown
    return m_wasKnown; 
  }

  // Threshold check for Real-valued lookups
  // Covers up a horde of sins, notably timers returning early (!)
  template <>
  bool ThresholdCacheImpl<Real>::thresholdsExceeded(CachedValue const *value) const
  {
    check_error_1(value); // paranoid check
    Real currentValue;
    if (!(value->getValue(currentValue)))
      // now unknown, was it last time?
      return m_wasKnown;
         
    // Is known from here down
    if (!m_wasKnown)
      return true; // was unknown

    if ((currentValue >= m_high) || (currentValue <= m_low))
      return true;

    // Put guard bands around thresholds
    Real epsilon = fabs(currentValue) * 1e-13; // on the order of 150 usec for time
    if (m_high - currentValue < epsilon)
      return true;
    if (currentValue - m_low < epsilon)
      return true;
    return false;
  }

  static ThresholdCache * ThresholdCacheFactory(ValueType typ)
  {
    switch (typ) {
    case INTEGER_TYPE:
      return new ThresholdCacheImpl<Integer>();

    case UNKNOWN_TYPE:
      warn("ThresholdCacheFactory: type unknown, defaulting to REAL");
      // drop thru

      // FIXME: Implement for non-Real date, duration types
    case DATE_TYPE:
    case DURATION_TYPE:

    case REAL_TYPE:
      return new ThresholdCacheImpl<Real>();

    default:
      assertTrue_2(ALWAYS_FAIL, "ThresholdCacheFactory: invalid or unimplemented type");
      return NULL;
    }
  }

  LookupOnChange::LookupOnChange(Expression *stateName,
                                 bool stateNameIsGarbage,
                                 ValueType declaredType,
                                 Expression *tolerance,
                                 bool toleranceIsGarbage,
                                 ExprVec *paramVec)
    : Lookup(stateName, stateNameIsGarbage, declaredType, paramVec),
      m_thresholds(NULL),
      m_cachedValue(NULL),
      m_tolerance(tolerance),
      m_toleranceIsGarbage(toleranceIsGarbage)
  {
  }

  LookupOnChange::~LookupOnChange()
  {
    delete m_thresholds;
    delete m_cachedValue;
    if (m_toleranceIsGarbage)
      delete m_tolerance;
  }

  char const *LookupOnChange::exprName() const
  {
    return "LookupOnChange";
  }

  /**
   * @brief Query whether this expression is a source of change events.
   * @return True if the value may change independently of any subexpressions, false otherwise.
   */
  bool Lookup::isPropagationSource() const
  {
    return true; // value changes independently of parameters
  }

  void Lookup::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(m_stateName);
    if (m_paramVec)
      m_paramVec->doSubexprs(f);
  }

  void LookupOnChange::handleActivate()
  {
    check_error_1(m_tolerance); // paranoid check
    debugMsg("LookupOnChange:handleActivate", " called");
    Lookup::handleActivate(); // may register lookup if state known,
                              // may cause calls to handleChange(), valueChanged()
    m_tolerance->activate();  // may cause calls to handleChange()
    updateInternal(true);     // may cause redundant notifications
    if (this->isKnown())
      publishChange();
  }

  // TODO: Optimization opportunity if state is known to be constant
  void LookupOnChange::handleDeactivate()
  {
    debugMsg("LookupOnChange:handleDeactivate", " called");
    Lookup::handleDeactivate();
    m_tolerance->deactivate();
    if (m_thresholds) {
      delete m_thresholds;
      m_thresholds = NULL;
      delete m_cachedValue;
      m_cachedValue = NULL;
    }
  }

  // Consider possibility that tolerance has changed.
  // Consider possibility lookup may not be fully activated yet.
  void LookupOnChange::handleChange()
  {
    if (updateInternal(Lookup::handleChangeInternal()))
      publishChange();
  }

  void LookupOnChange::doSubexprs(ExprUnaryOperator const &f)
  {
    (f)(m_tolerance);
    Lookup::doSubexprs(f);
  }

  void LookupOnChange::invalidateOldState()
  {
    Lookup::invalidateOldState();
    if (m_thresholds) {
      delete m_thresholds;
      m_thresholds = NULL;
      delete m_cachedValue;
      m_cachedValue = NULL;
    }
  }

  // May be called before lookup fully activated
  void LookupOnChange::valueChanged()
  {
    if (!this->isActive()) {
      debugMsg("LookupOnChange:valueChanged", " for " << m_cachedState << " not active, ignoring");
      return;
    }
    if (updateInternal(true)) {
      debugMsg("LookupOnChange:valueChanged", " for " << m_cachedState << ": notifying listeners");
      publishChange();
    }
    else {
      debugMsg("LookupOnChange:valueChanged", " for " << m_cachedState << ": no change");
    }
  }

  bool LookupOnChange::getThresholds(Integer &high, Integer &low)
  {
    if (!m_thresholds)
      return false;
    assertTrueMsg(valueType() == INTEGER_TYPE,
                  "getThresholds: calling Integer method on non-Integer lookup");
    m_thresholds->getThresholds(high, low);
    return true;
  }

  bool LookupOnChange::getThresholds(Real &high, Real &low)
  {
    if (!m_thresholds)
      return false;
    assertTrueMsg(valueType() == REAL_TYPE,
                  "getThresholds: calling Real method on non-Real lookup");
    m_thresholds->getThresholds(high, low);
    return true;
  }

  // Called at: (1) activation (2) subexpression change (3) value change
  // Returns true if event should trigger notification, false otherwise.
  // Updated state and cache entry should be valid if state is known.
  bool LookupOnChange::updateInternal(bool valueChanged)
  {
    debugMsg("LookupOnChange:update", ' ' << this->m_cachedState << ", valueChanged = " << valueChanged);

    if (m_thresholds) {
      // Thresholds would have been deleted already if state had changed.
      // Therefore state is known, and cache entry valid

      if (!m_tolerance->isKnown()) {
        debugMsg("LookupOnChange:update",
                 ' ' << this->m_cachedState << " tolerance no longer known, deleting thresholds");
        delete m_thresholds;
        m_thresholds = NULL;
        delete m_cachedValue;
        m_cachedValue = NULL;
        // Tell the cache entry about it
        m_entry->updateThresholds(m_cachedState);
      }
      else {
        // Check whether the thresholds have changed
        if (m_thresholds->toleranceChanged(m_tolerance))
          m_thresholds->setThresholds(m_cachedValue, m_tolerance);

        // Has the (possibly updated) threshold been exceeded?
        CachedValue const *val = this->m_entry->cachedValue();
        if (m_thresholds->thresholdsExceeded(val)) {
          // TODO? Check that value hasn't changed type
          *m_cachedValue = *val;
          debugMsg("LookupOnChange:update",
                   ' ' << this->m_cachedState
                   << " threshold exceeded, propagating value and updating thresholds");
          m_thresholds->setThresholds(val, m_tolerance);
          m_entry->updateThresholds(m_cachedState);
          return true;
        }
        else {
          debugMsg("LookupOnChange:update",
                   ' ' << this->m_cachedState << " value changed but within tolerances");
          return false; // value or threshold updated, but value within tolerances
        }
      }
    }
    else if (this->m_entry && this->m_entry->isKnown() && m_tolerance->isKnown()) {
      // No thresholds yet, so cache current value and establish them
      CachedValue const *val = this->m_entry->cachedValue();
      debugMsg("LookupOnChange:update",
               ' ' << this->m_cachedState << " constructing initial threshold");
      m_thresholds = ThresholdCacheFactory(this->m_entry->valueType());
      m_cachedValue = val->clone();
      m_thresholds->setThresholds(val, m_tolerance);
      m_entry->updateThresholds(m_cachedState);
    }

    return valueChanged;
  }

  // Use local cache if we have a tolerance, as it may differ from state cache value

#define DEFINE_CHANGE_LOOKUP_GET_VALUE_METHOD(_rtype_)  \
  bool LookupOnChange::getValue(_rtype_ &result) const \
  { \
    if (!this->isActive() || !m_entry || !m_entry->cachedValue()) \
      return false; \
    else if (m_cachedValue) \
      return m_cachedValue->getValue(result); \
    else if (m_entry->isKnown()) \
      return m_entry->cachedValue()->getValue(result); \
    else \
      return false; \
  }

  // Explicit instantiations
  DEFINE_CHANGE_LOOKUP_GET_VALUE_METHOD(Integer)
  DEFINE_CHANGE_LOOKUP_GET_VALUE_METHOD(Real)

#undef DEFINE_CHANGE_LOOKUP_GET_VALUE_METHOD

  /**
   * @brief Get the value of this expression as a Value instance.
   * @return The Value instance.
   */
  Value LookupOnChange::toValue() const
  {
    if (!this->isActive() || !m_entry || !m_entry->cachedValue())
      return Value();
    // Use local cache if we have a tolerance, as it may differ from state cache value
    else if (m_cachedValue)
      return m_cachedValue->toValue();
    else if (m_entry->isKnown())
      return m_entry->cachedValue()->toValue();
    else
      return Value();
  }

} // namespace PLEXIL
