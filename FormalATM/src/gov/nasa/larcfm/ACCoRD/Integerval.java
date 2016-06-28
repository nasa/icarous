/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

public class Integerval {
  public int lb;
  public int ub;
  
  public Integerval(int lb, int ub) {
    this.lb = lb;
    this.ub = ub;
  }
  
  public String toString() {
    return "["+lb+","+ub+"]";
  }
}
