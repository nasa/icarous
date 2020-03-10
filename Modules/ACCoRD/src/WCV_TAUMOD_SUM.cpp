/*
 * Copyright (c) 2015-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "WCV_TAUMOD_SUM.h"
#include "WCV_TCOA.h"
#include "Horizontal.h"
#include "Interval.h"
#include "Util.h"
#include "format.h"
#include "string_util.h"
#include <math.h>

namespace larcfm {

double WCV_TAUMOD_SUM::MinError = 0.001;

void WCV_TAUMOD_SUM::initSUM() {
  h_pos_z_score_ = 0.0;
  h_pos_z_score_enabled_ = false;
  h_vel_z_score_min_ = 0.0;
  h_vel_z_score_min_enabled_ = false;
  h_vel_z_score_max_ = 0.0;
  h_vel_z_score_max_enabled_ = false;
  h_vel_z_distance_ = 0.0;
  h_vel_z_distance_enabled_ = false;
  h_vel_z_distance_units_ = "nmi";
  v_pos_z_score_ = 0.0;
  v_pos_z_score_enabled_ = false;
  v_vel_z_score_ = 0.0;
  v_vel_z_score_enabled_ = false;
}

/** Constructor that a default instance of the WCV tables. */
WCV_TAUMOD_SUM::WCV_TAUMOD_SUM() {
  wcv_vertical = new WCV_TCOA();
  initSUM();
}

/** Constructor that specifies a particular instance of the WCV tables. */
WCV_TAUMOD_SUM::WCV_TAUMOD_SUM(const WCVTable& tab) {
  table = tab;
  wcv_vertical = new WCV_TCOA();
  initSUM();
}

/**
 * @return one static WCV_TAUMOD_SUM
 */
const WCV_TAUMOD_SUM& WCV_TAUMOD_SUM::A_WCV_TAUMOD_SUM() {
  static WCV_TAUMOD_SUM dwc;
  return dwc;
}

void WCV_TAUMOD_SUM::copyFrom(const WCV_TAUMOD_SUM& wcv) {
  if (&wcv != this) {
    id = wcv.id;
    table = wcv.table;
    if (wcv_vertical != NULL) {
      delete wcv_vertical;
    }
    wcv_vertical = wcv.wcv_vertical != NULL ? wcv.wcv_vertical->copy() : NULL;
    h_pos_z_score_ = wcv.h_pos_z_score_;
    h_pos_z_score_enabled_ = wcv.h_pos_z_score_enabled_;
    h_vel_z_score_min_ = wcv.h_vel_z_score_min_;
    h_vel_z_score_min_enabled_ = wcv.h_vel_z_score_min_enabled_;
    h_vel_z_score_max_ = wcv.h_vel_z_score_max_;
    h_vel_z_score_max_enabled_ = wcv.h_vel_z_score_max_enabled_;
    h_vel_z_distance_ = wcv.h_vel_z_distance_;
    h_vel_z_distance_enabled_ = wcv.h_vel_z_distance_enabled_;
    h_vel_z_distance_units_ = wcv.h_vel_z_distance_units_;
    v_pos_z_score_ = wcv.v_pos_z_score_;
    v_pos_z_score_enabled_ = wcv.v_pos_z_score_enabled_;
    v_vel_z_score_ = wcv.v_vel_z_score_;
    v_vel_z_score_enabled_ = wcv.v_vel_z_score_enabled_;
  }
}

WCV_TAUMOD_SUM& WCV_TAUMOD_SUM::operator=(const WCV_TAUMOD_SUM& wcv) {
  copyFrom(wcv);
  return *this;
}


bool WCV_TAUMOD_SUM::sumof(const Vect2& v1, const Vect2& v2, const Vect2& w) const {
  double detv2v1 = v2.det(v1);
  return w.det(v1)*detv2v1>=0 && w.det(v2)*detv2v1 <=0;
}

Vect2 WCV_TAUMOD_SUM::average_direction(const Vect2& v1, const Vect2& v2) const {
  return v1.Add(v2).Hat();
}

