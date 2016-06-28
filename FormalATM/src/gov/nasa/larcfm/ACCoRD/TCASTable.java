/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ACCoRD;

import java.util.Arrays;

import gov.nasa.larcfm.Util.ParameterData;
import gov.nasa.larcfm.Util.Units;
import gov.nasa.larcfm.Util.f;

public class TCASTable implements ParameterTable {
  private boolean HMDFilter;
  private double TAU[]  = new double[7];  
  private double TCOA[] = new double[7];  
  private double DMOD[] = new double[7]; 
  private double ZTHR[] = new double[7]; 
  private double HMD[]  = new double[7];

  public TCASTable() {
    setDefaultRAThresholds(true);
  }

  public TCASTable(boolean ra) {
    setDefaultRAThresholds(ra);
  }

  /** Copy constructor */
  public TCASTable(TCASTable t) {
    HMDFilter = t.HMDFilter;
    for (int i=0; i < 7; i++) {
      TAU[i] = t.TAU[i];
      TCOA[i] = t.TCOA[i];
      DMOD[i] = t.DMOD[i];
      ZTHR[i] = t.ZTHR[i];
      HMD[i] = t.HMD[i];
    }
  }

  public TCASTable copy() {
    return new TCASTable(this);
  }

  /** Return sensitivity level from alt, specified in internal units */
  static public int getSensitivityLevel(double alt) {
    if (alt < Units.from(Units.ft,1000))    // 2: < 1000' 
      return 2;
    if (alt <= Units.from(Units.ft,2350))   // 3: 1000'-2350'
      return 3;
    if (alt <= Units.from(Units.ft,5000))   // 4: 2350'-5000'
      return 4;
    if (alt <= Units.from(Units.ft,10000))  // 5: 5000'-10000'
      return 5;
    if (alt <= Units.from(Units.ft,20000))  // 6: 10000'-20000'
      return 6;
    if (alt <= Units.from(Units.ft,42000))  // 7: 20000'-42000'
      return 7;
    // if (alt > Units.from("ft",42000))    // 8: > 42000'
    return 8;
  }

  /** Return sensitivity level from alt specified in u units */
  static public int getSensitivityLevel(double alt, String u) {
    return getSensitivityLevel(Units.from(u,alt));
  }

  /* TA TAU Threshold in seconds *///2  3  4  5  6  7  8
  static private double[] TA_TAU = {20,25,30,40,45,48,48};

  /* RA TAU Threshold in seconds *///2  3  4  5  6  7  8 (-1 is N/A)
  static private double[] RA_TAU = {-1,15,20,25,30,35,35};

  /* TA DMOD in internal units */
  static private double[] TA_DMOD = {
    Units.from(Units.NM,0.30),
    Units.from(Units.NM,0.33),
    Units.from(Units.NM,0.48),
    Units.from(Units.NM,0.75),
    Units.from(Units.NM,1.0),
    Units.from(Units.NM,1.3),
    Units.from(Units.NM,1.3)};

  /* RA DMOD in internal units (-1 if N/A) */
  static private double[] RA_DMOD = {-1,    // 2
    Units.from(Units.NM,0.2),               // 3
    Units.from(Units.NM,0.35),              // 4
    Units.from(Units.NM,0.55),              // 5
    Units.from(Units.NM,0.8),               // 6
    Units.from(Units.NM,1.1),               // 7
    Units.from(Units.NM,1.1)};              // 8

  /* TA ZTHR in internal units */
  static private double[] TA_ZTHR = {
    Units.from(Units.ft,850),               // 2
    Units.from(Units.ft,850),               // 3
    Units.from(Units.ft,850),               // 4
    Units.from(Units.ft,850),               // 5
    Units.from(Units.ft,850),               // 6
    Units.from(Units.ft,850),               // 7
    Units.from(Units.ft,1200)};             // 8

  /* RA ZTHR in internal units (-1 if N/A) */
  static private double[] RA_ZTHR = {-1,    // 2
    Units.from(Units.ft,600),               // 3
    Units.from(Units.ft,600),               // 4
    Units.from(Units.ft,600),               // 5
    Units.from(Units.ft,600),               // 6
    Units.from(Units.ft,700),               // 7
    Units.from(Units.ft,800)};              // 8

