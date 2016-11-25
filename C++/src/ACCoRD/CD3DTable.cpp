/*
 * Copyright (c) 2014-2016 United States Government as represented by
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
  H = Units::from("ft",  1000.0);
}

CD3DTable::CD3DTable(double d, double h) {
  D = d;
  H = h;
}

CD3DTable::CD3DTable(double d, const std::string& dunit, double h, const std::string& hunit) {
  D = Units::from(dunit, d);
  H = Units::from(hunit, h);
}

/**
 * Copy constructor -- returns a fresh copy.
 */
CD3DTable::CD3DTable(const CD3DTable& tab) {
  D = tab.D;
  H = tab.H;
}

void CD3DTable::copyValues(const CD3DTable& tab) {
  D = tab.D;
  H = tab.H;
}

double CD3DTable::getHorizontalSeparation() const {
  return D;
}

void CD3DTable::setHorizontalSeparation(double d) {
  D = d;
}

double CD3DTable::getVerticalSeparation() const {
  return H;
}

void CD3DTable::setVerticalSeparation(double h) {
  H = h;
}

double CD3DTable::getHorizontalSeparation(const std::string& unit) const {
  return Units::to(unit, D);
}

void CD3DTable::setHorizontalSeparation(double d, const std::string& unit) {
  D = Units::from(unit, d);
}

double CD3DTable::getVerticalSeparation(const std::string& unit) const {
  return Units::to(unit, H);
}

void CD3DTable::setVerticalSeparation(double h, const std::string& unit) {
  H = Units::from(unit, h);
}


ParameterData CD3DTable::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}


void CD3DTable::updateParameterData(ParameterData& p) const {
  p.setInternal("D",D,"nmi",4);
  p.setInternal("H",H,"ft",4);
}

void CD3DTable::setParameters(const ParameterData& p) {
  if (p.contains("D")) {
    D = p.getValue("D");
  }
  if (p.contains("H")) {
    H = p.getValue("H");
  }
}

bool CD3DTable::equals(const CD3DTable& t2) const {
  return D == t2.D && H == t2.H;
}

std::string CD3DTable::toString() const {
  return "D: "+Units::str("NM",D)+"; H: "+Units::str("ft",H);
}

std::string CD3DTable::toPVS(int prec) const {
  return "(# D:= "+FmPrecision(D,prec)+", H:= "+FmPrecision(H,prec)+" #)";
}

}
