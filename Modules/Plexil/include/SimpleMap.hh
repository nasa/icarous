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

#ifndef PLEXIL_SIMPLE_MAP_HH
#define PLEXIL_SIMPLE_MAP_HH

#include <algorithm>
#include <vector>
#include <utility>

namespace PLEXIL
{
  /**
   * @class SimpleKeyComparator
   * @brief A templatized comparator class for sorting and inserting in SimpleMap.
   * Any class used as a key comparator must implement this API.
   */
  template <typename KEY_TYPE>
  struct SimpleKeyComparator
  {
    //* Compare key a less than key b
    bool operator()(KEY_TYPE const &a, KEY_TYPE const &b) const
    {
      return a < b;
    }

    //* Compare key a equal to key b
    bool equal(KEY_TYPE const &a, KEY_TYPE const &b) const
    {
      return a == b;
    }
  };

  /**
   * @class SimpleIndexComparator
   * @brief A templatized comparator class for lookups in SimpleMap.
   * Any class used as an index comparator must implement this API.
   */
  template <typename KEY_TYPE, typename INDEX_TYPE>
  struct SimpleIndexComparator
  {
    //* Compare entry a less than index
    bool operator()(KEY_TYPE const &a, INDEX_TYPE const &b) const
    {
      return a < b;
    }

    //* Compare entry a equal to index
    bool equal(KEY_TYPE const &a, INDEX_TYPE const &b) const
    {
      return a == b;
    }

    //* Compare entry a "equal" to index (e.g. b begins with prefix a)
    bool match(KEY_TYPE const &a, INDEX_TYPE const &b) const
    {
      return a == b;
    }
  };

  /**
   * @class SimpleMap
   * @brief A key-value mapping sorted by key value.
   */
  template <typename KEY_TYPE,
            typename VALUE_TYPE,
            class KEY_COMP = SimpleKeyComparator<KEY_TYPE> >
  class SimpleMap
  {
  public:
    typedef std::pair<KEY_TYPE, VALUE_TYPE> MAP_ENTRY_TYPE;
    typedef std::vector<MAP_ENTRY_TYPE> MAP_STORE_TYPE;

    typedef typename MAP_STORE_TYPE::const_iterator const_iterator;
    typedef typename MAP_STORE_TYPE::iterator iterator;

    SimpleMap()
    {
    }

    SimpleMap(size_t initialCapacity)
    {
      m_store.reserve(initialCapacity);
    }

    // Virtual to allow for derived classes.
    virtual ~SimpleMap()
    {
    }

    /*
     * @brief Reserve space for n additional entries.
     * @param n The number of additional entries.
     */
    void grow(size_t n)
    {
      size_t desired = m_store.size() + n;
      m_store.reserve(desired);
    }

    bool insert(KEY_TYPE const &index, VALUE_TYPE const &val)
    {
      static EntryComparator s_comp;
      typename MAP_STORE_TYPE::iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it != m_store.end() && s_comp.equal(*it, index))
        return false; // duplicate
      this->insertEntry(it, index, val);
      return true;
    }

    const_iterator find(KEY_TYPE const &index) const
    {
      static EntryComparator s_comp;
      typename MAP_STORE_TYPE::const_iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it == m_store.end() || s_comp.equal(*it, index))
        return it;
      else
        return m_store.end();
    }

    iterator find(KEY_TYPE const &index)
    {
      static EntryComparator s_comp;
      typename MAP_STORE_TYPE::iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it == m_store.end() || s_comp.equal(*it, index))
        return it;
      else
        return m_store.end();
    }

    VALUE_TYPE &operator[](KEY_TYPE const &index)
    {
      static EntryComparator s_comp;
      typename MAP_STORE_TYPE::iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it == m_store.end() || !s_comp.equal(*it, index))
        it = this->insertEntry(it, index, VALUE_TYPE());
      return it->second;
    }

    VALUE_TYPE const &operator[](KEY_TYPE const &index) const
    {
      static EntryComparator s_comp;
      typename MAP_STORE_TYPE::const_iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it == m_store.end() || !s_comp.equal(*it, index)) {
        static VALUE_TYPE const sl_empty;
        return sl_empty;
      }
      return it->second;
    }

    //
    // Support for map lookups using non-key-type indices
    // Insertion is not possible with these template members
    //

    // Return the entry exactly equal to the index.
    template <typename INDEX_TYPE, class INDEX_COMP>
    iterator find(INDEX_TYPE const &index)
    {
      static IndexComparator<INDEX_TYPE, INDEX_COMP> s_comp;
      typename MAP_STORE_TYPE::iterator it = 
        std::lower_bound(m_store.begin(), m_store.end(), index, s_comp);
      if (it == m_store.end() || s_comp.equal(*it, index))
        return it;
      else
        return m_store.end();
    }

    const_iterator begin() const
    {
      return m_store.begin();
    }

    iterator begin()
    {
      return m_store.begin();
    }

    const_iterator end() const
    {
      return m_store.end();
    }

    iterator end()
    {
      return m_store.end();
    }

    virtual void clear()
    {
      m_store.clear();
    }

    bool empty() const
    {
      return m_store.empty();
    }

    size_t size() const
    {
      return m_store.size();
    }

    size_t capacity() const
    {
      return m_store.capacity();
    }

  protected:

    //
    // Extension API provided for implementors of derived classes
    //

    // Returns iterator to the new entry
    virtual iterator insertEntry(iterator it, KEY_TYPE const &k, VALUE_TYPE const &v)
    {
      return m_store.insert(it, MAP_ENTRY_TYPE(k, v));
    }
    
    MAP_STORE_TYPE m_store;

  private:
    // Not implemented
#if __cplusplus >= 201103L
    SimpleMap(SimpleMap const &) = delete;
    SimpleMap(SimpleMap &&) = delete;
    SimpleMap &operator=(SimpleMap const &) = delete;
    SimpleMap &operator=(SimpleMap &&) = delete;
#else
    SimpleMap(SimpleMap const &);
    SimpleMap &operator=(SimpleMap const &);
#endif

    // Wrap a used-defined comparator class for map entries

    struct EntryComparator
    {
      bool operator() (MAP_ENTRY_TYPE const &a, MAP_ENTRY_TYPE const &b)
      {
        return KEY_COMP()(a.first, b.first);
      }

      bool operator() (MAP_ENTRY_TYPE const &a, KEY_TYPE const &b)
      {
        return KEY_COMP()(a.first, b);
      }

      bool equal(MAP_ENTRY_TYPE const &a, KEY_TYPE const &b)
      {
        return KEY_COMP().equal(a.first, b);
      }
    };

    // Wrap a used-defined comparator class for comparing map entries to indices

    template <typename INDEX_TYPE, class INDEX_COMP>
    struct IndexComparator
    {
      bool operator() (MAP_ENTRY_TYPE const &a, INDEX_TYPE const &b)
      {
        return INDEX_COMP()(a.first, b);
      }

      // Only used in SimpleMap::find().
      bool equal(MAP_ENTRY_TYPE const &a, INDEX_TYPE const &b)
      {
        return INDEX_COMP().equal(a.first, b);
      }

      // Only used in SimpleMap::findFirst() and SimpleMap::findLast().
      bool match(MAP_ENTRY_TYPE const &a, INDEX_TYPE const &b)
      {
        return INDEX_COMP().match(a.first, b);
      }
    };
    
  };

} // namespace PLEXIL

#endif // PLEXIL_SIMPLE_MAP_HH
