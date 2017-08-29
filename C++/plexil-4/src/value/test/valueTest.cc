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

#include "ArrayImpl.hh"
#include "CommandHandle.hh"
#include "NodeConstants.hh"
#include "TestSupport.hh"
#include "Value.hh"

using namespace PLEXIL;

static bool testConstructorsAndAccessors()
{
  {
    Value *unkv = new Value;
    assertTrue_1(!unkv->isKnown());
    assertTrue_1(UNKNOWN_TYPE == unkv->valueType());
    delete unkv;
  }

  {
    Value *boolv = new Value(true);
    bool tempb;
    assertTrue_1(boolv->isKnown());
    assertTrue_1(BOOLEAN_TYPE == boolv->valueType());
    assertTrue_1(boolv->getValue(tempb));
    assertTrue_1(tempb);
    delete boolv;
  }

  {
    Value *intv = new Value((Integer) 42);
    Integer tempi;
    assertTrue_1(intv->isKnown());
    assertTrue_1(INTEGER_TYPE == intv->valueType());
    assertTrue_1(intv->getValue(tempi));
    assertTrue_1(tempi == 42);
    delete intv;
  }

  {
    Value *realv = new Value(2.5);
    Real tempr;
    assertTrue_1(realv->isKnown());
    assertTrue_1(REAL_TYPE == realv->valueType());
    assertTrue_1(realv->getValue(tempr));
    assertTrue_1(tempr == 2.5);
    delete realv;
  }

  {
    String temps;
    String const *tempsp;
    String foo("Foo");

    {
      Value *stringv = new Value(foo);
      assertTrue_1(stringv->isKnown());
      assertTrue_1(STRING_TYPE == stringv->valueType());
      assertTrue_1(stringv->getValue(temps));
      assertTrue_1(temps == foo);
      assertTrue_1(stringv->getValuePointer(tempsp));
      assertTrue_1(*tempsp == foo);
      delete stringv;
    }

    {
      Value *stringv2 = new Value(foo.c_str());
      assertTrue_1(stringv2->isKnown());
      assertTrue_1(STRING_TYPE == stringv2->valueType());
      assertTrue_1(stringv2->getValue(temps));
      assertTrue_1(temps == foo);
      assertTrue_1(stringv2->getValuePointer(tempsp));
      assertTrue_1(*tempsp == foo);
      delete stringv2;
    }
  }

  {
    BooleanArray const * tempbap;
    {
      BooleanArray *emptyBool = new BooleanArray;
      Value *bav = new Value(*emptyBool);
      assertTrue_1(bav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == bav->valueType());
      assertTrue_1(bav->getValuePointer(tempbap));
      assertTrue_1(*emptyBool == *tempbap);
      delete bav;
      delete emptyBool;
    }
    {
      BooleanArray *sizedBool = new BooleanArray(2);
      Value *sbav = new Value(*sizedBool);
      assertTrue_1(sbav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == sbav->valueType());
      assertTrue_1(sbav->getValuePointer(tempbap));
      assertTrue_1(*sizedBool == *tempbap);
      assertTrue_1(tempbap->size() == 2);
      delete sbav;
      delete sizedBool;
    }
    {
      std::vector<bool> bv(2);
      bv[0] = false;
      bv[1] = true;
      BooleanArray *initedBool = new BooleanArray(bv);
      Value *ibav = new Value(*initedBool);
      assertTrue_1(ibav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == ibav->valueType());
      assertTrue_1(ibav->getValuePointer(tempbap));
      assertTrue_1(*initedBool == *tempbap);
      assertTrue_1(tempbap->size() == 2);
      delete ibav;
      delete initedBool;
    }
  }

  {
    IntegerArray const * tempiap;
    {
      IntegerArray *emptyInt = new IntegerArray;
      Value *iav = new Value(*emptyInt);
      assertTrue_1(iav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == iav->valueType());
      assertTrue_1(iav->getValuePointer(tempiap));
      assertTrue_1(*emptyInt == *tempiap);
      delete iav;
      delete emptyInt;
    }

    {
      IntegerArray *sizedInt = new IntegerArray(2);
      Value *siav = new Value(*sizedInt);
      assertTrue_1(siav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == siav->valueType());
      assertTrue_1(siav->getValuePointer(tempiap));
      assertTrue_1(*sizedInt == *tempiap);
      delete siav;
      delete sizedInt;
    }

    {
      std::vector<Integer> iv(2);
      iv[0] = 42;
      iv[1] = 6;
      IntegerArray *initedInt = new IntegerArray(iv);
      Value *iiav = new Value(*initedInt);
      assertTrue_1(iiav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == iiav->valueType());
      assertTrue_1(iiav->getValuePointer(tempiap));
      assertTrue_1(*initedInt == *tempiap);
      delete iiav;
      delete initedInt;
    }
  }
  {
    RealArray const * temprap;
  
    {
      RealArray *emptyReal = new RealArray;
      Value *rav = new Value(*emptyReal);
      assertTrue_1(rav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == rav->valueType());
      assertTrue_1(rav->getValuePointer(temprap));
      assertTrue_1(*emptyReal == *temprap);
      delete rav;
      delete emptyReal;
    }
  
    {
      RealArray *sizedReal = new RealArray(2);
      Value *srav = new Value(*sizedReal);
      assertTrue_1(srav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == srav->valueType());
      assertTrue_1(srav->getValuePointer(temprap));
      assertTrue_1(*sizedReal == *temprap);
      delete srav;
      delete sizedReal;
    }

    {
      std::vector<Real> dv(2);
      dv[0] = 3.14;
      dv[1] = 4.5;
      RealArray *initedReal = new RealArray(dv);
      Value *irav = new Value(*initedReal);
      assertTrue_1(irav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == irav->valueType());
      assertTrue_1(irav->getValuePointer(temprap));
      assertTrue_1(*initedReal == *temprap);
      delete irav;
      delete initedReal;
    }
  }

  {
    StringArray const * tempsap;

    {
      StringArray *emptyString = new StringArray;
      Value *sav = new Value(*emptyString);
      assertTrue_1(sav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == sav->valueType());
      assertTrue_1(sav->getValuePointer(tempsap));
      assertTrue_1(*emptyString == *tempsap);
      delete sav;
      delete emptyString;
    }

    {
      StringArray *sizedString = new StringArray(2);
      Value *ssav = new Value(*sizedString);
      assertTrue_1(ssav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == ssav->valueType());
      assertTrue_1(ssav->getValuePointer(tempsap));
      assertTrue_1(*sizedString == *tempsap);
      delete ssav;
      delete sizedString;
    }

    {
      std::vector<String> sv(2);
      sv[0] = String("yo ");
      sv[1] = String("mama");
      StringArray *initedString = new StringArray(sv);
      Value *isav = new Value(*initedString);
      assertTrue_1(isav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == isav->valueType());
      assertTrue_1(isav->getValuePointer(tempsap));
      assertTrue_1(*initedString == *tempsap);
      delete isav;
      delete initedString;
    }
  }

  return true;
}

