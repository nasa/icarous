/*
 * CDII - conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.PlanUtil;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.ErrorReporter;
import gov.nasa.larcfm.Util.ErrorLog;
import gov.nasa.larcfm.Util.Plan;

import java.text.*;           // for DecimalFormat
import java.util.ArrayList;

/** 
 * This class implements the algorithm for conflict detection between
 * an ownship and a traffic aircraft, where the trajectories of both
 * aircraft are modeled with intent information.  <p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *   
 * When using the object method, all method getTimeIn(), etc, assume
 * that the detection() method has been called first.<p>
 * 
 * Usage:
 *     CDII.detection(ownship,traffic,B,T)
 *     
 *     If there are long legs in the ownship Plan, then it is prudent to call
 *     
 *        PlanUtil.interpolateVirtuals(ownship, Units.from("NM",0.1) ,0 , Tinfinity);
 *        
 *     before the detection call.   

 * 
 */
public final class CDII implements ErrorReporter, Detection3DAcceptor {  

  private CDIICore cdiicore;
  private ErrorLog error;

  //  /** 
  //   * Create a new conflict detection (intent information for both the ownship
  //   * and traffic) object.
  //   *
  //   * @param distance the minimum horizontal separation distance [nmi]
  //   * @param height the minimum vertical separation height [ft].
  //   */
  //  public CDII(double distance, double height) {
  //    core = new CDIICore(Units.from("nm", distance), Units.from("ft", height));
  //    error = new ErrorLog("CDII");
  //  }

  /** 
   * Create a new conflict detection (intent information for both the ownship
   * and traffic) object.
   *
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  private CDII(Detection3D cd) {
    cdiicore = new CDIICore(cd);
    error = new ErrorLog("CDII");
  }

  /**
   * Factory method for cylindrical detection
   * @param distance
   * @param dUnits
   * @param height
   * @param hUnits
   * @return CDII object
   */
  public static CDII make(double distance, String dUnits, double height, String hUnits) {
    CDCylinder cd = CDCylinder.make(distance, dUnits, height, hUnits);
    return new CDII(cd);
  }

  /**
   * Factory method for cylindrical detection
   * 
   * @param distance
   * @param height
   * @param haccuracy
   * @param vaccuracy
   * @param minDt
   * @return CDII object
   */
  public static CDII mk(double distance, double height, double haccuracy, double vaccuracy, double minDt) {
    CDCylinder cd = CDCylinder.mk(distance, height);
    return new CDII(cd);
  }


  /**
   * Factory method for cylindrical detection
   * @param distance
   * @param dUnits
   * @param height
   * @param hUnits
   * @return CDII object
   */
  public static CDII mk(double distance, double height) {
    CDCylinder cd = CDCylinder.mk(distance, height);
    return new CDII(cd);
  }


     /**
       * Create a new conflict detection (intent information for both the ownship
       * and traffic) object.
       */
  public CDII() {
    cdiicore = new CDIICore();
    error = new ErrorLog("CDII");
  }


  public CDIICore getCore() {
    return cdiicore;
  }

  /** Returns the minimum horizontal separation distance in [m] */
  private double getDistance() {
    Detection3D cd = cdiicore.getCoreDetection(); 
    if (cd instanceof CDCylinder) {
      return ((CDCylinder)cd).getHorizontalSeparation();
    }
    error.addError("getDistance: incorrect detection core for this function");
    return Double.NaN;
  }

  /** Returns the minimum vertical separation distance in [m] */
  private double getHeight() {
    Detection3D cd = cdiicore.getCoreDetection(); 
    if (cd instanceof CDCylinder) {
      return ((CDCylinder)cd).getVerticalSeparation();
    }
    error.addError("getHeight: incorrect detection core for this function");
    return Double.NaN;
  }

  /** Returns the minimum horizontal separation distance in the given units */
  public double getDistance(String units) {
    return Units.to(units, getDistance());
  }

  /** Returns the minimum vertical separation distance in the given units */
  public double getHeight(String units) {
    return Units.to(units, getHeight());
  }

  /**
   * Returns the conflict detection filter time.
   * 
   * @return the conflict detection filter time seconds
   */
  public double getFilterTime() {
    return cdiicore.getFilterTime();
  }

  /** Sets the minimum horizontal separation distance in [m] */
  private void setDistance(double distance) {
    Detection3D cd = cdiicore.getCoreDetection(); 
    if (cd instanceof CDCylinder) {
      ((CDCylinder)cd).setHorizontalSeparation(distance);
    } else {
      error.addError("setDistance: incorrect detection core for this function");
    }
  }

  /** Sets the minimum vertical separation distance in [m] */
  private void setHeight(double height) {
    Detection3D cd = cdiicore.getCoreDetection(); 
    if (cd instanceof CDCylinder) {
      ((CDCylinder)cd).setVerticalSeparation(height);
    } else {
      error.addError("setHeight: incorrect detection core for this function");
    }
  }

  /** Sets the minimum horizontal separation distance in the specified units */
  public void setDistance(double distance, String units) {
    setDistance(Units.from(units, distance));
  }

  /** Sets the minimum vertical separation distance in the specified units */
  public void setHeight(double height, String units) {
    setHeight(Units.from(units, height));
  }

  /**
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  public void setFilterTime(double cdfilter) {
    cdiicore.setFilterTime(cdfilter);
  }

  /** Returns the number of conflicts */
  public int size() {
    return cdiicore.size();
  }

  /** Returns if there were any conflicts */
  public boolean conflict() {
    return cdiicore.size() > 0;
  }

  //
  // Per conflict information
  //

