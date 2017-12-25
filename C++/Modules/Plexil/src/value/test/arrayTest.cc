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

#include "ArrayImpl.hh"
#include "TestSupport.hh"

using namespace PLEXIL;

static bool testConstructors()
{
  // BooleanArray
  {
    BooleanArray        emptyBool;
    assertTrue_1(emptyBool.size()   == 0);

    bool btemp;

    {
      BooleanArray sizedBool(2);
      assertTrue_1(sizedBool.size()   == 2);
      assertTrue_1(!sizedBool.elementKnown(0));
      assertTrue_1(!sizedBool.elementKnown(1));

      assertTrue_1(!sizedBool.getElement(0, btemp));
      assertTrue_1(!sizedBool.getElement(1, btemp));
    }

    {
      BooleanArray sizedInitedBool(2, true);
      assertTrue_1(sizedInitedBool.size()   == 2);
      assertTrue_1(sizedInitedBool.getElement(0, btemp));
      assertTrue_1(btemp);
      assertTrue_1(sizedInitedBool.getElement(1, btemp));
      assertTrue_1(btemp);
    }

    std::vector<bool> bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray        initedBool(bv);
    assertTrue_1(initedBool.size()   == 2);
    assertTrue_1(initedBool.elementKnown(0));
    assertTrue_1(initedBool.elementKnown(1));
    assertTrue_1(initedBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(initedBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);

    BooleanArray        copyBool(initedBool);
    assertTrue_1(copyBool.size()   == 2);
    assertTrue_1(copyBool.elementKnown(0));
    assertTrue_1(copyBool.elementKnown(1));
    assertTrue_1(copyBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(copyBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);

    emptyBool   = copyBool;
    assertTrue_1(emptyBool.size()   == 2);
    assertTrue_1(emptyBool.elementKnown(0));
    assertTrue_1(emptyBool.elementKnown(1));
    assertTrue_1(emptyBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(emptyBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);
  }

  // Integer array
  {
    IntegerArray     emptyInt;
    assertTrue_1(emptyInt.size()    == 0);

    int32_t itemp;

    {
      IntegerArray     sizedInt(2);
      assertTrue_1(sizedInt.size()    == 2);
      assertTrue_1(!sizedInt.elementKnown(0));
      assertTrue_1(!sizedInt.elementKnown(1));
      assertTrue_1(!sizedInt.getElement(0, itemp));
      assertTrue_1(!sizedInt.getElement(1, itemp));
    }

    IntegerArray sizedInitedInt(2, 42);
    assertTrue_1(sizedInitedInt.size()    == 2);
    assertTrue_1(sizedInitedInt.getElement(0, itemp));
    assertTrue_1(itemp == 42);
    assertTrue_1(sizedInitedInt.getElement(1, itemp));
    assertTrue_1(itemp == 42);

    std::vector<int32_t>     iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray     initedInt(iv);
    assertTrue_1(initedInt.size()    == 2);
    assertTrue_1(initedInt.elementKnown(0));
    assertTrue_1(initedInt.elementKnown(1));
    assertTrue_1(initedInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(initedInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);

    IntegerArray     copyInt(initedInt);
    assertTrue_1(copyInt.size()    == 2);
    assertTrue_1(copyInt.elementKnown(0));
    assertTrue_1(copyInt.elementKnown(1));
    assertTrue_1(copyInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(copyInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);

    emptyInt    = copyInt;
    assertTrue_1(emptyInt.size()    == 2);
    assertTrue_1(emptyInt.elementKnown(0));
    assertTrue_1(emptyInt.elementKnown(1));
    assertTrue_1(emptyInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(emptyInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);

  }

  // Real array
  {
    RealArray      emptyReal;
    assertTrue_1(emptyReal.size()   == 0);

    double dtemp;

    {
      RealArray      sizedReal(2);
      assertTrue_1(sizedReal.size()   == 2);
      assertTrue_1(!sizedReal.elementKnown(0));
      assertTrue_1(!sizedReal.elementKnown(1));
      assertTrue_1(!sizedReal.getElement(0, dtemp));
      assertTrue_1(!sizedReal.getElement(1, dtemp));
    }

    RealArray    sizedInitedReal(2, 2.5);
    assertTrue_1(sizedInitedReal.size()   == 2);
    assertTrue_1(sizedInitedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == 2.5);
    assertTrue_1(sizedInitedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == 2.5);

    std::vector<double>      dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray      initedReal(dv);
    assertTrue_1(initedReal.size()   == 2);
    assertTrue_1(initedReal.elementKnown(0));
    assertTrue_1(initedReal.elementKnown(1));
    assertTrue_1(initedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(initedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);

    RealArray      copyReal(initedReal);
    assertTrue_1(copyReal.size()   == 2);
    assertTrue_1(copyReal.elementKnown(0));
    assertTrue_1(copyReal.elementKnown(1));
    assertTrue_1(copyReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(copyReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);

    emptyReal   = copyReal;
    assertTrue_1(emptyReal.size()   == 2);
    assertTrue_1(emptyReal.elementKnown(0));
    assertTrue_1(emptyReal.elementKnown(1));
    assertTrue_1(emptyReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(emptyReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);

  }

  {
    StringArray emptyString;
    assertTrue_1(emptyString.size() == 0);

    std::string stemp;

    {
      StringArray sizedString(2);
      assertTrue_1(sizedString.size() == 2);
      assertTrue_1(!sizedString.elementKnown(0));
      assertTrue_1(!sizedString.elementKnown(1));
      assertTrue_1(!sizedString.getElement(0, stemp));
      assertTrue_1(!sizedString.getElement(1, stemp));
    }

    StringArray  sizedInitedString(2, "foo");
    assertTrue_1(sizedInitedString.size() == 2);
    assertTrue_1(sizedInitedString.getElement(0, stemp));
    assertTrue_1(stemp == "foo");
    assertTrue_1(sizedInitedString.getElement(1, stemp));
    assertTrue_1(stemp == "foo");

    std::vector<std::string> sv(2);
    sv[0] = std::string("yo ");
    sv[1] = std::string("mama");
    StringArray initedString(sv);
    assertTrue_1(initedString.size() == 2);
    assertTrue_1(initedString.elementKnown(0));
    assertTrue_1(initedString.elementKnown(1));
    assertTrue_1(initedString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(initedString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);

    StringArray copyString(initedString);
    assertTrue_1(copyString.size() == 2);
    assertTrue_1(copyString.elementKnown(0));
    assertTrue_1(copyString.elementKnown(1));
    assertTrue_1(copyString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(copyString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);

    // Test assignment operator
    emptyString = copyString;
    assertTrue_1(emptyString.size() == 2);
    assertTrue_1(emptyString.elementKnown(0));
    assertTrue_1(emptyString.elementKnown(1));
    assertTrue_1(emptyString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(emptyString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);
  }

  return true;
}

bool testResize()
{
  {
    BooleanArray        emptyBool;
    assertTrue_1(emptyBool.size()   == 0);

    BooleanArray        sizedBool(2);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(sizedBool   != emptyBool);

    emptyBool.resize(2);
    assertTrue_1(emptyBool.size()   == 2);
    assertTrue_1(!emptyBool.elementKnown(0));
    assertTrue_1(!emptyBool.elementKnown(1));

    bool btemp;

    assertTrue_1(!emptyBool.getElement(0, btemp));
    assertTrue_1(!emptyBool.getElement(1, btemp));
    assertTrue_1(sizedBool   == emptyBool);

    emptyBool.resize(1);
    assertTrue_1(emptyBool.size() == 1);

    std::vector<bool>        bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray        initedBool(bv);
    assertTrue_1(initedBool.size()   == 2);
    assertTrue_1(initedBool.elementKnown(0));
    assertTrue_1(initedBool.elementKnown(1));
    assertTrue_1(initedBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(initedBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);

    initedBool.resize(4);
    assertTrue_1(initedBool.size()   == 4);
    assertTrue_1(initedBool.elementKnown(0));
    assertTrue_1(initedBool.elementKnown(1));
    assertTrue_1(initedBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(initedBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);
    assertTrue_1(!initedBool.elementKnown(2));
    assertTrue_1(!initedBool.elementKnown(3));
    assertTrue_1(!initedBool.getElement(2, btemp));
    assertTrue_1(!initedBool.getElement(3, btemp));

    initedBool.resize(2);
    assertTrue_1(initedBool.size()   == 2);
    assertTrue_1(initedBool.elementKnown(0));
    assertTrue_1(initedBool.elementKnown(1));
    assertTrue_1(initedBool.getElement(0, btemp));
    assertTrue_1(btemp == bv[0]);
    assertTrue_1(initedBool.getElement(1, btemp));
    assertTrue_1(btemp == bv[1]);
  }

  {
    IntegerArray     emptyInt;
    assertTrue_1(emptyInt.size()    == 0);

    IntegerArray     sizedInt(2);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(sizedInt    != emptyInt);

    emptyInt.resize(2);
    assertTrue_1(emptyInt.size()    == 2);
    assertTrue_1(!emptyInt.elementKnown(0));
    assertTrue_1(!emptyInt.elementKnown(1));

    int32_t itemp;
    assertTrue_1(!emptyInt.getElement(0, itemp));
    assertTrue_1(!emptyInt.getElement(1, itemp));
    assertTrue_1(sizedInt    == emptyInt);

    emptyInt.resize(1);
    assertTrue_1(emptyInt.size()    == 1);

    std::vector<int32_t>     iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray     initedInt(iv);
    assertTrue_1(initedInt.size()    == 2);
    assertTrue_1(initedInt.elementKnown(0));
    assertTrue_1(initedInt.elementKnown(1));
    assertTrue_1(initedInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(initedInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);

    initedInt.resize(4);
    assertTrue_1(initedInt.size()    == 4);
    assertTrue_1(initedInt.elementKnown(0));
    assertTrue_1(initedInt.elementKnown(1));
    assertTrue_1(initedInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(initedInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);
    assertTrue_1(!initedInt.elementKnown(2));
    assertTrue_1(!initedInt.elementKnown(3));
    assertTrue_1(!initedInt.getElement(2, itemp));
    assertTrue_1(!initedInt.getElement(3, itemp));

    initedInt.resize(2);
    assertTrue_1(initedInt.size()    == 2);
    assertTrue_1(initedInt.elementKnown(0));
    assertTrue_1(initedInt.elementKnown(1));
    assertTrue_1(initedInt.getElement(0, itemp));
    assertTrue_1(itemp == iv[0]);
    assertTrue_1(initedInt.getElement(1, itemp));
    assertTrue_1(itemp == iv[1]);
  }

  {
    RealArray      emptyReal;
    assertTrue_1(emptyReal.size()   == 0);

    RealArray      sizedReal(2);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(sizedReal   != emptyReal);

    emptyReal.resize(2);
    assertTrue_1(emptyReal.size()   == 2);
    assertTrue_1(!emptyReal.elementKnown(0));
    assertTrue_1(!emptyReal.elementKnown(1));

    double dtemp;

    assertTrue_1(!emptyReal.getElement(0, dtemp));
    assertTrue_1(!emptyReal.getElement(1, dtemp));
    assertTrue_1(sizedReal   == emptyReal);

    emptyReal.resize(1);
    assertTrue_1(emptyReal.size()   == 1);
    std::vector<double>      dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;

    RealArray      initedReal(dv);
    assertTrue_1(initedReal.size()   == 2);
    assertTrue_1(initedReal.elementKnown(0));
    assertTrue_1(initedReal.elementKnown(1));
    assertTrue_1(initedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(initedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);

    initedReal.resize(4);
    assertTrue_1(initedReal.size()   == 4);
    assertTrue_1(initedReal.elementKnown(0));
    assertTrue_1(initedReal.elementKnown(1));
    assertTrue_1(initedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(initedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);
    assertTrue_1(!initedReal.elementKnown(2));
    assertTrue_1(!initedReal.elementKnown(3));
    assertTrue_1(!initedReal.getElement(2, dtemp));
    assertTrue_1(!initedReal.getElement(3, dtemp));

    initedReal.resize(2);
    assertTrue_1(initedReal.size()   == 2);
    assertTrue_1(initedReal.elementKnown(0));
    assertTrue_1(initedReal.elementKnown(1));
    assertTrue_1(initedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dv[0]);
    assertTrue_1(initedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dv[1]);

  }

  {
    StringArray emptyString;
    assertTrue_1(emptyString.size() == 0);

    StringArray sizedString(2);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(sizedString != emptyString);

    emptyString.resize(2);
    assertTrue_1(emptyString.size() == 2);
    assertTrue_1(!emptyString.elementKnown(0));
    assertTrue_1(!emptyString.elementKnown(1));

    std::string stemp;

    assertTrue_1(!emptyString.getElement(0, stemp));
    assertTrue_1(!emptyString.getElement(1, stemp));
    assertTrue_1(sizedString == emptyString);

    emptyString.resize(1);
    assertTrue_1(emptyString.size() == 1);

    std::vector<std::string> sv(2);
    sv[0] = std::string("yo ");
    sv[1] = std::string("mama");
    StringArray initedString(sv);

    assertTrue_1(initedString.size() == 2);
    assertTrue_1(initedString.elementKnown(0));
    assertTrue_1(initedString.elementKnown(1));
    assertTrue_1(initedString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(initedString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);

    initedString.resize(4);
    assertTrue_1(initedString.size() == 4);
    assertTrue_1(initedString.elementKnown(0));
    assertTrue_1(initedString.elementKnown(1));
    assertTrue_1(initedString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(initedString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);
    assertTrue_1(!initedString.elementKnown(2));
    assertTrue_1(!initedString.elementKnown(3));
    assertTrue_1(!initedString.getElement(2, stemp));
    assertTrue_1(!initedString.getElement(3, stemp));

    initedString.resize(2);
    assertTrue_1(initedString.size() == 2);
    assertTrue_1(initedString.elementKnown(0));
    assertTrue_1(initedString.elementKnown(1));
    assertTrue_1(initedString.getElement(0, stemp));
    assertTrue_1(stemp == sv[0]);
    assertTrue_1(initedString.getElement(1, stemp));
    assertTrue_1(stemp == sv[1]);
  }
  return true;
}

static bool testSetters()
{
  {
    BooleanArray        sizedBool(2);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(!sizedBool.elementKnown(0));
    assertTrue_1(!sizedBool.elementKnown(1));

    bool btemp;

    assertTrue_1(!sizedBool.getElement(0, btemp));
    assertTrue_1(!sizedBool.getElement(1, btemp));

    bool bval = true;

    sizedBool.setElement(0, bval);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(sizedBool.elementKnown(0));
    assertTrue_1(!sizedBool.elementKnown(1));
    assertTrue_1(sizedBool.getElement(0, btemp));
    assertTrue_1(btemp == bval);
    assertTrue_1(!sizedBool.getElement(1, btemp));

    sizedBool.setElement(1, bval);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(sizedBool.elementKnown(0));
    assertTrue_1(sizedBool.elementKnown(1));
    assertTrue_1(sizedBool.getElement(0, btemp));
    assertTrue_1(btemp == bval);
    assertTrue_1(sizedBool.getElement(1, btemp));
    assertTrue_1(btemp == bval);

    sizedBool.setElementUnknown(0);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(!sizedBool.elementKnown(0));
    assertTrue_1(sizedBool.elementKnown(1));
    assertTrue_1(!sizedBool.getElement(0, btemp));
    assertTrue_1(sizedBool.getElement(1, btemp));
    assertTrue_1(btemp == bval);

    sizedBool.setElementUnknown(1);
    assertTrue_1(sizedBool.size()   == 2);
    assertTrue_1(!sizedBool.elementKnown(0));
    assertTrue_1(!sizedBool.elementKnown(1));
    assertTrue_1(!sizedBool.getElement(0, btemp));
    assertTrue_1(!sizedBool.getElement(1, btemp));
  }
  
  {
    IntegerArray     sizedInt(2);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(!sizedInt.elementKnown(0));
    assertTrue_1(!sizedInt.elementKnown(1));

    int32_t itemp;

    assertTrue_1(!sizedInt.getElement(0, itemp));
    assertTrue_1(!sizedInt.getElement(1, itemp));

    int32_t ival = 69;

    sizedInt.setElement(0, ival);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(sizedInt.elementKnown(0));
    assertTrue_1(!sizedInt.elementKnown(1));
    assertTrue_1(sizedInt.getElement(0, itemp));
    assertTrue_1(itemp == ival);
    assertTrue_1(!sizedInt.getElement(1, itemp));

    sizedInt.setElement(1, ival);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(sizedInt.elementKnown(0));
    assertTrue_1(sizedInt.elementKnown(1));
    assertTrue_1(sizedInt.getElement(0, itemp));
    assertTrue_1(itemp == ival);
    assertTrue_1(sizedInt.getElement(1, itemp));
    assertTrue_1(itemp == ival);

    sizedInt.setElementUnknown(0);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(!sizedInt.elementKnown(0));
    assertTrue_1(sizedInt.elementKnown(1));
    assertTrue_1(!sizedInt.getElement(0, itemp));
    assertTrue_1(sizedInt.getElement(1, itemp));
    assertTrue_1(itemp == ival);

    sizedInt.setElementUnknown(1);
    assertTrue_1(sizedInt.size()    == 2);
    assertTrue_1(!sizedInt.elementKnown(0));
    assertTrue_1(!sizedInt.elementKnown(1));
    assertTrue_1(!sizedInt.getElement(0, itemp));
    assertTrue_1(!sizedInt.getElement(1, itemp));
  }

  {
    RealArray      sizedReal(2);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(!sizedReal.elementKnown(0));
    assertTrue_1(!sizedReal.elementKnown(1));

    double dtemp;

    assertTrue_1(!sizedReal.getElement(0, dtemp));
    assertTrue_1(!sizedReal.getElement(1, dtemp));

    double dval = 2.718;

    sizedReal.setElement(0, dval);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(sizedReal.elementKnown(0));
    assertTrue_1(!sizedReal.elementKnown(1));
    assertTrue_1(sizedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dval);
    assertTrue_1(!sizedReal.getElement(1, dtemp));

    sizedReal.setElement(1, dval);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(sizedReal.elementKnown(0));
    assertTrue_1(sizedReal.elementKnown(1));
    assertTrue_1(sizedReal.getElement(0, dtemp));
    assertTrue_1(dtemp == dval);
    assertTrue_1(sizedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dval);

    sizedReal.setElementUnknown(0);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(!sizedReal.elementKnown(0));
    assertTrue_1(sizedReal.elementKnown(1));
    assertTrue_1(!sizedReal.getElement(0, dtemp));
    assertTrue_1(sizedReal.getElement(1, dtemp));
    assertTrue_1(dtemp == dval);

    sizedReal.setElementUnknown(1);
    assertTrue_1(sizedReal.size()   == 2);
    assertTrue_1(!sizedReal.elementKnown(0));
    assertTrue_1(!sizedReal.elementKnown(1));
    assertTrue_1(!sizedReal.getElement(0, dtemp));
    assertTrue_1(!sizedReal.getElement(1, dtemp));
  }

  {
    StringArray sizedString(2);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(!sizedString.elementKnown(0));
    assertTrue_1(!sizedString.elementKnown(1));

    std::string stemp;

    assertTrue_1(!sizedString.getElement(0, stemp));
    assertTrue_1(!sizedString.getElement(1, stemp));

    std::string sval("yahoo!");
    sizedString.setElement(0, sval);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(sizedString.elementKnown(0));
    assertTrue_1(!sizedString.elementKnown(1));
    assertTrue_1(sizedString.getElement(0, stemp));
    assertTrue_1(stemp == sval);
    assertTrue_1(!sizedString.getElement(1, stemp));

    sizedString.setElement(1, sval);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(sizedString.elementKnown(0));
    assertTrue_1(sizedString.elementKnown(1));
    assertTrue_1(sizedString.getElement(0, stemp));
    assertTrue_1(stemp == sval);
    assertTrue_1(sizedString.getElement(1, stemp));
    assertTrue_1(stemp == sval);

    sizedString.setElementUnknown(0);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(!sizedString.elementKnown(0));
    assertTrue_1(sizedString.elementKnown(1));
    assertTrue_1(!sizedString.getElement(0, stemp));
    assertTrue_1(sizedString.getElement(1, stemp));
    assertTrue_1(stemp == sval);

    sizedString.setElementUnknown(1);
    assertTrue_1(sizedString.size() == 2);
    assertTrue_1(!sizedString.elementKnown(0));
    assertTrue_1(!sizedString.elementKnown(1));
    assertTrue_1(!sizedString.getElement(0, stemp));
    assertTrue_1(!sizedString.getElement(1, stemp));
  }
  return true;
}

static bool testEquality()
{
  {
    BooleanArray emptyBool;
    assertTrue_1(emptyBool == emptyBool);
    assertTrue_1(!(emptyBool != emptyBool));

    BooleanArray sizedBool(2);
    assertTrue_1(sizedBool == sizedBool);
    assertTrue_1(!(sizedBool != sizedBool));
    assertTrue_1(emptyBool != sizedBool);

    std::vector<bool>        bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray initedBool(bv);
    assertTrue_1(initedBool == initedBool);
    assertTrue_1(!(initedBool != initedBool));
    assertTrue_1(emptyBool != initedBool);
    assertTrue_1(initedBool != sizedBool);

    BooleanArray copyBool(initedBool);
    assertTrue_1(copyBool == copyBool);
    assertTrue_1(!(copyBool != copyBool));
    assertTrue_1(copyBool   == initedBool);
    assertTrue_1(copyBool   != emptyBool);
    assertTrue_1(copyBool   != sizedBool);

    emptyBool   = copyBool;
    assertTrue_1(emptyBool == emptyBool);
    assertTrue_1(!(emptyBool != emptyBool));
    assertTrue_1(emptyBool   != sizedBool);
    assertTrue_1(emptyBool   == initedBool);
    assertTrue_1(emptyBool   == copyBool);
  }

  {
    IntegerArray emptyInt;
    assertTrue_1(emptyInt == emptyInt);
    assertTrue_1(!(emptyInt != emptyInt));

    IntegerArray sizedInt(2);
    assertTrue_1(sizedInt == sizedInt);
    assertTrue_1(!(sizedInt != sizedInt));
    assertTrue_1(emptyInt != sizedInt);

    std::vector<int32_t>     iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray initedInt(iv);
    assertTrue_1(initedInt == initedInt);
    assertTrue_1(!(initedInt != initedInt));
    assertTrue_1(emptyInt != initedInt);
    assertTrue_1(initedInt != sizedInt);

    IntegerArray copyInt(initedInt);
    assertTrue_1(copyInt == copyInt);
    assertTrue_1(!(copyInt != copyInt));
    assertTrue_1(copyInt    == initedInt);
    assertTrue_1(copyInt    != emptyInt);
    assertTrue_1(copyInt    != sizedInt);

    emptyInt    = copyInt;
    assertTrue_1(emptyInt == emptyInt);
    assertTrue_1(!(emptyInt != emptyInt));
    assertTrue_1(emptyInt    != sizedInt);
    assertTrue_1(emptyInt    == initedInt);
    assertTrue_1(emptyInt    == copyInt);
  }

  {
    RealArray emptyReal;
    assertTrue_1(emptyReal == emptyReal);
    assertTrue_1(!(emptyReal != emptyReal));

    RealArray sizedReal(2);
    assertTrue_1(sizedReal == sizedReal);
    assertTrue_1(!(sizedReal != sizedReal));
    assertTrue_1(emptyReal != sizedReal);

    std::vector<double>      dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray initedReal(dv);
    assertTrue_1(initedReal == initedReal);
    assertTrue_1(!(initedReal != initedReal));
    assertTrue_1(emptyReal != initedReal);
    assertTrue_1(initedReal != sizedReal);

    RealArray copyReal(initedReal);
    assertTrue_1(copyReal == copyReal);
    assertTrue_1(!(copyReal != copyReal));
    assertTrue_1(copyReal   == initedReal);
    assertTrue_1(copyReal   != emptyReal);
    assertTrue_1(copyReal   != sizedReal);

    emptyReal   = copyReal;
    assertTrue_1(emptyReal == emptyReal);
    assertTrue_1(!(emptyReal != emptyReal));
    assertTrue_1(emptyReal   != sizedReal);
    assertTrue_1(emptyReal   == initedReal);
    assertTrue_1(emptyReal   == copyReal);
  }

  {
    StringArray emptyString;
    assertTrue_1(emptyString == emptyString);
    assertTrue_1(!(emptyString != emptyString));

    StringArray sizedString(2);
    assertTrue_1(sizedString == sizedString);
    assertTrue_1(!(sizedString != sizedString));
    assertTrue_1(emptyString != sizedString);

    std::vector<std::string> sv(2);
    sv[0] = std::string("yo ");
    sv[1] = std::string("mama");
    StringArray initedString(sv);
    assertTrue_1(initedString == initedString);
    assertTrue_1(!(initedString != initedString));
    assertTrue_1(emptyString != initedString);
    assertTrue_1(initedString != sizedString);

    StringArray copyString(initedString);
    assertTrue_1(copyString == copyString);
    assertTrue_1(!(copyString != copyString));
    assertTrue_1(copyString == initedString);
    assertTrue_1(copyString != emptyString);
    assertTrue_1(copyString != sizedString);

    emptyString = copyString;
    assertTrue_1(emptyString == emptyString);
    assertTrue_1(!(emptyString != emptyString));
    assertTrue_1(emptyString != sizedString);
    assertTrue_1(emptyString == initedString);
    assertTrue_1(emptyString == copyString);
  }
  return true;
}

static bool testLessThan()
{
  {
    BooleanArray emptyBool;
    assertTrue_1(!(emptyBool < emptyBool));

    BooleanArray sizedBool(2);
    assertTrue_1(!(sizedBool < sizedBool));
    assertTrue_1(emptyBool < sizedBool);
    assertTrue_1(!(sizedBool < emptyBool));

    std::vector<bool> bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray initedBool(bv);
    assertTrue_1(!(initedBool < initedBool));
    assertTrue_1(emptyBool < initedBool);
    assertTrue_1(!(initedBool < emptyBool));
    assertTrue_1(sizedBool < initedBool);
    assertTrue_1(!(initedBool < sizedBool));

    std::vector<bool> bv2(2);
    bv2[0] = false;
    bv2[1] = false;
    BooleanArray initedBool2(bv2);
    assertTrue_1(!(initedBool2 < initedBool2));
    assertTrue_1(emptyBool < initedBool2);
    assertTrue_1(!(initedBool2 < emptyBool));
    assertTrue_1(sizedBool < initedBool2);
    assertTrue_1(!(initedBool2 < sizedBool));
    // Same size, contents determine the victor
    assertTrue_1(!(initedBool < initedBool2));
    assertTrue_1(initedBool2 < initedBool);
  }

  {
    IntegerArray emptyInt;
    assertTrue_1(!(emptyInt < emptyInt));

    IntegerArray sizedInt(2);
    assertTrue_1(!(sizedInt < sizedInt));
    assertTrue_1(emptyInt < sizedInt);
    assertTrue_1(!(sizedInt < emptyInt));

    std::vector<int32_t> iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray initedInt(iv);
    assertTrue_1(!(initedInt < initedInt));
    assertTrue_1(emptyInt < initedInt);
    assertTrue_1(!(initedInt < emptyInt));
    assertTrue_1(sizedInt < initedInt);
    assertTrue_1(!(initedInt < sizedInt));

    std::vector<int32_t> iv2(2);
    iv2[0] = 42;
    iv2[1] = 7;
    IntegerArray initedInt2(iv2);
    assertTrue_1(!(initedInt2 < initedInt2));
    assertTrue_1(emptyInt < initedInt2);
    assertTrue_1(!(initedInt2 < emptyInt));
    assertTrue_1(sizedInt < initedInt2);
    assertTrue_1(!(initedInt2 < sizedInt));
    // Same size, contents determine the victor
    assertTrue_1(initedInt < initedInt2);
    assertTrue_1(!(initedInt2 < initedInt));
  }

  {
    RealArray emptyReal;
    assertTrue_1(!(emptyReal < emptyReal));

    RealArray sizedReal(2);
    assertTrue_1(!(sizedReal < sizedReal));
    assertTrue_1(emptyReal < sizedReal);
    assertTrue_1(!(sizedReal < emptyReal));

    std::vector<double> dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray initedReal(dv);
    assertTrue_1(!(initedReal < initedReal));
    assertTrue_1(emptyReal < initedReal);
    assertTrue_1(!(initedReal < emptyReal));
    assertTrue_1(sizedReal < initedReal);
    assertTrue_1(!(initedReal < sizedReal));

    std::vector<double> dv2(2);
    dv2[0] = 3.14;
    dv2[1] = 4.6;
    RealArray initedReal2(dv2);
    assertTrue_1(!(initedReal2 < initedReal2));
    assertTrue_1(emptyReal < initedReal2);
    assertTrue_1(!(initedReal2 < emptyReal));
    assertTrue_1(sizedReal < initedReal2);
    assertTrue_1(!(initedReal2 < sizedReal));
    // Same size, contents determine the victor
    assertTrue_1(initedReal < initedReal2);
    assertTrue_1(!(initedReal2 < initedReal));
  }

  {
    StringArray emptyString;
    assertTrue_1(!(emptyString < emptyString));

    StringArray sizedString(2);
    assertTrue_1(!(sizedString < sizedString));
    assertTrue_1(emptyString < sizedString);
    assertTrue_1(!(sizedString < emptyString));

    std::vector<std::string> sv(2);
    sv[0] = std::string("yo ");
    sv[1] = std::string("mama");
    StringArray initedString(sv);
    assertTrue_1(!(initedString < initedString));
    assertTrue_1(emptyString < initedString);
    assertTrue_1(!(initedString < emptyString));
    assertTrue_1(sizedString < initedString);
    assertTrue_1(!(initedString < sizedString));

    std::vector<std::string> sv2(2);
    sv2[0] = std::string("yo ");
    sv2[1] = std::string("mamb");
    StringArray initedString2(sv2);
    assertTrue_1(!(initedString2 < initedString2));
    assertTrue_1(emptyString < initedString2);
    assertTrue_1(!(initedString2 < emptyString));
    assertTrue_1(sizedString < initedString2);
    assertTrue_1(!(initedString2 < sizedString));
    // Same size, contents determine the victor
    assertTrue_1(initedString < initedString2);
    assertTrue_1(!(initedString2 < initedString));
  }

  return true;
}

bool arrayTest()
{
  runTest(testConstructors);
  runTest(testResize);
  runTest(testSetters);
  runTest(testEquality);
  runTest(testLessThan);

  return true;
}

