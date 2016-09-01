/*
 * CDxx.h
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

#include <cmath>

#include "CD3D.h"
#include "format.h"
#include "LossData.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Horizontal.h"

namespace larcfm {

bool CD3D::LoS(const Vect3& s, const double D, const double H) {
  return s.vect2().sqv() < sq(D) && std::abs(s.z) < H;
}

bool CD3D::lossOfSep(const Vect3& so, const Vect3& si, const double D, const double H) {
  return LoS(so.Sub(si),D,H);
}

LossData CD3D::detection(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H, const double B, const double T) {
  double t_in  = T+1;
  double t_out = B;
  if (B >= 0 && B < T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    double vz = vo.z-vi.z;
    if (vo2.almostEquals(vi2) && Horizontal::almost_horizontal_los(s2,D)) {
      if (!Util::almost_equals(vo.z,vi.z)) {
        t_in  = std::min(std::max(Vertical::Theta_H(s.z,vz,larcfm::Entry,H),B),T);
        t_out = std::max(std::min(Vertical::Theta_H(s.z,vz,larcfm::Exit,H),T),B);
      } else if (Vertical::almost_vertical_los(s.z,H)) {
        t_in  = B;
        t_out = T;
      }
    } else {
      Vect2 v2 = vo2 - vi2;
      if (Horizontal::Delta(s2,v2,D) > 0.0) {
        double td1 = Horizontal::Theta_D(s2,v2,larcfm::Entry,D);
        double td2 = Horizontal::Theta_D(s2,v2,larcfm::Exit,D);
        if (!Util::almost_equals(vo.z,vi.z)) {
          double tin  = std::max(td1,Vertical::Theta_H(s.z,vz,larcfm::Entry,H));
          double tout = std::min(td2,Vertical::Theta_H(s.z,vz,larcfm::Exit,H));
          t_in  = std::min(std::max(tin,B),T);
          t_out = std::max(std::min(tout,T),B);
        } else if (Vertical::almost_vertical_los(s.z,H)) {
          t_in  = std::min(std::max(td1,B),T);
          t_out = std::max(std::min(td2,T),B);
        }
      }
    }
  }
  return LossData(t_in, t_out);
}

LossData CD3D::detectionActual(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H) {
  double t_in  = PINFINITY;
  double t_out = NINFINITY;
  Vect2 s2  = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  double vz = vo.z-vi.z;
  if (vo2.almostEquals(vi2) && Horizontal::almost_horizontal_los(s2,D)) {
    if (!Util::almost_equals(vo.z,vi.z)) {
      t_in  = Vertical::Theta_H(s.z,vz,larcfm::Entry,H);
      t_out = Vertical::Theta_H(s.z,vz,larcfm::Exit,H);
    } else if (Vertical::almost_vertical_los(s.z,H)) {
      t_in  = NINFINITY;
      t_out = PINFINITY;
    }
  } else {
    Vect2 v2 = vo2 - vi2;
    if (Horizontal::Delta(s2,v2,D) > 0.0) {
      double td1 = Horizontal::Theta_D(s2,v2,larcfm::Entry,D);
      double td2 = Horizontal::Theta_D(s2,v2,larcfm::Exit,D);
      if (!Util::almost_equals(vo.z,vi.z)) {
        t_in  = std::max(td1,Vertical::Theta_H(s.z,vz,larcfm::Entry,H));
        t_out = std::min(td2,Vertical::Theta_H(s.z,vz,larcfm::Exit,H));
       } else if (Vertical::almost_vertical_los(s.z,H)) {
        t_in  = td1;
        t_out = td2;
      }
    }
  }
  return LossData(t_in, t_out);
}

bool CD3D::cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H, const double B, const double T) {
  if (B < 0 || B >= T) return false;
  Vect2 s2  = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();

  if (Util::almost_equals(vo.z,vi.z) && std::abs(s.z) < H) {
    return CD2D::cd2d(s.vect2(),vo2,vi2,D,B,T);
  }
  double vz = vo.z - vi.z;
  double m1 = std::max(-H-sign(vz)*s.z,B*std::abs(vz));
  double m2 = std::min(H-sign(vz)*s.z,T*std::abs(vz));
  if (!Util::almost_equals(vo.z,vi.z) && m1 < m2) {
    return CD2D::cd2d(s2*std::abs(vz),vo2,vi2,D*std::abs(vz),m1,m2);
  } else {
    return false;
  }
}

bool CD3D::cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi,  const double D, const double H, const double T) {
  return cd3d(s,vo,vi,D,H,0.0,T);
}

bool CD3D::cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi,  const double D, const double H) {
  return cd3d(s,vo,vi,D,H,0.0,PINFINITY);
}

double CD3D::tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H) {
  Vect3 v   = vo-vi;
  Vect2 s2  = s.vect2();
  Vect2 vo2 = vo.vect2();
  Vect2 vi2 = vi.vect2();
  Vect2 v2  = v.vect2();
  double mint = 0.0;
  double mind = s.cyl_norm(D,H);
  if (!vo2.almostEquals(vi2)) {
    double t = -(s2*v2)/v2.sqv();
    if (t > 0) {
      double d = v.ScalAdd(t,s).cyl_norm(D,H);
      if (d < mind) {
        mint = t;
        mind = d;
      }
    }
  }
  if (!Util::almost_equals(vo.z, vi.z)) {
    double t = -s.z/v.z;
    if (t > 0.0) {
      double d = v.ScalAdd(t,s).cyl_norm(D,H);
      if (d < mind) {
        mint = t;
        mind = d;
      }
    }
  }
  double a = v2.sqv()/sq(D) - sq(v.z/H);
  double b = (s2*v2)/sq(D) - (s.z*v.z)/sq(H);
  double c = s2.sqv()/sq(D) - sq(s.z/H);
  for (int eps = -1; eps <= 1; eps += 2) {
    double t = root2b(a,b,c,eps);
    if (!ISNAN(t) && t > 0) {
      double d = v.ScalAdd(t,s).cyl_norm(D,H);
      if (d < mind) {
        mint = t;
        mind = d;
      }
    }
  }
  return mint;
}

double CD3D::tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H, const double B, const double T) {
  double tau = tccpa(s,vo,vi,D,H);
  return std::min(std::max(B,tau),T);
}

double CD3D::tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi,
    const double D, const double H, const double T) {
  return tccpa(s,vo,vi,D,H,0,T);
}

}

