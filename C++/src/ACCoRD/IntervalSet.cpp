/* 
 * IntervalSet
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "IntervalSet.h"
#include "Units.h"
#include "format.h"
#include "ErrorLog.h"
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace std;
using namespace larcfm;

const Interval IntervalSet::empty = Interval();

IntervalSet::IntervalSet() {//: error("IntervalSet") {
	length = 0;
}

IntervalSet::IntervalSet(const IntervalSet& l) {//: error("IntervalSet") {
	length = l.length;

	memcpy(r,l.r,sizeof(Interval)*length);
	//for (int i = 0; i < length; i++) {
	//  r[i] = Interval(l.r[i]);  // deep copy
	//}
}

IntervalSet::IntervalSet(const std::vector<Interval>& v) {//: error("IntervalSet") {
	length = v.size();
	memcpy(r,&v[0],sizeof(Interval)*length);
}

std::vector<Interval> IntervalSet::toVector() const {
	std::vector<Interval> v = std::vector<Interval>();
	v.reserve(length);
	for (int i = 0; i < length; i++) {
		v.push_back(v[i]);
	}
	return v;
}

void IntervalSet::clear() {
	length = 0;
}

const Interval& IntervalSet::getInterval(int i) const {
	if (i >= length || i < 0) {
		//    error.addError("getInterval: Index out of bounds.");
		return empty;
	}

	return r[i];
}

int IntervalSet::size() const {
	return length;
}

bool IntervalSet::isEmpty() const {
	return size()==0;
}

bool IntervalSet::in(double x) const {
	return order(x) >= 0;
}

void IntervalSet::unions(const Interval& rn) {
	if (rn.isEmpty()) {
		return;   //nothing to add
	}

	int iLow = order(rn.low);
	int iHigh = order(rn.up);

	double low, high;
	int start, end;

	if (iLow < 0) {
		low = rn.low;
		start = -(iLow+1);
	} else {
		low = r[iLow].low;
		start = iLow;
	}

	if (iHigh < 0) {
		high = rn.up;
		end = -(iHigh+1)-1;
	} else {
		high = r[iHigh].up;
		end = iHigh;
	}

	remove(start,end-start+1);  // start to end inclusive
	insert(start, Interval(low, high));
} // union

void IntervalSet::unions(const IntervalSet& n) {
	for (int i = 0; i < n.length; i++) {
		unions(n.r[i]);
	}
}

/**
 * Add the given interval into this set. If this interval overlaps any
 * interval in the set, then the intervals are merged.
 * This method uses "almost" inequalities to compute the addition.
 */
void IntervalSet::almost_add(double l, double u) {
	if (Util::almost_less(l,u)) {
		IntervalSet m = IntervalSet(*this);
		clear();
		bool go = false;
		for (int i=0; i < m.size(); ++i) {
			Interval ii = m.getInterval(i);
			if (go) {
				unions(ii);
			} else if ((Util::almost_leq(ii.low,l) && Util::almost_leq(l,ii.up)) ||
					(Util::almost_leq(l,ii.low) && Util::almost_leq(ii.low,u))) {
				l = Util::min(ii.low,l);
				u = Util::max(ii.up,u);
			} else if (Util::almost_less(u,ii.low)) {
				unions(Interval(l,u));
				unions(ii);
				go = true;
			} else {
				unions(ii);
			}
		}
		if (!go) {
			unions(Interval(l,u));
		}
	}
}

/**
 * Intersect the given IntervalSet into the current IntervalSet. Set n is
 * unmodified. This method uses "almost" inequalities to compute the intersection.
 */
void IntervalSet::almost_intersect(const IntervalSet& n) {
	IntervalSet m = IntervalSet(*this);
	clear();
	if (!m.isEmpty() && !n.isEmpty()) {
		int i=0;
		int j=0;
		while (i < m.size() && j < n.size()) {
			Interval ii = m.getInterval(i);
			Interval jj = n.getInterval(j);
			if (Util::almost_leq(jj.low,ii.low) &&
					Util::almost_less(ii.low,jj.up)) {
				if (Util::almost_leq(ii.up,jj.up)) {
					unions(ii);
					++i;
				} else {
					unions(Interval(ii.low,jj.up));
					++j;
				}
			} else if (Util::almost_leq(ii.low,jj.low) &&
					Util::almost_less(jj.low,ii.up)) {
				if (Util::almost_leq(jj.up,ii.up)) {
					unions(jj);
					++j;
				} else {
					unions(Interval(jj.low,ii.up));
					++i;
				}
			} else if (Util::almost_leq(ii.up,jj.low)){
				++i;
			} else if (Util::almost_leq(jj.up,ii.low)){
				++j;
			}
		}
	}
}

