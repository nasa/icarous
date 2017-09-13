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

#include "allocateCache.hh"
#include "TestSupport.hh"
#include "ValueType.hh"

using PLEXIL::Boolean;
using PLEXIL::Integer;
using PLEXIL::Real;
using PLEXIL::allocateCache;
using PLEXIL::deallocateCache;

template <typename T, size_t N>
void lifoTest()
{
  std::vector<T *> caches(N, NULL);
  int i;
  // Allocate a bunch
  for (i = 0; i < (int) N; ++i)
    caches[i] = allocateCache<T>();
  // Return them all in last-in, first-out order
  for (i = (int) N - 1; i >= 0; --i) {
    deallocateCache<T>(caches[i]);
    caches[i] = NULL;
  }
}

static bool lifoAllocationTest()
{
  lifoTest<Boolean, 129>();
  lifoTest<Integer, 129>();
  lifoTest<Real, 129>();
  return true;
}

// Pick a modulus that is relatively prime to N

template <typename T, size_t N, size_t MODULUS>
void moduloTest()
{
  std::vector<T *> caches(N, NULL);
  // Allocate a bunch
  for (size_t i = 0; i < N; ++i)
    caches[i] = allocateCache<T>();
  size_t ix = 0; // could be any number between 0 .. N-1
  for (size_t i = 0; i < N; ++i) {
    if (!caches[ix]) {
      std::cout << "moduloTest: repeated index " << ix << " on iteration " << i << std::endl;
      return;
    }
    deallocateCache<T>(caches[ix]);
    caches[ix] = NULL;
    ix = (ix + MODULUS) % N;
  }
}

static bool moduloAllocationTest()
{
  moduloTest<Boolean, 129, 37>();
  moduloTest<Integer, 129, 37>();
  moduloTest<Real, 129, 37>();
  return true;
}

// Pick a modulus that is relatively prime to N

template <typename T, size_t N, size_t MODULUS>
void mixedTest()
{
  std::vector<T *> caches(N, NULL);
  // Allocate a bunch
  size_t i;
  for (i = 0; i < N; ++i)
    caches[i] = allocateCache<T>();
  size_t const INITIAL_IX = 0; // could be any number between 0 .. N-1
  size_t ix = INITIAL_IX;
  // Delete half (ish)
  for (i = 0; i < N/2; ++i) {
    if (!caches[ix]) {
      std::cout << "moduloTest: repeated deletion index " << ix << " on iteration " << i << std::endl;
      return;
    }
    deallocateCache<T>(caches[ix]);
    caches[ix] = NULL;
    ix = (ix + MODULUS) % N;
  }
  // Reallocate those
  ix = INITIAL_IX;
  for (i = 0; i < N/2; ++i) {
    if (caches[ix]) {
      std::cout << "moduloTest: repeated reallocation index " << ix << " on iteration " << i << std::endl;
      return;
    }
    caches[ix] = allocateCache<T>();
    ix = (ix + MODULUS) % N;
  }
  // Return them all in linear order
  for (i = 0; i < N; ++i) {
    deallocateCache<T>(caches[i]);
    caches[i] = NULL;
  }
}

static bool mixedAllocationTest()
{
  mixedTest<Boolean, 129, 37>();
  mixedTest<Integer, 129, 37>();
  mixedTest<Real, 129, 37>();
  return true;
}

bool allocatorTest()
{
  runTest(lifoAllocationTest);
  runTest(moduloAllocationTest);
  runTest(mixedAllocationTest);
  // more todo?

  return true;
}

