/**
 * CriteriaCore.cpp
 *
 * State-based Implicit Criteria
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "CriteriaCore.h"
#include "format.h"
#include "Util.h"
#include "Kinematics.h"
#include "ACCoRDConfig.h"
#include <string>
#include <float.h>

namespace larcfm {

int CriteriaCore::horizontalCoordination(const Vect2& s, const Vect2& v) {
  return sign(v.det(s));
}

int CriteriaCore::horizontalCoordination(const Vect3& s, const Vect3& v) {
  return horizontalCoordination(s.vect2(), v.vect2());
}

/**
 * An algebraic way to determine a*sqrt(b) = e.
 * @return true, if a*sqrt(b) = e
 */
bool eq(double a,double b,double e) {
  return a*e >= 0 && sq(a)*b == sq(e);
}

/**
 * An algebraic way to determine a*sqrt(b) > e.
 * @return true, if a*sqrt(b) > e
 */
bool gt(double a,double b,double e) {
  return a >= 0 ? e < 0 || sq(a)*b > sq(e):
      e < 0 && sq(a)*b < sq(e);
}

int CriteriaCore::verticalCoordination(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H, std::string ownship, std::string traffic) {
  if (CD3D::LoS(s,D,H)) {
    return verticalCoordinationLoS(s,vo,vi,ownship,traffic);
  } else {
    Vect3 v = vo.Sub(vi);
    return verticalCoordinationConflict(s,v,D,ownship,traffic);
  }
}

int CriteriaCore::verticalCoordinationConflict(const Vect3& s, const Vect3& v, double D, std::string ownship, std::string traffic) {
  Vect2 s2 = s.vect2();
  Vect2 v2 = v.vect2();
  double a = v2.sqv();
  double b = s2*v2;
  double c = s2.sqv()-sq(D);
  double d = sq(b)-a*c;
  if (Util::almost_equals(v.z,0) || v2.isZero() ||
      d < 0 || eq(v.z,Util::sq(b)-a*c,s.z*a-v.z*b)) {
    //fpln(" verticalCoordination: Part 1, s = "+f.sStr(s));
    return breakSymmetry(s, ownship, traffic);
  }
  if (gt(v.z,Util::sq(b)-a*c,s.z*a-v.z*b)) {
    //fpln(" verticalCoordination: Part 2, s = "+f.sStr(s));
    return -1;
  }
  //fpln(" verticalCoordination: Part 3, s = "+f.sStr(s));
  return 1;
}

int CriteriaCore::verticalCoordinationLoS(const Vect3& s, const Vect3& vo, const Vect3& vi, std::string ownship, std::string traffic) {
  int epsv;
  epsv = losr_vs_dir(s,vo,vi,ACCoRDConfig::NMAC_D, ACCoRDConfig::NMAC_H,ownship,traffic);
  return epsv;
}

double R(const Vect2& sp, double D) {
  return sqrt(sp.sqv()-sq(D))/D;
}

bool CriteriaCore::horizontal_criterion(const Vect2& sp, const  Vect2& v, double D, int epsh) {
  return sp.dot(v) >= R(sp,D)*epsh*sp.det(v);
}


bool  closed_region_3D(const Vect3& s,const Vect3& p, int eps, int dir, const Vect3& v, double D, double H) {
  Vect2 s2 = s.vect2();
  Vect2 v2 = v.vect2();
  Vect2 p2 = p.vect2();
  double t = (sq(D)-s2.dot(p2))/(v2.dot(p2));
  return v2.dot(p2) != 0 && sign(v2.dot(p2)) == dir && t >= 0 &&  eps*(s.z + t*v.z) >= H &&
      ((std::abs(s.z) >= H && dir == eps*sign(s.z)) || (std::abs(s.z) < H && dir == -1));
}

bool  CriteriaCore::vertical_criterion(int eps, const Vect3& s, const Vect3& v, const Vect3& nv, double D, double H) {
  int Entry = -1;
  int dir = Entry;
  if (std::abs(s.z) >= H) dir = eps*sign(s.z);
  Vect2 s2 = s.vect2();
  Vect2 v2 = v.vect2();
  Vect3 p = Vect3(s2.Add(v2.Scal(Horizontal::Theta_D(s2,v2,dir,D))), eps*H);
  return ((v.x == 0 && v.y == 0) && eps*nv.z >= 0 && eps*s.z >= H)
      || (Horizontal::Delta(s2,v2,D) > 0 && Horizontal::Theta_D(s2,v2,dir,D) > 0
          && closed_region_3D(s,p,eps,dir,nv,D,H));

}


