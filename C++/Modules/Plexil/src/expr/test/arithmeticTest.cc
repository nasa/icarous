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

#include "ArithmeticOperators.hh"
#include "Function.hh"
#include "Constant.hh"
#include "TestSupport.hh"
#include "UserVariable.hh"

using namespace PLEXIL;

// Work both two-arg and three-arg versions
bool additionTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant won(1);
  IntegerConstant tree(3);
  IntegerVariable i;

  // Unary not implemented in schema

  {
    // Binary

    Addition<Integer> addi;
    Integer tempi;

    // Set up functions
    Function *iadd2_1 = makeFunction(&addi, 2);
    iadd2_1->setArgument(0, &m1, false);
    iadd2_1->setArgument(1, &tree, false);

    Function *iadd2_2 = makeFunction(&addi, 2);
    iadd2_2->setArgument(0, &i, false);
    iadd2_2->setArgument(1, &won, false);

    Function *iadd2_3 = makeFunction(&addi, 2);
    iadd2_3->setArgument(0, &tree, false);
    iadd2_3->setArgument(1, &i, false);

    iadd2_1->activate();
    iadd2_2->activate();
    iadd2_3->activate();

    assertTrue_1(iadd2_1->getValue(tempi));
    assertTrue_1(tempi == 2);

    // Should be unknown because i not initialized yet
    assertTrue_1(!iadd2_2->getValue(tempi));
    assertTrue_1(!iadd2_3->getValue(tempi));

    i.setValue((Integer) 5);
    // should be known now
    assertTrue_1(iadd2_2->getValue(tempi));
    assertTrue_1(tempi == 6);
    assertTrue_1(iadd2_3->getValue(tempi));
    assertTrue_1(tempi == 8);

    // N-ary

    Function *iadd3_1 = makeFunction(&addi, 3);
    iadd3_1->setArgument(0, &m1, false);
    iadd3_1->setArgument(1, &won, false);
    iadd3_1->setArgument(2, &tree, false);

    Function *iadd3_2 = makeFunction(&addi, 3);
    iadd3_2->setArgument(0, &won, false);
    iadd3_2->setArgument(1, &tree, false);
    iadd3_2->setArgument(2, &i, false);

    Function *iadd3_3 = makeFunction(&addi, 3);
    iadd3_3->setArgument(0, &i, false);
    iadd3_3->setArgument(1, &m1, false);
    iadd3_3->setArgument(2, &won, false);

    iadd3_1->activate();
    iadd3_2->activate();
    iadd3_3->activate();
  
    i.setUnknown();

    assertTrue_1(iadd3_1->getValue(tempi));
    assertTrue_1(tempi == 3);

    // Should be unknown because i unknown
    assertTrue_1(!iadd3_2->getValue(tempi));
    assertTrue_1(!iadd3_3->getValue(tempi));

    i.setValue((Integer) 27);
    // should be known now
    assertTrue_1(iadd3_2->getValue(tempi));
    assertTrue_1(tempi == 31);
    assertTrue_1(iadd3_3->getValue(tempi));
    assertTrue_1(tempi == 27);

    // Integer operator conversion to real
    Real tempr;

    assertTrue_1(iadd2_1->getValue(tempr));
    assertTrue_1(tempr == 2.0);
    assertTrue_1(iadd2_2->getValue(tempr));
    assertTrue_1(tempr == 28.0);
    assertTrue_1(iadd2_3->getValue(tempr));
    assertTrue_1(tempr == 30.0);

    assertTrue_1(iadd3_1->getValue(tempr));
    assertTrue_1(tempr == 3.0);
    assertTrue_1(iadd3_2->getValue(tempr));
    assertTrue_1(tempr == 31.0);
    assertTrue_1(iadd3_3->getValue(tempr));
    assertTrue_1(tempr == 27.0);

    delete iadd3_3;
    delete iadd3_2;
    delete iadd3_1;
    delete iadd2_3;
    delete iadd2_2;
    delete iadd2_1;
  }

  // Real
  RealConstant too(2.5);
  RealConstant fore(4.5);
  RealVariable x;
  Addition<Real> addr;
  Real tempr;

  // Unary not supported in schema

  {
    // Binary

    // Set up arglists
    Function *radd2_1 = makeFunction(&addr, 2);
    radd2_1->setArgument(0, &too, false);
    radd2_1->setArgument(1, &fore, false);

    Function *radd2_2 = makeFunction(&addr, 2);
    radd2_2->setArgument(0, &x, false);
    radd2_2->setArgument(1, &too, false);

    Function *radd2_3 = makeFunction(&addr, 2);
    radd2_3->setArgument(0, &fore, false);
    radd2_3->setArgument(1, &x, false);

    radd2_1->activate();
    radd2_2->activate();
    radd2_3->activate();

    assertTrue_1(radd2_1->getValue(tempr));
    assertTrue_1(tempr == 7.0);

    // Should be unknown because x not initialized yet
    assertTrue_1(!radd2_2->getValue(tempr));
    assertTrue_1(!radd2_3->getValue(tempr));

    x.setValue(-0.5);
    // should be known now
    assertTrue_1(radd2_2->getValue(tempr));
    assertTrue_1(tempr == 2.0);
    assertTrue_1(radd2_3->getValue(tempr));
    assertTrue_1(tempr == 4.0);

    delete radd2_3;
    delete radd2_2;
    delete radd2_1;
  }

  {
    // N-ary
    Function *radd3_1 = makeFunction(&addr, 3);
    radd3_1->setArgument(0, &too, false);
    radd3_1->setArgument(1, &fore, false);
    radd3_1->setArgument(2, &too, false);

    Function *radd3_2 = makeFunction(&addr, 3);
    radd3_2->setArgument(0, &x, false);
    radd3_2->setArgument(1, &too, false);
    radd3_2->setArgument(2, &fore, false);

    Function *radd3_3 = makeFunction(&addr, 3);
    radd3_3->setArgument(0, &too, false);
    radd3_3->setArgument(1, &too, false);
    radd3_3->setArgument(2, &x, false);

    radd3_1->activate();
    radd3_2->activate();
    radd3_3->activate();
  
    x.setUnknown();

    assertTrue_1(radd3_1->getValue(tempr));
    assertTrue_1(tempr == 9.5);

    // Should be unknown because x unknown
    assertTrue_1(!radd3_2->getValue(tempr));
    assertTrue_1(!radd3_3->getValue(tempr));

    x.setValue(3.25);
    // should be known now
    assertTrue_1(radd3_2->getValue(tempr));
    assertTrue_1(tempr == 10.25);
    assertTrue_1(radd3_3->getValue(tempr));
    assertTrue_1(tempr == 8.25);

    delete radd3_3;
    delete radd3_2;
    delete radd3_1;
  }

  // Mixed numeric

  {
    // Binary

    // Set up arglists
    Function *madd2_1 = makeFunction(&addr, 2);
    madd2_1->setArgument(0, &too, false);
    madd2_1->setArgument(1, &tree, false);

    Function *madd2_2 = makeFunction(&addr, 2);
    madd2_2->setArgument(0, &i, false);
    madd2_2->setArgument(1, &too, false);

    Function *madd2_3 = makeFunction(&addr, 2);
    madd2_3->setArgument(0, &tree, false);
    madd2_3->setArgument(1, &x, false);

    madd2_1->activate();
    madd2_2->activate();
    madd2_3->activate();
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(madd2_1->getValue(tempr));
    assertTrue_1(tempr == 5.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!madd2_2->getValue(tempr));
    assertTrue_1(!madd2_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(madd2_2->getValue(tempr));
    assertTrue_1(tempr == 44.5);
    assertTrue_1(madd2_3->getValue(tempr));
    assertTrue_1(tempr == 2.5);

    delete madd2_3;
    delete madd2_2;
    delete madd2_1;
  }

  // N-ary
  {
    Function *madd3_1 = makeFunction(&addr, 3);
    madd3_1->setArgument(0, &tree, false);
    madd3_1->setArgument(1, &fore, false);
    madd3_1->setArgument(2, &m1, false);

    Function *madd3_2 = makeFunction(&addr, 3);
    madd3_2->setArgument(0, &x, false);
    madd3_2->setArgument(1, &won, false);
    madd3_2->setArgument(2, &fore, false);

    Function *madd3_3 = makeFunction(&addr, 3);
    madd3_3->setArgument(0, &tree, false);
    madd3_3->setArgument(1, &too, false);
    madd3_3->setArgument(2, &i, false);

    madd3_1->activate();
    madd3_2->activate();
    madd3_3->activate();
  
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(madd3_1->getValue(tempr));
    assertTrue_1(tempr == 6.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!madd3_2->getValue(tempr));
    assertTrue_1(!madd3_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(madd3_2->getValue(tempr));
    assertTrue_1(tempr == 5.0);
    assertTrue_1(madd3_3->getValue(tempr));
    assertTrue_1(tempr == 47.5);

    delete madd3_3;
    delete madd3_2;
    delete madd3_1;
  }
  return true;
}

bool subtractionTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant won(1);
  IntegerConstant tree(3);
  IntegerVariable i;

  {
    Subtraction<Integer> subi;
    Integer tempi;

    {
      // Unary
      Function *isub1_1 = makeFunction(&subi, 1);
      isub1_1->setArgument(0, &m1, false);
    
      Function *isub1_2 = makeFunction(&subi, 1);
      isub1_2->setArgument(0, &tree, false);

      Function *isub1_3 = makeFunction(&subi, 1);
      isub1_3->setArgument(0, &i, false);

      isub1_1->activate();
      isub1_2->activate();
      isub1_3->activate();

      assertTrue_1(isub1_1->getValue(tempi));
      assertTrue_1(tempi == 1);
      assertTrue_1(isub1_2->getValue(tempi));
      assertTrue_1(tempi == -3);

      // Should be unknown because i not initialized yet
      assertTrue_1(!isub1_3->getValue(tempi));

      i.setValue((Integer) 7);
      // should be known now
      assertTrue_1(isub1_3->getValue(tempi));
      assertTrue_1(tempi == -7);

      // Integer operator to real result
      Real tempr;

      assertTrue_1(isub1_1->getValue(tempr));
      assertTrue_1(tempr == 1.0);
      assertTrue_1(isub1_2->getValue(tempr));
      assertTrue_1(tempr == -3.0);
      assertTrue_1(isub1_3->getValue(tempr));
      assertTrue_1(tempr == -7.0);

      delete isub1_3;
      delete isub1_2;
      delete isub1_1;
    }

    {
      // Binary
      Function *isub2_1 = makeFunction(&subi, 2);
      isub2_1->setArgument(0, &m1, false);
      isub2_1->setArgument(1, &tree, false);

      Function *isub2_2 = makeFunction(&subi, 2);
      isub2_2->setArgument(0, &i, false);
      isub2_2->setArgument(1, &won, false);

      Function *isub2_3 = makeFunction(&subi, 2);
      isub2_3->setArgument(0, &tree, false);
      isub2_3->setArgument(1, &i, false);

      isub2_1->activate();
      isub2_2->activate();
      isub2_3->activate();

      i.setUnknown();
  
      assertTrue_1(isub2_1->getValue(tempi));
      assertTrue_1(tempi == -4);

      // Should be unknown because i unknown
      assertTrue_1(!isub2_2->getValue(tempi));
      assertTrue_1(!isub2_3->getValue(tempi));

      i.setValue((Integer) 5);
      // should be known now
      assertTrue_1(isub2_2->getValue(tempi));
      assertTrue_1(tempi == 4);
      assertTrue_1(isub2_3->getValue(tempi));
      assertTrue_1(tempi == -2);

      delete isub2_3;
      delete isub2_2;
      delete isub2_1;
    }

    {
      // N-ary (not supported in schema)

      // Set up arglists
      Function *isub3_1 = makeFunction(&subi, 3);
      isub3_1->setArgument(0, &m1, false);
      isub3_1->setArgument(1, &won, false);
      isub3_1->setArgument(2, &tree, false);

      Function *isub3_2 = makeFunction(&subi, 3);
      isub3_2->setArgument(0, &won, false);
      isub3_2->setArgument(1, &tree, false);
      isub3_2->setArgument(2, &i, false);

      Function *isub3_3 = makeFunction(&subi, 3);
      isub3_3->setArgument(0, &i, false);
      isub3_3->setArgument(1, &m1, false);
      isub3_3->setArgument(2, &won, false);

      isub3_1->activate();
      isub3_2->activate();
      isub3_3->activate();
  
      i.setUnknown();

      assertTrue_1(isub3_1->getValue(tempi));
      assertTrue_1(tempi == -5);

      // Should be unknown because i unknown
      assertTrue_1(!isub3_2->getValue(tempi));
      assertTrue_1(!isub3_3->getValue(tempi));

      i.setValue((Integer) 27);
      // should be known now
      assertTrue_1(isub3_2->getValue(tempi));
      assertTrue_1(tempi == -29);
      assertTrue_1(isub3_3->getValue(tempi));
      assertTrue_1(tempi == 27);

      delete isub3_3;
      delete isub3_2;
      delete isub3_1;
    }
  }

    // Real
    RealConstant too(2.5);
    RealConstant fore(4.5);
    RealVariable x;
    Subtraction<Real> subr;
    Real tempr;

    {
      // Unary
      Function *rsub1_1 = makeFunction(&subr, 1);
      rsub1_1->setArgument(0, &too, false);

      Function *rsub1_2 = makeFunction(&subr, 1);
      rsub1_2->setArgument(0, &fore, false);

      Function *rsub1_3 = makeFunction(&subr, 1);
      rsub1_3->setArgument(0, &x, false);

      rsub1_1->activate();
      rsub1_2->activate();
      rsub1_3->activate();

      assertTrue_1(rsub1_1->getValue(tempr));
      assertTrue_1(tempr == -2.5);
      assertTrue_1(rsub1_2->getValue(tempr));
      assertTrue_1(tempr == -4.5);

      // Should be unknown because x not initialized yet
      assertTrue_1(!rsub1_3->getValue(tempr));

      x.setValue(1.5);
      // should be known now
      assertTrue_1(rsub1_3->getValue(tempr));
      assertTrue_1(tempr == -1.5);

      delete rsub1_3;
      delete rsub1_2;
      delete rsub1_1;
    }

    {
      // Binary
      Function *rsub2_1 = makeFunction(&subr, 2);
      rsub2_1->setArgument(0, &too, false);
      rsub2_1->setArgument(1, &fore, false);

      Function *rsub2_2 = makeFunction(&subr, 2);
      rsub2_2->setArgument(0, &x, false);
      rsub2_2->setArgument(1, &too, false);

      Function *rsub2_3 = makeFunction(&subr, 2);
      rsub2_3->setArgument(0, &fore, false);
      rsub2_3->setArgument(1, &x, false);

      rsub2_1->activate();
      rsub2_2->activate();
      rsub2_3->activate();
  
      x.setUnknown();

      assertTrue_1(rsub2_1->getValue(tempr));
      assertTrue_1(tempr == -2.0);

      // Should be unknown because x unknown
      assertTrue_1(!rsub2_2->getValue(tempr));
      assertTrue_1(!rsub2_3->getValue(tempr));

      x.setValue(-0.5);
      // should be known now
      assertTrue_1(rsub2_2->getValue(tempr));
      assertTrue_1(tempr == -3.0);
      assertTrue_1(rsub2_3->getValue(tempr));
      assertTrue_1(tempr == 5.0);

      delete rsub2_3;
      delete rsub2_2;
      delete rsub2_1;
    }

    {
      // N-ary (not supported in schema)
      Function *rsub3_1 = makeFunction(&subr, 3);
      rsub3_1->setArgument(0, &too, false);
      rsub3_1->setArgument(1, &fore, false);
      rsub3_1->setArgument(2, &too, false);

      Function *rsub3_2 = makeFunction(&subr, 3);
      rsub3_2->setArgument(0, &x, false);
      rsub3_2->setArgument(1, &too, false);
      rsub3_2->setArgument(2, &fore, false);

      Function *rsub3_3 = makeFunction(&subr, 3);
      rsub3_3->setArgument(0, &too, false);
      rsub3_3->setArgument(1, &too, false);
      rsub3_3->setArgument(2, &x, false);

      rsub3_1->activate();
      rsub3_2->activate();
      rsub3_3->activate();
  
      x.setUnknown();

      assertTrue_1(rsub3_1->getValue(tempr));
      assertTrue_1(tempr == -4.5);

      // Should be unknown because x unknown
      assertTrue_1(!rsub3_2->getValue(tempr));
      assertTrue_1(!rsub3_3->getValue(tempr));

      x.setValue(3.25);
      // should be known now
      assertTrue_1(rsub3_2->getValue(tempr));
      assertTrue_1(tempr == -3.75);
      assertTrue_1(rsub3_3->getValue(tempr));
      assertTrue_1(tempr == -3.25);

      delete rsub3_3;
      delete rsub3_2;
      delete rsub3_1;
    }

    // Mixed numeric

    {
      // Unary
      Function *msub1_1 = makeFunction(&subr, 1);
      msub1_1->setArgument(0, &m1, false);

      Function *msub1_2 = makeFunction(&subr, 1);
      msub1_2->setArgument(0, &tree, false);

      Function *msub1_3 = makeFunction(&subr, 1);
      msub1_3->setArgument(0, &i, false);

      msub1_1->activate();
      msub1_2->activate();
      msub1_3->activate();

      i.setUnknown();

      assertTrue_1(msub1_1->getValue(tempr));
      assertTrue_1(tempr == 1);
      assertTrue_1(msub1_2->getValue(tempr));
      assertTrue_1(tempr == -3);

      // Should be unknown because i unknown
      assertTrue_1(!msub1_3->getValue(tempr));

      i.setValue((Integer) 7);
      // should be known now
      assertTrue_1(msub1_3->getValue(tempr));
      assertTrue_1(tempr == -7);

      delete msub1_3;
      delete msub1_2;
      delete msub1_1;
    }
    
    {
      // Binary
      Function *msub2_1 = makeFunction(&subr, 2);
      msub2_1->setArgument(0, &too, false);
      msub2_1->setArgument(1, &tree, false);

      Function *msub2_2 = makeFunction(&subr, 2);
      msub2_2->setArgument(0, &i, false);
      msub2_2->setArgument(1, &too, false);

      Function *msub2_3 = makeFunction(&subr, 2);
      msub2_3->setArgument(0, &tree, false);
      msub2_3->setArgument(1, &x, false);

      msub2_1->activate();
      msub2_2->activate();
      msub2_3->activate();
      i.setUnknown();
      x.setUnknown();

      assertTrue_1(msub2_1->getValue(tempr));
      assertTrue_1(tempr == -0.5);

      // Should be unknown because i, x unknown
      assertTrue_1(!msub2_2->getValue(tempr));
      assertTrue_1(!msub2_3->getValue(tempr));

      i.setValue((Integer) 42);
      x.setValue(-0.5);
      // should be known now
      assertTrue_1(msub2_2->getValue(tempr));
      assertTrue_1(tempr == 39.5);
      assertTrue_1(msub2_3->getValue(tempr));
      assertTrue_1(tempr == 3.5);

      delete msub2_3;
      delete msub2_2;
      delete msub2_1;
    }

    {
      // N-ary (not supported in schema)
      Function *msub3_1 = makeFunction(&subr, 3);
      msub3_1->setArgument(0, &tree, false);
      msub3_1->setArgument(1, &fore, false);
      msub3_1->setArgument(2, &m1, false);

      Function *msub3_2 = makeFunction(&subr, 3);
      msub3_2->setArgument(0, &x, false);
      msub3_2->setArgument(1, &won, false);
      msub3_2->setArgument(2, &fore, false);

      Function *msub3_3 = makeFunction(&subr, 3);
      msub3_3->setArgument(0, &tree, false);
      msub3_3->setArgument(1, &too, false);
      msub3_3->setArgument(2, &i, false);

      msub3_1->activate();
      msub3_2->activate();
      msub3_3->activate();
  
      i.setUnknown();
      x.setUnknown();

      assertTrue_1(msub3_1->getValue(tempr));
      assertTrue_1(tempr == -0.5);

      // Should be unknown because i, x unknown
      assertTrue_1(!msub3_2->getValue(tempr));
      assertTrue_1(!msub3_3->getValue(tempr));

      i.setValue((Integer) 42);
      x.setValue(-0.5);
      // should be known now
      assertTrue_1(msub3_2->getValue(tempr));
      assertTrue_1(tempr == -6.0);
      assertTrue_1(msub3_3->getValue(tempr));
      assertTrue_1(tempr == -41.5);

      delete msub3_3;
      delete msub3_2;
      delete msub3_1;
    }

    return true;
  }

