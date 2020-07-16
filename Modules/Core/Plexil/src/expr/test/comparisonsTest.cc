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


#include "Comparisons.hh"  // includes Operator.hh
#include "Constant.hh"     // includes Expression.hh
#include "Function.hh"     // includes NotifierImpl.hh
#include "TestSupport.hh"  // includes Error.hh
#include "UserVariable.hh"

using namespace PLEXIL;

static bool testIsKnown()
{
  bool tempb;

  BooleanVariable varb;
  IntegerVariable vari;
  RealVariable varr;
  StringVariable vars;
  
  Function *iskfnb = makeFunction(IsKnown::instance(), &varb, false);
  Function *iskfni = makeFunction(IsKnown::instance(), &vari, false);
  Function *iskfnr = makeFunction(IsKnown::instance(), &varr, false);
  Function *iskfns = makeFunction(IsKnown::instance(), &vars, false);

  iskfnb->activate();
  iskfni->activate();
  iskfnr->activate();
  iskfns->activate();

  // All should be known but false because variables not inited
  assertTrue_1(iskfnb->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(iskfni->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(iskfnr->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(iskfns->getValue(tempb));
  assertTrue_1(!tempb);

  // Set variables and try again
  varb.setValue(true);
  vari.setValue((Integer) 93);
  varr.setValue(27.1);
  vars.setValue(String("Oop"));

  // Now all should be known and true
  assertTrue_1(iskfnb->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(iskfni->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(iskfnr->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(iskfns->getValue(tempb));
  assertTrue_1(tempb);

  delete iskfns;
  delete iskfnr;
  delete iskfni;
  delete iskfnb;

  return true;
}

static bool testBoolean()
{
  bool tempb;

  // Boolean
  BooleanConstant troo(true);
  BooleanConstant falls(false);
  BooleanVariable varb;

  Function *boolEq1 = makeFunction(Equal::instance(),
                                   &troo, &troo,
                                   false, false);
  Function *boolEq2 = makeFunction(Equal::instance(),
                                   &falls, &falls,
                                   false, false);
  Function *boolEq3 = makeFunction(Equal::instance(),
                                   &troo, &falls,
                                   false, false);
  Function *boolEq4 = makeFunction(Equal::instance(),
                                   &falls, &troo,
                                   false, false);
  Function *boolEq5 = makeFunction(Equal::instance(),
                                   &troo, &varb,
                                   false, false);
  Function *boolEq6 = makeFunction(Equal::instance(),
                                   &varb, &falls,
                                   false, false);

  Function *boolNeq1 = makeFunction(NotEqual::instance(),
                                    &troo, &troo,
                                    false, false);
  Function *boolNeq2 = makeFunction(NotEqual::instance(),
                                    &falls, &falls,
                                    false, false);
  Function *boolNeq3 = makeFunction(NotEqual::instance(),
                                    &troo, &falls,
                                    false, false);
  Function *boolNeq4 = makeFunction(NotEqual::instance(),
                                    &falls, &troo,
                                    false, false);
  Function *boolNeq5 = makeFunction(NotEqual::instance(),
                                    &troo, &varb,
                                    false, false);
  Function *boolNeq6 = makeFunction(NotEqual::instance(),
                                    &varb, &falls,
                                    false, false);

  boolEq1->activate();
  boolEq2->activate();
  boolEq3->activate();
  boolEq4->activate();
  boolEq5->activate();
  boolEq6->activate();

  boolNeq1->activate();
  boolNeq2->activate();
  boolNeq3->activate();
  boolNeq4->activate();
  boolNeq5->activate();
  boolNeq6->activate();

  assertTrue_1(boolEq1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolEq2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolEq3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolEq4->getValue(tempb));
  assertTrue_1(!tempb);

  // These should be unknown because varb is uninitialized
  assertTrue_1(!boolEq5->getValue(tempb));
  assertTrue_1(!boolEq6->getValue(tempb));

  assertTrue_1(boolNeq1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolNeq2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolNeq3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolNeq4->getValue(tempb));
  assertTrue_1(tempb);

  // These should be unknown because varb is uninitialized
  assertTrue_1(!boolNeq5->getValue(tempb));
  assertTrue_1(!boolNeq6->getValue(tempb));

  // Set varb and try again
  varb.setValue(false);
  assertTrue_1(boolEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolEq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolNeq6->getValue(tempb));
  assertTrue_1(!tempb);

  varb.setValue(true);
  assertTrue_1(boolEq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(boolEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolNeq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(boolNeq6->getValue(tempb));
  assertTrue_1(tempb);

  delete boolNeq6;
  delete boolNeq5;
  delete boolNeq4;
  delete boolNeq3;
  delete boolNeq2;
  delete boolNeq1;

  delete boolEq6;
  delete boolEq5;
  delete boolEq4;
  delete boolEq3;
  delete boolEq2;
  delete boolEq1;

  return true;
}

static bool testString()
{
  bool tempb;

  // String
  StringConstant yo("Yo!");
  StringConstant mama("Mama");
  StringVariable vars;

  Function *strEq1 = makeFunction(Equal::instance(),
                                  &yo, &yo,
                                  false, false);
  Function *strEq2 = makeFunction(Equal::instance(),
                                  &mama, &mama,
                                  false, false);
  Function *strEq3 = makeFunction(Equal::instance(),
                                  &yo, &mama,
                                  false, false);
  Function *strEq4 = makeFunction(Equal::instance(),
                                  &mama, &yo,
                                  false, false);
  Function *strEq5 = makeFunction(Equal::instance(),
                                  &yo, &vars,
                                  false, false);
  Function *strEq6 = makeFunction(Equal::instance(),
                                  &vars, &mama,
                                  false, false);

  Function *strNeq1 = makeFunction(NotEqual::instance(),
                                   &yo, &yo,
                                   false, false);
  Function *strNeq2 = makeFunction(NotEqual::instance(),
                                   &mama, &mama,
                                   false, false);
  Function *strNeq3 = makeFunction(NotEqual::instance(),
                                   &yo, &mama,
                                   false, false);
  Function *strNeq4 = makeFunction(NotEqual::instance(),
                                   &mama, &yo,
                                   false, false);
  Function *strNeq5 = makeFunction(NotEqual::instance(),
                                   &yo, &vars,
                                   false, false);
  Function *strNeq6 = makeFunction(NotEqual::instance(),
                                   &vars, &mama,
                                   false, false);

  Function *strGt1 = makeFunction(GreaterThan<String>::instance(),
                                  &yo, &yo,
                                  false, false);
  Function *strGt2 = makeFunction(GreaterThan<String>::instance(),
                                  &mama, &mama,
                                  false, false);
  Function *strGt3 = makeFunction(GreaterThan<String>::instance(),
                                  &yo, &mama,
                                  false, false);
  Function *strGt4 = makeFunction(GreaterThan<String>::instance(),
                                  &mama, &yo,
                                  false, false);
  Function *strGt5 = makeFunction(GreaterThan<String>::instance(),
                                  &yo, &vars,
                                  false, false);
  Function *strGt6 = makeFunction(GreaterThan<String>::instance(),
                                  &vars, &mama,
                                  false, false);

  Function *strGe1 = makeFunction(GreaterEqual<String>::instance(),
                                  &yo, &yo,
                                  false, false);
  Function *strGe2 = makeFunction(GreaterEqual<String>::instance(),
                                  &mama, &mama,
                                  false, false);
  Function *strGe3 = makeFunction(GreaterEqual<String>::instance(),
                                  &yo, &mama,
                                  false, false);
  Function *strGe4 = makeFunction(GreaterEqual<String>::instance(),
                                  &mama, &yo,
                                  false, false);
  Function *strGe5 = makeFunction(GreaterEqual<String>::instance(),
                                  &yo, &vars,
                                  false, false);
  Function *strGe6 = makeFunction(GreaterEqual<String>::instance(),
                                  &vars, &mama,
                                  false, false);

  Function *strLt1 = makeFunction(LessThan<String>::instance(),
                                  &yo, &yo,
                                  false, false);
  Function *strLt2 = makeFunction(LessThan<String>::instance(),
                                  &mama, &mama,
                                  false, false);
  Function *strLt3 = makeFunction(LessThan<String>::instance(),
                                  &yo, &mama,
                                  false, false);
  Function *strLt4 = makeFunction(LessThan<String>::instance(),
                                  &mama, &yo,
                                  false, false);
  Function *strLt5 = makeFunction(LessThan<String>::instance(),
                                  &yo, &vars,
                                  false, false);
  Function *strLt6 = makeFunction(LessThan<String>::instance(),
                                  &vars, &mama,
                                  false, false);

  Function *strLe1 = makeFunction(LessEqual<String>::instance(),
                                  &yo, &yo,
                                  false, false);
  Function *strLe2 = makeFunction(LessEqual<String>::instance(),
                                  &mama, &mama,
                                  false, false);
  Function *strLe3 = makeFunction(LessEqual<String>::instance(),
                                  &yo, &mama,
                                  false, false);
  Function *strLe4 = makeFunction(LessEqual<String>::instance(),
                                  &mama, &yo,
                                  false, false);
  Function *strLe5 = makeFunction(LessEqual<String>::instance(),
                                  &yo, &vars,
                                  false, false);
  Function *strLe6 = makeFunction(LessEqual<String>::instance(),
                                  &vars, &mama,
                                  false, false);

  strEq1->activate();
  strEq2->activate();
  strEq3->activate();
  strEq4->activate();
  strEq5->activate();
  strEq6->activate();

  strNeq1->activate();
  strNeq2->activate();
  strNeq3->activate();
  strNeq4->activate();
  strNeq5->activate();
  strNeq6->activate();

  strGt1->activate();
  strGt2->activate();
  strGt3->activate();
  strGt4->activate();
  strGt5->activate();
  strGt6->activate();

  strGe1->activate();
  strGe2->activate();
  strGe3->activate();
  strGe4->activate();
  strGe5->activate();
  strGe6->activate();

  strLt1->activate();
  strLt2->activate();
  strLt3->activate();
  strLt4->activate();
  strLt5->activate();
  strLt6->activate();

  strLe1->activate();
  strLe2->activate();
  strLe3->activate();
  strLe4->activate();
  strLe5->activate();
  strLe6->activate();

  // "Yo!" ? "Yo!"
  assertTrue_1(strEq1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strNeq1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGe1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe1->getValue(tempb));
  assertTrue_1(tempb);

  // "Mama" ? "Mama"
  assertTrue_1(strEq2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strNeq2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGe2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe2->getValue(tempb));
  assertTrue_1(tempb);

  // "Yo!" ? "Mama"
  assertTrue_1(strEq3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGe3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe3->getValue(tempb));
  assertTrue_1(!tempb);

  // "Mama" ? "Yo!"
  assertTrue_1(strEq4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGe4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLt4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLe4->getValue(tempb));
  assertTrue_1(tempb);

  // These should be unknown because vars is uninitialized
  assertTrue_1(!strEq5->getValue(tempb));
  assertTrue_1(!strNeq5->getValue(tempb));
  assertTrue_1(!strGt5->getValue(tempb));
  assertTrue_1(!strGe5->getValue(tempb));
  assertTrue_1(!strLt5->getValue(tempb));
  assertTrue_1(!strLe5->getValue(tempb));

  assertTrue_1(!strEq6->getValue(tempb));
  assertTrue_1(!strNeq6->getValue(tempb));
  assertTrue_1(!strGt6->getValue(tempb));
  assertTrue_1(!strGe6->getValue(tempb));
  assertTrue_1(!strLt6->getValue(tempb));
  assertTrue_1(!strLe6->getValue(tempb));

  // Set vars and try again
  vars.setValue(String("Mama"));

  // "Yo!" ? "Mama"
  assertTrue_1(strEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe5->getValue(tempb));
  assertTrue_1(!tempb);

  // "Mama" ? "Mama"
  assertTrue_1(strEq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strNeq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGe6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe6->getValue(tempb));
  assertTrue_1(tempb);

  // "Yo!" ? "Yo!"
  vars.setValue(String("Yo!"));
  assertTrue_1(strEq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strNeq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe5->getValue(tempb));
  assertTrue_1(tempb);

  // "Yo!" ? "Mama"
  assertTrue_1(strEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGe6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe6->getValue(tempb));
  assertTrue_1(!tempb);

  // "Yo!" ? "Yo mama!"
  vars.setValue(String("Yo Mama!"));
  assertTrue_1(strEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe5->getValue(tempb));
  assertTrue_1(!tempb);

  // "Yo mama!" ? "Mama"
  assertTrue_1(strEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strGe6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(strLt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(strLe6->getValue(tempb));
  assertTrue_1(!tempb);

  delete strLe6;
  delete strLe5;
  delete strLe4;
  delete strLe3;
  delete strLe2;
  delete strLe1;

  delete strLt6;
  delete strLt5;
  delete strLt4;
  delete strLt3;
  delete strLt2;
  delete strLt1;

  delete strGe6;
  delete strGe5;
  delete strGe4;
  delete strGe3;
  delete strGe2;
  delete strGe1;

  delete strGt6;
  delete strGt5;
  delete strGt4;
  delete strGt3;
  delete strGt2;
  delete strGt1;

  delete strNeq6;
  delete strNeq5;
  delete strNeq4;
  delete strNeq3;
  delete strNeq2;
  delete strNeq1;

  delete strEq6;
  delete strEq5;
  delete strEq4;
  delete strEq3;
  delete strEq2;
  delete strEq1;

  return true;
}

static bool testInteger()
{
  bool tempb;

  // Integer
  IntegerConstant won(1);
  IntegerConstant too(2);
  IntegerVariable vari;

  Function *intEq1 = makeFunction(Equal::instance(),
                                  &won, &won,
                                  false, false);
  Function *intEq2 = makeFunction(Equal::instance(),
                                  &too, &too,
                                  false, false);
  Function *intEq3 = makeFunction(Equal::instance(),
                                  &won, &too,
                                  false, false);
  Function *intEq4 = makeFunction(Equal::instance(),
                                  &too, &won,
                                  false, false);
  Function *intEq5 = makeFunction(Equal::instance(),
                                  &won, &vari,
                                  false, false);
  Function *intEq6 = makeFunction(Equal::instance(),
                                  &vari, &too,
                                  false, false);

  Function *intNeq1 = makeFunction(NotEqual::instance(),
                                   &won, &won,
                                   false, false);
  Function *intNeq2 = makeFunction(NotEqual::instance(),
                                   &too, &too,
                                   false, false);
  Function *intNeq3 = makeFunction(NotEqual::instance(),
                                   &won, &too,
                                   false, false);
  Function *intNeq4 = makeFunction(NotEqual::instance(),
                                   &too, &won,
                                   false, false);
  Function *intNeq5 = makeFunction(NotEqual::instance(),
                                   &won, &vari,
                                   false, false);
  Function *intNeq6 = makeFunction(NotEqual::instance(),
                                   &vari, &too,
                                   false, false);

  Function *intGt1 = makeFunction(GreaterThan<Integer>::instance(),
                                  &won, &won,
                                  false, false);
  Function *intGt2 = makeFunction(GreaterThan<Integer>::instance(),
                                  &too, &too,
                                  false, false);
  Function *intGt3 = makeFunction(GreaterThan<Integer>::instance(),
                                  &won, &too,
                                  false, false);
  Function *intGt4 = makeFunction(GreaterThan<Integer>::instance(),
                                  &too, &won,
                                  false, false);
  Function *intGt5 = makeFunction(GreaterThan<Integer>::instance(),
                                  &won, &vari,
                                  false, false);
  Function *intGt6 = makeFunction(GreaterThan<Integer>::instance(),
                                  &vari, &too,
                                  false, false);

  Function *intGe1 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &won, &won,
                                  false, false);
  Function *intGe2 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &too, &too,
                                  false, false);
  Function *intGe3 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &won, &too,
                                  false, false);
  Function *intGe4 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &too, &won,
                                  false, false);
  Function *intGe5 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &won, &vari,
                                  false, false);
  Function *intGe6 = makeFunction(GreaterEqual<Integer>::instance(),
                                  &vari, &too,
                                  false, false);

  Function *intLt1 = makeFunction(LessThan<Integer>::instance(),
                                  &won, &won,
                                  false, false);
  Function *intLt2 = makeFunction(LessThan<Integer>::instance(),
                                  &too, &too,
                                  false, false);
  Function *intLt3 = makeFunction(LessThan<Integer>::instance(),
                                  &won, &too,
                                  false, false);
  Function *intLt4 = makeFunction(LessThan<Integer>::instance(),
                                  &too, &won,
                                  false, false);
  Function *intLt5 = makeFunction(LessThan<Integer>::instance(),
                                  &won, &vari,
                                  false, false);
  Function *intLt6 = makeFunction(LessThan<Integer>::instance(),
                                  &vari, &too,
                                  false, false);

  Function *intLe1 = makeFunction(LessEqual<Integer>::instance(),
                                  &won, &won,
                                  false, false);
  Function *intLe2 = makeFunction(LessEqual<Integer>::instance(),
                                  &too, &too,
                                  false, false);
  Function *intLe3 = makeFunction(LessEqual<Integer>::instance(),
                                  &won, &too,
                                  false, false);
  Function *intLe4 = makeFunction(LessEqual<Integer>::instance(),
                                  &too, &won,
                                  false, false);
  Function *intLe5 = makeFunction(LessEqual<Integer>::instance(),
                                  &won, &vari,
                                  false, false);
  Function *intLe6 = makeFunction(LessEqual<Integer>::instance(),
                                  &vari, &too,
                                  false, false);

  intEq1->activate();
  intEq2->activate();
  intEq3->activate();
  intEq4->activate();
  intEq5->activate();
  intEq6->activate();

  intNeq1->activate();
  intNeq2->activate();
  intNeq3->activate();
  intNeq4->activate();
  intNeq5->activate();
  intNeq6->activate();

  intGt1->activate();
  intGt2->activate();
  intGt3->activate();
  intGt4->activate();
  intGt5->activate();
  intGt6->activate();

  intGe1->activate();
  intGe2->activate();
  intGe3->activate();
  intGe4->activate();
  intGe5->activate();
  intGe6->activate();

  intLt1->activate();
  intLt2->activate();
  intLt3->activate();
  intLt4->activate();
  intLt5->activate();
  intLt6->activate();

  intLe1->activate();
  intLe2->activate();
  intLe3->activate();
  intLe4->activate();
  intLe5->activate();
  intLe6->activate();

  // 1 ? 1
  assertTrue_1(intEq1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intNeq1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe1->getValue(tempb));
  assertTrue_1(tempb);

  // 2 ? 2
  assertTrue_1(intEq2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intNeq2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe2->getValue(tempb));
  assertTrue_1(tempb);

  // 1 ? 2
  assertTrue_1(intEq3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLt3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLe3->getValue(tempb));
  assertTrue_1(tempb);

  // 2 ? 1
  assertTrue_1(intEq4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGe4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe4->getValue(tempb));
  assertTrue_1(!tempb);

  // These should be unknown because vari is uninitialized
  // 1 ? vari
  assertTrue_1(!intEq5->getValue(tempb));
  assertTrue_1(!intNeq5->getValue(tempb));
  assertTrue_1(!intGt5->getValue(tempb));
  assertTrue_1(!intGe5->getValue(tempb));
  assertTrue_1(!intLt5->getValue(tempb));
  assertTrue_1(!intLe5->getValue(tempb));

  // vari ? 2
  assertTrue_1(!intEq6->getValue(tempb));
  assertTrue_1(!intNeq6->getValue(tempb));
  assertTrue_1(!intGt6->getValue(tempb));
  assertTrue_1(!intGe6->getValue(tempb));
  assertTrue_1(!intLt6->getValue(tempb));
  assertTrue_1(!intLe6->getValue(tempb));

  // Set vari and try again
  vari.setValue((Integer) 2);
    
  // 1 ? vari
  assertTrue_1(intEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLe5->getValue(tempb));
  assertTrue_1(tempb);

  // vari ? 2
  assertTrue_1(intEq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intNeq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe6->getValue(tempb));
  assertTrue_1(tempb);

  vari.setValue((Integer) 1);

  // 1 ? vari
  assertTrue_1(intEq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intNeq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe5->getValue(tempb));
  assertTrue_1(tempb);

  // vari ? 2
  assertTrue_1(intEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLe6->getValue(tempb));
  assertTrue_1(tempb);

  vari.setValue((Integer) -1);

  // 1 ? vari
  assertTrue_1(intEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLe5->getValue(tempb));
  assertTrue_1(!tempb);

  // vari ? 2
  assertTrue_1(intEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intGe6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(intLt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(intLe6->getValue(tempb));
  assertTrue_1(tempb);

  delete intLe6;
  delete intLe5;
  delete intLe4;
  delete intLe3;
  delete intLe2;
  delete intLe1;

  delete intLt6;
  delete intLt5;
  delete intLt4;
  delete intLt3;
  delete intLt2;
  delete intLt1;

  delete intGe6;
  delete intGe5;
  delete intGe4;
  delete intGe3;
  delete intGe2;
  delete intGe1;

  delete intGt6;
  delete intGt5;
  delete intGt4;
  delete intGt3;
  delete intGt2;
  delete intGt1;

  delete intNeq6;
  delete intNeq5;
  delete intNeq4;
  delete intNeq3;
  delete intNeq2;
  delete intNeq1;

  delete intEq6;
  delete intEq5;
  delete intEq4;
  delete intEq3;
  delete intEq2;
  delete intEq1;

  return true;
}

static bool testReal()
{
  bool tempb;

  // Real
  RealConstant wontoo(1);
  RealConstant tootoo(2);
  RealVariable varr;

  std::vector<bool> garbage2(2, false);
  std::vector<Expression *> v1, v2, v3, v4, v5, v6;

  Function *dblEq1 = makeFunction(Equal::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblEq2 = makeFunction(Equal::instance(),
                                  &tootoo, &tootoo,
                                  false, false);
  Function *dblEq3 = makeFunction(Equal::instance(),
                                  &wontoo, &tootoo,
                                  false, false);
  Function *dblEq4 = makeFunction(Equal::instance(),
                                  &tootoo, &wontoo,
                                  false, false);
  Function *dblEq5 = makeFunction(Equal::instance(),
                                  &wontoo, &varr,
                                  false, false);
  Function *dblEq6 = makeFunction(Equal::instance(),
                                  &varr, &tootoo,
                                  false, false);

  Function *dblNeq1 = makeFunction(NotEqual::instance(),
                                   &wontoo, &wontoo,
                                   false, false);
  Function *dblNeq2 = makeFunction(NotEqual::instance(),
                                   &tootoo, &tootoo,
                                   false, false);
  Function *dblNeq3 = makeFunction(NotEqual::instance(),
                                   &wontoo, &tootoo,
                                   false, false);
  Function *dblNeq4 = makeFunction(NotEqual::instance(),
                                   &tootoo, &wontoo,
                                   false, false);
  Function *dblNeq5 = makeFunction(NotEqual::instance(),
                                   &wontoo, &varr,
                                   false, false);
  Function *dblNeq6 = makeFunction(NotEqual::instance(),
                                   &varr, &tootoo,
                                   false, false);

  Function *dblGt1 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblGt2 = makeFunction(GreaterThan<Real>::instance(),
                                  &tootoo, &tootoo,
                                  false, false);
  Function *dblGt3 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);
  Function *dblGt4 = makeFunction(GreaterThan<Real>::instance(),
                                  &tootoo, &wontoo,
                                  false, false);
  Function *dblGt5 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &varr,
                                  false, false);
  Function *dblGt6 = makeFunction(GreaterThan<Real>::instance(),
                                  &varr, &tootoo,
                                  false, false);

  Function *dblGe1 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblGe2 = makeFunction(GreaterEqual<Real>::instance(),
                                  &tootoo, &tootoo,
                                  false, false);
  Function *dblGe3 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);
  Function *dblGe4 = makeFunction(GreaterEqual<Real>::instance(),
                                  &tootoo, &wontoo,
                                  false, false);
  Function *dblGe5 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &varr,
                                  false, false);
  Function *dblGe6 = makeFunction(GreaterEqual<Real>::instance(),
                                  &varr, &tootoo,
                                  false, false);

  Function *dblLt1 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblLt2 = makeFunction(LessThan<Real>::instance(),
                                  &tootoo, &tootoo,
                                  false, false);
  Function *dblLt3 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);
  Function *dblLt4 = makeFunction(LessThan<Real>::instance(),
                                  &tootoo, &wontoo,
                                  false, false);
  Function *dblLt5 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &varr,
                                  false, false);
  Function *dblLt6 = makeFunction(LessThan<Real>::instance(),
                                  &varr, &tootoo,
                                  false, false);

  Function *dblLe1 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblLe2 = makeFunction(LessEqual<Real>::instance(),
                                  &tootoo, &tootoo,
                                  false, false);
  Function *dblLe3 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);
  Function *dblLe4 = makeFunction(LessEqual<Real>::instance(),
                                  &tootoo, &wontoo,
                                  false, false);
  Function *dblLe5 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &varr,
                                  false, false);
  Function *dblLe6 = makeFunction(LessEqual<Real>::instance(),
                                  &varr, &tootoo,
                                  false, false);

  dblEq1->activate();
  dblEq2->activate();
  dblEq3->activate();
  dblEq4->activate();
  dblEq5->activate();
  dblEq6->activate();

  dblNeq1->activate();
  dblNeq2->activate();
  dblNeq3->activate();
  dblNeq4->activate();
  dblNeq5->activate();
  dblNeq6->activate();

  dblGt1->activate();
  dblGt2->activate();
  dblGt3->activate();
  dblGt4->activate();
  dblGt5->activate();
  dblGt6->activate();

  dblGe1->activate();
  dblGe2->activate();
  dblGe3->activate();
  dblGe4->activate();
  dblGe5->activate();
  dblGe6->activate();

  dblLt1->activate();
  dblLt2->activate();
  dblLt3->activate();
  dblLt4->activate();
  dblLt5->activate();
  dblLt6->activate();

  dblLe1->activate();
  dblLe2->activate();
  dblLe3->activate();
  dblLe4->activate();
  dblLe5->activate();
  dblLe6->activate();

  // 1 ? 1
  assertTrue_1(dblEq1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe1->getValue(tempb));
  assertTrue_1(tempb);

  // 2 ? 2
  assertTrue_1(dblEq2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe2->getValue(tempb));
  assertTrue_1(tempb);

  // 1 ? 2
  assertTrue_1(dblEq3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe3->getValue(tempb));
  assertTrue_1(tempb);

  // 2 ? 1
  assertTrue_1(dblEq4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGe4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe4->getValue(tempb));
  assertTrue_1(!tempb);

  // These should be unknown because varr is uninitialized
  assertTrue_1(!dblEq5->getValue(tempb));
  assertTrue_1(!dblNeq5->getValue(tempb));
  assertTrue_1(!dblGt5->getValue(tempb));
  assertTrue_1(!dblGe5->getValue(tempb));
  assertTrue_1(!dblLt5->getValue(tempb));
  assertTrue_1(!dblLe5->getValue(tempb));

  assertTrue_1(!dblEq6->getValue(tempb));
  assertTrue_1(!dblNeq6->getValue(tempb));
  assertTrue_1(!dblGt6->getValue(tempb));
  assertTrue_1(!dblGe6->getValue(tempb));
  assertTrue_1(!dblLt6->getValue(tempb));
  assertTrue_1(!dblLe6->getValue(tempb));

  // Set varr and try again
  varr.setValue((Integer) 2);

  // 1 ? 2
  assertTrue_1(dblEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe5->getValue(tempb));
  assertTrue_1(tempb);

  // 2 ? 2
  assertTrue_1(dblEq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe6->getValue(tempb));
  assertTrue_1(tempb);

  varr.setValue((Integer) 1);
  // 1 ? 1
  assertTrue_1(dblEq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe5->getValue(tempb));
  assertTrue_1(tempb);

  // 1 ? 2
  assertTrue_1(dblEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe6->getValue(tempb));
  assertTrue_1(tempb);

  varr.setValue((Integer) -1);
  // 1 ? -1
  assertTrue_1(dblEq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe5->getValue(tempb));
  assertTrue_1(!tempb);

  // -1 ? 2
  assertTrue_1(dblEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe6->getValue(tempb));
  assertTrue_1(tempb);

  delete dblLe6;
  delete dblLe5;
  delete dblLe4;
  delete dblLe3;
  delete dblLe2;
  delete dblLe1;

  delete dblLt6;
  delete dblLt5;
  delete dblLt4;
  delete dblLt3;
  delete dblLt2;
  delete dblLt1;

  delete dblGe6;
  delete dblGe5;
  delete dblGe4;
  delete dblGe3;
  delete dblGe2;
  delete dblGe1;

  delete dblGt6;
  delete dblGt5;
  delete dblGt4;
  delete dblGt3;
  delete dblGt2;
  delete dblGt1;

  delete dblNeq6;
  delete dblNeq5;
  delete dblNeq4;
  delete dblNeq3;
  delete dblNeq2;
  delete dblNeq1;

  delete dblEq6;
  delete dblEq5;
  delete dblEq4;
  delete dblEq3;
  delete dblEq2;
  delete dblEq1;

  return true;
}

