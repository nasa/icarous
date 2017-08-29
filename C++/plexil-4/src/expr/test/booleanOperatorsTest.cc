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

#include "BooleanOperators.hh"
#include "Constant.hh"
#include "Function.hh"
#include "TestSupport.hh"

using namespace PLEXIL;

static bool testBooleanNot()
{
  BooleanConstant unk;
  BooleanConstant troo(true);
  BooleanConstant falls(false);

  // Test logic
  Function *n1 = makeFunction(BooleanNot::instance(), &unk, false);
  Function *n2 = makeFunction(BooleanNot::instance(), &troo, false);
  Function *n3 = makeFunction(BooleanNot::instance(), &falls, false);

  n1->activate();
  n2->activate();
  n3->activate();

  bool temp;
  assertTrue_1(!n1->getValue(temp));

  assertTrue_1(n2->getValue(temp));
  assertTrue_1(!temp);

  assertTrue_1(n3->getValue(temp));
  assertTrue_1(temp);

  delete n3;
  delete n2;
  delete n1;

  return true;
}

static bool testBooleanAnd()
{
  BooleanConstant unk;
  BooleanConstant troo(true);
  BooleanConstant falls(false);

  // Basic three-valued AND semantics of two args
  Function *c1= makeFunction(BooleanAnd::instance(),
                             &falls, &falls,
                             false, false);
  Function *c2= makeFunction(BooleanAnd::instance(),
                             &falls, &unk,
                             false, false);
  Function *c3= makeFunction(BooleanAnd::instance(),
                             &falls, &troo,
                             false, false);
  Function *c4= makeFunction(BooleanAnd::instance(),
                             &unk, &falls,
                             false, false);
  Function *c5= makeFunction(BooleanAnd::instance(),
                             &unk, &unk,
                             false, false);
  Function *c6= makeFunction(BooleanAnd::instance(),
                             &unk, &troo,
                             false, false);
  Function *c7= makeFunction(BooleanAnd::instance(),
                             &troo, &falls,
                             false, false);
  Function *c8= makeFunction(BooleanAnd::instance(),
                             &troo, &unk,
                             false, false);
  Function *c9= makeFunction(BooleanAnd::instance(),
                             &troo, &troo,
                             false, false);

  c1->activate();
  c2->activate();
  c3->activate();
  c4->activate();
  c5->activate();
  c6->activate();
  c7->activate();
  c8->activate();
  c9->activate();

  bool temp;
  assertTrue_1(c1->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(c2->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(c3->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(c4->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(!c5->getValue(temp));
  assertTrue_1(!c6->getValue(temp));
  assertTrue_1(c7->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(!c8->getValue(temp));
  assertTrue_1(c9->getValue(temp));
  assertTrue_1(temp);

  delete c9;
  delete c8;
  delete c7;
  delete c6;
  delete c5;
  delete c4;
  delete c3;
  delete c2;
  delete c1;

  // TODO: 3 or more args

  return true;
}

static bool testBooleanOr()
{
  BooleanConstant unk;
  BooleanConstant troo(true);
  BooleanConstant falls(false);

  // Three-valued OR semantics of two args
  Function *d1= makeFunction(BooleanOr::instance(),
                             &falls, &falls,
                             false, false);
  Function *d2= makeFunction(BooleanOr::instance(),
                             &falls, &unk,
                             false, false);
  Function *d3= makeFunction(BooleanOr::instance(),
                             &falls, &troo,
                             false, false);
  Function *d4= makeFunction(BooleanOr::instance(),
                             &unk, &falls,
                             false, false);
  Function *d5= makeFunction(BooleanOr::instance(),
                             &unk, &unk,
                             false, false);
  Function *d6= makeFunction(BooleanOr::instance(),
                             &unk, &troo,
                             false, false);
  Function *d7= makeFunction(BooleanOr::instance(),
                             &troo, &falls,
                             false, false);
  Function *d8= makeFunction(BooleanOr::instance(),
                             &troo, &unk,
                             false, false);
  Function *d9= makeFunction(BooleanOr::instance(),
                             &troo, &troo,
                             false, false);

  d1->activate();
  d2->activate();
  d3->activate();
  d4->activate();
  d5->activate();
  d6->activate();
  d7->activate();
  d8->activate();
  d9->activate();

  bool temp;
  assertTrue_1(d1->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(!d2->getValue(temp));
  assertTrue_1(d3->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(!d4->getValue(temp));
  assertTrue_1(!d5->getValue(temp));
  assertTrue_1(d6->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(d7->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(d8->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(d9->getValue(temp));
  assertTrue_1(temp);

  delete d9;
  delete d8;
  delete d7;
  delete d6;
  delete d5;
  delete d4;
  delete d3;
  delete d2;
  delete d1;

  // TODO: 3 or more args

  return true;
}

static bool testBooleanXor()
{
  BooleanConstant unk;
  BooleanConstant troo(true);
  BooleanConstant falls(false);

  //test basic three-valued XOR semantics
  Function *d1= makeFunction(BooleanXor::instance(),
                             &falls, &falls,
                             false, false);
  Function *d2= makeFunction(BooleanXor::instance(),
                             &falls, &unk,
                             false, false);
  Function *d3= makeFunction(BooleanXor::instance(),
                             &falls, &troo,
                             false, false);
  Function *d4= makeFunction(BooleanXor::instance(),
                             &unk, &falls,
                             false, false);
  Function *d5= makeFunction(BooleanXor::instance(),
                             &unk, &unk,
                             false, false);
  Function *d6= makeFunction(BooleanXor::instance(),
                             &unk, &troo,
                             false, false);
  Function *d7= makeFunction(BooleanXor::instance(),
                             &troo, &falls,
                             false, false);
  Function *d8= makeFunction(BooleanXor::instance(),
                             &troo, &unk,
                             false, false);
  Function *d9= makeFunction(BooleanXor::instance(),
                             &troo, &troo,
                             false, false);

  d1->activate();
  d2->activate();
  d3->activate();
  d4->activate();
  d5->activate();
  d6->activate();
  d7->activate();
  d8->activate();
  d9->activate();

  bool temp;
  assertTrue_1(d1->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(!d2->getValue(temp));
  assertTrue_1(d3->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(!d4->getValue(temp));
  assertTrue_1(!d5->getValue(temp));
  assertTrue_1(!d6->getValue(temp));
  assertTrue_1(d7->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(!d8->getValue(temp));
  assertTrue_1(d9->getValue(temp));
  assertTrue_1(!temp);

  delete d9;
  delete d8;
  delete d7;
  delete d6;
  delete d5;
  delete d4;
  delete d3;
  delete d2;
  delete d1;

  // TODO: 3 or more args
    
  return true;
}

bool booleanOperatorsTest()
{
  runTest(testBooleanNot);
  runTest(testBooleanAnd);
  runTest(testBooleanOr);
  runTest(testBooleanXor);

  return true;
}
