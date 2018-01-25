/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

class CacheTestInterface : public ExternalInterface {
public:
  CacheTestInterface(const StateCacheId& cache)
  : ExternalInterface(), m_cache(cache), m_lookupNowCalled(false)
  {}

  Value lookupNow(const State& state) 
  {
    m_lookupNowCalled = true;
    return m_values[state];
  }

  void subscribe(const State& /* state */)
  {
  }

  void unsubscribe(const State& /* state */)
  {
  }

  void setThresholds(const State& /* state */, double /* hi */, double /* lo */)
  {
  }

  void setThresholds(const State& /* state */, int32_t /* hi */, int32_t /* lo */)
  {
  }

  double currentTime()
  {
    return 0.0;
  }

  bool lookupNowCalled() {return m_lookupNowCalled;}
  void clearLookupNowCalled() {m_lookupNowCalled = false;}
  void setValue(const State& state, double value, StateCacheId cache, bool update = true) {
    std::map<State, Value>::iterator it = m_values.find(state);
    if (it == m_values.end())
      m_values.insert(std::make_pair(state, Value(value)));
    else
      it->second = Value(value);
    if (update)
      cache->updateState(state, Value(value));
  }
protected:

  void executeCommand(Command * /* cmd */)
  {
  }

  void invokeAbort(Command * /* cmd */)
  {
  }

  void executeUpdate(Update * /* update */)
  {
  }

private:
  std::map<State, Value> m_values;
  StateCacheId m_cache;
  bool m_lookupNowCalled;
};

class StateCacheTest {
public:
  static bool test() {
    runTest(testLookupNow);
    runTest(testChangeLookup);
    return true;
  }
private:
  static bool testLookupNow() {
    StateCache cache;
    CacheTestInterface iface(cache.getId());
    cache.setExternalInterface(iface.getId());

    IntegerVariable destVar;
    destVar.activate();

    State st("foo", std::vector<Value>());

    iface.setValue(st, 1, cache.getId(), false);
    cache.handleQuiescenceStarted();

    //single lookup for new state
    assertTrue_1(destVar.getValue().isUnknown());
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue_1(iface.lookupNowCalled());
    assertTrue_1(destVar.getValue().getDoubleValue() == 1);
    cache.unregisterLookupNow(destVar.getId());

    //re-lookup for same state in same quiescence
    iface.setValue(st, 2, cache.getId(), false);
    iface.clearLookupNowCalled();
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue_1(!iface.lookupNowCalled());
    assertTrue_1(destVar.getValue().getDoubleValue() == 1);
    cache.unregisterLookupNow(destVar.getId());

    //re-lookup for same state in next quiescence
    cache.handleQuiescenceEnded();
    cache.handleQuiescenceStarted();
    cache.registerLookupNow(destVar.getId(), st);
    assertTrue_1(iface.lookupNowCalled());
    assertTrue_1(destVar.getValue().getDoubleValue() == 2);
    cache.unregisterLookupNow(destVar.getId());

    // *** TODO: Add test for updating LookupNow that 
    // *** remains active across multiple quiescence cycles

    return true;
  }

  static bool testChangeLookup() {
    StateCache cache;
    CacheTestInterface iface(cache.getId());
    cache.setExternalInterface(iface.getId());

    IntegerVariable destVar1, destVar2;
    destVar1.activate();
    destVar2.activate();

    State st("foo", std::vector<Value>());

    //lookup
    iface.setValue(st, 1, cache.getId(), false);
    cache.handleQuiescenceStarted();
    cache.registerChangeLookup(destVar1.getId(), st, 1);
    assertTrue_1(destVar1.getValue().getDoubleValue() == 1);
    cache.registerChangeLookup(destVar2.getId(), st, 2);
    assertTrue_1(destVar2.getValue().getDoubleValue() == 1);
    cache.handleQuiescenceEnded();

    //update value
    iface.setValue(st, 2, cache.getId());
    assertTrue_1(destVar1.getValue().getDoubleValue() == 2);
    assertTrue_1(destVar2.getValue().getDoubleValue() == 1);

    //lookupNow triggering change
    IntegerVariable nowDestVar;
    nowDestVar.activate();
    iface.setValue(st, 3, cache.getId(), false);
    cache.handleQuiescenceStarted();
    cache.handleQuiescenceEnded();
    cache.handleQuiescenceStarted();
    cache.registerLookupNow(nowDestVar.getId(), st);
    assertTrue_1(nowDestVar.getValue().getDoubleValue() == 3);
    assertTrue_1(destVar1.getValue().getDoubleValue() == 3);
    assertTrue_1(destVar2.getValue().getDoubleValue() == 3);

    //unregister
    cache.unregisterLookupNow(nowDestVar.getId());
    cache.unregisterChangeLookup(destVar2.getId());
    cache.handleQuiescenceEnded();
    iface.setValue(st, 5, cache.getId());
    assertTrue_1(destVar2.getValue().getDoubleValue() == 3);
    assertTrue_1(destVar1.getValue().getDoubleValue() == 5);
    return true;
  }
};