static bool testMixedNumerics()
{
  bool tempb;

  RealConstant won(1);
  RealConstant too(2);
  IntegerConstant wontoo(1);
  IntegerConstant tootoo(2);

  Function *dblEq1 = makeFunction(Equal::instance(),
                                  &won, &wontoo,
                                  false, false);
  Function *dblEq2 = makeFunction(Equal::instance(),
                                  &tootoo, &too,
                                  false, false);
  Function *dblEq3 = makeFunction(Equal::instance(),
                                  &wontoo, &too,
                                  false, false);
  Function *dblEq4 = makeFunction(Equal::instance(),
                                  &won, &tootoo,
                                  false, false);
  Function *dblEq5 = makeFunction(Equal::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblEq6 = makeFunction(Equal::instance(),
                                  &wontoo, &tootoo,
                                  false, false);

  Function *dblNeq1 = makeFunction(NotEqual::instance(),
                                   &won, &wontoo,
                                   false, false);
  Function *dblNeq2 = makeFunction(NotEqual::instance(),
                                   &tootoo, &too,
                                   false, false);
  Function *dblNeq3 = makeFunction(NotEqual::instance(),
                                   &wontoo, &too,
                                   false, false);
  Function *dblNeq4 = makeFunction(NotEqual::instance(),
                                   &won, &tootoo,
                                   false, false);
  Function *dblNeq5 = makeFunction(NotEqual::instance(),
                                   &wontoo, &wontoo,
                                   false, false);
  Function *dblNeq6 = makeFunction(NotEqual::instance(),
                                   &wontoo, &tootoo,
                                   false, false);

  Function *dblGt1 = makeFunction(GreaterThan<Real>::instance(),
                                  &won, &wontoo,
                                  false, false);
  Function *dblGt2 = makeFunction(GreaterThan<Real>::instance(),
                                  &tootoo, &too,
                                  false, false);
  Function *dblGt3 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &too,
                                  false, false);
  Function *dblGt4 = makeFunction(GreaterThan<Real>::instance(),
                                  &won, &tootoo,
                                  false, false);
  Function *dblGt5 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblGt6 = makeFunction(GreaterThan<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);

  Function *dblGe1 = makeFunction(GreaterEqual<Real>::instance(),
                                  &won, &wontoo,
                                  false, false);
  Function *dblGe2 = makeFunction(GreaterEqual<Real>::instance(),
                                  &tootoo, &too,
                                  false, false);
  Function *dblGe3 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &too,
                                  false, false);
  Function *dblGe4 = makeFunction(GreaterEqual<Real>::instance(),
                                  &won, &tootoo,
                                  false, false);
  Function *dblGe5 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblGe6 = makeFunction(GreaterEqual<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);

  Function *dblLt1 = makeFunction(LessThan<Real>::instance(),
                                  &won, &wontoo,
                                  false, false);
  Function *dblLt2 = makeFunction(LessThan<Real>::instance(),
                                  &tootoo, &too,
                                  false, false);
  Function *dblLt3 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &too,
                                  false, false);
  Function *dblLt4 = makeFunction(LessThan<Real>::instance(),
                                  &won, &tootoo,
                                  false, false);
  Function *dblLt5 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblLt6 = makeFunction(LessThan<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);

  Function *dblLe1 = makeFunction(LessEqual<Real>::instance(),
                                  &won, &wontoo,
                                  false, false);
  Function *dblLe2 = makeFunction(LessEqual<Real>::instance(),
                                  &tootoo, &too,
                                  false, false);
  Function *dblLe3 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &too,
                                  false, false);
  Function *dblLe4 = makeFunction(LessEqual<Real>::instance(),
                                  &won, &tootoo,
                                  false, false);
  Function *dblLe5 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &wontoo,
                                  false, false);
  Function *dblLe6 = makeFunction(LessEqual<Real>::instance(),
                                  &wontoo, &tootoo,
                                  false, false);

  dblEq1->activate();
  dblEq2->activate();
  dblEq3->activate();
  dblEq4->activate();
  dblEq5->activate();
  dblEq6->activate();

  dblNeq1->activate();
  dblNeq2->activate();
  dblNeq3->activate();
  dblNeq4->activate();
  dblNeq5->activate();
  dblNeq6->activate();

  dblGt1->activate();
  dblGt2->activate();
  dblGt3->activate();
  dblGt4->activate();
  dblGt5->activate();
  dblGt6->activate();

  dblGe1->activate();
  dblGe2->activate();
  dblGe3->activate();
  dblGe4->activate();
  dblGe5->activate();
  dblGe6->activate();

  dblLt1->activate();
  dblLt2->activate();
  dblLt3->activate();
  dblLt4->activate();
  dblLt5->activate();
  dblLt6->activate();

  dblLe1->activate();
  dblLe2->activate();
  dblLe3->activate();
  dblLe4->activate();
  dblLe5->activate();
  dblLe6->activate();

  // Real 1 ? Integer 1
  assertTrue_1(dblEq1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe1->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt1->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe1->getValue(tempb));
  assertTrue_1(tempb);

  // Int 2 ? Real 2
  assertTrue_1(dblEq2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe2->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt2->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe2->getValue(tempb));
  assertTrue_1(tempb);

  // Int 1 ? Real 2
  assertTrue_1(dblEq3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe3->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt3->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe3->getValue(tempb));
  assertTrue_1(tempb);

  // Real 1 ? Int 2
  assertTrue_1(dblEq4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe4->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt4->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe4->getValue(tempb));
  assertTrue_1(tempb);

  // Int 1 ? Int 1
  assertTrue_1(dblEq5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblNeq5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe5->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLt5->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLe5->getValue(tempb));
  assertTrue_1(tempb);

  // Int 1 ? Int 2
  assertTrue_1(dblEq6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblNeq6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblGt6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblGe6->getValue(tempb));
  assertTrue_1(!tempb);
  assertTrue_1(dblLt6->getValue(tempb));
  assertTrue_1(tempb);
  assertTrue_1(dblLe6->getValue(tempb));
  assertTrue_1(tempb);

  delete dblLe6;
  delete dblLe5;
  delete dblLe4;
  delete dblLe3;
  delete dblLe2;
  delete dblLe1;

  delete dblLt6;
  delete dblLt5;
  delete dblLt4;
  delete dblLt3;
  delete dblLt2;
  delete dblLt1;

  delete dblGe6;
  delete dblGe5;
  delete dblGe4;
  delete dblGe3;
  delete dblGe2;
  delete dblGe1;

  delete dblGt6;
  delete dblGt5;
  delete dblGt4;
  delete dblGt3;
  delete dblGt2;
  delete dblGt1;

  delete dblNeq6;
  delete dblNeq5;
  delete dblNeq4;
  delete dblNeq3;
  delete dblNeq2;
  delete dblNeq1;

  delete dblEq6;
  delete dblEq5;
  delete dblEq4;
  delete dblEq3;
  delete dblEq2;
  delete dblEq1;
    
  return true;
}

bool comparisonsTest()
{
  runTest(testIsKnown);
  runTest(testBoolean);
  runTest(testString);
  runTest(testInteger);
  runTest(testReal);
  runTest(testMixedNumerics);
  return true;
}
