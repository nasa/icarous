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
#include "ExternalInterface.hh"
#include "Constant.hh"
#include "Lookup.hh"
#include "StateCacheEntry.hh"
#include "StateCacheMap.hh"
#include "TestSupport.hh"
#include "test/TrivialListener.hh"
#include "UserVariable.hh"

#include <map>
#include <set>

using namespace PLEXIL;

class TestInterface : public ExternalInterface 
{
public:
  TestInterface()
    : ExternalInterface()
  {
  }

  ~TestInterface() 
  {
  }

  //
  // ExternalInterface API
  //

  void lookupNow(const State& state, StateCacheEntry &entry) 
  {
    if (state.name() == "test1") {
      entry.update((Real) 2.0);
      return;
    }
    else if (state.name() == "test2") {
      check_error_1(state.parameters().size() == 1);
      std::string const *param = NULL;
      state.parameters()[0].getValuePointer(param);
      if (*param == "high") {
        entry.update((Real) 1.0);
        return;
      }
      else if (*param == "low") {
        entry.update((Real) -1.0);
        return;
      }
      assertTrue_2(ALWAYS_FAIL, "ERROR: no matching param for TestInterface::lookupNow, state name = \"test2\"");
    }
    else if (state.name() == "time") {
      entry.update((Real) 0.0);
      return;
    }
    else {
      entry.update(m_changingExprs[state.name()]->toValue());
      return;
    }
    assertTrue_2(ALWAYS_FAIL, "ERROR: reached end of TestInterface::lookupNow()");
    entry.update((Real) 0.0);
  }

  void subscribe(const State& /* state */)
  {
    // TODO
  }

  void unsubscribe(State const &state)
  {
    m_thresholds.erase(state.name());
  }

  void setThresholds(State const &state, Real hi, Real lo)
  {
    m_thresholds[state.name()] = std::make_pair(hi, lo);
  }

  void setThresholds(const State& /* state */, Integer /* hi */, Integer /* lo */)
  {
    // TODO
  }

  Real currentTime()
  {
    return 0.0;
  }

  //
  // API for unit test
  //
 
  void watch(const char* name, Expression *expr)
  {
    if (m_listeners.find(expr) == m_listeners.end()) {
      ChangeListener * l = newListener(expr);
      expr->addListener(l);
      m_listeners[expr] = l;
    }
    std::string nameStr(name);
    m_changingExprs.insert(std::pair<std::string, Expression *>(nameStr, expr));
    m_exprsToStateName.insert(std::make_pair(expr, nameStr));
  }

  void unwatch(const char* name, Expression *expr)
  {
    if (m_listeners.find(expr) != m_listeners.end()) {
      ChangeListener *l = m_listeners[expr];
      expr->removeListener(l);
      m_listeners.erase(expr);
      delete l;
    }
    std::string nameStr(name);
    m_changingExprs.erase(nameStr);
    m_exprsToStateName.erase(expr);
  }

  bool getThresholds(std::string const &stateName, Real &hi, Real &lo)
  {
    ThresholdMap::const_iterator it = m_thresholds.find(stateName);
    if (it == m_thresholds.end())
      return false;
    hi = it->second.first;
    lo = it->second.second;
    return true;
  }

protected:

  // Not used

  void executeCommand(Command * /* cmd */)
  {}
  
  void reportCommandArbitrationFailure(Command * /* cmd */)
  {}

  void invokeAbort(Command * /* cmd */)
  {}

  void executeUpdate(Update * /* upd */)
  {}

  //
  // API for unit test
  //

  void expressionChanged(Expression const *n)
  {
    Expression const *expression = dynamic_cast<Expression const *>(n);
    assertTrue_1(expression);
    std::multimap<Expression const *, std::string>::const_iterator it = m_exprsToStateName.find(expression);
    while (it != m_exprsToStateName.end() && it->first == expression) {
      State st(it->second);
      StateCacheMap::instance().ensureStateCacheEntry(st)->update(expression->toValue());
      ++it;
    }
  }

private:

  class ChangeListener : public ExpressionListener 
  {
  public:
    ChangeListener(TestInterface *intf, Expression *expr)
    : ExpressionListener(),
      m_intf(intf),
      m_expr(expr)
    {
    }

    ~ChangeListener() {}

    void notifyChanged()
    {
      m_intf->expressionChanged(m_expr);
    }

  private:
    // Not implemented
    ChangeListener(ChangeListener const &);
    ChangeListener &operator=(ChangeListener const &);

    TestInterface *m_intf;
    Expression *m_expr;
  };

