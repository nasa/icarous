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

#include "ArithmeticOperators.hh"
#include "Constant.hh"
#include "Function.hh"
#include "TestSupport.hh"
#include "test/TrivialListener.hh"
#include "UserVariable.hh"

using namespace PLEXIL;

//
// Functions and operators
//

template <typename R>
class Passthrough : public OperatorImpl<R>
{
public:
  Passthrough()
    : OperatorImpl<R>("PT")
  {
  }

  ~Passthrough()
  {
  }

  bool checkArgCount(size_t count) const
  {
    return count == 1;
  }

  bool operator()(R &result, Expression const * arg) const
  {
    R temp;
    if (!arg->getValue(temp))
      return false;
    result = temp;
    return true;
  }
};

Passthrough<Boolean> ptb;
Passthrough<Integer> pti;
Passthrough<Real> ptd;
Passthrough<String> pts;

// TODO - test propagation of changes through variable and fn
static bool testUnaryBasics()
{
  {
    BooleanConstant treu(true);
    Function *boule = makeFunction(&ptb, 1);
    boule->setArgument(0, &treu, false);
    assertTrue_1(boule->isKnown());
    boule->activate();
    bool tempb;
    assertTrue_1(boule->getValue(tempb));
    assertTrue_1(tempb == true);
    delete boule;
  }

  {
    IntegerConstant fortytwo(42);
    {
      Function *inty = makeFunction(&pti, 1);
      inty->setArgument(0, &fortytwo, false);
      assertTrue_1(inty->isKnown());
      inty->activate();
      Integer tempi;
      assertTrue_1(inty->getValue(tempi));
      assertTrue_1(tempi == 42);
      delete inty;
    }

    {
      Function *intd = makeFunction(&ptd, 1);
      intd->setArgument(0, &fortytwo, false);
      assertTrue_1(intd->isKnown());
      intd->activate();
      Real tempdi;
      assertTrue_1(intd->getValue(tempdi));
      assertTrue_1(tempdi == 42.0);
      delete intd;
    }
  }

  {
    RealConstant pie(3.14);
    Function *dub = makeFunction(&ptd, 1);
    dub->setArgument(0, &pie, false);
    assertTrue_1(dub->isKnown());
    dub->activate();
    Real tempd;
    assertTrue_1(dub->getValue(tempd));
    assertTrue_1(tempd == 3.14);
    delete dub;
  }

  {
    StringConstant fou("Foo");
    Function *str = makeFunction(&pts, 1);
    str->setArgument(0, &fou, false);
    assertTrue_1(str->isKnown());
    str->activate();
    String temps;
    assertTrue_1(str->getValue(temps));
    assertTrue_1(temps == String("Foo"));
    delete str;
  }

  return true;
}
 
static bool testUnaryPropagation()
{
  {
    BooleanVariable treu(true);
    Function *boule = makeFunction(&ptb, 1);
    boule->setArgument(0, &treu, false);
    bool bchanged = false;
    TrivialListener bl(bchanged);
    boule->addListener(&bl);

    treu.setValue(false);
    assertTrue_1(!bchanged);
    boule->activate();
    assertTrue_1(treu.isActive());

    treu.setValue(false);
    bool boolv;
    assertTrue_1(boule->getValue(boolv));
    assertTrue_1(!boolv);

    treu.setUnknown();
    assertTrue_1(bchanged);

    boule->removeListener(&bl);
    delete boule; // *should* remove listener from treu 
  }

  {
    IntegerVariable fortytwo(42);

    {
      Function *inty = makeFunction(&pti, 1);
      inty->setArgument(0, &fortytwo, false);
      bool ichanged = false;
      TrivialListener il(ichanged);
      inty->addListener(&il);

      fortytwo.setValue((Integer) 43);
      assertTrue_1(!ichanged);
      inty->activate();
      assertTrue_1(fortytwo.isActive());

      fortytwo.setValue((Integer) 43);
      Integer intv;
      assertTrue_1(inty->getValue(intv));
      assertTrue_1(intv == 43);

      fortytwo.setUnknown();
      assertTrue_1(ichanged);

      fortytwo.deactivate();
      inty->removeListener(&il);
      delete inty;
    }

    {
      Function *intd = makeFunction(&ptd, 1);
      intd->setArgument(0, &fortytwo, false);
      bool r2changed = false;
      TrivialListener rl2(r2changed);
      intd->addListener(&rl2);

      fortytwo.setValue((Integer) 43);
      assertTrue_1(!r2changed);
      intd->activate();
      assertTrue_1(fortytwo.isActive());

      fortytwo.setValue((Integer) 43);
      Real dubv;
      assertTrue_1(intd->getValue(dubv));
      assertTrue_1(dubv == 43);

      fortytwo.setUnknown();
      assertTrue_1(r2changed);
      intd->removeListener(&rl2);
      delete intd;
    }
  }

  {
    RealVariable pie(3.14);
    Function *dub = makeFunction(&ptd, 1);
    dub->setArgument(0, &pie, false);
    bool rchanged = false;
    TrivialListener rl(rchanged);
    dub->addListener(&rl);

    pie.setValue(2.718);
    assertTrue_1(!rchanged);
    dub->activate();
    assertTrue_1(pie.isActive());

    pie.setValue(2.718);
    Real dubv;
    assertTrue_1(dub->getValue(dubv));
    assertTrue_1(dubv == 2.718);

    pie.setUnknown();
    assertTrue_1(rchanged);

    dub->removeListener(&rl);
    delete dub;
  }

  {
    StringVariable fou("Foo");
    Function *str = makeFunction(&pts, 1);
    str->setArgument(0, &fou, false);
    bool schanged = false;
    TrivialListener sl(schanged);
    str->addListener(&sl);

    fou.setValue(String("fu"));
    assertTrue_1(!schanged);
    str->activate();
    assertTrue_1(fou.isActive());

    fou.setValue(String("fu"));
    String strv;
    assertTrue_1(str->getValue(strv));
    assertTrue_1(strv == String("fu"));

    fou.setUnknown();
    assertTrue_1(schanged);

    str->removeListener(&sl);
    delete str;
  }

  return true;
}