#if __cplusplus >= 201103L
static bool testMoveConstructors()
{
  {
    Value *unkv = new Value(Value());
    assertTrue_1(!unkv->isKnown());
    assertTrue_1(UNKNOWN_TYPE == unkv->valueType());
    delete unkv;
  }

  {
    Value *boolv = new Value(Value(true));
    bool tempb;
    assertTrue_1(boolv->isKnown());
    assertTrue_1(BOOLEAN_TYPE == boolv->valueType());
    assertTrue_1(boolv->getValue(tempb));
    assertTrue_1(tempb);
    delete boolv;
  }

  {
    Value *intv = new Value(Value((Integer) 42));
    Integer tempi;
    assertTrue_1(intv->isKnown());
    assertTrue_1(INTEGER_TYPE == intv->valueType());
    assertTrue_1(intv->getValue(tempi));
    assertTrue_1(tempi == 42);
    delete intv;
  }

  {
    Value *realv = new Value(Value(2.5));
    Real tempr;
    assertTrue_1(realv->isKnown());
    assertTrue_1(REAL_TYPE == realv->valueType());
    assertTrue_1(realv->getValue(tempr));
    assertTrue_1(tempr == 2.5);
    delete realv;
  }

  // Node enums
  {
    Value *statev = new Value(Value(INACTIVE_STATE, NODE_STATE_TYPE));
    uint16_t temps;
    assertTrue_1(statev->isKnown());
    assertTrue_1(NODE_STATE_TYPE == statev->valueType());
    assertTrue_1(statev->getValue(temps));
    assertTrue_1(temps == INACTIVE_STATE);
    delete statev;
  }

  {
    Value *outcomev = new Value(Value(SUCCESS_OUTCOME, OUTCOME_TYPE));
    uint16_t tempo;
    assertTrue_1(outcomev->isKnown());
    assertTrue_1(OUTCOME_TYPE == outcomev->valueType());
    assertTrue_1(outcomev->getValue(tempo));
    assertTrue_1(tempo == SUCCESS_OUTCOME);
    delete outcomev;
  }

  {
    Value *failv = new Value(Value(EXITED, FAILURE_TYPE));
    uint16_t tempf;
    assertTrue_1(failv->isKnown());
    assertTrue_1(FAILURE_TYPE == failv->valueType());
    assertTrue_1(failv->getValue(tempf));
    assertTrue_1(tempf == EXITED);
    delete failv;
  }

  {
    Value *handlev = new Value(Value(COMMAND_SUCCESS, COMMAND_HANDLE_TYPE));
    uint16_t temph;
    assertTrue_1(handlev->isKnown());
    assertTrue_1(COMMAND_HANDLE_TYPE == handlev->valueType());
    assertTrue_1(handlev->getValue(temph));
    assertTrue_1(temph == COMMAND_SUCCESS);
    delete handlev;
  }

  {
    String temps;
    String const *tempsp;
    String foo("Foo");

    {
      Value *stringv = new Value(Value(foo));
      assertTrue_1(stringv->isKnown());
      assertTrue_1(STRING_TYPE == stringv->valueType());
      assertTrue_1(stringv->getValue(temps));
      assertTrue_1(temps == foo);
      assertTrue_1(stringv->getValuePointer(tempsp));
      assertTrue_1(*tempsp == foo);
      delete stringv;
    }

    {
      Value *stringv2 = new Value(Value(foo.c_str()));
      assertTrue_1(stringv2->isKnown());
      assertTrue_1(STRING_TYPE == stringv2->valueType());
      assertTrue_1(stringv2->getValue(temps));
      assertTrue_1(temps == foo);
      assertTrue_1(stringv2->getValuePointer(tempsp));
      assertTrue_1(*tempsp == foo);
      delete stringv2;
    }
  }

  {
    BooleanArray const * tempbap;
    {
      BooleanArray *emptyBool = new BooleanArray;
      Value *bav = new Value(Value(*emptyBool));
      assertTrue_1(bav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == bav->valueType());
      assertTrue_1(bav->getValuePointer(tempbap));
      assertTrue_1(*emptyBool == *tempbap);
      delete bav;
      delete emptyBool;
    }
    {
      BooleanArray *sizedBool = new BooleanArray(2);
      Value *sbav = new Value(Value(*sizedBool));
      assertTrue_1(sbav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == sbav->valueType());
      assertTrue_1(sbav->getValuePointer(tempbap));
      assertTrue_1(*sizedBool == *tempbap);
      assertTrue_1(tempbap->size() == 2);
      delete sbav;
      delete sizedBool;
    }
    {
      std::vector<bool> bv(2);
      bv[0] = false;
      bv[1] = true;
      BooleanArray *initedBool = new BooleanArray(bv);
      Value *ibav = new Value(Value(*initedBool));
      assertTrue_1(ibav->isKnown());
      assertTrue_1(BOOLEAN_ARRAY_TYPE == ibav->valueType());
      assertTrue_1(ibav->getValuePointer(tempbap));
      assertTrue_1(*initedBool == *tempbap);
      assertTrue_1(tempbap->size() == 2);
      delete ibav;
      delete initedBool;
    }
  }

  {
    IntegerArray const * tempiap;
    {
      IntegerArray *emptyInt = new IntegerArray;
      Value *iav = new Value(Value(*emptyInt));
      assertTrue_1(iav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == iav->valueType());
      assertTrue_1(iav->getValuePointer(tempiap));
      assertTrue_1(*emptyInt == *tempiap);
      delete iav;
      delete emptyInt;
    }

    {
      IntegerArray *sizedInt = new IntegerArray(2);
      Value *siav = new Value(Value(*sizedInt));
      assertTrue_1(siav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == siav->valueType());
      assertTrue_1(siav->getValuePointer(tempiap));
      assertTrue_1(*sizedInt == *tempiap);
      delete siav;
      delete sizedInt;
    }

    {
      std::vector<Integer> iv(2);
      iv[0] = 42;
      iv[1] = 6;
      IntegerArray *initedInt = new IntegerArray(iv);
      Value *iiav = new Value(Value(*initedInt));
      assertTrue_1(iiav->isKnown());
      assertTrue_1(INTEGER_ARRAY_TYPE == iiav->valueType());
      assertTrue_1(iiav->getValuePointer(tempiap));
      assertTrue_1(*initedInt == *tempiap);
      delete iiav;
      delete initedInt;
    }
  }
  {
    RealArray const * temprap;
  
    {
      RealArray *emptyReal = new RealArray;
      Value *rav = new Value(Value(*emptyReal));
      assertTrue_1(rav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == rav->valueType());
      assertTrue_1(rav->getValuePointer(temprap));
      assertTrue_1(*emptyReal == *temprap);
      delete rav;
      delete emptyReal;
    }
  
    {
      RealArray *sizedReal = new RealArray(2);
      Value *srav = new Value(Value(*sizedReal));
      assertTrue_1(srav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == srav->valueType());
      assertTrue_1(srav->getValuePointer(temprap));
      assertTrue_1(*sizedReal == *temprap);
      delete srav;
      delete sizedReal;
    }

    {
      std::vector<Real> dv(2);
      dv[0] = 3.14;
      dv[1] = 4.5;
      RealArray *initedReal = new RealArray(dv);
      Value *irav = new Value(Value(*initedReal));
      assertTrue_1(irav->isKnown());
      assertTrue_1(REAL_ARRAY_TYPE == irav->valueType());
      assertTrue_1(irav->getValuePointer(temprap));
      assertTrue_1(*initedReal == *temprap);
      delete irav;
      delete initedReal;
    }
  }

  {
    StringArray const * tempsap;

    {
      StringArray *emptyString = new StringArray;
      Value *sav = new Value(Value(*emptyString));
      assertTrue_1(sav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == sav->valueType());
      assertTrue_1(sav->getValuePointer(tempsap));
      assertTrue_1(*emptyString == *tempsap);
      delete sav;
      delete emptyString;
    }

    {
      StringArray *sizedString = new StringArray(2);
      Value *ssav = new Value(Value(*sizedString));
      assertTrue_1(ssav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == ssav->valueType());
      assertTrue_1(ssav->getValuePointer(tempsap));
      assertTrue_1(*sizedString == *tempsap);
      delete ssav;
      delete sizedString;
    }

    {
      std::vector<String> sv(2);
      sv[0] = String("yo ");
      sv[1] = String("mama");
      StringArray *initedString = new StringArray(sv);
      Value *isav = new Value(Value(*initedString));
      assertTrue_1(isav->isKnown());
      assertTrue_1(STRING_ARRAY_TYPE == isav->valueType());
      assertTrue_1(isav->getValuePointer(tempsap));
      assertTrue_1(*initedString == *tempsap);
      delete isav;
      delete initedString;
    }
  }

  return true;
}
#endif

