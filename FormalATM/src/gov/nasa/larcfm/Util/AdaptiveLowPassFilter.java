/* AdaptiveLowPassFilter
 * 
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */
package gov.nasa.larcfm.Util;

/**
 * This class implements a low-pass filter to an input in {-1,1} that guarantees a stable output for at 
 * least "n" iterations. This number "n" of iterations is called "tolerance". The tolerance is also the maximum 
 * delay for any given input. The filter has the property of being symmetric, i.e., it produces opposite outputs 
 * to opposite inputs. All filters for a given signal should have the same tolerance and they should be reset 
 * at the same time. Furthermore, this filter is adaptive, i.e., the level of Tolerance is dynamically adjusted to 
 * match the fluctuations in the input burst.
 *   
 * This filter was designed by Mahyar R. Malekpour (NASA Langley) 
 * 
 */

public class AdaptiveLowPassFilter {

  private final int ToleranceMin;
  private final int ToleranceMax;
  private final int ToleranceStep;
  private final int StabilityDurationMax;
  private int tolerance;
  private int duration;
  private int internval;
  private int output;

  /**
   * Creates a filter for a given tolerance.
   * @param tolerancemin  Minimum tolerance (in number of iterations)
   * @param tolerancemax  Maximum tolerance (in number of iterations)
   * @param step Tolerance step (in number of iterations)
   */
  public AdaptiveLowPassFilter(int tolerancemin, int tolerancemax, int tolerancestep, int stabilitydurationmax) {
    ToleranceMin = Math.max(2,tolerancemin);
    ToleranceMax = Math.max(2,ToleranceMin);
    ToleranceStep = Math.max(2,tolerancestep);
    StabilityDurationMax = Math.max(2,stabilitydurationmax);
    tolerance = ToleranceMin;
    output = 0;
    internval = 0;
    duration = 0;
  }

  /**
   * Reset filter. All filters for the same signal need to be reset at the same time.
   */
  public void reset() {
	  tolerance = ToleranceMin;
	  output = 0;
	  internval = 0;
	  duration = 0;
  }
  
  /**
   * Return a (deep) copy of this filter.  This copy will have the same state as the original.
   */
  public AdaptiveLowPassFilter copy() {
	  AdaptiveLowPassFilter rtn = new AdaptiveLowPassFilter(ToleranceMin, ToleranceMax,ToleranceStep,StabilityDurationMax);
	  rtn.tolerance = tolerance;
	  rtn.output = output;
	  rtn.internval = internval;
	  rtn.duration = duration;
	  return rtn;
  }
  
  /**
   * Filter a given input.
   * @param input Input in {-1,1}
   */
  public int filter(int input) {
	int nexttolerance = tolerance;
	if (internval == 0 && Util.sign(output) * input * tolerance < 0 &&
			tolerance + ToleranceStep <= ToleranceMax) 
		nexttolerance = tolerance + ToleranceStep;
	else if (duration >= StabilityDurationMax)
		nexttolerance = ToleranceMin;
	int nextinternval = internval;
	if (internval == 0) 
		nextinternval = input*tolerance;
	else if (input*internval < tolerance)
		nextinternval = internval+input;
	int nextduration = duration;
	if (internval == 0 || duration >= StabilityDurationMax)
		nextduration = 0;
	else 
		nextduration = duration+1;
	if (internval > 0 || internval == 0 && input*tolerance > 0)
		output = 1;
	else if (internval < 0 || internval == 0 && input*tolerance < 0)
		output = -1;
	tolerance = nexttolerance;
	internval = nextinternval;
	duration = nextduration;
	return output;
  }
}
