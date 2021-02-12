/*
 * Copyright (c) 2012-2020 United States Government as represented by
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
#include "Triple.h"
#include "CDCylinder.h"
#include "WCV_TAUMOD.h"
#include <cfloat>
#include <limits>
#include <cmath>

namespace larcfm {

TCAS3D::TCAS3D() {
  table_ = TCASTable::make_TCASII_Table(true);
  id = "";
}

TCAS3D::TCAS3D(const TCASTable& tab) {
  table_ = tab;
  id = "";
}

/**
 * @return one static TCAS3D
 */
const TCAS3D& TCAS3D::A_TCAS3D() {
  static TCAS3D dwc;
  return dwc;
}

/**
 * @return one static TCASII_RA
 */
const TCAS3D& TCAS3D::TCASII_RA() {
  static TCAS3D ra = make_TCASII_RA();
  return ra;
}

/**
 * @return one static TCASII_TA
 */
const TCAS3D& TCAS3D::TCASII_TA() {
  static TCAS3D ta = make_TCASII_TA();
  return ta;
}

/** Make TCAS3D object with empty Table **/
TCAS3D TCAS3D::make_Empty() {
  TCAS3D tcas3d;
  tcas3d.table_.clear();
  return tcas3d;
}

/** Make TCAS3D object with an RA Table **/
TCAS3D TCAS3D::make_TCASII_RA() {
  TCAS3D tcas3d;
  return tcas3d;
}

/** Make TCAS3D objec with a TA Table **/
TCAS3D TCAS3D::make_TCASII_TA() {
  TCAS3D tcas3d;
  tcas3d.table_.setDefaultTCASIIThresholds(false);
  return tcas3d;
}

TCASTable& TCAS3D::getTCASTable() {
  return table_;
}

/**
 * Set table to TCASII Thresholds (RA Table when ra is true, TA Table when ra is false)
 */
void TCAS3D::setDefaultTCASIIThresholds(bool ra) {
  table_.setDefaultTCASIIThresholds(ra);
}

ConflictData TCAS3D::conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return RA3D(so,vo,si,vi,B,T);
}

// pointer to new instance of this object
TCAS3D* TCAS3D::make() const {
  return new TCAS3D();
}

// pointer to deep copy of current object
TCAS3D* TCAS3D::copy() const {
  TCAS3D* cd = new TCAS3D();
  cd->table_ = table_;
  cd->id = id;
  return cd;
}

bool TCAS3D::vertical_RA(double sz, double vz, double ZTHR, double TCOA) {
  if (std::abs(sz) <= ZTHR) return true;
  if (Util::almost_equals(vz,0)) return false; // [CAM] Changed from == to almost_equals to mitigate numerical problems
  double tcoa = Vertical::time_coalt(sz,vz);
  return 0 <= tcoa && tcoa <= TCOA;
}

bool TCAS3D::cd2d_TCAS_after(double HMD, Vect2 s, Vect2 vo, Vect2 vi, double t) {
  Vect2 v = vo.Sub(vi);
  return
      (vo.almostEquals(vi) && s.sqv() <= Util::sq(HMD)) ||
      (v.sqv() > 0 && Horizontal::Delta(s,v,HMD) >= 0 &&
          Horizontal::Theta_D(s,v,1,HMD) >= t);
}

bool TCAS3D::cd2d_TCAS(double HMD, Vect2 s, Vect2 vo, Vect2 vi) {
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
  int sl = table_.getSensitivityLevel(so.z);
  bool usehmdf = table_.getHMDFilter();
  double TAU  = table_.getTAU(sl);
  double TCOA = table_.getTCOA(sl);
  double DMOD = table_.getDMOD(sl);
  double HMD  = table_.getHMD(sl);
  double ZTHR = table_.getZTHR(sl);

  return (!usehmdf || cd2d_TCAS(HMD,s2,vo2,vi2)) &&
      TCAS2D::horizontal_RA(DMOD,TAU,s2,v2) &&
      vertical_RA(so.z-si.z,vo.z-vi.z,ZTHR,TCOA);
}

// if true, within lookahead time interval [B,T], the ownship has a TCAS resolution advisory (effectively conflict detection)
// B must be non-negative and T > B