static bool testScalarEquality()
{
  // Basics
  Value *stringv = NULL;
  {
    String foo("Foo");
    stringv = new Value(foo);
  }
  Value *unkv = new Value;
  Value *tempv = new Value; // both type & value unknown
  Value *boolv = new Value(true);
  Value *intv = new Value((Integer) 42);
  Value *realv = new Value(2.5);

  // Identity
  assertTrue_1(*unkv == *unkv);
  assertTrue_1(!(*unkv != *unkv));

  assertTrue_1(*tempv == *tempv);
  assertTrue_1(!(*tempv != *tempv));

  assertTrue_1(*boolv == *boolv);
  assertTrue_1(!(*boolv != *boolv));

  assertTrue_1(*intv == *intv);
  assertTrue_1(!(*intv != *intv));

  assertTrue_1(*realv == *realv);
  assertTrue_1(!(*realv != *realv));

  assertTrue_1(*stringv == *stringv);
  assertTrue_1(!(*stringv != *stringv));

  // Unknown type, unknown value
  assertTrue_1(*unkv == *tempv);
  assertTrue_1(!(*unkv != *tempv));
  assertTrue_1(*tempv == *unkv);
  assertTrue_1(!(*tempv != *unkv));

  // Across types
  assertTrue_1(!(*unkv == *boolv));
  assertTrue_1(*unkv != *boolv);
  assertTrue_1(!(*boolv == *unkv));
  assertTrue_1(*boolv != *unkv);

  assertTrue_1(!(*unkv == *intv));
  assertTrue_1(*unkv != *intv);
  assertTrue_1(!(*intv == *unkv));
  assertTrue_1(*intv != *unkv);

  assertTrue_1(!(*unkv == *realv));
  assertTrue_1(*unkv != *realv);
  assertTrue_1(!(*realv == *unkv));
  assertTrue_1(*realv != *unkv);

  assertTrue_1(!(*unkv == *stringv));
  assertTrue_1(*unkv != *stringv);
  assertTrue_1(!(*stringv == *unkv));
  assertTrue_1(*stringv != *unkv);

  assertTrue_1(!(*boolv == *intv));
  assertTrue_1(*boolv != *intv);
  assertTrue_1(!(*intv == *boolv));
  assertTrue_1(*intv != *boolv);

  assertTrue_1(!(*boolv == *realv));
  assertTrue_1(*boolv != *realv);
  assertTrue_1(!(*realv == *boolv));
  assertTrue_1(*realv != *boolv);

  assertTrue_1(!(*boolv == *stringv));
  assertTrue_1(*boolv != *stringv);
  assertTrue_1(!(*stringv == *boolv));
  assertTrue_1(*stringv != *boolv);

  assertTrue_1(!(*intv == *realv));
  assertTrue_1(*intv != *realv);
  assertTrue_1(!(*realv == *intv));
  assertTrue_1(*realv != *intv);

  assertTrue_1(!(*intv == *stringv));
  assertTrue_1(*intv != *stringv);
  assertTrue_1(!(*stringv == *intv));
  assertTrue_1(*stringv != *intv);

  assertTrue_1(!(*realv == *stringv));
  assertTrue_1(*realv != *stringv);
  assertTrue_1(!(*stringv == *realv));
  assertTrue_1(*stringv != *realv);

  // Via assignment
  *tempv = *unkv;
  assertTrue_1(*tempv == *unkv);
  assertTrue_1(!(*tempv != *unkv));
  assertTrue_1(*unkv == *tempv);
  assertTrue_1(!(*unkv != *tempv));
  
  *tempv = *boolv;
  assertTrue_1(*tempv == *boolv);
  assertTrue_1(!(*tempv != *boolv));
  assertTrue_1(*boolv == *tempv);
  assertTrue_1(!(*boolv != *tempv));
  
  *tempv = *intv;
  assertTrue_1(*tempv == *intv);
  assertTrue_1(!(*tempv != *intv));
  assertTrue_1(*intv == *tempv);
  assertTrue_1(!(*intv != *tempv));
  
  *tempv = *realv;
  assertTrue_1(*tempv == *realv);
  assertTrue_1(!(*tempv != *realv));
  assertTrue_1(*realv == *tempv);
  assertTrue_1(!(*realv != *tempv));
  
  *tempv = *stringv;
  assertTrue_1(*tempv == *stringv);
  assertTrue_1(!(*tempv != *stringv));
  assertTrue_1(*stringv == *tempv);
  assertTrue_1(!(*stringv != *tempv));

#if __cplusplus >= 201103L
  //
  // Via move assignment
  //
  *tempv = Value(*unkv);
  assertTrue_1(*tempv == *unkv);
  assertTrue_1(!(*tempv != *unkv));
  assertTrue_1(*unkv == *tempv);
  assertTrue_1(!(*unkv != *tempv));
  
  *tempv = Value(*boolv);
  assertTrue_1(*tempv == *boolv);
  assertTrue_1(!(*tempv != *boolv));
  assertTrue_1(*boolv == *tempv);
  assertTrue_1(!(*boolv != *tempv));
  
  *tempv = Value(*intv);
  assertTrue_1(*tempv == *intv);
  assertTrue_1(!(*tempv != *intv));
  assertTrue_1(*intv == *tempv);
  assertTrue_1(!(*intv != *tempv));
  
  *tempv = Value(*realv);
  assertTrue_1(*tempv == *realv);
  assertTrue_1(!(*tempv != *realv));
  assertTrue_1(*realv == *tempv);
  assertTrue_1(!(*realv != *tempv));
  
  *tempv = Value(*stringv);
  assertTrue_1(*tempv == *stringv);
  assertTrue_1(!(*tempv != *stringv));
  assertTrue_1(*stringv == *tempv);
  assertTrue_1(!(*stringv != *tempv));
#endif

  // Real vs integer
  {
    Value *real42v = new Value((Real) 42);
    assertTrue_1(real42v->valueType() == REAL_TYPE);
    assertTrue_1(*intv == *real42v);
    assertTrue_1(!(*intv != *real42v));
    assertTrue_1(*real42v == *intv);
    assertTrue_1(!(*real42v != *intv));

    assertTrue_1(!(*realv == *real42v));
    assertTrue_1(*realv != *real42v);
    assertTrue_1(!(*real42v == *realv));
    assertTrue_1(*real42v != *realv);

    delete real42v;
  }

  delete realv;
  delete intv;
  delete boolv;
  delete tempv;
  delete unkv;

  return true;
}

static bool testBooleanArrayEquality()
{
  Value *bav = NULL;
  {
    BooleanArray *emptyBool = new BooleanArray;
    bav = new Value(*emptyBool);
    delete emptyBool;
  }

  assertTrue_1(*bav == *bav);
  assertTrue_1(!(*bav != *bav));

  Value *sbav = NULL;
  {
    BooleanArray *sizedBool = new BooleanArray(2);
    sbav = new Value(*sizedBool);
    delete sizedBool;
  }

  assertTrue_1(*sbav == *sbav);
  assertTrue_1(!(*sbav != *sbav));

  assertTrue_1(!(*bav == *sbav));
  assertTrue_1(*bav != *sbav);
  assertTrue_1(!(*sbav == *bav));
  assertTrue_1(*sbav != *bav);

  Value *ibav = NULL;
  {
    std::vector<bool> bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray *initedBool = new BooleanArray(bv);
    ibav = new Value(*initedBool);
    delete initedBool;
  }

  assertTrue_1(*ibav == *ibav);
  assertTrue_1(!(*ibav != *ibav));

  assertTrue_1(!(*bav == *ibav));
  assertTrue_1(*bav != *ibav);
  assertTrue_1(!(*ibav == *bav));
  assertTrue_1(*ibav != *bav);

  assertTrue_1(!(*sbav == *ibav));
  assertTrue_1(*sbav != *ibav);
  assertTrue_1(!(*ibav == *sbav));
  assertTrue_1(*ibav != *sbav);

  Value *tempv = new Value;

  *tempv = *bav;
  assertTrue_1(*tempv == *bav);
  assertTrue_1(!(*tempv != *bav));
  assertTrue_1(*bav == *tempv);
  assertTrue_1(!(*bav != *tempv));

  *tempv = *sbav;
  assertTrue_1(*tempv == *sbav);
  assertTrue_1(!(*tempv != *sbav));
  assertTrue_1(*sbav == *tempv);
  assertTrue_1(!(*sbav != *tempv));

  *tempv = *ibav;
  assertTrue_1(*tempv == *ibav);
  assertTrue_1(!(*tempv != *ibav));
  assertTrue_1(*ibav == *tempv);
  assertTrue_1(!(*ibav != *tempv));

  delete tempv;

  Value *unkv = new Value;
  assertTrue_1(!(*unkv == *bav));
  assertTrue_1(*unkv != *bav);
  assertTrue_1(!(*bav == *unkv));
  assertTrue_1(*bav != *unkv);

  assertTrue_1(!(*unkv == *ibav));
  assertTrue_1(*unkv != *ibav);
  assertTrue_1(!(*ibav == *unkv));
  assertTrue_1(*ibav != *unkv);

  assertTrue_1(!(*unkv == *sbav));
  assertTrue_1(*unkv != *sbav);
  assertTrue_1(!(*sbav == *unkv));
  assertTrue_1(*sbav != *unkv);

  delete unkv;
  delete ibav;
  delete sbav;
  delete bav;

  return true;
}