bool multiplicationTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant six(6);
  IntegerConstant tree(3);
  IntegerVariable i;

  {
    Multiplication<Integer> muli;
    Integer tempi;

    // Unary not supported in schema

    {
      // Binary
      Function *imul2_1 = makeFunction(&muli, 2);
      imul2_1->setArgument(0, &m1, false);
      imul2_1->setArgument(1, &tree, false);

      Function *imul2_2 = makeFunction(&muli, 2);
      imul2_2->setArgument(0, &i, false);
      imul2_2->setArgument(1, &six, false);

      Function *imul2_3 = makeFunction(&muli, 2);
      imul2_3->setArgument(0, &tree, false);
      imul2_3->setArgument(1, &i, false);

      imul2_1->activate();
      imul2_2->activate();
      imul2_3->activate();

      assertTrue_1(imul2_1->getValue(tempi));
      assertTrue_1(tempi == -3);

      // Should be unknown because i not initialized yet
      assertTrue_1(!imul2_2->getValue(tempi));
      assertTrue_1(!imul2_3->getValue(tempi));

      i.setValue((Integer) 5);
      // should be known now
      assertTrue_1(imul2_2->getValue(tempi));
      assertTrue_1(tempi == 30);
      assertTrue_1(imul2_3->getValue(tempi));
      assertTrue_1(tempi == 15);

      delete imul2_3;
      delete imul2_2;
      delete imul2_1;
    }

    {
      // N-ary

      // Set up arglists
      Function *imul3_1 = makeFunction(&muli, 3);
      imul3_1->setArgument(0, &m1, false);
      imul3_1->setArgument(1, &six, false);
      imul3_1->setArgument(2, &tree, false);

      Function *imul3_2 = makeFunction(&muli, 3);
      imul3_2->setArgument(0, &six, false);
      imul3_2->setArgument(1, &tree, false);
      imul3_2->setArgument(2, &i, false);

      Function *imul3_3 = makeFunction(&muli, 3);
      imul3_3->setArgument(0, &i, false);
      imul3_3->setArgument(1, &m1, false);
      imul3_3->setArgument(2, &six, false);

      imul3_1->activate();
      imul3_2->activate();
      imul3_3->activate();
  
      i.setUnknown();

      assertTrue_1(imul3_1->getValue(tempi));
      assertTrue_1(tempi == -18);

      // Should be unknown because i unknown
      assertTrue_1(!imul3_2->getValue(tempi));
      assertTrue_1(!imul3_3->getValue(tempi));

      i.setValue((Integer) 2);
      // should be known now
      assertTrue_1(imul3_2->getValue(tempi));
      assertTrue_1(tempi == 36);
      assertTrue_1(imul3_3->getValue(tempi));
      assertTrue_1(tempi == -12);

      delete imul3_3;
      delete imul3_2;
      delete imul3_1;
    }
  }

  // Real
  RealConstant too(2.5);
  RealConstant fore(4);
  RealVariable x;
  Multiplication<Real> mulr;
  Real tempr;

  // Unary not supported in schema

  {
    // Binary
    Function *rmul2_1 = makeFunction(&mulr, 2);
    rmul2_1->setArgument(0, &too, false);
    rmul2_1->setArgument(1, &fore, false);

    Function *rmul2_2 = makeFunction(&mulr, 2);
    rmul2_2->setArgument(0, &x, false);
    rmul2_2->setArgument(1, &too, false);

    Function *rmul2_3 = makeFunction(&mulr, 2);
    rmul2_3->setArgument(0, &fore, false);
    rmul2_3->setArgument(1, &x, false);

    rmul2_1->activate();
    rmul2_2->activate();
    rmul2_3->activate();

    assertTrue_1(rmul2_1->getValue(tempr));
    assertTrue_1(tempr == 10);

    // Should be unknown because x not initialized yet
    assertTrue_1(!rmul2_2->getValue(tempr));
    assertTrue_1(!rmul2_3->getValue(tempr));

    x.setValue(-0.5);
    // should be known now
    assertTrue_1(rmul2_2->getValue(tempr));
    assertTrue_1(tempr == -1.25);
    assertTrue_1(rmul2_3->getValue(tempr));
    assertTrue_1(tempr == -2.0);

    delete rmul2_3;
    delete rmul2_2;
    delete rmul2_1;
  }

  {
    // N-ary
    Function *rmul3_1 = makeFunction(&mulr, 3);
    rmul3_1->setArgument(0, &too, false);
    rmul3_1->setArgument(1, &fore, false);
    rmul3_1->setArgument(2, &too, false);

    Function *rmul3_2 = makeFunction(&mulr, 3);
    rmul3_2->setArgument(0, &x, false);
    rmul3_2->setArgument(1, &too, false);
    rmul3_2->setArgument(2, &fore, false);

    Function *rmul3_3 = makeFunction(&mulr, 3);
    rmul3_3->setArgument(0, &too, false);
    rmul3_3->setArgument(1, &too, false);
    rmul3_3->setArgument(2, &x, false);

    rmul3_1->activate();
    rmul3_2->activate();
    rmul3_3->activate();
  
    x.setUnknown();

    assertTrue_1(rmul3_1->getValue(tempr));
    assertTrue_1(tempr == 25);

    // Should be unknown because x unknown
    assertTrue_1(!rmul3_2->getValue(tempr));
    assertTrue_1(!rmul3_3->getValue(tempr));

    x.setValue((Integer) 8);
    // should be known now
    assertTrue_1(rmul3_2->getValue(tempr));
    assertTrue_1(tempr == 80);
    assertTrue_1(rmul3_3->getValue(tempr));
    assertTrue_1(tempr == 50);

    delete rmul3_3;
    delete rmul3_2;
    delete rmul3_1;
  }

  // Mixed numeric

  // Unary not supported in schema

  {
    // Binary
    Function *mmul2_1 = makeFunction(&mulr, 2);
    mmul2_1->setArgument(0, &too, false);
    mmul2_1->setArgument(1, &tree, false);

    Function *mmul2_2 = makeFunction(&mulr, 2);
    mmul2_2->setArgument(0, &i, false);
    mmul2_2->setArgument(1, &too, false);

    Function *mmul2_3 = makeFunction(&mulr, 2);
    mmul2_3->setArgument(0, &tree, false);
    mmul2_3->setArgument(1, &x, false);

    mmul2_1->activate();
    mmul2_2->activate();
    mmul2_3->activate();
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmul2_1->getValue(tempr));
    assertTrue_1(tempr == 7.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmul2_2->getValue(tempr));
    assertTrue_1(!mmul2_3->getValue(tempr));

    i.setValue((Integer) 2);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmul2_2->getValue(tempr));
    assertTrue_1(tempr == 5);
    assertTrue_1(mmul2_3->getValue(tempr));
    assertTrue_1(tempr == -1.5);

    delete mmul2_3;
    delete mmul2_2;
    delete mmul2_1;
  }

  {
    // N-ary
    Function *mmul3_1 = makeFunction(&mulr, 3);
    mmul3_1->setArgument(0, &tree, false);
    mmul3_1->setArgument(1, &fore, false);
    mmul3_1->setArgument(2, &m1, false);

    Function *mmul3_2 = makeFunction(&mulr, 3);
    mmul3_2->setArgument(0, &x, false);
    mmul3_2->setArgument(1, &six, false);
    mmul3_2->setArgument(2, &fore, false);

    Function *mmul3_3 = makeFunction(&mulr, 3);
    mmul3_3->setArgument(0, &tree, false);
    mmul3_3->setArgument(1, &too, false);
    mmul3_3->setArgument(2, &i, false);

    mmul3_1->activate();
    mmul3_2->activate();
    mmul3_3->activate();
  
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmul3_1->getValue(tempr));
    assertTrue_1(tempr == -12);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmul3_2->getValue(tempr));
    assertTrue_1(!mmul3_3->getValue(tempr));

    i.setValue((Integer) 2);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmul3_2->getValue(tempr));
    assertTrue_1(tempr == -12);
    assertTrue_1(mmul3_3->getValue(tempr));
    assertTrue_1(tempr == 15);

    delete mmul3_3;
    delete mmul3_2;
    delete mmul3_1;
  }
  
  return true;
}

