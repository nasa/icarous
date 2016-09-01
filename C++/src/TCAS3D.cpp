/*
 * Copyright (c) 2012-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "TCAS3D.h"
#include "TCAS2D.h"
#include "CD2D.h"
#include "CD3D.h"
#include "Vect3.h"
#include "Velocity.h"
#include "TCASTable.h"
#include "Util.h"
#include "Vertical.h"
#include "ConflictData.h"
#include <cfloat>

namespace larcfm {

TCAS3D::TCAS3D() {
  table = TCASTable(true);
  id = "";
}

TCAS3D::TCAS3D(const TCASTable& tab) {
  table.copyValues(tab);
  id = "";
}

TCASTable TCAS3D::getTCASTable() {
  return table;
}

void TCAS3D::setTCASTable(const TCASTable& tab) {
  table.copyValues(tab);
}

bool TCAS3D::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return TCASII_RA(so, vo, si, vi);
}

bool TCAS3D::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return RA3D(so,vo,si,vi,B,T).conflict();
}

ConflictData TCAS3D::conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return RA3D(so,vo,si,vi,B,T);
}

double time_coalt(double sz, double voz, double viz, double h) {
  if (std::abs(sz) <= h)
    return 0;
  if (Util::almost_equals(voz, viz))
    return -1;
  return -sz / (voz-viz);
}

bool vertical_RA(double sz, double vz, double ZTHR, double TAU) {
  if (std::abs(sz) <= ZTHR) return true;
  if (Util::almost_equals(vz,0)) return false; // [CAM] Changed from == to almost_equals to mitigate numerical problems
  double tcoa = Vertical::time_coalt(sz,vz);
  return 0 <= tcoa && tcoa <= TAU;
}

bool cd2d_TCAS_after(double HMD, Vect2 s, Vect2 vo, Vect2 vi, double t) {
  Vect2 v = vo.Sub(vi);
  return
      (vo.almostEquals(vi) && s.sqv() <= Util::sq(HMD)) ||
      (v.sqv() > 0 && Horizontal::Delta(s,v,HMD) >= 0 &&
          Horizontal::Theta_D(s,v,1,HMD) >= t);
}

bool cd2d_TCAS(double HMD, Vect2 s, Vect2 vo, Vect2 vi) {
  return cd2d_TCAS_after(HMD,s,vo,vi,0);
}

// if true, then ownship has a TCAS resolution advisory at current time
bool TCAS3D::TCASII_RA(const Vect3& so, const Vect3& vo, const Vect3& si, const Vect3& vi) const {

  Vect2 so2 = so.vect2();
  Vect2 si2 = si.vect2();
  Vect2 s2 = so2.Sub(si2);
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  Vect2 v2 = vo2.Sub(vi2);
  int sl = TCASTable::getSensitivityLevel(so.z);
  bool usehmdf = table.getHMDFilter();
  double TAU  = table.getTAU(sl);
  double DMOD = table.getDMOD(sl);
  double HMD  = table.getHMD(sl);
  double ZTHR = table.getZTHR(sl);

  return (!usehmdf || cd2d_TCAS(HMD,s2,vo2,vi2)) &&
      TCAS2D::horizontal_RA(DMOD,TAU,s2,v2) &&
      vertical_RA(so.z-si.z,vo.z-vi.z,ZTHR,TAU);
}

// if true, within lookahead time interval [B,T], the ownship has a TCAS resolution advisory (effectively conflict detection)
// B must be non-negative and T > B

ConflictData TCAS3D::RA3D(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return RA3D_interval(so,vo,si,vi,B,T);
  //  return time_in <= time_out;
}

// Assumes 0 <= B < T
ConflictData TCAS3D::RA3D_interval(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
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
  int sl = TCASTable::getSensitivityLevel(so.z);
  bool usehmdf = table.getHMDFilter();
  double TAU  = table.getTAU(sl);
  double DMOD = table.getDMOD(sl);
  double HMD  = table.getHMD(sl);
  double ZTHR = table.getZTHR(sl);

  if (usehmdf && !cd2d_TCAS_after(HMD,s2,vo2,vi2,B)) {
    time_mintau = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,vi);
  }
  double sz = so.z-si.z;
  if (Util::almost_equals(vo.z, vi.z) && std::abs(sz) > ZTHR) {
    time_mintau = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
  }
  double nzvz = vo.z-vi.z;
  double centry = B;
  double cexit  = T;
  if (!Util::almost_equals(vo.z, vi.z)) {
    double act_H = std::max(ZTHR,std::abs(nzvz)*TAU);
    centry = Vertical::Theta_H(sz,nzvz,-1,act_H);
    cexit = Vertical::Theta_H(sz,nzvz,1,ZTHR);
  }
  Vect2 ventry = v2.ScalAdd(centry,s2);
  bool exit_at_centry = ventry.dot(v2) >= 0;
  bool los_at_centry = ventry.sqv() <= Util::sq(HMD);
  if (cexit < B || T < centry) {
    time_mintau = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
  }
  double tin = std::max(B,centry);
  double tout = std::min(T,cexit);
  TCAS2D tcas2d;
  tcas2d.RA2D_interval(DMOD,TAU,tin,tout,s2,vo2,vi2);
  double RAin2D = tcas2d.t_in;
  double RAout2D = tcas2d.t_out;
  double RAin2D_lookahead = std::max(tin,std::min(tout,RAin2D));
  double RAout2D_lookahead = std::max(tin,std::min(tout,RAout2D));
  if (RAin2D > RAout2D || RAout2D<tin || RAin2D > tout ||
      (usehmdf && HMD < DMOD && exit_at_centry && !los_at_centry)) {
    time_mintau = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
  }
  if (usehmdf && HMD < DMOD) {
    double exitTheta = T;
    if (v2.sqv() > 0)
      exitTheta = std::max(B,std::min(Horizontal::Theta_D(s2,v2,1,HMD),T));
    double minRAoutTheta = std::min(RAout2D_lookahead,exitTheta);
    time_in = RAin2D_lookahead;
    time_out = minRAoutTheta;
    if (RAin2D_lookahead <= minRAoutTheta) {
      time_mintau = TCAS2D::time_of_min_tau(DMOD,RAin2D_lookahead,minRAoutTheta,s2,v2);
      dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
      return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
    }
    time_mintau = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
    return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
  }
  time_in = RAin2D_lookahead;
  time_out = RAout2D_lookahead;
  time_mintau = TCAS2D::time_of_min_tau(DMOD,RAin2D_lookahead,RAout2D_lookahead,s2,v2);
  dist_mintau = so.linear(vo, time_mintau).Sub(si.linear(vi, time_mintau)).cyl_norm(table.getDMOD(8), table.getZTHR(8));
  return ConflictData(time_in,time_out,time_mintau,dist_mintau,s,v);
}

// pointer to new instance of this object
TCAS3D* TCAS3D::make() const {
  return new TCAS3D();
}

// pointer to deep copy of current object
TCAS3D* TCAS3D::copy() const {
  TCAS3D* cd = new TCAS3D();
  cd->table.copyValues(table);
  cd->id = id;
  return cd;
}

ParameterData TCAS3D::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void TCAS3D::updateParameterData(ParameterData& p) const {
  table.updateParameterData(p);
  p.set("id",id);
}

void TCAS3D::setParameters(const ParameterData& p) {
  table.setParameters(p);
  if (p.contains("id")) {
    id = p.getString("id");
  }
}

std::string TCAS3D::getIdentifier() const {
  return id;
}

void TCAS3D::setIdentifier(const std::string& s) {
  id = s;
}

bool TCAS3D::equals(Detection3D* d) const {
  if (!larcfm::equals(getCanonicalClassName(), d->getCanonicalClassName())) return false;
  if (!larcfm::equals(id, d->getIdentifier())) return false;
  if (!table.equals(((TCAS3D*)d)->table)) return false;
  return true;
}

std::string TCAS3D::getSimpleClassName() const {
  return "TCAS3D";
}

std::string TCAS3D::toString() const {
  return (id == "" ? "" : id+" = ")+getSimpleClassName()+": {"+table.toString()+"}";
}

std::string TCAS3D::toPVS(int prec) const {
  return table.toPVS(prec);
}

bool TCAS3D::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    TCAS3D* d = (TCAS3D*)cd;
    return table.contains(d->table);
  }
  return false;
}

}