  /* RA HMD in internal units (-1 if N/A) */
  static private double[] RA_HMD = {-1,     // 2
    Units.from(Units.ft,1215),              // 3
    Units.from(Units.ft,2126),              // 4
    Units.from(Units.ft,3342),              // 5
    Units.from(Units.ft,4861),              // 6
    Units.from(Units.ft,6683),              // 7
    Units.from(Units.ft,6683)};             // 8

  public void setDefaultRAThresholds(boolean ra) {
    HMDFilter = ra;
    for (int i=0; i < 7; i++) {
      TAU[i] = (ra ? RA_TAU[i] : TA_TAU[i]);
      TCOA[i] = TAU[i];
      DMOD[i] = (ra ? RA_DMOD[i] : TA_DMOD[i]);
      ZTHR[i] = (ra ? RA_ZTHR[i] : TA_ZTHR[i]);
      HMD[i] = (ra ? RA_HMD[i] : TA_DMOD[i]);
    }
  }

  /**
   * Returns TAU threshold for sensitivity level sl in seconds
   */
  public double getTAU(int sl)  {
    if (2 <= sl && sl <= 8) 
      return TAU[sl-2];
    return -1;
  }

  /**
   * Returns TCOA threshold for sensitivity level sl in seconds
   */
  public double getTCOA(int sl)  {
    if (2 <= sl && sl <= 8) 
      return TCOA[sl-2];
    return -1;
  }

  /**
   * Returns DMOD for sensitivity level sl in internal units.
   */
  public double getDMOD(int sl)  {
    return DMOD[sl-2];
  }

  /**
   * Returns DMOD for sensitivity level sl in u units.
   */
  public double getDMOD(int sl, String u)  {
    if (2 <= sl && sl <= 8) 
      return Units.to(u,DMOD[sl-2]);
    return -1;
  }

  /**
   * Returns Z threshold for sensitivity level sl in internal units.
   */
  public double getZTHR(int sl)  {
    return ZTHR[sl-2];
  }

  /**
   * Returns Z threshold for sensitivity level sl in u units.
   */
  public double getZTHR(int sl,String u)  {
    if (2 <= sl && sl <= 8) 
      return Units.to(u,ZTHR[sl-2]);
    return -1;
  }

  /**
   * Returns HMD for sensitivity level sl in internal units.
   */
  public double getHMD(int sl)  {
    if (2 <= sl && sl <= 8) 
      return HMD[sl-2];
    return -1;
  }

  /**
   * Returns HMD for sensitivity level sl in u units.
   */
  public double getHMD(int sl, String u)  {
    if (2 <= sl && sl <= 8) 
      return Units.to(u,HMD[sl-2]);
    return -1;
  }

  /** Modify the value of Tau Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  public void setTAU(int sl, double val) {
    if (2 <= sl && sl <= 8)
      TAU[sl-2] = val;
  }

  /** 
   * Modify the value of TAU for all sensitivity levels 
   */
  public void setTAU(double val) {
    for (int sl=0; sl < 7; ++sl) {
      TAU[sl] = val;
    }
  }

  /** Modify the value of TCOA Threshold for a given sensitivity level (2-8)
   * Parameter val is given in seconds 
   */
  public void setTCOA(int sl, double val) {
    if (2 <= sl && sl <= 8)
      TCOA[sl-2] = val;
  }

  /** 
   * Modify the value of TCOA for all sensitivity levels 
   */
  public void setTCOA(double val) {
    for (int sl=0; sl < 7; ++sl) {
      TCOA[sl] = val;
    }
  }

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setDMOD(int sl, double val) { 
    if (2 <= sl && sl <= 8)
      DMOD[sl-2] = val;
  }

  /** Modify the value of DMOD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setDMOD(int sl, double val, String u) { 
    if (2 <= sl && sl <= 8)
      DMOD[sl-2] = Units.from(u,val);
  }
  
  /** 
   * Modify the value of DMOD for all sensitivity levels 
   */
  public void setDMOD(double val, String u) {
    double v = Units.from(u,val);
    for (int sl=0; sl < 7; ++sl) {
      DMOD[sl] = v;
    }
  }

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setZTHR(int sl, double val) {
    if (2 <= sl && sl <= 8)
      ZTHR[sl-2] = val;
  }