bool divisionTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant five(5);
  IntegerConstant tree(3);
  IntegerVariable i;

  {
    Division<Integer> divi;
    Integer tempi;

    // Binary
    Function *idiv2_1 = makeFunction(&divi, 2);
    idiv2_1->setArgument(0, &five, false);
    idiv2_1->setArgument(1, &tree, false);

    Function *idiv2_2 = makeFunction(&divi, 2);
    idiv2_2->setArgument(0, &i, false);
    idiv2_2->setArgument(1, &five, false);

    Function *idiv2_3 = makeFunction(&divi, 2);
    idiv2_3->setArgument(0, &tree, false);
    idiv2_3->setArgument(1, &i, false);

    idiv2_1->activate();
    idiv2_2->activate();
    idiv2_3->activate();

    assertTrue_1(idiv2_1->getValue(tempi));
    assertTrue_1(tempi == 1);

    // Should be unknown because i not initialized yet
    assertTrue_1(!idiv2_2->getValue(tempi));
    assertTrue_1(!idiv2_3->getValue(tempi));

    i.setValue((Integer) 2);
    // should be known now
    assertTrue_1(idiv2_2->getValue(tempi));
    assertTrue_1(tempi == 0);
    assertTrue_1(idiv2_3->getValue(tempi));
    assertTrue_1(tempi == 1);

    delete idiv2_3;
    delete idiv2_2;
    delete idiv2_1;
  }

  // Real
  RealConstant too(2.5);
  RealConstant fore(4);
  RealVariable x;
  Division<Real> divr;
  Real tempr;

  {
    // Binary
    Function *rdiv2_1 = makeFunction(&divr, 2);
    rdiv2_1->setArgument(0, &fore, false);
    rdiv2_1->setArgument(1, &too, false);

    Function *rdiv2_2 = makeFunction(&divr, 2);
    rdiv2_2->setArgument(0, &x, false);
    rdiv2_2->setArgument(1, &fore, false);

    Function *rdiv2_3 = makeFunction(&divr, 2);
    rdiv2_3->setArgument(0, &too, false);
    rdiv2_3->setArgument(1, &x, false);

    rdiv2_1->activate();
    rdiv2_2->activate();
    rdiv2_3->activate();

    assertTrue_1(rdiv2_1->getValue(tempr));
    assertTrue_1(tempr == 1.6);

    // Should be unknown because x not initialized yet
    assertTrue_1(!rdiv2_2->getValue(tempr));
    assertTrue_1(!rdiv2_3->getValue(tempr));

    x.setValue(-1.25);
    // should be known now
    assertTrue_1(rdiv2_2->getValue(tempr));
    assertTrue_1(tempr == -0.3125);
    assertTrue_1(rdiv2_3->getValue(tempr));
    assertTrue_1(tempr == -2);

    delete rdiv2_3;
    delete rdiv2_2;
    delete rdiv2_1;
  }

  // Mixed

  {
    // Binary
    Function *mdiv2_1 = makeFunction(&divr, 2);
    mdiv2_1->setArgument(0, &too, false);
    mdiv2_1->setArgument(1, &five, false);

    Function *mdiv2_2 = makeFunction(&divr, 2);
    mdiv2_2->setArgument(0, &i, false);
    mdiv2_2->setArgument(1, &too, false);

    Function *mdiv2_3 = makeFunction(&divr, 2);
    mdiv2_3->setArgument(0, &tree, false);
    mdiv2_3->setArgument(1, &x, false);

    mdiv2_1->activate();
    mdiv2_2->activate();
    mdiv2_3->activate();
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mdiv2_1->getValue(tempr));
    assertTrue_1(tempr == 0.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!mdiv2_2->getValue(tempr));
    assertTrue_1(!mdiv2_3->getValue(tempr));

    i.setValue((Integer) 5);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mdiv2_2->getValue(tempr));
    assertTrue_1(tempr == 2);
    assertTrue_1(mdiv2_3->getValue(tempr));
    assertTrue_1(tempr == -6);

    delete mdiv2_3;
    delete mdiv2_2;
    delete mdiv2_1;
  }

  return true;
}

