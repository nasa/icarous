/*
 * CriticalVectors2D.h
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

#ifndef CRITICALVECTORS2D_H_
#define CRITICALVECTORS2D_H_

#include <list>
#include "Consts.h"
#include "Horizontal.h"

namespace larcfm {

class CriticalVectors2D {
public:
  static std::list<Vect2> tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
              const double D, const double B, const double T);
  
  static std::list<Vect2> tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
              const double D, const double T);
  
  static std::list<Vect2> groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
                const double D, const double B, const double T);
  
  static std::list<Vect2> groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
                const double D, const double T);
};

//  std::list<Vect2> tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
//			  const double D, const double B, const double T);
//
//  std::list<Vect2> tracks(const Vect2& s, const Vect2& vo, const Vect2& vi,
//			  const double D, const double T);
//
//  std::list<Vect2> groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
//				const double D, const double B, const double T);
//
//  std::list<Vect2> groundSpeeds(const Vect2& s, const Vect2& vo, const Vect2& vi,
//				const double D, const double T);
  
}

#endif /* CRITICALVECTORS2D_H_ */