std::pair<Vect2,Vect2> WCV_TAUMOD_SUM::optimal_pair(const Vect2& v1, const Vect2& v2, const Vect2& w1, const Vect2& w2) const {
  if (sumof(v1,v2,w1) && sumof(v1,v2,w2)) {
    Vect2 avg_dir = average_direction(w1,w2);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else if (sumof(w1,w2,v1) && sumof(w1,w2,v2)) {
    Vect2 avg_dir = average_direction(v1,v2);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else if (sumof(w1,w2,v1) && sumof(v1,v2,w1)) {
    Vect2 avg_dir = average_direction(v1,w1);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else if (sumof(w1,w2,v1) && sumof(v1,v2,w2)) {
    Vect2 avg_dir = average_direction(v1,w2);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else if (sumof(w1,w2,v2) && sumof(v1,v2,w1)) {
    Vect2 avg_dir = average_direction(v2,w1);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else if (sumof(w1,w2,v2) && sumof(v1,v2,w2)) {
    Vect2 avg_dir = average_direction(v2,w2);
    return std::pair<Vect2,Vect2>(avg_dir,avg_dir);
  } else {
    double d11 = v1.dot(w1);
    double d12 = v1.dot(w2);
    double d21 = v2.dot(w1);
    double d22 = v2.dot(w2);
    if (d11>=d12 && d11>=d21 && d11>=d22) {
      return std::pair<Vect2,Vect2>(v1,w1);
    } else if (d12>=d11 && d12>=d21 && d12>=d22) {
      return std::pair<Vect2,Vect2>(v1,w2);
    } else if (d21>=d11 && d21>=d12 && d21>=d22) {
      return std::pair<Vect2,Vect2>(v2,w1);
    } else {
      return std::pair<Vect2,Vect2>(v2, w2);
    }
  }
}

std::pair<Vect2,Vect2> WCV_TAUMOD_SUM::optimal_wcv_pair_comp_init(const Vect2& s, const Vect2& v, double s_err, double v_err) const {
  Vect2 v1 = TangentLine(s,s_err,-1);
  v1 = v1.Hat();
  Vect2 v2 = TangentLine(s,s_err,1);
  v2 = v2.Hat();
  Vect2 w = v.Neg();
  Vect2 w1 = TangentLine(w,v_err,-1);
  w1 = w1.Hat();
  Vect2 w2 = TangentLine(w,v_err,1);
  w2 = w2.Hat();
  std::pair<Vect2,Vect2> op = optimal_pair(v1,v2,w1,w2);
  return std::pair<Vect2,Vect2>(op.first.Neg(),op.second);
}

std::pair<Vect2,Vect2> WCV_TAUMOD_SUM::optimal_wcv_pair(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps1, int eps2) const {
  std::pair<Vect2,Vect2> owpci = optimal_wcv_pair_comp_init(s,v,s_err,v_err);
  return std::pair<Vect2,Vect2>(owpci.first.Scal(s.norm()+eps1*s_err),owpci.second.Scal(v.norm()-eps2*v_err));
}

bool WCV_TAUMOD_SUM::horizontal_wcv_taumod_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err) const {
  if (horizontal_WCV(s,v)) {
    return true;
  }
  if (s.sqv()<=Util::sq(table.DTHR+s_err)) {
    return true;
  }
  if (v.sqv()<=Util::sq(v_err)) {
    Vect2 s_hat = s.Hat();
    return horizontal_WCV(s_hat.Scal(s.norm()-s_err),s_hat.Scal(-(v.norm()+v_err)));
  }
  std::pair<Vect2,Vect2> owp = optimal_wcv_pair(s,v,s_err,v_err,-1,-1);
  return horizontal_WCV(owp.first,owp.second);
}

bool WCV_TAUMOD_SUM::vertical_WCV_uncertain(double sz, double vz, double sz_err, double vz_err) const {
  int ssign = Util::sign(sz);
  double snew = sz-ssign*Util::min(sz_err,std::abs(sz));
  double vnew = vz-ssign*vz_err;
  return wcv_vertical->vertical_WCV(table.ZTHR,table.TCOA,snew,vnew);
}

bool WCV_TAUMOD_SUM::WCV_taumod_uncertain(const Vect3& s, const Vect3& v, double s_err, double sz_err, double v_err, double vz_err) const {
  return horizontal_wcv_taumod_uncertain(s.vect2(),v.vect2(),s_err,v_err) &&
      vertical_WCV_uncertain(s.z,v.z,sz_err,vz_err);
}

double WCV_TAUMOD_SUM::horizontal_wcv_taumod_uncertain_entry(const Vect2& s, const Vect2& v, double s_err, double v_err, double T) const {
  if (horizontal_WCV(s,v) || s.sqv()<=Util::sq(table.DTHR+s_err)) {
    return 0;
  }
  if (v.sqv() <= Util::sq(v_err)) {
    Vect2 s_hat = s.Hat();
    LossData ee = horizontal_WCV_interval(T,s_hat.Scal(s.norm()-s_err),s_hat.Scal(-(v.norm()+v_err)));
    if (ee.getTimeOut() < ee.getTimeIn()) {
      return T+1;
    } else {
      return ee.getTimeIn();
    }
  } else {
    std::pair<Vect2,Vect2> op = optimal_wcv_pair(s,v,s_err,v_err,-1,-1);
    if (op.first.dot(op.second) < 0) {
      return  Util::min(horizontal_WCV_interval(T,s,v).getTimeIn(),
          horizontal_WCV_interval(T,op.first,op.second).getTimeIn());
    } else {
      return T+1;
    }
  }
}

double WCV_TAUMOD_SUM::Theta_D_uncertain(const Vect2& s, const Vect2& v, double s_err, double v_err, int eps) const {
  if (v.sqv() <= Util::sq(v_err)) {
    return -1;
  }
  else {
    double rt = Util::root(v.sqv()-Util::sq(v_err),2*(s.dot(v)-v_err*(table.DTHR+s_err)),s.sqv()-Util::sq(table.DTHR+s_err),eps);
    if (ISFINITE(rt)) {
      return rt;
    }
    return -1;
  }
}

double WCV_TAUMOD_SUM::horizontal_wcv_taumod_uncertain_exit(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const {
  if (v.sqv() <= Util::sq(v_err) && s.sqv() <= Util::sq(table.DTHR+s_err)) {
    return T;
  } else if (v.sqv() <= Util::sq(v_err)) {
    Vect2 s_hat = s.Hat();
    LossData ee = horizontal_WCV_interval(T,s_hat.Scal(s.norm()-s_err),s_hat.Scal(-(v.norm()+v_err)));
    if (ee.getTimeOut() < ee.getTimeIn()) {
      return -1;
    } else {
      return T;
    }
  } else {
    return Theta_D_uncertain(s,v,s_err,v_err,1);
  }
}

LossData WCV_TAUMOD_SUM::horizontal_wcv_taumod_uncertain_interval(const Vect2& s, const Vect2& v,double s_err, double v_err, double T) const {
  double entrytime = horizontal_wcv_taumod_uncertain_entry(s,v,s_err,v_err,T);
  double exittime = horizontal_wcv_taumod_uncertain_exit(s,v,s_err,v_err,T);
  if (entrytime > T || exittime < 0 || entrytime > exittime) {
    return LossData();
  }
  return LossData(Util::max(0.0,entrytime),Util::min(T,exittime));
}

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_full_interval_szpos_vzpos(double T, double minsz/*,double maxsz*/, double minvz/*, double maxvz*/) const {
  Interval ii = wcv_vertical->vertical_WCV_interval(table.ZTHR,table.TCOA,0,T,minsz,minvz);
  return LossData(ii.low,ii.up);
}

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_full_interval_szpos_vzneg(double T, double minsz,double maxsz, double minvz, double maxvz) const {
  Interval entryint = wcv_vertical->vertical_WCV_interval(table.ZTHR,table.TCOA,0,T,minsz,minvz);
  Interval exitint = wcv_vertical->vertical_WCV_interval(table.ZTHR,table.TCOA,0,T,maxsz,maxvz);
  if (entryint.low > entryint.up) {
    return LossData();
  } else if (exitint.low > exitint.up) {
    return LossData(entryint.low,T);
  } else {
    return LossData(entryint.low,exitint.up);
  }
}

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_full_interval_szpos(double T, double minsz,double maxsz, double minvz, double maxvz) const {
  bool vel_only_pos = minvz >= 0;
  bool vel_only_neg = !vel_only_pos && maxvz <= 0;
  LossData intp = vel_only_neg ? LossData() : vertical_WCV_uncertain_full_interval_szpos_vzpos(T,minsz/*,maxsz*/,Util::max(minvz,0.0)/*,maxvz*/);
  LossData intn = vel_only_pos ? LossData() : vertical_WCV_uncertain_full_interval_szpos_vzneg(T,minsz,maxsz,minvz,Util::min(maxvz,0.0));
  if (vel_only_pos || intn.getTimeIn() > intn.getTimeOut()) {
    return intp;
  } else if (vel_only_neg || intp.getTimeIn() > intp.getTimeOut()) {
    return intn;
  } else {
    return  LossData(Util::min(intp.getTimeIn(),intn.getTimeIn()),Util::max(intp.getTimeOut(),intn.getTimeOut()));
  }
}

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_full_interval_split(double T, double minsz,double maxsz, double minvz, double maxvz) const {
  bool pos_only_pos = minsz >= 0;
  bool pos_only_neg = !pos_only_pos && maxsz <= 0;
  LossData intp = pos_only_neg ? LossData() : vertical_WCV_uncertain_full_interval_szpos(T,Util::max(minsz,0.0),maxsz,minvz,maxvz);
  LossData intn = pos_only_pos ? LossData() : vertical_WCV_uncertain_full_interval_szpos(T,-Util::min(maxsz,0.0),-minsz,-maxvz,-minvz);
  if (pos_only_pos || intn.getTimeIn() > intn.getTimeOut()) {
    return intp;
  } else if (pos_only_neg || intp.getTimeIn() > intp.getTimeOut()) {
    return intn;
  } else {
    return  LossData(Util::min(intp.getTimeIn(),intn.getTimeIn()),Util::max(intp.getTimeOut(),intn.getTimeOut()));
  }
}

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_interval(double B, double T, double sz, double vz, double sz_err, double vz_err) const {
  LossData posint = vertical_WCV_uncertain_full_interval_split(T,sz-sz_err,sz+sz_err,vz-vz_err,vz+vz_err);
  if (posint.getTimeIn() > posint.getTimeOut() || posint.getTimeOut() < B) {
    return LossData();
  } else {
    return LossData(Util::max(B,posint.getTimeIn()),Util::min(T,posint.getTimeOut()));
  }
}

LossData WCV_TAUMOD_SUM::WCV_taumod_uncertain_interval(double B, double T, const Vect3& s, const Vect3& v,
    double s_err, double sz_err, double v_err, double vz_err) const {
  LossData vint = vertical_WCV_uncertain_interval(B,T,s.z,v.z,sz_err,vz_err);
  if (vint.getTimeIn() > vint.getTimeOut()) {
    return vint; // Empty interval
  }
  LossData hint = horizontal_wcv_taumod_uncertain_interval(s.vect2(),v.vect2(),s_err,v_err,T);
  if (hint.getTimeIn() > hint.getTimeOut()) {
    return hint; // Empty interval
  }
  if (hint.getTimeOut() < B) {
    return LossData();
  }
  return  LossData(Util::max(vint.getTimeIn(),hint.getTimeIn()),Util::min(vint.getTimeOut(),hint.getTimeOut()));
}

bool WCV_TAUMOD_SUM::WCV_taumod_uncertain_detection(double B, double T, const Vect3& s, const Vect3& v,
    double s_err, double sz_err, double v_err, double vz_err) const {
  if (B > T) {
    return false;
  }
  LossData interval = B == T ?  WCV_taumod_uncertain_interval(B,B+1,s,v,s_err,sz_err,v_err,vz_err):
      WCV_taumod_uncertain_interval(B,T,s,v,s_err,sz_err,v_err,vz_err);
  if (B == T) {
    return interval.conflict() && interval.getTimeIn()<=B;
  }
  return interval.conflict();
}

/**
 * This functional call returns a ConflictData object detailing the conflict between times B and T from now (relative), if any.
 * @param ownship   ownship state
 * @param intruder  intruder state
 * @param D   horizontal separation
 * @param H   vertical separation
 * @param B   beginning of detection time (>=0)
 * @param T   end of detection time (if T < 0 then use an "infinite" lookahead time)
 * @return a ConflictData object detailing the conflict
 */
ConflictData WCV_TAUMOD_SUM::conflictDetectionWithTrafficState(const TrafficState& ownship, const TrafficState& intruder,
    double B, double T) const {
  double s_err = relativeHorizontalPositionError(ownship,intruder);
  double sz_err = relativeVerticalPositionError(ownship,intruder);
  double v_err = relativeHorizontalSpeedError(ownship,intruder,s_err);
  double vz_err = relativeVerticalSpeedError(ownship,intruder);

  Vect3 so = ownship.get_s();
  Velocity vo = ownship.get_v();
  Vect3 si = intruder.get_s();
  Velocity vi = intruder.get_v();

  if (s_err == 0.0 && sz_err == 0.0 && v_err == 0.0 && vz_err == 0.0) {
    return conflictDetection(so,vo,si,vi,B,T);
  }

  s_err = Util::max(s_err, MinError);
  sz_err = Util::max(sz_err, MinError);
  v_err = Util::max(v_err, MinError);
  vz_err = Util::max(vz_err, MinError);

  Vect3 s = so.Sub(si);
  Vect3 v = vo.Sub(vi);
  LossData ld = WCV_taumod_uncertain_interval(B,T,s,v,s_err,sz_err,v_err,vz_err);
  double t_tca = (ld.getTimeIn() + ld.getTimeOut())/2.0;
  double dist_tca = s.linear(v, t_tca).cyl_norm(table.DTHR,table.ZTHR);
  return ConflictData(ld,t_tca,dist_tca,s,v);
}

Detection3D* WCV_TAUMOD_SUM::make() const {
  return new WCV_TAUMOD_SUM();
}

/**
 * Returns a deep copy of this WCV_TAUMOD object, including any results that have been calculated.
 */
Detection3D* WCV_TAUMOD_SUM::copy() const {
  WCV_TAUMOD_SUM* ret = new WCV_TAUMOD_SUM();
  ret->copyFrom(*this);
  return ret;
}

std::string WCV_TAUMOD_SUM::getSimpleClassName() const {
  return "WCV_TAUMOD_SUM";
}

bool WCV_TAUMOD_SUM::containsSUM(WCV_TAUMOD_SUM* wcv) const {
  return h_pos_z_score_ == wcv->h_pos_z_score_ &&
      h_vel_z_score_min_ == wcv->h_vel_z_score_min_ &&
      h_vel_z_score_max_ == wcv->h_vel_z_score_max_ &&
      h_vel_z_distance_ == wcv->h_vel_z_distance_ &&
      v_pos_z_score_ == wcv->v_pos_z_score_ &&
      v_vel_z_score_ == wcv->v_vel_z_score_;
}

bool WCV_TAUMOD_SUM::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(),cd->getCanonicalClassName())) {
    if (!containsSUM((WCV_TAUMOD_SUM*)cd)) {
      return false;
    }
  }
  if (larcfm::equals(getCanonicalClassName(),cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TAUMOD", cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TCPA", cd->getCanonicalClassName()) ) {
    return containsTable((WCV_tvar*)cd);
  }
  return false;
}

double WCV_TAUMOD_SUM::relativeHorizontalPositionError(const TrafficState& own, const TrafficState& ac) const {
  return h_pos_z_score_*
      (own.sum().getHorizontalPositionError()+ac.sum().getHorizontalPositionError());
}

double WCV_TAUMOD_SUM::relativeVerticalPositionError(const TrafficState& own, const TrafficState& ac) const {
  return v_pos_z_score_*
      (own.sum().getVerticalPositionError()+ac.sum().getVerticalPositionError());
}

double WCV_TAUMOD_SUM::weighted_z_score(double range) const {
  if (range>=h_vel_z_distance_) {
    return h_vel_z_score_min_;
  } else {
    double perc = range/h_vel_z_distance_;
    return (1-perc)*h_vel_z_score_max_+perc*h_vel_z_score_min_;
  }
}

double WCV_TAUMOD_SUM::relativeHorizontalSpeedError(const TrafficState& own, const TrafficState& ac, double s_err) const {
  double range = own.get_s().distanceH(ac.get_s());
  double  z_score = weighted_z_score(Util::max(range-s_err,0.0));
  return z_score*
      (own.sum().getHorizontalSpeedError()+ac.sum().getHorizontalSpeedError());
}

double WCV_TAUMOD_SUM::relativeVerticalSpeedError(const TrafficState& own, const TrafficState& ac) const {
  return v_vel_z_score_*
      (own.sum().getVerticalSpeedError()+ac.sum().getVerticalSpeedError());
}

void WCV_TAUMOD_SUM::updateParameterData(ParameterData& p) const {
  WCV_TAUMOD::updateParameterData(p);
  if (h_pos_z_score_enabled_) {
    p.setInternal("h_pos_z_score", h_pos_z_score_, "unitless");
  }
  if (h_vel_z_score_min_enabled_) {
    p.setInternal("h_vel_z_score_min", h_vel_z_score_min_, "unitless");
  }
  if (h_vel_z_score_max_enabled_) {
    p.setInternal("h_vel_z_score_max", h_vel_z_score_max_, "unitless");
  }
  if (h_vel_z_distance_enabled_) {
    p.setInternal("h_vel_z_distance", h_vel_z_distance_, h_vel_z_distance_units_);
  }
  if (v_pos_z_score_enabled_) {
    p.setInternal("v_pos_z_score", v_pos_z_score_, "unitless");
  }
  if (v_vel_z_score_enabled_) {
    p.setInternal("v_vel_z_score", v_vel_z_score_, "unitless");
  }
}

void WCV_TAUMOD_SUM::setParameters(const ParameterData& p) {
  WCV_TAUMOD::setParameters(p);
  if (p.contains("h_pos_z_score")) {
    setHorizontalPositionZScore(p.getValue("h_pos_z_score"));
  }
  if (p.contains("h_vel_z_score_min")) {
    setHorizontalVelocityZScoreMin(p.getValue("h_vel_z_score_min"));
  }
  if (p.contains("h_vel_z_score_max")) {
    setHorizontalVelocityZScoreMax(p.getValue("h_vel_z_score_max"));
  }
  if (p.contains("h_vel_z_distance")) {
    setHorizontalVelocityZDistance(p.getValue("h_vel_z_distance"));
    h_vel_z_distance_units_ = p.getUnit("h_vel_z_distance");
  }
  if (p.contains("v_pos_z_score")) {
    setVerticalPositionZScore(p.getValue("v_pos_z_score"));
  }
  if (p.contains("v_vel_z_score")) {
    setVerticalSpeedZScore(p.getValue("v_vel_z_score"));
  }
}

void WCV_TAUMOD_SUM::set_global_SUM_parameters(const DaidalusParameters& p){
  if (!h_pos_z_score_enabled_) {
    h_pos_z_score_ = p.getHorizontalPositionZScore();
  }
  if (!h_vel_z_score_min_enabled_) {
    h_vel_z_score_min_ = p.getHorizontalVelocityZScoreMin();
  }
  if (!h_vel_z_score_max_enabled_) {
    h_vel_z_score_max_ = p.getHorizontalVelocityZScoreMax();
  }
  if (!h_vel_z_distance_enabled_) {
    h_vel_z_distance_ = p.getHorizontalVelocityZDistance();
    h_vel_z_distance_units_ = p.getUnitsOf("h_vel_z_distance");
  }
  if (!v_pos_z_score_enabled_) {
    v_pos_z_score_ = p.getVerticalPositionZScore();
  }
  if (!v_vel_z_score_enabled_) {
    v_vel_z_score_ = p.getVerticalSpeedZScore();
  }
}

/**
 * @return get z-score (number of standard deviations) for horizontal position
 */
double WCV_TAUMOD_SUM::getHorizontalPositionZScore() const {
  return h_pos_z_score_;
}

/**
 * @return set z-score (number of standard deviations) for horizontal position (non-negative value)
 */
void WCV_TAUMOD_SUM::setHorizontalPositionZScore(double val) {
  h_pos_z_score_enabled_ = true;
  h_pos_z_score_ = std::abs(val);
}

/**
 * @return get min z-score (number of standard deviations) for horizontal velocity
 */
double WCV_TAUMOD_SUM::getHorizontalVelocityZScoreMin() const {
  return h_vel_z_score_min_;
}

/**
 * @return set min z-score (number of standard deviations) for horizontal velocity (non-negative value)
 */
void WCV_TAUMOD_SUM::setHorizontalVelocityZScoreMin(double val) {
  h_vel_z_score_min_enabled_ = true;
  h_vel_z_score_min_ = std::abs(val);
}

/**
 * @return get max z-score (number of standard deviations) for horizontal velocity
 */
double WCV_TAUMOD_SUM::getHorizontalVelocityZScoreMax() const {
  return h_vel_z_score_max_;
}

/**
 * @return set max z-score (number of standard deviations) for horizontal velocity (non-negative value)
 */
void WCV_TAUMOD_SUM::setHorizontalVelocityZScoreMax(double val) {
  h_vel_z_score_max_enabled_ = true;
  h_vel_z_score_max_ = std::abs(val);
}

/**
 * @return Distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
 */
double WCV_TAUMOD_SUM::getHorizontalVelocityZDistance() const {
  return h_vel_z_distance_;
}

/**
 * @return Set distance (in internal units) at which h_vel_z_score scales from min to max as range decreases
 */
void WCV_TAUMOD_SUM::setHorizontalVelocityZDistance(double val) {
  h_vel_z_distance_enabled_ = true;
  h_vel_z_distance_ = std::abs(val);
}

/**
 * @return Distance (in given units) at which h_vel_z_score scales from min to max as range decreases
 */
double WCV_TAUMOD_SUM::getHorizontalVelocityZDistance(const std::string& u) const {
  return Units::to(u,h_vel_z_distance_);
}

/**
 * @return Set distance (in given units) at which h_vel_z_score scales from min to max as range decreases
 */
void WCV_TAUMOD_SUM::setHorizontalVelocityZDistance(double val, const std::string& u) {
  setHorizontalVelocityZDistance(Units::from(u,val));
  h_vel_z_distance_units_ = u;
}

/**
 * @return get z-score (number of standard deviations) for vertical position
 */
double WCV_TAUMOD_SUM::getVerticalPositionZScore() const {
  return v_pos_z_score_;
}

/**
 * @return set z-score (number of standard deviations) for vertical position (non-negative value)
 */
void WCV_TAUMOD_SUM::setVerticalPositionZScore(double val) {
  v_pos_z_score_enabled_ = true;
  v_pos_z_score_ = std::abs(val);
}

/**
 * @return get z-score (number of standard deviations) for vertical velocity
 */
double WCV_TAUMOD_SUM::getVerticalSpeedZScore() const {
  return v_vel_z_score_;
}

/**
 * @return set z-score (number of standard deviations) for vertical velocity (non-negative value)
 */
void WCV_TAUMOD_SUM::setVerticalSpeedZScore(double val) {
  v_vel_z_score_enabled_ = true;
  v_vel_z_score_ = std::abs(val);
}

std::string WCV_TAUMOD_SUM::toString() const {
  std::string str = WCV_TAUMOD::toString();
  str += ", {h_pos_z_score = "+FmPrecision(h_pos_z_score_);
  str += ", h_vel_z_score_min = "+FmPrecision(h_vel_z_score_min_);
  str += ", h_vel_z_score_max = "+FmPrecision(h_vel_z_score_max_);
  str += ", h_vel_z_distance = "+Units::str(h_vel_z_distance_units_,h_vel_z_distance_);
  str += ", v_pos_z_score = "+ FmPrecision(v_pos_z_score_);
  str += ", v_vel_z_score = "+ FmPrecision(v_vel_z_score_)+"}";
  return str;
}

std::string WCV_TAUMOD_SUM::toPVS() const {
  std::string str = getSimpleClassName()+"((# "+table.toPVS_();
  str += ", h_pos_z_score := "+FmPrecision(h_pos_z_score_);
  str += ", h_vel_z_score_min := "+FmPrecision(h_vel_z_score_min_);
  str += ", h_vel_z_score_max := "+FmPrecision(h_vel_z_score_max_);
  str += ", h_vel_z_distance := "+FmPrecision(h_vel_z_distance_);
  str += ", v_pos_z_score := "+ FmPrecision(v_pos_z_score_);
  str += ", v_vel_z_score := "+ FmPrecision(v_vel_z_score_);
  str += " #))";
  return str;
}

}

