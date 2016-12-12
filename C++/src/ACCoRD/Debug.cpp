/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Debug.h"
#include "string_util.h"
#include <string>
#include <iostream>
//#ifdef __GLIBC__
//#include <execinfo.h>
//#endif


namespace larcfm {
  
  using std::string;
  using std::endl;
  using std::cout;

  bool Debug::userDebugFlag = false;

  void Debug::setUserDebugFlag(bool b) {
	  userDebugFlag = b;
  }

  void Debug::pln(const string& tag, const string& msg) {
	  if (userDebugFlag) {
			std::vector<string> lines = split(msg,"\n");
			for (unsigned int i = 0; i < lines.size(); i++) {
				cout << "<!! " << tag << " !!> " << lines[i] << endl;
			}
			// no msg string
			if (lines.size() == 0) {
				cout << "<!! " << tag << " !!> " << endl;
			}
	  }
  }
  
//  void Debug::pln(const string& msg)
//  {
//#ifdef __GLIBC__
//      void *array[4];
//      size_t size;
//      char **strings;
//      size_t i;
//
//      size = backtrace (array, 4);
//      strings = backtrace_symbols (array, size);
//
//      if (size > 2) {
//    	  Debug::pln(strings[2],msg);
//      } else {
//    	  Debug::pln("",msg);
//      }
//      free (strings);
//#else
//        Debug::pln("",msg);
//#endif
//  }

}
