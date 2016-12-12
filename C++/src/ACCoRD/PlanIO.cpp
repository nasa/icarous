
/*
 * PlanUtil.cpp - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "PlanIO.h"
#include "Plan.h"
#include "PlanWriter.h"
//#include "UnitSymbols.h"

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;



void PlanIO::savePlan(const Plan& plan, std::string fileName) {
	if (plan.getName() == "") {
		//plan.setName("noname");    // removed because const nature of plan
	}
	PlanWriter pw;
	pw.setPrecision(12);
	pw.open(fileName);
	pw.writePlan(plan,true);
	pw.close();
	if (pw.hasMessage()) {
		fdln(pw.getMessage());
	}
}


///** Write the plans to the given file */
//void PlanIO::savePlans(List<Plan> plans, String fileName) {
//	PlanWriter pw = new PlanWriter();
//	pw.setPrecision(12);
//	pw.open(fileName);
//	int count = 0;
//	for (Plan plan: plans) {
//		if (plan.getName().equals("")) {
//			plan.setName("noname"+count);
//			count++;
//		}
//		pw.writePlan(plan,true);
//	}
//	pw.close();
//	if (pw.hasMessage()) {
//		f.dln(pw.getMessage());
//	}
//}




}
