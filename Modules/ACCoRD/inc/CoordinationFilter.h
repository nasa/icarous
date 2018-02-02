/* Filter
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * manages a history of aircraft state information
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 */
#ifndef COORDINATIONFILTER_H_
#define COORDINATIONFILTER_H_

namespace larcfm {
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

class CoordinationFilter {

private:
	int Tolerance;
    int intern;

public:
  /** Default constructor */
  CoordinationFilter();


  /**
   * Creates a filter for a given tolerance.
   * @param tolerance Tolerance in number of iterations.
   */
  CoordinationFilter(int tolerance);

  /**
   * Reset filter. All filters for the same signal need to be reset at the same time.
   */
  void reset();

  /**
   * Filter a given input.
   * @param input Input in {-1,1}
   */
  int filter(int input);

};

}

#endif
