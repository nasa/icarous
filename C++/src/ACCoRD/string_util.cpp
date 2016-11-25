/* 
 * string_util - String utilities available in Java that we want (or
 *               we want an easier interface to) in C++
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Cesar Munoz, George Hagen
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "string_util.h"
#include "format.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace larcfm {
  
  using std::string;
  using std::vector;

    vector<string> split(const string& str,const string& delimiters) {
      vector<string> tokens;
      string::size_type lastPos = str.find_first_not_of(delimiters, 0);
      string::size_type pos     = str.find_first_of(delimiters, lastPos);
      while (string::npos != pos || string::npos != lastPos) {
    	tokens.push_back(str.substr(lastPos, pos - lastPos));
        lastPos = str.find_first_not_of(delimiters, pos);
        pos = str.find_first_of(delimiters, lastPos);
      }
      return tokens;
    }
  
    vector<string> split_empty(const string& str,const string& delimiters) {
      vector<string> tokens;
      string::size_type lastPos = 0; //str.find_first_not_of(delimiters, 0);
      string::size_type pos     = str.find_first_of(delimiters, lastPos);
      while (string::npos != pos) {
    	  //fpln(" www "+str.substr(lastPos, pos - lastPos));
    	  if (pos == lastPos) {
    		  tokens.push_back("");
    	  } else {
    	      tokens.push_back(str.substr(lastPos, pos - lastPos));
    	  }
        lastPos = pos+1;
        pos = str.find_first_of(delimiters, lastPos);
      }
      if (lastPos != str.size()) {
    	  tokens.push_back(str.substr(lastPos, str.size() - lastPos));
      }
      if (lastPos == str.size()) {
    	  tokens.push_back("");
      }
      return tokens;
    }

    string substring(const string& s, int begin, int end){
    	end = (end < (int) s.length()) ? end : s.length();
    	if (begin < end || begin < 0) {
        	return s.substr(begin,end-begin);
    	} else {
    		return "";
    	}
    }

    string substring(const string& s, int begin){
    	return substring(s, begin, s.length());
    }

    void trim(string& s,const string& drop){
      s.erase(s.find_last_not_of(drop)+1);
      s.erase(0,s.find_first_not_of(drop));
    }
  
  string toLowerCase(const string& strToConvert) {//change each element of the string to lower case
    string nstr;
    nstr.resize(strToConvert.size());

    for(unsigned int i=0;i<strToConvert.size();i++)  {
      nstr[i] = tolower(strToConvert[i]);
    }
    return nstr;  //return the converted string
    
  }
  
  bool equals(const string& s1, const string& s2) {
    return (s1.compare(s2) == 0);
  }
  
  bool equalsIgnoreCase(const string& s1, const string& s2) {
    return (toLowerCase(s1).compare(toLowerCase(s2)) == 0);
  }
  
  void replace(std::string& s, char c1, char c2) {
	  std::replace(s.begin(), s.end(), c1, c2);
  }

  bool contains(const std::vector<std::string>& v, const std::string& s) {
	  for(int i = 0; i < (int) v.size(); i++) {
		  if (equals(v[i],s)) return true;
	  }
	  return false;
  }

  bool contains(const std::string& str, const std::string& sub) {
	  return str.find(sub) != string::npos;
  }

  bool startsWith(const std::string& str, const std::string& prefix) {
	  return str.substr(0,prefix.length()).compare(prefix) == 0;
  }

  bool endsWith(const std::string& str, const std::string& suffix) {
	  return suffix.length() <= str.length() && str.substr(str.length() - suffix.length()).compare(suffix) == 0;
  }



}