bool moduloTest()
{
  // Integer
  IntegerConstant too(2);
  IntegerConstant tree(3);
  IntegerConstant nein(9);
  IntegerVariable i;

  {
    // Integer
    Modulo<Integer> modi;
    Integer tempi;

    Function *imod2_1 = makeFunction(&modi, 2);
    imod2_1->setArgument(0, &tree, false);
    imod2_1->setArgument(1, &too, false);

    Function *imod2_2 = makeFunction(&modi, 2);
    imod2_2->setArgument(0, &i, false);
    imod2_2->setArgument(1, &tree, false);

    Function *imod2_3 = makeFunction(&modi, 2);
    imod2_3->setArgument(0, &nein, false);
    imod2_3->setArgument(1, &i, false);

    imod2_1->activate();
    imod2_2->activate();
    imod2_3->activate();

    assertTrue_1(imod2_1->getValue(tempi));
    assertTrue_1(tempi == 1);

    // Not yet known
    assertTrue_1(!imod2_2->getValue(tempi));
    assertTrue_1(!imod2_3->getValue(tempi));

    i.setValue((Integer) 5);
    assertTrue_1(imod2_2->getValue(tempi));
    assertTrue_1(tempi == 2);
    assertTrue_1(imod2_3->getValue(tempi));
    assertTrue_1(tempi == 4);

    delete imod2_3;
    delete imod2_2;
    delete imod2_1;
  }

  // Real
  RealConstant tue(2);
  RealConstant three(3);
  RealConstant nin(9);
  RealVariable x;
  Modulo<Real> modd;
  Real tempd;

  {
    Function *dmod2_1 = makeFunction(&modd, 2);
    dmod2_1->setArgument(0, &three, false);
    dmod2_1->setArgument(1, &tue, false);

    Function *dmod2_2 = makeFunction(&modd, 2);
    dmod2_2->setArgument(0, &x, false);
    dmod2_2->setArgument(1, &three, false);

    Function *dmod2_3 = makeFunction(&modd, 2);
    dmod2_3->setArgument(0, &nin, false);
    dmod2_3->setArgument(1, &x, false);

    dmod2_1->activate();
    dmod2_2->activate();
    dmod2_3->activate();

    assertTrue_1(dmod2_1->getValue(tempd));
    assertTrue_1(tempd == 1);

    // Not yet known
    assertTrue_1(!dmod2_2->getValue(tempd));
    assertTrue_1(!dmod2_3->getValue(tempd));

    x.setValue((Integer) 5);
    assertTrue_1(dmod2_2->getValue(tempd));
    assertTrue_1(tempd == 2);
    assertTrue_1(dmod2_3->getValue(tempd));
    assertTrue_1(tempd == 4);

    delete dmod2_3;
    delete dmod2_2;
    delete dmod2_1;
  }
  // TODO: Mixed

  return true;
}

