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

#include "Constant.hh"
#include "Function.hh"
#include "StringOperators.hh"
#include "TestSupport.hh"
#include "UserVariable.hh"

using namespace PLEXIL;

static bool testStringLength()
{
  StringVariable var; // initially unknown

  Function *strlen =
    makeFunction(StringLength::instance(), &var, false);
  Integer result;

  strlen->activate(); // will also activate var

  // unknown
  assertTrue_1(!strlen->getValue(result));

  // empty
  var.setValue("");
  assertTrue_1(strlen->getValue(result));
  assertTrue_1(result == 0);

  // arbitrary contents
  var.setValue("now is the time");
  assertTrue_1(strlen->getValue(result));
  assertTrue_1(result == 15);

  delete strlen;

  return true;
}

static bool testStringConcat()
{
  StringConstant foo(std::string("foo!"));
  StringVariable bar, baz, bletch;
  std::string result, result2;

  {
    // Unary function of constant
    Function *fooConc = makeFunction(StringConcat::instance(),
                                     &foo, false);
    fooConc->activate();
    assertTrue_1(fooConc->getValue(result));
    assertTrue_1(foo.getValue(result2));
    assertTrue_1(result == result2);

    delete fooConc;
  }

  {
    // Unary of unint'ed variable
    Function *barConc = makeFunction(StringConcat::instance(),
                                     &bar, false);
    barConc->activate();
    assertTrue_1(!barConc->getValue(result));

    // Set variable
    bar.setValue(std::string(" bar?"));
    assertTrue_1(barConc->getValue(result));
    assertTrue_1(bar.getValue(result2));
    assertTrue_1(result == result2);

    delete barConc;
  }
  
  {
    // Binary of constant, variable
    Function *fooBazConc = makeFunction(StringConcat::instance(),
                                        &foo, &baz,
                                        false, false);
    fooBazConc->activate();
    assertTrue_1(!fooBazConc->getValue(result));

    // Set baz to empty string
    baz.setValue(std::string(""));
    assertTrue_1(fooBazConc->getValue(result));
    assertTrue_1(foo.getValue(result2));
    assertTrue_1(result == result2);

    // Set baz non-empty
    baz.setValue(std::string(" bazzz"));
    assertTrue_1(fooBazConc->getValue(result));
    assertTrue_1(result == std::string("foo! bazzz"));

    delete fooBazConc;
  }

  {
    // N-ary
    Function *nConc = makeFunction(StringConcat::instance(), 4);
    nConc->setArgument(0, &foo, false);
    nConc->setArgument(1, &bar, false);
    nConc->setArgument(2, &baz, false);
    nConc->setArgument(3, &bletch, false);
    nConc->activate();

    // bletch unknown
    assertTrue_1(!nConc->getValue(result));

    // set bletch
    bletch.setValue(std::string(" BLETCH."));
    assertTrue_1(nConc->getValue(result));
    assertTrue_1(result == std::string("foo! bar? bazzz BLETCH."));

    delete nConc;
  }
  return true;
}

bool stringTest()
{
  runTest(testStringLength);
  runTest(testStringConcat);
  return true;
}
