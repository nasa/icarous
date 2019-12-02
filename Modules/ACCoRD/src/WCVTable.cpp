/*
 * Copyright (c) 2012-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Units.h"
#include "WCVTable.h"
#include "format.h"
#include "ParameterData.h"

namespace larcfm {

WCVTable::WCVTable() {
  DTHR = Units::from("nmi",0.66);
  ZTHR = Units::from("ft",450);
  TTHR = 35; // [s]
  TCOA = 0;  // [s]
  units_["WCV_DTHR"] = "nmi";
  units_["WCV_ZTHR"] = "ft";
  units_["WCV_TTHR"] = "s";
  units_["WCV_TCOA"] = "s";
}

WCVTable::WCVTable(double dthr, double zthr, double tthr, double tcoa) {
  DTHR = std::abs(dthr);
  ZTHR = std::abs(zthr);
  TTHR = std::abs(tthr);
  TCOA = std::abs(tcoa);
  units_["WCV_DTHR"] = "m";
  units_["WCV_ZTHR"] = "m";
  units_["WCV_TTHR"] = "s";
  units_["WCV_TCOA"] = "s";
}

/**
 * Table containing specified values (specified units)
 */
WCVTable::WCVTable(double dthr, const std::string& udthr, double zthr, const std::string& uzthr,
    double tthr, const std::string& utthr, double tcoa, const std::string& utcoa) {
  DTHR = Units::from(udthr,std::abs(dthr));
  ZTHR = Units::from(uzthr,std::abs(zthr));
  TTHR = Units::from(utthr,std::abs(tthr));
  TCOA = Units::from(utcoa,std::abs(tcoa));
  units_["WCV_DTHR"] = udthr;
  units_["WCV_ZTHR"] = uzthr;
  units_["WCV_TTHR"] = utthr;
  units_["WCV_TCOA"] = utcoa;
}

WCVTable::~WCVTable() { }

double WCVTable::getDTHR() const {
  return DTHR;
}
double WCVTable::getDTHR(const std::string& u) const {
  return Units::to(u,DTHR);
}

double WCVTable::getZTHR() const {
  return ZTHR;
}
double WCVTable::getZTHR(const std::string& u) const {
  return Units::to(u,ZTHR);
}

double WCVTable::getTTHR() const {
  return TTHR;
}
double WCVTable::getTTHR(const std::string& u) const {
  return Units::to(u,TTHR);
}

double WCVTable::getTCOA() const {
  return TCOA;
}
double WCVTable::getTCOA(const std::string& u) const {
  return Units::to(u,TCOA);
}

void WCVTable::setDTHR(double val) {
  DTHR = std::abs(val);
}

void WCVTable::setDTHR(double val, const std::string& u) {
  setDTHR(Units::from(u,val));
  units_["WCV_DTHR"] = u;
}

void WCVTable::setZTHR(double val) {
  ZTHR = std::abs(val);
}
void WCVTable::setZTHR(double val, const std::string& u) {
  setZTHR(Units::from(u,val));
  units_["WCV_ZTHR"] = u;
}

void WCVTable::setTTHR(double val) {
  TTHR =std::abs(val);
}
void WCVTable::setTTHR(double val, const std::string& u) {
  setTTHR(Units::from(u,val));
  units_["WCV_TTHR"] = u;
}

void WCVTable::setTCOA(double val) {
  TCOA = std::abs(val);
}
void WCVTable::setTCOA(double val, const std::string& u) {
  setTCOA(Units::from(u,val));
  units_["WCV_TCOA"] = u;
}

ParameterData WCVTable::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void WCVTable::updateParameterData(ParameterData& p) const {
  p.setInternal("WCV_DTHR",DTHR,getUnits("WCV_DTHR"));
  p.setInternal("WCV_ZTHR",ZTHR,getUnits("WCV_ZTHR"));
  p.setInternal("WCV_TTHR",TTHR,getUnits("WCV_TTHR"));
  p.setInternal("WCV_TCOA",TCOA,getUnits("WCV_TCOA"));
}

void WCVTable::setParameters(const ParameterData& p) {
  if (p.contains("WCV_DTHR")) {
    setDTHR(p.getValue("WCV_DTHR"));
    units_["WCV_DTHR"] = p.getUnit("WCV_DTHR");
  }
  if (p.contains("WCV_ZTHR")) {
    setZTHR(p.getValue("WCV_ZTHR"));
    units_["WCV_ZTHR"] = p.getUnit("WCV_ZTHR");
  }
  if (p.contains("WCV_TTHR")) {
    setTTHR(p.getValue("WCV_TTHR"));
    units_["WCV_TTHR"] = p.getUnit("WCV_TTHR");
  }
  if (p.contains("WCV_TCOA")) {
    setTCOA(p.getValue("WCV_TCOA"));
    units_["WCV_TCOA"] = p.getUnit("WCV_TCOA");
  }
}

std::string WCVTable::getUnits(const std::string& key) const {
  std::map<std::string,std::string>::const_iterator got = units_.find(key);
  if (got == units_.end()) {
    return "unspecified";
  }
  return got->second;
}

bool WCVTable::equals(const WCVTable& t) const {
  if (DTHR != t.DTHR) return false;
  if (ZTHR != t.ZTHR) return false;
  if (TTHR != t.TTHR) return false;
  if (TCOA != t.TCOA) return false;
  return true;
}

std::string WCVTable::toString() const {
  return "WCV_DTHR = "+Units::str(getUnits("WCV_DTHR"),DTHR)+
      ", WCV_ZTHR = "+Units::str(getUnits("WCV_ZTHR"),ZTHR)+
      ", WCV_TTHR = "+Units::str(getUnits("WCV_TTHR"),TTHR)+
      ", WCV_TCOA = "+Units::str(getUnits("WCV_TCOA"),TCOA);
}

std::string WCVTable::toPVS() const {
  return "(# DTHR := "+FmPrecision(DTHR)+", ZTHR := "+FmPrecision(ZTHR)+
      ", TTHR := "+FmPrecision(TTHR)+", TCOA := "+FmPrecision(TCOA)+" #)";
}

bool WCVTable::contains(const WCVTable& tab) const {
  return DTHR >= tab.DTHR && ZTHR >= tab.ZTHR && TTHR >= tab.TTHR && TCOA >= tab.TCOA;
}

}