static bool testBinaryBasics()
{
  {
    Addition<Integer> intAdd;
    IntegerVariable won(1);
    IntegerConstant too(2);
    Function *intFn = makeFunction(&intAdd, 2);
    intFn->setArgument(0, &won, false);
    intFn->setArgument(1, &too, false);
    Integer itemp;
    bool ichanged = false;
    TrivialListener il(ichanged);
    intFn->addListener(&il);

    assertTrue_1(!intFn->isActive());
    assertTrue_1(!won.isActive());
    assertTrue_1(!won.isKnown());
    assertTrue_1(!won.getValue(itemp));
    assertTrue_1(!intFn->isKnown());
    assertTrue_1(!intFn->getValue(itemp));

    intFn->activate();
    assertTrue_1(intFn->isActive());
    assertTrue_1(won.isActive());
    assertTrue_1(won.isKnown());
    assertTrue_1(intFn->isKnown());
    assertTrue_1(won.getValue(itemp));
    assertTrue_1(itemp == 1);
    assertTrue_1(intFn->getValue(itemp));
    assertTrue_1(itemp == 3);
    assertTrue_1(ichanged);

    ichanged = false;
    won.setUnknown();
    assertTrue_1(!won.isKnown());
    assertTrue_1(!won.getValue(itemp));
    assertTrue_1(!intFn->isKnown());
    assertTrue_1(!intFn->getValue(itemp));
    assertTrue_1(ichanged);

    ichanged = false;
    won.deactivate();
    won.activate();
    assertTrue_1(won.isKnown());
    assertTrue_1(won.getValue(itemp));
    assertTrue_1(itemp == 1);
    assertTrue_1(intFn->getValue(itemp));
    assertTrue_1(itemp == 3);
    assertTrue_1(ichanged);

    intFn->removeListener(&il);
    delete intFn;
  }

  {
    Addition<Real> realAdd;
    RealVariable tree(3);
    RealConstant fore(4);
    Function *realFn = makeFunction(&realAdd, 2);
    realFn->setArgument(0, &tree, false);
    realFn->setArgument(1, &fore, false);
    Real rtemp;
    bool rchanged = false;
    TrivialListener rl(rchanged);
    realFn->addListener(&rl);

    assertTrue_1(!realFn->isActive());
    assertTrue_1(!tree.isActive());
    assertTrue_1(!tree.isKnown());
    assertTrue_1(!tree.getValue(rtemp));
    assertTrue_1(!realFn->isKnown());
    assertTrue_1(!realFn->getValue(rtemp));

    realFn->activate();
    assertTrue_1(realFn->isActive());
    assertTrue_1(tree.isActive());

    assertTrue_1(tree.isKnown());
    assertTrue_1(realFn->isKnown());
    assertTrue_1(tree.getValue(rtemp));
    assertTrue_1(rtemp == 3);
    assertTrue_1(realFn->getValue(rtemp));
    assertTrue_1(rtemp == 7);
    assertTrue_1(rchanged);

    rchanged = false;
    tree.setUnknown();
    assertTrue_1(!tree.isKnown());
    assertTrue_1(!tree.getValue(rtemp));
    assertTrue_1(!realFn->isKnown());
    assertTrue_1(!realFn->getValue(rtemp));
    assertTrue_1(rchanged);

    rchanged = false;
    tree.deactivate();
    tree.activate();
    assertTrue_1(tree.isKnown());
    assertTrue_1(realFn->isKnown());
    assertTrue_1(tree.getValue(rtemp));
    assertTrue_1(rtemp == 3);
    assertTrue_1(realFn->getValue(rtemp));
    assertTrue_1(rtemp == 7);
    assertTrue_1(rchanged);

    realFn->removeListener(&rl);
    delete realFn;
  }
  
  return true;
}

