/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Units.h"
#include "TCASTable.h"
#include "format.h"
#include "ParameterData.h"

namespace larcfm {

TCASTable::TCASTable() {
  setDefaultRAThresholds(true);
  HMDFilter = true;
}

TCASTable::TCASTable(bool ra) {
  setDefaultRAThresholds(ra);
  HMDFilter = true;
}

/** Copy constructor */
TCASTable::TCASTable(const TCASTable& t) {
  HMDFilter = t.HMDFilter;
  for (int i=0; i < 7; i++) {
    TAU[i] = t.TAU[i];
    TCOA[i] = t.TCOA[i];
    DMOD[i] = t.DMOD[i];
    ZTHR[i] = t.ZTHR[i];
    HMD[i] = t.HMD[i];
  }
}


int TCASTable::getSensitivityLevel(double alt) {
  if (alt < Units::from(Units::ft,1000))
    return 2;
  if (alt <= Units::from(Units::ft,2350))
    return 3;
  if (alt <= Units::from(Units::ft,5000))
    return 4;
  if (alt <= Units::from(Units::ft,10000))
    return 5;
  if (alt <= Units::from(Units::ft,20000))
    return 6;
  if (alt <= Units::from(Units::ft,42000))
    return 7;
  // if (alt > Units::from("ft",42000))
  return 8;
}

/* RA Tau in seconds (-1 if N/A) */
double TCASTable::RA_TAU[7] = {-1,15,20,25,30,35,35};
double TCASTable::TA_TAU[7] = {20,25,30,40,45,48,48};

// C++ is crap.  These do not reliably initialize:
//  /* RA DMOD in internal units (-1 if N/A) */
//  double TCASTable::RA_DMOD[7] = {-1,
//    Units::from(Units::nmi,0.2),
//    Units::from(Units::nmi,0.35),
//    Units::from(Units::nmi,0.55),
//    Units::from(Units::nmi,0.8),
//    Units::from(Units::nmi,1.1),
//    Units::from(Units::nmi,1.1)};
//
//  double TCASTable::TA_DMOD[7] = {
//    Units::from(Units::nmi,0.3),
//    Units::from(Units::nmi,0.33),
//    Units::from(Units::nmi,0.48),
//    Units::from(Units::nmi,0.75),
//    Units::from(Units::nmi,1.0),
//    Units::from(Units::nmi,1.3),
//    Units::from(Units::nmi,1.3)};
//
//
//  /* RA ZTHR in internal units (-1 if N/A) */
//  double TCASTable::RA_ZTHR[7] = {-1,
//    Units::from(Units::ft,600),
//    Units::from(Units::ft,600),
//    Units::from(Units::ft,600),
//    Units::from(Units::ft,600),
//    Units::from(Units::ft,700),
//    Units::from(Units::ft,800)};
//
//  double TCASTable::TA_ZTHR[7] = {
//      Units::from(Units::ft,850),
//      Units::from(Units::ft,850),
//      Units::from(Units::ft,850),
//      Units::from(Units::ft,850),
//      Units::from(Units::ft,850),
//      Units::from(Units::ft,850),
//    Units::from(Units::ft,1200)};
//
//  /* RA HMD in internal units (-1 if N/A) */
//  double TCASTable::RA_HMD[7] = {-1,
//    Units::from(Units::nmi,0.4),
//    Units::from(Units::nmi,0.57),
//    Units::from(Units::nmi,0.74),
//    Units::from(Units::nmi,0.82),
//    Units::from(Units::nmi,0.98),
//    Units::from(Units::nmi,0.98)};

double TCASTable::RA_DMOD[7] = {-1,370.4,648.2,1018.6,1481.6,2037.2,2037.2};
double TCASTable::TA_DMOD[7] = {555.6,611.16,888.96,1389,1852,2407.6,2407.6};
double TCASTable::RA_ZTHR[7] = {-1,182.88,182.88,182.88,182.88,213.36,243.84};
double TCASTable::TA_ZTHR[7] = {259.08,259.08,259.08,259.08,259.08,259.08,365.76};
double TCASTable::RA_HMD[7] = {-1,370.332,648.0048,1018.6416,1481.6328,2036.9784,2036.9784};

void TCASTable::copyValues(const TCASTable& t) {
  for (int i=0; i < 7; i++) {
    TAU[i] = t.TAU[i];
    TCOA[i] = t.TCOA[i];
    DMOD[i] = t.DMOD[i];
    ZTHR[i] = t.ZTHR[i];
    HMD[i] = t.HMD[i];
  }
  HMDFilter = t.HMDFilter;
}

void TCASTable::setDefaultRAThresholds(bool ra) {
  for (int i=0; i < 7; i++) {
    TAU[i] = (ra ? RA_TAU[i] : TA_TAU[i]);
    TCOA[i] = TAU[i];
    DMOD[i] = (ra ? RA_DMOD[i] : TA_DMOD[i]);
    ZTHR[i] = (ra ? RA_ZTHR[i] : TA_ZTHR[i]);
    HMD[i] = (ra ? RA_HMD[i] : TA_DMOD[i]);
  }
}

double TCASTable::getTAU(int sl) const {
  if (2 <= sl && sl <= 8)
    return TAU[sl-2];
  return -1;
}

double TCASTable::getTCOA(int sl) const {
  if (2 <= sl && sl <= 8)
    return TCOA[sl-2];
  return -1;
}

double TCASTable::getDMOD(int sl) const {
  if (2 <= sl && sl <= 8)
    return DMOD[sl-2];
  return -1;
}

double TCASTable::getZTHR(int sl) const {
  if (2 <= sl && sl <= 8)
    return ZTHR[sl-2];
  return -1;
}

double TCASTable::getHMD(int sl) const {
  if (2 <= sl && sl <= 8)
    return HMD[sl-2];
  return -1;
}

double TCASTable::getTAU(int sl, const std::string& u) const {
  if (2 <= sl && sl <= 8)
    return Units::to(u,TAU[sl-2]);
  return -1;
}

double TCASTable::getTCOA(int sl, const std::string& u) const {
  if (2 <= sl && sl <= 8)
    return Units::to(u,TCOA[sl-2]);
  return -1;
}

double TCASTable::getDMOD(int sl, const std::string& u) const {
  if (2 <= sl && sl <= 8)
    return Units::to(u,DMOD[sl-2]);
  return -1;
}

double TCASTable::getZTHR(int sl, const std::string& u) const {
  if (2 <= sl && sl <= 8)
    return Units::to(u,ZTHR[sl-2]);
  return -1;
}

double TCASTable::getHMD(int sl, const std::string& u) const {
  if (2 <= sl && sl <= 8)
    return Units::to(u,HMD[sl-2]);
  return -1;
}

void TCASTable::setTAU(int sl, double val) {
  if (2 <= sl && sl <= 8)
    TAU[sl-2] = val;
}
void TCASTable::setTCOA(int sl, double val) {
  if (2 <= sl && sl <= 8)
    TCOA[sl-2] = val;
}

void TCASTable::setDMOD(int sl, double val) {
  if (2 <= sl && sl <= 8)
    DMOD[sl-2] = val;
}

void TCASTable::setZTHR(int sl, double val) {
  if (2 <= sl && sl <= 8)
    ZTHR[sl-2] = val;
}

void TCASTable::setHMD(int sl, double val) {
  if (2 <= sl && sl <= 8)
    HMD[sl-2] = val;
}

void TCASTable::setTAU(int sl, double val, const std::string& u) {
  if (2 <= sl && sl <= 8)
    TAU[sl-2] = Units::from(u,val);
}

void TCASTable::setTCOA(int sl, double val, const std::string& u) {
  if (2 <= sl && sl <= 8)
    TCOA[sl-2] = Units::from(u,val);
}

void TCASTable::setDMOD(int sl, double val, const std::string& u) {
  if (2 <= sl && sl <= 8)
    DMOD[sl-2] = Units::from(u,val);
}

void TCASTable::setZTHR(int sl, double val, const std::string& u) {
  if (2 <= sl && sl <= 8)
    ZTHR[sl-2] = Units::from(u,val);
}

void TCASTable::setHMD(int sl, double val, const std::string& u) {
  if (2 <= sl && sl <= 8)
    HMD[sl-2] = Units::from(u,val);
}

void TCASTable::setTAU(double val, const std::string& u) {
  for (int sl=0; sl < 7; ++sl) {
    TAU[sl] = Units::from(u,val);
  }
}

void TCASTable::setTCOA(double val, const std::string& u) {
  for (int sl=0; sl < 7; ++sl) {
    TCOA[sl] = Units::from(u,val);
  }
}

void TCASTable::setDMOD(double val, const std::string& u) {
  for (int sl=0; sl < 7; ++sl) {
    DMOD[sl] = Units::from(u,val);
  }
}

void TCASTable::setZTHR(double val, const std::string& u) {
  for (int sl=0; sl < 7; ++sl) {
    ZTHR[sl] = Units::from(u,val);
  }
}

void TCASTable::setHMD(double val, const std::string& u) {
  for (int sl=0; sl < 7; ++sl) {
    HMD[sl] = Units::from(u,val);
  }
}

void TCASTable::setHMDFilter(bool flag) {
  HMDFilter = flag;
}

bool TCASTable::getHMDFilter() const {
  return HMDFilter;
}


/** Return true if the values in the table correspond to the standard RA values */
bool TCASTable::isRAStandard() const {
  bool ra = true;
  for (int i=0; i < 7; i++) {
    ra = ra && TAU[i]==RA_TAU[i] && DMOD[i]==RA_DMOD[i] && TCOA[i] == TAU[i] && ZTHR[i]==RA_ZTHR[i] && HMD[i]==RA_HMD[i];
  }
  return ra;
}

/** Return true if the values in the table correspond to the standard TA values */
bool TCASTable::isTAStandard() const {
  bool ta = true;
  for (int i=0; i < 7; i++) {
    ta = ta && TAU[i]==TA_TAU[i] && DMOD[i]==TA_DMOD[i] && TCOA[i] == TAU[i] && ZTHR[i]==TA_ZTHR[i] && HMD[i]==TA_DMOD[i];
  }
  return ta;
}

ParameterData TCASTable::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;

}


