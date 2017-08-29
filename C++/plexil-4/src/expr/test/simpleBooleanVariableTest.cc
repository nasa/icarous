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

#include "SimpleBooleanVariable.hh"
#include "TestSupport.hh"
#include "Value.hh"
#include "test/TrivialListener.hh"

using namespace PLEXIL;


static bool testUninitializedSBV()
{
  SimpleBooleanVariable vub;

  // Test that it is not constant
  assertTrue_1(!vub.isConstant());

  // Test that it is created inactive
  assertTrue_1(!vub.isActive());

  // Test that it is known and false while inactive
  assertTrue_1(vub.isKnown());
  bool foob;
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(!foob);

  // Activate and confirm it is known and false
  vub.activate();
  assertTrue_1(vub.isKnown());
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(!foob);

  // Assign and check result
  vub.setValue(true);
  assertTrue_1(vub.isKnown());
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob);

  // Reset and check that value is now false
  vub.deactivate();
  vub.activate();
  assertTrue_1(vub.isKnown());
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(!foob);

  // Set value and check result
  vub.setValue(true);
  assertTrue_1(vub.isKnown());
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob);

  // Set value and check that it changed
  vub.setValue(false);
  assertTrue_1(vub.isKnown());
  assertTrue_1(vub.getValue(foob));
  assertTrue_1(foob == false);

  return true;
}

static bool testNotificationSBV()
{
  SimpleBooleanVariable vub;
  bool bchanged = false;
  TrivialListener lb(bchanged);
  vub.addListener(&lb);

  // Activation alone should not notify
  vub.activate();
  assertTrue_1(!bchanged);

  // Activate after reset should not notify
  vub.deactivate();
  vub.activate();
  assertTrue_1(!bchanged);

  // Assign same value should not notify
  bchanged = false;
  vub.setValue(false);
  assertTrue_1(!bchanged);
  vub.setValue(true);
  assertTrue_1(bchanged);

  // Should not notify when deactivated and activated again
  bchanged = false;
  vub.deactivate();
  vub.activate();
  assertTrue_1(!bchanged);

  // Clean up
  vub.removeListener(&lb);

  return true;
}

bool simpleBooleanVariableTest()
{
  runTest(testUninitializedSBV);
  runTest(testNotificationSBV);

  return true;
}