bool horizontal_los(const Vect2& s, double D) {
  return s.sqv() < sq(D);
}


bool vertical_los(double sz, double H) {
  return std::abs(sz) < H;
}


// sp = relative position
//  v = relative velocity
bool CriteriaCore::horizontal_criterion_0(const Vect2& sp, int eps, const Vect2& v, double D) {
  Vect2 vv = v;  // copy?
  if (Util::almost_equals(v.norm(),0.0,PRECISION13)) vv = Vect2::ZERO();        // criterion_3D's last term passes in nv - v which is exactly 0 for vertical solutions
  return horizontal_criterion(sp,vv,D,eps);
}


// from PVS
bool CriteriaCore::criterion_3D(const Vect3& sp, const Velocity&  v, int epsH, int epsV, const Velocity&  nv, double D, double H) {
  //      std::cout << "@@@@@ criterion_3D: "
  //        << horizontal_sep(sp.vect2(),D)
  //        << horizontal_criterion(sp.vect2(),epsH,nv.vect2(),D)
  //        << vertical_criterion(epsV,sp,v,nv,D,H)
  //        << horizontal_los(sp.vect2(),D)
  //        << horizontal_criterion(sp.vect2(),epsH,(nv.Sub(v)).vect2(),D)
  //        << std::endl;
  return (Horizontal::horizontal_sep(sp.vect2(),D) && horizontal_criterion_0(sp.vect2(),epsH,nv.vect2(),D)) ||
      (vertical_criterion(epsV,sp,v,nv,D,H) &&
          (horizontal_los(sp.vect2(),D) ||
              horizontal_criterion_0(sp.vect2(),epsH,(nv.Sub(v)).vect2(),D)));
}

// ***************************************** Repulsive Criteria ******************************************


// caD and caH are the diameter and height of a collision avoidance zone, e.g., 350ft x 100 ft
Vect3 CriteriaCore::vertical_decision_vect(const Vect3& s, const Vect3& vo, const Vect3& vi, double caD, double caH) {
  Vect2 s2 = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  Vect3 v = vo.Sub(vi);
  //fpln("vertical_decision_vect:  CD3D::cd3d(s,vo,vi,caD,caH) = "+CD3D::cd3d(s,vo,vi,caD,caH));
  if ((!s.isZero() && CD3D::cd3d(s,vo,vi,caD,caH)) || Util::almost_equals(vo.z,vi.z)) {
    //fpln(" vertical_decision_vect: Part 1, s = "+f.sStr(s));
    return s;
  } else if (vo2.almostEquals(vi2) || s.isZero())  {
    //fpln(" vertical_decision_vect: Part 2, s = "+f.sStr(s));
    return v;
  } else if (s2.dot(v.vect2()) <= 0) {
    //fpln(" vertical_decision_vect: Part 3, s = "+f.sStr(s));
    return s.AddScal(CD2D::tcpa(s2,vo2,vi2),v);
  } else {
    //fpln(" vertical_decision_vect: Part 4, s = "+f.sStr(s));
    return s;
  }
}

// Compute an absolute repulsive vertical direction
int CriteriaCore::losr_vs_dir(const Vect3& s, const Vect3& vo, const Vect3& vi,
    double caD, double caH, std::string ownship, std::string traffic) {
  int rtn = breakSymmetry(vertical_decision_vect(s,vo,vi,caD,caH),ownship,traffic);
  //fpln(" >>>>>>>>>>> losr_vs_dir: s.z = "+s.z+ " rtn = "+rtn);
  return rtn;
}

//
//Vect2 v = vo.Sub(vi);
//Vect2 nv = nvo.Sub(vi);
//boolean rtn = !s.isZero() && !nv.isZero() &&
//  eps*s.det(v) <= 0 && eps*s.det(nv) <= 0
//  && ((s.dot(v) < 0 &&  eps*nv.det(v) < 0)
//    || (s.dot(v) >= 0 && (!v.isZero() || s.dot(nv) >= 0) &&
//        (v.isZero() || s.dot(nv) > s.dot(v))));
////printRepulsiveCriteria2DTerms(s,vo,vi,nvo,eps);
//return rtn;

