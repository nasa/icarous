/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "WCV_tvar.h"
#include "Vect3.h"
#include "Velocity.h"
#include "WCV_Vertical.h"
#include "Horizontal.h"
#include "WCVTable.h"
#include "ParameterData.h"
#include "ConflictData.h"
#include "LossData.h"
#include "format.h"
#include "string_util.h"
#include <cfloat>

namespace larcfm {

WCV_tvar::~WCV_tvar() {
  delete wcv_vertical;
}

/**
 * Sets the internal table to be a copy of the supplied one.
 **/
void WCV_tvar::setWCVTable(const WCVTable& tab) {
  table.copyValues(tab);
}

double WCV_tvar::getDTHR() const {
  return table.getDTHR();
}
double WCV_tvar::getDTHR(const std::string& u) const {
  return table.getDTHR(u);
}

double WCV_tvar::getZTHR() const {
  return table.getZTHR();
}
double WCV_tvar::getZTHR(const std::string& u) const {
  return table.getZTHR(u);
}

double WCV_tvar::getTTHR() const {
  return table.getTTHR();
}
double WCV_tvar::getTTHR(const std::string& u) const {
  return table.getTTHR(u);
}

double WCV_tvar::getTCOA() const {
  return table.getTCOA();
}
double WCV_tvar::getTCOA(const std::string& u) const {
  return table.getTCOA(u);
}

void WCV_tvar::setDTHR(double val) {
  table.setDTHR(val);
}
void WCV_tvar::setDTHR(double val, const std::string& u) {
  table.setDTHR(val, u);
}

void WCV_tvar::setZTHR(double val) {
  table.setZTHR(val);
}
void WCV_tvar::setZTHR(double val, const std::string& u) {
  table.setZTHR(val,u);
}

void WCV_tvar::setTTHR(double val) {
  table.setTTHR(val);
}
void WCV_tvar::setTTHR(double val, const std::string& u) {
  table.setTTHR(val,u);
}

void WCV_tvar::setTCOA(double val) {
  table.setTCOA(val);
}
void WCV_tvar::setTCOA(double val, const std::string& u) {
  table.setTCOA(val,u);
}

bool WCV_tvar::horizontal_WCV(const Vect2& s, const Vect2& v) const {
  if (s.norm() <= table.getDTHR()) return true;
  if (Horizontal::dcpa(s,v) <= table.getDTHR()) {
    double tvar = horizontal_tvar(s,v);
    return 0  <= tvar && tvar <= table.getTTHR();
  }
  return false;
}

bool WCV_tvar::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  Vect2 so2 = so.vect2();
  Vect2 si2 = si.vect2();
  Vect2 s2 = so2.Sub(si2);
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  Vect2 v2 = vo2.Sub(vi2);
  return horizontal_WCV(s2,v2) &&
      wcv_vertical->vertical_WCV(table.getZTHR(),table.getTCOA(),so.z-si.z,vo.z-vi.z);
}

bool WCV_tvar::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return WCV3D(so,vo,si,vi,B,T).conflict();
}


ConflictData WCV_tvar::conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  LossData ret = WCV3D(so,vo,si,vi,B,T);
  double t_tca = (ret.getTimeIn() + ret.getTimeOut())/2;
  double dist_tca = so.linear(vo, t_tca).Sub(si.linear(vi, t_tca)).cyl_norm(table.getDTHR(),table.getZTHR());
  return ConflictData(ret, t_tca,dist_tca,so.Sub(si),vo.Sub(vi));
}

LossData WCV_tvar::WCV3D(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return WCV_interval(so,vo,si,vi,B,T);
}

// Assumes 0 <= B < T
LossData WCV_tvar::WCV_interval(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
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

  Interval ii = wcv_vertical->vertical_WCV_interval(table.getZTHR(),table.getTCOA(),B,T,sz,vz);

  if (ii.low > ii.up) {
    return LossData(time_in,time_out);
  }
  Vect2 step = v2.ScalAdd(ii.low,s2);
  if (Util::almost_equals(ii.low,ii.up)) { // [CAM] Changed from == to almost_equals to mitigate numerical problems
    if (horizontal_WCV(step,v2)) {
      time_in = ii.low;
      time_out = ii.up;
    }
    return LossData(time_in,time_out);
  }
  LossData ld = horizontal_WCV_interval(ii.up-ii.low,step,v2);
  time_in = ld.getTimeIn() + ii.low;
  time_out = ld.getTimeOut() + ii.low;
  return LossData(time_in,time_out);
}

bool WCV_tvar::containsTable(WCV_tvar* wcv) const {
  return table.contains(wcv->table);
}

std::string WCV_tvar::toString() const {
  return (id == "" ? "" : id+" = ")+getSimpleClassName()+": {"+table.toString()+"}";
}

std::string WCV_tvar::toPVS(int prec) const {
  return table.toPVS(prec);
}

ParameterData WCV_tvar::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void WCV_tvar::updateParameterData(ParameterData& p) const {
  table.updateParameterData(p);
  p.set("id",id);
}

void WCV_tvar::setParameters(const ParameterData& p) {
  table.setParameters(p);
  if (p.contains("id")) {
    id = p.getString("id");
  }
}

std::string WCV_tvar::getIdentifier() const {
  return id;
}

void WCV_tvar::setIdentifier(const std::string& s) {
  id = s;
}

bool WCV_tvar::equals(Detection3D *obj) const {
  if (!larcfm::equals(getCanonicalClassName(), obj->getCanonicalClassName())) return false;
  if (!table.equals(((WCV_tvar*)obj)->table)) return false;
  if (!larcfm::equals(id, ((WCV_tvar*)obj)->id)) return false;
  return true;
}

}

