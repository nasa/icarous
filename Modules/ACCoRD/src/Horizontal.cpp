/*
 * Horizontal.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <algorithm>
#include <cmath>

#include "Horizontal.h"
#include "Util.h"
#include "Consts.h"
#include "Vertical.h"

namespace larcfm {

Horizontal::Horizontal() : Vect2(0.0,0.0), k(0.0) {}

Horizontal::Horizontal(const Vect2& v) : Vect2(v), k(1.0) {}

Horizontal::Horizontal(double kk, const Vect2& v) : Vect2(v), k(kk) {}

const Horizontal& Horizontal::NoHorizontalSolution() {
  static Horizontal* tmp = new Horizontal();
  return *tmp;
}

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
    t = Util::min(tcpa(s,v),T);
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
  return Vect2::ZERO();
}

/* Solve the following equation on k and l:
 *   k*nv = l*vo-vi.
 */
Horizontal Horizontal::gs_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  double det_vo_v = vo.det(nv);
  if (det_vo_v != 0) {
    double l = Util::max(0.0,vi.det(nv)/det_vo_v);
    double k = vi.det(vo)/det_vo_v;
    Horizontal gso = Horizontal(vo*l);
    gso.k = k;
    return gso;
  }
  return NoHorizontalSolution();
}

Horizontal Horizontal::gs_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  Horizontal gso = gs_only_line(nv,vo,vi);
  if (gso.k < 0) {
    return NoHorizontalSolution();
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
  return NoHorizontalSolution();
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
  return NoHorizontalSolution();
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
    Vect2 nvo = vo*Util::max(l,0.0);
    if (horizontal_dir_at(s,nvo-vi,t,dir))
      return Horizontal(nvo);
  }
  return NoHorizontalSolution();
}

Horizontal Horizontal::gs_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const int dir, const int irt, const double D, const double H) {
  if (!Util::almost_equals(vo.z,vi.z)) {
    double t = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    return gs_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
  }
  return NoHorizontalSolution();
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
    return Horizontal(k, nv.ScalAdd(k,vi));
  }
  return NoHorizontalSolution();
}

Horizontal Horizontal::trk_only_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  return best_horizontal(vo,trk_only_line_irt(nv,vo,vi,1),
      trk_only_line_irt(nv,vo,vi,-1));
}

Horizontal Horizontal::trk_line_irt(const Vect2& nv,const Vect2& vo,const Vect2& vi,
    const int irt) {
  Horizontal trko = trk_only_line_irt(nv,vo,vi,irt);
  if (trko.k < 0) {
    return NoHorizontalSolution();
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
  return NoHorizontalSolution();
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
  return NoHorizontalSolution();
}

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
  return NoHorizontalSolution();
}

Horizontal Horizontal::trk_circle(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const int dir, const int irt, const double D, const double H) {
  if (!Util::almost_equals(vo.z, vi.z)) {
    double t = Vertical::Theta_H(s.z,vo.z-vi.z,-dir,H);
    return trk_only_circle(s.vect2(),vo.vect2(),vi.vect2(),t,dir,irt,D);
  }
  return NoHorizontalSolution();
}

/*
 * Solve the following equation on k and l:
 *   nv * (k*nv-v) = 0, where v = vo-vi.
 */
Horizontal Horizontal::opt_trk_gs_line(const Vect2& nv, const Vect2& vo,
    const Vect2& vi) {
  if (!nv.isZero()) {
    Vect2 v = vo - vi;
    double k = (nv * v)/nv.sqv();
    return Horizontal(k, nv.ScalAdd(k,vi));
  }
  return NoHorizontalSolution();
}

Horizontal Horizontal::opt_line(const Vect2& nv,const Vect2& vo,const Vect2& vi) {
  Horizontal opt = opt_trk_gs_line(nv,vo,vi);
  if (opt.k < 0) {
    return NoHorizontalSolution();
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
  return NoHorizontalSolution();
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
  return NoHorizontalSolution();
}

/* Point in relative coordinates that is tangent to the circle of radius D, around
 * so, in the direction of the relative velocity vo-vi.
 */
Vect3 Horizontal::hmd_tangent_point(double D, const Vect3& v) {
  return v.Hat2D().PerpL().Scal(D);
}


}
