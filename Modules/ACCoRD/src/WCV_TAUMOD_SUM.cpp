/*
 * Copyright (c) 2015-2018 United States Government as represented by
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

/** Constructor that a default instance of the WCV tables. */
WCV_TAUMOD_SUM::WCV_TAUMOD_SUM() {
    wcv_vertical = new WCV_TCOA();
}

/** Constructor that specifies a particular instance of the WCV tables. */
WCV_TAUMOD_SUM::WCV_TAUMOD_SUM(const WCVTable& tab) {
    table = tab;
    wcv_vertical = new WCV_TCOA();
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

LossData WCV_TAUMOD_SUM::vertical_WCV_uncertain_full_interval_szpos_vzpos(double T, double minsz,double maxsz, double minvz, double maxvz) const {
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
    LossData intp = vel_only_neg ? LossData() : vertical_WCV_uncertain_full_interval_szpos_vzpos(T,minsz,maxsz,Util::max(minvz,0.0),maxvz);
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

bool WCV_TAUMOD_SUM::violationSUMAt(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
        double s_err, double sz_err, double v_err, double vz_err, double t) const {
    if (s_err == 0.0 && sz_err == 0.0 && v_err == 0.0 && vz_err == 0.0) {
        return violation(so.AddScal(t,vo),vo,si.AddScal(t,vi),vi);
    }
    Vect3 s = so.Sub(si);
    Vect3 v = vo.Sub(vi);
    Vect3 st = t == 0 ? s : v.ScalAdd(t,s);
    return WCV_taumod_uncertain(st,v,s_err+t*v_err,sz_err+t*vz_err,v_err,vz_err);
}


bool WCV_TAUMOD_SUM::conflictSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T,
        double s_err, double sz_err, double v_err, double vz_err) const {
    if (s_err == 0.0 && sz_err == 0.0 && v_err == 0.0 && vz_err == 0.0) {
        return conflict(so,vo,si,vi,B,T);
    }
    Vect3 s = so.Sub(si);
    Vect3 v = vo.Sub(vi);
    return WCV_taumod_uncertain_detection(B,T,s,v,s_err,sz_err,v_err,vz_err);
}

ConflictData WCV_TAUMOD_SUM::conflictDetectionSUM(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T,
        double s_err, double sz_err, double v_err, double vz_err) const {
    if (s_err == 0.0 && sz_err == 0.0 && v_err == 0.0 && vz_err == 0.0) {
        return conflictDetection(so,vo,si,vi,B,T);
    }
    Vect3 s = so.Sub(si);
    Velocity v = Velocity(vo.Sub(vi));
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
    WCV_TAUMOD_SUM* ret = new WCV_TAUMOD_SUM(table);
    ret->id = id;
    return ret;
}

std::string WCV_TAUMOD_SUM::getSimpleClassName() const {
  return "WCV_TAUMOD_SUM";
}

bool WCV_TAUMOD_SUM::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TAUMOD", cd->getCanonicalClassName()) ||
      larcfm::equals("gov.nasa.larcfm.ACCoRD.WCV_TCPA", cd->getCanonicalClassName()) ) {
    return containsTable((WCV_tvar*)cd);
  }
  return false;
}

}