bool minimumTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant won(1);
  IntegerConstant tree(3);
  IntegerVariable i;

  {
    Minimum<Integer> mini;
    Integer tempi;

    // Unary not supported in schema

    {
      // Binary
      Function *imin2_1 = makeFunction(&mini, 2);
      imin2_1->setArgument(0, &m1, false);
      imin2_1->setArgument(1, &tree, false);

      Function *imin2_2 = makeFunction(&mini, 2);
      imin2_2->setArgument(0, &i, false);
      imin2_2->setArgument(1, &won, false);

      Function *imin2_3 = makeFunction(&mini, 2);
      imin2_3->setArgument(0, &tree, false);
      imin2_3->setArgument(1, &i, false);

      imin2_1->activate();
      imin2_2->activate();
      imin2_3->activate();

      assertTrue_1(imin2_1->getValue(tempi));
      assertTrue_1(tempi == -1);

      // Should be unknown because i not initialized yet
      assertTrue_1(!imin2_2->getValue(tempi));
      assertTrue_1(!imin2_3->getValue(tempi));

      i.setValue((Integer) 5);
      // should be known now
      assertTrue_1(imin2_2->getValue(tempi));
      assertTrue_1(tempi == 1);
      assertTrue_1(imin2_3->getValue(tempi));
      assertTrue_1(tempi == 3);

      delete imin2_3;
      delete imin2_2;
      delete imin2_1;
    }
  
    {
      // N-ary (not supported in schema)

      // Set up arglists
      Function *imin3_1 = makeFunction(&mini, 3);
      imin3_1->setArgument(0, &m1, false);
      imin3_1->setArgument(1, &won, false);
      imin3_1->setArgument(2, &tree, false);

      Function *imin3_2 = makeFunction(&mini, 3);
      imin3_2->setArgument(0, &won, false);
      imin3_2->setArgument(1, &tree, false);
      imin3_2->setArgument(2, &i, false);

      Function *imin3_3 = makeFunction(&mini, 3);
      imin3_3->setArgument(0, &i, false);
      imin3_3->setArgument(1, &m1, false);
      imin3_3->setArgument(2, &won, false);

      imin3_1->activate();
      imin3_2->activate();
      imin3_3->activate();
  
      i.setUnknown();

      assertTrue_1(imin3_1->getValue(tempi));
      assertTrue_1(tempi == -1);

      // Should be unknown because i unknown
      assertTrue_1(!imin3_2->getValue(tempi));
      assertTrue_1(!imin3_3->getValue(tempi));

      i.setValue((Integer) 27);
      // should be known now
      assertTrue_1(imin3_2->getValue(tempi));
      assertTrue_1(tempi == 1);
      assertTrue_1(imin3_3->getValue(tempi));
      assertTrue_1(tempi == -1);

      delete imin3_3;
      delete imin3_2;
      delete imin3_1;
    }
  }
  
  // Real
  RealConstant too(2.5);
  RealConstant fore(4.5);
  RealVariable x;
  Minimum<Real> minr;
  Real tempr;

  // Unary not supported in schema

  {
    // Binary
    Function *rmin2_1 = makeFunction(&minr, 2);
    rmin2_1->setArgument(0, &too, false);
    rmin2_1->setArgument(1, &fore, false);

    Function *rmin2_2 = makeFunction(&minr, 2);
    rmin2_2->setArgument(0, &x, false);
    rmin2_2->setArgument(1, &too, false);

    Function *rmin2_3 = makeFunction(&minr, 2);
    rmin2_3->setArgument(0, &fore, false);
    rmin2_3->setArgument(1, &x, false);

    rmin2_1->activate();
    rmin2_2->activate();
    rmin2_3->activate();

    assertTrue_1(rmin2_1->getValue(tempr));
    assertTrue_1(tempr == 2.5);

    // Should be unknown because x not initialized yet
    assertTrue_1(!rmin2_2->getValue(tempr));
    assertTrue_1(!rmin2_3->getValue(tempr));

    x.setValue(-0.5);
    // should be known now
    assertTrue_1(rmin2_2->getValue(tempr));
    assertTrue_1(tempr == -0.5);
    assertTrue_1(rmin2_3->getValue(tempr));
    assertTrue_1(tempr == -0.5);

    delete rmin2_3;
    delete rmin2_2;
    delete rmin2_1;
  }
  
  {
    // N-ary (not supported in schema)
    Function *rmin3_1 = makeFunction(&minr, 3);
    rmin3_1->setArgument(0, &too, false);
    rmin3_1->setArgument(1, &fore, false);
    rmin3_1->setArgument(2, &too, false);

    Function *rmin3_2 = makeFunction(&minr, 3);
    rmin3_2->setArgument(0, &x, false);
    rmin3_2->setArgument(1, &too, false);
    rmin3_2->setArgument(2, &fore, false);

    Function *rmin3_3 = makeFunction(&minr, 3);
    rmin3_3->setArgument(0, &fore, false);
    rmin3_3->setArgument(1, &fore, false);
    rmin3_3->setArgument(2, &x, false);

    rmin3_1->activate();
    rmin3_2->activate();
    rmin3_3->activate();
  
    x.setUnknown();

    assertTrue_1(rmin3_1->getValue(tempr));
    assertTrue_1(tempr == 2.5);

    // Should be unknown because x unknown
    assertTrue_1(!rmin3_2->getValue(tempr));
    assertTrue_1(!rmin3_3->getValue(tempr));

    x.setValue(3.25);
    // should be known now
    assertTrue_1(rmin3_2->getValue(tempr));
    assertTrue_1(tempr == 2.5);
    assertTrue_1(rmin3_3->getValue(tempr));
    assertTrue_1(tempr == 3.25);

    delete rmin3_3;
    delete rmin3_2;
    delete rmin3_1;
  }

  // Mixed numeric

  // Unary not supported in schema

  {
    // Binary
    Function *mmin2_1 = makeFunction(&minr, 2);
    mmin2_1->setArgument(0, &too, false);
    mmin2_1->setArgument(1, &tree, false);

    Function *mmin2_2 = makeFunction(&minr, 2);
    mmin2_2->setArgument(0, &i, false);
    mmin2_2->setArgument(1, &too, false);

    Function *mmin2_3 = makeFunction(&minr, 2);
    mmin2_3->setArgument(0, &tree, false);
    mmin2_3->setArgument(1, &x, false);

    mmin2_1->activate();
    mmin2_2->activate();
    mmin2_3->activate();
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmin2_1->getValue(tempr));
    assertTrue_1(tempr == 2.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmin2_2->getValue(tempr));
    assertTrue_1(!mmin2_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmin2_2->getValue(tempr));
    assertTrue_1(tempr == 2.5);
    assertTrue_1(mmin2_3->getValue(tempr));
    assertTrue_1(tempr == -0.5);

    delete mmin2_3;
    delete mmin2_2;
    delete mmin2_1;
  }

  {
    // N-ary (not supported in schema)
    Function *mmin3_1 = makeFunction(&minr, 3);
    mmin3_1->setArgument(0, &tree, false);
    mmin3_1->setArgument(1, &fore, false);
    mmin3_1->setArgument(2, &m1, false);

    Function *mmin3_2 = makeFunction(&minr, 3);
    mmin3_2->setArgument(0, &x, false);
    mmin3_2->setArgument(1, &won, false);
    mmin3_2->setArgument(2, &fore, false);

    Function *mmin3_3 = makeFunction(&minr, 3);
    mmin3_3->setArgument(0, &tree, false);
    mmin3_3->setArgument(1, &too, false);
    mmin3_3->setArgument(2, &i, false);

    mmin3_1->activate();
    mmin3_2->activate();
    mmin3_3->activate();
  
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmin3_1->getValue(tempr));
    assertTrue_1(tempr == -1);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmin3_2->getValue(tempr));
    assertTrue_1(!mmin3_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmin3_2->getValue(tempr));
    assertTrue_1(tempr == -0.5);
    assertTrue_1(mmin3_3->getValue(tempr));
    assertTrue_1(tempr == 2.5);

    delete mmin3_3;
    delete mmin3_2;
    delete mmin3_1;
  }
  
  return true;
}

