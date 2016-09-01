/*
 * Horizontal.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "ACCoRDConfig.h"
#include "Horizontal.h"
#include "Vertical.h"
#include <cmath>
#include <limits>
#include "Kinematics.h"
#include "VectFuns.h"
//#include "Repulsion.h"

namespace larcfm {

Horizontal::Horizontal() : Vect2(0.0,0.0) {
  k = 0;
}

Horizontal::Horizontal(const Vect2& v) : Vect2(v) {
  k = 1;
  //x = v.x;
  //y = v.y;
}

Horizontal::Horizontal(double kk, const Vect2& v) : Vect2(v) {
  k = kk;
  //x = v.x;
  //y = v.y;
}

const Horizontal Horizontal::NoHorizontalSolution = Horizontal();

bool Horizontal::undef() const {
  return isZero();
}



Horizontal Horizontal::best_horizontal(const Vect2& vo,const Horizontal& v1, const Horizontal& v2) {
  if (v1.undef())
    return v2;
  else if (v2.undef() || v1.leq(v2,vo))
    return v1;
  return v2;
}

// Time of closest approach (horizontal plane)
double Horizontal::tcpa(const Vect2& s, const Vect2& v) {
  if (!v.isZero())
    return -(s*v)/v.sqv();
  return NaN;
}

double Horizontal::dcpa(const Vect2& s, const Vect2& v) {
    return v.ScalAdd(tcpa(s,v),s).norm();
}

/* Horizontal miss distance within lookahead time */
double Horizontal::hmd(const Vect2& s, const Vect2& v, double T) {
  double t = 0;
  if (s.dot(v) < 0) {
    // aircraft are horizontally converging
    t = std::min(tcpa(s,v),T);
  }
  return v.ScalAdd(t,s).norm();
}

// Computes times when (s,v) intersects circle of radius D
// eps = -1 : larcfm::Entry
// eps =  1 : larcfm::Exit
double Horizontal::Theta_D(const Vect2& s, const Vect2& v,
	       const int eps, const double D) {
  double a = v.sqv();
  double b = s*v;
  double c = s.sqv()-sq(D);
  return root2b(a,b,c,eps);
}

double Horizontal::Delta(const Vect2& s, const Vect2& v, const double D) {
  return sq(D)*v.sqv() - sq(s.det(v));
}

bool Horizontal::almost_horizontal_los(const Vect2& s, const double D) {
  double sqs = s.sqv();
  double sqD = sq(D);
  return !Util::almost_equals(sqs,sqD) && sqs < sqD;
}

bool Horizontal::horizontal_sep(const Vect2& s, const double D) {
  return s.sqv() >= sq(D);
}

bool Horizontal::horizontal_dir(const Vect2& s, const Vect2& v, const int dir) {
  return dir*(s*v) >= 0;
}

bool Horizontal::horizontal_dir_at(const Vect2& s, const Vect2& v, const double t,
		       const int dir) {
  Vect2 sp = s+v*t;
  return horizontal_dir(sp,v,dir);
}


bool Horizontal::horizontal_entry(const Vect2& s, const Vect2& v) {
 return horizontal_dir(s,v,-1);
}



Vect2 Horizontal::Vdir(const Vect2& s, const Vect2& v) {
  Vect2 ps = s.PerpR();
  return ps * sign(ps*v);
}

Vect2 Horizontal::W0(const Vect2& s, const double j) {
  if (!s.isZero())
    return s * (j/s.sqv());
  return Vect2::ZERO;
}

/* Solve the following equation on k and l:
 *   k*nv = l*vo-vi.
 */
Horizontal Horizontal::gs_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  double det_vo_v = vo.det(nv);
  if (det_vo_v != 0) {
    double l = std::max(0.0,vi.det(nv)/det_vo_v);
    double k = vi.det(vo)/det_vo_v;
    Horizontal gso = Horizontal(vo*l);
    gso.k = k;
    return gso;
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::gs_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  Horizontal gso = gs_only_line(nv,vo,vi);
  if (gso.k < 0) {
    gso = Vect2::ZERO;
  }
  return gso;
}