void TCASTable::updateParameterData(ParameterData& p) const {
  p.setBool("TCAS_HMDilter", HMDFilter);
  for (int i = 0; i < 7; i++) {
    p.setInternal("TCAS_TAU_"+Fmi(i+2),TAU[i],"s",4);
  }
  for (int i = 0; i < 7; i++) {
    p.setInternal("TCAS_TCOA_"+Fmi(i+2),TCOA[i],"s",4);
  }
  for (int i = 0; i < 7; i++) {
    p.setInternal("TCAS_DMOD_"+Fmi(i+2),DMOD[i],"nmi",4);
  }
  for (int i = 0; i < 7; i++) {
    p.setInternal("TCAS_ZTHR_"+Fmi(i+2),ZTHR[i],"ft",4);
  }
  for (int i = 0; i < 7; i++) {
    p.setInternal("TCAS_HMD_"+Fmi(i+2),HMD[i],"ft",4);
  }
}


void TCASTable::setParameters(const ParameterData& p) {
  if (p.contains("TCAS_HMDilter")) {
    HMDFilter = p.getBool("TCAS_HMDilter");
  }
  for (int i = 0; i < 7; i++) {
    if (p.contains("TCAS_TAU_"+Fmi(i+2))) {
      TAU[i] = p.getValue("TCAS_TAU_"+Fmi(i+2));
    }
  }
  for (int i = 0; i < 7; i++) {
    if (p.contains("TCAS_TCOA_"+Fmi(i+2))) {
      TCOA[i] = p.getValue("TCAS_TCOA_"+Fmi(i+2));
    }
  }
  for (int i = 0; i < 7; i++) {
    if (p.contains("TCAS_DMOD_"+Fmi(i+2))) {
      DMOD[i] = p.getValue("TCAS_DMOD_"+Fmi(i+2));
    }
  }
  for (int i = 0; i < 7; i++) {
    if (p.contains("TCAS_ZTHR_"+Fmi(i+2))) {
      ZTHR[i] = p.getValue("TCAS_ZTHR_"+Fmi(i+2));
    }
  }
  for (int i = 0; i < 7; i++) {
    if (p.contains("TCAS_HMD_"+Fmi(i+2))) {
      HMD[i] = p.getValue("TCAS_HMD_"+Fmi(i+2));
    }
  }
}

