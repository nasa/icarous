/* 
 * Generic Bands (an interface)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Interval;

/**
 * This provides an interface to classes for various conflict
 * prevention bands. The methods for adding ownship and traffic
 * information are described in subclass interfaces GenericStateBands 
 * and GenericIntentBands.  The input to these objects is the position and
 * velocity of the ownship, the output is ranges of guidance
 * maneuvers: track angles, ground speeds, and vertical speeds. <p>
 */
public interface GenericBands {   

  /** Returns whether the aircraft positions are specified in
   * latitude/longitude or Euclidean coordinates.  If the ownship
   * has not been set, then this value is undefined.
   */
  public boolean isLatLon();

  /** Sets the (relative) lookahead time [s].  Any existing
   * bands information is cleared. 
   * If there are more than one lookahead times, this sets the nearest time. */
  public void setLookaheadTime(double d, String unit);
  
  /** Returns the lookahead time [s] 
   * If there are more than one lookahead times, this gets the nearest time. */
  public double getLookahedTime(String unit);

  /** This method sets the range of ground speeds that are produced
   * by bands.  The range is 0..maxGroundSpeed in knots.  When
   * this method is executed any existing bands information is
   * cleared. */
  public void setMaxGroundSpeed(double gs, String unit);
  /** Returns the maximum ground speed in knots */
  public double getMaxGroundSpeed(String unit);

  /** Sets the range of vertical speeds that are produced by bands.
   * -max_vs to max_vs in feet/min. When this method is executed,
   * any existing bands information is cleared. */
  public void setMaxVerticalSpeed(double vs, String unit);
  /** Returns the range of vertical speeds output by bands in feet/min. */
  public double getMaxVerticalSpeed(String unit);

  /** Clear all band information. */
  public void clear();

  /** Return the number of bands of track angles */
  public int trackLength();
  /** Return the i-th interval of the track band in the given unit.  */
  public Interval track(int i, String unit);
  /** Return the region type of the i-th track band.  If a single
   * time version of bands is used, then [NEAR|NONE] is returned.
   * If a version of bands with two times is used, then
   * [NEAR|MID|NONE] is returned. */
  public BandsRegion trackRegion(int i);
  /** Return the region type of the given track angle [degrees,
   * clockwise from true north]. Angles outside of 0..360 return
   * NEAR. If a single time version of bands is used, then
   * [NEAR|NONE] is returned.  If a version of bands with two times
   * is used, then [NEAR|MID|NONE] is returned. */
  public BandsRegion regionOfTrack(double trk, String unit);


  /** Return the number of bands of ground speeds */
  public int groundSpeedLength();
  /** Return i-th interval of the ground speed band in the given unit.  */
  public Interval groundSpeed(int i, String unit);
  /** Return the region type of the i-th ground speed band. If a
   * single time version of bands is used, then [NEAR|NONE] is
   * returned.  If a version of bands with two times is used, then
   * [NEAR|MID|NONE] is returned. */
  public BandsRegion groundSpeedRegion(int i);
  /** Return the region type of the given ground speed [knot], out
   * of range speeds are returned as NEAR. If a single time version
   * of bands is used, then [NEAR|NONE] is returned.  If a version
   * of bands with two times is used, then [NEAR|MID|NONE] is
   * returned. */
  public BandsRegion regionOfGroundSpeed(double gs, String unit);


  /** Return the number of bands of vertical speeds */
  public int verticalSpeedLength();
  /** Return the i-th interval of the vertical speed band in the given unit.  */
  public Interval verticalSpeed(int i, String unit);
  /** Return the region type of the i-th vertical speed band. If a
   * single time version of bands is used, then [NEAR|NONE] is
   * returned.  If a version of bands with two times is used, then
   * [NEAR|MID|NONE] is returned. */
  public BandsRegion verticalSpeedRegion(int i);
  /** Return the region type of the given vertical speed [feet/min],
   * out of range speeds are returned as NEAR.  If a single time
   * version of bands is used, then [NEAR|NONE] is returned.  If a
   * version of bands with two times is used, then [NEAR|MID|NONE]
   * is returned. */
  public BandsRegion regionOfVerticalSpeed(double vs, String unit);

}
