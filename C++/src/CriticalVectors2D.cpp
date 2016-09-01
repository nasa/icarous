/*
 * CriticalVectors2D.cpp
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

#include "CriticalVectors2D.h"

namespace larcfm {
  
  std::list<Vect2> CriticalVectors2D::tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
			  const double D, const double B, const double T) {
    TangentLine nv;
    std::list<Vect2> trks;
    Horizontal nvo;
    if (Horizontal::horizontal_sep(s,D)) {
      for (int eps=-1;eps <= 1; eps+=2) {
	nv = TangentLine(s,D,eps);
	nvo = Horizontal::trk_line_irt(nv,vo,vi,1);
	if (!nvo.undef())
	  trks.push_back(nvo);
	nvo = Horizontal::trk_line_irt(nv,vo,vi,-1);
	if (!nvo.undef())
	  trks.push_back(nvo);
	nvo = Horizontal::trk_only_circle(s,vo,vi,T,larcfm::Entry,eps,D);
	if (!nvo.undef())
	  trks.push_back(nvo);
      }
    }
    if (B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
	nvo = Horizontal::trk_only_circle(s,vo,vi,B,larcfm::Exit,eps,D);
	if (!nvo.undef())
	  trks.push_back(nvo);
      }
    }
    return trks;
  }
  
  std::list<Vect2> CriticalVectors2D::tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
			  const double D, const double T) {
    return tracks(s,vo,vi,D,0,T);
  }
  
  std::list<Vect2> CriticalVectors2D::groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
				const double D, const double B, const double T) {
    TangentLine nv;
    std::list<Vect2> gss;
    Horizontal nvo;
    if (Horizontal::horizontal_sep(s,D)) {
      for (int eps=-1;eps <= 1; eps+=2) {
	nv = TangentLine(s,D,eps);
	nvo = Horizontal::gs_line(nv,vo,vi);
	if   (!nvo.undef())
	  gss.push_back(nvo);
	nvo = Horizontal::gs_only_circle(s,vo,vi,T,larcfm::Entry,eps,D);
	if (!nvo.undef())
	  gss.push_back(nvo);
      }
    }
    if (B > 0) {
      for (int eps=-1;eps <= 1; eps+=2) {
	nvo = Horizontal::gs_only_circle(s,vo,vi,B,larcfm::Exit,eps,D);
	if (!nvo.undef())
	  gss.push_back(nvo);
      }
    }
    return gss;
  }
  
  std::list<Vect2> CriticalVectors2D::groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
				const double D, const double T) {
    return groundSpeeds(s,vo,vi,D,0,T);
  }

}
