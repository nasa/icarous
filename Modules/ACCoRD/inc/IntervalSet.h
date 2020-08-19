/* 
 * IntervalSet
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef INTERVALSET_H_
#define INTERVALSET_H_

#include "Interval.h"
#include "ErrorReporter.h"
#include "ErrorLog.h"
#include "Util.h"
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

namespace larcfm {

/**
 * <p>The IntervalSet class represents a set of "double" values.  Ranges
 * of doubles are maintained as intervals (Interval).  These Intervals
 * are ordered consecutively from lowest to highest. Standard set
 * operations of <code>in</code> (membership), <code>union</code> (set union), 
 * <code>intersect</code> (set intersection), and <code>diff</code> (set difference) 
 * are provided.</p>
 *
 * <p>Within the IntervalSet, intervals are generally considered closed (including end-points), and the
 * results off operations are closed intervals.  This implies that the interval difference
 * is between a set of closed intervals and one or more open intervals.</p>
 * 
 * <p>The intervals are numbered 0 to size()-1.  To cycle through the
 * intervals one may:</p>
 *
 * <pre><code>
 * IntervalSet set;
 *
 * for( int i = 0; i &lt; set.size(); i++) {
 *   Interval r;
 *   r = set.getInterval(i);
 *   ... work with r ...
 * }
 * </code></pre><p>
 *
 * The current implementation does not allocate any dynamic (heap) memory.
 */
class IntervalSet {// : ErrorReporter {

public:
	/** The maximum number of intervals */
	static const int max_intervals = 400;

public:
	/** Construct an empty IntervalSet */
	IntervalSet();

	/** Copy the IntervalSet into a new set 
	 * @param l IntervalSet to copy
	 * */ 
	IntervalSet(const IntervalSet& l);

	/** Build an IntervalSet from the given vector */
	explicit IntervalSet(const std::vector<Interval>& v);

	/** Return this set as a vector */
	std::vector<Interval> toVector() const;

	/** Empty this IntervalSet */
	void clear();


	/**
	 * Return Interval i from this set.  The Intervals are numbered
	 * 0..size()-1.  If i is invalid, then an empty interval is
	 * returned.
	 *
	 * @param i the index of the desired Interval.
	 */
	const Interval& getInterval(int i) const;
	/** Return the total number of intervals 
	 * @return number of intervals
	 * */
	int size() const;

	bool isEmpty() const;

	/** Is the given value a member of this set? 
	 * 
	 * @param x value
	 * @return true, if in set
	 * */
	bool in(double x) const;


	/**
	 * Merge the given interval into this set.  If this interval
	 * overlaps any interval in the set, then the intervals are
	 * merged.
	 */
	void unions(const Interval& rn);

	/**
	 * Union the given IntervalSet into the current
	 * IntervalSet. IntervalSet n is unmodified.
	 */
	void unions(const IntervalSet& n);

	/**
	 * Union the given IntervalSet into the current IntervalSet. Set n is
	 * unmodified. This method uses "almost" inequalities to compute the intersection.
	 * 
	 * @param n set
	 */
	void almost_unions(const IntervalSet& n, INT64FM maxUlps);

	/**
	 * Add the given interval into this set. If this interval overlaps any
	 * interval in the set, then the intervals are merged. 
	 * This method uses "almost" inequalities to compute the addition.
	 * 
	 * @param l lower bound of interval
	 * @param u upper bound of interval
	 */
	void almost_add(double l, double u);

	/**
	 * Add the given interval into this set. If this interval overlaps any
	 * interval in the set, then the intervals are merged. 
	 * This method uses "almost" inequalities to compute the addition.
	 * 
	 * @param l lower bound of interval
	 * @param u upper bound of interval
	 * @param maxUlps units of least precision
	 */
	void almost_add(double l, double u, INT64FM maxUlps);

	/**
	 * Intersect the given IntervalSet into the current IntervalSet. Set n is
	 * unmodified. This method uses "almost" inequalities to compute the intersection.
	 * 
	 * @param n set
	 */
	void almost_intersect(const IntervalSet& n);

	/**
	 * Intersect the given IntervalSet into the current IntervalSet. Set n is
	 * unmodified. This method uses "almost" inequalities to compute the intersection.
	 * 
	 * @param n set
	 * @param maxUlps units of least precision
	 */
	void almost_intersect(const IntervalSet& n, INT64FM maxUlps);

	/**
	 * Remove the given open interval from the given set of closed intervals.
	 * Note: the semantics of this method mean that [1,2] - (1,2) = [1,1] and
	 * [2,2]. To get rid of the extraneous singletons use methods like
	 * removeSingle() or sweepSingle().
	 * 
	 * @param rn interval
	 */
	void diff(const Interval& rn);
	/** 
	 * Perform a set difference between these two IntervalSets.  The
	 * parameter is interpreted as a set of open intervals.
	 * 
	 * @param n set
	 */
	void diff(const IntervalSet& n);

	/**
	 * Remove the single-valued interval x from this IntervalSet.  If x is
	 * not a single-valued interval (of width or less), then this method does nothing.
	 * 
	 * @param x value
	 * @param width space around value
	 */
	void removeSingle(double x, double width);

	/**
	 * Remove the single-valued interval x from this IntervalSet.  If x is
	 * not a single-valued interval, then this method does nothing.
	 * 
	 * @param x value
	 */
	void removeSingle(double x);

	/**
	 * Remove all the single-valued intervals from this IntervalSet.
	 */
	void sweepSingle();

	/**
	 * Remove all the single-valued intervals (of width or less) from this IntervalSet.
	 * 
	 * @param width space around single values
	 */
	void sweepSingle(double width);

	/**
	 * Remove all breaks of less than width from this IntervalSet
	 * 
	 * @param width max open space to remove
	 */
	void sweepBreaks(double width);

	/** Print the contents of this IntervalSet */
	std::string toString() const;


	/** Print the contents of this IntervalSet */
	std::string toString(const std::string& unit) const;

private:
	int order(double x) const;
	void insert(int i, const Interval& r);
	void remove(int i);
	void remove(int i, int len);

	static const Interval empty;
	Interval r[max_intervals];
	int length;
};

}

#endif
