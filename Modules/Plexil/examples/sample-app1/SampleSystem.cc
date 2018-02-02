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

#include <iostream>
#include "subscriber.hh"
#include "SampleSystem.hh"

using std::cout;
using std::endl;
using std::string;
using std::pair;

SampleSystem::SampleSystem ()
  : m_size (5.1),
	m_speed (4),
	m_color ("Blue"),
	m_at_location ("Home"),
	m_at_coordinates (0,0)
{ }


void SampleSystem::setSize (float s)
{
  if (s != m_size) {
    m_size = s;
    publish ("Size", s);
  }
}


void SampleSystem::setSpeed (int s)
{
  if (s != m_speed) {
    m_speed = s;
    publish ("Speed", s);
  }
}

void SampleSystem::setColor (const string& c)
{
  if (c != m_color) {
    m_color = c;
    publish ("Color", c);
  }
}


void SampleSystem::move (const string& location, int x, int y)
{
  if (x != m_at_coordinates.first || y != m_at_coordinates.second) {
    m_at_coordinates.first = x;
    m_at_coordinates.second = y;
    publish ("At", true, x, y);
  }
  if (location != m_at_location) {
    m_at_location = location;
    publish ("At", true, location);
  }
}


void SampleSystem::hello ()
{
  cout << "Hello World" << endl;
}
