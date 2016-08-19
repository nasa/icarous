/*
 * Copyright (c) 2014-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;


public class CD3DTable implements ParameterTable {
  double D;
  double H;
  
  public CD3DTable() {
    D = Units.from("nmi",5);
    H = Units.from("ft",1000);
  }

  public CD3DTable(double d, double h) {
    D = d;
    H = h;
  }

  public CD3DTable(double d, String dunit, double h, String hunit) {
    D = Units.from(dunit, d);
    H = Units.from(hunit, h);
  }
  
  /**
   * Copy constructor -- returns a fresh copy.
   */
  public CD3DTable(CD3DTable tab) {
    D = tab.D;
    H = tab.H;
  }

  public CD3DTable copy() {
    return new CD3DTable(this);
  }

  public void copyValues(CD3DTable tab) {
    D = tab.D;
    H = tab.H;
  }
  
  public double getHorizontalSeparation() {
    return D;
  }

  public void setHorizontalSeparation(double d) {
    D = d;
  }

  public double getVerticalSeparation() {
    return H;
  }

  public void setVerticalSeparation(double h) {
    H = h;
  }
 
  public double getHorizontalSeparation(String unit) {
    return Units.to(unit, D);
  }

  public void setHorizontalSeparation(double d, String unit) {
    D = Units.from(unit, d);
  }

  public double getVerticalSeparation(String unit) {
    return Units.to(unit, H);
  }

  public void setVerticalSeparation(double h, String unit) {
    H = Units.from(unit, h);
  }

  
  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p;
  }


  public void updateParameterData(ParameterData p) {
    p.setInternal("D",D,"nmi",4);
    p.setInternal("H",H,"ft",4);
  }

  
  public void setParameters(ParameterData p) {
    if (p.contains("D")) {
      D = p.getValue("D");
    }
    if (p.contains("H")) {
      H = p.getValue("H");
    }
  }
  
  public String toString() {
    return "D: "+Units.str("NM",D)+"; H: "+Units.str("ft",H);
  }
  
  public String toPVS(int prec) {
    return "(# D:= "+f.FmPrecision(D,prec)+", H:= "+f.FmPrecision(H,prec)+" #)";
  }

  @Override
 public int hashCode() {
   final int prime = 31;
   int result = 1;
   long temp;
   temp = Double.doubleToLongBits(D);
   result = prime * result + (int) (temp ^ (temp >>> 32));
   temp = Double.doubleToLongBits(H);
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
   CD3DTable other = (CD3DTable) obj;
   if (Double.doubleToLongBits(D) != Double.doubleToLongBits(other.D))
     return false;
   if (Double.doubleToLongBits(H) != Double.doubleToLongBits(other.H))
     return false;
   return true;
 }
  
}

