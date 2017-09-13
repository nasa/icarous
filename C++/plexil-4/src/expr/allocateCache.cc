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

#include "bitsetUtils.hh"

#include "ArrayImpl.hh"
#include "Error.hh"
#include "ValueType.hh"

namespace PLEXIL
{

  // Default behavior
  template <typename T>
  T *allocateCache()
  {
    return new T();
  }

  template <typename T>
  void deallocateCache(T *ptr)
  {
    delete ptr;
  }

  //
  // Pool allocation for small types
  //

  //
  // API for a "bucket" in the cache pool
  // 

  template <typename T>
  class Bucket
  {
  public:
    Bucket() {}
    virtual ~Bucket() {}

    virtual bool isEmpty() const = 0;
    virtual bool isFull() const = 0;
    virtual T *allocate() = 0;
    virtual bool deallocate(T *ptr) = 0;
  };

  template <typename T, size_t N_ENTRIES>
  class BucketImpl : public Bucket<T>
  {
  private:

    std::bitset<N_ENTRIES> inUse;
    T bucket[N_ENTRIES];

    BucketImpl(BucketImpl const &);
    BucketImpl &operator=(BucketImpl const &);

  public:

    BucketImpl()
      : inUse(),
        bucket()
    {
    }

    virtual ~BucketImpl()
    {
      // assertTrue_1(inUse.none()); // must all be cleared first (?)
    }

    bool isEmpty() const
    {
      return inUse.none();
    }

    bool isFull() const
    {
      return inUse.all();
    }
    
    T *allocate()
    {
      if (inUse.all())
        return NULL; // full up

      int i = findFirstZero(inUse);
      if (i < 0 || N_ENTRIES <= (size_t) i) {
        // error
        return NULL;
      }

      inUse.set(i);
      return &bucket[i];
    }

    bool deallocate(T *ptr)
    {
      // Is it mine?
      int i = indexInBucket(ptr);
      if (i < 0)
        return false; // not mine

      // it's mine
      assertTrueMsg(inUse[i],
                    "deallocateCache: same pointer deallocated twice");
      inUse.reset(i);
      return true;
    }

  private:

    int indexInBucket(T const *ptr) const
    {
      if (ptr < &bucket[0] || ptr >= &bucket[N_ENTRIES])
        return -1; // not in this bucket

      // it's mine
      return (int) (ptr - &bucket[0]);
    }

  };

  // Forward declaration
  template <typename T>
  Bucket<T> *newBucket();

  template <typename T>
  class CachePool
  {
  private:
    CachePool()
      : m_buckets()
      {
      }

  public:

    static CachePool &instance()
    {
      static CachePool sl_instance;
      return sl_instance;
    }

    ~CachePool()
    {
      for (typename BucketVector::iterator it = m_buckets.begin();
           it != m_buckets.end();
           ++it)
        delete *it;
    }

    T *allocate()
    {
      // See if there's room in an existing bucket
      // Check newest first
      for (int i = m_buckets.size() - 1; i >= 0; --i) 
        if (!m_buckets[i]->isFull())
          return m_buckets[i]->allocate();
      
      // No, so add one
      addBucket();
      return m_buckets.back()->allocate();
    }

    void deallocate(T *ptr)
    {
      // simple linear search for now, in reverse order
      for (typename BucketVector::reverse_iterator rit = m_buckets.rbegin();
           rit != m_buckets.rend();
           ++rit) {
        Bucket<T> *b = *rit;
        if (b->deallocate(ptr)) {
          // Delete buckets when empty
          if (b->isEmpty()) {
            delete b;
            m_buckets.erase(rit.base() - 1);
          }
          return;
        }
      }

      assertTrue_2(ALWAYS_FAIL, "deallocate: Object not allocated");
    }

  private:

    // TODO get more sophisticated about sizing
    void addBucket()
    {
      m_buckets.push_back(newBucket<T>());
    }

    typedef std::vector<Bucket<T> *> BucketVector;

    BucketVector m_buckets;
  };

  //
  // Allocating new buckets
  //

  //
  // Size of a BucketImpl<T, N> on a 64-bit system is:
  //  N * sizeof(T) + ceil(N, 64) * 8 + 8
  //  (includes vtbl pointer)
  // 
  // Examples:
  //
  // sizeof(BucketImpl<Boolean, 64>) ->
  //  64 * 1 + 1 * 8 + 8 = 80 bytes
  //
  // If the allocation quantum is 16 bytes, this is a win if 6 or more Booleans are allocated.
  // 
  // sizeof(BucketImpl<Integer, 64>) ->
  //  64 * 4 + 1 * 8 + 8 = 272 bytes
  //
  // If the allocation quantum is 16 bytes, this is a win if 18 or more Integers are allocated.
  // 
  // sizeof(BucketImpl<Real, 64>) ->
  //  64 * 8 + 1 * 8 + 8 = 528 bytes
  //
  // If the allocation quantum is 16 bytes, this is a win if 34 or more Reals are allocated.
  //

  static size_t const BOOLEAN_BUCKET_SIZE = 64;
  static size_t const INTEGER_BUCKET_SIZE = 64;
  static size_t const REAL_BUCKET_SIZE = 64;

  template <>
  Bucket<Boolean> *newBucket()
  {
    return new BucketImpl<Boolean, BOOLEAN_BUCKET_SIZE>();
  }

  template <>
  Bucket<Integer> *newBucket()
  {
    return new BucketImpl<Integer, INTEGER_BUCKET_SIZE>();
  }

  template <>
  Bucket<Real> *newBucket()
  {
    return new BucketImpl<Real, REAL_BUCKET_SIZE>();
  }

  //
  // Instantiations
  //

  template <>
  Boolean *allocateCache<Boolean>()
  {
    return CachePool<Boolean>::instance().allocate();
  }

  template <>
  Integer *allocateCache<Integer>()
  {
    return CachePool<Integer>::instance().allocate();
  }

  template <>
  Real *allocateCache<Real>()
  {
    return CachePool<Real>::instance().allocate();
  }

  template <>
  void deallocateCache<Boolean>(Boolean *ptr)
  {
    CachePool<Boolean>::instance().deallocate(ptr);
  }

  template <>
  void deallocateCache<Integer>(Integer *ptr)
  {
    CachePool<Integer>::instance().deallocate(ptr);
  }

  template <>
  void deallocateCache<Real>(Real *ptr)
  {
    CachePool<Real>::instance().deallocate(ptr);
  }

  // Explicit instantiation of default template for other types

  template String *allocateCache<String>();
  template void deallocateCache<String>(String *);

  // Not yet

  // template ArrayImpl<Boolean> *allocateCache<ArrayImpl<Boolean> >();
  // template void deallocateCache<ArrayImpl<Boolean> >(ArrayImpl<Boolean> *);

  // template ArrayImpl<Integer> *allocateCache<ArrayImpl<Integer> >();
  // template void deallocateCache<ArrayImpl<Integer> >(ArrayImpl<Integer> *);

  // template ArrayImpl<Real> *allocateCache<ArrayImpl<Real> >();
  // template void deallocateCache<ArrayImpl<Real> >(ArrayImpl<Real> *);

  // template ArrayImpl<String> *allocateCache<ArrayImpl<String> >();
  // template void deallocateCache<ArrayImpl<String> >(ArrayImpl<String> *);
  
}