  /** 
   * Returns the start time of the conflict in [s].  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeIn(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getTimeIn");
      return 0.0;
    }
    return cdiicore.getTimeIn(i);
  }

  /** 
   * Returns the end time of the conflict in [s].  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getTimeOut(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getTimeOut");
      return 0.0;
    }
    return cdiicore.getTimeOut(i);
  }

  /** 
   * Returns the segment number of the entry into a conflict from the ownship aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public int getSegmentIn(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getSegmentIn");
      return 0;
    }
    return cdiicore.getSegmentIn(i);
  }

  /** 
   * Returns the segment number of the exit from a conflict from the ownship aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public int getSegmentOut(int i) {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getSegmentOut");
      return 0;
    }
    return cdiicore.getSegmentOut(i);
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
    return cdiicore.getTimeClosest(i);
  }

  /** 
   * Returns the distance index at the time of closest approach. 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  public double getDistanceClosest(int i) {
    if (i < 0 || i >= size()) {
      error.addError("Out of range error 0 <= "+i+" < "+size()+" in getClosestHoriz()");
      return 0.0;
    }
    return cdiicore.getDistanceClosest(i);
  }


  /**
   * Is there a conflict at any time in the interval from start to
   * end (inclusive). This method assumes that the detection()
   * method has been called first.
   *
   * @param start the time to begin looking for conflicts [s]
   * @param end the time to end looking for conflicts [s]
   * @return true if there is a conflict
   */
  public boolean conflictBetween(double start, double end) {
    return cdiicore.conflictBetween(Units.from("s", start), Units.from("s", end));
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param distance the minimum horizontal separation distance in [nmi]
   * @param height the minimum vertical separation distance in [ft]
   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts 
   * @return true if there is a conflict
   */
  public boolean iterDetection(Plan ownship,Plan traffic,  double startT, double endT, double tStep, double tol) {
    if (ownship.isLatLon() != traffic.isLatLon()) {
      error.addError("Ownship and traffic flight plans are not both Euclidean or Lat/Lon");
    }
    return cdiicore.iterDetection(ownship,traffic, startT, endT, tStep, tol);
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts (absolute time)
   * @return true if there is a conflict
   * 
   * Note: If the aircraft are in loss of separation at time startT, 
   * the time in reported for that conflict will be startT. 
   */
  public boolean detection(Plan ownship, Plan traffic, double startT, double endT, boolean interpolateVirtuals) {
    if (ownship.isLatLon() != traffic.isLatLon()) {
      error.addError("Ownship and traffic flight plans are not both Euclidean or Lat/Lon");
      return false;
    }
    if (startT > endT) {
      error.addWarning("startT > endT in detection()");
      return false;
    }
    Plan ownshipCopy = ownship;
    if (interpolateVirtuals) {
      ownshipCopy = ownship.copy();
      PlanUtil.interpolateVirtuals(ownshipCopy,Units.from("NM",0.1) , startT, endT);
    }
    return cdiicore.detection(ownshipCopy, traffic, startT, endT);
  }

  public boolean detection(Plan ownship, Plan traffic, double startT, double endT) {
    return detection(ownship,traffic,startT,endT,true);
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  This version can be less efficient than the normal detection() call,
   * but all reported conflict time in and time out values will not be constrained to be within the
   * lookahead times used.<p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts (absolute time)
   * @return true if there is a conflict
   * 
   * Note: All time in and time out data reported will be accurate within the length of the given plans. 
   */
  public boolean detectionExtended(Plan ownship, Plan traffic, double startT, double endT, boolean interpolateVirtuals) {
    if (ownship.isLatLon() != traffic.isLatLon()) {
      error.addError("Ownship and traffic flight plans are not both Euclidean or Lat/Lon");
      return false;
    }
    if (startT > endT) {
      error.addWarning("startT > endT in detection()");
      return false;
    }
    Plan ownshipCopy = ownship;
    if (interpolateVirtuals) {
      ownshipCopy = ownship.copy();
      PlanUtil.interpolateVirtuals(ownshipCopy,Units.from("NM",0.1) , startT, endT); 
    }
    return cdiicore.detectionExtended(ownshipCopy, traffic, startT, endT);
  }

  public boolean detectionExtended(Plan ownship, Plan traffic, double startT, double endT) {
    return detectionExtended(ownship,traffic,startT,endT,true);
  }


  public void setCoreDetection(Detection3D d) {
    cdiicore.setCoreDetection(d);
  }

  public Detection3D getCoreDetection() {
    return cdiicore.getCoreDetection();
  }


  public String toString() {
    String str = " TimeIn    Timeout    SegmentIn  SegmentOut \n";
    str = str +  " -------   ---------  ---------  ---------- \n";
    for (int k = 0; k < size(); k++) {
      str = str + Fm4(getTimeIn(k))+"  "+Fm4(getTimeOut(k))
          +"      "+getSegmentIn(k)+"          "+getSegmentOut(k)+" \n";
    }
    Detection3D cd = getCoreDetection();
    str = str + "\n"+" cd = "+cd.toString();       
    return str;  
  }


  private static final DecimalFormat Frm = new DecimalFormat("0.0000");

  private String Fm4(double v) {
    return Frm.format(v);
  }

  // ErrorReporter Interface Methods

  public boolean hasError() {
    return error.hasError() || cdiicore.hasError();
  }
  public boolean hasMessage() {
    return error.hasMessage() || cdiicore.hasMessage();
  }
  public String getMessage() {
    return error.getMessage() + cdiicore.getMessage();
  }
  public String getMessageNoClear() {
    return error.getMessageNoClear() + cdiicore.getMessageNoClear();
  }


}

