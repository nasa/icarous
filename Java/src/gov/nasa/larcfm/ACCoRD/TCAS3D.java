/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.*;

public class TCAS3D implements Detection3D {
  String id = "";

  private TCASTable table;

  /** Constructor that uses the default TCAS tables. */
  public TCAS3D() {
    table = new TCASTable();
  }

  /** Constructor that specifies a particular instance of the TCAS tables. */
  public TCAS3D(TCASTable tables) {
    table = tables.copy();
  }

  /** This returns a copy of the internal TCAS table */
  public TCASTable getTCASTable() {
    return table.copy();
  }

  /** This sets the internal table to be a deep copy of the supplied one.  Any previous links will be broken. */
  public void setTCASTable(TCASTable tables) {
    table = tables.copy();
  }

  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    return TCASII_RA(so, vo, si, vi);
  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return RA3D(so,vo,si,vi,B,T).conflict(); 
  }

  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return RA3D(so,vo,si,vi,B,T);
  }

  public TCAS3D make() {
    return new TCAS3D();
  }

  /**
   * Returns a deep copy of this TCAS3D object, including any results that have been calculated.  This will duplicate parameter data, but will NOT
   * link any existing TCASTable.  Call setTCASTables() if linking is necessary.
   */
  public TCAS3D copy() {
    TCAS3D ret = new TCAS3D();
    ret.table = new TCASTable(table); 
    ret.id = id;
    return ret;
  }

  static public boolean vertical_RA(double sz, double vz, double ZTHR, double TCOA) {
    if (Math.abs(sz) <= ZTHR) return true;
    if (Util.almost_equals(vz,0)) return false; // [CAM] Changed from == to almost_equals to mitigate numerical problems 
    double tcoa = Vertical.time_coalt(sz,vz);
    return 0 <= tcoa && tcoa <= TCOA;
  }

  static boolean cd2d_TCAS_after(double HMD, Vect2 s, Vect2 vo, Vect2 vi, double t) {
    Vect2 v = vo.Sub(vi);
    return  
        (vo.almostEquals(vi) && s.sqv() <= Util.sq(HMD)) ||
        (v.sqv() > 0 && Horizontal.Delta(s,v,HMD) >= 0 &&
        Horizontal.Theta_D(s,v,1,HMD) >= t);
  } 

  static boolean cd2d_TCAS(double HMD, Vect2 s, Vect2 vo, Vect2 vi) {
    return cd2d_TCAS_after(HMD,s,vo,vi,0);
  }

  // if true, then ownship has a TCAS resolution advisory at current time
  public boolean TCASII_RA(Vect3 so, Vect3 vo, Vect3 si, Vect3 vi) {

    Vect2 so2 = so.vect2();
    Vect2 si2 = si.vect2();
    Vect2 s2 = so2.Sub(si2);
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect2 v2 = vo2.Sub(vi2);
    int sl = TCASTable.getSensitivityLevel(so.z);
    boolean usehmdf = table.getHMDFilter();
    double TAU  = table.getTAU(sl);
    double TCOA = table.getTCOA(sl);
    double DMOD = table.getDMOD(sl);
    double HMD  = table.getHMD(sl);
    double ZTHR = table.getZTHR(sl);

    return (!usehmdf || cd2d_TCAS(HMD,s2,vo2,vi2)) &&
        TCAS2D.horizontal_RA(DMOD,TAU,s2,v2) &&
        vertical_RA(so.z-si.z,vo.z-vi.z,ZTHR,TCOA);
  }

  // if true, within lookahead time interval [B,T], the ownship has a TCAS resolution advisory (effectively conflict detection)
  // B must be non-negative and T > B

  public ConflictData RA3D(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return RA3D_interval(so,vo,si,vi,B,T);
  }

  // Assumes 0 <= B < T
  public ConflictData RA3D_interval(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    double time_in     = T;
    double time_out    = B;
    double time_mintau = -1;
    double dist_mintau = -1;

    Vect3 s = so.Sub(si);
    Velocity v = vo.Sub(vi);
    Vect2 s2 = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect2 v2 = v.vect2();
    int sl = TCASTable.getSensitivityLevel(so.z);
    boolean usehmdf = table.getHMDFilter();
    double TAU  = table.getTAU(sl);
    double TCOA = table.getTCOA(sl);
    double DMOD = table.getDMOD(sl);
    double HMD  = table.getHMD(sl);
    double ZTHR = table.getZTHR(sl);

    if (usehmdf && !cd2d_TCAS_after(HMD,s2,vo2,vi2,B)) {
      time_mintau = TCAS2D.time_of_min_tau(DMOD,B,T,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    double sz = so.z-si.z;
    if (Util.almost_equals(vo.z, vi.z) && Math.abs(sz) > ZTHR) {
      time_mintau = TCAS2D.time_of_min_tau(DMOD,B,T,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    double vz = vo.z-vi.z;
    double tentry = B;
    double texit  = T;
    if (!Util.almost_equals(vo.z, vi.z)) {
      double act_H = Util.max(ZTHR,Math.abs(vz)*TCOA);
      tentry = Vertical.Theta_H(sz,vz,-1,act_H);
      texit = Vertical.Theta_H(sz,vz,1,ZTHR);
    }
    Vect2 ventry = v2.ScalAdd(tentry,s2);
    boolean exit_at_centry = ventry.dot(v2) >= 0;
    boolean los_at_centry = ventry.sqv() <= Util.sq(HMD);
    if (texit < B || T < tentry) {
      time_mintau = TCAS2D.time_of_min_tau(DMOD,B,T,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    double tin = Util.max(B,tentry);
    double tout = Util.min(T,texit);
    TCAS2D tcas2d = new TCAS2D();
    tcas2d.RA2D_interval(DMOD,TAU,tin,tout,s2,vo2,vi2);
    double RAin2D = tcas2d.time_in;
    double RAout2D = tcas2d.time_out;
    double RAin2D_lookahead = Util.max(tin,Util.min(tout,RAin2D));
    double RAout2D_lookahead = Util.max(tin,Util.min(tout,RAout2D));
    if (RAin2D > RAout2D || RAout2D<tin || RAin2D > tout ||
    (usehmdf && HMD < DMOD && exit_at_centry && !los_at_centry)) { 
      time_mintau = TCAS2D.time_of_min_tau(DMOD,B,T,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    if (usehmdf && HMD < DMOD) {
      double exitTheta = T;
      if (v2.sqv() > 0) 
        exitTheta = Util.max(B,Util.min(Horizontal.Theta_D(s2,v2,1,HMD),T));
      double minRAoutTheta = Util.min(RAout2D_lookahead,exitTheta);
      time_in = RAin2D_lookahead;
      time_out = minRAoutTheta;
      if (RAin2D_lookahead <= minRAoutTheta) {
        time_mintau = TCAS2D.time_of_min_tau(DMOD,RAin2D_lookahead,minRAoutTheta,s2,v2);
        dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
        return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
      }
      time_mintau = TCAS2D.time_of_min_tau(DMOD,B,T,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    time_in = RAin2D_lookahead;
    time_out = RAout2D_lookahead;
    time_mintau = TCAS2D.time_of_min_tau(DMOD,RAin2D_lookahead,RAout2D_lookahead,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return new ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
  }

  /**
   * Returns TAU threshold for sensitivity level sl in seconds
   */
  public double getTAU(int sl)  {
    return table.getTAU(sl);
  }

  /**
   * Returns TCOA threshold for sensitivity level sl in seconds
   */
  public double getTCOA(int sl)  {
    return table.getTCOA(sl);
  }

  /**
   * Returns DMOD for sensitivity level sl in internal units.
   */
  public double getDMOD(int sl)  {
    return table.getDMOD(sl);
  }

  /**
   * Returns DMOD for sensitivity level sl in u units.
   */
  public double getDMOD(int sl, String u)  {
    return table.getDMOD(sl,u);
  }

  /**
   * Returns Z threshold for sensitivity level sl in internal units.
   */
  public double getZTHR(int sl)  {
    return table.getZTHR(sl);
  }

  /**
   * Returns Z threshold for sensitivity level sl in u units.
   */
  public double getZTHR(int sl,String u)  {
    return table.getZTHR(sl,u);
  }

  /**
   * Returns HMD for sensitivity level sl in internal units.
   */
  public double getHMD(int sl)  {
    return table.getHMD(sl);
  }

  /**
   * Returns HMD for sensitivity level sl in u units.
   */
  public double getHMD(int sl, String u)  {
    return table.getHMD(sl,u);
  }

  /** Modify the value of Tau Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  public void setTAU(int sl, double val) {
    table.setTAU(sl,val);
  }

  /** 
   * Modify the value of TAU for all sensitivity levels 
   */
  public void setTAU(double val) {
    table.setTAU(val);
  }

  /** Modify the value of TCOA Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  public void setTCOA(int sl, double val) {
    table.setTCOA(sl,val);
  }

  /** 
   * Modify the value of TCOA for all sensitivity levels 
   */
  public void setTCOA(double val) {
    table.setTCOA(val);
  }

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setDMOD(int sl, double val) { 
    table.setDMOD(sl, val);
  }

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setDMOD(int sl, double val, String u) { 
    table.setDMOD(sl,val,u);
  }

  /** 
   * Modify the value of DMOD for all sensitivity levels 
   */
  public void setDMOD(double val, String u) {
    table.setDMOD(val,u);
  }

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setZTHR(int sl, double val) {
    table.setZTHR(sl,val);
  }

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setZTHR(int sl, double val, String u) {
    table.setZTHR(sl,val,u);
  }

  /** 
   * Modify the value of ZTHR for all sensitivity levels 
   */
  public void setZTHR(double val, String u) {
    table.setZTHR(val,u);
  }

  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setHMD(int sl, double val) {
    table.setHMD(sl,val);
  }

  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setHMD(int sl, double val, String u) {
    table.setHMD(sl,val,u);
  }

  /** 
   * Modify the value of HMD for all sensitivity levels 
   */
  public void setHMD(double val, String u) {
    table.setHMD(val,u);
  }

  public void setHMDFilter(boolean flag) {
    table.setHMDFilter(flag);
  }

  public boolean getHMDFilter() {
    return table.getHMDFilter();
  }

  public String toString() {
    return (id.equals("") ? "" : id+" = ")+getSimpleClassName()+": {"+table.toString()+"}";
  }
  
  public String toPVS(int prec) {
    return table.toPVS(prec);
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
    TCAS3D other = (TCAS3D) obj;
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
    if (cd instanceof TCAS3D) {
      TCAS3D d = (TCAS3D) cd;
      return table.contains(d.table);
    }
    return false;
  }

}
