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

#include "State.hh"
#include "TestSupport.hh"

using namespace PLEXIL;

static bool testConstructorsAndAccessors()
{
  // Default constructor
  State mt;
  assertTrue_1(mt.name().empty());
  assertTrue_1(mt.parameters().empty());
  assertTrue_1(mt.parameterCount() == 0);
  // Array bounds checking test
  assertTrue_1(!mt.isParameterKnown(1));
  assertTrue_1(mt.parameterType(1) == UNKNOWN_TYPE);

  // Name only
  std::string const foo("Foo");
  State named(foo);
  assertTrue_1(!named.name().empty());
  assertTrue_1(named.parameters().empty());
  assertTrue_1(named.parameterCount() == 0);
  assertTrue_1(named.name() == foo);

  // Name and value
  Value fortytwo((Integer) 42);
  std::string const goo("Goo");
  State nameval(goo, fortytwo);
  assertTrue_1(!nameval.name().empty());
  assertTrue_1(!nameval.parameters().empty());
  assertTrue_1(nameval.parameterCount() == 1);
  assertTrue_1(nameval.name() == goo);
  assertTrue_1(nameval.parameter(0) == fortytwo);

  // Set up parameters
  Value too((Integer) 2);
  Value roo(3.5);
  std::string soo("Soo");
  Value sue(soo);

  State test0("Foo", 3);
  test0.setParameter(0, too);
  test0.setParameter(1, roo);
  test0.setParameter(2, sue);

  assertTrue_1(!test0.name().empty());
  assertTrue_1(test0.name() == foo);
  assertTrue_1(!test0.parameters().empty());
  assertTrue_1(test0.parameterCount() == 3);
  assertTrue_1(test0.parameter(0) == too);
  assertTrue_1(test0.parameter(1) == roo);
  assertTrue_1(test0.parameter(2) == sue);
  assertTrue_1(!test0.isParameterKnown(3));
  assertTrue_1(test0.parameterType(3) == UNKNOWN_TYPE);

  // Name and params
  State test1(foo, 3);
  test1.setParameter(0, too);
  test1.setParameter(1, roo);
  test1.setParameter(2, sue);

  assertTrue_1(!test1.name().empty());
  assertTrue_1(test1.name() == foo);
  assertTrue_1(!test1.parameters().empty());
  assertTrue_1(test1.parameterCount() == 3);
  assertTrue_1(test1.parameter(0) == too);
  assertTrue_1(test1.parameter(1) == roo);
  assertTrue_1(test1.parameter(2) == sue);
  assertTrue_1(!test1.isParameterKnown(3));
  assertTrue_1(test1.parameterType(3) == UNKNOWN_TYPE);

  // Copy
  State mtclone(mt);
  assertTrue_1(mtclone.name().empty());
  assertTrue_1(mtclone.parameters().empty());
  assertTrue_1(mtclone.parameterCount() == 0);
  assertTrue_1(!mtclone.isParameterKnown(1));
  assertTrue_1(mtclone.parameterType(1) == UNKNOWN_TYPE);

  State namedclone(named);
  assertTrue_1(!namedclone.name().empty());
  assertTrue_1(namedclone.parameters().empty());
  assertTrue_1(namedclone.parameterCount() == 0);
  assertTrue_1(namedclone.name() == foo);

  State namevalclone(nameval);
  assertTrue_1(!namevalclone.name().empty());
  assertTrue_1(!namevalclone.parameters().empty());
  assertTrue_1(namevalclone.parameterCount() == 1);
  assertTrue_1(namevalclone.name() == goo);
  assertTrue_1(namevalclone.parameter(0) == fortytwo);

  State test2(test1);
  assertTrue_1(!test2.name().empty());
  assertTrue_1(test2.name() == foo);
  assertTrue_1(!test2.parameters().empty());
  assertTrue_1(test2.parameterCount() == 3);
  assertTrue_1(test2.parameter(0) == too);
  assertTrue_1(test2.parameter(1) == roo);
  assertTrue_1(test2.parameter(2) == sue);
  assertTrue_1(!test2.isParameterKnown(3));
  assertTrue_1(test2.parameterType(3) == UNKNOWN_TYPE);

#if __cplusplus >= 201103L
  // Move
  State mtmove = std::move(mtclone);
  assertTrue_1(mtmove.name().empty());
  assertTrue_1(mtmove.parameters().empty());
  assertTrue_1(mtmove.parameterCount() == 0);
  assertTrue_1(!mtmove.isParameterKnown(1));
  assertTrue_1(mtmove.parameterType(1) == UNKNOWN_TYPE);

  State namedmove = std::move(namedclone);
  assertTrue_1(!namedmove.name().empty());
  assertTrue_1(namedmove.parameters().empty());
  assertTrue_1(namedmove.parameterCount() == 0);
  assertTrue_1(namedmove.name() == foo);

  State namevalmove = std::move(namevalclone);
  assertTrue_1(!namevalmove.name().empty());
  assertTrue_1(!namevalmove.parameters().empty());
  assertTrue_1(namevalmove.parameterCount() == 1);
  assertTrue_1(namevalmove.name() == goo);
  assertTrue_1(namevalmove.parameter(0) == fortytwo);

  State test3 = std::move(test2);
  assertTrue_1(!test3.name().empty());
  assertTrue_1(test3.name() == foo);
  assertTrue_1(!test3.parameters().empty());
  assertTrue_1(test3.parameterCount() == 3);
  assertTrue_1(test3.parameter(0) == too);
  assertTrue_1(test3.parameter(1) == roo);
  assertTrue_1(test3.parameter(2) == sue);
  assertTrue_1(!test3.isParameterKnown(3));
  assertTrue_1(test3.parameterType(3) == UNKNOWN_TYPE);
#endif
  
  return true;
}

