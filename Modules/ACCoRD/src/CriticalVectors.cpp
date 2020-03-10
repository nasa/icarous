/*
 * CriticalVectors.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CriticalVectors.h"
#include "Util.h"
#include <cmath>

namespace larcfm {
  
  std::list<Vect2> CriticalVectors::tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
			  const double D, const double H, const double B, const double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    
    if (Util::almost_equals(vo.z,vi.z) && std::abs(s.z) < H) {
      return CriticalVectors2D::tracks(s2,vo2,vi2,D,B,T);
    } else if (!Util::almost_equals(vo.z,vi.z)) {
      double b = Util::max(Vertical::Theta_H(s.z,vo.z-vi.z,larcfm::Entry,H),B);
      double t = Util::min(Vertical::Theta_H(s.z,vo.z-vi.z,larcfm::Exit,H),T);
      if (b < t) {
        return CriticalVectors2D::tracks(s2,vo2,vi2,D,b,t);
      }
    }
    std::list<Vect2> trks;
    return trks;
  }
  
  std::list<Vect2> CriticalVectors::tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
			  const double D, const double H, const double T) {
    return tracks(s,vo,vi,D,H,0,T);
  }

  std::list<Vect2> CriticalVectors::groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
				const double D, const double H, const double B, const double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    if (Util::almost_equals(vo.z,vi.z) && std::abs(s.z) < H) {
      return CriticalVectors2D::groundSpeeds(s2,vo2,vi2,D,B,T);
    } else if (!Util::almost_equals(vo.z,vi.z)) {
      double b = Util::max(Vertical::Theta_H(s.z,vo.z-vi.z,larcfm::Entry,H),B);
      double t = Util::min(Vertical::Theta_H(s.z,vo.z-vi.z,larcfm::Exit,H),T);
      if (b < t) {
        return CriticalVectors2D::groundSpeeds(s2,vo2,vi2,D,b,t);
      }
    }
    std::list<Vect2> gss;
    return gss;
  }

  std::list<Vect2> CriticalVectors::groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
				const double D, const double H, const double T) {
    return groundSpeeds(s,vo,vi,D,H,0,T);
  }

  std::list<double> verticalSpeeds1D(const double sz, const double viz,
				     const double H, const double B, const double T) {
    std::list<double> vss;
    Vertical nvoz;
    if (std::abs(sz) < H && B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
        nvoz = Vertical::vs_circle_at(sz,viz,B,eps,larcfm::Exit,H);
        if (!nvoz.undef()) 
          vss.push_back(nvoz.z);
      }
    } else if (std::abs(sz) >= H && B > 0) {
      nvoz = Vertical::vs_circle_at(sz,viz,B,-sign(sz),larcfm::Exit,H);
      if (!nvoz.undef()) 
        vss.push_back(nvoz.z);
    }
    if (std::abs(sz) >= H) {
      nvoz = Vertical::vs_circle_at(sz,viz,T,sign(sz),larcfm::Entry,H);
      if (!nvoz.undef()) 
        vss.push_back(nvoz.z);
    }
    return vss;
  }

  std::list<double> CriticalVectors::verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
				   const double D, const double H, const double B, const double T) {
    Vect2 s2  = s.vect2();
    Vect2 vo2 = vo.vect2();
    Vect2 vi2 = vi.vect2();
    
    if (vo2.almostEquals(vi2) && s2.sqv() < sq(D)) {
      return verticalSpeeds1D(s.z,vi.z,H,B,T);
    } else {
      Vect2 v2 = vo2 - vi2;
      if (Horizontal::Delta(s2,v2,D) > 0) {
        double b = Util::max(Horizontal::Theta_D(s2,v2,larcfm::Entry,D),B);
        double t = Util::min(Horizontal::Theta_D(s2,v2,larcfm::Exit,D),T);
        if (b < t) {
          return verticalSpeeds1D(s.z,vi.z,H,b,t); 
        }
      }
    }
    std::list<double> vss;
    return vss;
  }

  std::list<double> CriticalVectors::verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
				   const double D, const double H, const double T) {
    return verticalSpeeds(s,vo,vi,D,H,0,T);
  }
  
}
