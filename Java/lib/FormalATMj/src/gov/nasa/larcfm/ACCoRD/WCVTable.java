/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;

public class WCVTable implements ParameterTable {
  protected double DTHR; // Distance threshold
  protected double ZTHR; // Vertical threshold
  protected double TTHR; // Time threshold
  protected double TCOA; // Time to co-altitude threshold

  /**
   * WCV table SARP concept
   */
  public WCVTable() {
    DTHR = Units.from("nmi",0.66);   
    ZTHR = Units.from("ft",450);
    TTHR = 35; // [s]
    TCOA = 0;  // [s]
  }

  /**
   * Copy constructor -- returns a fresh copy
   * @param tab
   */
  public WCVTable(WCVTable tab) {
    copyValues(tab);
  }
  
  /**
   * Table containing specified values (internal units)
   */
  
  public WCVTable(double dthr, double zthr, double tthr, double tcoa) {
    DTHR = dthr;
    ZTHR = zthr;
    TTHR = tthr;
    TCOA = tcoa;
  }

  public WCVTable copy() {
    return new WCVTable(this);
  }

  /**
   * Initialize this table's values with those of another table
   */
  public void copyValues(WCVTable tab) {
    DTHR = tab.DTHR;
    ZTHR = tab.ZTHR;
    TTHR = tab.TTHR;
    TCOA = tab.TCOA;
  }
  
  /**
   * Return horizontal distance threshold DTHR in internal units
   */

  public double getDTHR()  {
    return DTHR;
  }

  /**
   * Return horizontal distance threshold DTHR in u units
   */

  public double getDTHR(String u)  {
    return Units.to(u,DTHR);
  }

  /**
   * Return vertical distance threshold ZTHR in internal units
   */

  public double getZTHR()  {
    return ZTHR;
  }

  /**
   * Return vertical distance threshold ZTHR in u units
   */

  public double getZTHR(String u)  {
    return Units.to(u,ZTHR);
  }

  /**
   * Return horizontal time threshold TTHR in seconds
   */

  public double getTTHR()  {
    return TTHR;
  }
  
  public double getTTHR(String u)  {
    return Units.to(u, TTHR);
  }
  
  /**
   * Return vertical time threshold TCOA in seconds
   */

  public double getTCOA()  {
    return TCOA;
  }
  public double getTCOA(String u)  {
    return Units.to(u,TCOA);
  }
  
  /* 
   * Set horizontal distance threshold DTHR to val, which is specified in internal units
   */
  
  public void setDTHR(double val) {
    DTHR = val;
  }

  /*
   *  Set horizontal distance threshold DTHR to val, which is specified in u units
   */
  
  public void setDTHR(double val, String u) {
    DTHR = Units.from(u,val);
  }

  /* 
   * Set vertical distance threshold ZTHR to val, which is specified in u units
   */
  
  public void setZTHR(double val) {
    ZTHR = val;
  }

  /*
   *  Set vertical distance threshold ZTHR to val, which is specified in internal units
   */
  
  public void setZTHR(double val, String u) {
    ZTHR = Units.from(u,val);
  }

  /*
   *  Set horizontal time threshold TTHR to val, which is specified in seconds
   */
 
  public void setTTHR(double val) {
    TTHR = val;
  }

  public void setTTHR(double val, String u) {
    TTHR = Units.from(u,val);
  }
  
  
  /* 
   * Set vertical time threshold TCOA to val, which is specified in seconds
   */
  
  public void setTCOA(double val) {
    TCOA = val;
  }
  
  public void setTCOA(double val, String u) {
    TCOA = Units.from(u,val);
  }

  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p;
  }
  
  public void updateParameterData(ParameterData p) {
    p.setInternal("WCV_DTHR",DTHR,"ft");
    p.setInternal("WCV_ZTHR",ZTHR,"ft");
    p.setInternal("WCV_TTHR",TTHR,"s");
    p.setInternal("WCV_TCOA",TCOA,"s");
  }

  public void setParameters(ParameterData p) {
    if (p.contains("WCV_DTHR")) {
      DTHR = p.getValue("WCV_DTHR");
    }
    if (p.contains("WCV_ZTHR")) {
      ZTHR = p.getValue("WCV_ZTHR");
    }
    if (p.contains("WCV_TTHR")) {
      TTHR = p.getValue("WCV_TTHR");
    }
    if (p.contains("WCV_TCOA")) {
      TCOA = p.getValue("WCV_TCOA");
    }
  }
  
  public String toString() {
    return "DTHR: "+Units.str("NM",DTHR)+"; ZTHR: "+Units.str("ft",ZTHR)+"; TTHR: "+
        Units.str("s",TTHR)+"; TCOA: "+Units.str("s",TCOA);
  }
  
  public String toPVS(int prec) {
    return "(# DTHR := "+f.FmPrecision(DTHR,prec)+", ZTHR := "+f.FmPrecision(ZTHR,prec)+
        ", TTHR := "+f.Fm1(TTHR)+", TCOA := "+f.Fm1(TCOA)+" #)";
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    long temp;
    temp = Double.doubleToLongBits(DTHR);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(TCOA);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(TTHR);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    temp = Double.doubleToLongBits(ZTHR);
    result = prime * result + (int) (temp ^ (temp >>> 32));
    return result;
  }

  @Override
  public boolean equals(Object obj) {
    if (this == obj)
      return true;
    if (obj == null)
      return false;
    if (getClass() != obj.getClass())
      return false;
    WCVTable other = (WCVTable) obj;
    if (Double.doubleToLongBits(DTHR) != Double.doubleToLongBits(other.DTHR))
      return false;
    if (Double.doubleToLongBits(TCOA) != Double.doubleToLongBits(other.TCOA))
      return false;
    if (Double.doubleToLongBits(TTHR) != Double.doubleToLongBits(other.TTHR))
      return false;
    if (Double.doubleToLongBits(ZTHR) != Double.doubleToLongBits(other.ZTHR))
      return false;
    return true;
  }
  
  public boolean contains(WCVTable tab) {
    return DTHR >= tab.DTHR && TCOA >= tab.TCOA && TTHR >= tab.TTHR && ZTHR >= tab.ZTHR;
  }
  
}
