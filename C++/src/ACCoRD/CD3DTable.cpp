/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "Units.h"
#include "CD3DTable.h"
#include "ParameterData.h"
#include "format.h"

namespace larcfm {

CD3DTable::CD3DTable() {
  D = Units::from("nmi", 5.0);
  H = Units::from("ft", 1000.0);
  units_["D"] = "nmi";
  units_["H"] = "ft";
}

CD3DTable::CD3DTable(double d, double h) {
  D = std::abs(d);
  H = std::abs(h);
  units_["D"] = "m";
  units_["H"] = "m";
}

CD3DTable::CD3DTable(double d, const std::string& dunit, double h, const std::string& hunit) {
  D = Units::from(dunit, std::abs(d));
  H = Units::from(hunit, std::abs(h));
  units_["D"] = dunit;
  units_["H"] = hunit;
}

/**
 * Copy constructor -- returns a fresh copy.
 */
CD3DTable::CD3DTable(const CD3DTable& tab) {
  copyValues(tab);
}

CD3DTable CD3DTable::copy() const {
  CD3DTable tab;
  tab.D = D;
  tab.H = H;
  tab.units_ = units_;
  return tab;
}

void CD3DTable::copyValues(const CD3DTable& tab) {
  D = tab.D;
  H = tab.H;
  units_ = tab.units_;
}

double CD3DTable::getHorizontalSeparation() const {
  return D;
}

void CD3DTable::setHorizontalSeparation(double d) {
  D = std::abs(d);
}

double CD3DTable::getVerticalSeparation() const {
  return H;
}

void CD3DTable::setVerticalSeparation(double h) {
  H = std::abs(h);
}

double CD3DTable::getHorizontalSeparation(const std::string& u) const {
  return Units::to(u, D);
}

void CD3DTable::setHorizontalSeparation(double d, const std::string& u) {
  setHorizontalSeparation(Units::from(u,d));
  units_["D"] = u;
}

double CD3DTable::getVerticalSeparation(const std::string& u) const {
  return Units::to(u, H);
}

void CD3DTable::setVerticalSeparation(double h, const std::string& u) {
  setVerticalSeparation(Units::from(u,h));
  units_["H"] = u;
}

ParameterData CD3DTable::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void CD3DTable::updateParameterData(ParameterData& p) const {
  p.setInternal("D",D,getUnits("D"));
  p.setInternal("H",H,getUnits("H"));
}

void CD3DTable::setParameters(const ParameterData& p) {
  if (p.contains("D")) {
    setHorizontalSeparation(p.getValue("D"));
    units_["D"] = p.getUnit("D");
  }
  if (p.contains("H")) {
    setVerticalSeparation(p.getValue("H"));
    units_["H"] = p.getUnit("H");
  }
}

std::string CD3DTable::getUnits(const std::string& key) const {
  std::map<std::string,std::string>::const_iterator got = units_.find(key);
  if (got == units_.end()) {
    return "unspecified";
  }
  return got->second;
}

bool CD3DTable::equals(const CD3DTable& t2) const {
  return D == t2.D && H == t2.H;
}

std::string CD3DTable::toString() const {
  return "D = "+Units::str(getUnits("D"),D)+", H = "+Units::str(getUnits("H"),H);
}

std::string CD3DTable::toPVS(int prec) const {
  return "(# D:= "+FmPrecision(D,prec)+", H:= "+FmPrecision(H,prec)+" #)";
}

}