bool CriteriaCore::horizontal_los_criterion(const Vect2& s, const Vect2& vo, const Vect2& vi,const Vect2& nvo, int eps) {
  Vect2 v = vo.Sub(vi);
  Vect2 nv = nvo.Sub(vi);
  bool rtn = !s.isZero() && !nv.isZero() &&
      eps*s.det(v) <= 0 && eps*s.det(nv) <= 0
      && ((s.dot(v) < 0 &&  eps*nv.det(v) < 0)
          || (s.dot(v) >= 0 && (!v.isZero() || s.dot(nv) >= 0) &&
              (v.isZero() || s.dot(nv) > s.dot(v))));
  return rtn;
}


void CriteriaCore::printRepulsiveCriteria2DTerms(const Vect2& s, const Vect2& vo, const Vect2& vi,const Vect2& nvo, int eps) {
  Vect2 v = vo.Sub(vi);
  Vect2 nv = nvo.Sub(vi);
  bool rtn = eps*s.det(v) <= 0 && eps*s.det(nv) <= 0
      && (((s.dot(v) < 0 &&  eps*nv.det(v) < 0))
          || (s.dot(v) >= 0 && s.dot(nv) > s.dot(v)));
  fpln("#### repulsiveCriteria, nvo = "+fvStr2(nvo)+" vo = "+fvStr2(vo)+" vi = "+fvStr2(vi));
  fpln("#### repulsiveCriteria: s = "+fsStr(s)+" eps*s.det(v) <= 0 = "+bool2str(eps*s.det(v) <= 0)+" eps*s.det(nv) <= 0 = "+bool2str(eps*s.det(nv) <= 0));
  fpln("#### repulsiveCriteria: s.dot(v) < 0 = "+bool2str(s.dot(v) < 0)+ "  eps*nv.det(v) < 0 = "+bool2str(eps*nv.det(v) < 0));
  fpln("#### repulsiveCriteria: eps = "+Fmi(eps)+ " s.dot(nv) >= 0 = "+bool2str(s.dot(nv) >=0));
  fpln("#### repulsiveCriteria: (s.dot(v) >=0 && s.dot(nv) >s.dot(v)) = "+bool2str((s.dot(v) >=0 && s.dot(nv) >s.dot(v))));
  fpln("#### repulsiveCriteria: s.det(v) = "+Fm4(s.det(v))+"s.dot(v) = "+Fm4(s.dot(v))+"  nv.det(v) =" +Fm4(nv.det(v))+"  s.dot(nv) ="+Fm4(s.dot(nv)));
  fpln("#### rtn = "+Fmi(rtn));
}


void CriteriaCore::printRepulsiveCriteriaTerms(const Vect3& s, const Vect3& vo, const Vect3& vi,const Vect3& nvo, int eps) {
  printRepulsiveCriteria2DTerms(s.vect2(),vo.vect2(),vi.vect2(),nvo.vect2(),eps);
}


bool CriteriaCore::horizontalRepulsiveCriteria(const Vect3& s3, const Velocity& vo3, const Velocity& vi3, const Velocity& nvo3, int eps) {
  return horizontal_los_criterion(s3.vect2(),vo3.vect2(), vi3.vect2(),nvo3.vect2(),eps);
}


bool CriteriaCore::vs_bound_crit(const Vect3& s, const Vect3& v, const Vect3& nv, int eps) {
  Vect2 v2 = v.vect2();
  bool rtn;
  if (eps*v.z > 0)
    rtn = eps*nv.z > eps*v.z && -eps*v.z*nv.vect2().dot(v2) + eps*nv.z*v2.sqv() >= 0;
  else
    rtn = eps*nv.z >= 0;
  //fpln(">>>>>>>>>>>>> vs_bound_crit: eps = "+Fmi(eps)+" rtn = "+bool2str(rtn));
  return rtn;
}

