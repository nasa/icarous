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

#include "Error.hh"
#include "SimpleMap.hh"
#include "TestSupport.hh"
#include "map-utils.hh"

using namespace PLEXIL;

bool testInts()
{
  SimpleMap<int, std::string> intMap;

  assertTrue_1(intMap.empty());
  assertTrue_1(intMap.size() == 0);

  // insert out of order
  intMap.insert(1, "one");
  intMap.insert(3, "three");
  intMap.insert(7, "seven");
  intMap.insert(2, "two");
  intMap.insert(6, "six");
  intMap.insert(5, "five");
  intMap.insert(4, "four");
  intMap.insert(8, "eight");

  assertTrue_1(!intMap.empty());
  assertTrue_1(intMap.size() == 8);

  assertTrue_1(intMap.find(4) != intMap.end());
  assertTrue_1(intMap.find(4)->second == "four");
  assertTrue_1(intMap.find(1) != intMap.end());
  assertTrue_1(intMap.find(1)->second == "one");
  assertTrue_1(intMap.find(8) != intMap.end());
  assertTrue_1(intMap.find(8)->second == "eight");
  assertTrue_1(intMap.find(0) == intMap.end());
  assertTrue_1(intMap.find(9) == intMap.end());

  // test overwrite
  assertTrue_1(intMap[8] == "eight");
  intMap[8] = "ate";
  assertTrue_1(!intMap.empty());
  assertTrue_1(intMap.size() == 8);
  assertTrue_1(intMap.find(8) != intMap.end());
  assertTrue_1(intMap.find(8)->second != "eight");
  assertTrue_1(intMap.find(8)->second == "ate");
  assertTrue_1(intMap[8] != "eight");
  assertTrue_1(intMap[8] == "ate");
  
  return true;
}

bool testStrings()
{
  SimpleMap<std::string, int> stringMap;

  assertTrue_1(stringMap.empty());
  assertTrue_1(stringMap.size() == 0);

  // insert out of order
  stringMap.insert("one", 1);
  stringMap.insert("three", 3);
  stringMap.insert("seven", 7);
  stringMap.insert("two", 2);
  stringMap.insert("six", 6);
  stringMap.insert("five", 5);
  stringMap.insert("four", 4);
  stringMap.insert("eight", 8);

  assertTrue_1(!stringMap.empty());
  assertTrue_1(stringMap.size() == 8);

  assertTrue_1(stringMap.find("four") != stringMap.end());
  assertTrue_1(stringMap.find("four")->second == 4);
  assertTrue_1(stringMap.find("one") != stringMap.end());
  assertTrue_1(stringMap.find("one")->second == 1);
  assertTrue_1(stringMap.find("eight") != stringMap.end());
  assertTrue_1(stringMap.find("eight")->second == 8);
  assertTrue_1(stringMap.find("zero") == stringMap.end());
  assertTrue_1(stringMap.find("nine") == stringMap.end());

  // test overwrite
  assertTrue_1(stringMap["eight"] == 8);
  stringMap["eight"] = 88;
  assertTrue_1(!stringMap.empty());
  assertTrue_1(stringMap.size() == 8);
  assertTrue_1(stringMap.find("eight") != stringMap.end());
  assertTrue_1(stringMap.find("eight")->second != 8);
  assertTrue_1(stringMap.find("eight")->second == 88);
  assertTrue_1(stringMap["eight"] != 8);
  assertTrue_1(stringMap["eight"] == 88);
  
  return true;
}

bool testCStrings()
{
  SimpleMap<char const *, int, CStringComparator> cStringMap;

  assertTrue_1(cStringMap.empty());
  assertTrue_1(cStringMap.size() == 0);

  // insert out of order
  cStringMap.insert("one", 1);
  cStringMap.insert("three", 3);
  cStringMap.insert("seven", 7);
  cStringMap.insert("two", 2);
  cStringMap.insert("six", 6);
  cStringMap.insert("five", 5);
  cStringMap.insert("four", 4);
  cStringMap.insert("eight", 8);

  assertTrue_1(!cStringMap.empty());
  assertTrue_1(cStringMap.size() == 8);

  assertTrue_1(cStringMap.find("four") != cStringMap.end());
  assertTrue_1(cStringMap.find("four")->second == 4);
  assertTrue_1(cStringMap.find("one") != cStringMap.end());
  assertTrue_1(cStringMap.find("one")->second == 1);
  assertTrue_1(cStringMap.find("eight") != cStringMap.end());
  assertTrue_1(cStringMap.find("eight")->second == 8);
  assertTrue_1(cStringMap.find("zero") == cStringMap.end());
  assertTrue_1(cStringMap.find("nine") == cStringMap.end());

  // test overwrite
  assertTrue_1(cStringMap["eight"] == 8);
  cStringMap["eight"] = 88;
  assertTrue_1(!cStringMap.empty());
  assertTrue_1(cStringMap.size() == 8);
  assertTrue_1(cStringMap.find("eight") != cStringMap.end());
  assertTrue_1(cStringMap.find("eight")->second != 8);
  assertTrue_1(cStringMap.find("eight")->second == 88);
  assertTrue_1(cStringMap["eight"] != 8);
  assertTrue_1(cStringMap["eight"] == 88);
  
  return true;
}

bool testCStringIndexStringMap()
{
  typedef SimpleMap<std::string, int> StringMap;
  StringMap stringMap;

  assertTrue_1(stringMap.empty());
  assertTrue_1(stringMap.size() == 0);

  // insert out of order
  stringMap.insert("one", 1);
  stringMap.insert("three", 3);
  stringMap.insert("seven", 7);
  stringMap.insert("two", 2);
  stringMap.insert("six", 6);
  stringMap.insert("five", 5);
  stringMap.insert("four", 4);
  stringMap.insert("eight", 8);

  assertTrue_1(!stringMap.empty());
  assertTrue_1(stringMap.size() == 8);

  StringMap::const_iterator it = stringMap.find<const char *, CStringComparator>("four");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 4);
  it = stringMap.find<const char *, CStringComparator>("one");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 1);
  it = stringMap.find<const char *, CStringComparator>("eight");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 8);
  it = stringMap.find<const char *, CStringComparator>("zero");
  assertTrue_1(it == stringMap.end());
  it = stringMap.find<const char *, CStringComparator>("nine");
  assertTrue_1(it == stringMap.end());

  return true;
}

bool testPrefixStrings()
{
  typedef SimpleMap<std::string, int> StringMap;
  StringMap stringMap;

  assertTrue_1(stringMap.empty());
  assertTrue_1(stringMap.size() == 0);
  
  stringMap.insert("two", 2);
  stringMap.insert("twenty", 20);
  stringMap.insert("tu", -2);
  stringMap.insert("tutu", 22);
  stringMap.insert("twotwenty", 220);
  stringMap.insert("twotwentytwo", 222);

  assertTrue_1(!stringMap.empty());
  assertTrue_1(stringMap.size() == 6);

  StringMap::const_iterator it = stringMap.find<char const *, CStringComparator>("two");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 2);

  it = stringMap.find<char const *, CStringComparator>("tututu");
  assertTrue_1(it == stringMap.end());

  it = stringMap.find<char const *, CStringComparator>("twotwenty");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 220);

  it = stringMap.find<char const *, CStringComparator>("twotwentytwo");
  assertTrue_1(it != stringMap.end());
  assertTrue_1(it->second == 222);

  return true;
}

bool SimpleMapTest()
{
  runTest(testInts);
  runTest(testStrings);
  runTest(testCStrings);
  runTest(testCStringIndexStringMap);
  runTest(testPrefixStrings);

  return true;
}