bool maximumTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant won(1);
  IntegerConstant tree(3);
  IntegerVariable i;

  {
    Maximum<Integer> maxi;
    Integer tempi;

    // Unary not supported in schema

    {
      // Binary
      Function *imax2_1 = makeFunction(&maxi, 2);
      imax2_1->setArgument(0, &m1, false);
      imax2_1->setArgument(1, &tree, false);

      Function *imax2_2 = makeFunction(&maxi, 2);
      imax2_2->setArgument(0, &i, false);
      imax2_2->setArgument(1, &won, false);

      Function *imax2_3 = makeFunction(&maxi, 2);
      imax2_3->setArgument(0, &tree, false);
      imax2_3->setArgument(1, &i, false);

      imax2_1->activate();
      imax2_2->activate();
      imax2_3->activate();

      assertTrue_1(imax2_1->getValue(tempi));
      assertTrue_1(tempi == 3);

      // Should be unknown because i not initialized yet
      assertTrue_1(!imax2_2->getValue(tempi));
      assertTrue_1(!imax2_3->getValue(tempi));

      i.setValue((Integer) 5);
      // should be known now
      assertTrue_1(imax2_2->getValue(tempi));
      assertTrue_1(tempi == 5);
      assertTrue_1(imax2_3->getValue(tempi));
      assertTrue_1(tempi == 5);

      delete imax2_3;
      delete imax2_2;
      delete imax2_1;
    }

    {
      // N-ary (not supported in schema)

      // Set up arglists
      Function *imax3_1 = makeFunction(&maxi, 3);
      imax3_1->setArgument(0, &m1, false);
      imax3_1->setArgument(1, &won, false);
      imax3_1->setArgument(2, &tree, false);

      Function *imax3_2 = makeFunction(&maxi, 3);
      imax3_2->setArgument(0, &won, false);
      imax3_2->setArgument(1, &tree, false);
      imax3_2->setArgument(2, &i, false);

      Function *imax3_3 = makeFunction(&maxi, 3);
      imax3_3->setArgument(0, &i, false);
      imax3_3->setArgument(1, &m1, false);
      imax3_3->setArgument(2, &won, false);

      imax3_1->activate();
      imax3_2->activate();
      imax3_3->activate();
  
      i.setUnknown();

      assertTrue_1(imax3_1->getValue(tempi));
      assertTrue_1(tempi == 3);

      // Should be unknown because i unknown
      assertTrue_1(!imax3_2->getValue(tempi));
      assertTrue_1(!imax3_3->getValue(tempi));

      i.setValue((Integer) 27);
      // should be known now
      assertTrue_1(imax3_2->getValue(tempi));
      assertTrue_1(tempi == 27);
      assertTrue_1(imax3_3->getValue(tempi));
      assertTrue_1(tempi == 27);

      delete imax3_3;
      delete imax3_2;
      delete imax3_1;
    }
  }
  
  // Real
  RealConstant too(2.5);
  RealConstant fore(4.5);
  RealVariable x;
  Maximum<Real> maxr;
  Real tempr;

  // Unary not supported in schema

  {
    // Binary
    Function *rmax2_1 = makeFunction(&maxr, 2);
    rmax2_1->setArgument(0, &too, false);
    rmax2_1->setArgument(1, &fore, false);

    Function *rmax2_2 = makeFunction(&maxr, 2);
    rmax2_2->setArgument(0, &x, false);
    rmax2_2->setArgument(1, &too, false);

    Function *rmax2_3 = makeFunction(&maxr, 2);
    rmax2_3->setArgument(0, &fore, false);
    rmax2_3->setArgument(1, &x, false);

    rmax2_1->activate();
    rmax2_2->activate();
    rmax2_3->activate();

    assertTrue_1(rmax2_1->getValue(tempr));
    assertTrue_1(tempr == 4.5);

    // Should be unknown because x not initialized yet
    assertTrue_1(!rmax2_2->getValue(tempr));
    assertTrue_1(!rmax2_3->getValue(tempr));

    x.setValue(-0.5);
    // should be known now
    assertTrue_1(rmax2_2->getValue(tempr));
    assertTrue_1(tempr == 2.5);
    assertTrue_1(rmax2_3->getValue(tempr));
    assertTrue_1(tempr == 4.5);

    delete rmax2_3;
    delete rmax2_2;
    delete rmax2_1;
  }

  {
    // N-ary (not supported in schema)
    Function *rmax3_1 = makeFunction(&maxr, 3);
    rmax3_1->setArgument(0, &too, false);
    rmax3_1->setArgument(1, &fore, false);
    rmax3_1->setArgument(2, &too, false);

    Function *rmax3_2 = makeFunction(&maxr, 3);
    rmax3_2->setArgument(0, &x, false);
    rmax3_2->setArgument(1, &too, false);
    rmax3_2->setArgument(2, &fore, false);

    Function *rmax3_3 = makeFunction(&maxr, 3);
    rmax3_3->setArgument(0, &too, false);
    rmax3_3->setArgument(1, &too, false);
    rmax3_3->setArgument(2, &x, false);

    rmax3_1->activate();
    rmax3_2->activate();
    rmax3_3->activate();
  
    x.setUnknown();

    assertTrue_1(rmax3_1->getValue(tempr));
    assertTrue_1(tempr == 4.5);

    // Should be unknown because x unknown
    assertTrue_1(!rmax3_2->getValue(tempr));
    assertTrue_1(!rmax3_3->getValue(tempr));

    x.setValue(3.25);
    // should be known now
    assertTrue_1(rmax3_2->getValue(tempr));
    assertTrue_1(tempr == 4.5);
    assertTrue_1(rmax3_3->getValue(tempr));
    assertTrue_1(tempr == 3.25);

    delete rmax3_3;
    delete rmax3_2;
    delete rmax3_1;
  }

  // Mixed numeric

  // Unary not supported in schema

  {
    // Binary
    Function *mmax2_1 = makeFunction(&maxr, 2);
    mmax2_1->setArgument(0, &too, false);
    mmax2_1->setArgument(1, &tree, false);

    Function *mmax2_2 = makeFunction(&maxr, 2);
    mmax2_2->setArgument(0, &i, false);
    mmax2_2->setArgument(1, &too, false);

    Function *mmax2_3 = makeFunction(&maxr, 2);
    mmax2_3->setArgument(0, &tree, false);
    mmax2_3->setArgument(1, &x, false);

    mmax2_1->activate();
    mmax2_2->activate();
    mmax2_3->activate();
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmax2_1->getValue(tempr));
    assertTrue_1(tempr == 3);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmax2_2->getValue(tempr));
    assertTrue_1(!mmax2_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmax2_2->getValue(tempr));
    assertTrue_1(tempr == 42);
    assertTrue_1(mmax2_3->getValue(tempr));
    assertTrue_1(tempr == 3);

    delete mmax2_3;
    delete mmax2_2;
    delete mmax2_1;
  }

  {
    // N-ary (not supported in schema)
    Function *mmax3_1 = makeFunction(&maxr, 3);
    mmax3_1->setArgument(0, &tree, false);
    mmax3_1->setArgument(1, &fore, false);
    mmax3_1->setArgument(2, &m1, false);

    Function *mmax3_2 = makeFunction(&maxr, 3);
    mmax3_2->setArgument(0, &x, false);
    mmax3_2->setArgument(1, &won, false);
    mmax3_2->setArgument(2, &fore, false);

    Function *mmax3_3 = makeFunction(&maxr, 3);
    mmax3_3->setArgument(0, &tree, false);
    mmax3_3->setArgument(1, &too, false);
    mmax3_3->setArgument(2, &i, false);

    mmax3_1->activate();
    mmax3_2->activate();
    mmax3_3->activate();
  
    i.setUnknown();
    x.setUnknown();

    assertTrue_1(mmax3_1->getValue(tempr));
    assertTrue_1(tempr == 4.5);

    // Should be unknown because i, x unknown
    assertTrue_1(!mmax3_2->getValue(tempr));
    assertTrue_1(!mmax3_3->getValue(tempr));

    i.setValue((Integer) 42);
    x.setValue(-0.5);
    // should be known now
    assertTrue_1(mmax3_2->getValue(tempr));
    assertTrue_1(tempr == 4.5);
    assertTrue_1(mmax3_3->getValue(tempr));
    assertTrue_1(tempr == 42);

    delete mmax3_3;
    delete mmax3_2;
    delete mmax3_1;
  }
  return true;
}

