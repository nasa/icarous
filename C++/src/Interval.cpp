/* 
 * Interval
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Interval.h"
#include "Util.h"
#include "format.h"
#include "Constants.h"
#include <cmath>
#include <sstream>

using namespace larcfm;

const Interval Interval::EMPTY(0.0,-1.0);

Interval::Interval(double lb, double ub){ 
	low = lb;
	up =  ub;
}

Interval::Interval(){ 
	low = 0.0;
	up = -1.0;
}

Interval::Interval(const Interval& r) {
	low = r.low;
	up = r.up;
}

int Interval::operator==(const Interval& p) const {
	return low == p.low && up == p.up;
}

std::string Interval::toString() const {
	return toString(Constants::get_output_precision());
}

std::string Interval::toString(int precision) const {
	if (isEmpty()) {
		return "[]";
	} else {
		return "["+FmPrecision(low,precision)+", "+FmPrecision(up,precision)+"]";
	}
}

std::string Interval::toStringUnits(const std::string& unit) const {
	if (isEmpty()) {
		return "[]";
	} else {
		return "["+Units::str(unit,low)+", "+Units::str(unit,up)+"]";
	}
}

bool Interval::isEmpty() const {
	return low > up;
}

/* Is this interval a single value? */
bool Interval::isSingle() const {
	return low == up;
}

bool Interval::isSingle(double width) const {
	return low+width >= up;
}

/* Is the element in this closed/closed interval? */
bool Interval::in(double x) const {
	return low <= x && x <= up;
}

/* Is the element in this closed/closed interval? */
bool Interval::inCC(double x) const {
	return low <= x && x <= up;
}

/* Is the element in this closed/open interval? */
bool Interval::inCO(double x) const {
	return low <= x && x < up;
}

/* Is the element in this open/closed interval? */
bool Interval::inOC(double x) const {
	return low < x && x <= up;
}

/* Is the element in this open/open interval? */
bool Interval::inOO(double x) const {
	return low < x && x < up;
}

/** Is the element in this interval, where close/open conditions are given as parameters */
bool Interval::in(double x, bool lb_close, bool ub_close) {
	if (lb_close && ub_close) {
		return low <= x && x <= up;
	} else if (ub_close) {
		return low < x && x <= up;
	} else if (lb_close) {
		return low <= x && x < up;
	} else {
		return low < x && x < up;
	}
}

bool Interval::overlap(const Interval& r) const {
	if (isEmpty()) return false;
	if (r.isEmpty()) return false;

	if ( low <= r.low && r.up <= up ) return true;
	if ( r.low <= low && low <  r.up ) return true;
	if ( r.low <= low && up <= r.up ) return true;
	if ( r.low <  up && up <= r.up ) return true;
	return false;
}

/* Returns a new interval which is the intersection of the current
 * Interval and the given Interval.  If the two regions do not
 * overlap, then an empty region is returned. 
 */
Interval Interval::intersect(const Interval& r) const {
	if ( r == *this ) {
		return *this;
	}

	if ( ! overlap(r)) {
		return Interval();   // empty region
	}

	return Interval(std::max(low, r.low), std::min(up, r.up));
}

std::string Interval::toPVS(int precision) const {
	return "(# lb:= "+FmPrecision(low, precision)+", ub:= "+FmPrecision(up, precision)+" #)";
}






