/*
 * Copyright (c) 2016-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "GeneralPlan.h"
#include "Plan.h"
#include "PolyPath.h"
#include "GeneralState.h"
#include "ErrorReporter.h"
#include <string>

namespace larcfm {

const GeneralPlan GeneralPlan::INVALID = GeneralPlan();


GeneralPlan::GeneralPlan() {
	plan = false;
	poly = false;
	containment = false;
}

GeneralPlan::GeneralPlan(const Plan& p) {
	fp = p;
	plan = true;
	poly = false;
	containment = false;
}

GeneralPlan::GeneralPlan(const PolyPath& p) {
	plan = false;
	poly = true;
	pp = p;
	containment = false;
}


GeneralPlan::GeneralPlan(const PolyPath& p, bool cont) {
	plan = false;
	poly = true;
	pp = p;
	containment = cont;
}

GeneralPlan::GeneralPlan(const GeneralPlan& g) {
	fp = g.fp;
	pp = g.pp;
	plan = g.plan;
	poly = g.poly;
	containment = g.containment;
	//		t = g.t;
}

GeneralState GeneralPlan::state(double time)  {
	//		if (time >= t) {
	if (plan) {
		if (time >= fp.getFirstTime() && time <= fp.getLastTime()) {
			return  GeneralState(fp.getID(), fp.position(time), fp.velocity(time), time);
		}
	} else if (poly) {
		if (time >= pp.getFirstTime() && time <= pp.getLastTime()) {
			return  GeneralState(pp.getID(), pp.getSimpleMovingPoly(time), time, containment);
		}
	}
	//		}
	return GeneralState::INVALID;
}

double GeneralPlan::getFirstTime() const {
	if (plan) {
		return fp.getFirstTime();
	} else if (poly) {
		return pp.getFirstTime();
	}
	return -1.0;
}

double GeneralPlan::getLastTime() const {
	if (plan) {
		return fp.getLastTime();
	} else if (poly) {
		return pp.getLastTime();
	}
	return -1.0;
}

int GeneralPlan::size() const {
	if (plan) {
		return fp.size();
	} else if (poly) {
		return pp.size();
	}
	return 0;
}

std::string GeneralPlan::getID() const {
	if (plan) {
		return fp.getID();
	} else if (poly) {
		return pp.getID();
	}
	return "";
}

bool GeneralPlan::isLatLon() const {
	if (plan) {
		return fp.isLatLon();
	} else if (poly) {
		return pp.isLatLon();
	}
	return false;
}

bool GeneralPlan::hasPlan() const {
	return plan;
}

bool GeneralPlan::hasPolyPath() const {
	return poly;
}

Plan GeneralPlan::getPlan() const {
	return fp;
}

PolyPath GeneralPlan::getPolyPath() const {
	return pp;
}

bool GeneralPlan::isContainment() const {
	return poly && containment;
}

bool GeneralPlan::isInvalid() const {
	return !poly && !plan;
}

GeneralState GeneralPlan::point(int i)  {
	if (i >= 0) {
		if (plan) {
			if (i < fp.size()) {
				double time = fp.time(i);
				return  GeneralState(fp.getID(), fp.point(i).position(), fp.initialVelocity(i), time);
			}
		} else if (poly) {
			if (i < pp.size()) {
				double time = pp.getTime(i);
				return  GeneralState(pp.getID(), pp.getSimpleMovingPoly(i), time, containment);
			}
		}
	}
	return GeneralState::INVALID;
}

int GeneralPlan::getSegment(double t) const {
	if (plan) {
		return fp.getSegment(t);
	} else if (poly) {
		return pp.getSegment(t);
	}
	return -1;
}

bool GeneralPlan::validate() const {
	if (plan) {
		return fp.isWeakFlyable(true);     // **RWB** use weak Consistent rather than is consistent
	} else if (poly) {
		return pp.validate();
	}
	return false;
}

std::string GeneralPlan::toString()  {
	if (plan) {
		return fp.toString();
	} else if (poly) {
		return pp.toString();
	}
	return "INVALID";
}

//std::string GeneralPlan::toOutput(int precision)  {
//	if (plan) {
//		return fp.toOutput();
//	} else if (poly) {
//		return pp.toOutput();
//	} else {
//		return "";
//	}
//}
//
//

}