static bool testAssignment()
{
  // Default constructor
  State temp;

  // empty
  State mt;
  temp = mt;
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  // Name only
  std::string const foo("Foo");
  State named(foo);
  temp = named;
  assertTrue_1(!temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);
  assertTrue_1(temp.name() == foo);

  // Set empty again
  temp = mt;
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  // Set up parameters
  Value too((int32_t) 2);
  Value roo(3.5);
  std::string soo("Soo");
  Value sue(soo);

  // Name and params
  State test1(foo, 3);
  test1.setParameter(0, too);
  test1.setParameter(1, roo);
  test1.setParameter(2, sue);

  temp = test1;

  assertTrue_1(!temp.name().empty());
  assertTrue_1(temp.name() == foo);
  assertTrue_1(!temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 3);
  assertTrue_1(temp.parameter(0) == too);
  assertTrue_1(temp.parameter(1) == roo);
  assertTrue_1(temp.parameter(2) == sue);

  // Set empty again
  temp = mt;
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  return true;
}

#if __cplusplus >= 201103L
static bool testMoveAssignment()
{
  // Default constructor
  State temp;

  // empty
  temp = std::move(State());
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  // Name only
  std::string const foo("Foo");
  temp = std::move(State(foo));
  assertTrue_1(!temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);
  assertTrue_1(temp.name() == foo);

  // Set empty again
  temp = std::move(State());
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  // Set up parameters
  Value too((int32_t) 2);
  Value roo(3.5);
  std::string soo("Soo");
  Value sue(soo);

  // Name and params
  State test1(foo, 3);
  test1.setParameter(0, too);
  test1.setParameter(1, roo);
  test1.setParameter(2, sue);

  temp = std::move(State(test1));

  assertTrue_1(!temp.name().empty());
  assertTrue_1(temp.name() == foo);
  assertTrue_1(!temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 3);
  assertTrue_1(temp.parameter(0) == too);
  assertTrue_1(temp.parameter(1) == roo);
  assertTrue_1(temp.parameter(2) == sue);

  // Set empty again
  temp = std::move(State());
  assertTrue_1(temp.name().empty());
  assertTrue_1(temp.parameters().empty());
  assertTrue_1(temp.parameterCount() == 0);

  return true;
}
#endif

