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
 * 
 */

package gov.nasa.larcfm.Util;

/** Interval represents a interval of double's from a lower bound to
 * an upper bound.  This class is immutable.
 * 
 *  Whether the interval is interpreted as open or closed is context-dependent.
 *  There are various membership tests allowing for the different interpretations.
 */
public final class Interval {

  /** The lower bound of this interval */
  public final double low;
  /** The upper bound of this interval */
  public final double up;

  /** An empty interval. */
  public static final Interval EMPTY = new Interval(0.0, -1.0);

  /** Interval with bounds (low,up) */
  public Interval(double low, double up){ 
    this.low = low;
    this.up = up;
  }

  /** Construct a new Interval which is a copy of the given Interval. */
  public Interval(Interval i) { 
    this.low = i.low;
    this.up = i.up;
  }

  /** Returns interval width */
  public double width() {
    return up-low;
  }

  //  /** Equality between intervals */
  //  public boolean equals(Object p) {
  //    Interval pp = (Interval)p;
  //    return low==pp.low && up==pp.up;
  //  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    long temp;
    temp = Double.doubleToLongBits(low);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(up);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    Interval other = (Interval) obj;
    if (Double.doubleToLongBits(low) != Double.doubleToLongBits(other.low))
      return false;
    if (Double.doubleToLongBits(up) != Double.doubleToLongBits(other.up))
      return false;
    return true;
  }

  /** String representation (as a closed interval) */
  public String toString() {
    return toString(Constants.get_output_precision());
  }

  public String toString(int precision) {
    if (isEmpty()) {
      return "[]";
    } else {
      return "["+f.FmPrecision(low, precision)+", "+f.FmPrecision(up, precision)+"]";
    }
  }

  public String toStringUnits(String unit) {
    if (isEmpty()) {
      return "[]";
    } else {
      return "["+Units.str(unit,low)+", "+Units.str(unit,up)+"]";
    }
  }

  public String toPVS(int precision) {
    return "(# lb:= "+f.FmPrecision(low, precision)+", ub:= "+f.FmPrecision(up, precision)+" #)";
  }

  /** Return true if the interval is empty, or otherwise ill-formed. */
  public boolean isEmpty() {
    return low > up;
  }

  /** Is this interval a single value? */
  public boolean isSingle() {
    return low == up;
  }

  /** Is this interval a single value? (with intervals of the indicated width or smaller counting) */
  public boolean isSingle(double width) {
    return low+width >= up;
  }	

  /** Is the element in this closed/closed interval? */
  public boolean in(double x) {
    return low <= x && x <= up;
  }

  /** Is the element in this closed/closed interval? */
  public boolean inCC(double x) {
    return low <= x && x <= up;
  }

  /** Is the element in this closed/open interval? */
  public boolean inCO(double x) {
    return low <= x && x < up;
  }

  /** Is the element in this open/closed interval? */
  public boolean inOC(double x) {
    return low < x && x <= up;
  }

  /** Is the element in this open/open interval? */
  public boolean inOO(double x) {
    return low < x && x < up;
  }

  /** Is the element in this interval, where close/open conditions are given as parameters */
  public boolean in(double x, boolean lb_close, boolean ub_close) {
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
  
  /** Returns a new interval which is the intersection of the current
   * Interval and the given Interval.  If the two regions do not
   * overlap, then an empty region is returned. 
   */
  public Interval intersect(Interval r) {
    if ( equals(r) ) {
      return this;
    }

    if ( ! overlap(r)) {
      return EMPTY;
    }

    return new Interval(Math.max(low, r.low), Math.min(up, r.up));
  }

  /** 
   * Does the given Interval overlap with this Interval.  Intervals that only
   * share an endpoint do not overlap, for example, (1.0,2.0) and
   * (2.0,3.0) do not overlap.
   */
  public boolean overlap(Interval r) {
    if (isEmpty()) return false;
    if (r.isEmpty()) return false;

    if ( low <= r.low && r.up <= up ) return true;
    if ( r.low <= low && low <  r.up ) return true;
    if ( r.low <= low && up <= r.up ) return true;
    if ( r.low <  up && up <= r.up ) return true;

    return false;
  }

//  /**
//   * Compare two intervals.  If this interval comes entirely before r, return -1. if this interval comes entirely after r, return 1, otherwise return 0 (including if either interval is empty)
//   * @param r
//   * @return
//   */
//  public int compare(Interval r) {
//	    if (isEmpty()) return 0;
//	    if (r.isEmpty()) return 0;
//	    if (low < r.up && up <= r.low) return -1;
//	    if (r.low < up && r.up <= low) return +1;
//	    return 0;
//  }
  
}

