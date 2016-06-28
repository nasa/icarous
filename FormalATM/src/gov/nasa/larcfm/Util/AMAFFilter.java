/* AMAFFilter
 * 
 * Averaging Median-Average Feedback Filter (AMAFF)
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */
package gov.nasa.larcfm.Util;

import java.util.Arrays ;

/**
 * This class implements an averaging median-average feedback filter. This filter produces an output 
 * without any delays, i.e., the first non-zero input results in a non-zero output.
 * 
 * This filter was designed by Mahyar R. Malekpour (NASA Langley).
 * 
 */

public class AMAFFilter {

	private int    iWindowSize;
	private int    iMidPoint;
	private double [] dInputFIFO;
	private double [] dInputFIFOSorted;

	/**
	 * Creates a filter for a given window size. 
	 * @param windowsize Window size (>= 0)
	 */
	public AMAFFilter(int windowsize) {
		iWindowSize = 2*Math.max(0,windowsize)+3;
		iMidPoint = (iWindowSize-1) / 2 ;
		dInputFIFO = new double [iWindowSize] ;
		dInputFIFOSorted = new double [iWindowSize] ;
		for (int i=0; i < iWindowSize; i++) {
			dInputFIFO[i] = 0;
			dInputFIFOSorted[i] = 0;
		}	 
	}
	
	/**
	 * Filter a given input.
	 * @param input Input 
	 */
	public double core_filter(double input) {		
		for (int j = 0 ; j < iWindowSize-1 ; j ++)
			dInputFIFO [j] = dInputFIFO [j + 1] ;

		// Append the new value at the right of the array.
		dInputFIFO [iWindowSize-1] = input ;

		// Sort the FIFO and select the median value.
		dInputFIFOSorted = dInputFIFO.clone() ; // First make a copy.
		Arrays.sort (dInputFIFOSorted) ;        // Sorts low to high.

		// Determine the average, excluding the current point, i.e., the element at MidPoint.
		double dSum = 0 ;
		for (int j = 0 ; j < iWindowSize ; j ++)
			dSum = dSum + dInputFIFO [j] ;
		double dAverage = (dSum - dInputFIFO [iMidPoint]) / (iWindowSize-1) ;

		// Replace and remember the mid point value.
		dInputFIFO [iMidPoint] = (dInputFIFOSorted [iMidPoint] + dAverage) / 2 ;

		return ((double) ((int) (dInputFIFO [iMidPoint] * 100 + 0.5)) / 100) ; // for double output
	}

	/**
	 * Filter a given input (smooth output)
	 * @param input Input 
	 */
	public double filter(double input) {
		return core_filter(input); 
	}
	
}