static bool testIntegerArrayEquality()
{
  Value *iav = NULL;
  {
    IntegerArray *emptyInt = new IntegerArray;
    iav = new Value(*emptyInt);
    delete emptyInt;
  }

  assertTrue_1(*iav == *iav);
  assertTrue_1(!(*iav != *iav));

  Value *siav = NULL;
  {
    IntegerArray *sizedInt = new IntegerArray(2);
    siav = new Value(*sizedInt);
    delete sizedInt;
  }

  assertTrue_1(*siav == *siav);
  assertTrue_1(!(*siav != *siav));

  assertTrue_1(!(*iav == *siav));
  assertTrue_1(*iav != *siav);
  assertTrue_1(!(*siav == *iav));
  assertTrue_1(*siav != *iav);

  Value *iiav = NULL;
  {
    std::vector<Integer> iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray *initedInt = new IntegerArray(iv);
    iiav = new Value(*initedInt);
    delete initedInt;
  }

  assertTrue_1(*iiav == *iiav);
  assertTrue_1(!(*iiav != *iiav));

  assertTrue_1(!(*iav == *iiav));
  assertTrue_1(*iav != *iiav);
  assertTrue_1(!(*iiav == *iav));
  assertTrue_1(*iiav != *iav);

  assertTrue_1(!(*siav == *iiav));
  assertTrue_1(*siav != *iiav);
  assertTrue_1(!(*iiav == *siav));
  assertTrue_1(*iiav != *siav);

  Value *tempv = new Value;

  *tempv = *iav;
  assertTrue_1(*tempv == *iav);
  assertTrue_1(!(*tempv != *iav));
  assertTrue_1(*iav == *tempv);
  assertTrue_1(!(*iav != *tempv));

  *tempv = *siav;
  assertTrue_1(*tempv == *siav);
  assertTrue_1(!(*tempv != *siav));
  assertTrue_1(*siav == *tempv);
  assertTrue_1(!(*siav != *tempv));

  *tempv = *iiav;
  assertTrue_1(*tempv == *iiav);
  assertTrue_1(!(*tempv != *iiav));
  assertTrue_1(*iiav == *tempv);
  assertTrue_1(!(*iiav != *tempv));

  delete tempv;

  Value *unkv = new Value;

  assertTrue_1(!(*unkv == *iav));
  assertTrue_1(*unkv != *iav);
  assertTrue_1(!(*iav == *unkv));
  assertTrue_1(*iav != *unkv);

  assertTrue_1(!(*unkv == *iiav));
  assertTrue_1(*unkv != *iiav);
  assertTrue_1(!(*iiav == *unkv));
  assertTrue_1(*iiav != *unkv);

  assertTrue_1(!(*unkv == *siav));
  assertTrue_1(*unkv != *siav);
  assertTrue_1(!(*siav == *unkv));
  assertTrue_1(*siav != *unkv);

  delete unkv;
  delete iiav;
  delete siav;
  delete iav;

  return true;
}

static bool testRealArrayEquality()
{
  Value *rav = NULL;
  {
    RealArray *emptyReal = new RealArray;
    rav = new Value(*emptyReal);
    delete emptyReal;
  }

  assertTrue_1(*rav == *rav);
  assertTrue_1(!(*rav != *rav));

  Value *srav = NULL;
  {
    RealArray *sizedReal = new RealArray(2);
    srav = new Value(*sizedReal);
    delete sizedReal;
  }

  assertTrue_1(*srav == *srav);
  assertTrue_1(!(*srav != *srav));

  assertTrue_1(!(*rav == *srav));
  assertTrue_1(*rav != *srav);
  assertTrue_1(!(*srav == *rav));
  assertTrue_1(*srav != *rav);

  Value *irav = NULL;
  {
    std::vector<Real> dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray* initedReal = new RealArray(dv);
    irav = new Value(*initedReal);
    delete initedReal;
  }

  assertTrue_1(*irav == *irav);
  assertTrue_1(!(*irav != *irav));

  assertTrue_1(!(*rav == *irav));
  assertTrue_1(*rav != *irav);
  assertTrue_1(!(*irav == *rav));
  assertTrue_1(*irav != *rav);

  assertTrue_1(!(*srav == *irav));
  assertTrue_1(*srav != *irav);
  assertTrue_1(!(*irav == *srav));
  assertTrue_1(*irav != *srav);

  Value *tempv = new Value;

  *tempv = *rav;
  assertTrue_1(*tempv == *rav);
  assertTrue_1(!(*tempv != *rav));
  assertTrue_1(*rav == *tempv);
  assertTrue_1(!(*rav != *tempv));

  *tempv = *srav;
  assertTrue_1(*tempv == *srav);
  assertTrue_1(!(*tempv != *srav));
  assertTrue_1(*srav == *tempv);
  assertTrue_1(!(*srav != *tempv));

  *tempv = *irav;
  assertTrue_1(*tempv == *irav);
  assertTrue_1(!(*tempv != *irav));
  assertTrue_1(*irav == *tempv);
  assertTrue_1(!(*irav != *tempv));

  delete tempv;

  Value *unkv = new Value;

  assertTrue_1(!(*unkv == *rav));
  assertTrue_1(*unkv != *rav);
  assertTrue_1(!(*rav == *unkv));
  assertTrue_1(*rav != *unkv);

  assertTrue_1(!(*unkv == irav));
  assertTrue_1(*unkv != irav);
  assertTrue_1(!(*irav == *unkv));
  assertTrue_1(*irav != *unkv);

  assertTrue_1(!(*unkv == *srav));
  assertTrue_1(*unkv != *srav);
  assertTrue_1(!(*srav == *unkv));
  assertTrue_1(*srav != *unkv);

  delete unkv;
  delete irav;
  delete srav;
  delete rav;

  return true;
}

static bool testStringArrayEquality()
{
  Value *sav = NULL;
  {
    StringArray *emptyString = new StringArray;
    sav = new Value(*emptyString);
    delete emptyString;
  }

  assertTrue_1(*sav == *sav);
  assertTrue_1(!(*sav != *sav));

  Value *ssav;
  {
    StringArray *sizedString = new StringArray(2);
    ssav = new Value(*sizedString);
    delete sizedString;
  }

  assertTrue_1(*ssav == *ssav);
  assertTrue_1(!(*ssav != *ssav));

  assertTrue_1(!(*sav == *ssav));
  assertTrue_1(*sav != *ssav);
  assertTrue_1(!(*ssav == *sav));
  assertTrue_1(*ssav != *sav);

  Value *isav = NULL;
  {
    std::vector<String> sv(2);
    sv[0] = String("yo ");
    sv[1] = String("mama");
    StringArray *initedString = new StringArray(sv);
    isav = new Value(*initedString);
    delete initedString;
  }

  assertTrue_1(*isav == *isav);
  assertTrue_1(!(*isav != *isav));

  assertTrue_1(!(sav == *isav));
  assertTrue_1(sav != *isav);
  assertTrue_1(!(*isav == sav));
  assertTrue_1(*isav != sav);

  assertTrue_1(!(*ssav == *isav));
  assertTrue_1(*ssav != *isav);
  assertTrue_1(!(*isav == *ssav));
  assertTrue_1(*isav != *ssav);

  Value *tempv = new Value;

  *tempv = *sav;
  assertTrue_1(*tempv == *sav);
  assertTrue_1(!(*tempv != *sav));
  assertTrue_1(*sav == *tempv);
  assertTrue_1(!(*sav != *tempv));

  *tempv = *ssav;
  assertTrue_1(*tempv == *ssav);
  assertTrue_1(!(*tempv != *ssav));
  assertTrue_1(*ssav == *tempv);
  assertTrue_1(!(*ssav != *tempv));

  *tempv = *isav;
  assertTrue_1(*tempv == *isav);
  assertTrue_1(!(*tempv != *isav));
  assertTrue_1(*isav == *tempv);
  assertTrue_1(!(*isav != *tempv));

  delete tempv;

  Value *unkv = new Value;

  assertTrue_1(!(*unkv == *sav));
  assertTrue_1(*unkv != *sav);
  assertTrue_1(!(*sav == *unkv));
  assertTrue_1(*sav != *unkv);

  assertTrue_1(!(*unkv == *isav));
  assertTrue_1(*unkv != *isav);
  assertTrue_1(!(*isav == *unkv));
  assertTrue_1(*isav != *unkv);

  assertTrue_1(!(*unkv == *ssav));
  assertTrue_1(*unkv != *ssav);
  assertTrue_1(!(*ssav == *unkv));
  assertTrue_1(*ssav != *unkv);

  delete unkv;
  delete isav;
  delete ssav;
  delete sav;
 
  return true;
}

