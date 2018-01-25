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

#include "ArrayVariable.hh"
#include "Constant.hh"
#include "TestSupport.hh"
#include "test/TrivialListener.hh"
#include "UserVariable.hh"
#include "Value.hh"

using namespace PLEXIL;

static bool arrayConstantReadTest()
{
  // Set up test data
  std::vector<Boolean>        vb(2);
  std::vector<Integer>     vi(4);
  std::vector<Real>      vd(4);
  std::vector<String> vs(4);

  vb[0] = false;
  vb[1] = true;

  vi[0] = 0;
  vi[1] = 1;
  vi[2] = 2;
  vi[3] = 3;

  vd[0] = 0;
  vd[1] = 1;
  vd[2] = 2;
  vd[3] = 3;

  vs[0] = std::string("zero");
  vs[1] = std::string("one");
  vs[2] = std::string("two");
  vs[3] = std::string("three");

  BooleanArrayConstant bc(vb);
  IntegerArrayConstant ic(vi);
  RealArrayConstant    dc(vd);
  StringArrayConstant  sc(vs);

  BooleanArray const        *pab = NULL;
  IntegerArray const     *pai = NULL;
  RealArray const      *pad = NULL;
  StringArray const *pas = NULL;

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  // Constants are always active
  assertTrue_1(bc.isKnown());
  assertTrue_1(bc.getValuePointer(pab));
  assertTrue_1(pab != NULL);
  assertTrue_1(pab->allElementsKnown());
  pab->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(vb == *pvb);

  assertTrue_1(ic.isKnown());
  assertTrue_1(ic.getValuePointer(pai));
  assertTrue_1(pai != NULL);
  assertTrue_1(pai->allElementsKnown());
  pai->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(vi == *pvi);

  assertTrue_1(dc.isKnown());
  assertTrue_1(dc.getValuePointer(pad));
  assertTrue_1(pad != NULL);
  assertTrue_1(pad->allElementsKnown());
  pad->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(vd == *pvd);

  assertTrue_1(sc.isKnown());
  assertTrue_1(sc.getValuePointer(pas));
  assertTrue_1(pas != NULL);
  assertTrue_1(pas->allElementsKnown());
  pas->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(vs == *pvs);

  return true;
}

