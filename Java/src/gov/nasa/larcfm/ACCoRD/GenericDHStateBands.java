/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

public interface GenericDHStateBands extends GenericStateBands {

  /** Sets the minimum horizontal separation distance in the given units.  Any existing
   * bands information is cleared. */
  public void setDistance(double d, String unit);
  /** Returns the minimum horizontal separation distance in the given units */
  public double getDistance(String unit);

  /** Sets the minimum vertical separation distance in the given units. Any existing
   * bands information is cleared. */
  public void setHeight(double h, String unit);
  /** Returns the minimum vertical separation distance in the given units. */
  public double getHeight(String unit);

}