  ChangeListener *newListener(Expression *exp)
  {
    return new ChangeListener(this, exp);
  }

  typedef std::map<std::string, std::pair<Real, Real > > ThresholdMap; 

  std::map<Expression const *, ChangeListener *> m_listeners;
  std::map<std::string, Expression *> m_changingExprs; //map of names to expressions being watched
  ThresholdMap m_thresholds;
  std::multimap<Expression const *, std::string> m_exprsToStateName; //make of watched expressions to their state names
  std::multimap<Expression const *, Expression *> m_listeningExprs; //map of changing expressions to listening expressions
  std::map<Expression const *, Real> m_tolerances; //map of dest expressions to tolerances
  std::map<Expression const *, Value> m_cachedValues; //cache of the previously returned values (dest expression, value pairs)
};

static TestInterface *theInterface = NULL;

// TODO:
// - test state parameter changes
// - test integer lookups

static bool testLookupNow() 
{
  StringConstant test1("test1");
  StringConstant test2("test2");

  StringConstant high("high");
  StringConstant low("low");

  StringVariable test4;
  test4.setInitializer(new StringConstant("test1"), true);

  Expression *l1 = new Lookup(&test1, false, UNKNOWN_TYPE);

  ExprVec *t2vec = makeExprVec(1);
  t2vec->setArgument(0, &high, false);
  Expression *l2 = new Lookup(&test2, false, UNKNOWN_TYPE, t2vec);

  ExprVec *t3vec = makeExprVec(1);
  t3vec->setArgument(0, &low, false);
  Expression *l3 = new Lookup(&test2, false, UNKNOWN_TYPE, t3vec);

  Expression *l4 = new Lookup(&test4, false, UNKNOWN_TYPE);

  bool l1changed = false;
  bool l2changed = false;
  bool l3changed = false;
  bool l4changed = false;

  TrivialListener l1listener(l1changed);
  TrivialListener l2listener(l2changed);
  TrivialListener l3listener(l3changed);
  TrivialListener l4listener(l4changed);

  l1->addListener(&l1listener);
  l2->addListener(&l2listener);
  l3->addListener(&l3listener);
  l4->addListener(&l4listener);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  l1->activate();
  assertTrue_1(l1changed);
  l2->activate();
  assertTrue_1(l2changed);
  l3->activate();
  assertTrue_1(l3changed)
  l4->activate();
  assertTrue_1(l4changed);
  assertTrue_1(test4.isActive());

  Real temp;
  assertTrue_1(l1->getValue(temp));
  assertTrue_1(temp == 2.0);
  assertTrue_1(l2->getValue(temp));
  assertTrue_1(temp == 1.0);
  assertTrue_1(l3->getValue(temp));
  assertTrue_1(temp == -1.0);
  assertTrue_1(l4->isKnown());
  assertTrue_1(l4->getValue(temp));
  assertTrue_1(temp == 2.0);

  l4changed = false;

  test4.setValue("time");
  assertTrue_1(l4changed);
  assertTrue_1(l4->isKnown());
  assertTrue_1(l4->getValue(temp));
  assertTrue_1(temp == 0.0);

  l4changed = false;

  test4.setUnknown();
  assertTrue_1(l4changed);
  assertTrue_1(!l4->isKnown());
  assertTrue_1(!l4->getValue(temp));

  l4changed = false;

  test4.setValue("test1");
  assertTrue_1(l4changed);
  assertTrue_1(l4->isKnown());
  assertTrue_1(l4->getValue(temp));
  assertTrue_1(temp == 2.0);

  // Clean up
  l1->deactivate();
  l2->deactivate();
  l3->deactivate();
  l4->deactivate();

  l1->removeListener(&l1listener);
  l2->removeListener(&l2listener);
  l3->removeListener(&l3listener);
  l4->removeListener(&l4listener);

  delete l4;
  delete l3;
  delete l2;
  delete l1;

  return true;
}

// TODO:
// - test integer lookups

