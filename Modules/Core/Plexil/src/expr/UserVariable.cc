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

//
// Member function definitions and explicit instantiation for UserVariable classes
//

#include "UserVariable.hh"

#include "Constant.hh"
#include "ExpressionConstants.hh"
#include "PlanError.hh"
#include "PlexilTypeTraits.hh"
#include "Value.hh"

#include <cstdlib> // free()
#include <cstring> // strdup()

namespace PLEXIL {

  template <typename T>
  UserVariable<T>::UserVariable()
    : NotifierImpl(),
    m_initializer(NULL),
    m_name(NULL),
    m_node(NULL),
    m_known(false),
    m_savedKnown(false)
  {
  }

  UserVariable<String>::UserVariable()
    : NotifierImpl(),
    m_initializer(NULL),
    m_name(NULL),
    m_node(NULL),
    m_known(false),
    m_savedKnown(false)
  {
  }

  template <typename T>
  UserVariable<T>::UserVariable(T const &initVal)
  : NotifierImpl(),
    m_initializer(new Constant<T>(initVal)),
    m_name(NULL),
    m_node(NULL),
    m_known(false),
    m_savedKnown(false),
    m_initializerIsGarbage(true)
  {
  }

  UserVariable<String>::UserVariable(String const &initVal)
  : NotifierImpl(),
    m_initializer(new Constant<String>(initVal)),
    m_name(NULL),
    m_node(NULL),
    m_known(false),
    m_savedKnown(false),
    m_initializerIsGarbage(true)
  {
  }

  // Only two possible constant initializers for BooleanVariable
  template <>
  UserVariable<Boolean>::UserVariable(const Boolean &initVal)
  : NotifierImpl(),
    m_initializer(initVal ? TRUE_EXP() : FALSE_EXP()),
    m_name(NULL),
    m_node(NULL),
    m_known(false),
    m_savedKnown(false),
    m_initializerIsGarbage(false)
  {
  }

  template <typename T>
  UserVariable<T>::UserVariable(NodeConnector *node,
                                char const *name)
    : NotifierImpl(),
    m_initializer(NULL),
    m_name(strdup(name)),
    m_node(node),
    m_known(false),
    m_savedKnown(false),
    m_initializerIsGarbage(false)
  {
  }

  UserVariable<String>::UserVariable(NodeConnector *node,
                                          char const *name)
    : NotifierImpl(),
    m_initializer(NULL),
    m_name(strdup(name)),
    m_node(node),
    m_known(false),
    m_savedKnown(false),
    m_initializerIsGarbage(false)
  {
  }
    
  /**
   * @brief Destructor.
   * @note Specializations may have more work to do.
   */
  template <typename T>
  UserVariable<T>::~UserVariable()
  {
    free((void *) m_name);
    if (m_initializerIsGarbage)
      delete m_initializer;
  }

  UserVariable<String>::~UserVariable()
  {
    free((void *) m_name);
    if (m_initializerIsGarbage)
      delete m_initializer;
  }

  //
  // Essential Expression API
  //

  template <typename T>
  bool UserVariable<T>::isAssignable() const
  {
    return true;
  }

  bool UserVariable<String>::isAssignable() const
  {
    return true;
  }

  template <typename T>
  Assignable const *UserVariable<T>::asAssignable() const
  {
    return static_cast<Assignable const *>(this);
  }

  Assignable const *UserVariable<String>::asAssignable() const
  {
    return static_cast<Assignable const *>(this);
  }

  template <typename T>
  Assignable *UserVariable<T>::asAssignable()
  {
    return static_cast<Assignable *>(this);
  }

  Assignable *UserVariable<String>::asAssignable()
  {
    return static_cast<Assignable *>(this);
  }

  template <typename T>
  char const *UserVariable<T>::getName() const
  {
    if (m_name)
      return m_name;
    static char const *sl_anon = "anonymous";
    return sl_anon;
  }

  char const *UserVariable<String>::getName() const
  {
    if (m_name)
      return m_name;
    static char const *sl_anon = "anonymous";
    return sl_anon;
  }

  template <typename T>
  const char *UserVariable<T>::exprName() const
  {
    return "Variable";
  }

  const char *UserVariable<String>::exprName() const
  {
    return "Variable";
  }

  template <typename T>
  bool UserVariable<T>::isKnown() const
  {
    return this->isActive() && m_known;
  }

  bool UserVariable<String>::isKnown() const
  {
    return this->isActive() && m_known;
  }

