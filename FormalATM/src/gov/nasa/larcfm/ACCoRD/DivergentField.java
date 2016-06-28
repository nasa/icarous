/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.Matrix2d;
import gov.nasa.larcfm.Util.Pair;
import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.Vect3;
import gov.nasa.larcfm.Util.VectFuns;
import gov.nasa.larcfm.Util.Velocity;

/**
 * This only detects if aircraft are divergent (conflict/LoS free) or not (conflict/LoS) 
 * 
 */
public class DivergentField implements Detection3D {

//  /** The time to loss of separation */
//  private double t_in;
//
//  /** The time to recovery of loss of separation */
//  private double t_out;
//
//  private double t_tca;
//
//  private double dist_tca;

  private String id = "";
  
  /**
   * Instantiates a new WCV_USAF object.  This is a rough (and highly inefficient) implementation of the USAF's well-clear volume.
   * This is ported from WCBoundary_AirForce_Forward.m
   */
  public DivergentField() {
  }
  
  private boolean divergent(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    return (so.Sub(si)).dot(vo.Sub(vi)) > 0;    
  }
  
  /**
   * Returns true is aircraft are in loss of separation at time 0.
   * 
   * @param so the relative position of the ownship aircraft
   * @param si the relative position of the traffic aircraft
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if aircraft are in loss of separation
   */
  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    return !divergent(so, vo, si, vi);
  }
  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H) {
    return violation(so,vo,si,vi);
  }


  /**
   * Note: 
   * This technically generates an over approximation, as the overall conflict region is generally not convex and there
   * may be multiple conflicts for a single probe.  So it is possible that for time bounds A < B < C < D, conflict (A,B)
   * and conflict (C,D) both return true, but conflict (B,C) returns false. 
   * This method calculates a time in and time out that should contain all conflict regions within the time bounds.
   * The TCA value calculated may be outside the time bounds provided.
   */
  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    boolean ret = !divergent(so, vo, si, vi);
    double t_in,t_out,t_tca,dist_tca;
    if (ret) {
      t_in = B;
      t_out = T;
    } else {
      t_in = 0;
      t_out = -1;
    }
    t_tca = VectFuns.tau(so.Sub(si), vo, vi);
    dist_tca = so.linear(vo, t_tca).Sub(si.linear(vi, t_tca)).norm();
    return new ConflictData(t_in,t_out,t_tca,dist_tca,so.Sub(si),vo.Sub(vi));
  }
  
//  public boolean conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
//    return conflictDetection(so,vo,si,vi,B,T);
//  }
  
  public String toString() {
    String rtn =  "Divergent Field";
    return rtn;
  }

  public String toPVS(int prec) {
    return "";
  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return !divergent(so, vo, si, vi);
  }

//  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
//    return conflict(so,vo,si,vi,B,T);
//  }


//  public double timeOfClosestApproach(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
//    return VectFuns.tau(so.Sub(si), vo, vi);
//  }
//
//  public double timeOfClosestApproach(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double D, double H, double B, double T) {
//    return timeOfClosestApproach(so,vo,si,vi,B,T);
//  }
//  
//  public double getTimeIn() {
//    return t_in;
//  }
//
//  public double getTimeOut() {
//    return t_out;
//  }
//
//  public double getCriticalTimeOfConflict() {
//    return t_tca;
//  }
//
//  @Override
//  public double getDistanceAtCriticalTime() {
//    // TODO Auto-generated method stub
//    return dist_tca;
//  }


  public DivergentField make() {
    return new DivergentField();
  }

  public DivergentField copy() {
    DivergentField ret = new DivergentField();
//    ret.t_in = t_in;
//    ret.t_out = t_out;
//    ret.t_tca = t_tca;
//    ret.dist_tca = dist_tca;
    ret.id = id;
    return ret;
  }

  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p;
  }
  
    public void updateParameterData(ParameterData p) {
      p.set("id",id);

  }
    
  public void setParameters(ParameterData p) {
    if (p.contains("id")) {
      id = p.getString("id");
    }
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
    DivergentField other = (DivergentField) obj;
    if (id == null) {
      if (other.id != null)
        return false;
    } else if (!id.equals(other.id))
      return false;

    return true;
  }

  public boolean contains(Detection3D cd) {
    return cd instanceof DivergentField;
  }
  
}