static bool uninitializedVariableTest()
{
  BooleanArrayVariable vuba;
  IntegerArrayVariable vuia;
  RealArrayVariable vuda;
  StringArrayVariable vusa;

  // Test that they are assignable and not constant
  assertTrue_1(vuba.isAssignable());
  assertTrue_1(vuia.isAssignable());
  assertTrue_1(vuda.isAssignable());
  assertTrue_1(vusa.isAssignable());

  assertTrue_1(!vuba.isConstant());
  assertTrue_1(!vuia.isConstant());
  assertTrue_1(!vuda.isConstant());
  assertTrue_1(!vusa.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vuba.isActive());
  assertTrue_1(!vuia.isActive());
  assertTrue_1(!vuda.isActive());
  assertTrue_1(!vusa.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  BooleanArray const *pfooba = NULL;
  IntegerArray const *pfooia = NULL;
  RealArray const *pfooda = NULL;
  StringArray const *pfoosa = NULL;

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Activate and confirm they are still unknown
  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Assign and check result
  std::vector<Boolean> vb(1, true);
  std::vector<Integer> vi(1, 42);
  std::vector<Real> vd(1, 3.1416);
  std::vector<String> vs(1, std::string("Foo"));

  BooleanArrayConstant acb(vb);
  IntegerArrayConstant aci(vi);
  RealArrayConstant acd(vd);
  StringArrayConstant acs(vs);

  vuba.setValue(acb.toValue());
  vuia.setValue(aci.toValue());
  vuda.setValue(acd.toValue());
  vusa.setValue(acs.toValue());

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(vb == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(vi == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(vd == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(vs == *pvs);

  // Reset and check that value is now unknown
  vuba.deactivate();
  vuia.deactivate();
  vuda.deactivate();
  vusa.deactivate();

  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Set value and check result
  vuba.setValue(acb.toValue());
  vuia.setValue(aci.toValue());
  vuda.setValue(acd.toValue());
  vusa.setValue(acs.toValue());

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(vb == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(vi == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(vd == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvd != NULL);
  assertTrue_1(vs == *pvs);

  // Set values and check that they changed
  std::vector<Boolean> vab(2, false);
  std::vector<Integer> vai(2, 69);
  std::vector<Real> vad(2, 2.718);
  std::vector<String> vas(2, std::string("bar"));

  BooleanArrayConstant acab(vab);
  IntegerArrayConstant acai(vai);
  RealArrayConstant acad(vad);
  StringArrayConstant acas(vas);

  vuba.setValue(acab.toValue());
  vuia.setValue(acai.toValue());
  vuda.setValue(acad.toValue());
  vusa.setValue(acas.toValue());

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(vab == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(vai == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(vad == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(vas == *pvs);

  return true;
}

static bool testVariableInitialValue()
{
  BooleanArrayVariable vba;
  std::vector<Boolean> bv(2, true);
  vba.setInitializer(new BooleanArrayConstant(BooleanArray(bv)), true);

  IntegerArrayVariable via;
  std::vector<Integer> iv(2, 56);
  via.setInitializer(new IntegerArrayConstant(IntegerArray(iv)), true);

  RealArrayVariable vda;
  std::vector<Real> dv(2, 1.414);
  vda.setInitializer(new RealArrayConstant(RealArray(dv)), true);
  
  StringArrayVariable vsa;
  std::vector<String> sv(2, std::string("yahoo"));
  vsa.setInitializer(new StringArrayConstant(StringArray(sv)), true);

  // Test that they are assignable and not constant
  assertTrue_1(vba.isAssignable());
  assertTrue_1(via.isAssignable());
  assertTrue_1(vda.isAssignable());
  assertTrue_1(vsa.isAssignable());

  assertTrue_1(!vba.isConstant());
  assertTrue_1(!via.isConstant());
  assertTrue_1(!vda.isConstant());
  assertTrue_1(!vsa.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vba.isActive());
  assertTrue_1(!via.isActive());
  assertTrue_1(!vda.isActive());
  assertTrue_1(!vsa.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  // Activate and confirm they are known
  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  // Check values
  BooleanArray const *pfooba = NULL;
  IntegerArray const *pfooia = NULL;
  RealArray const *pfooda = NULL;
  StringArray const *pfoosa = NULL;

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Set unknown
  vba.setUnknown();
  via.setUnknown();
  vda.setUnknown();
  vsa.setUnknown();

  // Confirm that they are now unknown
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!via.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Reset and confirm unknown
  vba.deactivate();
  via.deactivate();
  vda.deactivate();
  vsa.deactivate();

  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  // Activate and check that initial value is restored
  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Set values and check
  std::vector<Boolean> bv2(1, true);
  std::vector<Integer> iv2(1, 65);
  std::vector<Real> dv2(1, 3.162);
  std::vector<String> sv2(1, std::string("yoohoo"));

  BooleanArrayConstant bac(bv2);
  IntegerArrayConstant iac(iv2);
  RealArrayConstant dac(dv2);
  StringArrayConstant sac(sv2);

  vba.setValue(bac.toValue());
  via.setValue(iac.toValue());
  vda.setValue(dac.toValue());
  vsa.setValue(sac.toValue());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv2 == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv2 == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv2 == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv2 == *pvs);

  return true;
}

// Confirm that initializer expression is only invoked
// on an inactive to active transition.
static bool testVariableInitializers()
{
  std::vector<Boolean> bv(2, true);
  std::vector<Integer> iv(2, 56);
  std::vector<Real> dv(2, 1.414);
  std::vector<String> sv(2, std::string("yahoo"));

  BooleanArrayVariable vba;
  BooleanArrayConstant cba(bv);
  vba.setInitializer(&cba, false);

  IntegerArrayVariable via;
  IntegerArrayConstant cia(iv);
  via.setInitializer(&cia, false);

  RealArrayVariable vda;
  RealArrayConstant cda(dv);
  vda.setInitializer(&cda, false);

  StringArrayVariable vsa;
  StringArrayConstant csa(sv);
  vsa.setInitializer(&csa, false);

  // Test that they are assignable and not constant
  assertTrue_1(vba.isAssignable());
  assertTrue_1(via.isAssignable());
  assertTrue_1(vda.isAssignable());
  assertTrue_1(vsa.isAssignable());

  assertTrue_1(!vba.isConstant());
  assertTrue_1(!via.isConstant());
  assertTrue_1(!vda.isConstant());
  assertTrue_1(!vsa.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vba.isActive());
  assertTrue_1(!via.isActive());
  assertTrue_1(!vda.isActive());
  assertTrue_1(!vsa.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  // Activate and confirm they are known
  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  // Check values
  BooleanArray const *pfooba = NULL;
  IntegerArray const *pfooia = NULL;
  RealArray const *pfooda = NULL;
  StringArray const *pfoosa = NULL;

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Set unknown
  vba.setUnknown();
  via.setUnknown();
  vda.setUnknown();
  vsa.setUnknown();

  // Confirm that they are now unknown
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!via.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Reset and confirm unknown
  vba.deactivate();
  via.deactivate();
  vda.deactivate();
  vsa.deactivate();

  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  // Activate and check that initial value is restored
  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Set values and check
  std::vector<Boolean> bv2(1, true);
  std::vector<Integer> iv2(1, 65);
  std::vector<Real> dv2(1, 3.162);
  std::vector<String> sv2(1, std::string("yoohoo"));

  BooleanArrayConstant bac(bv2);
  IntegerArrayConstant iac(iv2);
  RealArrayConstant dac(dv2);
  StringArrayConstant sac(sv2);

  vba.setValue(bac.toValue());
  via.setValue(iac.toValue());
  vda.setValue(dac.toValue());
  vsa.setValue(sac.toValue());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv2 == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv2 == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv2 == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv2 == *pvs);

  return true;
}

static bool testVariableSavedValue()
{
  BooleanArrayVariable vuba;
  IntegerArrayVariable vuia;
  RealArrayVariable vuda;
  StringArrayVariable vusa;

  BooleanArray const *pfooba = NULL;
  IntegerArray const *pfooia = NULL;
  RealArray const *pfooda = NULL;
  StringArray const *pfoosa = NULL;

  // Activate
  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  // Confirm that they are unknown
  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Save current value (should be unknown)
  vuba.saveCurrentValue();
  vuia.saveCurrentValue();
  vuda.saveCurrentValue();
  vusa.saveCurrentValue();

  // Assign and check result
  std::vector<Boolean> bv(2, true);
  std::vector<Integer> iv(2, 56);
  std::vector<Real> dv(2, 1.414);
  std::vector<String> sv(2, std::string("yahoo"));

  BooleanArrayConstant bac(bv);
  IntegerArrayConstant iac(iv);
  RealArrayConstant dac(dv);
  StringArrayConstant sac(sv);

  vuba.setValue(bac.toValue());
  vuia.setValue(iac.toValue());
  vuda.setValue(dac.toValue());
  vusa.setValue(sac.toValue());

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Restore saved value and check result
  vuba.restoreSavedValue();
  vuia.restoreSavedValue();
  vuda.restoreSavedValue();
  vusa.restoreSavedValue();

  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Assign again
  vuba.setValue(bac.toValue());
  vuia.setValue(iac.toValue());
  vuda.setValue(dac.toValue());
  vusa.setValue(sac.toValue());

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Save current value
  vuba.saveCurrentValue();
  vuia.saveCurrentValue();
  vuda.saveCurrentValue();
  vusa.saveCurrentValue();

  // Make unknown
  vuba.setUnknown();
  vuia.setUnknown();
  vuda.setUnknown();
  vusa.setUnknown();

  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Restore saved and check that it has returned
  vuba.restoreSavedValue();
  vuia.restoreSavedValue();
  vuda.restoreSavedValue();
  vusa.restoreSavedValue();

  assertTrue_1(vuba.isKnown());
  assertTrue_1(vuia.isKnown());
  assertTrue_1(vuda.isKnown());
  assertTrue_1(vusa.isKnown());

  assertTrue_1(vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Reset
  vuba.deactivate();
  vuia.deactivate();
  vuda.deactivate();
  vusa.deactivate();

  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  assertTrue_1(!vuba.isKnown());
  assertTrue_1(!vuia.isKnown());
  assertTrue_1(!vuda.isKnown());
  assertTrue_1(!vusa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vuba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!vuia.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vuda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vusa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  return true;
}

// Confirm that we can do all the Assignable operations
// through an Assignable * pointer.
static bool testAssignablePointer()
{

  BooleanArrayVariable vba;
  std::vector<Boolean> bv(2, true);
  vba.setInitializer(new BooleanArrayConstant(BooleanArray(bv)), true);

  IntegerArrayVariable via;
  std::vector<Integer> iv(2, 56);
  via.setInitializer(new IntegerArrayConstant(IntegerArray(iv)), true);

  RealArrayVariable vda;
  std::vector<Real> dv(2, 1.414);
  vda.setInitializer(new RealArrayConstant(RealArray(dv)), true);
  
  StringArrayVariable vsa;
  std::vector<String> sv(2, std::string("yahoo"));
  vsa.setInitializer(new StringArrayConstant(StringArray(sv)), true);

  Assignable *eba(vba.asAssignable());
  Assignable *eia(via.asAssignable());
  Assignable *eda(vda.asAssignable());
  Assignable *esa(vsa.asAssignable());

  // Confirm that we actually got pointers
  assertTrue_1(eba != NULL);
  assertTrue_1(eia != NULL);
  assertTrue_1(eda != NULL);
  assertTrue_1(esa != NULL);

  // Test that they are assignable and not constant
  assertTrue_1(vba.isAssignable());
  assertTrue_1(via.isAssignable());
  assertTrue_1(vda.isAssignable());
  assertTrue_1(vsa.isAssignable());

  assertTrue_1(!vba.isConstant());
  assertTrue_1(!via.isConstant());
  assertTrue_1(!vda.isConstant());
  assertTrue_1(!vsa.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vba.isActive());
  assertTrue_1(!via.isActive());
  assertTrue_1(!vda.isActive());
  assertTrue_1(!vsa.isActive());

  // Test that values are unknown while inactive
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  // Activate
  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  // Test that they are now known
  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  // Check values
  BooleanArray const *pfooba = NULL;
  IntegerArray const *pfooia = NULL;
  RealArray const *pfooda = NULL;
  StringArray const *pfoosa = NULL;

  std::vector<Boolean> const *pvb;
  std::vector<Integer> const *pvi;
  std::vector<Real> const *pvd;
  std::vector<String> const *pvs;

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  // Set values

  std::vector<Boolean> bv2(1, false);
  std::vector<Integer> iv2(1, 99);
  std::vector<Real> dv2(1, 2.718);
  std::vector<String> sv2(1, std::string("yeehaw"));

  BooleanArrayConstant bac(bv2);
  IntegerArrayConstant iac(iv2);
  RealArrayConstant dac(dv2);
  StringArrayConstant sac(sv2);

  eba->setValue(bac.toValue());
  eia->setValue(iac.toValue());
  eda->setValue(dac.toValue());
  esa->setValue(sac.toValue());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv2 == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv2 == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvi != NULL);
  assertTrue_1(dv2 == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv2 == *pvs);

  // Save
  eba->saveCurrentValue();
  eia->saveCurrentValue();
  eda->saveCurrentValue();
  esa->saveCurrentValue();

  // Set unknown
  eba->setUnknown();
  eia->setUnknown();
  eda->setUnknown();
  esa->setUnknown();

  // Test that values are now unknown
  assertTrue_1(!vba.isKnown());
  assertTrue_1(!via.isKnown());
  assertTrue_1(!vda.isKnown());
  assertTrue_1(!vsa.isKnown());

  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(!vba.getValuePointer(pfooba));
  assertTrue_1(pfooba == NULL);

  assertTrue_1(!via.getValuePointer(pfooia));
  assertTrue_1(pfooia == NULL);

  assertTrue_1(!vda.getValuePointer(pfooda));
  assertTrue_1(pfooda == NULL);

  assertTrue_1(!vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa == NULL);

  // Restore
  eba->restoreSavedValue();
  eia->restoreSavedValue();
  eda->restoreSavedValue();
  esa->restoreSavedValue();

  // Check that saved values are restored
  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv2 == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv2 == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv2 == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv2 == *pvs);

  // Reset
  vba.deactivate();
  via.deactivate();
  vda.deactivate();
  vsa.deactivate();

  vba.activate();
  via.activate();
  vda.activate();
  vsa.activate();

  // Check initial values are restored
  pfooba = NULL;
  pfooia = NULL;
  pfooda = NULL;
  pfoosa = NULL;

  assertTrue_1(vba.isKnown());
  assertTrue_1(via.isKnown());
  assertTrue_1(vda.isKnown());
  assertTrue_1(vsa.isKnown());

  assertTrue_1(vba.getValuePointer(pfooba));
  assertTrue_1(pfooba != NULL);
  assertTrue_1(pfooba->allElementsKnown());
  pfooba->getContentsVector(pvb);
  assertTrue_1(pvb != NULL);
  assertTrue_1(bv == *pvb);

  assertTrue_1(via.getValuePointer(pfooia));
  assertTrue_1(pfooia != NULL);
  assertTrue_1(pfooia->allElementsKnown());
  pfooia->getContentsVector(pvi);
  assertTrue_1(pvi != NULL);
  assertTrue_1(iv == *pvi);

  assertTrue_1(vda.getValuePointer(pfooda));
  assertTrue_1(pfooda != NULL);
  assertTrue_1(pfooda->allElementsKnown());
  pfooda->getContentsVector(pvd);
  assertTrue_1(pvd != NULL);
  assertTrue_1(dv == *pvd);

  assertTrue_1(vsa.getValuePointer(pfoosa));
  assertTrue_1(pfoosa != NULL);
  assertTrue_1(pfoosa->allElementsKnown());
  pfoosa->getContentsVector(pvs);
  assertTrue_1(pvs != NULL);
  assertTrue_1(sv == *pvs);

  return true;
}

static bool testVariableNotification()
{
  BooleanArrayVariable vuba;
  IntegerArrayVariable vuia;
  RealArrayVariable vuda;
  StringArrayVariable vusa;

  bool bachanged = false,
    iachanged = false, 
    dachanged = false,
    sachanged = false;

  TrivialListener lba(bachanged);
  TrivialListener lia(iachanged);
  TrivialListener lda(dachanged);
  TrivialListener lsa(sachanged);

  vuba.addListener(&lba);
  vuia.addListener(&lia);
  vuda.addListener(&lda);
  vusa.addListener(&lsa);

  // Activation alone should not notify
  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  assertTrue_1(!bachanged);
  assertTrue_1(!iachanged);
  assertTrue_1(!dachanged);
  assertTrue_1(!sachanged);

  // Reset for uninitialized should NOT notify
  vuba.deactivate();
  vuia.deactivate();
  vuda.deactivate();
  vusa.deactivate();

  vuba.activate();
  vuia.activate();
  vuda.activate();
  vusa.activate();

  assertTrue_1(!bachanged);
  assertTrue_1(!iachanged);
  assertTrue_1(!dachanged);
  assertTrue_1(!sachanged);

  // Assign should notify this time
  std::vector<Boolean> bv(2, true);
  std::vector<Integer> iv(2, 56);
  std::vector<Real> dv(2, 1.414);
  std::vector<String> sv(2, std::string("yahoo"));

  BooleanArrayConstant bac(bv);
  IntegerArrayConstant iac(iv);
  RealArrayConstant dac(dv);
  StringArrayConstant sac(sv);

  vuba.setValue(bac.toValue());
  vuia.setValue(iac.toValue());
  vuda.setValue(dac.toValue());
  vusa.setValue(sac.toValue());

  assertTrue_1(bachanged);
  assertTrue_1(iachanged);
  assertTrue_1(dachanged);
  assertTrue_1(sachanged);

  // Save current value shouldn't notify
  bachanged = iachanged = dachanged = sachanged =
    false;
  vuba.saveCurrentValue();
  vuia.saveCurrentValue();
  vuda.saveCurrentValue();
  vusa.saveCurrentValue();

  assertTrue_1(!bachanged);
  assertTrue_1(!iachanged);
  assertTrue_1(!dachanged);
  assertTrue_1(!sachanged);

  // Restoring same value shouldn't notify
  vuba.restoreSavedValue();
  vuia.restoreSavedValue();
  vuda.restoreSavedValue();
  vusa.restoreSavedValue();

  assertTrue_1(!bachanged);
  assertTrue_1(!iachanged);
  assertTrue_1(!dachanged);
  assertTrue_1(!sachanged);

  // Making unknown should notify
  vuba.setUnknown();
  vuia.setUnknown();
  vuda.setUnknown();
  vusa.setUnknown();

  assertTrue_1(bachanged);
  assertTrue_1(iachanged);
  assertTrue_1(dachanged);
  assertTrue_1(sachanged);

  // Now restoring should notify
  bachanged = iachanged = dachanged = sachanged =
    false;
  vuba.restoreSavedValue();
  vuia.restoreSavedValue();
  vuda.restoreSavedValue();
  vusa.restoreSavedValue();

  assertTrue_1(bachanged);
  assertTrue_1(iachanged);
  assertTrue_1(dachanged);
  assertTrue_1(sachanged);

  // Clean up
  vuba.removeListener(&lba);
  vuia.removeListener(&lia);
  vuda.removeListener(&lda);
  vusa.removeListener(&lsa);

  return true;
}

bool arrayVariableTest()
{
  runTest(arrayConstantReadTest);
  runTest(uninitializedVariableTest);
  runTest(testVariableInitialValue);
  runTest(testVariableInitializers);
  runTest(testVariableSavedValue);
  runTest(testAssignablePointer);
  runTest(testVariableNotification);
  return true;
}
