/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * This class is an implementation of a {@link BreakpointSchedule} where the 
 * breakpoint values are arbitrarily spaced.  The breakpoint values must be 
 * monotonically increasing, and there must be at least two breakpoints.<p>
 */
public class GeneralBreakpointSchedule extends BreakpointSchedule {

	private final double[] breakpoints;
	private final double lowestBreakpoint;
	private final double highestBreakpoint;
	private final int sz;

	private double previousValue;


	/*
	 *  constructor
	 */

	/**
	 * Construct a GeneralBreakpointSchedule, using the given breakpoint array.
	 * The values in the breakpoint array must be monotonically increasing. 
	 *
	 * @param breakpoints The breakpoint array.
	 *
	 * @exception Exception Thrown if any of the constraints mentioned 
	 *               above are violated.
	 */
	public GeneralBreakpointSchedule(double[] breakpoints) throws Exception {

		if (breakpoints == null || breakpoints.length < 2) {
			throw new Exception("There must be at least two breakpoints.");
		}
		for (int i = 1; i < breakpoints.length; i ++) {
			if (breakpoints[i] <= breakpoints[i - 1]) {
				throw new Exception("Breakpoints must be monotonically increasing.");
			}
		}

		this.breakpoints = breakpoints;
		highestIndex = breakpoints.length - 2;
		lowestBreakpoint  = breakpoints[0];
		highestBreakpoint = breakpoints[highestIndex + 1];
		
		previousValue = lowestBreakpoint;		// pre-initialize at lowest breakpoint
		lowIndex = 0;
		weightHold = 0.0;
		weightExtrap = 0.0;
		
		sz = breakpoints.length;

	}

    @Override
    public int size() {
    	return sz;
    }
    

	/*
	 *  lookup method
	 */


	/** Set the value of the independent variable associated with this 
	 *  BreakpointSchedule.  This method performs the required breakpoint lookup 
	 *  and sets the corresponding values for lowIndex and weight 
	 *  for use by table interpolation methods.
	 *
	 * @param value the value of the independent variable for this BreakpontSchedule.
	 */
	public void setIndependentValue(double value) {

		// handle special cases

		if (value == previousValue) {			// same as last lookup; do nothing
			return;
		}

		if (value <= lowestBreakpoint) {		// at or below lower bound
			lowIndex = 0;
			weightHold = 0;
			weightExtrap = (value - lowestBreakpoint) / (breakpoints[1] - lowestBreakpoint);
			previousValue = value;
			return;
		}

		if (value >= highestBreakpoint) {		// at or above upper bound
			lowIndex = highestIndex;
			weightHold = 1;
			weightExtrap = (value - breakpoints[lowIndex])
					/ (highestBreakpoint - breakpoints[lowIndex]);
			previousValue = value;
			return;
		}

		// search for correct lowIndex

		if (value < previousValue) {			// search lower
			while (value < breakpoints[lowIndex]) {
				lowIndex --;
			}
		} else {						// search higher
			while (value >= breakpoints[lowIndex + 1]) {
				lowIndex ++;
			}
		}

		// compute weights and finish up

		weightExtrap = (value - breakpoints[lowIndex])
				/ (breakpoints[lowIndex + 1] - breakpoints[lowIndex]);
		weightHold = weightExtrap;
		previousValue = value;

	}

	@Override
	public double getLimitLow() {
		return lowestBreakpoint;
	}

	@Override
	public double getLimitHigh() {
		return highestBreakpoint;
	}

}