bool absTest()
{
  // Integer
  IntegerConstant m1(-1);
  IntegerConstant won(1);
  IntegerVariable i;

  {
    AbsoluteValue<Integer> absi;
    Integer tempi;

    
    Function *iabs1 = makeFunction(&absi, 1);
    iabs1->setArgument(0, &m1, false);

    Function *iabs2 = makeFunction(&absi, 1);
    iabs2->setArgument(0, &won, false);

    Function *iabs3 = makeFunction(&absi, 1);
    iabs3->setArgument(0, &i, false);

    iabs1->activate();
    iabs2->activate();
    iabs3->activate();

    assertTrue_1(iabs1->getValue(tempi));
    assertTrue_1(tempi == 1);
    assertTrue_1(iabs2->getValue(tempi));
    assertTrue_1(tempi == 1);
  
    assertTrue_1(!iabs3->getValue(tempi));

    i.setValue((Integer) -22);
    assertTrue_1(iabs3->getValue(tempi));
    assertTrue_1(tempi == 22);

    delete iabs3;
    delete iabs2;
    delete iabs1;
  }

  // Real
  RealConstant mtoo(-2);
  RealConstant tree(3.5);
  RealVariable x;
  AbsoluteValue<Real> absr;
  Real tempr;

  {
    Function *rabs1 = makeFunction(&absr, 1);
    rabs1->setArgument(0, &mtoo, false);

    Function *rabs2 = makeFunction(&absr, 1);
    rabs2->setArgument(0, &tree, false);

    Function *rabs3 = makeFunction(&absr, 1);
    rabs3->setArgument(0, &x, false);

    rabs1->activate();
    rabs2->activate();
    rabs3->activate();

    assertTrue_1(rabs1->getValue(tempr));
    assertTrue_1(tempr == 2);
    assertTrue_1(rabs2->getValue(tempr));
    assertTrue_1(tempr == 3.5);
  
    assertTrue_1(!rabs3->getValue(tempr));

    x.setValue(-18.5);
    assertTrue_1(rabs3->getValue(tempr));
    assertTrue_1(tempr == 18.5);

    delete rabs3;
    delete rabs2;
    delete rabs1;
  }

  // Mixed
  {
    Function *mabs1 = makeFunction(&absr, 1);
    mabs1->setArgument(0, &m1, false);

    Function *mabs2 = makeFunction(&absr, 1);
    mabs2->setArgument(0, &won, false);

    Function *mabs3 = makeFunction(&absr, 1);
    mabs3->setArgument(0, &i, false);

    mabs1->activate();
    mabs2->activate();
    mabs3->activate();

    i.setUnknown();

    assertTrue_1(mabs1->getValue(tempr));
    assertTrue_1(tempr == 1);
    assertTrue_1(mabs2->getValue(tempr));
    assertTrue_1(tempr == 1);
  
    assertTrue_1(!mabs3->getValue(tempr));

    i.setValue((Integer) 14);
    assertTrue_1(mabs3->getValue(tempr));
    assertTrue_1(tempr == 14);

    delete mabs3;
    delete mabs2;
    delete mabs1;
  }
  
  return true;
}

bool sqrtTest()
{

  // Real
  SquareRoot<Real> sqrt;
  Real tempr;

  {
    RealConstant too(2.25);
    RealConstant nein(9);
    RealVariable x;

    Function *rsqrt1 = makeFunction(&sqrt, 1);
    rsqrt1->setArgument(0, &too, false);

    Function *rsqrt2 = makeFunction(&sqrt, 1);
    rsqrt2->setArgument(0, &nein, false);

    Function *rsqrt3 = makeFunction(&sqrt, 1);
    rsqrt3->setArgument(0, &x, false);

    rsqrt1->activate();
    rsqrt2->activate();
    rsqrt3->activate();

    assertTrue_1(rsqrt1->getValue(tempr));
    assertTrue_1(tempr == 1.5);
    assertTrue_1(rsqrt2->getValue(tempr));
    assertTrue_1(tempr == 3);
  
    assertTrue_1(!rsqrt3->getValue(tempr));

    x.setValue(0.25);
    assertTrue_1(rsqrt3->getValue(tempr));
    assertTrue_1(tempr == 0.5);

    delete rsqrt3;
    delete rsqrt2;
    delete rsqrt1;
  }

  // Mixed
  {
    IntegerConstant fore(4);
    IntegerConstant sixteen(16);
    IntegerVariable i;

    Function *msqrt1 = makeFunction(&sqrt, 1);
    msqrt1->setArgument(0, &fore, false);

    Function *msqrt2 = makeFunction(&sqrt, 1);
    msqrt2->setArgument(0, &sixteen, false);

    Function *msqrt3 = makeFunction(&sqrt, 1);
    msqrt3->setArgument(0, &i, false);

    msqrt1->activate();
    msqrt2->activate();
    msqrt3->activate();

    assertTrue_1(msqrt1->getValue(tempr));
    assertTrue_1(tempr == 2);
    assertTrue_1(msqrt2->getValue(tempr));
    assertTrue_1(tempr == 4);
  
    assertTrue_1(!msqrt3->getValue(tempr));

    i.setValue((Integer) 49);
    assertTrue_1(msqrt3->getValue(tempr));
    assertTrue_1(tempr == 7);

    delete msqrt3;
    delete msqrt2;
    delete msqrt1;
  }

  return true;
}

static bool testCeiling()
{
  RealConstant three(3);
  RealConstant pi(3.14);
  RealConstant e(2.718);
  RealConstant minusthree(-3);
  RealConstant minuspi(-3.14);
  RealConstant minuse(-2.718);
  RealConstant toobig(3000000000.5);
  RealConstant toonegative(-3000000000.5);

  Ceiling<Real> rCeiling;
  Ceiling<Integer> iCeiling;

  RealVariable x;

  Function *realCeiling = makeFunction(&rCeiling, 1);
  realCeiling->setArgument(0, &x, false);

  Function *intCeiling = makeFunction(&iCeiling, 1);
  intCeiling->setArgument(0, &x, false);

  Real rtemp;
  Integer itemp;

  x.activate();
  realCeiling->activate();
  intCeiling->activate();

  assertTrue_1(!x.isKnown());
  assertTrue_1(!realCeiling->isKnown());
  assertTrue_1(!intCeiling->isKnown());
  assertTrue_1(!x.getValue(rtemp));
  assertTrue_1(!realCeiling->getValue(rtemp));
  assertTrue_1(!intCeiling->getValue(itemp));

  x.setValue(three);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(pi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3.14);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == 4);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == 4);

  x.setValue(e);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 2.718);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(minusthree);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuspi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3.14);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuse);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -2.718);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == -2);
  assertTrue_1(intCeiling->getValue(itemp));
  assertTrue_1(itemp == -2);

  x.setValue(toobig);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(!intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.5);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == 3000000001.0);
  assertTrue_1(!intCeiling->getValue(itemp));

  x.setValue(toonegative);
  assertTrue_1(x.isKnown());
  assertTrue_1(realCeiling->isKnown());
  assertTrue_1(!intCeiling->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.5);
  assertTrue_1(realCeiling->getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.0);
  assertTrue_1(!intCeiling->getValue(itemp));
  
  delete intCeiling;
  delete realCeiling;

  return true;
}

