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

#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <string>
#include <vector>

namespace larcfm {

/** Interval represents a interval of double's from a lower bound to
 * an upper bound.  This class is immutable.
 * 
 *  Whether the interval is interpreted as open or closed is context-dependent.
 *  There are various membership tests allowing for the different interpretations.
 */
class Interval { 

public:

	// This numeric type is used for index variables over vectors of Interval
	typedef std::vector<Interval>::size_type nat;

	/** The lower bound of this interval */
	double low;
	/** The upper bound of this interval */
	double up;

	/** An empty interval. */
	static const Interval EMPTY;

	/** Construct an Interval
	 *
	 * @param lb lower bound of the region
	 * @param ub upper bound of the region
	 */
	Interval(double lb, double ub);
	/** Construct an empty interval */
	Interval();
	/** Construct a new Interval which is a copy of the given Interval. */
	Interval(const Interval& i);

	/** Returns true if the bounds of the intervals are equal. */
	int operator==(const Interval& p) const;

	/** Return true if the interval is empty, or otherwise ill-formed. */
	bool isEmpty() const;

	/** String representation (as a closed interval) */
	std::string toString() const;

	std::string toString(int precision) const;

	std::string toStringUnits(const std::string& unit) const;

	/** Is this interval a single value? */
	bool isSingle() const;
	/** Is this interval a single value? (with intervals of the indicated width or smaller counting) */
	bool isSingle(double width) const;

	/** Is the element in this closed/closed interval? */
	bool in(double x) const;
	/** Is the element in this closed/closed interval? */
	bool inCC(double x) const;
	/** Is the element in this closed/open interval? */
	bool inCO(double x) const;
	/** Is the element in this open/closed interval? */
	bool inOC(double x) const;
	/** Is the element in this open/open interval? */
	bool inOO(double x) const;
	/** Is the element in this interval, where close/open conditions are given as parameters */
	bool in(double x, bool lb_close, bool ub_close);

	/**
	 * Does the given Interval overlap with this Interval.  Intervals that only
	 * share an endpoint do not overlap, for example, (1.0,2.0) and
	 * (2.0,3.0) do not overlap.
	 */
	bool overlap(const Interval& r) const;

	/** Returns a new interval which is the intersection of the current
	 * Interval and the given Interval.  If the two regions do not
	 * overlap, then an empty region is returned.
	 */
	Interval intersect(const Interval& r) const;

	std::string toPVS(int precision) const;

};

}

#endif
