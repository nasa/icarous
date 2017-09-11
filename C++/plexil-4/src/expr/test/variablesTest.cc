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

#include "UserVariable.hh"
#include "Constant.hh"
#include "TestSupport.hh"
#include "Value.hh"
#include "test/TrivialListener.hh"

using namespace PLEXIL;

//
// Variables
//

static bool testUninitialized()
{
  // Default constructors
  BooleanVariable vub;
  IntegerVariable vui;
  RealVariable vud;
  StringVariable vus;

  // Test that they are assignable and not constant
  assertTrue_1(vub.isAssignable());
  assertTrue_1(vui.isAssignable());
  assertTrue_1(vud.isAssignable());
  assertTrue_1(vus.isAssignable());

  assertTrue_1(!vub.isConstant());
  assertTrue_1(!vui.isConstant());
  assertTrue_1(!vud.isConstant());
  assertTrue_1(!vus.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vub.isActive());
  assertTrue_1(!vui.isActive());
  assertTrue_1(!vud.isActive());
  assertTrue_1(!vus.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  bool foob;
  Integer fooi;
  Real food;
  String foos;

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));
  // Numeric conversion -- C++ sucks at inheritance
  assertTrue_1(!((Expression const &) vui).getValue(food));

  // Activate and confirm they are still unknown
  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));
  // Numeric conversion
  assertTrue_1(!((Expression const &) vui).getValue(food));

  // Assign and check result
  vub.setValue(Value(true));
  vui.setValue(Value((Integer) 42));
  vud.setValue(Value(3.14));
  vus.setValue(Value("yoohoo"));

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());
    
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(fooi == 42);
  assertTrue_1(vud.getValue(food));
  assertTrue_1(food == 3.14);
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foos == String("yoohoo"));
  // Test getValue type conversion
  assertTrue_1(((Expression const &) vui).getValue(food));
  assertTrue_1(food == 42);

  // Arrays
  // Test setValue type conversion
  const Integer fiftySeven = 57;
  vud.setValue(Value(fiftySeven));
  assertTrue_1(vud.getValue(food));
  assertTrue_1(food == 57);
  vus.setValue(Value("yoho"));
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foos == String("yoho"));

  // Reset and check that value is now unknown
  // Can't reset while active
  vub.deactivate();
  vui.deactivate();
  vud.deactivate();
  vus.deactivate();

  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));

  // Set value and check result
  vub.setValue(Value(true));
  vui.setValue(Value((Integer) 42));
  vud.setValue(Value(3.14));
  vus.setValue(Value("yoohoo"));

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());

  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(fooi == 42);
  assertTrue_1(vud.getValue(food));
  assertTrue_1(food == 3.14);
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foos == String("yoohoo"));

  // Set values and check that they changed
  vub.setValue(Value(false));
  vui.setValue(Value((Integer) 69));
  vud.setValue(Value(1.414));
  vus.setValue(Value("yo"));

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());

  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vud.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foos == String("yo"));
  // Arrays

  return true;
}

// *** TODO: This test should be superseded by testInitializers() below.
static bool testInitialValue()
{
  BooleanVariable vb(false);
  IntegerVariable vi(69);
  RealVariable vd(1.414);
  StringVariable vs(String("yo"));

  // Test that they are assignable and not constant
  assertTrue_1(vb.isAssignable());
  assertTrue_1(vi.isAssignable());
  assertTrue_1(vd.isAssignable());
  assertTrue_1(vs.isAssignable());

  assertTrue_1(!vb.isConstant());
  assertTrue_1(!vi.isConstant());
  assertTrue_1(!vd.isConstant());
  assertTrue_1(!vs.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vb.isActive());
  assertTrue_1(!vi.isActive());
  assertTrue_1(!vd.isActive());
  assertTrue_1(!vs.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  // Activate and confirm they are known
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  // Check values
  Real food;
  String foos;
  Integer fooi;
  bool foob;
    
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));
  // Numeric conversion
  assertTrue_1(((Expression const &) vi).getValue(food));
  assertTrue_1(food == 69);

  // Set unknown
  vb.setUnknown();
  vi.setUnknown();
  vd.setUnknown();
  vs.setUnknown();

  // Confirm that they are now unknown
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  assertTrue_1(!vb.getValue(foob));
  assertTrue_1(!vi.getValue(fooi));
  assertTrue_1(!vd.getValue(food));
  assertTrue_1(!vs.getValue(foos));

  // Reset and confirm unknown
  vb.deactivate();
  vi.deactivate();
  vd.deactivate();
  vs.deactivate();

  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  // Activate and check that initial value is restored
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));

  // Set values and check
  vb.setValue(Value(true));
  vi.setValue(Value((Integer) 42));
  vd.setValue(Value(2.718));
  vs.setValue(Value("mama"));

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 42);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 2.718);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("mama"));

  return true;
}

