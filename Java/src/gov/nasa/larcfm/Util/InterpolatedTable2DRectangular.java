/*
 * Copyright (c) 2016-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * This class implements a rectangular two-dimensional linear interpolation table. 
 * As a rectangular table, each dimension must have a uniform size throughout the 
 * table.  Linear extrapolation is performed if allowed by the extrapolation flag 
 * for the corresponding dimension is set in the constructor invocation.<p>
 */
public class InterpolatedTable2DRectangular implements InterpolatedTable {

	private BreakpointSchedule dim1;
	private BreakpointSchedule dim2;
	private boolean extrap1;
	private boolean extrap2;
	private double[][] data;


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
	 * @param data  The two-dimensional array of dependent-variable data.
	 *
	 * @exception Exception  Thrown if a dimension of the data array does 
	 *               not match the corresponding breakpoint schedule.
	 */
	public InterpolatedTable2DRectangular(BreakpointSchedule schedule1, 
			boolean extrapolate1, 
			BreakpointSchedule schedule2, 
			boolean extrapolate2, 
			double[][] data) throws Exception {
		if (schedule1.highestIndex != data.length - 2) {
			throw new Exception("First dimension does not match breakpoint schedule.");
		}

		for (int i = 0; i < data.length; i++) {
			if (schedule2.highestIndex != data[i].length - 2) {
				throw new Exception("Second dimension does not match breakpoint schedule.");
			}
		}

		this.dim1 = schedule1;
		this.dim2 = schedule2;
		this.extrap1 = extrapolate1;
		this.extrap2 = extrapolate2;
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
		double w1 = extrap1 ? dim1.weightExtrap : dim1.weightHold;
		double w2 = extrap2 ? dim2.weightExtrap : dim2.weightHold;

		double d00 = data[i1]    [i2];
		double d01 = data[i1]    [i2 + 1];
		double d10 = data[i1 + 1][i2];
		double d11 = data[i1 + 1][i2 + 1];

		double d0 = d00 + w2 * (d01 - d00);
		double d1 = d10 + w2 * (d11 - d10);
		return d0 + w1 * (d1 - d0);

	}

}
