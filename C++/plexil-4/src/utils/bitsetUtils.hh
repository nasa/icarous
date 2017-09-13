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

#include "Error.hh"

#include <bitset>
#include <climits>

namespace PLEXIL
{

  // O(1) via binary search
  extern int findFirstOne(unsigned long const b);

  extern int findFirstZero(unsigned long const b);

  // O(n) via iterative search
  template <size_t N_ENTRIES>
  int findFirstOne(std::bitset<N_ENTRIES> const &b)
  {
    if (b.none())
      return -1;

    // There must be at least one 1 bit

    // Find first word with a 1
    size_t const WORD_SIZE = sizeof(unsigned long) * 8;
    
    std::bitset<N_ENTRIES> const mask(ULONG_MAX);
    std::bitset<N_ENTRIES> tmp = b;
    for (size_t n = 0;
         n < N_ENTRIES;
         n += WORD_SIZE,
           tmp >>= WORD_SIZE) {
      unsigned long tmp2 = (tmp & mask).to_ulong();
      if (tmp2)
        return n + findFirstOne(tmp2);
    }

    // better not happen after debugged
    check_error_2(ALWAYS_FAIL, "findFirstOne: bitset all zeroes");
    return -1;
  }

  // O(n) via iterative search
  template <size_t N_ENTRIES>
  int findFirstZero(std::bitset<N_ENTRIES> const &b)
  {
    if (b.all())
      return -1;

    // There must be at least one 0 bit

    // Find first word with a 0
    size_t const WORD_SIZE = sizeof(unsigned long) * 8;
    
    std::bitset<N_ENTRIES> const mask(ULONG_MAX);
    std::bitset<N_ENTRIES> tmp = b;
    for (size_t n = 0;
         n < N_ENTRIES;
         n += WORD_SIZE,
           tmp >>= WORD_SIZE) {
      unsigned long tmp2 = (tmp & mask).to_ulong();
      if (~tmp2)
        return n + findFirstZero(tmp2);
    }

    // better not happen after debugged
    check_error_2(ALWAYS_FAIL, "findFirstZero: bitset all ones");
    return -1;
  }

}
