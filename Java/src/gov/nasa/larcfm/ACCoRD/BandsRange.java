/**
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;

public class BandsRange {
  public Interval    interval;
  public BandsRegion region;
  
  public BandsRange(Interval i, BandsRegion r) {
    interval = i;
    region = r;
  }
  
  public String toString() {
    String s = "";
    s+=interval.toString(4)+" "+region;
    return s;
  }
  
}

