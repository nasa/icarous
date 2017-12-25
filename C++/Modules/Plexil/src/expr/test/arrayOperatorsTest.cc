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

#include "ArrayOperators.hh"
#include "ArrayReference.hh"
#include "ArrayVariable.hh"
#include "Comparisons.hh"
#include "Constant.hh"
#include "Error.hh"
#include "Function.hh"
#include "TestSupport.hh"
#include "UserVariable.hh"

using namespace PLEXIL;

static bool testArraySize()
{
  BooleanArrayVariable bav;
  IntegerArrayVariable iav;
  RealArrayVariable rav;
  StringArrayVariable sav;

  ArraySize lop;

  Function *bl = makeFunction(&lop, &bav, false);
  Function *il = makeFunction(&lop, &iav, false);
  Function *rl = makeFunction(&lop, &rav, false);
  Function *sl = makeFunction(&lop, &sav, false);

  Integer len;

  // test inactive
  assertTrue_1(!bl->getValue(len));
  assertTrue_1(!il->getValue(len));
  assertTrue_1(!rl->getValue(len));
  assertTrue_1(!sl->getValue(len));

  bl->activate();
  il->activate();
  rl->activate();
  sl->activate();

  // test uninitialized
  assertTrue_1(!bl->getValue(len));
  assertTrue_1(!il->getValue(len));
  assertTrue_1(!rl->getValue(len));
  assertTrue_1(!sl->getValue(len));

  // Assign empty arrays
  BooleanArrayConstant emptybac(0);
  IntegerArrayConstant emptyiac(0);
  RealArrayConstant emptyrac(0);
  StringArrayConstant emptysac(0);

  bav.setValue(emptybac);
  iav.setValue(emptyiac);
  rav.setValue(emptyrac);
  sav.setValue(emptysac);

  assertTrue_1(bl->getValue(len));
  assertTrue_1(len == 0);
  assertTrue_1(il->getValue(len));
  assertTrue_1(len == 0);
  assertTrue_1(rl->getValue(len));
  assertTrue_1(len == 0);
  assertTrue_1(sl->getValue(len));
  assertTrue_1(len == 0);

  // Assign short (but uninited) arrays
  BooleanArrayConstant shortbac(8);
  IntegerArrayConstant shortiac(8);
  RealArrayConstant shortrac(8);
  StringArrayConstant shortsac(8);

  bav.setValue(shortbac);
  iav.setValue(shortiac);
  rav.setValue(shortrac);
  sav.setValue(shortsac);

  assertTrue_1(bl->getValue(len));
  assertTrue_1(len == 8);
  assertTrue_1(il->getValue(len));
  assertTrue_1(len == 8);
  assertTrue_1(rl->getValue(len));
  assertTrue_1(len == 8);
  assertTrue_1(sl->getValue(len));
  assertTrue_1(len == 8);

  // Deactivate and try again
  bl->deactivate();
  il->deactivate();
  rl->deactivate();
  sl->deactivate();

  assertTrue_1(!bl->getValue(len));
  assertTrue_1(!il->getValue(len));
  assertTrue_1(!rl->getValue(len));
  assertTrue_1(!sl->getValue(len));

  // Reactivate, should revert to unknown
  bl->activate();
  il->activate();
  rl->activate();
  sl->activate();

  assertTrue_1(!bl->getValue(len));
  assertTrue_1(!il->getValue(len));
  assertTrue_1(!rl->getValue(len));
  assertTrue_1(!sl->getValue(len));

  // Assign unknown arrays
  BooleanArrayConstant unknownbac;
  IntegerArrayConstant unknowniac;
  RealArrayConstant unknownrac;
  StringArrayConstant unknownsac;

  bav.setValue(unknownbac);
  iav.setValue(unknowniac);
  rav.setValue(unknownrac);
  sav.setValue(unknownsac);

  assertTrue_1(!bl->getValue(len));
  assertTrue_1(!il->getValue(len));
  assertTrue_1(!rl->getValue(len));
  assertTrue_1(!sl->getValue(len));

  delete sl;
  delete rl;
  delete il;
  delete bl;

  return true;
}