bool TCASTable::equals(const TCASTable& t) const {
  for (int i=0; i < 7; i++) {
    if (TAU[i] != t.TAU[i]) return false;
    if (TCOA[i] != t.TCOA[i]) return false;
    if (DMOD[i] != t.DMOD[i]) return false;
    if (ZTHR[i] != t.ZTHR[i]) return false;
    if (HMD[i] != t.HMD[i]) return false;
  }
  return HMDFilter == t.HMDFilter;
}

std::string array7_units(const std::string& units, double const v[]) {
  std::string s="";
  bool comma = false;
  for (int i=0; i<7; ++i) {
    if (comma) {
      s+=", ";
    } else {
      comma = true;
    }
    s+=Units::str(units,v[i]);
  }
  return s;
}

std::string TCASTable::toString() const {
  std::string s = "HMDFilter: "+Fmb(HMDFilter);
  if (isRAStandard()) s = s+"; (RA vals) ";
  else if (isTAStandard()) s= s+"; (TA vals) ";
  s= s+"; TAU: "+array7_units("s",TAU)+"; TCOA: "+array7_units("s",TCOA)+
      "; DMOD: "+array7_units("NM",DMOD)+"; ZTHR: "+array7_units("ft",ZTHR)+
      "; HMD: "+array7_units("ft",HMD);
  return s;
}

