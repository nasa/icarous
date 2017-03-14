/*
 * Copyright (c) 2012-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.*;

/* Well Clear Volume concept based on time variable
 * DTHR, ZTHR, and TTHR are distance, altitude, and time thresholds, respectively 
 */

public abstract class WCV_tvar implements Detection3D {

  protected WCVTable table;
  protected WCV_Vertical wcv_vertical;

  protected String id = "";
  
  public abstract WCV_tvar copy();
  
  public abstract WCV_tvar make();

  /** 
   * Sets the internal table to be a copy of the supplied one. 
   **/
  public void setWCVTable(WCVTable tab) {
    table = tab.copy();
  }

  public double getDTHR()  {
    return table.getDTHR();
  }

  public double getDTHR(String u)  {
    return table.getDTHR(u);
  }

  public double getZTHR()  {
    return table.getZTHR();
  }

  public double getZTHR(String u)  {
    return table.getZTHR(u);
  }

  public double getTTHR()  {
    return table.getTTHR();
  }

  public double getTTHR(String u)  {
    return table.getTTHR(u);
  }

  public double getTCOA()  {
    return table.getTCOA();
  }

  public double getTCOA(String u)  {
    return table.getTCOA(u);
  }

  public void setDTHR(double val) {
    table.setDTHR(val);
  }

  public void setDTHR(double val, String u) {
    table.setDTHR(val,u);
  }   
  
  public void setZTHR(double val) {
    table.setZTHR(val);
  }

  public void setZTHR(double val, String u) {
    table.setZTHR(val,u);
  }

  public void setTTHR(double val) {
    table.setTTHR(val);
  }

  public void setTTHR(double val, String u) {
    table.setTTHR(val,u);
  }

  public void setTCOA(double val) {
    table.setTCOA(val);
  }

  public void setTCOA(double val, String u) {
    table.setTCOA(val,u);
  }

  abstract public double horizontal_tvar(Vect2 s, Vect2 v);

  abstract public LossData horizontal_WCV_interval(double T, Vect2 s, Vect2 v);
  
  public boolean horizontal_WCV(Vect2 s, Vect2 v) {
    if (s.norm() <= table.DTHR) return true;
    if (Horizontal.dcpa(s,v) <= table.DTHR) {
      double tvar = horizontal_tvar(s,v);
      return 0  <= tvar && tvar <= table.TTHR;
    }
    return false;
  }

  public boolean violation(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
    Vect2 so2 = so.vect2();
    Vect2 si2 = si.vect2();
    Vect2 s2 = so2.Sub(si2);
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect2 v2 = vo2.Sub(vi2);
    return horizontal_WCV(s2,v2) &&
        wcv_vertical.vertical_WCV(table.ZTHR,table.TCOA,so.z-si.z,vo.z-vi.z);
  }

  public boolean conflict(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return WCV3D(so,vo,si,vi,B,T).conflict(); 
  }

  public ConflictData conflictDetection(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {    
    LossData ld = WCV3D(so,vo,si,vi,B,T);
    double t_tca = (ld.getTimeIn() + ld.getTimeOut())/2;
    double dist_tca = so.linear(vo, t_tca).Sub(si.linear(vi, t_tca)).cyl_norm(table.DTHR,table.ZTHR);
    return new ConflictData(ld,t_tca,dist_tca,so.Sub(si),vo.Sub(vi));
  }

  public LossData WCV3D(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    return WCV_interval(so,vo,si,vi,B,T);
  }

  // Assumes 0 <= B < T
  public LossData WCV_interval(Vect3 so, Velocity vo, Vect3 si, Velocity vi, double B, double T) {
    double time_in = T;
    double time_out = B;

    Vect2 so2 = so.vect2();
    Vect2 si2 = si.vect2();
    Vect2 s2 = so2.Sub(si2);
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    Vect2 v2 = vo2.Sub(vi2);
    double sz = so.z-si.z;
    double vz = vo.z-vi.z;

    Interval ii = wcv_vertical.vertical_WCV_interval(table.ZTHR,table.TCOA,B,T,sz,vz);

    if (ii.low > ii.up) {
      return new LossData(time_in, time_out);
    }
    Vect2 step = v2.ScalAdd(ii.low,s2);
    if (Util.almost_equals(ii.low,ii.up)) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
      if (horizontal_WCV(step,v2)) {
        time_in = ii.low;
        time_out = ii.up;
      }
      return new LossData(time_in, time_out);
    }
    LossData ld = horizontal_WCV_interval(ii.up-ii.low,step,v2);
    time_in = ld.getTimeIn() + ii.low;
    time_out = ld.getTimeOut() + ii.low;
    return new LossData(time_in, time_out);
  }
  
  public boolean containsTable(WCV_tvar wcv) {
    return table.contains(wcv.table);
  }

  public String toString() {
    return (id.equals("") ? "" : id+" : ")+getSimpleClassName()+" = {"+table.toString()+"}";
  }

  public String toPVS(int prec) {
    return table.toPVS(prec);
  }

  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p;
  }

  public  void updateParameterData(ParameterData p) {
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
    WCV_tvar other = (WCV_tvar) obj;
    if (id == null) {
      if (other.id != null)
        return false;
    } else if (!id.equals(other.id))
      return false;
    if (!super.equals(other))
      return false;
    return true;
  }

}