// Cross-type testing of array equality
static bool testEmptyArrayEquality()
{
  Value *sav = NULL;
  {
    StringArray *emptyString = new StringArray;
    sav = new Value(*emptyString);
    delete emptyString;
  }

  Value *rav = NULL;
  {
    RealArray *emptyReal = new RealArray;
    rav = new Value(*emptyReal);
    delete emptyReal;
  }

  {
    Value *iav = NULL;
    {
      IntegerArray *emptyInt = new IntegerArray;
      iav = new Value(*emptyInt);
      delete emptyInt;
    }

    {
      Value *bav = NULL;
      {
	BooleanArray *emptyBool = new BooleanArray;
	bav = new Value(*emptyBool);
	delete emptyBool;
      }

      assertTrue_1(!(*bav == *iav));
      assertTrue_1(*bav != *iav);
      assertTrue_1(!(*iav == *bav));
      assertTrue_1(*iav != *bav);

      assertTrue_1(!(*bav == rav));
      assertTrue_1(*bav != rav);
      assertTrue_1(!(rav == *bav));
      assertTrue_1(rav != *bav);

      assertTrue_1(!(*bav == *sav));
      assertTrue_1(*bav != *sav);
      assertTrue_1(!(*sav == *bav));
      assertTrue_1(*sav != *bav);

      delete bav;
    }

    assertTrue_1(!(*iav == *rav));
    assertTrue_1(*iav != *rav);
    assertTrue_1(!(*rav == *iav));
    assertTrue_1(*rav != *iav);

    assertTrue_1(!(*iav == *sav));
    assertTrue_1(*iav != *sav);
    assertTrue_1(!(*sav == *iav));
    assertTrue_1(*sav != *iav);

    delete iav;
  }

  assertTrue_1(!(*rav == *sav));
  assertTrue_1(*rav != *sav);
  assertTrue_1(!(*sav == *rav));
  assertTrue_1(*sav != *rav);

  delete rav;
  delete sav;

  return true;
}

static bool testScalarBooleanArrayEquality()
{
  // Boolean arrays
  Value *bav = NULL;
  {
    BooleanArray *emptyBool = new BooleanArray;
    bav = new Value(*emptyBool);
    delete emptyBool;
  }
  Value *sbav = NULL;
  {
    BooleanArray *sizedBool = new BooleanArray(2);
    sbav = new Value(*sizedBool);
    delete sizedBool;
  }
  Value *ibav = NULL;
  {
    std::vector<bool> bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray *initedBool = new BooleanArray(bv);
    ibav = new Value(*initedBool);
    delete initedBool;
  }

  {
    Value *boolv = new Value(true);

    assertTrue_1(!(*boolv == *bav));
    assertTrue_1(*boolv != *bav);
    assertTrue_1(!(*bav == *boolv));
    assertTrue_1(*bav != *boolv);

    assertTrue_1(!(*boolv == *ibav));
    assertTrue_1(*boolv != *ibav);
    assertTrue_1(!(*ibav == *boolv));
    assertTrue_1(*ibav != *boolv);

    assertTrue_1(!(*boolv == *sbav));
    assertTrue_1(*boolv != *sbav);
    assertTrue_1(!(*sbav == *boolv));
    assertTrue_1(*sbav != *boolv);

    delete boolv;
  }

  {
    Value *intv = new Value((Integer) 42);

    assertTrue_1(!(*intv == *bav));
    assertTrue_1(*intv != *bav);
    assertTrue_1(!(*bav == *intv));
    assertTrue_1(*bav != *intv);

    assertTrue_1(!(*intv == *ibav));
    assertTrue_1(*intv != *ibav);
    assertTrue_1(!(*ibav == *intv));
    assertTrue_1(*ibav != *intv);

    assertTrue_1(!(*intv == *sbav));
    assertTrue_1(*intv != *sbav);
    assertTrue_1(!(*sbav == *intv));
    assertTrue_1(*sbav != *intv);

    delete intv;
  }

  {
    Value *realv = new Value(2.5);

    assertTrue_1(!(*realv == *bav));
    assertTrue_1(*realv != *bav);
    assertTrue_1(!(*bav == *realv));
    assertTrue_1(*bav != *realv);

    assertTrue_1(!(*realv == *ibav));
    assertTrue_1(*realv != *ibav);
    assertTrue_1(!(*ibav == *realv));
    assertTrue_1(*ibav != *realv);

    assertTrue_1(!(*realv == *sbav));
    assertTrue_1(*realv != *sbav);
    assertTrue_1(!(*sbav == *realv));
    assertTrue_1(*sbav != *realv);

    delete realv;
  }

  {
    Value *stringv = new Value("Foo");;

    assertTrue_1(!(*stringv == *bav));
    assertTrue_1(*stringv != *bav);
    assertTrue_1(!(*bav == *stringv));
    assertTrue_1(*bav != *stringv);

    assertTrue_1(!(*stringv == *ibav));
    assertTrue_1(*stringv != *ibav);
    assertTrue_1(!(*ibav == *stringv));
    assertTrue_1(*ibav != *stringv);

    assertTrue_1(!(*stringv == *sbav));
    assertTrue_1(*stringv != *sbav);
    assertTrue_1(!(*sbav == *stringv));
    assertTrue_1(*sbav != *stringv);

    delete stringv;
  }

  delete ibav;
  delete sbav;
  delete bav;

  return true;
}

static bool testScalarIntegerArrayEquality()
{
  Value *iav = NULL;
  {
    IntegerArray *emptyInt = new IntegerArray;
    iav = new Value(*emptyInt);
    delete emptyInt;
  }
  Value *siav = NULL;
  {
    IntegerArray *sizedInt = new IntegerArray(2);
    siav = new Value(*sizedInt);
    delete sizedInt;
  }
  Value *iiav = NULL;
  {
    std::vector<Integer> iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray *initedInt = new IntegerArray(iv);
    iiav = new Value(*initedInt);
    delete initedInt;
  }

  {
    Value *boolv = new Value(true);

    assertTrue_1(!(*boolv == *iav));
    assertTrue_1(*boolv != *iav);
    assertTrue_1(!(*iav == *boolv));
    assertTrue_1(*iav != *boolv);

    assertTrue_1(!(*boolv == *iiav));
    assertTrue_1(*boolv != *iiav);
    assertTrue_1(!(*iiav == *boolv));
    assertTrue_1(*iiav != *boolv);

    assertTrue_1(!(*boolv == *siav));
    assertTrue_1(*boolv != *siav);
    assertTrue_1(!(*siav == *boolv));
    assertTrue_1(*siav != *boolv);

    delete boolv;
  }

  {
    Value *intv = new Value((Integer) 42);

    assertTrue_1(!(*intv == *iav));
    assertTrue_1(*intv != *iav);
    assertTrue_1(!(*iav == *intv));
    assertTrue_1(*iav != *intv);

    assertTrue_1(!(*intv == *iiav));
    assertTrue_1(*intv != *iiav);
    assertTrue_1(!(*iiav == *intv));
    assertTrue_1(*iiav != *intv);

    assertTrue_1(!(*intv == *siav));
    assertTrue_1(*intv != *siav);
    assertTrue_1(!(*siav == *intv));
    assertTrue_1(*siav != *intv);

    delete intv;
  }

  {
    Value *realv = new Value(2.5);

    assertTrue_1(!(*realv == *iav));
    assertTrue_1(*realv != *iav);
    assertTrue_1(!(*iav == *realv));
    assertTrue_1(*iav != *realv);

    assertTrue_1(!(*realv == *iiav));
    assertTrue_1(*realv != *iiav);
    assertTrue_1(!(*iiav == *realv));
    assertTrue_1(*iiav != *realv);

    assertTrue_1(!(*realv == *siav));
    assertTrue_1(*realv != *siav);
    assertTrue_1(!(*siav == *realv));
    assertTrue_1(*siav != *realv);

    delete realv;
  }

  {
    Value *stringv = new Value("Foo");;

    assertTrue_1(!(*stringv == *iav));
    assertTrue_1(*stringv != *iav);
    assertTrue_1(!(*iav == *stringv));
    assertTrue_1(*iav != *stringv);

    assertTrue_1(!(*stringv == *iiav));
    assertTrue_1(*stringv != *iiav);
    assertTrue_1(!(*iiav == *stringv));
    assertTrue_1(*iiav != *stringv);

    assertTrue_1(!(*stringv == *siav));
    assertTrue_1(*stringv != *siav);
    assertTrue_1(!(*siav == *stringv));
    assertTrue_1(*siav != *stringv);

    delete stringv;
  }

  delete iiav;
  delete siav;
  delete iav;

  return true;
}