  template <typename T>
  bool UserVariable<T>::getValue(T &result) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      result = m_value;
    return m_known;
  }

  bool UserVariable<String>::getValue(String &result) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      result = m_value;
    return m_known;
  }

  bool UserVariable<String>::getValuePointer(String const *&ptr) const
  {
    if (!this->isActive())
      return false;
    if (m_known)
      ptr = &m_value;
    return m_known;
  }

  template <typename U>
  bool UserVariable<String>::getValuePointer(U const *&ptr) const
  {
    assertTrue_2(ALWAYS_FAIL, "UserVariable::getValuePointer type error");
    return false;
  }

  // A variable takes its initial value when activated.
  // If no initializer, its initial value is unknown.
  template <typename T>
  void UserVariable<T>::handleActivate()
  {
    m_savedKnown = false;

    if (m_initializer) {
      m_initializer->activate();
      m_known = m_initializer->getValue(m_value);
      if (m_known)
        this->publishChange();
    }
    else
      m_known = false;
  }

  void UserVariable<String>::handleActivate()
  {
    m_savedKnown = false;

    if (m_initializer) {
      m_initializer->activate();
      String const *valptr;
      m_known = m_initializer->getValuePointer(valptr);
      m_value = *valptr;
      if (m_known)
        this->publishChange();
    }
    else
      m_known = false;
  }

  template <typename T>
  void UserVariable<T>::handleDeactivate()
  {
    if (m_initializer)
      m_initializer->deactivate();
  }

  void UserVariable<String>::handleDeactivate()
  {
    if (m_initializer)
      m_initializer->deactivate();
  }

  template <typename T>
  void UserVariable<T>::printSpecialized(std::ostream &s) const
  {
    s << getName() << ' ';
  }

  void UserVariable<String>::printSpecialized(std::ostream &s) const
  {
    s << getName() << ' ';
  }

  template <typename T>
  void UserVariable<T>::setValue(Value const &val)
  {
    T temp;
    if (val.getValue(temp))
      setValueImpl(temp);
    else
      this->setUnknown();
  }

  void UserVariable<String>::setValue(Value const &val)
  {
    String temp;
    if (val.getValue(temp))
      setValueImpl(temp);
    else
      this->setUnknown();
  }

  template <typename T>
  void UserVariable<T>::setValue(Expression const &val)
  {
    T temp;
    if (val.getValue(temp))
      setValueImpl(temp);
    else
      this->setUnknown();
  }

  void UserVariable<String>::setValue(Expression const &val)
  {
    String temp;
    if (val.getValue(temp))
      setValueImpl(temp);
    else
      this->setUnknown();
  }

  template <typename T>
  void UserVariable<T>::setValueImpl(T const &value)
  {
    bool changed = !m_known || value != m_value;
    m_value = value;
    m_known = true;
    if (changed)
      this->publishChange();
  }

  void UserVariable<String>::setValueImpl(String const &value)
  {
    bool changed = !m_known || value != m_value;
    m_value = value;
    m_known = true;
    if (changed)
      this->publishChange();
  }

  template <typename T>
  void UserVariable<T>::setUnknown()
  {
    bool changed = m_known;
    m_known = false;
    if (changed)
      this->publishChange();
  }

  void UserVariable<String>::setUnknown()
  {
    bool changed = m_known;
    m_known = false;
    if (changed)
      this->publishChange();
  }

  template <typename T>
  void UserVariable<T>::saveCurrentValue()
  {
    m_savedValue = m_value;
    m_savedKnown = m_known;
  }

  void UserVariable<String>::saveCurrentValue()
  {
    m_savedValue = m_value;
    m_savedKnown = m_known;
  }

  // Should only be called when active.
  template <typename T>
  void UserVariable<T>::restoreSavedValue()
  {
    bool changed = (m_known != m_savedKnown) || (m_value != m_savedValue);
    m_value = m_savedValue;
    m_known = m_savedKnown;
    if (changed)
      this->publishChange();
  }

  void UserVariable<String>::restoreSavedValue()
  {
    bool changed = (m_known != m_savedKnown) || (m_value != m_savedValue);
    m_value = m_savedValue;
    m_known = m_savedKnown;
    if (changed)
      this->publishChange();
  }

  template <typename T>
  Value UserVariable<T>::getSavedValue() const
  {
    return Value(m_savedValue);
  }

  Value UserVariable<String>::getSavedValue() const
  {
    return Value(m_savedValue);
  }

  template <typename T>
  NodeConnector *UserVariable<T>::getNode()
  {
    return m_node;
  }

  NodeConnector *UserVariable<String>::getNode()
  {
    return m_node;
  }

  template <typename T>
  NodeConnector const *UserVariable<T>::getNode() const
  {
    return m_node;
  }

  NodeConnector const *UserVariable<String>::getNode() const
  {
    return m_node;
  }

  template <typename T>
  Expression *UserVariable<T>::getBaseVariable()
  {
    return this;
  }

  Expression *UserVariable<String>::getBaseVariable()
  {
    return this;
  }

  template <typename T>
  Expression const *UserVariable<T>::getBaseVariable() const
  {
    return this;
  }

  Expression const *UserVariable<String>::getBaseVariable() const
  {
    return this;
  }

  template <typename T>
  void UserVariable<T>::setInitializer(Expression *expr, bool garbage)
  {
    checkPlanError(expr->valueType() == PlexilValueType<T>::value
                   || expr->valueType() == UNKNOWN_TYPE,
                   "Variable " << this->getName()
                   << " of type " << valueTypeName(PlexilValueType<T>::value)
                   << " cannot have initializer of type " << valueTypeName(expr->valueType()));
    m_initializer = expr;
    m_initializerIsGarbage = garbage;
  }

  template <>
  void UserVariable<Real>::setInitializer(Expression *expr, bool garbage)
  {
    checkPlanError(expr->valueType() == REAL_TYPE
                   || expr->valueType() == INTEGER_TYPE
                   || expr->valueType() == UNKNOWN_TYPE,
                   "Variable " << this->getName()
                   << " of type Real cannot have initializer of type "
                   << valueTypeName(expr->valueType()));
    m_initializer = expr;
    m_initializerIsGarbage = garbage;
  }

  void UserVariable<String>::setInitializer(Expression *expr, bool garbage)
  {
    checkPlanError(expr->valueType() == STRING_TYPE || expr->valueType() == UNKNOWN_TYPE,
                   "Variable " << this->getName()
                   << " of type String cannot have initializer of type "
                   << valueTypeName(expr->valueType()));
    m_initializer = expr;
    m_initializerIsGarbage = garbage;
  }
  
  //
  // Explicit instantiations
  //

  template class UserVariable<Boolean>;
  template class UserVariable<Integer>;
  template class UserVariable<Real>;

} // namespace PLEXIL