// Confirm that initializer expression is only invoked
// on an inactive to active transition.
static bool testInitializers()
{
  BooleanVariable vb;
  BooleanConstant cb(false);
  vb.setInitializer(&cb, false);

  IntegerVariable vi;
  IntegerConstant ci(69);
  vi.setInitializer(&ci, false);

  RealVariable vd;
  RealConstant cd(1.414);
  vd.setInitializer(&cd, false);

  StringVariable vs;
  StringConstant cs("yo");
  vs.setInitializer(&cs, false);

  // Test that they are assignable and not constant
  assertTrue_1(vb.isAssignable());
  assertTrue_1(vi.isAssignable());
  assertTrue_1(vd.isAssignable());
  assertTrue_1(vs.isAssignable());

  assertTrue_1(!vb.isConstant());
  assertTrue_1(!vi.isConstant());
  assertTrue_1(!vd.isConstant());
  assertTrue_1(!vs.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vb.isActive());
  assertTrue_1(!vi.isActive());
  assertTrue_1(!vd.isActive());
  assertTrue_1(!vs.isActive());

  // Test that they are unknown while inactive
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  // Activate and confirm they are known
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  // Check values
  Real food;
  String foos;
  Integer fooi;
  bool foob;
    
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));
  // Numeric conversion
  assertTrue_1(((Expression const &) vi).getValue(food));
  assertTrue_1(food == 69);

  // Set unknown
  vb.setUnknown();
  vi.setUnknown();
  vd.setUnknown();
  vs.setUnknown();

  // Confirm that they are now unknown
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  assertTrue_1(!vb.getValue(foob));
  assertTrue_1(!vi.getValue(fooi));
  assertTrue_1(!vd.getValue(food));
  assertTrue_1(!vs.getValue(foos));

  // Reset and confirm unknown
  vb.deactivate();
  vi.deactivate();
  vd.deactivate();
  vs.deactivate();

  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  // Activate and check that initial value is restored
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));

  // Set values and check
  vb.setValue(Value(true));
  vi.setValue(Value((Integer) 42));
  vd.setValue(Value(2.718));
  vs.setValue(Value("mama"));

  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 42);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 2.718);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("mama"));

  return true;
}