double CriteriaCore::min_rel_vert_speed(double sz, double vz, int eps, double minrelvs) {
  double rtn;
  if (eps*vz <= 0)
    rtn = minrelvs;
  else
    rtn = std::max(minrelvs,std::abs(vz));
  //fpln(">>>>> min_rel_vert_speed: rtn = "+Units::str("fpm",rtn));
  return rtn;
}

bool CriteriaCore:: vertical_los_criterion(const Vect3& s, const Vect3& v, const Vect3& nv, int eps, double H, double minrelvs) {
  return std::abs(s.z) < H &&
      vs_bound_crit(s,v,nv,eps) &&
      eps*nv.z >= min_rel_vert_speed(s.z,v.z,eps,minrelvs);
}

// caD and caH are the dimater and height of a collision avoidance zone, e.g., 350ft x 100 ft
// minrelvs is a minium relative vertical speed
bool CriteriaCore::verticalRepulsiveCriterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo,
    double H, double minrelvs, int epsh, int epsv) {
  bool rtn =  vertical_los_criterion(s,vo.Sub(vi),nvo.Sub(vi),epsv,H,minrelvs);
  //fpln(">>>>>>>>>>>>> vertical_los_repulsive_criterion: ownship = "+ownship+" traffic = "+traffic+" eps = "+Fmi(eps)+" rtn = "+bool2str(rtn));
  return rtn;
}


/** Perform a symmetry calculation */
int CriteriaCore::breakSymmetry(const Vect3& s, std::string ownship, std::string traffic) {
  if (Util::almost_equals(s.z,0)) {
    //fpln(" ^^ BEFORE ownship = "+ownship);
    reverse(ownship.begin(), ownship.end());
    reverse(traffic.begin(), traffic.end());
    //fpln(" ^^ AFTER ownship = "+ownship);
    return Util::less_or_equal(ownship, traffic) ? 1 : -1;
  } else if (s.z > 0) {
    return 1;
  } else {
    return -1;
  }
}



static bool trkChanged(Velocity vo, Velocity nvo) {
  return std::abs(vo.trk() - nvo.trk()) > Units::from("deg",0.001);
}


static bool gsChanged(Velocity vo, Velocity nvo) {
  return std::abs(vo.gs() - nvo.gs()) > Units::from("kn",0.001);
}

static bool vsChanged(Velocity vo, Velocity nvo) {
  return std::abs(vo.vs() - nvo.vs()) > Units::from("fpm",0.001);
}


bool CriteriaCore::criteria(const Vect3& s, const Velocity&  vo, const Velocity&  vi, const Velocity& nvo,
    double minRelVs, double D, double H, int epsh, int epsv) {
  if (horizontal_los(s.vect2(),D) && vertical_los(s.z,H)) {
    bool horizChange = trkChanged(vo,nvo) || gsChanged(vo,nvo);
    bool vertChange = vsChanged(vo,nvo);
    bool vlc, hlc;
    //		if (ACCoRDConfig::LosRepulsiveCrit)
    vlc = verticalRepulsiveCriterion(s,vo,vi,nvo,H, minRelVs, epsh, epsv);
    //		else
    //			vlc = OldLosCriteria::vertical_los_criterion(s,vo,vi, 90, nvo,H, epsv);
    //		if (ACCoRDConfig::LosRepulsiveCrit) {
    //int epsH = horizontalCoordination(s.vect2(),vo.Sub(vi).vect2());
    hlc = horizontalRepulsiveCriteria(s,  vo, vi, nvo, epsh);
    //		} else {
    //			hlc = OldLosCriteria::horizontal_los_criterion(s.vect2(),vo.vect2(),vi.vect2(),120, nvo.vect2(), D);
    //		}
    if (horizChange && vertChange) return hlc && vlc;
    else if (horizChange) return hlc;
    else if (vertChange) return vlc;
    else return (hlc || vlc);
  } else {
    //std::cout << "criteria case 2" << std::endl;
    Velocity v = Velocity::make(vo.Sub(vi));
    //		int epsH = horizontalCoordination(s.vect2(),v.vect2());
    //		int epsV = verticalCoordination(s,v,D,epsh, epsv);
    Velocity nv = Velocity::make(nvo.Sub(vi));
    //std::cout << s.toString() << " " << v.toString() << " " << epsH << " " << epsV << " " << nv.toString() << " " << D << " " << H << std::endl;
    return criterion_3D(s,v,epsh,epsv,nv,D,H);
  }
}