Horizontal Horizontal::gs_only_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
		       const double j) {
  return gs_only_line(Vdir(u,vo-vi),vo,vi+W0(u,j));
}

Horizontal Horizontal::gs_only_vertical(const Vect2& s, const Vect2& vo, const Vect2& vi,
			    const double th, const int dir, const double D) {
  Vect2 v = vo-vi;
  if (Delta(s,v,D) > 0) {
    double td = Theta_D(s,v,dir,D);
    if (td > 0) {
      Vect2 p = s.AddScal(td, v); //s + v*td;
      return gs_only_dot(p*th,vo,vi,sq(D)-s*p);
    }
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::gs_vertical(const Vect3& s, const Vect3& vo, const Vect3& vi,
		       const TangentLine& l, const int epsv,
		       const double D, const double H) {
  if (!Util::almost_equals(vo.z,vi.z)) {
    Vect3  v   = vo-vi;
    int    dir = std::abs(s.z) >= H ? epsv*sign(s.z) : larcfm::Entry;
    double t   = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    if (t > 0 && epsv == sign(s.z + t*v.z)) {
      Horizontal nvo2 = gs_only_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,D);
      if (almost_horizontal_los(s.vect2(),D) || l.horizontal_criterion(nvo2-vo.vect2()))
	return nvo2;
    }
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::gs_only(const TangentLine& nv,const Vect3& s, const Vect3& vo,const Vect3& vi, int epsv,
		   const double D, const double H) {
  return best_horizontal(vo.vect2(),gs_line(nv,vo.vect2(),vi.vect2()),
			 gs_vertical(s,vo,vi,nv,epsv,D,H));
}

Horizontal Horizontal::gs_only_circle(const Vect2& s, const Vect2& vo, const Vect2& vi,
    const double t, const int dir, const int irt, const double D) {
  Vect2 w = s.AddScal(-t, vi); //s-vi*t;
  double a = sq(t)*vo.sqv();
  double b = t*(w*vo);
  double c = w.sqv()-sq(D);
  double l = root2b(a,b,c,irt);
  if (! ISNAN(l)) {
    Vect2 nvo = vo*std::max(l,0.0);
    if (horizontal_dir_at(s,nvo-vi,t,dir))
      return Horizontal(nvo);
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::gs_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
		     const int dir, const int irt, const double D, const double H) {
  if (!Util::almost_equals(vo.z,vi.z)) {
    double t = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    return gs_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
  }
  return NoHorizontalSolution;
}

/* Solve the following equation on k:
 * || k*nv + vi || = || vo ||.
 */
Horizontal Horizontal::trk_only_line_irt(const Vect2& nv,const Vect2& vo,const Vect2& vi,
			     const int irt) {
  double a = nv.sqv();
  double b = nv * vi;
  double c = vi.sqv() - vo.sqv();
  double k = root2b(a,b,c,irt);
  if (! ISNAN(k)) {
    //Horizontal trko = Horizontal(nv);
    //trko.k = k;
    //trko.scal(k);
    //trko.add(vi);
    //return trko;
    return Horizontal(k, nv.ScalAdd(k,vi));
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::trk_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  return best_horizontal(vo,trk_only_line_irt(nv,vo,vi,1),
      trk_only_line_irt(nv,vo,vi,-1));
}

Horizontal Horizontal::trk_line_irt(const Vect2& nv,const Vect2& vo,const Vect2& vi,
    const int irt) {
  Horizontal trko = trk_only_line_irt(nv,vo,vi,irt);
  if (trko.k < 0) {
    trko = Horizontal(trko.k, Vect2::ZERO);
    //trko.zero();
  }
  return trko;
}

Horizontal Horizontal::trk_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  return best_horizontal(vo,trk_line_irt(nv,vo,vi,1),
      trk_line_irt(nv,vo,vi,-1));
}

Horizontal Horizontal::trk_only_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
    const double j,const int irt) {
  return trk_only_line_irt(Vdir(u,vo-vi),vo,vi+W0(u,j),irt);
}

Horizontal Horizontal::trk_only_vertical(const Vect2& s,const Vect2& vo,const Vect2& vi,
			     const double th,const int dir,const int irt,const double D) {
  Vect2 v = vo-vi;
  if (Delta(s,v,D) > 0) {
    double td = Theta_D(s,v,dir,D);
    if (td > 0) {
      Vect2 p = s + v*td;
      return trk_only_dot(p*th,vo,vi,sq(D)-s*p,irt);
    }
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::trk_vertical_irt(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
			    const int epsv,const int irt,const double D,const double H) {
  if (!Util::almost_equals(vo.z,vi.z)) {
    Vect3  v   = vo-vi;
    int    dir = std::abs(s.z) >= H ? epsv*sign(s.z) : larcfm::Entry;
    double t   = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    if (t > 0 && epsv == sign(s.z + t*v.z)) {
      Horizontal nvo2 = trk_only_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
      if (almost_horizontal_los(s.vect2(),D) || l.horizontal_criterion(nvo2-vo.vect2()))
	return nvo2;
    }
  }
  return NoHorizontalSolution;
}

//Horizontal Horizontal::repulsiveTrk(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed) {
//  Horizontal rtn = Horizontal::NoHorizontalSolution;
//  int sgn = Repulsion::repulsiveTrkSearchDirection(s, vo, vi);                                          // Don't use CriteriaCore version
//  double trk0 = vo.vect2().track();
//  double k = 0;
//  if (sgn != 0) {
//    double voGs = vo.vect2().norm();                        // .groundSpeed()
//    double voVs = vo.z;                                     //. verticalSpeed();
//    //double minRelSpeed = ACCoRDConfig::minRelSpeedLoSSearch; // Units::from("kn",100);
//    double checkSpeed = minRelSpeed;
//    Vect2 nvoWithSpeed = Vect2::mkTrkGs(trk0+sgn*M_PI/2.0,voGs);
//    bool origDiv = s.vect2().dot(vo.Sub(vi).vect2()) >= 0;
//    bool found = false;
//    Vect3 prevNvo = Velocity::ZEROV;
//    for (double trkDelta = 0; trkDelta <= M_PI; trkDelta = trkDelta + Units::from("deg",1)) {
//      double nvoTrk = trk0+sgn*trkDelta;
//      Velocity nvo;
//      // direction will be determined by criteria
//      if (trkDelta == 0) nvo = Velocity::make(vo);
//      else nvo = Velocity::mkTrkGsVs(nvoTrk,voGs,voVs);
//      //f.pln("@@@@ repulsiveTrk: nvTrk = "+Units::str("deg",nvoTrk)+" rel Speed = "+Units::str("kn",nvo.Sub(vo).vect2().norm()));
//      found = Repulsion::noLongerTrkRepulsive(sgn, s, nvo, vi) || VectFuns::divergentHorizGt(s.vect2(), nvo.vect2(), vi.vect2(), minRelSpeed) ;
//      if (origDiv) found = found && s.vect2().dot(nvo.Sub(vi).vect2()) > s.vect2().dot(vo.Sub(vi).vect2());
//      if (found) {
//          if (origDiv)
//              rtn = Horizontal(k, nvo.vect2());
//          else
//              rtn = Horizontal(k, prevNvo.vect2());
//        //f.pln("@@@@ repulsiveTrk: Trk Resolution found, rtn = "+f.vStr2(rtn));
//        break;
//      }
//      prevNvo = nvo;
//      // THIS SECTION WILL SOON BE COMMENTED OUT
//      double relSpeed = nvo.Sub(vo).vect2().norm();
//      if (relSpeed > checkSpeed) {
//          nvoWithSpeed =  Vect2::mkTrkGs(nvoTrk,voGs);
//          checkSpeed = 1E20;
//      }
//    }//for
//    if (!found) {
//      // entire half circle is repulsive, use first track that was at least minRelSpeed
//      if (!origDiv)
//         rtn = Horizontal(k, nvoWithSpeed);
//    }
//  }
//  //f.pln(" @@@@ repulsiveTrk: Horizontal::repulsiveTrk: rtn = "+f.vStr2(rtn));
//  return rtn;
//}

//// No LOnger Used
//Horizontal Horizontal::repulsiveGs(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed, double minGs, double maxGs) {
//  Horizontal rtn = Horizontal::NoHorizontalSolution;
//  int sgn = Repulsion::repulsiveGsSearchDirection(s, vo, vi);                                          // Don't use CriteriaCore version
//  //f.pln("######################### repulsiveGsSearchDirection = "+sgn);
//  Velocity nvo = Velocity::ZEROV;
//  if (sgn != 0) {
//    double voTrk = vo.vect2().track();
//    double voGs = vo.vect2().norm();                        // .groundSpeed()
//    double voVs = vo.z;                                     //. verticalSpeed();
//    //double minRelSpeed = ACCoRDConfig::minRelSpeedLoSSearch; // Units::from("kn",100);
//    bool found = false;
//    for (double gsDelta = 0; gsDelta < maxGs ; gsDelta = gsDelta + Units::from("kn",10)) {
//      double nvoGs = voGs + sgn*gsDelta;
//      if (gsDelta == 0) nvo = Velocity::make(vo);
//      else
//        nvo = Velocity::mkTrkGsVs(voTrk,nvoGs,voVs);
//      if ((nvoGs > maxGs) || (nvoGs < minGs)) break;
//      //f.pln("@@@@ repulsiveGs: nvoGs = "+Units::str("kn",nvoGs)+" rel Speed = "+Units::str("kn",nvo.Sub(vo).vect2().norm()));
//      //bool notrepulsive = Repulsion::noLongerGsRepulsive(sgn, s, nvo, vi);
//      found = VectFuns::divergentHorizGt(s.vect2(), nvo.vect2(), vi.vect2(), minRelSpeed) ;
//      if (found) {
//        rtn = Horizontal(0, nvo.vect2());
//        //f.pln("@@@@ repulsiveGs: Gs Resolution found, rtn = "+f.vStr2(rtn));
//        break;
//      }
//    }//for
//    if (!found) {
//      double nGs;
//      if (sgn >= 0) nGs = (voGs + maxGs)/2.0;
//      else nGs = (voGs + minGs)/2.0;
//      nvo  =  Velocity::mkTrkGsVs(voTrk,nGs,voVs);
//    }
//    //double tau = Kinematics::tau(s.vect2(),nvo2,vi.vect2());
//    double distAtTau =  Kinematics::distAtTau(s,nvo,vi,true);
//    if (distAtTau > ACCoRDConfig::gsSearchLosDiscard)                    // $$$$$$$$$$$$$$$ PARAMETER $$$$$$$$$$$$$$$$$$$$
//         rtn = Horizontal(0, nvo.vect2());
//  }
//  //f.pln(" @@@@ Horizontal::repulsiveGs: rtn = "+f.vStr2(rtn));
//  return rtn;
//}



Horizontal Horizontal::trk_vertical(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
			const int epsv,const double D,const double H) {
  return best_horizontal(vo.vect2(),trk_vertical_irt(s,vo,vi,l,epsv,1,D,H),
			 trk_vertical_irt(s,vo,vi,l,epsv,-1,D,H));
}

Horizontal Horizontal::trk_only(const TangentLine& nv,const Vect3& s, const Vect3& vo,const Vect3& vi,
		    const int epsv,const double D, const double H) {
  return best_horizontal(vo.vect2(),trk_line(nv,vo.vect2(),vi.vect2()),
			 trk_vertical(s,vo,vi,nv,epsv,D,H));
}

Horizontal Horizontal::trk_only_circle(const Vect2& s, const Vect2& vo, const Vect2& vi,
			   const double t, const int dir, const int irt, const double D) {
  if (t > 0) {
    Vect2 w = s.AddScal(-t, vi); //s-vi*t;
    double e = (sq(D) - s.sqv() - sq(t)*(vo.sqv()-vi.sqv()))/(2*t);
    if (!s.almostEquals(vi*t)) {
      Vect2 nvo = trk_only_dot(w,vo,vi,e,irt);
      if (horizontal_dir_at(s,nvo-vi,t,dir))
        return Horizontal(nvo);
    }
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::trk_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
		      const int dir, const int irt, const double D, const double H) {
  if (!Util::almost_equals(vo.z, vi.z)) {
    double t = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    return trk_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
  }
  return NoHorizontalSolution;
}

/*
 * Solve the following equation on k and l:
 *   nv * (k*nv-v) = 0, where v = vo-vi.
 */
Horizontal Horizontal::opt_trk_gs_line(const Vect2& nv, const Vect2& vo,
    const Vect2& vi) {
  if (!nv.isZero()) {
    //Horizontal trkgs = Horizontal(nv);
    Vect2 v = vo - vi;
    double k = (nv * v)/nv.sqv();
    //trkgs.k = k;
    //trkgs.scal(k);
    //trkgs.add(vi);
    //return trkgs;
    return Horizontal(k, nv.ScalAdd(k,vi));
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::opt_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  Horizontal opt = opt_trk_gs_line(nv,vo,vi);
  if (opt.k < 0) {
    //opt.zero();
    opt = Vect2::ZERO;
  }
  return opt;
}

Horizontal Horizontal::opt_trk_gs(const TangentLine& nv,const Vect3& s,const Vect3& vo,const Vect3& vi,
		      const int epsv,const double D,const double H) {
  return best_horizontal(vo.vect2(),opt_trk_gs_line(nv,vo.vect2(),vi.vect2()),
			 opt_vertical(s,vo,vi,nv,epsv,D,H));
}

Horizontal Horizontal::opt_trk_gs_dot(const Vect2& u,const Vect2& vo,const Vect2& vi,
			  const double j) {
  return opt_trk_gs_line(Vdir(u,vo-vi),vo,vi+W0(u,j));
}

Horizontal Horizontal::opt_trk_gs_vertical(const Vect2& s,const Vect2& vo,const Vect2& vi,
			       const double th,const int dir,const double D) {
  Vect2 v = vo-vi;
  if (Delta(s,v,D) > 0) {
    double td = Theta_D(s,v,dir,D);
    if (td > 0) {
      Vect2 p = s.AddScal(td, s); //s + v*td;
      opt_trk_gs_dot(p*th,vo,vi,sq(D)-s*p);
    }
  }
  return NoHorizontalSolution;
}

Horizontal Horizontal::opt_vertical(const Vect3& s,const Vect3& vo,const Vect3& vi,const TangentLine& l,
			const int epsv,const double D,const double H) {
  if (!Util::almost_equals(vo.z,vi.z)) {
    Vect3  v   = vo-vi;
    int    dir = std::abs(s.z) >= H ? epsv*sign(s.z) : larcfm::Entry;
    double t   = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    if (t > 0 && epsv == sign(s.z + t*v.z)) {
      Horizontal nvo2 = opt_trk_gs_vertical(s.vect2(),vo.vect2(),vi.vect2(),t,dir,D);
      if (almost_horizontal_los(s.vect2(),D) || l.horizontal_criterion(nvo2-vo.vect2()))
	return nvo2;
    }
  }
  return NoHorizontalSolution;
}

}