static bool testScalarRealArrayEquality()
{
  Value *rav = NULL;
  {
    RealArray *emptyReal = new RealArray;
    rav = new Value(*emptyReal);
    delete emptyReal;
  }
  Value *srav = NULL;
  {
    RealArray *sizedReal = new RealArray(2);
    srav = new Value(*sizedReal);
    delete sizedReal;
  }
  Value *irav = NULL;
  {
    std::vector<Real> dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray* initedReal = new RealArray(dv);
    irav = new Value(*initedReal);
    delete initedReal;
  }

  {
    Value *boolv = new Value(true);

    assertTrue_1(!(*boolv == *rav));
    assertTrue_1(*boolv != *rav);
    assertTrue_1(!(*rav == *boolv));
    assertTrue_1(*rav != *boolv);

    assertTrue_1(!(*boolv == *irav));
    assertTrue_1(*boolv != *irav);
    assertTrue_1(!(*irav == *boolv));
    assertTrue_1(*irav != *boolv);

    assertTrue_1(!(*boolv == *srav));
    assertTrue_1(*boolv != *srav);
    assertTrue_1(!(*srav == *boolv));
    assertTrue_1(*srav != *boolv);

    delete boolv;
  }

  {
    Value *intv = new Value((Integer) 42);

    assertTrue_1(!(*intv == *rav));
    assertTrue_1(*intv != *rav);
    assertTrue_1(!(*rav == *intv));
    assertTrue_1(*rav != *intv);

    assertTrue_1(!(*intv == *irav));
    assertTrue_1(*intv != *irav);
    assertTrue_1(!(*irav == *intv));
    assertTrue_1(*irav != *intv);

    assertTrue_1(!(*intv == *srav));
    assertTrue_1(*intv != *srav);
    assertTrue_1(!(*srav == *intv));
    assertTrue_1(*srav != *intv);

    delete intv;
  }

  {
    Value *realv = new Value(2.5);

    assertTrue_1(!(*realv == *rav));
    assertTrue_1(*realv != *rav);
    assertTrue_1(!(*rav == *realv));
    assertTrue_1(*rav != *realv);

    assertTrue_1(!(*realv == *irav));
    assertTrue_1(*realv != *irav);
    assertTrue_1(!(*irav == *realv));
    assertTrue_1(*irav != *realv);

    assertTrue_1(!(*realv == *srav));
    assertTrue_1(*realv != *srav);
    assertTrue_1(!(*srav == *realv));
    assertTrue_1(*srav != *realv);

    delete realv;
  }

  {
    Value *stringv = new Value("Foo");;

    assertTrue_1(!(*stringv == *rav));
    assertTrue_1(*stringv != *rav);
    assertTrue_1(!(*rav == *stringv));
    assertTrue_1(*rav != *stringv);

    assertTrue_1(!(*stringv == *irav));
    assertTrue_1(*stringv != *irav);
    assertTrue_1(!(*irav == *stringv));
    assertTrue_1(*irav != *stringv);

    assertTrue_1(!(*stringv == *srav));
    assertTrue_1(*stringv != *srav);
    assertTrue_1(!(*srav == *stringv));
    assertTrue_1(*srav != *stringv);

    delete stringv;
  }

  delete irav;
  delete srav;
  delete rav;

  return true;
}

static bool testScalarStringArrayEquality()
{
  Value *sav = NULL;
  {
    StringArray *emptyString = new StringArray;
    sav = new Value(*emptyString);
    delete emptyString;
  }
  Value *ssav;
  {
    StringArray *sizedString = new StringArray(2);
    ssav = new Value(*sizedString);
    delete sizedString;
  }
  Value *isav = NULL;
  {
    std::vector<String> sv(2);
    sv[0] = String("yo ");
    sv[1] = String("mama");
    StringArray *initedString = new StringArray(sv);
    isav = new Value(*initedString);
    delete initedString;
  }

  {
    Value *boolv = new Value(true);

    assertTrue_1(!(*boolv == *sav));
    assertTrue_1(*boolv != *sav);
    assertTrue_1(!(*sav == *boolv));
    assertTrue_1(*sav != *boolv);

    assertTrue_1(!(*boolv == *isav));
    assertTrue_1(*boolv != *isav);
    assertTrue_1(!(*isav == *boolv));
    assertTrue_1(*isav != *boolv);

    assertTrue_1(!(*boolv == *ssav));
    assertTrue_1(*boolv != *ssav);
    assertTrue_1(!(*ssav == *boolv));
    assertTrue_1(*ssav != *boolv);

    delete boolv;
  }

  {
    Value *intv = new Value((Integer) 42);

    assertTrue_1(!(*intv == *sav));
    assertTrue_1(*intv != *sav);
    assertTrue_1(!(*sav == *intv));
    assertTrue_1(*sav != *intv);

    assertTrue_1(!(*intv == *isav));
    assertTrue_1(*intv != *isav);
    assertTrue_1(!(*isav == *intv));
    assertTrue_1(*isav != *intv);

    assertTrue_1(!(*intv == *ssav));
    assertTrue_1(*intv != *ssav);
    assertTrue_1(!(*ssav == *intv));
    assertTrue_1(*ssav != *intv);

    delete intv;
  }

  {
    Value *realv = new Value(2.5);

    assertTrue_1(!(*realv == *sav));
    assertTrue_1(*realv != *sav);
    assertTrue_1(!(*sav == *realv));
    assertTrue_1(*sav != *realv);

    assertTrue_1(!(*realv == *isav));
    assertTrue_1(*realv != *isav);
    assertTrue_1(!(*isav == *realv));
    assertTrue_1(*isav != *realv);

    assertTrue_1(!(*realv == *ssav));
    assertTrue_1(*realv != *ssav);
    assertTrue_1(!(*ssav == *realv));
    assertTrue_1(*ssav != *realv);

    delete realv;
  }
  
  {
    Value *stringv = new Value("Foo");;

    assertTrue_1(!(*stringv == *sav));
    assertTrue_1(*stringv != *sav);
    assertTrue_1(!(*sav == *stringv));
    assertTrue_1(*sav != *stringv);

    assertTrue_1(!(*stringv == *isav));
    assertTrue_1(*stringv != *isav);
    assertTrue_1(!(*isav == *stringv));
    assertTrue_1(*isav != *stringv);

    assertTrue_1(!(*stringv == *ssav));
    assertTrue_1(*stringv != *ssav);
    assertTrue_1(!(*ssav == *stringv));
    assertTrue_1(*ssav != *stringv);

    delete stringv;
  }

  delete isav;
  delete ssav;
  delete sav;

  return true;
}