static bool testAllElementsKnown()
{
  BooleanArrayVariable bav;
  IntegerArrayVariable iav;
  RealArrayVariable rav;
  StringArrayVariable sav;

  AllElementsKnown op;
  
  Function *bl = makeFunction(&op, &bav, false);
  Function *il = makeFunction(&op, &iav, false);
  Function *rl = makeFunction(&op, &rav, false);
  Function *sl = makeFunction(&op, &sav, false);

  bool temp;

  // test inactive
  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  bl->activate();
  il->activate();
  rl->activate();
  sl->activate();

  // test uninitialized
  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  // Assign empty arrays
  BooleanArrayConstant emptybac(0);
  IntegerArrayConstant emptyiac(0);
  RealArrayConstant emptyrac(0);
  StringArrayConstant emptysac(0);

  bav.setValue(emptybac);
  iav.setValue(emptyiac);
  rav.setValue(emptyrac);
  sav.setValue(emptysac);

  // *** Boundary case -- see Array.cc ***
  assertTrue_1(bl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(temp);

  // Assign short (but uninited) arrays
  BooleanArrayConstant shortbac(2);
  IntegerArrayConstant shortiac(2);
  RealArrayConstant shortrac(2);
  StringArrayConstant shortsac(2);

  bav.setValue(shortbac);
  iav.setValue(shortiac);
  rav.setValue(shortrac);
  sav.setValue(shortsac);

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(!temp);

  // Assign elements and try again
  IntegerVariable index((int32_t) 0);
  MutableArrayReference bref(&bav, &index);
  MutableArrayReference iref(&iav, &index);
  MutableArrayReference rref(&rav, &index);
  MutableArrayReference sref(&sav, &index);

  bref.activate();
  iref.activate();
  rref.activate();
  sref.activate();

  bref.setValue(false);
  iref.setValue((int32_t) 0);
  rref.setValue(0.0);
  sref.setValue(std::string(""));

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(!temp);

  // Set other element and try again
  index.setValue((int32_t) 1);
  bref.setValue(false);
  iref.setValue((int32_t) 0);
  rref.setValue(0.0);
  sref.setValue(std::string(""));

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(temp);

  // Assign unknown arrays
  BooleanArrayConstant unknownbac;
  IntegerArrayConstant unknowniac;
  RealArrayConstant unknownrac;
  StringArrayConstant unknownsac;

  bav.setValue(unknownbac);
  iav.setValue(unknowniac);
  rav.setValue(unknownrac);
  sav.setValue(unknownsac);

  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  delete sl;
  delete rl;
  delete il;
  delete bl;

  return true;
}

// *** TODO: Implement tests for ArrayMaxSize ***

static bool testAnyElementsKnown()
{
  BooleanArrayVariable bav;
  IntegerArrayVariable iav;
  RealArrayVariable rav;
  StringArrayVariable sav;

  AnyElementsKnown op;

  Function *bl = makeFunction(&op, &bav, false);
  Function *il = makeFunction(&op, &iav, false);
  Function *rl = makeFunction(&op, &rav, false);
  Function *sl = makeFunction(&op, &sav, false);

  bool temp;

  // test inactive
  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  bl->activate();
  il->activate();
  rl->activate();
  sl->activate();

  // test uninitialized
  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  // Assign empty arrays
  BooleanArrayConstant emptybac(0);
  IntegerArrayConstant emptyiac(0);
  RealArrayConstant emptyrac(0);
  StringArrayConstant emptysac(0);

  bav.setValue(emptybac);
  iav.setValue(emptyiac);
  rav.setValue(emptyrac);
  sav.setValue(emptysac);

  // *** Boundary case -- see Array.cc ***
  assertTrue_1(bl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(!temp);

  // Assign short (but uninited) arrays
  BooleanArrayConstant shortbac(2);
  IntegerArrayConstant shortiac(2);
  RealArrayConstant shortrac(2);
  StringArrayConstant shortsac(2);

  bav.setValue(shortbac);
  iav.setValue(shortiac);
  rav.setValue(shortrac);
  sav.setValue(shortsac);

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(!temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(!temp);

  // Assign elements and try again
  IntegerVariable index((int32_t) 0);
  MutableArrayReference bref(&bav, &index);
  MutableArrayReference iref(&iav, &index);
  MutableArrayReference rref(&rav, &index);
  MutableArrayReference sref(&sav, &index);

  bref.activate();
  iref.activate();
  rref.activate();
  sref.activate();

  bref.setValue(false);
  iref.setValue((int32_t) 0);
  rref.setValue(0.0);
  sref.setValue(std::string(""));

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(temp);

  // Set other element and try again
  index.setValue((int32_t) 1);
  bref.setValue(false);
  iref.setValue((int32_t) 0);
  rref.setValue(0.0);
  sref.setValue(std::string(""));

  assertTrue_1(bl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(il->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(rl->getValue(temp));
  assertTrue_1(temp);
  assertTrue_1(sl->getValue(temp));
  assertTrue_1(temp);

  // Assign unknown arrays
  BooleanArrayConstant unknownbac;
  IntegerArrayConstant unknowniac;
  RealArrayConstant unknownrac;
  StringArrayConstant unknownsac;

  bav.setValue(unknownbac);
  iav.setValue(unknowniac);
  rav.setValue(unknownrac);
  sav.setValue(unknownsac);

  assertTrue_1(!bl->getValue(temp));
  assertTrue_1(!il->getValue(temp));
  assertTrue_1(!rl->getValue(temp));
  assertTrue_1(!sl->getValue(temp));

  delete sl;
  delete rl;
  delete il;
  delete bl;

  return true;
}

static bool testBooleanArrayEqual()
{
  Operator const *beq = Equal::instance();
  Operator const *bneq = NotEqual::instance();
  Boolean tempb;

  BooleanArrayConstant unknownbac;
  BooleanArrayConstant emptybac(0);

  // Build a constant array
  BooleanArray ba(3);
  ba.setElement(0, true);
  ba.setElement(1, false);
  // ba[2] is unknown
  BooleanArrayConstant bac3(ba);

  {
    Function *equu = makeFunction(beq,
                                  &unknownbac,
                                  &unknownbac,
                                  false,
                                  false);
    Function *nequu = makeFunction(bneq,
                                   &unknownbac,
                                   &unknownbac,
                                   false,
                                   false);

    Function *eqee = makeFunction(beq,
                                  &emptybac,
                                  &emptybac,
                                  false,
                                  false);
    Function *neqee = makeFunction(bneq,
                                   &emptybac,
                                   &emptybac,
                                   false,
                                   false);

    Function *eq33 = makeFunction(beq,
                                  &bac3,
                                  &bac3,
                                  false,
                                  false);
    Function *neq33 = makeFunction(bneq,
                                   &bac3,
                                   &bac3,
                                   false,
                                   false);

    Function *eque = makeFunction(beq,
                                  &unknownbac,
                                  &emptybac,
                                  false,
                                  false);
    Function *neque = makeFunction(bneq,
                                   &unknownbac,
                                   &emptybac,
                                   false,
                                   false);

    Function *eqeu = makeFunction(beq,
                                  &emptybac,
                                  &unknownbac,
                                  false,
                                  false);
    Function *neqeu = makeFunction(bneq,
                                   &emptybac,
                                   &unknownbac,
                                   false,
                                   false);

    equu->activate();
    nequu->activate();
    eqee->activate();
    neqee->activate();
    eq33->activate();
    neq33->activate();
    eque->activate();
    neque->activate();
    eqeu->activate();
    neqeu->activate();

    // unknown == anything -> unknown
    assertTrue_1(!equu->getValue(tempb));
    assertTrue_1(!nequu->getValue(tempb));
    assertTrue_1(!eque->getValue(tempb));
    assertTrue_1(!neque->getValue(tempb));
    assertTrue_1(!eqeu->getValue(tempb));
    assertTrue_1(!neqeu->getValue(tempb));

    // empty == empty -> true
    assertTrue_1(eqee->getValue(tempb));
    assertTrue_1(tempb);
    // empty != empty -> false
    assertTrue_1(neqee->getValue(tempb));
    assertTrue_1(!tempb);

    // bac3 == bac3 -> true
    assertTrue_1(eq33->getValue(tempb));
    assertTrue_1(tempb);
    // bac3 != bac3 -> false
    assertTrue_1(neq33->getValue(tempb));
    assertTrue_1(!tempb);

    delete equu;
    delete nequu;
    delete eqee;
    delete neqee;
    delete eq33;
    delete neq33;
    delete eque;
    delete neque;
    delete eqeu;
    delete neqeu;
  }

  BooleanArrayVariable bav;

  {
    Function *eqvv = makeFunction(beq,
                                  &bav, &bav,
                                  false, false);
    Function *neqvv = makeFunction(bneq,
                                   &bav, &bav,
                                   false, false);

    Function *equv = makeFunction(beq,
                                  &unknownbac, &bav,
                                  false, false);
    Function *nequv = makeFunction(bneq,
                                   &unknownbac, &bav,
                                   false, false);

    Function *eqvu = makeFunction(beq,
                                  &bav, &unknownbac,
                                  false, false);
    Function *neqvu = makeFunction(bneq,
                                   &bav, &unknownbac,
                                   false, false);

    Function *eqev = makeFunction(beq,
                                  &emptybac, &bav,
                                  false, false);
    Function *neqev = makeFunction(bneq,
                                   &emptybac, &bav,
                                   false, false);

    Function *eqve = makeFunction(beq,
                                  &bav, &emptybac,
                                  false, false);
    Function *neqve = makeFunction(bneq,
                                   &bav, &emptybac,
                                   false, false);

    Function *eq3v = makeFunction(beq,
                                  &bac3, &bav,
                                  false, false);
    Function *neq3v = makeFunction(bneq,
                                   &bac3, &bav,
                                   false, false);

    Function *eqv3 = makeFunction(beq,
                                  &bav, &bac3,
                                  false, false);
    Function *neqv3 = makeFunction(bneq,
                                   &bav, &bac3,
                                   false, false);

    eqvv->activate();
    neqvv->activate();
    equv->activate();
    nequv->activate();
    eqvu->activate();
    neqvu->activate();
    eqev->activate();
    neqev->activate();
    eqve->activate();
    neqve->activate();
    eq3v->activate();
    neq3v->activate();
    eqv3->activate();
    neqv3->activate();

    // bav initially unknown
    // unknown == anything -> unknown
    assertTrue_1(!eqvv->getValue(tempb));
    assertTrue_1(!neqvv->getValue(tempb));
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));
    assertTrue_1(!eqev->getValue(tempb));
    assertTrue_1(!neqev->getValue(tempb));
    assertTrue_1(!eqve->getValue(tempb));
    assertTrue_1(!neqve->getValue(tempb));
    assertTrue_1(!eq3v->getValue(tempb));
    assertTrue_1(!neq3v->getValue(tempb));
    assertTrue_1(!eqv3->getValue(tempb));
    assertTrue_1(!neqv3->getValue(tempb));

    bav.setValue(bac3);

    // bac3 == bac3 -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // bac3 != bac3 -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == non-empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // bac3 == empty -> false
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(!tempb);
    // bac3 != empty -> true
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(tempb);

    // bac3 == bac3 -> true
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(tempb);
    // bac3 != bac3 -> false
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(!tempb);

    bav.setValue(emptybac);

    // Empty == empty -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // Empty == empty -> true
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(!tempb);

    // Empty == non-empty -> false
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(!tempb);
    // Empty != non-empty -> true
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(tempb);

    delete eqvv;
    delete neqvv;
    delete equv;
    delete nequv;
    delete eqvu;
    delete neqvu;
    delete eqev;
    delete neqev;
    delete eqve;
    delete neqve;
    delete eq3v;
    delete neq3v;
    delete eqv3;
    delete neqv3;
  }

  return true;
}

static bool testIntegerArrayEqual()
{
  Operator const *ieq = Equal::instance();
  Operator const *ineq = NotEqual::instance();
  Boolean tempb;

  IntegerArrayConstant unknowniac;
  IntegerArrayConstant emptyiac(0);

  // Build a constant array
  IntegerArray ia(3);
  ia.setElement(0, 24);
  ia.setElement(1, 69);
  // ia[2] is unknown
  IntegerArrayConstant iac3(ia);

  {
    Function *equu = makeFunction(ieq,
                                  &unknowniac,
                                  &unknowniac,
                                  false,
                                  false);
    Function *nequu = makeFunction(ineq,
                                   &unknowniac,
                                   &unknowniac,
                                   false,
                                   false);

    Function *eqee = makeFunction(ieq,
                                  &emptyiac,
                                  &emptyiac,
                                  false,
                                  false);
    Function *neqee = makeFunction(ineq,
                                   &emptyiac,
                                   &emptyiac,
                                   false,
                                   false);

    Function *eq33 = makeFunction(ieq,
                                  &iac3,
                                  &iac3,
                                  false,
                                  false);
    Function *neq33 = makeFunction(ineq,
                                   &iac3,
                                   &iac3,
                                   false,
                                   false);

    Function *eque = makeFunction(ieq,
                                  &unknowniac,
                                  &emptyiac,
                                  false,
                                  false);
    Function *neque = makeFunction(ineq,
                                   &unknowniac,
                                   &emptyiac,
                                   false,
                                   false);

    Function *eqeu = makeFunction(ieq,
                                  &emptyiac,
                                  &unknowniac,
                                  false,
                                  false);
    Function *neqeu = makeFunction(ineq,
                                   &emptyiac,
                                   &unknowniac,
                                   false,
                                   false);

    equu->activate();
    nequu->activate();
    eqee->activate();
    neqee->activate();
    eq33->activate();
    neq33->activate();
    eque->activate();
    neque->activate();
    eqeu->activate();
    neqeu->activate();

    // unknown == anything -> unknown
    assertTrue_1(!equu->getValue(tempb));
    assertTrue_1(!nequu->getValue(tempb));
    assertTrue_1(!eque->getValue(tempb));
    assertTrue_1(!neque->getValue(tempb));
    assertTrue_1(!eqeu->getValue(tempb));
    assertTrue_1(!neqeu->getValue(tempb));

    // empty == empty -> true
    assertTrue_1(eqee->getValue(tempb));
    assertTrue_1(tempb);
    // empty != empty -> false
    assertTrue_1(neqee->getValue(tempb));
    assertTrue_1(!tempb);

    // iac3 == iac3 -> true
    assertTrue_1(eq33->getValue(tempb));
    assertTrue_1(tempb);
    // iac3 != iac3 -> false
    assertTrue_1(neq33->getValue(tempb));
    assertTrue_1(!tempb);

    delete equu;
    delete nequu;
    delete eqee;
    delete neqee;
    delete eq33;
    delete neq33;
    delete eque;
    delete neque;
    delete eqeu;
    delete neqeu;
  }

  IntegerArrayVariable iav;

  {
    Function *eqvv = makeFunction(ieq,
                                  &iav, &iav,
                                  false, false);
    Function *neqvv = makeFunction(ineq,
                                   &iav, &iav,
                                   false, false);

    Function *equv = makeFunction(ieq,
                                  &unknowniac, &iav,
                                  false, false);
    Function *nequv = makeFunction(ineq,
                                   &unknowniac, &iav,
                                   false, false);

    Function *eqvu = makeFunction(ieq,
                                  &iav, &unknowniac,
                                  false, false);
    Function *neqvu = makeFunction(ineq,
                                   &iav, &unknowniac,
                                   false, false);

    Function *eqev = makeFunction(ieq,
                                  &emptyiac, &iav,
                                  false, false);
    Function *neqev = makeFunction(ineq,
                                   &emptyiac, &iav,
                                   false, false);

    Function *eqve = makeFunction(ieq,
                                  &iav, &emptyiac,
                                  false, false);
    Function *neqve = makeFunction(ineq,
                                   &iav, &emptyiac,
                                   false, false);

    Function *eq3v = makeFunction(ieq,
                                  &iac3, &iav,
                                  false, false);
    Function *neq3v = makeFunction(ineq,
                                   &iac3, &iav,
                                   false, false);

    Function *eqv3 = makeFunction(ieq,
                                  &iav, &iac3,
                                  false, false);
    Function *neqv3 = makeFunction(ineq,
                                   &iav, &iac3,
                                   false, false);

    eqvv->activate();
    neqvv->activate();
    equv->activate();
    nequv->activate();
    eqvu->activate();
    neqvu->activate();
    eqev->activate();
    neqev->activate();
    eqve->activate();
    neqve->activate();
    eq3v->activate();
    neq3v->activate();
    eqv3->activate();
    neqv3->activate();

    // iav initially unknown
    // unknown == anything -> unknown
    assertTrue_1(!eqvv->getValue(tempb));
    assertTrue_1(!neqvv->getValue(tempb));
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));
    assertTrue_1(!eqev->getValue(tempb));
    assertTrue_1(!neqev->getValue(tempb));
    assertTrue_1(!eqve->getValue(tempb));
    assertTrue_1(!neqve->getValue(tempb));
    assertTrue_1(!eq3v->getValue(tempb));
    assertTrue_1(!neq3v->getValue(tempb));
    assertTrue_1(!eqv3->getValue(tempb));
    assertTrue_1(!neqv3->getValue(tempb));

    iav.setValue(iac3);

    // iac3 == iac3 -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // iac3 != iac3 -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == non-empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // iac3 == empty -> false
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(!tempb);
    // iac3 != empty -> true
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(tempb);

    // iac3 == iac3 -> true
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(tempb);
    // iac3 != iac3 -> false
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(!tempb);

    iav.setValue(emptyiac);

    // Empty == empty -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // Empty == empty -> true
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(!tempb);

    // Empty == non-empty -> false
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(!tempb);
    // Empty != non-empty -> true
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(tempb);

    delete eqvv;
    delete neqvv;
    delete equv;
    delete nequv;
    delete eqvu;
    delete neqvu;
    delete eqev;
    delete neqev;
    delete eqve;
    delete neqve;
    delete eq3v;
    delete neq3v;
    delete eqv3;
    delete neqv3;
  }

  return true;
}

static bool testRealArrayEqual()
{
  Operator const *req = Equal::instance();
  Operator const *rneq = NotEqual::instance();
  Boolean tempb;

  RealArrayConstant unknownrac;
  RealArrayConstant emptyrac(0);

  // Build a constant array
  RealArray ra(3);
  ra.setElement(0, 2.718);
  ra.setElement(1, 3.1416);
  // ra[2] is unknown
  RealArrayConstant rac3(ra);

  {
    Function *equu = makeFunction(req,
                                  &unknownrac,
                                  &unknownrac,
                                  false,
                                  false);
    Function *nequu = makeFunction(rneq,
                                   &unknownrac,
                                   &unknownrac,
                                   false,
                                   false);

    Function *eqee = makeFunction(req,
                                  &emptyrac,
                                  &emptyrac,
                                  false,
                                  false);
    Function *neqee = makeFunction(rneq,
                                   &emptyrac,
                                   &emptyrac,
                                   false,
                                   false);

    Function *eq33 = makeFunction(req,
                                  &rac3,
                                  &rac3,
                                  false,
                                  false);
    Function *neq33 = makeFunction(rneq,
                                   &rac3,
                                   &rac3,
                                   false,
                                   false);

    Function *eque = makeFunction(req,
                                  &unknownrac,
                                  &emptyrac,
                                  false,
                                  false);
    Function *neque = makeFunction(rneq,
                                   &unknownrac,
                                   &emptyrac,
                                   false,
                                   false);

    Function *eqeu = makeFunction(req,
                                  &emptyrac,
                                  &unknownrac,
                                  false,
                                  false);
    Function *neqeu = makeFunction(rneq,
                                   &emptyrac,
                                   &unknownrac,
                                   false,
                                   false);

    equu->activate();
    nequu->activate();
    eqee->activate();
    neqee->activate();
    eq33->activate();
    neq33->activate();
    eque->activate();
    neque->activate();
    eqeu->activate();
    neqeu->activate();

    // unknown == anything -> unknown
    assertTrue_1(!equu->getValue(tempb));
    assertTrue_1(!nequu->getValue(tempb));
    assertTrue_1(!eque->getValue(tempb));
    assertTrue_1(!neque->getValue(tempb));
    assertTrue_1(!eqeu->getValue(tempb));
    assertTrue_1(!neqeu->getValue(tempb));

    // empty == empty -> true
    assertTrue_1(eqee->getValue(tempb));
    assertTrue_1(tempb);
    // empty != empty -> false
    assertTrue_1(neqee->getValue(tempb));
    assertTrue_1(!tempb);

    // rac3 == rac3 -> true
    assertTrue_1(eq33->getValue(tempb));
    assertTrue_1(tempb);
    // rac3 != rac3 -> false
    assertTrue_1(neq33->getValue(tempb));
    assertTrue_1(!tempb);

    delete equu;
    delete nequu;
    delete eqee;
    delete neqee;
    delete eq33;
    delete neq33;
    delete eque;
    delete neque;
    delete eqeu;
    delete neqeu;
  }

  RealArrayVariable rav;

  {
    Function *eqvv = makeFunction(req,
                                  &rav, &rav,
                                  false, false);
    Function *neqvv = makeFunction(rneq,
                                   &rav, &rav,
                                   false, false);

    Function *equv = makeFunction(req,
                                  &unknownrac, &rav,
                                  false, false);
    Function *nequv = makeFunction(rneq,
                                   &unknownrac, &rav,
                                   false, false);

    Function *eqvu = makeFunction(req,
                                  &rav, &unknownrac,
                                  false, false);
    Function *neqvu = makeFunction(rneq,
                                   &rav, &unknownrac,
                                   false, false);

    Function *eqev = makeFunction(req,
                                  &emptyrac, &rav,
                                  false, false);
    Function *neqev = makeFunction(rneq,
                                   &emptyrac, &rav,
                                   false, false);

    Function *eqve = makeFunction(req,
                                  &rav, &emptyrac,
                                  false, false);
    Function *neqve = makeFunction(rneq,
                                   &rav, &emptyrac,
                                   false, false);

    Function *eq3v = makeFunction(req,
                                  &rac3, &rav,
                                  false, false);
    Function *neq3v = makeFunction(rneq,
                                   &rac3, &rav,
                                   false, false);

    Function *eqv3 = makeFunction(req,
                                  &rav, &rac3,
                                  false, false);
    Function *neqv3 = makeFunction(rneq,
                                   &rav, &rac3,
                                   false, false);

    eqvv->activate();
    neqvv->activate();
    equv->activate();
    nequv->activate();
    eqvu->activate();
    neqvu->activate();
    eqev->activate();
    neqev->activate();
    eqve->activate();
    neqve->activate();
    eq3v->activate();
    neq3v->activate();
    eqv3->activate();
    neqv3->activate();

    // rav initially unknown
    // unknown == anything -> unknown
    assertTrue_1(!eqvv->getValue(tempb));
    assertTrue_1(!neqvv->getValue(tempb));
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));
    assertTrue_1(!eqev->getValue(tempb));
    assertTrue_1(!neqev->getValue(tempb));
    assertTrue_1(!eqve->getValue(tempb));
    assertTrue_1(!neqve->getValue(tempb));
    assertTrue_1(!eq3v->getValue(tempb));
    assertTrue_1(!neq3v->getValue(tempb));
    assertTrue_1(!eqv3->getValue(tempb));
    assertTrue_1(!neqv3->getValue(tempb));

    rav.setValue(rac3);

    // rac3 == rac3 -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // rac3 != rac3 -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == non-empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // rac3 == empty -> false
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(!tempb);
    // rac3 != empty -> true
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(tempb);

    // rac3 == rac3 -> true
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(tempb);
    // rac3 != rac3 -> false
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(!tempb);

    rav.setValue(emptyrac);

    // Empty == empty -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // Empty == empty -> true
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(!tempb);

    // Empty == non-empty -> false
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(!tempb);
    // Empty != non-empty -> true
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(tempb);

    delete eqvv;
    delete neqvv;
    delete equv;
    delete nequv;
    delete eqvu;
    delete neqvu;
    delete eqev;
    delete neqev;
    delete eqve;
    delete neqve;
    delete eq3v;
    delete neq3v;
    delete eqv3;
    delete neqv3;
  }

  return true;
}

