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

#include "StateCacheMap.hh"

#include "State.hh"
#include "StateCacheEntry.hh"

#include <map>

namespace PLEXIL
{
  class StateCacheMapImpl : public StateCacheMap
  {
  private:
    typedef std::map<State, StateCacheEntry> EntryMap;
    EntryMap m_map;

  public:
    StateCacheMapImpl()
      : StateCacheMap()
    {
      // Initialize time state to 0
      ensureStateCacheEntry(State::timeState())->update((double) 0);
    }

    virtual ~StateCacheMapImpl()
    {
    }

    virtual StateCacheEntry *ensureStateCacheEntry(State const &state)
    {
      EntryMap::iterator it = m_map.find(state);
      if (it == m_map.end())
	it = m_map.insert(std::make_pair(state, StateCacheEntry())).first;
      return &(it->second);
    }

    virtual StateCacheEntry *findStateCacheEntry(State const &state)
    {
      EntryMap::iterator it = m_map.find(state);
      if (it == m_map.end())
	return NULL;
      else
	return &(it->second);
    }

    virtual void removeStateCacheEntry(State const &state)
    {
      EntryMap::iterator it = m_map.find(state);
      if (it == m_map.end())
	return;
      m_map.erase(it);
    }
  };

  StateCacheMap &StateCacheMap::instance()
  {
    static StateCacheMapImpl sl_instance;
    return static_cast<StateCacheMap &>(sl_instance);
  }

} // namespace PLEXIL