static bool testScalarLessThan()
{
  // Basics
  
  // both type & value unknown
  Value *unkv = new Value;
  {
    Value *tempv = new Value;
    assertTrue_1(!(*unkv < *unkv));
    assertTrue_1(!(*unkv < *tempv));
    assertTrue_1(!(*tempv < *unkv));
    delete tempv;
  }

  // unknown but typed
  Value *unkbool = new Value(0, BOOLEAN_TYPE);
  Value *unkint = new Value(0, INTEGER_TYPE);
  Value *unkreal = new Value(0, REAL_TYPE);
  Value *unkstr = new Value(0, STRING_TYPE);
  assertTrue_1(!(*unkbool < *unkbool));
  assertTrue_1(!(*unkint < *unkint));
  assertTrue_1(!(*unkreal < *unkreal));
  assertTrue_1(!(*unkstr < *unkstr));
  // Unknown unknown always less than typed unknown
  assertTrue_1(*unkv < *unkbool);
  assertTrue_1(!(*unkbool < *unkv));
  assertTrue_1(*unkv < *unkint);
  assertTrue_1(!(*unkint < *unkv));
  assertTrue_1(*unkv < *unkreal);
  assertTrue_1(!(*unkreal < *unkv));
  assertTrue_1(*unkv < *unkstr);
  assertTrue_1(!(*unkstr < *unkv));

  Value *troo = new Value(true);
  Value *falls = new Value(false);
  assertTrue_1(!(*troo < *troo));
  assertTrue_1(!(*falls < *falls));
  assertTrue_1(!(*troo < *falls));
  assertTrue_1(*falls < *troo);

  // Unknown unknown always < typed known
  assertTrue_1(*unkv < *troo);
  assertTrue_1(!(*troo < *unkv));
  assertTrue_1(*unkv < *falls);
  assertTrue_1(!(*falls < *unkv));

  // Same type unknown always < known
  assertTrue_1(*unkbool < *falls);
  assertTrue_1(*unkbool < *troo);
  assertTrue_1(!(*falls < *unkbool));
  assertTrue_1(!(*troo < *unkbool));

  // Greater type unknown always < known
  assertTrue_1(*unkint < *falls);
  assertTrue_1(*unkint < *troo);
  assertTrue_1(!(*falls < *unkint));
  assertTrue_1(!(*troo < *unkint));

  assertTrue_1(*unkreal < *falls);
  assertTrue_1(*unkreal < *troo);
  assertTrue_1(!(*falls < *unkreal));
  assertTrue_1(!(*troo < *unkreal));

  assertTrue_1(*unkstr < *falls);
  assertTrue_1(*unkstr < *troo);
  assertTrue_1(!(*falls < *unkstr));
  assertTrue_1(!(*troo < *unkstr));

  delete falls;

  Value *fortytwo = new Value((Integer) 42);
  {
    Value *fortythree = new Value((Integer) 43);
    assertTrue_1(!(*fortytwo < *fortytwo));
    assertTrue_1(!(*fortythree < *fortythree));
    assertTrue_1(*fortytwo < *fortythree);
    assertTrue_1(!(*fortythree < *fortytwo));

    assertTrue_1(*unkv < *fortytwo);
    assertTrue_1(!(*fortytwo < *unkv));
    assertTrue_1(*unkv < *fortythree);
    assertTrue_1(!(*fortythree < *unkv));

    // Lesser typed unknown always less than typed known
    assertTrue_1(*unkbool < *fortytwo);
    assertTrue_1(!(*fortytwo < *unkbool));
    assertTrue_1(*unkbool < *fortythree);
    assertTrue_1(!(*fortythree < *unkbool));

    assertTrue_1(*unkint < *fortytwo);
    assertTrue_1(!(*fortytwo < *unkint));
    assertTrue_1(*unkint < *fortythree);
    assertTrue_1(!(*fortythree < *unkint));

    // Greater typed unknown always less than typed known
    assertTrue_1(*unkreal < *fortytwo);
    assertTrue_1(!(*fortytwo < *unkreal));
    assertTrue_1(*unkreal < *fortythree);
    assertTrue_1(!(*fortythree < *unkreal));

    assertTrue_1(*unkstr < *fortytwo);
    assertTrue_1(!(*fortytwo < *unkstr));
    assertTrue_1(*unkstr < *fortythree);
    assertTrue_1(!(*fortythree < *unkstr));

    delete fortythree;
  }

  // Both known, lesser type is less
  assertTrue_1(*troo < *fortytwo);
  assertTrue_1(!(*fortytwo < *troo));

  {
    Value *ev = new Value(2.718);
    Value *piv = new Value(3.14);
    assertTrue_1(!(*ev < *ev));
    assertTrue_1(!(*piv < *piv));
    assertTrue_1(*ev < *piv);
    assertTrue_1(!(*piv < *ev));

    assertTrue_1(*unkv < *ev);
    assertTrue_1(!(*ev < *unkv));
    assertTrue_1(*unkv < *piv);
    assertTrue_1(!(*piv < *unkv));

    // Lesser typed unknown always less than typed known
    assertTrue_1(*unkbool < *ev);
    assertTrue_1(!(*ev < *unkbool));
    assertTrue_1(*unkbool < *piv);
    assertTrue_1(!(*piv < *unkbool));

    assertTrue_1(*unkint < *ev);
    assertTrue_1(!(*ev < *unkint));
    assertTrue_1(*unkint < *piv);
    assertTrue_1(!(*piv < *unkint));

    assertTrue_1(*unkreal < *ev);
    assertTrue_1(!(*ev < *unkreal));
    assertTrue_1(*unkreal < *piv);
    assertTrue_1(!(*piv < *unkreal));

    // Greater typed unknown always less than typed known
    assertTrue_1(*unkstr < *ev);
    assertTrue_1(!(*ev < *unkstr));
    assertTrue_1(*unkstr < *piv);
    assertTrue_1(!(*piv < *unkstr));

    // Both known, lesser type is less
    assertTrue_1(*troo < *ev);
    assertTrue_1(!(*ev < *troo));

    // Standard numeric rules apply
    assertTrue_1(!(*fortytwo < *ev));
    assertTrue_1(*ev < *fortytwo);

    {
      Value *foov = new Value("Foo");
      Value *fopv = new Value("Fop");
      assertTrue_1(!(*foov < *foov));
      assertTrue_1(!(*fopv < *fopv));
      assertTrue_1(*foov < *fopv);
      assertTrue_1(!(*fopv < *foov));

      assertTrue_1(*unkv < *foov);
      assertTrue_1(!(*foov < *unkv));
      assertTrue_1(*unkv < *fopv);
      assertTrue_1(!(*fopv < *unkv));

      delete unkv;

      // Lesser typed unknown always less than typed known
      assertTrue_1(*unkbool < *foov);
      assertTrue_1(!(*foov < *unkbool));
      assertTrue_1(*unkbool < *fopv);
      assertTrue_1(!(*fopv < *unkbool));

      delete unkbool;

      assertTrue_1(*unkint < *foov);
      assertTrue_1(!(*foov < *unkint));
      assertTrue_1(*unkint < *fopv);
      assertTrue_1(!(*fopv < *unkint));

      delete unkint;

      assertTrue_1(*unkreal < *foov);
      assertTrue_1(!(*foov < *unkreal));
      assertTrue_1(*unkreal < *fopv);
      assertTrue_1(!(*fopv < *unkreal));

      delete unkreal;

      // Same type, unknown less than known
      assertTrue_1(*unkstr < *foov);
      assertTrue_1(!(*foov < *unkstr));
      assertTrue_1(*unkstr < *fopv);
      assertTrue_1(!(*fopv < *unkstr));

      delete unkstr;

      // Both known, lesser type is less
      assertTrue_1(*troo < *foov);
      assertTrue_1(!(*foov < *troo));

      delete troo;

      assertTrue_1(*fortytwo < *foov);
      assertTrue_1(!(*foov < *fortytwo));

      assertTrue_1(*piv < *foov);
      assertTrue_1(!(*foov < *piv));

    }

    delete piv;

    // Real vs integer - std numerical rules
    assertTrue_1(*ev < *fortytwo);
    assertTrue_1(!(*fortytwo < *ev));

    delete ev;
  }

  {
    Value *real42v = new Value((Real) 42);
    assertTrue_1(real42v->valueType() == REAL_TYPE);
    // Equal
    assertTrue_1(!(*fortytwo < *real42v));
    assertTrue_1(!(*real42v < *fortytwo));
  }

  delete fortytwo;

  return true;
}

static bool testBooleanArrayLessThan()
{
  Value *ubav = new Value(0, BOOLEAN_ARRAY_TYPE);
  assertTrue_1(!(*ubav < *ubav));

  Value *ebav = NULL;
  {
    BooleanArray *emptyBool = new BooleanArray;
    ebav = new Value(*emptyBool);
    delete emptyBool;
  }
  assertTrue_1(!(*ebav < *ebav));

  assertTrue_1(*ubav < *ebav);
  assertTrue_1(!(*ebav < *ubav));

  Value *sbav = NULL;
  {
    BooleanArray *sizedBool = new BooleanArray(2);
    sbav = new Value(*sizedBool);
    delete sizedBool;
  }
  assertTrue_1(!(*sbav < *sbav));

  assertTrue_1(*ubav < *sbav);
  assertTrue_1(!(*sbav < *ubav));

  assertTrue_1(*ebav < *sbav);
  assertTrue_1(!(*sbav < *ebav));

  Value *ibav = NULL;
  {
    std::vector<bool> bv(2);
    bv[0] = false;
    bv[1] = true;
    BooleanArray *initedBool = new BooleanArray(bv);
    ibav = new Value(*initedBool);
    delete initedBool;
  }
  assertTrue_1(!(*ibav < *ibav));

  assertTrue_1(*ubav < *ibav);
  assertTrue_1(!(*ibav < *ubav));

  assertTrue_1(*ebav < *ibav);
  assertTrue_1(!(*ibav < *ebav));

  assertTrue_1(*sbav < *ibav);
  assertTrue_1(!(*ibav < *sbav));

  Value *ibav2 = NULL;
  {
    std::vector<bool> bv2(2);
    bv2[0] = false;
    bv2[1] = false;
    BooleanArray *initedBool2 = new BooleanArray(bv2);
    ibav2 = new Value(*initedBool2);
    delete initedBool2;
  }
  assertTrue_1(!(*ibav2 < *ibav2));

  assertTrue_1(*ubav < *ibav2);
  assertTrue_1(!(*ibav2 < *ubav));

  delete ubav;

  assertTrue_1(*ebav < *ibav2);
  assertTrue_1(!(*ibav2 < *ebav));

  delete ebav;

  assertTrue_1(*sbav < *ibav2);
  assertTrue_1(!(*ibav2 < *sbav));

  // Same size, contents determine the victor
  assertTrue_1(!(*ibav < *ibav2));
  assertTrue_1(*ibav2 < *ibav);

  delete ibav2;
  delete ibav;
  return true;
}

