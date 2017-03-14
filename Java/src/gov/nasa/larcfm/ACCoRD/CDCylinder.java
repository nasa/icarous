/*
 * CD3D.java 
 * Release: ACCoRDj-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * CD3D is an algorithm for 3-D conflict *detection*.
 *
 * Unit Convention
 * ---------------
 * All units in this file are *internal*:
 * - Units of distance are denoted [d]
 * - Units of time are denoted     [t]
 * - Units of speed are denoted    [d/t]
 *
 * REMARK: X Vect3s to East, Y Vect3s to North. 
 *
 * Naming Convention
 * -----------------
 *   The intruder is fixed at the origin of the coordinate system.
 * 
 *   D  : Diameter of the protected zone [d]
 *   H  : Height of the protected zone [d]
 *   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *   T  : Upper bound of lookahead time interval [t] (T < 0 means infinite lookahead time)
 *   s  : Relative 3-D position of the ownship [d,d,d]
 *   vo : Ownship velocity vector [d/t,d/t,d/t]
 *   vi : Traffic velocity vector [d/t,d/t,d/t]
 * 
 * Functions
 * ---------
 * violation : Check for 3-D loss of separation
 * detection : 3-D conflict detection with calculation of conflict interval 
 * cd3d      : Check for predicted conflict
 * 
 * Global variables (modified by detection)
 * ----------------
 * t_in  : Time to loss of separation
 * t_out : Time to recovery of loss of separation
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 * 
 * 
 * Note: The B and T parameters also affect the time in and time out of loss values.
 * 
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.LossData;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Velocity;

public class CDCylinder implements Detection3D {

  private CD3DTable table;

  private String id = "";

  /**
   * Instantiates a new CD3D object.
   */
  public CDCylinder() {
    table = new CD3DTable();
  }

  /**
   * This specifies the internal table is a copy of the provided table
   * @param tab
   */
  public CDCylinder(CD3DTable tab) {
    table = tab.copy();
  }

  public CDCylinder(double d, String dunit, double h, String hunit) {
    table = new CD3DTable(d,dunit,h,hunit);
  }

  /**
   * Create a new state-based conflict detection object using specified units.
   * 
   * @param distance the minimum horizontal separation distance in specified units
   * @param height the minimum vertical separation height in specified units.
   */
  public static CDCylinder make(double distance, String dUnits, double height, String hUnits) {
    return new CDCylinder(distance,dUnits,height,hUnits);  
  }

  /**
   * Create a new state-based conflict detection object using internal units.
   * 
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  public static CDCylinder mk(double distance, double height) {
    return new CDCylinder(distance, "m", height, "m");  
  }

  /**
   * Return a copy of this object's table
   */
  public CD3DTable getCD3DTable() {
    return table.copy();
  }

  /** Sets the internal table to be a copy of the supplied one */
  public void setCD3DTable(CD3DTable tab) {
    table = tab.copy();
  }

  public double getHorizontalSeparation() {
    return table.getHorizontalSeparation();
  }

  public double getVerticalSeparation() {
    return table.getVerticalSeparation();
  }

  public void setHorizontalSeparation(double d) {
    table.setHorizontalSeparation(d);
  }

  public void setVerticalSeparation(double h) {
    table.setVerticalSeparation(h);
  }

  public double getHorizontalSeparation(String unit) {
    return table.getHorizontalSeparation(unit);
  }

  public double getVerticalSeparation(String unit) {
    return table.getVerticalSeparation(unit);
  }

  public void setHorizontalSeparation(double d, String unit) {
    table.setHorizontalSeparation(d, unit);
  }

  public void setVerticalSeparation(double h, String unit) {
    table.setVerticalSeparation(h, unit);
  }

  /**
   * Computes the conflict time interval in [B,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time (B < T)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [B,T].
   */
  public LossData detection(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double B, double T) { 
    return CD3D.detection(s,vo,vi,D,H,B,T);
  }

  /**
   * Computes the conflict time interval in [0,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the the lookahead time (T > 0)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [0,T].
   */
  public LossData detection(Vect3 s, Vect3 vo, Vect3 vi, double D, double H, double T) {
    return detection(s,vo,vi,D,H,0,T);
  }

  /**
   * Computes the conflict time interval in [0,...).
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [0,...)
   */
  public LossData detection(Vect3 s, Vect3 vo, Vect3 vi, double D, double H) {
    return detection(s,vo,vi,D,H,0,Double.POSITIVE_INFINITY);
  }

  public String toString() {
    return (id.equals("") ? "" : id+" : ")+getSimpleClassName()+" = {"+table.toString()+"}";
  }
  
  public String toPVS(int prec) {
    return table.toPVS(prec);
  }

  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H) {
    return CD3D.lossOfSep(so,si,D,H);
  }


  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    return violation(so,vo,si,vi,table.getHorizontalSeparation(),table.getVerticalSeparation());
  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
    return CD3D.cd3d(so.Sub(si), vo, vi, D, H, B, T); 
  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return conflict(so, vo, si, vi, table.getHorizontalSeparation(), table.getVerticalSeparation(), B, T); 
  }

  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
    Vect3 s = so.Sub(si);
    Velocity v = vo.Sub(vi);
    double t_tca = CD3D.tccpa(s, vo, vi, D, H, B, T);
    double dist_tca = s.linear(v,t_tca).cyl_norm(D, H);
    LossData ld = CD3D.detection(s,vo,vi,D,H,B,T);
    return new ConflictData(ld,t_tca,dist_tca,s,v);
  }

  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return conflictDetection(so,vo,si,vi,table.getHorizontalSeparation(), table.getVerticalSeparation(), B, T); 
  }

  public double timeOfClosestApproach(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
    return CD3D.tccpa(so.Sub(si), vo, vi, D, H, B, T);
  }

  public double timeOfClosestApproach(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return timeOfClosestApproach(so,vo,si,vi,table.getHorizontalSeparation(), table.getVerticalSeparation(), B, T); 
  }

  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p;
  }

  public void updateParameterData(ParameterData p) {
    table.updateParameterData(p);
    p.set("id",id);
  }

  public void setParameters(ParameterData p) {
    table.setParameters(p);
    if (p.contains("id")) {
      id = p.getString("id");
    }
  }

  /**
   * Returns a fresh instance of this type of Detection3D with default parameter data.
   */
  public CDCylinder make() {
    return new CDCylinder();
  }

  /**
   * Returns a deep copy of this CDCylinder object, including any results that have been calculated.  This will duplicate parameter data, but will NOT
   * link any existing CD3DTable.  Call setCD3DTable() if linking is necessary.
   */
  public CDCylinder copy() {
    CDCylinder ret = new CDCylinder();
    ret.id = id;
    ret.table = new CD3DTable(table);
    return ret;
  }

  public String getSimpleClassName() {
    return getClass().getSimpleName();
  }
  
  public String getCanonicalClassName() {
    return getClass().getCanonicalName(); 
  }

  public String getIdentifier() {
    return id;
  }

  public void setIdentifier(String s) {
    id = s;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    CDCylinder other = (CDCylinder) obj;
    if (id == null) {
      if (other.id != null)
        return false;
    } else if (!id.equals(other.id))
      return false;
    if (table == null) {
      if (other.table != null)
        return false;
    } else if (!table.equals(other.table))
      return false;
    return true;
  }

  public boolean contains(Detection3D cd) {
    if (cd instanceof CDCylinder) {
      CDCylinder d = (CDCylinder) cd;
      return table.D >= d.table.D && table.H >= d.table.H; 
    }
    return false;
  }

}