static bool testSavedValue()
{
  BooleanVariable vub;
  IntegerVariable vui;
  RealVariable vud;
  StringVariable vus;

  Real food;
  String foos;
  Integer fooi;
  bool foob;

  // Activate
  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  // Confirm that they are unknown
  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));

  // Save current value (should be unknown)
  vub.saveCurrentValue();
  vui.saveCurrentValue();
  vud.saveCurrentValue();
  vus.saveCurrentValue();

  // Assign and check result
  vub.setValue(Value(true));
  vui.setValue(Value((Integer) 42));
  vud.setValue(Value(3.14));
  vus.setValue(Value("yoohoo"));

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());

  assertTrue_1(vub.getValue(foob));
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(vud.getValue(food));
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foob == true);
  assertTrue_1(fooi == 42);
  assertTrue_1(food == 3.14);
  assertTrue_1(foos == String("yoohoo"));

  // Restore saved value and check result
  vub.restoreSavedValue();
  vui.restoreSavedValue();
  vud.restoreSavedValue();
  vus.restoreSavedValue();

  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));

  // Assign again
  vub.setValue(Value(true));
  vui.setValue(Value((Integer) 42));
  vud.setValue(Value(3.14));
  vus.setValue(Value("yoohoo"));

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());

  assertTrue_1(vub.getValue(foob));
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(vud.getValue(food));
  assertTrue_1(vus.getValue(foos));
  assertTrue_1(foob == true);
  assertTrue_1(fooi == 42);
  assertTrue_1(food == 3.14);
  assertTrue_1(foos == String("yoohoo"));

  // Save current value
  vub.saveCurrentValue();
  vui.saveCurrentValue();
  vud.saveCurrentValue();
  vus.saveCurrentValue();

  // Make unknown
  vub.setUnknown();
  vui.setUnknown();
  vud.setUnknown();
  vus.setUnknown();

  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));

  // Restore saved and check that it has returned
  vub.restoreSavedValue();
  vui.restoreSavedValue();
  vud.restoreSavedValue();
  vus.restoreSavedValue();

  assertTrue_1(vub.isKnown());
  assertTrue_1(vui.isKnown());
  assertTrue_1(vud.isKnown());
  assertTrue_1(vus.isKnown());

  assertTrue_1(vub.getValue(foob));
  assertTrue_1(vui.getValue(fooi));
  assertTrue_1(vud.getValue(food));
  assertTrue_1(vus.getValue(foos));

  assertTrue_1(foob == true);
  assertTrue_1(fooi == 42);
  assertTrue_1(food == 3.14);
  assertTrue_1(foos == String("yoohoo"));

  // Reset
  vub.deactivate();
  vui.deactivate();
  vud.deactivate();
  vus.deactivate();

  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  assertTrue_1(!vub.isKnown());
  assertTrue_1(!vui.isKnown());
  assertTrue_1(!vud.isKnown());
  assertTrue_1(!vus.isKnown());

  assertTrue_1(!vub.getValue(foob));
  assertTrue_1(!vui.getValue(fooi));
  assertTrue_1(!vud.getValue(food));
  assertTrue_1(!vus.getValue(foos));

  return true;
}

// Confirm that we can do all the Assignable operations
// through an Assignable * pointer.
static bool testAssignablePointer()
{
  BooleanVariable vb(false);
  IntegerVariable vi(69);
  RealVariable vd(1.414);
  StringVariable vs(String("yo"));

  Assignable *eb(vb.asAssignable());
  Assignable *ei(vi.asAssignable());
  Assignable *ed(vd.asAssignable());
  Assignable *es(vs.asAssignable());

  // Confirm that we actually got pointers
  assertTrue_1(eb != NULL);
  assertTrue_1(ei != NULL);
  assertTrue_1(ed != NULL);
  assertTrue_1(es != NULL);

  // Test that they are assignable and not constant
  assertTrue_1(vb.isAssignable());
  assertTrue_1(vi.isAssignable());
  assertTrue_1(vd.isAssignable());
  assertTrue_1(vs.isAssignable());

  assertTrue_1(!vb.isConstant());
  assertTrue_1(!vi.isConstant());
  assertTrue_1(!vd.isConstant());
  assertTrue_1(!vs.isConstant());

  // Test that they are created inactive
  assertTrue_1(!vb.isActive());
  assertTrue_1(!vi.isActive());
  assertTrue_1(!vd.isActive());
  assertTrue_1(!vs.isActive());

  // Test that values are unknown while inactive
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  // Activate
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  // Test that they are now known
  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  // Check values
  Real food;
  String foos;
  Integer fooi;
  bool foob;
    
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));
  // Numeric conversion
  assertTrue_1(((Expression const &) vi).getValue(food));
  assertTrue_1(food == 69);

  // Set values
  eb->setValue(Value(true));
  ei->setValue(Value((Integer) 42));
  ed->setValue(Value(3.14));
  es->setValue(Value("yoohoo"));

  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == true);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 42);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 3.14);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yoohoo"));

  // Save
  eb->saveCurrentValue();
  ei->saveCurrentValue();
  ed->saveCurrentValue();
  es->saveCurrentValue();

  // Set unknown
  eb->setUnknown();
  ei->setUnknown();
  ed->setUnknown();
  es->setUnknown();

  // Test that values are now unknown
  assertTrue_1(!vb.isKnown());
  assertTrue_1(!vi.isKnown());
  assertTrue_1(!vd.isKnown());
  assertTrue_1(!vs.isKnown());

  assertTrue_1(!vb.getValue(foob));
  assertTrue_1(!vi.getValue(fooi));
  assertTrue_1(!vd.getValue(food));
  assertTrue_1(!vs.getValue(foos));

  // Restore
  eb->restoreSavedValue();
  ei->restoreSavedValue();
  ed->restoreSavedValue();
  es->restoreSavedValue();

  // Check that saved values are restored
  assertTrue_1(vb.isKnown());
  assertTrue_1(vi.isKnown());
  assertTrue_1(vd.isKnown());
  assertTrue_1(vs.isKnown());

  assertTrue_1(vb.getValue(foob));
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(vd.getValue(food));
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foob == true);
  assertTrue_1(fooi == 42);
  assertTrue_1(food == 3.14);
  assertTrue_1(foos == String("yoohoo"));

  // Reset
  vb.deactivate();
  vi.deactivate();
  vd.deactivate();
  vs.deactivate();
    
  vb.activate();
  vi.activate();
  vd.activate();
  vs.activate();

  // Check initial values are restored
  assertTrue_1(vb.getValue(foob));
  assertTrue_1(foob == false);
  assertTrue_1(vi.getValue(fooi));
  assertTrue_1(fooi == 69);
  assertTrue_1(vd.getValue(food));
  assertTrue_1(food == 1.414);
  assertTrue_1(vs.getValue(foos));
  assertTrue_1(foos == String("yo"));

  return true;
}

