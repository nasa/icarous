/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * This class implements a rectangular four-dimensional linear interpolation table. 
 * As a rectangular table, each dimension must have a uniform size throughout the 
 * table.  Linear extrapolation is performed if allowed by the extrapolation flag 
 * for the corresponding dimension is set in the constructor invocation.<p>
 */
public class InterpolatedTable4DRectangular implements InterpolatedTable {

    private BreakpointSchedule dim1;
    private BreakpointSchedule dim2;
    private BreakpointSchedule dim3;
    private BreakpointSchedule dim4;
    private boolean extrap1;
    private boolean extrap2;
    private boolean extrap3;
    private boolean extrap4;
    private double[][][][] data;


    /*
     *  constructor
     */

    /**
     * Construct a table using the supplied breakpoint schedules and data array. 
     * Linear extrapolation is performed if the corresponding flag is true, 
     * otherwise the value of the independent variable is considered to be 
     * limited to the range of the corresponding breakpoint schedule.<p>
     *
     * @param schedule1  The breakpoint schedule for the first dimension of this 
     *          table.
     * @param extrapolate1  If true, linear extrapolation is performed on the first 
     *          dimension.
     * @param schedule2  The breakpoint schedule for the second dimension of this 
     *          table.
     * @param extrapolate2  If true, linear extrapolation is performed on the 
     *          second dimension.
     * @param schedule3  The breakpoint schedule for the third dimension of this 
     *          table.
     * @param extrapolate3  If true, linear extrapolation is performed on the 
     *          third dimension.
     * @param schedule4  The breakpoint schedule for the fourth dimension of this 
     *          table.
     * @param extrapolate4  If true, linear extrapolation is performed on the 
     *          fourth dimension.
     * @param data  The three-dimensional array of dependent-variable data.
     *
     * @exception Exception  Thrown if a dimension of the data array does 
     *               not match the corresponding breakpoint schedule.
     */
    public InterpolatedTable4DRectangular(BreakpointSchedule schedule1, 
    		boolean extrapolate1, 
    		BreakpointSchedule schedule2, 
    		boolean extrapolate2, 
    		BreakpointSchedule schedule3, 
    		boolean extrapolate3, 
    		BreakpointSchedule schedule4, 
    		boolean extrapolate4, 
    		double[][][][] data) throws Exception {

    	if (schedule1.highestIndex != data.length - 2) {
    		throw new Exception("First dimension does not match breakpoint schedule.");
    	}
    	for (int i = 0; i < data.length; i++) {
    		if (schedule2.highestIndex != data[i].length - 2) {
    			throw new Exception("Second dimension does not match breakpoint schedule.");
    		}
    		for (int j = 0; j < data[i].length; j++) {
    			if (schedule3.highestIndex != data[i][j].length - 2) {
    				throw new Exception("Third dimension does not match breakpoint schedule.");
    			}
    			for (int k = 0; k < data[i][j].length; k++) {
    				if (schedule4.highestIndex != data[i][j][k].length - 2) {
    					throw new Exception("Fourth dimension does not match breakpoint schedule.");
    				}
    			}
    		}
    	}

    	this.dim1 = schedule1;
    	this.dim2 = schedule2;
    	this.dim3 = schedule3;
    	this.dim4 = schedule4;
    	this.extrap1 = extrapolate1;
    	this.extrap2 = extrapolate2;
    	this.extrap3 = extrapolate3;
    	this.extrap4 = extrapolate4;
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
     *  on the value of the corresponding extrapolation flag.<p>
     *
     * @return the value of the dependent variable for this table
     */
    public double getDependentValue() {

    	int i1 = dim1.lowIndex;
    	int i2 = dim2.lowIndex;
    	int i3 = dim3.lowIndex;
    	int i4 = dim4.lowIndex;
    	double w1 = extrap1 ? dim1.weightExtrap : dim1.weightHold;
    	double w2 = extrap2 ? dim2.weightExtrap : dim2.weightHold;
    	double w3 = extrap3 ? dim3.weightExtrap : dim3.weightHold;
    	double w4 = extrap4 ? dim4.weightExtrap : dim4.weightHold;

    	double d0, d1, dx0, dx1, dxx0, dxx1, dxxx0, dxxx1;
    	double[][] dt1;

    	dt1 = data[i1][i2];
    	dxxx0 = dt1[i3]    [i4]    ;
    	dxxx1 = dt1[i3]    [i4 + 1];
    	dxx0 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dxxx0 = dt1[i3 + 1][i4]    ;
    	dxxx1 = dt1[i3 + 1][i4 + 1];
    	dxx1 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dx0 = dxx0 + w3 * (dxx1 - dxx0);

    	dt1 = data[i1][i2 + 1];
    	dxxx0 = dt1[i3]    [i4]    ;
    	dxxx1 = dt1[i3]    [i4 + 1];
    	dxx0 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dxxx0 = dt1[i3 + 1][i4]    ;
    	dxxx1 = dt1[i3 + 1][i4 + 1];
    	dxx1 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dx1 = dxx0 + w3 * (dxx1 - dxx0);

    	d0 = dx0 + w2 * (dx1 - dx0);

    	dt1 = data[i1 + 1][i2];
    	dxxx0 = dt1[i3]    [i4]    ;
    	dxxx1 = dt1[i3]    [i4 + 1];
    	dxx0 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dxxx0 = dt1[i3 + 1][i4]    ;
    	dxxx1 = dt1[i3 + 1][i4 + 1];
    	dxx1 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dx0 = dxx0 + w3 * (dxx1 - dxx0);

    	dt1 = data[i1 + 1][i2 + 1];
    	dxxx0 = dt1[i3]    [i4]    ;
    	dxxx1 = dt1[i3]    [i4 + 1];
    	dxx0 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dxxx0 = dt1[i3 + 1][i4]    ;
    	dxxx1 = dt1[i3 + 1][i4 + 1];
    	dxx1 = dxxx0 + w4 * (dxxx1 - dxxx0);
    	dx1 = dxx0 + w3 * (dxx1 - dxx0);

    	d1 = dx0 + w2 * (dx1 - dx0);

    	return d0 + w1 * (d1 - d0);

    }

}