static bool testFloor()
{
  RealConstant three(3);
  RealConstant pi(3.14);
  RealConstant e(2.718);
  RealConstant minusthree(-3);
  RealConstant minuspi(-3.14);
  RealConstant minuse(-2.718);
  RealConstant toobig(3000000000.5);
  RealConstant toonegative(-3000000000.5);

  Floor<Real> rFloor;
  Floor<Integer> iFloor;

  RealVariable x;

  Function *realFloor = makeFunction(&rFloor, 1);
  realFloor->setArgument(0, &x, false);

  Function *intFloor = makeFunction(&iFloor, 1);
  intFloor->setArgument(0, &x, false);

  Real rtemp;
  Integer itemp;

  x.activate();
  realFloor->activate();
  intFloor->activate();

  assertTrue_1(!x.isKnown());
  assertTrue_1(!realFloor->isKnown());
  assertTrue_1(!intFloor->isKnown());
  assertTrue_1(!x.getValue(rtemp));
  assertTrue_1(!realFloor->getValue(rtemp));
  assertTrue_1(!intFloor->getValue(itemp));

  x.setValue(three);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(pi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3.14);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(e);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 2.718);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == 2);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == 2);

  x.setValue(minusthree);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuspi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3.14);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == -4);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == -4);

  x.setValue(minuse);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -2.718);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intFloor->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(toobig);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(!intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.5);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.0);
  assertTrue_1(!intFloor->getValue(itemp));

  x.setValue(toonegative);
  assertTrue_1(x.isKnown());
  assertTrue_1(realFloor->isKnown());
  assertTrue_1(!intFloor->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.5);
  assertTrue_1(realFloor->getValue(rtemp));
  assertTrue_1(rtemp == -3000000001.0);
  assertTrue_1(!intFloor->getValue(itemp));

  delete intFloor;
  delete realFloor;

  return true;
}

  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have round() or trunc()
#if !defined(__VXWORKS__)

static bool testRound()
{
  RealConstant three(3);
  RealConstant pi(3.14);
  RealConstant e(2.718);
  RealConstant minusthree(-3);
  RealConstant minuspi(-3.14);
  RealConstant minuse(-2.718);
  RealConstant toobig(3000000000.5);
  RealConstant toonegative(-3000000000.5);

  Round<Real> rRound;
  Round<Integer> iRound;

  RealVariable x;

  Function *realRound = makeFunction(&rRound, 1);
  realRound->setArgument(0, &x, false);

  Function *intRound = makeFunction(&iRound, 1);
  intRound->setArgument(0, &x, false);

  Real rtemp;
  Integer itemp;

  x.activate();
  realRound->activate();
  intRound->activate();

  assertTrue_1(!x.isKnown());
  assertTrue_1(!realRound->isKnown());
  assertTrue_1(!intRound->isKnown());
  assertTrue_1(!x.getValue(rtemp));
  assertTrue_1(!realRound->getValue(rtemp));
  assertTrue_1(!intRound->getValue(itemp));

  x.setValue(three);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(pi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3.14);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(e);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 2.718);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(minusthree);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuspi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3.14);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuse);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -2.718);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intRound->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(toobig);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(!intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.5);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == 3000000001.0);
  assertTrue_1(!intRound->getValue(itemp));

  x.setValue(toonegative);
  assertTrue_1(x.isKnown());
  assertTrue_1(realRound->isKnown());
  assertTrue_1(!intRound->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.5);
  assertTrue_1(realRound->getValue(rtemp));
  assertTrue_1(rtemp == -3000000001.0);
  assertTrue_1(!intRound->getValue(itemp));

  delete intRound;
  delete realRound;

  return true;
}

static bool testTruncate()
{
  RealConstant three(3);
  RealConstant pi(3.14);
  RealConstant e(2.718);
  RealConstant minusthree(-3);
  RealConstant minuspi(-3.14);
  RealConstant minuse(-2.718);
  RealConstant toobig(3000000000.5);
  RealConstant toonegative(-3000000000.5);

  Truncate<Real> rTruncate;
  Truncate<Integer> iTruncate;

  RealVariable x;

  Function *realTruncate = makeFunction(&rTruncate, 1);
  realTruncate->setArgument(0, &x, false);

  Function *intTruncate = makeFunction(&iTruncate, 1);
  intTruncate->setArgument(0, &x, false);

  Real rtemp;
  Integer itemp;

  x.activate();
  realTruncate->activate();
  intTruncate->activate();

  assertTrue_1(!x.isKnown());
  assertTrue_1(!realTruncate->isKnown());
  assertTrue_1(!intTruncate->isKnown());
  assertTrue_1(!x.getValue(rtemp));
  assertTrue_1(!realTruncate->getValue(rtemp));
  assertTrue_1(!intTruncate->getValue(itemp));

  x.setValue(three);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(pi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3.14);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(e);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 2.718);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == 2);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == 2);

  x.setValue(minusthree);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuspi);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3.14);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuse);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -2.718);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == -2);
  assertTrue_1(intTruncate->getValue(itemp));
  assertTrue_1(itemp == -2);

  x.setValue(toobig);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(!intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.5);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.0);
  assertTrue_1(!intTruncate->getValue(itemp));

  x.setValue(toonegative);
  assertTrue_1(x.isKnown());
  assertTrue_1(realTruncate->isKnown());
  assertTrue_1(!intTruncate->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.5);
  assertTrue_1(realTruncate->getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.0);
  assertTrue_1(!intTruncate->getValue(itemp));

  delete intTruncate;
  delete realTruncate;

  return true;
}

#endif // !defined(__VXWORKS__)

static bool testRealToInteger()
{
  RealConstant zero((Real) 0);
  RealConstant three(3);
  RealConstant pi(3.14);
  RealConstant minusthree(-3);
  RealConstant minuspi(-3.14);
  RealConstant toobig(3000000000.0);
  RealConstant toonegative(-3000000000.0);

  RealToInteger rtiOp;
  RealVariable x;

  Function *rti = makeFunction(&rtiOp, 1);
  rti->setArgument(0, &x, false);

  Real rtemp;
  Integer itemp;

  x.activate();
  rti->activate();

  // Uninitialized
  assertTrue_1(!x.isKnown());
  assertTrue_1(!rti->isKnown());
  assertTrue_1(!x.getValue(rtemp));
  assertTrue_1(!rti->getValue(itemp));

  x.setValue(zero);
  assertTrue_1(x.isKnown());
  assertTrue_1(rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 0);
  assertTrue_1(rti->getValue(itemp));
  assertTrue_1(itemp == 0);

  x.setValue(three);
  assertTrue_1(x.isKnown());
  assertTrue_1(rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3);
  assertTrue_1(rti->getValue(itemp));
  assertTrue_1(itemp == 3);

  x.setValue(pi);
  assertTrue_1(x.isKnown());
  assertTrue_1(!rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3.14);
  assertTrue_1(!rti->getValue(itemp));

  x.setValue(minusthree);
  assertTrue_1(x.isKnown());
  assertTrue_1(rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3);
  assertTrue_1(rti->getValue(itemp));
  assertTrue_1(itemp == -3);

  x.setValue(minuspi);
  assertTrue_1(x.isKnown());
  assertTrue_1(!rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3.14);
  assertTrue_1(!rti->getValue(itemp));

  x.setValue(toobig);
  assertTrue_1(x.isKnown());
  assertTrue_1(!rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == 3000000000.0);
  assertTrue_1(!rti->getValue(itemp));

  x.setValue(toonegative);
  assertTrue_1(x.isKnown());
  assertTrue_1(!rti->isKnown());
  assertTrue_1(x.getValue(rtemp));
  assertTrue_1(rtemp == -3000000000.0);
  assertTrue_1(!rti->getValue(itemp));

  delete rti;

  return true;
}

bool arithmeticTest()
{
  runTest(additionTest);
  runTest(subtractionTest);
  runTest(multiplicationTest);
  runTest(divisionTest);
  runTest(moduloTest);
  runTest(minimumTest);
  runTest(maximumTest);
  runTest(absTest);
  runTest(sqrtTest);
  runTest(testCeiling);
  runTest(testFloor);
  // Believe it or not, VxWorks 6.8 for PowerPC doesn't have round() or trunc()
#if !defined(__VXWORKS__)
  runTest(testRound);
  runTest(testTruncate);
#endif // !defined(__VXWORKS__)
  runTest(testRealToInteger);
  return true;
}