void IntervalSet::diff(const Interval& rn) {
	if (rn.isEmpty()) {
		return;   //nothing for set difference
	}
	if (rn.isSingle()) {
		return;   // If rn is a single-valued open interval
		// then there is nothing to remove
	}

	int iLow = order(rn.low);
	int iHigh = order(rn.up);

	if (iLow >= 0 && iLow == iHigh) {
		double r_iHigh_up = r[iHigh].up;
		r[iLow] = Interval(r[iLow].low, rn.low);
		insert(iLow+1, Interval(rn.up, r_iHigh_up));

		return;
	}

	int start, end;

	if (iLow < 0) {
		start = -(iLow+1);
	} else {
		r[iLow] = Interval(r[iLow].low, rn.low);
		start = iLow+1;
	}

	if (iHigh < 0) {
		end = -(iHigh+1)-1;
	} else {
		r[iHigh] = Interval(rn.up, r[iHigh].up);
		end = iHigh-1;
	}

	remove(start,end-start+1);  // start to end inclusive
} // diff

void IntervalSet::diff(const IntervalSet& n) {
	for (int i = 0; i < n.length; i++) {
		diff(n.r[i]);
	}
}

void IntervalSet::removeSingle(double x, double width) {
	int i = order(x);
	if (i >= 0 && r[i].isSingle(width)) {
		remove(i);
	}
}

void IntervalSet::removeSingle(double x) {
	removeSingle(x, 0.0);
}

void IntervalSet::sweepSingle(double width) {
	int i = 0;
	while (i < length) {
		if (r[i].isSingle(width)) {
			remove(i);
		} else {
			i++;
		}
	}
}

void IntervalSet::sweepSingle() {
	sweepSingle(0.0);
}

void IntervalSet::sweepBreaks(double width) {
	int i = 0;
	while (i < length-1) {
		if (r[i].up+width > r[i+1].low) {
			unions(Interval(r[i].low, r[i+1].up));
		} else {
			i++;
		}
	}
}


/* 
 * Insert the given interval at point i.  If the point i is greater than
 * the number of Intervals in the list, then add this interval to the
 * end.  
 */
void IntervalSet::insert(int i, const Interval& region) {
	if (region.isEmpty()) {
		return;
	}

	if (i < 0) {
		i = 0;
	}

	if (i > length) {
		i = length;
	}

	if (i == length && length < max_intervals) {
		r[length] = region;
		length++;
	} else {
		length++;
		int c = length;

		if (length >= max_intervals) {
			std::cout << "ERROR: IntervalSet is full, fixing this requires a recompile" << endl;
			exit(1);
		}

		while (i < c) {
			r[c] = r[c-1];
			c--;
		}

		r[i] = region;
	}
} // insert

/* 
 * Remove Interval i and return it
 */
void IntervalSet::remove(int i) {
	if (i < 0 || i >= length ) {
		return;
		//return Interval();
	}

	//	Interval t = r[i];

	while (i + 1 < length) {
		r[i] = r[i+1];
		i++;
	}

	length--;

	//return t;
}

/* 
 * Remove the len number of intervals starting at i.
 */
void IntervalSet::remove(int i, int len) {
	for( int j = 0; j < len; j++) {
		remove(i);
	}
}

/* 
 * Find the point where x fits into the list.  If x is a
 * member, then the returned value is the index of the interval.
 * If x is not a member, then -index-1 is returned, where index is
 * the index of the interval after x.
 *
 */
int IntervalSet::order(double x) const {
	for (int i = 0; i < length; i++) {
		if (r[i].in(x)) {
			return i;
		}
		if (x < r[i].low) {
			return -i-1;
		}

	}

	return -length-1;
}

/** Print the contents of this IntervalSet */
std::string  IntervalSet::toString() const {
	std::string s = "";
	for (int i = 0; i < length; i++) {
		s += "Interval ["+Fm0(i)+"]: ";
		s += r[i].toString();
		s += "\n";
	}
	return s;
}






