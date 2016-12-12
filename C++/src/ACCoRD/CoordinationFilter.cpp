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


#include "CoordinationFilter.h"
#include "Util.h"

namespace larcfm {

  CoordinationFilter::CoordinationFilter() {
	  Tolerance = 0;
	  intern = 0;
  }

  /**
   * Creates a filter for a given tolerance.
   * @param tolerance Tolerance in number of iterations.
   */
  CoordinationFilter::CoordinationFilter(int tolerance) {
    Tolerance = tolerance;
    intern = 0;
  }

  /**
   * Reset filter. All filters for the same signal need to be reset at the same time.
   */
  void CoordinationFilter::reset() {
    intern = 0;
  }

  /**
   * Filter a given input.
   * @param input Input in {-1,1}
   */
  int CoordinationFilter::filter(int input) {
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
      return Util::sign(intern);
    }
    return input;
  }
}
