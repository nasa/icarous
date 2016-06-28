/* EWMAFilter
 * 
 * Exponential Weighted Moving Average Filter (EWMAF)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */
package gov.nasa.larcfm.Util;

/**
 * This class implements the exponential weighted moving average filter. 
 * 
 * See: http://en.wikipedia.org/wiki/EWMA#Exponential_moving_average
 * 
 */

public class EWMAFilter {

	private double  dWeight;
	private double  dPrevOutput;
	private boolean bReset;
	
	/**
	 * Creates a filter given a weight. 
	 * @param weight (>= 0)
	 */
	public EWMAFilter(double weight) {
		dWeight = Math.max(0.2,weight);
		bReset = true; 
	}

	public EWMAFilter() {
		dWeight = 0.2;
		bReset = true;
	}

	/**
	 * Filter a given input (smooth output)
	 * @param input Input 
	 */
	public double filter(double input) {
		if (bReset) {
			bReset = false;
			dPrevOutput = input;
		} else {
			dPrevOutput = dPrevOutput + dWeight * (input - dPrevOutput);
		}
		return ((double) ((int) (dPrevOutput * 100 + 0.5)) / 100);
	}
}
