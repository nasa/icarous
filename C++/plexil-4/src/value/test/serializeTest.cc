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
#include "TestSupport.hh"
#include "Value.hh"

#include <cstring>

using namespace PLEXIL;

static size_t const BUFSIZE = 4096;

static char buffer[BUFSIZE];

//
// Scalars
//

static bool testBooleanSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;
  Boolean const falls = false;
  Boolean const treu = true;

  bufptr = serialize(falls, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(falls);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(treu, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(treu);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Boolean boolRead = true;
  offset = 0;

  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(boolRead == falls, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(falls);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(boolRead == treu, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(treu);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  oldcbufptr = cbufptr;
  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(boolRead, "deserialize modified result on bogus input");

  boolRead = false;
  cbufptr = deserialize(boolRead, oldcbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(!boolRead, "deserialize modified result on bogus input");

  return true;
}

static bool testCommandHandleSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;
  CommandHandleValue const sts = COMMAND_SENT_TO_SYSTEM;
  CommandHandleValue const acc = COMMAND_ACCEPTED;
  CommandHandleValue const rcv = COMMAND_RCVD_BY_SYSTEM;
  CommandHandleValue const fal = COMMAND_FAILED;
  CommandHandleValue const den = COMMAND_DENIED;
  CommandHandleValue const suc = COMMAND_SUCCESS;
  CommandHandleValue const err = COMMAND_INTERFACE_ERROR;

  bufptr = serialize(sts, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(sts);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(acc, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(acc);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(rcv, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(rcv);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(fal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(fal);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(den, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(den);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(suc, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(suc);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(err, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(err);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  uint16_t chRead = NO_COMMAND_HANDLE;
  offset = 0;

  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == sts, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(sts);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == acc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(acc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == rcv, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(rcv);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == fal, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(fal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == den, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(den);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == suc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(suc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == err, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(err);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  chRead = NO_COMMAND_HANDLE;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(chRead, "deserialize modified result on bogus input");

  return true;
}

static bool testIntegerSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  Integer zero = 0;
  Integer one = 1;
  Integer minusOne = -1;
  Integer largeInt = 2000000000;
  Integer largeNegInt = -2000000000;
  size_t offset = 0;

  bufptr = serialize(zero, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(zero);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  char *oldbufptr = bufptr;
  bufptr = serialize(one, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(one);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(minusOne, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(largeInt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeInt);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(largeNegInt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeNegInt);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Integer intRead = -42; // distinctive initial value
  offset = 0;

  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == zero, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == one, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == minusOne, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == largeInt, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == largeNegInt, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeNegInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(intRead == largeNegInt, "deserialize modified result on bogus input");
  return true;
}

static bool testRealSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  Real zero = 0;
  Real one = 1;
  Real minusOne = -1;
  Real largeReal = 2e100;
  Real smallNegReal = -2e-100;
  size_t offset = 0;

  bufptr = serialize(zero, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(zero);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  char *oldbufptr = bufptr;
  bufptr = serialize(one, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(one);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(minusOne, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(largeReal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeReal);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  oldbufptr = bufptr;
  bufptr = serialize(smallNegReal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(smallNegReal);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Real realRead = -42; // distinctive initial value
  offset = 0;

  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == zero, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == one, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == minusOne, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == largeReal, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == smallNegReal, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(smallNegReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(realRead == smallNegReal, "deserialize modified result on bogus input");
  return true;
}

static bool testStringSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  String mt = "";
  String simple = "simple";

  // Write
  bufptr = serialize(mt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(mt) == 4, "serialSize returned wrong size for empty string");
  offset += serialSize(mt); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(simple, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(simple) == 10, "serialSize returned wrong size for simple string");
  offset += serialSize(simple); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read

  char const *cbufptr = buffer;
  String stringRead;
  offset = 0;

  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(mt); 
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(stringRead.empty(), "deserialize put garbage in empty string");

  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > offset + (char *) buffer, "deserialize didn't return incremented pointer");
  offset += serialSize(simple);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment pointer by expected number");
  assertTrueMsg(!stringRead.empty(), "deserialize returned empty result string");
  assertTrueMsg(stringRead.size() == simple.size(), "deserialize returned wrong string length");
  assertTrueMsg(stringRead == simple, "deserialize put garbage in simple string");

  // Test reading junk
  String bogus = "bOgUs";
  stringRead = bogus;
  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(stringRead == bogus, "deserialize modified result on bogus input");

  return true;
}

static bool testCharStringSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  char const *mt = "";
  char const *simple = "simple";

  // Write
  bufptr = serialize(mt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(mt) == 4, "serialSize returned wrong size for empty string");
  offset += serialSize(mt); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(simple, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(simple) == 10, "serialSize returned wrong size for simple string");
  offset += serialSize(simple); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read

  char const *cbufptr = buffer;
  char *stringRead = NULL;
  offset = 0;

  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(mt); 
  assertTrueMsg(cbufptr == offset + (char *) buffer,
		"deserialize didn't increment buffer pointer by expected number " << serialSize(mt));
  assertTrueMsg(!strlen(stringRead), "deserialize put garbage in empty string");

  delete stringRead;
  stringRead = NULL;
  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > offset + (char *) buffer, "deserialize didn't return incremented pointer");
  offset += serialSize(simple);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment pointer by expected number");
  assertTrueMsg(strlen(stringRead) == strlen(simple), "deserialize returned wrong string length");
  assertTrueMsg(!strcmp(stringRead, simple), "deserialize put garbage in simple string");

  // Test reading junk
  delete stringRead;
  stringRead = NULL;
  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(stringRead == NULL, "deserialize modified result on bogus input");

  return true;
}

static bool testBasicSerDes()
{
  testBooleanSerDes();
  testCommandHandleSerDes();
  testIntegerSerDes();
  testRealSerDes();
  testStringSerDes();
  testCharStringSerDes();

  return true;
}

//
// Arrays
//

static bool testBooleanArraySerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  BooleanArray const b0;
  BooleanArray const b10f(10, false);
  BooleanArray brand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < brand.size(); ++i, ix += i)
    brand.setElement(ix, (ix & 1) != 0);

  bufptr = serialize(b0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(b0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(b10f, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(b10f);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(brand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(brand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  BooleanArray tmp(1, true); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(b0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == b0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(b10f);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == b10f, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(brand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == brand, "deserialize failed to extract data correctly");

  return true;
}

static bool testIntegerArraySerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  IntegerArray const i0;
  IntegerArray const i10_0(10, 0);
  IntegerArray irand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < irand.size(); ++i, ix += i)
    irand.setElement(ix, (Integer) i);

  bufptr = serialize(i0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(i0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(i10_0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(i10_0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(irand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(irand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  IntegerArray tmp(1, 1); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(i0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == i0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(i10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == i10_0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(irand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == irand, "deserialize failed to extract data correctly");

  return true;
}

static bool testRealArraySerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  RealArray const r0;
  RealArray const r10_0(10, 0);
  RealArray rrand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < rrand.size(); ++i, ix += i)
    rrand.setElement(ix, (Real) i);

  bufptr = serialize(r0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(r0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(r10_0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(r10_0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(rrand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(rrand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  RealArray tmp(1, 1); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(r0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == r0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(r10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == r10_0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(rrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == rrand, "deserialize failed to extract data correctly");

  return true;
}

static bool testStringArraySerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  StringArray const s0;
  StringArray const s10_e(10, "");
  StringArray srand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < srand.size(); ++i, ix += i)
    srand.setElement(ix, String(i, 'a'));

  bufptr = serialize(s0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(s0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(s10_e, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(s10_e);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(srand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(srand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  StringArray tmp(1, "1"); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(s0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == s0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(s10_e);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == s10_e, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(srand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == srand, "deserialize failed to extract data correctly");

  return true;
}

static bool testArraySerDes()
{
  testBooleanArraySerDes();
  testIntegerArraySerDes();
  testRealArraySerDes();
  testStringArraySerDes();

  return true;
}

//
// Value tests
//

// TODO:
// - Unknown
// - additional types?

static bool testBooleanValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;
  Value const falls = false;
  Value const treu = true;

  assertTrueMsg(serialSize((Boolean) false) == serialSize(falls), "serialSize differs between Value & Boolean");
  bufptr = serialize(falls, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(falls);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize((Boolean) true) == serialSize(treu), "serialSize differs between Value & Boolean");
  bufptr = serialize(treu, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(treu);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Boolean boolRead = true;
  offset = 0;

  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(boolRead == false, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize((Boolean) false);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(boolRead == true, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize((Boolean) true);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  oldcbufptr = cbufptr;
  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(boolRead, "deserialize modified result on bogus input");

  boolRead = false;
  cbufptr = deserialize(boolRead, oldcbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(!boolRead, "deserialize modified result on bogus input");

  // Read again as Value

  cbufptr = buffer;
  Value v;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(falls);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == BOOLEAN_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(boolRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == falls, "deserialize didn't set result equal to source");
  assertTrueMsg(boolRead == false, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(treu);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == BOOLEAN_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(boolRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == treu, "deserialize didn't set result equal to source");
  assertTrueMsg(boolRead == true, "deserialize didn't set result equal to source");

  // test reading past end
  cbufptr = deserialize(boolRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");

  return true;
}

static bool testCommandHandleValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  size_t offset = 0;
  CommandHandleValue const sts = COMMAND_SENT_TO_SYSTEM;
  CommandHandleValue const acc = COMMAND_ACCEPTED;
  CommandHandleValue const rcv = COMMAND_RCVD_BY_SYSTEM;
  CommandHandleValue const fal = COMMAND_FAILED;
  CommandHandleValue const den = COMMAND_DENIED;
  CommandHandleValue const suc = COMMAND_SUCCESS;
  Value const vsts(sts, COMMAND_HANDLE_TYPE);
  Value const vacc(acc, COMMAND_HANDLE_TYPE);
  Value const vrcv(rcv, COMMAND_HANDLE_TYPE);
  Value const vfal(fal, COMMAND_HANDLE_TYPE);
  Value const vden(den, COMMAND_HANDLE_TYPE);
  Value const vsuc(suc, COMMAND_HANDLE_TYPE);

  assertTrueMsg(serialSize(sts) == serialSize(vsts),
		"serialSize differs between Value " << serialSize(vsts)
		<< " and CommandHandleValue " << serialSize(sts));
  bufptr = serialize(vsts, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vsts);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(acc) == serialSize(vacc), "serialSize differs between Value & CommandHandleValue");
  bufptr = serialize(vacc, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vacc);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(rcv) == serialSize(vrcv), "serialSize differs between Value & CommandHandleValue");
  bufptr = serialize(vrcv, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vrcv);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(fal) == serialSize(vfal), "serialSize differs between Value & CommandHandleValue");
  bufptr = serialize(vfal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vfal);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(den) == serialSize(vden), "serialSize differs between Value & CommandHandleValue");
  bufptr = serialize(vden, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vden);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(suc) == serialSize(vsuc), "serialSize differs between Value & CommandHandleValue");
  bufptr = serialize(vsuc, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(vsuc);
  assertTrueMsg(bufptr == offset + (char *) buffer,
		"serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read as CommandHandleValue
  char const *cbufptr = buffer;
  uint16_t chRead = NO_COMMAND_HANDLE;
  offset = 0;

  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == sts, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(sts);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == acc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(acc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == rcv, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(rcv);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == fal, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(fal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == den, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(den);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(chRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(chRead == suc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(suc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // Read again as Value
  cbufptr = buffer;
  offset = 0;
  Value vRead;

  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vsts, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(vsts);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vacc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(vacc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vrcv, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(vrcv);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vfal, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(vfal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vden, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(vden);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(vRead.valueType() == COMMAND_HANDLE_TYPE, "deserialize got wrong type");
  assertTrueMsg(vRead == vsuc, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(vsuc);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  
  // test reading past end
  vRead.setUnknown();
  cbufptr = deserialize(vRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(!vRead.isKnown(), "deserialize modified result on bogus input");

  return true;
}

static bool testIntegerValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  Value zero = (Integer) 0;
  Value one = (Integer) 1;
  Value minusOne = (Integer) -1;
  Value largeInt = (Integer) 2000000000;
  Value largeNegInt = (Integer) -2000000000;
  size_t offset = 0;

  assertTrueMsg(serialSize(zero) == serialSize((Integer) 0), "serialSize differs between Value and Integer");
  bufptr = serialize(zero, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(zero);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(one) == serialSize((Integer) 1), "serialSize differs between Value and Integer");
  char *oldbufptr = bufptr;
  bufptr = serialize(one, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(one);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(minusOne) == serialSize((Integer) -1), "serialSize differs between Value and Integer");
  oldbufptr = bufptr;
  bufptr = serialize(minusOne, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(largeInt) == serialSize((Integer) 2000000000), "serialSize differs between Value and Integer");
  oldbufptr = bufptr;
  bufptr = serialize(largeInt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeInt);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(largeNegInt) == serialSize((Integer) -2000000000), "serialSize differs between Value and Integer");
  oldbufptr = bufptr;
  bufptr = serialize(largeNegInt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeNegInt);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Integer intRead = -42; // distinctive initial value
  offset = 0;

  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == 0, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == 1, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == -1, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == 2000000000, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(intRead == -2000000000, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeNegInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  cbufptr = deserialize(intRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(intRead == -2000000000, "deserialize modified result on bogus input");

  // Read again as Value

  cbufptr = buffer;
  Value v;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == INTEGER_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(intRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == zero, "deserialize didn't set result equal to source");
  assertTrueMsg(intRead == 0, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == INTEGER_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(intRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == one, "deserialize didn't set result equal to source");
  assertTrueMsg(intRead == 1, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == INTEGER_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(intRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == minusOne, "deserialize didn't set result equal to source");
  assertTrueMsg(intRead == -1, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == INTEGER_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(intRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == largeInt, "deserialize didn't set result equal to source");
  assertTrueMsg(intRead == 2000000000, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeNegInt);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == INTEGER_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(intRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == largeNegInt, "deserialize didn't set result equal to source");
  assertTrueMsg(intRead == -2000000000, "deserialize didn't set result equal to source");

  // test reading past end
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");

  return true;
}

static bool testRealValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);

  // Write
  char *bufptr = buffer;
  Value zero = (Real) 0;
  Value one = (Real) 1;
  Value minusOne = (Real) -1;
  Value largeReal = (Real) 2e100;
  Value smallNegReal = (Real) -2e-100;
  size_t offset = 0;

  assertTrueMsg(serialSize(zero) == serialSize((Real) 0), "serialSize differs between Value and Real");
  bufptr = serialize(zero, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  offset += serialSize(zero);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(one) == serialSize((Real) 1), "serialSize differs between Value and Real");
  char *oldbufptr = bufptr;
  bufptr = serialize(one, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(one);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(minusOne) == serialSize((Real) -1), "serialSize differs between Value and Real");
  oldbufptr = bufptr;
  bufptr = serialize(minusOne, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(largeReal) == serialSize((Real) 2e100), "serialSize differs between Value and Real");
  oldbufptr = bufptr;
  bufptr = serialize(largeReal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(largeReal);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(smallNegReal) == serialSize((Real) -2e-100), "serialSize differs between Value and Real");
  oldbufptr = bufptr;
  bufptr = serialize(smallNegReal, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > oldbufptr, "serialize didn't return incremented pointer");
  offset += serialSize(smallNegReal);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  Real realRead = -42; // distinctive initial value
  offset = 0;

  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == (Real) 0, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  char const *oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == (Real) 1, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == (Real) -1, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == (Real) 2e100, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(realRead == (Real) -2e-100, "deserialize didn't set result equal to source");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(smallNegReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");

  // test reading past end
  cbufptr = deserialize(realRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(realRead == smallNegReal, "deserialize modified result on bogus input");

  // Read again as Value

  cbufptr = buffer;
  Value v;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(zero);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == REAL_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(realRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == zero, "deserialize didn't set result equal to source");
  assertTrueMsg(realRead == (Real) 0, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(one);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == REAL_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(realRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == one, "deserialize didn't set result equal to source");
  assertTrueMsg(realRead == (Real) 1, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(minusOne);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == REAL_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(realRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == minusOne, "deserialize didn't set result equal to source");
  assertTrueMsg(realRead == (Real) -1, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(largeReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == REAL_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(realRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == largeReal, "deserialize didn't set result equal to source");
  assertTrueMsg(realRead == (Real) 2e100, "deserialize didn't set result equal to source");

  oldcbufptr = cbufptr;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > oldcbufptr, "deserialize didn't increment buffer pointer");
  offset += serialSize(smallNegReal);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == REAL_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(realRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == smallNegReal, "deserialize didn't set result equal to source");
  assertTrueMsg(realRead == (Real) -2e-100, "deserialize didn't set result equal to source");

  // test reading past end
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");

  return true;
}

static bool testStringValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  Value mt = "";
  Value simple = "simple";

  // Write
  bufptr = serialize(mt, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(mt) == 4, "serialSize returned wrong size for empty string");
  offset += serialSize(mt); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  bufptr = serialize(simple, bufptr);
  assertTrueMsg(bufptr, "serialize returned NULL");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize didn't return incremented pointer");
  assertTrueMsg(serialSize(simple) == 10, "serialSize returned wrong size for simple string");
  offset += serialSize(simple); 
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize didn't increment pointer by expected number");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read

  char const *cbufptr = buffer;
  String stringRead;
  offset = 0;

  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(String("")); 
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(stringRead.empty(), "deserialize put garbage in empty string");

  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > offset + (char *) buffer, "deserialize didn't return incremented pointer");
  offset += serialSize(String("simple"));
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment pointer by expected number");
  assertTrueMsg(!stringRead.empty(), "deserialize returned empty result string");
  assertTrueMsg(stringRead.size() == simple.valueToString().size(), "deserialize returned wrong string length");
  assertTrueMsg(stringRead == simple, "deserialize put garbage in simple string");

  // Test reading junk
  String const bogus = "bOgUs";
  stringRead = bogus;
  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");
  assertTrueMsg(stringRead == bogus, "deserialize modified result on bogus input");

  // Read again as Value

  cbufptr = buffer;
  offset = 0;
  Value v;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize didn't increment buffer pointer");
  offset += serialSize(mt); 
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment buffer pointer by expected number");
  assertTrueMsg(v.valueType() == STRING_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(stringRead), "deserialize failed to extract correct type");
  assertTrueMsg(v == mt, "deserialize didn't set result equal to source");
  assertTrueMsg(stringRead.empty(), "deserialize didn't set result equal to source");

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null buffer pointer");
  assertTrueMsg(cbufptr > offset + (char *) buffer, "deserialize didn't return incremented pointer");
  offset += serialSize(simple);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize didn't increment pointer by expected number");
  assertTrueMsg(v.valueType() == STRING_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValue(stringRead), "deserialize failed to extract correct type");
  assertTrueMsg(stringRead.size() == simple.valueToString().size(), "deserialize returned wrong string length");
  assertTrueMsg(v == simple, "deserialize didn't set result equal to source");
  assertTrueMsg(stringRead == String("simple"), "deserialize didn't set result equal to source");

  // Test reading junk
  stringRead = bogus;
  cbufptr = deserialize(stringRead, cbufptr);
  assertTrueMsg(!cbufptr, "deserialize failed to return null buffer pointer on bogus input");

  return true;
}

static bool testBooleanArrayValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  BooleanArray const b0;
  BooleanArray const b10f(10, false);
  BooleanArray brand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < brand.size(); ++i, ix += i)
    brand.setElement(ix, (ix & 1) != 0);

  Value const v0(b0);
  Value const v10f(b10f);
  Value const vrand(brand);

  assertTrueMsg(serialSize(b0) == serialSize(v0), "serialSize disagrees between array & value");
  bufptr = serialize(v0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(b10f) == serialSize(v10f), "serialSize disagrees between array & value");
  bufptr = serialize(v10f, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v10f);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(brand) == serialSize(vrand), "serialSize disagrees between array & value");
  bufptr = serialize(vrand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  BooleanArray tmp(1, true); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == b0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10f);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == b10f, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == brand, "deserialize failed to extract data correctly");

  // Read again as Value
  cbufptr = buffer;
  Value v;
  BooleanArray const *bap = NULL;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == BOOLEAN_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(bap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v0, "deserialize failed to extract data correctly");
  assertTrueMsg(*bap == b0, "deserialize failed to extract data correctly");

  v.setUnknown();
  bap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10f);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == BOOLEAN_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(bap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v10f, "deserialize failed to extract data correctly");
  assertTrueMsg(*bap == b10f, "deserialize failed to extract data correctly");

  v.setUnknown();
  bap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == BOOLEAN_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(bap), "deserialize failed to extract correct type");
  assertTrueMsg(v == vrand, "deserialize failed to extract data correctly");
  assertTrueMsg(*bap == brand, "deserialize failed to extract data correctly");

  return true;
}

static bool testIntegerArrayValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  IntegerArray const i0;
  IntegerArray const i10_0(10, 0);
  IntegerArray irand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < irand.size(); ++i, ix += i)
    irand.setElement(ix, (Integer) i);

  Value const v0(i0);
  Value const v10_0(i10_0);
  Value const vrand(irand);

  assertTrueMsg(serialSize(i0) == serialSize(v0), "serialSize differs between value & array");
  bufptr = serialize(v0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(i10_0) == serialSize(v10_0), "serialSize differs between value & array");
  bufptr = serialize(v10_0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(irand) == serialSize(vrand), "serialSize differs between value & array");
  bufptr = serialize(vrand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  IntegerArray tmp(1, 1); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == i0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == i10_0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == irand, "deserialize failed to extract data correctly");

  // Read again as Value
  cbufptr = buffer;
  Value v;
  IntegerArray const *iap = NULL;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == INTEGER_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(iap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v0, "deserialize failed to extract data correctly");
  assertTrueMsg(*iap == i0, "deserialize failed to extract data correctly");

  v.setUnknown();
  iap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == INTEGER_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(iap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v10_0, "deserialize failed to extract data correctly");
  assertTrueMsg(*iap == i10_0, "deserialize failed to extract data correctly");

  v.setUnknown();
  iap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == INTEGER_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(iap), "deserialize failed to extract correct type");
  assertTrueMsg(v == vrand, "deserialize failed to extract data correctly");
  assertTrueMsg(*iap == irand, "deserialize failed to extract data correctly");

  return true;
}

static bool testRealArrayValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  RealArray const r0;
  RealArray const r10_0(10, 0);
  RealArray rrand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < rrand.size(); ++i, ix += i)
    rrand.setElement(ix, (Real) i);

  Value const v0(r0);
  Value const v10_0(r10_0);
  Value const vrand(rrand);

  assertTrueMsg(serialSize(r0) == serialSize(v0), "serialSize differs between Value & array");
  bufptr = serialize(v0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(r10_0) == serialSize(v10_0), "serialSize differs between Value & array");
  bufptr = serialize(v10_0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(rrand) == serialSize(vrand), "serialSize differs between Value & array");
  bufptr = serialize(vrand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  RealArray tmp(1, 1); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == r0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == r10_0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == rrand, "deserialize failed to extract data correctly");

  // Read again as Value
  cbufptr = buffer;
  Value v;
  RealArray const *rap = NULL;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == REAL_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(rap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v0, "deserialize failed to extract data correctly");
  assertTrueMsg(*rap == r0, "deserialize failed to extract data correctly");

  v.setUnknown();
  rap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == REAL_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(rap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v10_0, "deserialize failed to extract data correctly");
  assertTrueMsg(*rap == r10_0, "deserialize failed to extract data correctly");

  v.setUnknown();
  rap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == REAL_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(rap), "deserialize failed to extract correct type");
  assertTrueMsg(v == vrand, "deserialize failed to extract data correctly");
  assertTrueMsg(*rap == rrand, "deserialize failed to extract data correctly");

  return true;
}

static bool testStringArrayValueSerDes()
{
  // Fill buffer
  memset((void *) buffer, 0xFF, BUFSIZE);
  char *bufptr = buffer;
  size_t offset = 0;

  // Initialize test data
  StringArray const s0;
  StringArray const s10_e(10, "");
  StringArray srand(32);
  size_t ix = 0;
  for (size_t i = 0; ix < srand.size(); ++i, ix += i)
    srand.setElement(ix, String(i, 'a'));

  Value const v0(s0);
  Value const v10_e(s10_e);
  Value const vrand(srand);

  assertTrueMsg(serialSize(s0) == serialSize(v0), "serialSize differs between Value & array");
  bufptr = serialize(v0, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr != (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(s10_e) == serialSize(v10_e), "serialSize differs between Value & array");
  bufptr = serialize(v10_e, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(v10_e);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  assertTrueMsg(serialSize(srand) == serialSize(vrand), "serialSize differs between Value & array");
  bufptr = serialize(vrand, bufptr);
  assertTrueMsg(bufptr, "serialize returned null pointer");
  assertTrueMsg(bufptr > offset + (char *) buffer, "serialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(bufptr == offset + (char *) buffer, "serialize failed to increment pointer as expected");
  assertTrueMsg(0xFF == (unsigned char) buffer[offset], "serialize wrote more than it should have");

  // Read
  char const *cbufptr = buffer;
  StringArray tmp(1, "1"); // initialize differently than test data
  offset = 0;

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 1, "deserialize failed to resize destination");
  assertTrueMsg(tmp == s0, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_e);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 0, "deserialize failed to resize destination");
  assertTrueMsg(tmp == s10_e, "deserialize failed to extract data correctly");

  cbufptr = deserialize(tmp, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(tmp.size() != 10, "deserialize failed to resize destination");
  assertTrueMsg(tmp == srand, "deserialize failed to extract data correctly");

  // Read again as Value
  cbufptr = buffer;
  Value v;
  StringArray const *sap = NULL;
  offset = 0;

  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v0);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == STRING_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(sap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v0, "deserialize failed to extract data correctly");
  assertTrueMsg(*sap == s0, "deserialize failed to extract data correctly");

  v.setUnknown();
  sap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(v10_e);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == STRING_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(sap), "deserialize failed to extract correct type");
  assertTrueMsg(v == v10_e, "deserialize failed to extract data correctly");
  assertTrueMsg(*sap == s10_e, "deserialize failed to extract data correctly");

  v.setUnknown();
  sap = NULL;
  cbufptr = deserialize(v, cbufptr);
  assertTrueMsg(cbufptr, "deserialize returned null pointer");
  assertTrueMsg(cbufptr > (char *) buffer, "deserialize failed to increment pointer");
  offset += serialSize(vrand);
  assertTrueMsg(cbufptr == offset + (char *) buffer, "deserialize failed to increment pointer as expected");
  assertTrueMsg(v.valueType() == STRING_ARRAY_TYPE, "deserialize failed to extract correct type");
  assertTrueMsg(v.getValuePointer(sap), "deserialize failed to extract correct type");
  assertTrueMsg(v == vrand, "deserialize failed to extract data correctly");
  assertTrueMsg(*sap == srand, "deserialize failed to extract data correctly");

  return true;
}

static bool testValueSerDes()
{
  testBooleanValueSerDes();
  testCommandHandleValueSerDes();
  testIntegerValueSerDes();
  testRealValueSerDes();
  testStringValueSerDes();

  testBooleanArrayValueSerDes();
  testIntegerArrayValueSerDes();
  testRealArrayValueSerDes();
  testStringArrayValueSerDes();

  // more later

  return true;
}

bool serializeTest()
{
  runTest(testBasicSerDes);
  runTest(testArraySerDes);
  runTest(testValueSerDes);
  return true;
}  