// -------------------- repulsive iterative search directions --------------------------------


// Compute a new track only vector that is one step to the dir of vo
// dir = 1 is right
Vect2 CriteriaCore::incr_trk_vect(const Vect2& vo, double step, int dir) {
  Vect2 rtn = vo.Scal(cos(dir*step)).Add(vo.PerpR().Scal(sin(dir*step)));
  return rtn;
}

int CriteriaCore::losr_trk_iter_dir(const Vect2& s, const Vect2& vo, const Vect2& vi, double step, int eps) {
  if (horizontal_los_criterion(s,vo,vi,incr_trk_vect(vo,step,1),eps))
    return 1;
  if (horizontal_los_criterion(s,vo,vi,incr_trk_vect(vo,step,-1),eps))
    return -1;
  return 0;
}

int CriteriaCore::trkSearchDirection(const Vect3& s, const Vect3& vo, const Vect3& vi, int eps) {
  return losr_trk_iter_dir(s.vect2(),vo.vect2(),vi.vect2(),Units::from("deg",1), eps);
}

Vect2 CriteriaCore::incr_gs_vect(const Vect2& vo, double step, int dir) {
  double normvo = vo.norm();
  return vo.Scal((normvo+dir*step)/normvo);
}

int CriteriaCore::losr_gs_iter_dir(const Vect2& s, const Vect2& vo, const Vect2& vi, double mings, double maxgs, double step, int eps) {
  if (vo.norm() + step <= maxgs && horizontal_los_criterion(s,vo,vi,incr_gs_vect(vo,step,1),eps))
    return 1;
  if (vo.norm() - step >= mings && horizontal_los_criterion(s,vo,vi,incr_gs_vect(vo,step,-1),eps))
    return -1;
  return 0;
}

int CriteriaCore::gsSearchDirection(const Vect3& s, const Vect3& vo, const Vect3& vi, int eps) {
  double mings = 0; // Units::from("kn",150);
  double maxgs = DBL_MAX; // Units::from("kn",700);
  return losr_gs_iter_dir(s.vect2(),vo.vect2(),vi.vect2(),mings, maxgs, Units::from("kn",1), eps);
}

int CriteriaCore::vsSearchDirection(int epsv) {
  return epsv;
}

int CriteriaCore::dataVsRateEpsilon(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsv, double vsRate){
  int trafSrchDir = vsSearchDirection(epsv);
  int absDir = -1;
  if (vsRate >= 0) absDir = 1;
  if (absDir == trafSrchDir) return epsv;
  else return -epsv;
}



int CriteriaCore::dataTurnEpsilon(const Vect3& s, const Velocity& vo, const Velocity& vi, int epsh, double trackRate){
  int trafSrchDir = trkSearchDirection(s.Neg(), vi, vo, epsh);
  int absDir = -1;
  if (trackRate >= 0) absDir = 1;
  if (absDir == trafSrchDir) return epsh;
  else return -epsh;
}


bool CriteriaCore::horizontal_new_repulsive_criterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo, int eps) {
  Vect2 s2 = s.vect2();
  Vect2 v = vo.Sub(vi).vect2();
  Vect2 nv = nvo.Sub(vi).vect2();
  bool rtn = !s.isZero() && !nv.isZero() &&
      eps*s2.det(v) <= 0 && eps*s2.det(nv) < 0
      && ((s2.dot(v) < 0 &&  eps*nv.det(v) < 0)
          || (s2.dot(v) >= 0
              && (!v.isZero() || s2.dot(nv) >= 0)
              && (v.isZero() || s2.dot(nv) > s2.dot(v))
              && eps*nv.det(v) <= 0));
  return rtn;
}


bool CriteriaCore::vertical_new_repulsive_criterion(const Vect3& s, const Vect3& vo, const Vect3& vi, const Vect3& nvo, int eps) {
  Vect3 v = vo.Sub(vi);
  Vect3 nv = nvo.Sub(vi);
  Vect2 v2 = v.vect2();
  return eps*nv.z > eps*v.z && -eps*v.z*nv.vect2().dot(v2) + eps*nv.z*v2.sqv() >= 0;
}


}
