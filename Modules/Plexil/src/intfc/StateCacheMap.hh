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

#ifndef PLEXIL_STATE_CACHE_MAP_HH
#define PLEXIL_STATE_CACHE_MAP_HH

namespace PLEXIL
{
  // Forward references
  class State;
  class StateCacheEntry;

  /**
   * @class StateCacheMap
   * @brief An index to the currently active StateCacheEntry instances
   */
  class StateCacheMap
  {
  public:
    StateCacheMap()
    {
    };
    
    virtual ~StateCacheMap()
    {
    }

    static StateCacheMap &instance();

    /**
     * @brief Construct or find the cache entry for this state.
     * @param state The state being looked up.
     * @return Pointer to the StateCacheEntry for the state.
     * @note Return value can be presumed to be non-null.
     */
    // FIXME: what if existing entry has different type?
    virtual StateCacheEntry *ensureStateCacheEntry(State const &state) = 0;

    /**
     * @brief Find the cache entry for this state.
     * @param state The state being looked up.
     * @return Pointer to the StateCacheEntry for the state; NULL if not found.
     */
    virtual StateCacheEntry *findStateCacheEntry(State const &state) = 0;

    /**
     * @brief Remove the cache entry for this state.
     * @param state The state being looked up.
     */
    virtual void removeStateCacheEntry(State const &state) = 0;
  };

} // namespace PLEXIL

#endif // PLEXIL_STATE_CACHE_MAP_HH
