/* Copyright (c) 2006-2010, Universities Space Research Association (USRA).
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

#include <iostream>
#include "subscriber.hh"

using std::cout;
using std::endl;
using std::string;
using std::pair;

// The system's state, as variables.
//
static float Size = 5.1;
static int Speed = 4;
static string Color = "Blue";
static string AtLocation = "Home";
static pair<int, int> AtCoordinates (0,0);


// Functions that provide access (read and write) for the simple parameter-less
// states.  These functions are very similar and thus conveniently defined with
// a macro.  Note that state readers are lookups in a Plexil plan; state writers
// are commands.

#define defAccessors(name, type) \
type get##name () \
{ \
  return name; \
} \
void set##name (const type & s) \
{ \
  if (s != name) { \
    name = s; \
    publish (#name, s); \
  } \
}

defAccessors(Size, float)
defAccessors(Speed, int)
defAccessors(Color, string)

// The overloaded state 'At' accessors are handled individually.  Note that 'At'
// is fundamentally different from the states above; without parameters it is a
// data accessor, and otherwise it is a predicate.

string at ()
{
  return AtLocation;
}

bool at (const string& location)
{
  return (location == AtLocation);
}

bool at (int x, int y)
{
  return (x == AtCoordinates.first && y == AtCoordinates.second);
}

void move (const string& location, int x, int y)
{
  if (x != AtCoordinates.first || y != AtCoordinates.second) {
    AtCoordinates.first = x;
    AtCoordinates.second = y;
    publish ("At", true, x, y);
  }
  if (location != AtLocation) {
    AtLocation = location;
    publish ("At", true, location);
  }
}


// Some trivial commands

void hello ()
{
  cout << "Hello World" << endl;
}

int square (int x)
{
  return (x * x);
}