ConflictData TCAS3D::RA3D(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {

  Vect3 s = so.Sub(si);
  Velocity v = Velocity(vo.Sub(vi));
  Vect2 so2 = so.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 si2 = si.vect2();
  Vect2 vi2 = vi.vect2();

  int max_sl = table_.getMaxSensitivityLevel();
  double DMOD_max = table_.getDMOD(max_sl);
  double ZTHR_max = table_.getZTHR(max_sl);

  double tin = INFINITY;
  double tout = -INFINITY;
  double tmin = INFINITY;
  int sl_first = table_.getSensitivityLevel(so.z+B*vo.z);
  int sl_last = table_.getSensitivityLevel(so.z+T*vo.z);
  if (sl_first == sl_last || Util::almost_equals(vo.z,0.0)) {
    Triple<double,double,double> ra3dint = RA3D_interval(sl_first,so2,so.z,vo2,vo.z,si2,si.z,vi2,vi.z,B,T);
    tin = ra3dint.first;
    tout = ra3dint.second;
    tmin = ra3dint.third;
  } else {
    int sl = sl_first;
    for (double t_B = B; t_B < T; sl = sl_first < sl_last ? sl+1 : sl-1) {
      if (table_.isValidSensitivityLevel(sl)) {
        double level = sl_first < sl_last ? table_.getLevelAltitudeUpperBound(sl) :table_.getLevelAltitudeLowerBound(sl);
        double t_level = !ISFINITE(level) ? INFINITY :(level-so.z)/vo.z;
        Triple<double,double,double> ra3dint = RA3D_interval(sl,so2,so.z,vo2,vo.z,si2,si.z,vi2,vi.z,t_B,Util::min(t_level,T));
        if (Util::almost_less(ra3dint.first,ra3dint.second)) {
          tin = Util::min(tin,ra3dint.first);
          tout = Util::max(tout,ra3dint.second);
        }
        tmin = Util::min(tmin,ra3dint.third);
        t_B = t_level;
        if (sl == sl_last) {
          break;
        }
      }
    }
  }
  double dmin = s.linear(v, tmin).cyl_norm(DMOD_max, ZTHR_max);
  return  ConflictData(tin,tout,tmin,dmin,s,v);}

// Assumes 0 <= B < T
Triple<double,double,double> TCAS3D::RA3D_interval(int sl, const Vect2& so2, double soz, const Vect2& vo2, double voz,
    const Vect2& si2, double siz, const Vect2& vi2, double viz, double B, double T) const {
  double time_in_     = T;
  double time_out_    = B;
  double time_mintau_ = INFINITY;
  Vect2 s2 = so2.Sub(si2);
  Vect2 v2 = vo2.Sub(vi2);
  double sz = soz-siz;
  double vz = voz-viz;
  bool usehmdf = table_.getHMDFilter();
  double TAU  = table_.getTAU(sl);
  double TCOA = table_.getTCOA(sl);
  double DMOD = table_.getDMOD(sl);
  double HMD  = table_.getHMD(sl);
  double ZTHR = table_.getZTHR(sl);

  if (usehmdf && !cd2d_TCAS_after(HMD,s2,vo2,vi2,B)) {
    time_mintau_ = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
  } else {
    if (Util::almost_equals(voz, viz) && std::abs(sz) > ZTHR) {
      time_mintau_ = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
    } else {
      double tentry = B;
      double texit  = T;
      if (!Util::almost_equals(voz, viz)) {
        double act_H = Util::max(ZTHR,std::abs(vz)*TCOA);
        tentry = Vertical::Theta_H(sz,vz,-1,act_H);
        texit = Vertical::Theta_H(sz,vz,1,ZTHR);
      }
      Vect2 ventry = v2.ScalAdd(tentry,s2);
      bool exit_at_centry = ventry.dot(v2) >= 0;
      bool los_at_centry = ventry.sqv() <= Util::sq(HMD);
      if (texit < B || T < tentry) {
        time_mintau_ = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
      } else {
        double tin = Util::max(B,tentry);
        double tout = Util::min(T,texit);
        TCAS2D tcas2d;
        tcas2d.RA2D_interval(DMOD,TAU,tin,tout,s2,vo2,vi2);
        double RAin2D = tcas2d.t_in;
        double RAout2D = tcas2d.t_out;
        double RAin2D_lookahead = Util::max(tin,Util::min(tout,RAin2D));
        double RAout2D_lookahead = Util::max(tin,Util::min(tout,RAout2D));
        if (RAin2D > RAout2D || RAout2D<tin || RAin2D > tout ||
            (usehmdf && HMD < DMOD && exit_at_centry && !los_at_centry)) {
          time_mintau_ = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
        } else {
          if (usehmdf && HMD < DMOD) {
            double exitTheta = T;
            if (v2.sqv() > 0)
              exitTheta = Util::max(B,Util::min(Horizontal::Theta_D(s2,v2,1,HMD),T));
            double minRAoutTheta = Util::min(RAout2D_lookahead,exitTheta);
            time_in_ = RAin2D_lookahead;
            time_out_ = minRAoutTheta;
            if (RAin2D_lookahead <= minRAoutTheta) {
              time_mintau_ = TCAS2D::time_of_min_tau(DMOD,RAin2D_lookahead,minRAoutTheta,s2,v2);
            } else {
              time_mintau_ = TCAS2D::time_of_min_tau(DMOD,B,T,s2,v2);
            }
          } else {
            time_in_ = RAin2D_lookahead;
            time_out_ = RAout2D_lookahead;
            time_mintau_ = TCAS2D::time_of_min_tau(DMOD,RAin2D_lookahead,RAout2D_lookahead,s2,v2);
          }
        }
      }
    }
  }
  return Triple<double,double,double>(time_in_, time_out_, time_mintau_);
}


/**
 * Returns TAU threshold for sensitivity level sl in seconds
 */
double TCAS3D::getTAU(int sl) const {
  return table_.getTAU(sl);
}

/**
 * Returns TCOA threshold for sensitivity level sl in seconds
 */
double TCAS3D::getTCOA(int sl) const {
  return table_.getTCOA(sl);
}

/**
 * Returns DMOD for sensitivity level sl in internal units.
 */
double TCAS3D::getDMOD(int sl) const {
  return table_.getDMOD(sl);
}

/**
 * Returns DMOD for sensitivity level sl in u units.
 */
double TCAS3D::getDMOD(int sl, const std::string& u) const {
  return table_.getDMOD(sl,u);
}

/**
 * Returns Z threshold for sensitivity level sl in internal units.
 */
double TCAS3D::getZTHR(int sl) const {
  return table_.getZTHR(sl);
}

/**
 * Returns Z threshold for sensitivity level sl in u units.
 */
double TCAS3D::getZTHR(int sl,const std::string& u) const {
  return table_.getZTHR(sl,u);
}

/**
 * Returns HMD for sensitivity level sl in internal units.
 */
double TCAS3D::getHMD(int sl) const {
  return table_.getHMD(sl);
}

/**
 * Returns HMD for sensitivity level sl in u units.
 */
double TCAS3D::getHMD(int sl, const std::string& u) const {
  return table_.getHMD(sl,u);
}

/** Modify the value of Tau Threshold for a given sensitivity level (2-8)
 * Parameter val is given in seconds
 */
void TCAS3D::setTAU(int sl, double val) {
  table_.setTAU(sl,val);
}

/** Modify the value of TCOA Threshold for a given sensitivity level (2-8)
 * Parameter val is given in seconds
 */
void TCAS3D::setTCOA(int sl, double val) {
  table_.setTCOA(sl,val);
}

/** Modify the value of DMOD for a given sensitivity level (2-8)
 * Parameter val is given in internal units
 */
void TCAS3D::setDMOD(int sl, double val) {
  table_.setDMOD(sl, val);
}

/** Modify the value of DMOD for a given sensitivity level (2-8)
 * Parameter val is given in u units
 */
void TCAS3D::setDMOD(int sl, double val, const std::string& u) {
  table_.setDMOD(sl,val,u);
}

/** Modify the value of ZTHR for a given sensitivity level (2-8)
 * Parameter val is given in internal units
 */
void TCAS3D::setZTHR(int sl, double val) {
  table_.setZTHR(sl,val);
}

/** Modify the value of ZTHR for a given sensitivity level (2-8)
 * Parameter val is given in u units
 */
void TCAS3D::setZTHR(int sl, double val, const std::string& u) {
  table_.setZTHR(sl,val,u);
}

/**
 * Modify the value of HMD for a given sensitivity level (2-8)
 * Parameter val is given in internal units
 */
void TCAS3D::setHMD(int sl, double val) {
  table_.setHMD(sl,val);
}

/**
 * Modify the value of HMD for a given sensitivity level (2-8)
 * Parameter val is given in u units
 */
void TCAS3D::setHMD(int sl, double val, const std::string& u) {
  table_.setHMD(sl,val,u);
}

void TCAS3D::setHMDFilter(bool flag) {
  table_.setHMDFilter(flag);
}

bool TCAS3D::getHMDFilter() {
  return table_.getHMDFilter();
}

std::string TCAS3D::toString() const {
  return (id == "" ? "" : id+" = ")+getSimpleClassName()+": {"+table_.toString()+"}";
}

std::string TCAS3D::toPVS() const {
  return getSimpleClassName()+"("+table_.toPVS()+")";
}

ParameterData TCAS3D::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void TCAS3D::updateParameterData(ParameterData& p) const {
  table_.updateParameterData(p);
  p.set("id",id);
}

void TCAS3D::setParameters(const ParameterData& p) {
  table_.setParameters(p);
  if (p.contains("id")) {
    id = p.getString("id");
  }
}

std::string TCAS3D::getSimpleClassName() const {
  return "TCAS3D";
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
  if (!table_.equals(((TCAS3D*)d)->table_)) return false;
  return true;
}

bool TCAS3D::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    TCAS3D* d = (TCAS3D*)cd;
    return table_.contains(d->table_);
  }
  return false;
}

