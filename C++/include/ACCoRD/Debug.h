/*
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#include <string>

namespace larcfm {
class Debug{
private:
	static bool userDebugFlag;
public:
	static void setUserDebugFlag(bool b);
	/**
	 * Print out a debugging message msg, with each line prepended with tag
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg the debug message
	 */
	static void pln(const std::string& tag, const std::string& msg);
//	static void pln(const std::string& msg);
};
}
#endif /* FORMAT_H_ */
