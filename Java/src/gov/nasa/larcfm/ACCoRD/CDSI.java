/*
 * CDSI - Conflict detection between an ownship state vector and a
 * traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Plan;
import gov.nasa.larcfm.Util.LatLonAlt;

/** 
 * This class implements the algorithm for conflict detection
 * between an ownship (modeled with a state vector) and a traffic
 * aircraft trajectory (modeled with intent information).  <p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *
 * In addition, for each of these types of use, the aircraft can be
 * specified in two different coordinate systems: a Euclidean space
 * and a latitude and longitude reference frame. <p>
 */
public final class CDSI implements ErrorReporter, Detection3DAcceptor {  

  private CDSICore core;
  private ErrorLog error;
  
//  /** 
//   * Create a new conflict detection (state information for the ownship
//   * and intent information for the traffic) object.
//   *
//   * @param distance the minimum horizontal separation distance [nmi]
//   * @param height the minimum vertical separation height [ft].
//   */
//  public CDSI(double distance, double height) {
//    core = new CDSICore(Units.from("nm", distance), Units.from("ft", height));
//    error = new ErrorLog("CDSI");
//  }


  /** 
   * Create a new conflict detection (state information for the ownship
   * and intent information for the traffic) object.
   *
   * @param distance the minimum horizontal separation distance [nmi]
   * @param height the minimum vertical separation height [ft].
   */
  public CDSI(Detection3D cd) {
    core = new CDSICore(cd);
    error = new ErrorLog("CDSI");
  }
  
  public CDSI() {
    core = new CDSICore(new CDCylinder());
    error = new ErrorLog("CDSI");
  }
  
//  /**
//   * Create a new CDSI conflict detection object using specified units.
//   * 
//   * @param distance the minimum horizontal separation distance [dStr]
//   * @param height the minimum vertical separation height [hStr].
//   */
//  public static CDSI make(double distance, String dStr, double height, String hStr) {
//    return new CDSI(Units.from(dStr, distance), Units.from(hStr,height));  
// }

//  /**
//   * Create a new CDSI conflict detection object using internal units.
//   * 
//   * @param distance the minimum horizontal separation distance [m]
//   * @param height the minimum vertical separation height [m].
//   */
//  public static CDSI mk(double distance, double height) {
//     return new CDSI(distance, height);  
//  }


//  /** Returns the minimum horizontal separation distance in [m] */
//  private double getDistance() {
//    return core.getDistance();
//  }
//
//  /** Returns the minimum vertical separation distance in [m] */
//  private double getHeight() {
//    return core.getHeight();
//  }
//  
//  /** Returns the minimum horizontal separation distance in specified units */
//  public double getDistance(String units) {
//    return Units.to(units, core.getDistance());
//  }
//
//  /** Returns the minimum vertical separation distance in specified units */
//  public double getHeight(String units) {
//    return Units.to(units, core.getHeight());
//  }
  
  /**
   * Returns the conflict detection filter time.
   * 
   * @return the conflict detection filter time seconds
   */
  public double getFilterTime() {
    return core.getFilterTime();
  }

//  /** Sets the minimum horizontal separation distance in [m] */
//  private void setDistance(double distance) {
//    core.setDistance(distance);
//  }
//
//  /** Sets the minimum vertical separation distance in [m] */
//  private void setHeight(double height) {
//    core.setHeight(height);
//  }
//
//  /** Sets the minimum horizontal separation distance in specified units */
//  public void setDistance(double distance, String units) {
//    core.setDistance(Units.from("nm", distance));
//  }
//
//  /** Sets the minimum vertical separation distance in specified units */
//  public void setHeight(double height, String units) {
//    core.setHeight(Units.from("ft", height));
//  }

  
  /**
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  public void setFilterTime(double cdfilter) {
    core.setFilterTime(cdfilter);
  }
  
  /** Returns the number of conflicts */
  public int size() {
    return core.size();
  }

  /** Returns if there were any conflicts */
  public boolean conflict() {
    return core.size() > 0;
  }

