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

#include "Constant.hh"
#include "TestSupport.hh"

using namespace PLEXIL;

static bool testArrayConstants()
{
  // Default constructor check
  BooleanArrayConstant ub;
  IntegerArrayConstant ui;
  RealArrayConstant ud;
  StringArrayConstant us;

  assertTrue_1(ub.isConstant());
  assertTrue_1(ui.isConstant());
  assertTrue_1(ud.isConstant());
  assertTrue_1(us.isConstant());
  assertTrue_1(!ub.isAssignable());
  assertTrue_1(!ui.isAssignable());
  assertTrue_1(!ud.isAssignable());
  assertTrue_1(!us.isAssignable());
  assertTrue_1(!ub.isKnown());
  assertTrue_1(!ui.isKnown());
  assertTrue_1(!ud.isKnown());
  assertTrue_1(!us.isKnown());

  // getValuePointer test
  BooleanArray const *pbool = NULL;
  IntegerArray const *pint = NULL;
  RealArray const *preal = NULL;
  StringArray const *pstr = NULL;

  assertTrue_1(!ub.getValuePointer(pbool));
  assertTrue_1(pbool == NULL);
  assertTrue_1(!ui.getValuePointer(pint));
  assertTrue_1(pint == NULL);
  assertTrue_1(!ud.getValuePointer(preal));
  assertTrue_1(preal == NULL);
  assertTrue_1(!us.getValuePointer(pstr));
  assertTrue_1(pstr == NULL);
  
  // Constants with values
  std::vector<bool> oob;
  oob.push_back(true);
  oob.push_back(false);
  BooleanArrayConstant troo(oob);

  std::vector<int32_t> ooi;
  ooi.push_back(42);
  ooi.push_back(69);
  ooi.push_back(14);
  IntegerArrayConstant too(ooi);

  std::vector<double> ood;
  ood.push_back(1.414);
  ood.push_back(2.718);
  ood.push_back(3.1416);
  ood.push_back(3.162);
  RealArrayConstant doo(ood);

  std::vector<std::string> oos;
  oos.push_back(std::string("foo"));
  oos.push_back(std::string("bar"));
  oos.push_back(std::string("baz"));
  oos.push_back(std::string("bletch"));
  oos.push_back(std::string("blorf"));
  StringArrayConstant soo(oos);

  std::vector<bool> const *pvb;
  std::vector<int32_t> const *pvi;
  std::vector<double> const *pvd;
  std::vector<std::string> const *pvs;

  assertTrue_1(troo.isConstant());
  assertTrue_1(too.isConstant());
  assertTrue_1(doo.isConstant());
  assertTrue_1(soo.isConstant());

  assertTrue_1(!troo.isAssignable());
  assertTrue_1(!too.isAssignable());
  assertTrue_1(!doo.isAssignable());
  assertTrue_1(!soo.isAssignable());

  assertTrue_1(troo.isKnown());
  assertTrue_1(too.isKnown());
  assertTrue_1(doo.isKnown());
  assertTrue_1(soo.isKnown());

  // Access via getValuePointer
  assertTrue_1(troo.getValuePointer(pbool));
  assertTrue_1(pbool != NULL);
  assertTrue_1(pbool->allElementsKnown());
  pbool->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(oob == *pvb);

  assertTrue_1(too.getValuePointer(pint));
  assertTrue_1(pint != NULL);
  assertTrue_1(pint->allElementsKnown());
  pint->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(ooi == *pvi);

  assertTrue_1(doo.getValuePointer(preal));
  assertTrue_1(preal != NULL);
  assertTrue_1(preal->allElementsKnown());
  preal->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(ood == *pvd);

  assertTrue_1(soo.getValuePointer(pstr));
  assertTrue_1(pstr != NULL);
  assertTrue_1(pstr->allElementsKnown());
  pstr->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(oos == *pvs);

  pbool = NULL;
  pint = NULL;
  preal = NULL;
  pstr = NULL;

  return true;
}

bool arrayConstantTest()
{
  runTest(testArrayConstants);
  return true;
}
