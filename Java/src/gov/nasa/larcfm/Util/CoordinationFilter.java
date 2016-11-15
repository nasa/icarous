/* Filter
 * 
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * 
 * manages a history of aircraft state information
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */
package gov.nasa.larcfm.Util;

/**
 * This class implements a simple filter to an input in {-1,1} that guarantees a stable output for at 
 * least "n" iterations. This number of iterations is called "tolerance". The tolerance is also the maximum 
 * delay for any given input. The filter has the property of being symmetric, i.e., it produces opposite outputs 
 * to opposite inputs. All filters for a given signal should have the same tolerance and they should be reset at the same time.
 * 
 * This filter was designed by Mahyar R. Malekpour (NASA Langley) and implemented by 
 * Cesar Munoz (NASA Langley)
 * 
 */

public class CoordinationFilter {

  private final int Tolerance;
  private int intern;

  /**
   * Creates a filter for a given tolerance.
   * @param tolerance Tolerance in number of iterations.
   */
  public CoordinationFilter(int tolerance) {
    Tolerance = tolerance;
    intern = 0;
  }

  /**
   * Reset filter. All filters for the same signal need to be reset at the same time.
   */
  public void reset() {
    intern = 0;
  }
  
  /**
   * Return a (deep) copy of this filter.  This copy will have the same state as the original.
   */
  public CoordinationFilter copy() {
	  CoordinationFilter rtn = new CoordinationFilter(Tolerance);
	  rtn.intern = intern;
	  return rtn;
  }
  
  /**
   * Filter a given input.
   * @param input Input in {-1,1}
   */
  public int filter(int input) {
    if (intern == 0) {
      intern = input*Tolerance;
      return input;
    }   
    if (input*intern < Tolerance) {
      intern += input;
      if (intern == 0) {
        intern = input*Tolerance;
        return input;
      } 
      return Util.sign(intern);
    }
    return input;
  }
}
