/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * This class implements a one-dimensional linear interpolation table.  Linear 
 * extrapolation is performed if allowed by the extrapolation flag set in the 
 * constructor invocation.<p>
 */
public class InterpolatedTable1D implements InterpolatedTable {

    private BreakpointSchedule dim1;
    private boolean extrap1;
    private double[] data;


    /*
     *  constructor
     */

    /**
     * Construct a table using the supplied breakpoint schedule and data array. 
     * Linear extrapolation is performed if the corresponding flag is true, 
     * otherwise the value of the independent variable is considered to be 
     * limited to the range of the corresponding breakpoint schedule.<p>
     *
     * @param schedule  The breakpoint schedule for this table.
     * @param extrapolate  Allows linear extrapolation if true.
     * @param data  The one-dimensional array of dependent-variable data.
     *
     * @exception Exception  Thrown if the size fo the data array does not 
     *               match the breakpoint schedule.
     */
    public InterpolatedTable1D(BreakpointSchedule schedule, boolean extrapolate, 
    		double[] data) throws Exception {
    	if (schedule.highestIndex != data.length - 2) {
    		throw new Exception("Breakpoint schedule does not match array size.");
    	}

    	this.dim1 = schedule;
    	this.extrap1 = extrapolate;
    	this.data = data;
    }


    /*
     *  lookup method
     */


    /** Returns the value of the dependent variable for this table, based on the 
     *  values loaded into the corresponding {@link BreakpointSchedule}.   
     *  Linear interpolation is performed.  If an independent variable is out of 
     *  range (as defined by its {@link BreakpointSchedule}), it will either limited 
     *  to the allowed range or linear extrapolation will be performed, depending 
     *  on the implementation.<p>
     *
     * @return the value of the dependent variable for this table
     */
    public double getDependentValue() {

    	int i1 = dim1.lowIndex;
    	double w1 = extrap1 ? dim1.weightExtrap : dim1.weightHold;

    	double d0 = data[i1];
    	double d1 = data[i1 + 1];

    	return d0 + w1 * (d1 - d0);

    }

}
