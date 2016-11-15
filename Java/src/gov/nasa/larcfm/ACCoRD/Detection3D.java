/*
 * Copyright (c) 2013-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;


import java.util.Optional;

import gov.nasa.larcfm.Util.ParameterAcceptor;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public interface Detection3D extends ParameterAcceptor {

  public static final Optional<Detection3D> NoDetector = Optional.empty();

  /**
   * This functional call returns true if there is a violation given the current states.  
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @return    true if there is a violation
   */
  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi);

  /**
   * This functional call returns true if there will be a violation between times B and T from now (relative).  
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return true if there is a conflict within times B to T
   */
  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T);
  
  /**
   * This functional call returns a ConflictData object detailing the conflict between times B and T from now (relative), if any.  
   * @param so  ownship position
   * @param vo  ownship velocity
   * @param si  intruder positon
   * @param vi  intruder velocity
   * @param D   horizontal separation
   * @param H   vertical separation
   * @param B   beginning of detection time (>=0)
   * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
   * @return a ConflictData object detailing the conflict
   */
  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T);

 
  /**
   * Returns a fresh instance of this type of Detection3D with default parameter data.
   */
  public Detection3D make();

  /**
   * Returns a deep copy of this Detection3D object, including any results that have been calculated.  This will duplicate parameter data, but will NOT
   * reference any external objects -- their data, if any, will be copied as well.
   */
  public Detection3D copy();

  /**
   * Return true if two instances are of the same type and have identical parameters (including identifiers).  Use address equality (==) to distinguish instances.
   */
  public boolean equals(Object o);
  
  /**
   * Returns a unique string identifying the class name
   */
  public String getCanonicalClassName();
  
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
  
  /**
   * Return true if this instance is guaranteed to contain the entire volume for detector cd, given the same state values.
   * In general, if cd is of a different type than this object, this method returns false.
   * This should be a reflexive and transitive relation.
   * @param cd
   * @return
   */
  public boolean contains(Detection3D cd);
  
  /** 
   * Return a PVS representation of the object.
   */
  public String toPVS(int prec);
  
}
