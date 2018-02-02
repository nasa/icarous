/* Copyright (c) 2006-2014, Universities Space Research Association (USRA).
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

#ifndef _H_SampleSystem
#define _H_SampleSystem

// This is a very simple abstraction of a real system that a Plexil plan might
// operate on.  It supports mutable state variables of varying types, and
// several commands.

class SampleSystem
{
 public:

  SampleSystem();
  // using compiler's destructor; no assignment or copy
  
  float getSize () { return m_size; } 
  void setSize (float);

  int getSpeed () { return m_speed; }
  void setSpeed (int);

  std::string getColor () { return m_color; }
  void setColor (const std::string&);

  // The overloaded 'at' functions support three variants of a parameterized
  // state called 'At'.  Note that 'At' is fundamentally different from the
  // states above; without parameters it is a data accessor, and otherwise it is
  // a predicate.

  std::string at () { return m_at_location; }
  bool at (const std::string& location) { return location == m_at_location; }
  bool at (int x, int y) { return (x == m_at_coordinates.first &&
								   y == m_at_coordinates.second); }

  // This command changes the 'at' state.
  void move (const std::string& location, int x, int y);

  // Some trivial commands
  void hello ();  
  int square (int x) { return x * x; }

 private:

  SampleSystem (const SampleSystem&);            // undefined - no copying
  SampleSystem& operator= (const SampleSystem&); // undefined - no assignment

  float m_size;
  int m_speed;
  std::string m_color;
  std::string m_at_location;
  std::pair<int, int> m_at_coordinates;
};

#endif