void TCAS3D::horizontalHazardZone(std::vector<Position>& haz, const TrafficState& ownship, const TrafficState& intruder,
    double T) const {
  int sl = table_.getSensitivityLevel(ownship.altitude());
  bool usehmdf = table_.getHMDFilter();
  double TAUMOD  = table_.getTAU(sl);
  double DMOD = Util::max(table_.getDMOD(sl),table_.getHMD(sl));
  haz.clear();
  Position po = ownship.getPosition();
  Velocity v = Velocity::make(ownship.getVelocity().Sub(intruder.getVelocity()));
  if (Util::almost_equals(TAUMOD+T,0) || Util::almost_equals(v.norm2D(),0)) {
    CDCylinder::circular_arc(haz,po,Velocity::mkVxyz(DMOD,0,0),2*Pi,false);
  } else {
    Vect3 sD = Horizontal::unit_perpL(v).Scal(DMOD);
    Velocity vD = Velocity::make(sD);
    CDCylinder::circular_arc(haz,po,vD,Pi,usehmdf);
    Position TAU_center = WCV_TAUMOD::TAU_center(po,v,TAUMOD,T);
    Vect3 vC = v.Scal(0.5*TAUMOD);     // TAUMOD Center (relative)
    if (usehmdf) {
      Vect3 vDC = vC.Sub(vD); // Far end point opposite to -vD (vC-relative);
      Vect3 nvDC = vC.Add(vD); // Far end point opposite to vD (vC-relative);
      double sqa = vDC.sqv2D();
      double alpha = Util::atan2_safe(vDC.det2D(nvDC)/sqa,vDC.dot2D(nvDC)/sqa);
      Velocity velDC = Velocity::make(vDC);
      CDCylinder::circular_arc(haz,TAU_center,velDC,alpha,true);
    } else {
      Vect3 nsCD=sD.Neg().Sub(vC);
      Vect3 sCD=sD.Sub(vC);
      double sqa = sCD.sqv2D();
      Velocity nvCD = Velocity::make(nsCD);
      if (Util::almost_equals(T,0)) { // Two circles: DMOD and TAUMO. They intersect at +/-vD
        double alpha = Util::atan2_safe(nsCD.det2D(sCD)/sqa,nsCD.dot2D(sCD)/sqa);
        CDCylinder::circular_arc(haz,TAU_center,nvCD,alpha,false);
      } else { // Two circles: DMOD and TAUMOD. They intersect at +/- vD.
        Vect3 sT = Horizontal::unit_perpL(v).Scal(std::sqrt(sqa));
        Velocity vT = Velocity::make(sT);
        Vect3 nsT = sT.Neg();
        Velocity nvT = Velocity::make(nsT);
        double alpha = Util::atan2_safe(nsCD.det2D(nsT)/sqa,nsCD.dot2D(nsT)/sqa);
        Position TAU_center_0 = WCV_TAUMOD::TAU_center(po,v,TAUMOD,0);
        CDCylinder::circular_arc(haz,TAU_center_0,nvCD,alpha,true);
        CDCylinder::circular_arc(haz,TAU_center,nvT,Pi,true);
        CDCylinder::circular_arc(haz,TAU_center_0,vT,alpha,false);                }
    }
  }
}


}