static bool testLookupOnChange() 
{
  StringConstant changeTest("changeTest");
  StringVariable changeWithToleranceTest;
  changeWithToleranceTest.setInitializer(new StringConstant("changeWithToleranceTest"), true);
  RealVariable watchVar;
  watchVar.setInitializer(new RealConstant(0.0), true);
  watchVar.activate();
  theInterface->watch("changeTest", &watchVar);
  theInterface->watch("changeWithToleranceTest", &watchVar);

  RealVariable tolerance;
  tolerance.setInitializer(new RealConstant(0.5), true);
  Real temp;

  Lookup l1(&changeTest, false, UNKNOWN_TYPE);
  LookupOnChange l2(&changeWithToleranceTest, false, UNKNOWN_TYPE,
                    &tolerance, false);

  bool changeNotified = false;
  bool changeWithToleranceNotified = false;
  TrivialListener changeListener(changeNotified);
  TrivialListener changeWithToleranceListener(changeWithToleranceNotified);
  l1.addListener(&changeListener);
  l2.addListener(&changeWithToleranceListener);

  assertTrue_1(!l1.isKnown());
  assertTrue_1(!l2.isKnown());

  // Bump the cycle count
  theInterface->incrementCycleCount();

  l1.activate();
  assertTrue_1(l1.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(changeNotified);

  changeWithToleranceTest.activate();
  l2.activate();
  assertTrue_1(tolerance.isActive());
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(changeWithToleranceNotified);

  changeNotified = false;
  changeWithToleranceNotified = false;
  watchVar.setValue(0.1);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l1.getValue(temp));
  assertTrue_1(temp == 0.1);
  assertTrue_1(changeNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(!changeWithToleranceNotified);

  changeNotified = false;
  watchVar.setValue(0.6);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l1.getValue(temp));
  assertTrue_1(temp == 0.6);
  assertTrue_1(changeNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.6);
  assertTrue_1(changeWithToleranceNotified);

  l1.deactivate();
  changeNotified = false;
  changeWithToleranceNotified = false;

  watchVar.setValue(0.7);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(!l1.isKnown());
  assertTrue_1(!changeNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.6);
  assertTrue_1(!changeWithToleranceNotified);

  watchVar.setValue(1.1);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(!l1.isKnown());
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.1);
  assertTrue_1(changeWithToleranceNotified);

  // Test changing tolerance

  l1.activate();
  changeNotified = false;
  changeWithToleranceNotified = false;
  watchVar.setValue(1.4);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l1.isKnown());
  assertTrue_1(l1.getValue(temp));
  assertTrue_1(temp == 1.4);
  assertTrue_1(changeNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.1);
  assertTrue_1(!changeWithToleranceNotified);

  tolerance.setValue(0.25);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.4);
  assertTrue_1(changeWithToleranceNotified);

  // Test making tolerance unknown
  tolerance.setUnknown();
  changeWithToleranceNotified = false;
  watchVar.setValue(1.5);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(changeWithToleranceNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.5); // should update on every change

  // Test making tolerance known again
  tolerance.setValue(0.125);
  changeWithToleranceNotified = false;
  watchVar.setValue(1.6);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(!changeWithToleranceNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.5); // threshold should be back in effect

  watchVar.setValue(1.7);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(changeWithToleranceNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.7); // threshold should be back in effect

  // Test making state name unknown
  changeWithToleranceNotified = false;
  changeWithToleranceTest.setUnknown();

  assertTrue_1(changeWithToleranceNotified);
  assertTrue_1(!l2.getValue(temp));

  // Set state name back
  changeWithToleranceNotified = false;
  changeWithToleranceTest.setValue("changeWithToleranceTest");

  assertTrue_1(changeWithToleranceNotified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.7);

  l1.deactivate();
  l2.deactivate();

  l1.removeListener(&changeListener);
  l2.removeListener(&changeWithToleranceListener);

  theInterface->unwatch("changeTest", &watchVar);
  theInterface->unwatch("changeWithToleranceTest", &watchVar);

  return true;
}

// TODO:
// - test integer lookups

