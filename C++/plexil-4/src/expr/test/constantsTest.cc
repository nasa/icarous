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

#include "Constant.hh"
#include "TestSupport.hh"

using namespace PLEXIL;

//
// Constants
//

static bool testScalarConstants()
{
  // Default constructor check
  BooleanConstant ub;
  IntegerConstant ui;
  RealConstant ud;
  StringConstant us;

  // Direct access
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

  // getValue on unknowns
  double food;
  std::string foos;
  int32_t fooi;
  bool foob;
  assertTrue_1(!ub.getValue(foob));
  assertTrue_1(!ui.getValue(fooi));
  assertTrue_1(!ud.getValue(food));
  assertTrue_1(!us.getValue(foos));

  // getValuePointer on unknowns
  std::string const *pfoos;
  assertTrue_1(!us.getValuePointer(pfoos));

  // Constants with values
  Expression const *troo = new BooleanConstant(true);
  Expression const *too = new IntegerConstant(2);
  Expression const *doo = new RealConstant(2.718);
  Expression const *soo = new StringConstant("Sue");

  assertTrue_1(troo->isConstant());
  assertTrue_1(too->isConstant());
  assertTrue_1(doo->isConstant());
  assertTrue_1(soo->isConstant());
  assertTrue_1(troo->isKnown());
  assertTrue_1(too->isKnown());
  assertTrue_1(doo->isKnown());
  assertTrue_1(soo->isKnown());
    
  // getValue() test
  assertTrue_1(troo->getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(too->getValue(fooi));
  assertTrue_1(fooi == 2);
  assertTrue_1(doo->getValue(food));
  assertTrue_1(food == 2.718);
  assertTrue_1(soo->getValue(foos));
  assertTrue_1(foos == std::string("Sue"));
  // Numeric conversion
  assertTrue_1(too->getValue(food));
  assertTrue_1(food == 2);

  // getValuePointer() tests
  assertTrue_1(soo->getValuePointer(pfoos));
  assertTrue_1(*pfoos == std::string("Sue"));

  return true;
}


bool constantsTest()
{
  runTest(testScalarConstants);
  return true;
}

