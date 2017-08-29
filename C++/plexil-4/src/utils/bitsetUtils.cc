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

#include "plexil-config.h"

#include <bitset>
#include <climits>

#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

namespace PLEXIL
{

  int findFirstOne(unsigned long const b)
  {
    if (!b)
      return -1;

    // Must be at least one 1 bit
    int n = 0;
    unsigned long tmp, tmp2;

#if (ULONG_MAX > UINT32_MAX)
    if (!(tmp = b & 0xFFFFFFFFUL)) {
      n += 32;
      tmp = b >> 32;
    }
#else
    tmp = b;
#endif

    tmp2 = tmp;
    tmp &= 0xFFFFUL;
    if (!tmp) {
      n += 16;
      tmp = tmp2 >> 16;
    }

    tmp2 = tmp;
    if (!(tmp &= 0xFFUL)) {
      n += 8;
      tmp = tmp2 >> 8;
    }

    tmp2 = tmp;
    if (!(tmp &= 0xFUL)) {
      n += 4;
      tmp = tmp2 >> 4;
    }

    tmp2 = tmp;
    if (!(tmp &= 3UL)) {
      n += 2;
      tmp = tmp2 >> 2;
    }

    if (tmp & 1UL)
      return n;
    else
      return n + 1;
  }

  int findFirstZero(unsigned long const b)
  {
    return findFirstOne(~b);
  }

}