static bool testStringArrayEqual()
{
  Operator const *seq = Equal::instance();
  Operator const *sneq = NotEqual::instance();
  Boolean tempb;

  StringArrayConstant unknownsac;
  StringArrayConstant emptysac(0);

  // Build a constant array
  StringArray sa(3);
  sa.setElement(0, String("Foo"));
  sa.setElement(1, String("bar"));
  // sa[2] is unknown
  StringArrayConstant sac3(sa);

  {
    Function *equu = makeFunction(seq,
                                  &unknownsac,
                                  &unknownsac,
                                  false,
                                  false);
    Function *nequu = makeFunction(sneq,
                                   &unknownsac,
                                   &unknownsac,
                                   false,
                                   false);

    Function *eqee = makeFunction(seq,
                                  &emptysac,
                                  &emptysac,
                                  false,
                                  false);
    Function *neqee = makeFunction(sneq,
                                   &emptysac,
                                   &emptysac,
                                   false,
                                   false);

    Function *eq33 = makeFunction(seq,
                                  &sac3,
                                  &sac3,
                                  false,
                                  false);
    Function *neq33 = makeFunction(sneq,
                                   &sac3,
                                   &sac3,
                                   false,
                                   false);

    Function *eque = makeFunction(seq,
                                  &unknownsac,
                                  &emptysac,
                                  false,
                                  false);
    Function *neque = makeFunction(sneq,
                                   &unknownsac,
                                   &emptysac,
                                   false,
                                   false);

    Function *eqeu = makeFunction(seq,
                                  &emptysac,
                                  &unknownsac,
                                  false,
                                  false);
    Function *neqeu = makeFunction(sneq,
                                   &emptysac,
                                   &unknownsac,
                                   false,
                                   false);

    equu->activate();
    nequu->activate();
    eqee->activate();
    neqee->activate();
    eq33->activate();
    neq33->activate();
    eque->activate();
    neque->activate();
    eqeu->activate();
    neqeu->activate();

    // unknown == anything -> unknown
    assertTrue_1(!equu->getValue(tempb));
    assertTrue_1(!nequu->getValue(tempb));
    assertTrue_1(!eque->getValue(tempb));
    assertTrue_1(!neque->getValue(tempb));
    assertTrue_1(!eqeu->getValue(tempb));
    assertTrue_1(!neqeu->getValue(tempb));

    // empty == empty -> true
    assertTrue_1(eqee->getValue(tempb));
    assertTrue_1(tempb);
    // empty != empty -> false
    assertTrue_1(neqee->getValue(tempb));
    assertTrue_1(!tempb);

    // sac3 == sac3 -> true
    assertTrue_1(eq33->getValue(tempb));
    assertTrue_1(tempb);
    // sac3 != sac3 -> false
    assertTrue_1(neq33->getValue(tempb));
    assertTrue_1(!tempb);

    delete equu;
    delete nequu;
    delete eqee;
    delete neqee;
    delete eq33;
    delete neq33;
    delete eque;
    delete neque;
    delete eqeu;
    delete neqeu;
  }

  StringArrayVariable sav;

  {
    Function *eqvv = makeFunction(seq,
                                  &sav, &sav,
                                  false, false);
    Function *neqvv = makeFunction(sneq,
                                   &sav, &sav,
                                   false, false);

    Function *equv = makeFunction(seq,
                                  &unknownsac, &sav,
                                  false, false);
    Function *nequv = makeFunction(sneq,
                                   &unknownsac, &sav,
                                   false, false);

    Function *eqvu = makeFunction(seq,
                                  &sav, &unknownsac,
                                  false, false);
    Function *neqvu = makeFunction(sneq,
                                   &sav, &unknownsac,
                                   false, false);

    Function *eqev = makeFunction(seq,
                                  &emptysac, &sav,
                                  false, false);
    Function *neqev = makeFunction(sneq,
                                   &emptysac, &sav,
                                   false, false);

    Function *eqve = makeFunction(seq,
                                  &sav, &emptysac,
                                  false, false);
    Function *neqve = makeFunction(sneq,
                                   &sav, &emptysac,
                                   false, false);

    Function *eq3v = makeFunction(seq,
                                  &sac3, &sav,
                                  false, false);
    Function *neq3v = makeFunction(sneq,
                                   &sac3, &sav,
                                   false, false);

    Function *eqv3 = makeFunction(seq,
                                  &sav, &sac3,
                                  false, false);
    Function *neqv3 = makeFunction(sneq,
                                   &sav, &sac3,
                                   false, false);

    eqvv->activate();
    neqvv->activate();
    equv->activate();
    nequv->activate();
    eqvu->activate();
    neqvu->activate();
    eqev->activate();
    neqev->activate();
    eqve->activate();
    neqve->activate();
    eq3v->activate();
    neq3v->activate();
    eqv3->activate();
    neqv3->activate();

    // sav initially unknown
    // unknown == anything -> unknown
    assertTrue_1(!eqvv->getValue(tempb));
    assertTrue_1(!neqvv->getValue(tempb));
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));
    assertTrue_1(!eqev->getValue(tempb));
    assertTrue_1(!neqev->getValue(tempb));
    assertTrue_1(!eqve->getValue(tempb));
    assertTrue_1(!neqve->getValue(tempb));
    assertTrue_1(!eq3v->getValue(tempb));
    assertTrue_1(!neq3v->getValue(tempb));
    assertTrue_1(!eqv3->getValue(tempb));
    assertTrue_1(!neqv3->getValue(tempb));

    sav.setValue(sac3);

    // sac3 == sac3 -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // sac3 != sac3 -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == non-empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // sac3 == empty -> false
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(!tempb);
    // sac3 != empty -> true
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(tempb);

    // sac3 == sac3 -> true
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(tempb);
    // sac3 != sac3 -> false
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(!tempb);

    sav.setValue(emptysac);

    // Empty == empty -> true
    assertTrue_1(eqvv->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqvv->getValue(tempb));
    assertTrue_1(!tempb);

    // unknown == empty -> unknown
    assertTrue_1(!equv->getValue(tempb));
    assertTrue_1(!nequv->getValue(tempb));
    assertTrue_1(!eqvu->getValue(tempb));
    assertTrue_1(!neqvu->getValue(tempb));

    // Empty == empty -> true
    assertTrue_1(eqev->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(eqve->getValue(tempb));
    assertTrue_1(tempb);
    // Empty != empty -> false
    assertTrue_1(neqev->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(neqve->getValue(tempb));
    assertTrue_1(!tempb);

    // Empty == non-empty -> false
    assertTrue_1(eq3v->getValue(tempb));
    assertTrue_1(!tempb);
    assertTrue_1(eqv3->getValue(tempb));
    assertTrue_1(!tempb);
    // Empty != non-empty -> true
    assertTrue_1(neq3v->getValue(tempb));
    assertTrue_1(tempb);
    assertTrue_1(neqv3->getValue(tempb));
    assertTrue_1(tempb);

    delete eqvv;
    delete neqvv;
    delete equv;
    delete nequv;
    delete eqvu;
    delete neqvu;
    delete eqev;
    delete neqev;
    delete eqve;
    delete neqve;
    delete eq3v;
    delete neq3v;
    delete eqv3;
    delete neqv3;
  }

  return true;
}

bool arrayOperatorsTest()
{
  runTest(testArraySize);
  runTest(testAllElementsKnown);
  runTest(testAnyElementsKnown);
  runTest(testBooleanArrayEqual);
  runTest(testIntegerArrayEqual);
  runTest(testRealArrayEqual);
  runTest(testStringArrayEqual);
  return true;
}
