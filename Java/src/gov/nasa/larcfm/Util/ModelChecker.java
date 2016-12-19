/*
 * This is an interface for tests for the testing simulation platform we are using (ThreadedMC).
 * 
 * This is not intended to general release.
 *
 *  Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


package gov.nasa.larcfm.Util;

/** This adds a "report" functionality to a Runnable interface */
public interface ModelChecker extends Runnable { 
	/** Reports the accumulated results as a string.  Results should be stored as static values so any instance can report the total final results. */
	public String finalReport();
	public String singleReport();
	
	// return loop start stop and step information
	public double getLoopStart();
	public double getLoopStop();
	public double getLoopStep();
}
   