static bool testNaryBasics()
{
  const std::vector<Boolean> garbage(3, false);

  {
    Addition<Integer> intAdd;
    IntegerVariable won(1);
    IntegerConstant too(2);
    IntegerVariable tree(3);
    Function *intFn = makeFunction(&intAdd, 3);
    intFn->setArgument(0, &won, false);
    intFn->setArgument(1, &too, false);
    intFn->setArgument(2, &tree, false);
    Integer itemp;
    bool ichanged = false;
    TrivialListener il(ichanged);
    intFn->addListener(&il);

    assertTrue_1(!intFn->isActive());
    assertTrue_1(!won.isActive());
    assertTrue_1(!tree.isActive());
    assertTrue_1(!intFn->isKnown());
    assertTrue_1(!intFn->getValue(itemp));

    intFn->activate();
    assertTrue_1(intFn->isActive());
    assertTrue_1(won.isActive());
    assertTrue_1(tree.isActive());
    assertTrue_1(intFn->isKnown());
    assertTrue_1(intFn->getValue(itemp));
    assertTrue_1(itemp == 6);
    assertTrue_1(ichanged);

    ichanged = false;
    tree.setUnknown();
    assertTrue_1(!tree.isKnown());
    assertTrue_1(!tree.getValue(itemp));
    assertTrue_1(!intFn->isKnown());
    assertTrue_1(!intFn->getValue(itemp));
    assertTrue_1(ichanged);
    ichanged = false;

    tree.deactivate();
    tree.activate();
    assertTrue_1(tree.isKnown());
    assertTrue_1(intFn->isKnown());
    assertTrue_1(tree.getValue(itemp));
    assertTrue_1(itemp == 3);
    assertTrue_1(intFn->getValue(itemp));
    assertTrue_1(itemp == 6);
    assertTrue_1(ichanged);

    intFn->removeListener(&il);
    delete intFn;
  }

  {
    Addition<Real> realAdd;
    RealConstant fore(4);
    RealVariable fivefive(5.5);
    RealVariable sixfive(6.5);
    Function *realFn = makeFunction(&realAdd, 3);
    realFn->setArgument(0, &fore, false);
    realFn->setArgument(1, &fivefive, false);
    realFn->setArgument(2, &sixfive, false);
    Real rtemp;
    bool rchanged = false;
    TrivialListener rl(rchanged);
    realFn->addListener(&rl);

    assertTrue_1(!realFn->isActive());
    assertTrue_1(!fivefive.isActive());
    assertTrue_1(!sixfive.isActive());
    assertTrue_1(!realFn->isKnown());
    assertTrue_1(!realFn->getValue(rtemp));

    realFn->activate();
    assertTrue_1(realFn->isActive());
    assertTrue_1(fivefive.isActive());
    assertTrue_1(sixfive.isActive());
    assertTrue_1(realFn->isKnown());
    assertTrue_1(realFn->getValue(rtemp));
    assertTrue_1(rtemp == 16);
    assertTrue_1(rchanged);

    rchanged = false;
    fivefive.setUnknown();
    assertTrue_1(!fivefive.isKnown());
    assertTrue_1(!fivefive.getValue(rtemp));
    assertTrue_1(!realFn->isKnown());
    assertTrue_1(!realFn->getValue(rtemp));
    assertTrue_1(rchanged);
    rchanged = false;

    // Reset variables, check that values are known and reasonable
    fivefive.deactivate();
    fivefive.activate();
    assertTrue_1(fivefive.isKnown());
    assertTrue_1(realFn->isKnown());
    assertTrue_1(fivefive.getValue(rtemp));
    assertTrue_1(rtemp == 5.5);
    assertTrue_1(realFn->getValue(rtemp));
    assertTrue_1(rtemp == 16);
    assertTrue_1(rchanged);

    realFn->removeListener(&rl);
    delete realFn;
  }

  return true;
}

bool functionsTest()
{
  runTest(testUnaryBasics);
  runTest(testUnaryPropagation);
  runTest(testBinaryBasics);
  runTest(testNaryBasics);
  return true;
}