  /** Modify the value of ZTHR for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setZTHR(int sl, double val, String u) {
    if (2 <= sl && sl <= 8)
      ZTHR[sl-2] = Units.from(u,val);
  }

  /** 
   * Modify the value of ZTHR for all sensitivity levels 
   */
  public void setZTHR(double val, String u) {
    double v = Units.from(u,val);
    for (int sl=0; sl < 7; ++sl) {
      ZTHR[sl] = v;
    }
  }

  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in internal units
   */
  public void setHMD(int sl, double val) {
    if (2 <= sl && sl <= 8)
      HMD[sl-2] = val;
  }

  /** 
   * Modify the value of HMD for a given sensitivity level (2-8)
   * Parameter val is given in u units
   */
  public void setHMD(int sl, double val, String u) {
    if (2 <= sl && sl <= 8)
      HMD[sl-2] = Units.from(u,val);
  }

  /** 
   * Modify the value of HMD for all sensitivity levels 
   */
  public void setHMD(double val, String u) {
    double v = Units.from(u,val);
    for (int sl=0; sl < 7; ++sl) {
      HMD[sl] = v;
    }
  }

  public void setHMDFilter(boolean flag) {
    HMDFilter = flag;
  }

  public boolean getHMDFilter() {
    return HMDFilter;
  }

  /** Return true if the values in the table correspond to the standard RA values */
  public boolean isRAStandard() {
    boolean ra = HMDFilter;
    for (int i=0; ra && i < 7; i++) {
      ra = ra && TAU[i]==RA_TAU[i] && TCOA[i]==TAU[i] && DMOD[i]==RA_DMOD[i] && 
          ZTHR[i]==RA_ZTHR[i] && HMD[i]==RA_HMD[i];
    }
    return ra;
  }

  /** Return true if the values in the table correspond to the standard TA values */
  public boolean isTAStandard() {
    boolean ta = !HMDFilter;
    for (int i=0; ta && i < 7; i++) {
      ta = ta && TAU[i]==TA_TAU[i] && TCOA[i]==TAU[i] &&DMOD[i]==TA_DMOD[i] && 
          ZTHR[i]==TA_ZTHR[i] && HMD[i]==TA_DMOD[i];
    }
    return ta;
  }

  private String array7_units(String units, double v[]) {
    String s="";
    boolean comma = false;
    for (int i=0; i<7; ++i) {
      if (comma) {
        s+=", ";
      } else {
        comma = true;
      }
      s+=Units.str(units,v[i]);
    }
    return s;
  }

  public String toString() {
      String s = "HMDFilter: "+HMDFilter;
      if (isRAStandard()) s = s+"; (RA vals) ";
      else if (isTAStandard()) s= s+"; (TA vals) ";
      s= s+"; TAU: "+array7_units("s",TAU)+"; TCOA: "+array7_units("s",TCOA)+
          "; DMOD: "+array7_units("NM",DMOD)+"; ZTHR: "+array7_units("ft",ZTHR)+
          "; HMD: "+array7_units("ft",HMD);
      return s;
  }
  
  public String toPVS(int prec) {
    String s = "(# ";
    s += "TAU := (: "+f.Fm1(TAU[0]);
    for (int i=1; i < 7; i++) {
      s += ", "+f.Fm1(TAU[i]);
    }
    s += " :), TCOA := (: "+f.Fm1(TCOA[0]);
    for (int i=1; i < 7; i++) {
      s += ", "+f.Fm1(TCOA[i]);
    }
    s += " :), DMOD := (: "+f.FmPrecision(DMOD[0],prec);
    for (int i=1; i < 7; i++) {
      s += ", "+f.FmPrecision(DMOD[i],prec);
    }
    s += " :), ZTHR := (: "+f.FmPrecision(ZTHR[0],prec);
    for (int i=1; i < 7; i++) {
      s += ", "+f.FmPrecision(ZTHR[i],prec);
    }
    s += " :), HMD := (: "+f.FmPrecision(HMD[0],prec);
    for (int i=1; i < 7; i++) {
      s += ", "+f.FmPrecision(HMD[i],prec);
    }
    s += " :), HMDFilter := " + (HMDFilter ? "TRUE" : "FALSE");
    return s + " #)";
  }