static bool testNotification()
{
  BooleanVariable vub;
  IntegerVariable vui;
  RealVariable vud;
  StringVariable vus;

  bool bchanged = false,
    ichanged = false, 
    dchanged = false,
    schanged = false;

  TrivialListener lb(bchanged);
  TrivialListener li(ichanged);
  TrivialListener ld(dchanged);
  TrivialListener ls(schanged);

  vub.addListener(&lb);
  vui.addListener(&li);
  vud.addListener(&ld);
  vus.addListener(&ls);

  // Activation alone should not notify
  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  assertTrue_1(!bchanged);
  assertTrue_1(!ichanged);
  assertTrue_1(!dchanged);
  assertTrue_1(!schanged);

  // Reset for uninitialized should NOT notify
  vub.deactivate();
  vui.deactivate();
  vud.deactivate();
  vus.deactivate();

  vub.activate();
  vui.activate();
  vud.activate();
  vus.activate();

  assertTrue_1(!bchanged);
  assertTrue_1(!ichanged);
  assertTrue_1(!dchanged);
  assertTrue_1(!schanged);

  // Assign should notify this time
  vub.setValue(Value(false));
  vui.setValue(Value((Integer) 69));
  vud.setValue(Value(1.414));
  vus.setValue(Value("jojo"));

  assertTrue_1(bchanged);
  assertTrue_1(ichanged);
  assertTrue_1(dchanged);
  assertTrue_1(schanged);

  // Save current value shouldn't notify
  bchanged = ichanged = dchanged = schanged =
    false;
  vub.saveCurrentValue();
  vui.saveCurrentValue();
  vud.saveCurrentValue();
  vus.saveCurrentValue();

  assertTrue_1(!bchanged);
  assertTrue_1(!ichanged);
  assertTrue_1(!dchanged);
  assertTrue_1(!schanged);

  // Restoring same value shouldn't notify
  vub.restoreSavedValue();
  vui.restoreSavedValue();
  vud.restoreSavedValue();
  vus.restoreSavedValue();

  assertTrue_1(!bchanged);
  assertTrue_1(!ichanged);
  assertTrue_1(!dchanged);
  assertTrue_1(!schanged);

  // Making unknown should notify
  vub.setUnknown();
  vui.setUnknown();
  vud.setUnknown();
  vus.setUnknown();

  assertTrue_1(bchanged);
  assertTrue_1(ichanged);
  assertTrue_1(dchanged);
  assertTrue_1(schanged);

  // Now restoring should notify
  bchanged = ichanged = dchanged = schanged = false;
  vub.restoreSavedValue();
  vui.restoreSavedValue();
  vud.restoreSavedValue();
  vus.restoreSavedValue();

  assertTrue_1(bchanged);
  assertTrue_1(ichanged);
  assertTrue_1(dchanged);
  assertTrue_1(schanged);

  // Clean up
  vub.removeListener(&lb);
  vui.removeListener(&li);
  vud.removeListener(&ld);
  vus.removeListener(&ls);

  return true;
}

bool variablesTest()
{
  runTest(testUninitialized);
  runTest(testInitialValue);
  runTest(testInitializers);
  runTest(testSavedValue);
  runTest(testAssignablePointer);
  runTest(testNotification);

  return true;
}
