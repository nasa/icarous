/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef DEBUGSUPPORT_H_
#define DEBUGSUPPORT_H_

#include <string>
#include "Plan.h"

namespace larcfm {

class DebugSupport{
public:


	/**
	 * Returns the base (no path) file name.
	 * a/b/c.txt = c.txt
	 * a.txt = a.txt
	 * a/b/ = ""
	 * @param filename  filename string
	 */
	static std::string getNameNoPath(std::string filename);

	static void dumpPlan(const Plan& plan, std::string str) ;

	//Position p1(LatLonAlt::make(45.01, -93.31, 10000.0));
	//NavPoint np1(p1,80);

	static void dumpAsUnitTest(const Plan& plan) ;


	static void dumpAsNavPointAsserts(const Plan& plan) ;


	static void halt();

};
}
#endif /* FORMAT_H_ */