static bool testIntegerArrayLessThan()
{
  Value *uiav = new Value(0, INTEGER_ARRAY_TYPE);
  assertTrue_1(!(*uiav < *uiav));

  Value *eiav = NULL;
  {
    IntegerArray *emptyInt = new IntegerArray;
    eiav = new Value(*emptyInt);
    delete emptyInt;
  }
  assertTrue_1(!(*eiav < *eiav));

  assertTrue_1(*uiav < *eiav);
  assertTrue_1(!(*eiav < *uiav));

  Value *siav = NULL;
  {
    IntegerArray *sizedInt = new IntegerArray(2);
    siav = new Value(*sizedInt);
    delete sizedInt;
  }
  assertTrue_1(!(*siav < *siav));

  assertTrue_1(*uiav < *siav);
  assertTrue_1(!(*siav < *uiav));

  assertTrue_1(*eiav < *siav);
  assertTrue_1(!(*siav < *eiav));

  Value *iiav = NULL;
  {
    std::vector<Integer> iv(2);
    iv[0] = 42;
    iv[1] = 6;
    IntegerArray *initedInt = new IntegerArray(iv);
    iiav = new Value(*initedInt);
    delete initedInt;
  }
  assertTrue_1(!(*iiav < *iiav));

  assertTrue_1(*uiav < *iiav);
  assertTrue_1(!(*iiav < *uiav));

  assertTrue_1(*eiav < *iiav);
  assertTrue_1(!(*iiav < *eiav));

  assertTrue_1(*siav < *iiav);
  assertTrue_1(!(*iiav < *siav));

  Value *iiav2 = NULL;
  {
    std::vector<Integer> iv2(2);
    iv2[0] = 42;
    iv2[1] = 7;
    IntegerArray *initedInt2 = new IntegerArray(iv2);
    iiav2 = new Value(*initedInt2);
    delete initedInt2;
  }
  assertTrue_1(!(*iiav2 < *iiav2));

  assertTrue_1(*uiav < *iiav2);
  assertTrue_1(!(*iiav2 < *uiav));

  delete uiav;

  assertTrue_1(*eiav < *iiav2);
  assertTrue_1(!(*iiav2 < *eiav));

  assertTrue_1(*siav < *iiav2);
  assertTrue_1(!(*iiav2 < *siav));

  // Same size, contents determine the victor
  assertTrue_1(*iiav < *iiav2);
  assertTrue_1(!(*iiav2 < *iiav));

  delete iiav2;
  delete iiav;

  return true;
}

static bool testRealArrayLessThan()
{
  Value *urav = new Value(0, REAL_ARRAY_TYPE);
  assertTrue_1(!(*urav < *urav));

  Value *erav = NULL;
  {
    RealArray *emptyReal = new RealArray;
    erav = new Value(*emptyReal);
    delete emptyReal;
  }
  assertTrue_1(!(*erav < *erav));

  assertTrue_1(*urav < *erav);
  assertTrue_1(!(*erav < *urav));

  Value *srav = NULL;
  {
    RealArray *sizedReal = new RealArray(2);
    srav = new Value(*sizedReal);
    delete sizedReal;
  }
  assertTrue_1(!(*srav < *srav));

  assertTrue_1(*urav < *srav);
  assertTrue_1(!(*srav < *urav));

  assertTrue_1(*erav < *srav);
  assertTrue_1(!(*srav < *erav));

  Value *irav = NULL;
  {
    std::vector<Real> dv(2);
    dv[0] = 3.14;
    dv[1] = 4.5;
    RealArray *initedReal = new RealArray(dv);
    irav = new Value(*initedReal);
    delete initedReal;
  }
  assertTrue_1(!(*irav < *irav));

  assertTrue_1(*urav < *irav);
  assertTrue_1(!(*irav < *urav));

  assertTrue_1(*erav < *irav);
  assertTrue_1(!(*irav < *erav));

  assertTrue_1(*srav < *irav);
  assertTrue_1(!(*irav < *srav));

  Value *irav2 = NULL;
  {
    std::vector<Real> dv2(2);
    dv2[0] = 3.14;
    dv2[1] = 4.6;
    RealArray *initedReal2 = new RealArray(dv2);
    irav2 = new Value(*initedReal2);
    delete initedReal2;
  }
  assertTrue_1(!(*irav2 < *irav2));

  assertTrue_1(*urav < *irav2);
  assertTrue_1(!(*irav2 < *urav));

  delete urav;

  assertTrue_1(*erav < *irav2);
  assertTrue_1(!(*irav2 < *erav));

  delete erav;

  assertTrue_1(*srav < *irav2);
  assertTrue_1(!(*irav2 < *srav));

  delete srav;

  // Same size, contents determine the victor
  assertTrue_1(*irav < *irav2);
  assertTrue_1(!(*irav2 < *irav));

  delete irav2;
  delete irav;

  return true;
}

static bool testStringArrayLessThan()
{ 
  Value *usav = new Value(0, STRING_ARRAY_TYPE);
  assertTrue_1(!(*usav < *usav));

  Value *esav = NULL;
  {
    StringArray *emptyString = new StringArray;
    esav = new Value(*emptyString);
    delete emptyString;
  }
  assertTrue_1(!(*esav < *esav));

  assertTrue_1(*usav < *esav);
  assertTrue_1(!(*esav < *usav));

  Value *ssav = NULL;
  {
    StringArray *sizedString = new StringArray(2);
    ssav = new Value(*sizedString);
    delete sizedString;
  }
  assertTrue_1(!(*ssav < *ssav));

  assertTrue_1(*usav < *ssav);
  assertTrue_1(!(*ssav < *usav));

  assertTrue_1(*esav < *ssav);
  assertTrue_1(!(*ssav < *esav));

  Value *isav = NULL;
  {
    std::vector<String> sv(2);
    sv[0] = String("yo ");
    sv[1] = String("mama");
    StringArray *initedString = new StringArray(sv);
    isav = new Value(*initedString);
  }
  assertTrue_1(!(*isav < *isav));

  assertTrue_1(*usav < *isav);
  assertTrue_1(!(*isav < *usav));

  assertTrue_1(*esav < *isav);
  assertTrue_1(!(*isav < *esav));

  assertTrue_1(*ssav < *isav);
  assertTrue_1(!(*isav < *ssav));

  Value *isav2 = NULL;
  {
    std::vector<String> sv2(2);
    sv2[0] = String("yo ");
    sv2[1] = String("mamb");
    StringArray *initedString2 = new StringArray(sv2);
    isav2 = new Value(*initedString2);
    delete initedString2;
  }
  assertTrue_1(!(*isav2 < *isav2));

  assertTrue_1(*usav < *isav2);
  assertTrue_1(!(*isav2 < *usav));

  delete usav;

  assertTrue_1(*esav < *isav2);
  assertTrue_1(!(*isav2 < *esav));

  delete esav;

  assertTrue_1(*ssav < *isav2);
  assertTrue_1(!(*isav2 < *ssav));

  delete ssav;

  // Same size, contents determine the victor
  assertTrue_1(*isav < *isav2);
  assertTrue_1(!(*isav2 < *isav));

  delete isav2;
  delete isav;

  return true;
}
  // *** TODO ***

  // Cross array type

  // Array to unknown

  // Array to Boolean

  // Array to Integer

  // Array to Real

  // Array to String

bool valueTest()
{
  runTest(testConstructorsAndAccessors);
#if __cplusplus >= 201103L
  runTest(testMoveConstructors);
#endif
  runTest(testScalarEquality);
  runTest(testBooleanArrayEquality);
  runTest(testIntegerArrayEquality);
  runTest(testRealArrayEquality);
  runTest(testStringArrayEquality);
  runTest(testEmptyArrayEquality);
  runTest(testScalarBooleanArrayEquality);
  runTest(testScalarIntegerArrayEquality);
  runTest(testScalarRealArrayEquality);
  runTest(testScalarStringArrayEquality);
  runTest(testScalarLessThan);
  runTest(testBooleanArrayLessThan);
  runTest(testIntegerArrayLessThan);
  runTest(testRealArrayLessThan);
  runTest(testStringArrayLessThan);

  return true;
}