  /** 
   * Returns the start time of the conflict in [s].  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeIn(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getTimeIn()");
      return 0.0;
    }
    return core.getTimeIn(i);
  }

  /** 
   * Returns the end time of the conflict in [s].  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i  the i-th conflict, must be between 0..size()-1
   */
  public double getTimeOut(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getTimeOut()");
      return 0.0;
    }
    return core.getTimeOut(i);
  }

  /** 
   * Returns the segment number of the entry into a conflict from the traffic aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public int getSegmentIn(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getSegmentIn()");
      return 0;
    }
    return core.getSegmentIn(i);
  }

  /** 
   * Returns the segment number of the exit from a conflict from the traffic aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public int getSegmentOut(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getSegmentOut()");
      return 0;
    }
    return core.getSegmentOut(i);
  }

  /** 
   * Returns an estimate of the time of closest approach.  This value is in absolute time 
   * (not relative from a waypoint).  This point approximates the point where the two aircraft
   * are closest.  The definition of closest is not simple.  Specifically, space in the vertical
   * dimension counts more than space in the horizontal dimension: encroaching in the protected
   * zone 100 vertically is much more serious than encroaching 100 ft. horizontally. 
   * 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeClosest(int i) {
    if (i < 0 || i >= size()) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getTimeClosest()");
      return 0.0;
    }
    return core.getTimeClosest(i);
  }

  /** 
   * Returns the cylindrical distance at the time of closest approach. 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getDistanceClosest(int i) {
    if (i < 0 || i >= size()) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getClosestHoriz()");
      return 0.0;
    }
    return core.getDistanceClosest(i);
  }
  
//  /**
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a cartesian reference
//   * frame. <p>
//   *
//   * @param sx the x position of the state aircraft in [nmi]
//   * @param sy the y position of the state aircraft in [nmi]
//   * @param sz the z position of the state aircraft in [feet]
//   * @param vx the x component of velocity of the state aircraft in [knot]
//   * @param vy the y component of velocity of the state aircraft in [knot]
//   * @param vz the z component of velocity of the state aircraft in [ft/min]
//   * @param t0 the time, in [s], of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
//   * @param intent the flight plan of the intent aircraft
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//  public static boolean cdsi_xyz(double sx, double sy, double sz, 
//      double vx, double vy, double vz, double t0, 
//      Plan intent, double distance, double height, double startT, double endT) {
//    if (startT > endT) {
//      return false;
//    }
//    Vect3 so = new Vect3(Units.from("nm", sx),
//        Units.from("nm", sy),
//        Units.from("ft", sz));
//    Velocity vo = Velocity.makeVxyz(vx, vy, "kn", vz, "fpm");
//    t0 = Units.from(Units.s, t0);
//    distance = Units.from("nm", distance);
//    height = Units.from("ft", height);
//    startT = Units.from(Units.s, startT);
//    endT = Units.from(Units.s, endT);
//    return CDSICore.cdsicore_xyz(so, vo, t0, Double.MAX_VALUE, intent, distance, height, startT, endT);
//  }
//
//
//  /**
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a latitude/longitude reference
//   * frame. <p>
//   *
//   * @param lat the latitude of the state aircraft in [deg]
//   * @param lon the longitude of the state aircraft in [deg]
//   * @param alt the altitude of the state aircraft in [feet]
//   * @param trk the track angle of the velocity of the state aircraft in [deg from true north]
//   * @param gs the ground speed of velocity of the state aircraft in [knot]
//   * @param vs the vertical speed of velocity of the state aircraft in [ft/min]
//   * @param t0 the time, in [s], of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
//   * @param intent the flight plan of the intent aircraft
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//  public static boolean cdsi_ll(double lat, double lon, double alt, 
//      double trk, double gs, double vs, double t0, 
//      Plan intent, double distance, double height, double startT, double endT) {
//    if (startT > endT) {
//      return false;
//    }
//    LatLonAlt so = LatLonAlt.make(lat, lon, alt);
//    Velocity vo = Velocity.makeTrkGsVs(trk, "deg", gs, "kn", vs, "fpm");
//    t0 = Units.from(Units.s, t0);
//    distance = Units.from("nm", distance);
//    height = Units.from("ft", height);
//    startT = Units.from(Units.s, startT);
//    endT = Units.from(Units.s, endT);
//    return CDSICore.cdsicore_ll(so, vo, t0, Double.MAX_VALUE, intent, distance, height, startT, endT);
//  }

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a cartesian reference
   * frame. <p>
   *
   * @param sx the x position of the state aircraft in [nmi]
   * @param sy the y position of the state aircraft in [nmi]
   * @param sz the z position of the state aircraft in [feet]
   * @param vx the x component of velocity of the state aircraft in [knot]
   * @param vy the y component of velocity of the state aircraft in [knot]
   * @param vz the z component of velocity of the state aircraft in [ft/min]
   * @param t0 the time, in [s], of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
   * @param horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  public boolean detectionXYZ(double sx, double sy, double sz, 
      double vx, double vy, double vz, double t0, double horizon,
      Plan intent, double startT, double endT) {
    if (startT > endT) {
      error.addWarning("startT > endT in detectionXYZ");
      return false;
    }
    Vect3 so = new Vect3(Units.from("nm", sx),
        Units.from("nm", sy),
        Units.from("ft", sz));
    Velocity vo = Velocity.makeVxyz(vx, vy, "kn", vz, "fpm");
    t0 = Units.from(Units.s, t0);
    startT = Units.from(Units.s, startT);
    endT = Units.from(Units.s, endT);
    return core.detectionXYZ(so, vo, t0, horizon, intent, startT, endT);
  }

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a latitude/longitude reference
   * frame. <p>
   *
   * @param lat the latitude of the state aircraft in [deg]
   * @param lon the longitude of the state aircraft in [deg]
   * @param alt the altitude of the state aircraft in [feet]
   * @param trk the track angle of the velocity of the state aircraft in [deg from true north]
   * @param gs the ground speed of velocity of the state aircraft in [knot]
   * @param vs the vertical speed of velocity of the state aircraft in [ft/min]
   * @param t0 the time, in [s], of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
   * @param horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  public boolean detectionLL(double lat, double lon, double alt, 
      double trk, double gs, double vs, double t0, double horizon,
      Plan intent, double startT, double endT) {
    if (startT > endT) {
      error.addWarning("startT > endT in detectionLL");
      return false;
    }
    LatLonAlt so = LatLonAlt.make(lat, lon, alt);
    Velocity vo = Velocity.makeTrkGsVs(trk, "deg", gs, "kn", vs, "fpm");
    t0 = Units.from(Units.s, t0);
    startT = Units.from(Units.s, startT);
    endT = Units.from(Units.s, endT);
    return core.detectionLL(so, vo, t0, horizon, intent, startT, endT);
  }

  public void setCoreDetection(Detection3D d) {
    core.setCoreDetection(d);
  }

  public Detection3D getCoreDetection() {
    return core.getCoreDetection();
  }

  // ErrorReporter Interface Methods

  public boolean hasError() {
    return error.hasError() || core.hasError();
  }
  public boolean hasMessage() {
    return error.hasMessage() || core.hasMessage();
  }
  public String getMessage() {
    return error.getMessage() + core.getMessage();
  }
  public String getMessageNoClear() {
    return error.getMessageNoClear() + core.getMessageNoClear();
  }
  
}

