/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
* This class is an implementation of a {@link BreakpointSchedule} where the 
* breakpoint values are evenly spaced.<p>
*/
public class EvenlySpacedBreakpointSchedule extends BreakpointSchedule {
    
    private final double interval;
    private final double lowestBreakpoint;
    private final double highestBreakpoint;
    private final int sz;
    
    
    /*
    *  constructor
    */
    
    /**
    * Construct an EvenlySpacedBreakpointSchedule, using the given lowest and 
    * highest breakpoint values.  Intermediate breakpoints are spaced at the 
    * specified interval, which must be positive.  The highest breakpoint 
    * must be greater than the lowest breakpoint.  The difference between 
    * the highest and lowest breakpoints must span at least one complete 
    * interval, and the difference should be an integer multiple of the 
    * interval.
    *
    * @param lowestBreakpoint The lowest breakpoint in the schedule.
    * @param highestBreakpoint The highest breakpoint in the schedule.
    * @param interval The interval between successive breakpoints.
    *
    * @exception Exception Thrown if any of the constraints mentioned 
    *               above are violated.
    */
    public EvenlySpacedBreakpointSchedule(double lowestBreakpoint, 
            double highestBreakpoint, double interval) throws Exception {
        
        if (interval <= 0.0) {
            throw new Exception("Interval must be positive.");
        }
        if (lowestBreakpoint > highestBreakpoint) {
            throw new Exception("Breakpoints out of order.");
        }
        
        this.interval = interval;
        this.lowestBreakpoint = lowestBreakpoint;
        highestIndex = (int) Math.round((highestBreakpoint - lowestBreakpoint) / interval) - 1;
        
        if (highestIndex < 0) {
            throw new Exception("Breakpoints must span at least one complete interval.");
        }
        
        sz = highestIndex + 1;
        this.highestBreakpoint = highestBreakpoint;
        
    }
    
	@Override
	public double getLimitLow() {
		return lowestBreakpoint;
	}

	@Override
	public double getLimitHigh() {
		return highestBreakpoint;
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
        
        double scale = (value - lowestBreakpoint) / interval;   // scaled index
        lowIndex = (int) Math.floor(scale);    // array index of lower breakpoint
        if (lowIndex < 0) {                    // value out of range low
            lowIndex = 0;                         // use lowest breakpoint
            weightHold = 0.0;                     // hold lowest value
            weightExtrap = scale;                 // for extrapolation
        } else if (lowIndex > highestIndex) {  // value out of range high
            lowIndex = highestIndex;              // use highest breakpoint
            weightHold = 1.0;                     // hold highest value
            weightExtrap = scale - (double) lowIndex;  // for extrapolation
        } else {                               // value in range
            weightHold = scale - (double) lowIndex;   // weight for interpolation
            weightExtrap = weightHold;                // use same weight
        }
    }


	@Override
	public String toString() {
		return "EvenlySpacedBreakpointSchedule [interval=" + interval + ", lowestBreakpoint=" + lowestBreakpoint + "]";
	}

}
