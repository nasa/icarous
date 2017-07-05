/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * This class is an abstraction of a general breakpoint schedule for use with 
 * interpolated tables.  BreakpointSchedule is used to look up the index 
 * and interpolation weight for the value of the associated independent 
 * variable along one dimension of an interpolated table.  The same schedule 
 * should be used with as many tables as possible to reduce model execution 
 * time.<p>
 */
public abstract class BreakpointSchedule {


    /** This is the index of the lower breakpoint.  The value of the independent 
     *  variable lies between the breakpoint at this index and the breakpoint at 
     *  the next higher index (lowIndex+1).  If the independent variable exactly 
     *  matches a breakpoint value, other than the highest, lowIndex will correspond 
     *  to that breakpoint value (and weight will be zero).  If the independent 
     *  variable equals the highest breakpoint, lowIndex will correspond to the next 
     *  to highest breakpoint and the weight will be one. 
     */
    protected int lowIndex = 0;


    /** This is the weight for the breakpoint difference.  Values will normally 
     *  range between zero and one, where zero weight corresponds to the lower 
     *  breakpoint (i.e. at lowIndex) and a weight of one corresponds to the 
     *  higher breakpoint (i.e. at lowIndex+1).  Negative weights will be returned 
     *  when the independent variable is less than the lowest breakpoint value, 
     *  and weights greater than one will be returned when the independent variable 
     *  is greater than the highest breakpoint value.  These weights outside the 
     *  range of [0.0, 1.0] allow for extrapolation.
     */
    protected double weightExtrap = 0.0;


    /** This weight is the same as weightExtrap, except that it is limited to the 
     *  range of [0.0, 1.0] in order to prevent extrapolation.
     */
    protected double weightHold = 0.0;


    /** This is the highest value that lowIndex will reach.  In other words, it is 
     *  the index of the second highest breakpoint value, which is two less than 
     *  the size of the corresponding dimension of the dependent data array.
     */
    protected int highestIndex;
    

    /*
     *  Method to be implemented by subclasses.
     */


    /** Set the value of the independent variable associated with this 
     *  BreakpointSchedule.  This method performs the required breakpoint lookup 
     *  and sets the corresponding values for lowIndex and weight 
     *  for use by table interpolation methods.
     *
     * @param value the value of the independent variable for this BreakpontSchedule.
     */
    public abstract void setIndependentValue(double value);
    
    public abstract double getLimitLow();
    public abstract double getLimitHigh();
	public abstract int size();

	@Override
	public String toString() {
		return "BreakpointSchedule [lowIndex = " + lowIndex + ", weightExtrap = " + weightExtrap + ", weightHold = "
				+ weightHold + ", highestIndex = " + highestIndex + "]";
	}
    
    

}
