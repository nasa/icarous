/* 
 * Interval
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef INTERVAL_H_
#define INTERVAL_H_

#include <string>
#include <vector>
#include "Util.h"

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

	/** Construct a new Interval which is a copy of the given Interval.
	 * @param i interval*/
	Interval(const Interval& i);

	/** Returns true if the bounds of the intervals are equal. */
	int operator==(const Interval& p) const;

	/** Return true if the interval is empty, or otherwise ill-formed.
	 * @return true if empty
	 * */
	bool isEmpty() const;

	/** String representation (as a closed interval) */
	std::string toString() const;

	std::string toString(int precision) const;

	std::string toStringUnits(const std::string& unit) const;

	/** Is this interval a single value?
	 * @return true if single value
	 *  */
	bool isSingle() const;
	/** Is this interval a single value? (with intervals of the indicated width or smaller counting)
	 * @param width width of interval
	 * @return true if single
	 * */
	bool isSingle(double width) const;

	/** Is the element in this closed/closed interval?
	 * @param x value
	 * @return true if value in interval
	 * */
	bool in(double x) const;
	/** Is the element in this closed/closed interval?
	 * @param x value
	 * @return true if value in interval
	 * */
	bool inCC(double x) const;
	/** Is the element in this closed/open interval?
	 * @param x value
	 * @return true if value in interval
	 * */
	bool inCO(double x) const;
	/** Is the element in this open/closed interval?
	 * @param x value
	 * @return true if value in interval
	 * */
	bool inOC(double x) const;
	/** Is the element in this open/open interval?
	 * @param x value
	 * @return true if value in interval
	 * */
	bool inOO(double x) const;
	/** Is the element (almost) in this interval, where close/open conditions are given as parameters
	 * @param x   value
	 * @param lb_close lower bound
	 * @param ub_close  upper bound
	 * @return true if in interval
	 * */
	bool almost_in(double x, bool lb_close, bool ub_close) const;
	/** Is the element (almost) in this interval, where close/open conditions are given as parameters
	 * @param x value
	 * @param lb_close lower bound
	 * @param ub_close upper bound
	 * @param maxUlps tolerance
	 * @return true if value in interval
	 * */
	bool almost_in(double x, bool lb_close, bool ub_close, INT64FM maxUlps) const;

	/**
	 * Does the given Interval overlap with this Interval.  Intervals that only
	 * share an endpoint do not overlap, for example, (1.0,2.0) and
	 * (2.0,3.0) do not overlap.
	 * @param r interval
	 * @return true if overlap
	 */
	bool overlap(const Interval& r) const;

  /** Returns a new interval which is the intersection of the current
   * Interval and the given Interval.  If the two regions do not
   * overlap, then an empty region is returned. 
   * @param r interval
   * @return intersection or empty
   */
	Interval intersect(const Interval& r) const;

	std::string toPVS() const;
	std::string toPVS(int precision) const;

};

}

#endif
