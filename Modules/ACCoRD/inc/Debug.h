/*
 * Copyright (c) 2011-2019 United States Government as represented by
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
	static const bool FAIL_FAST;
	static int VERBOSE;
	static void output(const std::string& tag, const std::string& msg);
public:
	
	static void setUserDebugFlag(bool b);

	// For compatibility with Java
	static void setVerbose(int level);

	/**
	 * Print out a debugging message msg, with each line prepended with tag
	 * 
	 * @param tag the tag to indicate the location of this debug message.
	 * @param msg the debug message
	 */
	static void pln(const std::string& tag, const std::string& msg);
	static void pln2(const std::string& msg, bool verbose);
	static void pln(const std::string& msg);
	static void pln(const std::string& tag, const std::string& msg, bool verbose);
	static void pln(int lvl, const std::string& tag, const std::string& msg);
	static void pln(int lvl, const std::string& msg);

	static void warning(const std::string& tag, const std::string& msg);
	static void warning(const std::string& msg);

	static void error(const std::string& msg, bool fail_fast);
	static void error(const std::string& msg);

	/**
	 * backtrace will only work under gcc
	 */
	static void printStack();

	/**
	 * Deliberately end program and print backtrace (may not work outside of gcc).
	 *  */
	static void halt();
};
}
#endif /* FORMAT_H_ */
