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

#include "State.hh"
#include "TestSupport.hh"

#include <cstring>

using namespace PLEXIL;

static size_t const BUFSIZE = 4096;

static char buffer[BUFSIZE];

static bool testBasicStateSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;
  State const s("foo");
  
  bufptr = serialize(s, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(s);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  State sread;
  offset = 0;

  cbufptr = deserialize(sread, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(sread == s, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(s);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  return true;
}

static bool testParamSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;

  State const s1("s1", Value());

  State s2("s2", 2);
  s2.setParameter(0, Value((Boolean) true));
  s2.setParameter(1, Value((Integer) 42));

  State s3("s3", 3);
  s3.setParameter(0, Value((Integer) 42));
  s3.setParameter(1, Value((Real) 3.14));
  s3.setParameter(2, Value("three"));

  bufptr = serialize(s1, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(s1);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(s2, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(s2);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(s3, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(s3);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  State sread;
  offset = 0;

  cbufptr = deserialize(sread, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(sread == s1, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(s1);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  cbufptr = deserialize(sread, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(sread == s2, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(s2);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  cbufptr = deserialize(sread, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(sread == s3, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(s3);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // Try read past end

  // test reading past end
  cbufptr = deserialize(sread, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(sread == s3, "deserialize modified result on bogus input");
  
  return true;
}


bool serializeTest()
{
  runTest(testBasicStateSerDes);
  runTest(testParamSerDes);
  // more to come
  return true;
}