std::string TCASTable::toPVS(int prec) const {
  std::string s = "(# ";
  s += "TAU := (: "+Fm1(TAU[0]);
  for (int i=1; i < 7; i++) {
    s += ", "+Fm1(TAU[i]);
  }
  s += " :), TCOA := (: "+Fm1(TCOA[0]);
  for (int i=1; i < 7; i++) {
    s += ", "+Fm1(TCOA[i]);
  }
  s += " :), DMOD := (: "+FmPrecision(DMOD[0],prec);
  for (int i=1; i < 7; i++) {
    s += ", "+FmPrecision(DMOD[i],prec);
  }
  s += " :), ZTHR := (: "+FmPrecision(ZTHR[0],prec);
  for (int i=1; i < 7; i++) {
    s += ", "+FmPrecision(ZTHR[i],prec);
  }
  s += " :), HMD := (: "+FmPrecision(HMD[0],prec);
  for (int i=1; i < 7; i++) {
    s += ", "+FmPrecision(HMD[i],prec);
  }
  s += " :), HMDFilter := ";
  s += (HMDFilter ? "TRUE" : "FALSE");
  return s + " #)";
}


bool TCASTable::contains(const TCASTable& tab) const {
  for (int i=0; i < 7; i++) {
    if (TAU[i] < tab.TAU[i] || TCOA[i] < tab.TCOA[i] || DMOD[i] < tab.DMOD[i] || ZTHR[i] < tab.ZTHR[i] || HMD[i] < tab.HMD[i]) return false;
  }
  return !HMDFilter || tab.HMDFilter;
}

}
