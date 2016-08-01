/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.List;

import gov.nasa.larcfm.Util.MovingPolygon3D;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.ParameterAcceptor;
import gov.nasa.larcfm.Util.SimplePoly;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;
import gov.nasa.larcfm.Util.Poly3D;

public interface DetectionPolygon extends ParameterAcceptor {

  /**
   * This returns true if there is a violation given the current states.  
   * This does not calculate timeIn, timeOut, or timeOfClosestApproach
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @return    true if there is a violation
   */
  public boolean violation(Vect3 so, Velocity vo, Poly3D si);


  /**
   * This returns true if there will be a violation between times B and T from now (relative).  
   * This does not modify stored values for timeIn, timeOut, and timeOfClosestApproach -- it either passes through to or mimics a static call.
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return true if there is a conflict within times B to T
   */
  public boolean conflict(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T);
  
  /**
   * This returns true if there will be a violation between times B and T from now (relative).  
   * This also sets the timeIn, timeOut, and timeOfClosestApproach values for the conflict.
   * These values will be truncated, if necessary, to fall within the interval [B,T].
   * If in violation, timeIn should be zero.  If there is no conflict, timeIn >= timeOut.
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return true if there is a conflict within times B to T
   */
  public boolean conflictDetection(Vect3 so, Velocity vo, MovingPolygon3D si, double B, double T);


//  /**
//   * @deprecated
//   * This returns the time of closest approach (relative, in seconds) between two aircraft between times B and T from now (relative)
//   * This does not assume that conflictDetection has been called previously.
//   * @param so  ownship position
//   * @param vo  ownship velocity
//   * @param si  intruder positon
//   * @param vi  intruder velocity
//   * @param D   horizontal separation
//   * @param H   vertical separation
//   * @param B   beginning of detection time (>=0)
//   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
//   * @return time of closest approach between times B and T
//   */
//  public double timeOfClosestApproach(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T);
//

  /**
   * Returns a list of the times into conflict calculated as part of conflictDetection().
   * These will be truncated to the B value used in the conflictDetction call.
   */
  public List<Double> getTimesIn();
  
  /**
   * Returns a list of the times out of conflict calculated as part of conflictDetection().
   * These will be truncated to the T value used in the conflictDetction call.
   */
  public List<Double> getTimesOut();

  /**
   * Returns the time of criticality calculated as part of conflictDetection().
   * This value is only defined if there is a conflict.
   */
  public List<Double> getCriticalTimesOfConflict();

  /**
   * Returns a metric to compare the relative distances between aircraft at time of closest approach.
   * The units of this value depends on the underlying algorithm, and this value may only has meaning when 
   * comparing distances  between two calls to the same algorithm with the same parameter tables.
   *   
   * @param so ownship initial position
   * @param vo ownship velocity
   * @param si intruder initial position
   * @param vi intruder velocity
   * @param t future time (relative, seconds)
   */
  public List<Double> getDistancesAtCriticalTimes();  
  
  
  /**
   * Return a ParameterData object that contains only parameters for this object.
   */
  public ParameterData getParameters();

  /**
   * Modify an existing ParameterData object to include parameters for this object.
   * The return value should reference the same object.
   */
  public void updateParameterData(ParameterData p);
  
  /**
   * Modify a ParameterData object p to include parameter data for this object.
   * Note that multiple instance of the 
   * @param p
   */
  public void setParameters(ParameterData p);
  
  
  /**
   * Returns a new fresh instance of this type of Detection3D.  Configuration information, if any,  (but not results) will be duplicated (with fresh copies).
   */
  public DetectionPolygon make();

  /**
   * Returns a deep copy of this Detection3D object, including any results that have been calculated.
   */
  public DetectionPolygon copy();
  
  /**
   * Returns a unique string identifying the class name
   */
  public String getClassName();

  /**
   * Returns a unique string identifying the class name
   */
  public String getSimpleClassName();

  /**
   * Return an optional user-specified instance identifier.  If not explicitly set (or copied), this will be the empty string.
   */
  public String getIdentifier();

  /**
   * Set an optional user-specified instance identifier.  This will propagate through copy() calls and ParameterData, but not make() calls.
   */
  public void setIdentifier(String s);


  public String toString();
  
}
