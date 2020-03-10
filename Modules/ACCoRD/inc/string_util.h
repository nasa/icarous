/* 
 * string_util.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef STRING_UTIL_H
#define STRING_UTIL_H

#include "string_util.h"
#include <string>
#include <sstream>
#include <vector>

/*
 * String manipulation & parsing utility functions.
 * 
 */
 
namespace larcfm {

  /** return a vector of strings, split from a list of delimiters in str.  The pattern is not a regular expression, instead
   * it is a delimiter represented as a string.    */
  std::vector<std::string> split(const std::string& str,const std::string& delimiters);
  
  
  /** return a vector of strings, split from a list of delimiters in str. Allows empty strings */
  std::vector<std::string> split_empty(const std::string& str,const std::string& delimiters);

  /** return a vector of strings, split from str based on the string. Allows empty strings */
  std::vector<std::string> split_string_empty(const std::string& str,const std::string& delimiter);

  /** return a vector of strings, split from str based on the pattern given as a regular expression. */
  std::vector<std::string> split_regex(const std::string& str,const std::string& delimiters);

  bool matches(const std::string& s, const std::string& rgx_str);


  /** a C++ substring method that behaves more like Java's String.subString method.
   * the beginning index is inclusive, and the ending index is exclusive. */
  std::string substring(const std::string& s, int begin, int end);

  /** a C++ substring method that behaves more like Java's String.subString method.
   * the beginning index is inclusive, and the ending index is exclusive. */
  std::string substring(const std::string& s, int begin);

  /** remove specified characters from beginning & end of string s */
  void trim(std::string& s,const std::string& drop = " \t\r\n");
  
  /** remove specified characters from beginning & end of string s */
  std::string trimCopy(const std::string& s,const std::string& drop = " \t\r\n");

  /** return an all lower case copy of str */
  std::string toLowerCase(const std::string& str);
  
  /** return an all upper case copy of str */
  std::string toUpperCase(const std::string& str);

  /** return true if two strings are the same values */
  bool equals(const std::string&, const std::string&);
  
  /** return true if two strings are the same values, ignoring case */
  bool equalsIgnoreCase(const std::string&, const std::string&);

  /** Replaces all occurrences of c1 with c2 in string s */
  void replace(std::string& s, char c1, char c2);

  /** Replaces all occurrences of s1 with s2 in string s */
  std::string replace(const std::string& s, const std::string& s1, const std::string& s2);

  /** Is the given string in the list? */
  bool contains(const std::vector<std::string>& v, const std::string& s);

  /** Is the given string sub a substring of str? */
  bool contains(const std::string& str, const std::string& sub);

  /** Does str start with the prefix? */
  bool startsWith(const std::string& str, const std::string& prefix);

  /** Does str end with the suffix? */
  bool endsWith(const std::string& str, const std::string& suffix);

  /** parse a string into an integer */
  int parseInt(const std::string& str);

}

#endif // STRING_UTIL_H