static bool testEquality()
{
  State mt;
  assertTrue_1(mt == mt);
  assertTrue_1(!(mt != mt));

  State named("Foo");
  assertTrue_1(named == named);
  assertTrue_1(!(named != named));
  assertTrue_1(!(named == mt));
  assertTrue_1(!(mt == named));
  assertTrue_1(named != mt);
  assertTrue_1(mt != named);

  std::string const foo("Foo");
  State named2(foo);
  assertTrue_1(named2 == named2);
  assertTrue_1(!(named2 != named2));
  assertTrue_1(!(named2 == mt));
  assertTrue_1(!(mt == named2));
  assertTrue_1(named2 != mt);
  assertTrue_1(mt != named2);
  assertTrue_1(named2 == named);
  assertTrue_1(named == named2);
  assertTrue_1(!(named2 != named));
  assertTrue_1(!(named != named2));

  State named3("Fop");
  assertTrue_1(named3 == named3);
  assertTrue_1(!(named3 != named3));
  assertTrue_1(!(named3 == mt));
  assertTrue_1(!(mt == named3));
  assertTrue_1(named3 != mt);
  assertTrue_1(mt != named3);
  assertTrue_1(!(named3 == named));
  assertTrue_1(!(named == named3));
  assertTrue_1(named3 != named);
  assertTrue_1(named != named3);
  assertTrue_1(!(named3 == named2));
  assertTrue_1(!(named2 == named3));
  assertTrue_1(named3 != named2);
  assertTrue_1(named2 != named3);

  // Set up parameter vector
  Value too((int32_t) 2);
  Value roo(3.5);
  std::string soo("Soo");
  Value sue(soo);

  // Name and params
  State test1(foo, 3);
  test1.setParameter(0, too);
  test1.setParameter(1, roo);
  test1.setParameter(2, soo);

  assertTrue_1(test1 == test1);
  assertTrue_1(!(test1 != test1));
  assertTrue_1(!(test1 == mt));
  assertTrue_1(!(mt == test1));
  assertTrue_1(test1 != mt);
  assertTrue_1(mt != test1);
  assertTrue_1(!(test1 == named));
  assertTrue_1(!(named == test1));
  assertTrue_1(test1 != named);
  assertTrue_1(named != test1);
  assertTrue_1(!(test1 == named2));
  assertTrue_1(!(named2 == test1));
  assertTrue_1(test1 != named2);
  assertTrue_1(named2 != test1);
  assertTrue_1(!(test1 == named3));
  assertTrue_1(!(named3 == test1));
  assertTrue_1(test1 != named3);
  assertTrue_1(named3 != test1);

  // Clone of test1
  State clone1(test1);
  assertTrue_1(clone1 == clone1);
  assertTrue_1(!(clone1 != clone1));
  assertTrue_1(!(clone1 == mt));
  assertTrue_1(!(mt == clone1));
  assertTrue_1(clone1 != mt);
  assertTrue_1(mt != clone1);
  assertTrue_1(!(clone1 == named));
  assertTrue_1(!(named == clone1));
  assertTrue_1(clone1 != named);
  assertTrue_1(named != clone1);
  assertTrue_1(!(clone1 == named2));
  assertTrue_1(!(named2 == clone1));
  assertTrue_1(clone1 != named2);
  assertTrue_1(named2 != clone1);
  assertTrue_1(!(clone1 == named3));
  assertTrue_1(!(named3 == clone1));
  assertTrue_1(clone1 != named3);
  assertTrue_1(named3 != clone1);
  assertTrue_1(clone1 == test1);
  assertTrue_1(test1 == clone1);
  assertTrue_1(!(clone1 != test1));
  assertTrue_1(!(test1 != clone1));

  State test2(test1);
  Value sop("Sop");
  test2.setParameter(2, sop);

  assertTrue_1(test2 == test2);
  assertTrue_1(!(test2 != test2));
  assertTrue_1(!(test2 == mt));
  assertTrue_1(!(mt == test2));
  assertTrue_1(test2 != mt);
  assertTrue_1(mt != test2);
  assertTrue_1(!(test2 == named));
  assertTrue_1(!(named == test2));
  assertTrue_1(test2 != named);
  assertTrue_1(named != test2);
  assertTrue_1(!(test2 == named2));
  assertTrue_1(!(named2 == test2));
  assertTrue_1(test2 != named2);
  assertTrue_1(named2 != test2);
  assertTrue_1(!(test2 == named3));
  assertTrue_1(!(named3 == test2));
  assertTrue_1(test2 != named3);
  assertTrue_1(named3 != test2);
  assertTrue_1(!(test2 == test1));
  assertTrue_1(!(test1 == test2));
  assertTrue_1(test2 != test1);
  assertTrue_1(test1 != test2);

  return true;
}

static bool testLessThan()
{
  State mt;
  assertTrue_1(!(mt < mt));

  State named("Foo");
  assertTrue_1(!(named < named));
  assertTrue_1(!(named < mt));
  assertTrue_1(mt < named);

  std::string const foo("Foo");
  State named2(foo);
  assertTrue_1(!(named2 < named2));
  assertTrue_1(!(named2 < mt));
  assertTrue_1(mt < named2);
  assertTrue_1(!(named2 < named));
  assertTrue_1(!(named < named2));

  State named3("Fop");
  assertTrue_1(!(named3 < named3));
  assertTrue_1(!(named3 < mt));
  assertTrue_1(mt < named3);
  assertTrue_1(!(named3 < named));
  assertTrue_1(named < named3);
  assertTrue_1(!(named3 < named2));
  assertTrue_1(named2 < named3);

  // Set up parameters
  Value too((int32_t) 2);
  Value roo(3.5);
  std::string soo("Soo");
  Value sue(soo);
  State test1(foo, 3);
  test1.setParameter(0, too);
  test1.setParameter(1, roo);
  test1.setParameter(2, sue);

  assertTrue_1(!(test1 < test1));
  assertTrue_1(!(test1 < mt));
  assertTrue_1(mt < test1);
  assertTrue_1(!(test1 < named));
  assertTrue_1(named < test1);
  assertTrue_1(!(test1 < named2));
  assertTrue_1(named2 < test1);
  assertTrue_1(test1 < named3);
  assertTrue_1(!(named3 < test1));

  State test2(test1);
  Value sop("Sop");
  test2.setParameter(2, sop);

  assertTrue_1(!(test2 < test2));
  assertTrue_1(!(test2 < mt));
  assertTrue_1(mt < test2);
  assertTrue_1(!(test2 < named));
  assertTrue_1(named < test2);
  assertTrue_1(!(test2 < named2));
  assertTrue_1(named2 < test2);
  assertTrue_1(test2 < named3);
  assertTrue_1(!(named3 < test2));
  assertTrue_1(!(test2 < test1));
  assertTrue_1(test1 < test2);

  return true;
}


bool stateTest()
{
  runTest(testConstructorsAndAccessors);
  runTest(testAssignment);
#if __cplusplus >= 201103L
  runTest(testMoveAssignment);
#endif
  runTest(testEquality);
  runTest(testLessThan);

  return true;
}
