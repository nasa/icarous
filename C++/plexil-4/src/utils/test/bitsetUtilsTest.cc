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

#include "bitsetUtils.hh"
#include "TestSupport.hh"

#include <climits>

#ifdef HAVE_STDINT_H
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#elif defined(__VXWORKS__)
#include <vxWorks.h>
#endif

using namespace PLEXIL;

static bool testUnsignedLong()
{
  unsigned long ul0 = 0;
  assertTrue_1(findFirstOne(ul0) == -1);

  unsigned long ul1 = 1;
  assertTrue_1(findFirstOne(ul1) == 0);

  unsigned long ul2 = ULONG_MAX - 1;
  assertTrue_1(findFirstOne(ul2) == 1);

  unsigned long ul3 = 1UL << 20;
  assertTrue_1(findFirstOne(ul3) == 20);

#if (ULONG_MAX > UINT32_MAX)

  unsigned long ul4 = 1UL << 40;
  assertTrue_1(findFirstOne(ul4) == 40);

#endif

  unsigned long ul10 = ULONG_MAX;
  assertTrue_1(findFirstZero(ul10) == -1);

  unsigned long ul11 = ~1UL;
  assertTrue_1(findFirstZero(ul11) == 0);

  unsigned long ul12 = ULONG_MAX - 4;
  assertTrue_1(findFirstZero(ul12) == 2);

  unsigned long ul13 = ~(1UL << 20);
  assertTrue_1(findFirstZero(ul13) == 20);

#if (ULONG_MAX > UINT32_MAX)

  unsigned long ul14 = ~(1UL << 40);
  assertTrue_1(findFirstZero(ul14) == 40);

#endif

  return true;
}

static bool testBitset()
{
  std::bitset<32> bs0 = 0;
  assertTrue_1(findFirstOne(bs0) == -1);

  std::bitset<32> bs1 = 1;
  assertTrue_1(findFirstOne(bs1) == 0);

  std::bitset<32> bs2 = ULONG_MAX - 1;
  assertTrue_1(findFirstOne(bs2) == 1);

  std::bitset<32> bs3 = 1UL << 20;
  assertTrue_1(findFirstOne(bs3) == 20);

#if (ULONG_MAX > UINT32_MAX)

  std::bitset<64> bs3a = 1UL << 20;
  assertTrue_1(findFirstOne(bs3a) == 20);

  std::bitset<64> bs4 = 1UL << 40;
  assertTrue_1(findFirstOne(bs4) == 40);

#endif

  std::bitset<128> bs10;
  assertTrue_1(findFirstOne(bs10) == -1);
  
  std::bitset<128> bs11;
  bs11.set(33);
  assertTrue_1(findFirstOne(bs11) == 33);

  std::bitset<128> bs12;
  bs12.set(77);
  assertTrue_1(findFirstOne(bs12) == 77);

  std::bitset<1024> bs13;
  bs13.set(197);
  assertTrue_1(findFirstOne(bs13) == 197);

  std::bitset<1024> bs14;
  bs14.set(995);
  assertTrue_1(findFirstOne(bs14) == 995);

  std::bitset<3095> bs15;
  bs15.set(2994);
  assertTrue_1(findFirstOne(bs15) == 2994);

  // findFirstZero

  std::bitset<32> bs20 = ULONG_MAX;
  assertTrue_1(findFirstZero(bs20) == -1);

  std::bitset<32> bs21 = ~1UL;
  assertTrue_1(findFirstZero(bs21) == 0);

  std::bitset<32> bs22 = ULONG_MAX - 4;
  assertTrue_1(findFirstZero(bs22) == 2);

  std::bitset<32> bs23 = ~(1UL << 20);
  assertTrue_1(findFirstZero(bs23) == 20);

#if (ULONG_MAX > UINT32_MAX)

  std::bitset<64> bs23a = ~(1UL << 20);
  assertTrue_1(findFirstZero(bs23a) == 20);

  std::bitset<64> bs24 = ~(1UL << 40);
  assertTrue_1(findFirstZero(bs24) == 40);

#endif
 
  std::bitset<128> bs30;
  bs30.set();
  assertTrue_1(findFirstZero(bs30) == -1);
  
  std::bitset<128> bs31;
  bs31.set();
  bs31.reset(33);
  assertTrue_1(findFirstZero(bs31) == 33);

  std::bitset<128> bs32;
  bs32.set();
  bs32.reset(77);
  assertTrue_1(findFirstZero(bs32) == 77);

  std::bitset<1024> bs33;
  bs33.set();
  bs33.reset(197);
  assertTrue_1(findFirstZero(bs33) == 197);

  std::bitset<1024> bs34;
  bs34.set();
  bs34.reset(995);
  assertTrue_1(findFirstZero(bs34) == 995);

  std::bitset<3095> bs35;
  bs35.set();
  bs35.reset(2994);
  assertTrue_1(findFirstZero(bs35) == 2994);

  return true;

}

bool bitsetUtilsTest()
{
  runTest(testUnsignedLong);
  runTest(testBitset);

  return true;
}