static bool testThresholdUpdate()
{
  StringVariable thresholdTest;
  thresholdTest.setInitializer(new StringConstant("thresholdTest"), true);
  RealVariable watchVar;
  watchVar.setInitializer(new RealConstant(0.0), true);
  watchVar.activate();
  theInterface->watch("thresholdTest", &watchVar);

  RealVariable tolerance2;
  tolerance2.setInitializer(new RealConstant(0.5), true);
  RealVariable tolerance3;
  tolerance3.setInitializer(new RealConstant(0.75), true);
  Real temp, hi, lo;

  LookupOnChange l2(&thresholdTest, false, UNKNOWN_TYPE,
                    &tolerance2, false);
  LookupOnChange l3(&thresholdTest, false, UNKNOWN_TYPE,
                    &tolerance3, false);
  
  bool l2Notified = false;
  bool l3Notified = false;
  TrivialListener l2Listener(l2Notified);
  TrivialListener l3Listener(l3Notified);
  l2.addListener(&l2Listener);
  l3.addListener(&l3Listener);

  assertTrue_1(!l2.isKnown());
  assertTrue_1(!l3.isKnown());

  // Bump the cycle count
  theInterface->incrementCycleCount();

  // Check that thresholds are not yet set
  assertTrue_1(!theInterface->getThresholds("thresholdTest", hi, lo));

  l2.activate();
  assertTrue_1(tolerance2.isActive());
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(l2Notified);
  l3.activate();
  assertTrue_1(tolerance3.isActive());
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(l3Notified);
  // Thresholds should now be set to the tighter tolerance
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi = 0.5);
  assertTrue_1(lo = -0.5);

  l2Notified = false;
  l3Notified = false;
  watchVar.setValue(0.25);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(!l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(!l3Notified);
  // Thresholds should not have changed
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi = 0.5);
  assertTrue_1(lo = -0.5);

  watchVar.setValue(0.5);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.5);
  assertTrue_1(l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 0.0);
  assertTrue_1(!l3Notified);
  // Low threshold should have updated with l2; high should now follow l3
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 0.75);
  assertTrue_1(lo == 0.0);

  l2Notified = false;

  watchVar.setValue(0.75);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 0.5);
  assertTrue_1(!l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 0.75);
  assertTrue_1(l3Notified);
  // Low threshold should be unchanged (l2 & l3 same); high should now follow l2
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 1.0);
  assertTrue_1(lo == 0);

  l3Notified = false;

  watchVar.setValue(1.25);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.25);
  assertTrue_1(l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 0.75);
  assertTrue_1(!l3Notified);
  // High follows l3, low l2
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 1.50);
  assertTrue_1(lo == 0.75);

  // Test changing tolerance

  l2Notified = false;
  watchVar.setValue(1.5);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.25);
  assertTrue_1(!l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 1.5);
  assertTrue_1(l3Notified);
  // High should follow l2, low is both
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 1.75);
  assertTrue_1(lo == 0.75);

  l3Notified = false;

  tolerance2.setValue(0.25);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.5);
  assertTrue_1(l2Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 1.5);
  assertTrue_1(!l3Notified);
  // Low and high should follow l2
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 1.75);
  assertTrue_1(lo == 1.25);

  // Test making tolerances unknown
  tolerance2.setUnknown();
  l2Notified = false;

  // Bump the cycle count
  theInterface->incrementCycleCount();

  // Low and high should now track l3
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 2.25);
  assertTrue_1(lo == 0.75);

  // Test deactivation
  l3.deactivate();

  // Bump the cycle count
  theInterface->incrementCycleCount();

  // Thresholds should no longer be in effect
  // Unfortunately there's no API to tell the interface that!
  // assertTrue_1(!theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(!l3.getValue(temp));
  assertTrue_1(!l3Notified);


  // Test deactivation
  l2.deactivate();

  // NOW thresholds should no longer be in effect
  assertTrue_1(!theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(!l2.getValue(temp));
  assertTrue_1(!l2Notified);

  // Test reactivation
  l3.activate();

  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 1.5);
  assertTrue_1(l3Notified);
  // Thresholds should track l3 again
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 2.25);
  assertTrue_1(lo == 0.75);

  l2.activate(); // restores initial value 0.5

  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.5);
  assertTrue_1(l2Notified);
  // Thresholds should now track l2
  assertTrue_1(theInterface->getThresholds("thresholdTest", hi, lo));
  assertTrue_1(hi == 2.0);
  assertTrue_1(lo == 1.0);

  // Test making tolerance known again
  tolerance2.setValue(0.25);
  watchVar.setValue(1.625);
  l2Notified = false;
  l3Notified = false;

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(!l2Notified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.5); // threshold should be back in effect
  assertTrue_1(!l3Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 1.5);

  watchVar.setValue(1.75);

  // Bump the cycle count
  theInterface->incrementCycleCount();

  assertTrue_1(l2Notified);
  assertTrue_1(l2.getValue(temp));
  assertTrue_1(temp == 1.75); // threshold should be back in effect
  assertTrue_1(!l3Notified);
  assertTrue_1(l3.getValue(temp));
  assertTrue_1(temp == 1.5);

  l2.deactivate();
  l3.deactivate();

  l2.removeListener(&l2Listener);
  l3.removeListener(&l3Listener);

  theInterface->unwatch("thresholdTest", &watchVar);

  return true;
}

bool lookupsTest()
{
  TestInterface foo;
  theInterface = &foo;
  g_interface = &foo;

  runTest(testLookupNow);
  runTest(testLookupOnChange);
  runTest(testThresholdUpdate);
  g_interface = NULL;
  return true;
}