  public ParameterData getParameters() {
    ParameterData p = new ParameterData();
    updateParameterData(p);
    return p; 
  }


  public void updateParameterData(ParameterData p) {
    p.set("TCAS_HMDFilter",HMDFilter);
    for (int i = 0; i < TAU.length; i++) {
      p.setInternal("TCAS_TAU_"+(i+2),TAU[i],"s");
    }
    for (int i = 0; i < TCOA.length; i++) {
      p.setInternal("TCAS_TCOA_"+(i+2),TCOA[i],"s");
    }
    for (int i = 0; i < DMOD.length; i++) {
      p.setInternal("TCAS_TTHR_"+(i+2),DMOD[i],"ft");
    }
    for (int i = 0; i < ZTHR.length; i++) {
      p.setInternal("TCAS_ZTHR_"+(i+2),ZTHR[i],"ft");
    }
    for (int i = 0; i < HMD.length; i++) {
      p.setInternal("TCAS_HMD_"+(i+2),HMD[i],"ft");
    }
  }

  public void setParameters(ParameterData p) {
    if (p.contains("TCAS_HMDFilter")) {
      HMDFilter = p.getBool("TCAS_HMDFilter");
    }
    for (int i = 0; i < TAU.length; i++) {
      if (p.contains("TCAS_TAU_"+(i+2))) {
        TAU[i] = p.getValue("TCAS_TAU_"+(i+2));
      }
    }
    for (int i = 0; i < TCOA.length; i++) {
      if (p.contains("TCAS_TCOA_"+(i+2))) {
        TCOA[i] = p.getValue("TCAS_TCOA_"+(i+2));
      }
    }
    for (int i = 0; i < DMOD.length; i++) {
      if (p.contains("TCAS_DMOD_"+(i+2))) {
        DMOD[i] = p.getValue("TCAS_DMOD_"+(i+2));
      }
    }
    for (int i = 0; i < ZTHR.length; i++) {
      if (p.contains("TCAS_ZTHR_"+(i+2))) {
        ZTHR[i] = p.getValue("TCAS_ZTHR_"+(i+2));
      }
    }
    for (int i = 0; i < HMD.length; i++) {
      if (p.contains("TCAS_HMD_"+(i+2))) {
        HMD[i] = p.getValue("TCAS_HMD_"+(i+2));
      }
    }
  }

  @Override
  public int hashCode() {
    final int prime = 31;
    int result = 1;
    result = prime * result + Arrays.hashCode(DMOD);
    result = prime * result + Arrays.hashCode(HMD);
    result = prime * result + (HMDFilter ? 1231 : 1237);
    result = prime * result + Arrays.hashCode(TAU);
    result = prime * result + Arrays.hashCode(TCOA);
    result = prime * result + Arrays.hashCode(ZTHR);
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
    TCASTable other = (TCASTable) obj;
    if (!Arrays.equals(DMOD, other.DMOD))
      return false;
    if (!Arrays.equals(HMD, other.HMD))
      return false;
    if (HMDFilter != other.HMDFilter)
      return false;
    if (!Arrays.equals(TAU, other.TAU))
      return false;
    if (!Arrays.equals(TCOA, other.TCOA))
      return false;
    if (!Arrays.equals(ZTHR, other.ZTHR))
      return false;
    return true;
  }

  public boolean contains(TCASTable tab) {
    for (int i=0; i < 7; i++) {
      if (TAU[i] < tab.TAU[i] || TCOA[i] < tab.TCOA[i] || DMOD[i] < tab.DMOD[i] || ZTHR[i] < tab.ZTHR[i] || HMD[i] < tab.HMD[i]) 
        return false;
    }
    return !HMDFilter || tab.HMDFilter;
  }

}
