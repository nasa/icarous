/*
 * CriticalVectors.h
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

#ifndef CRITICALVECTORS_H_
#define CRITICALVECTORS_H_

#include <list>
#include "CriticalVectors2D.h"
#include "Vertical.h"

namespace larcfm {
  class CriticalVectors {
  public:
    static std::list<Vect2> tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
                const double D, const double H, const double B, const double T);

    static std::list<Vect2> tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
                const double D, const double H, const double T);

    static std::list<Vect2> groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
                      const double D, const double H, const double B, const double T);

    static std::list<Vect2> groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
                      const double D, const double H, const double T);

    static std::list<double> verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
                     const double D, const double H, const double B, const double T);

    static std::list<double> verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
                     const double D, const double H, const double T);
  };

//std::list<Vect2> tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
//			const double D, const double H, const double B, const double T);
//
//std::list<Vect2> tracks(const Vect3& s, const Vect3& vo, const Vect3& vi,
//			const double D, const double H, const double T);
//
//std::list<Vect2> groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
//			      const double D, const double H, const double B, const double T);
//
//std::list<Vect2> groundSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
//			      const double D, const double H, const double T);
//
//std::list<double> verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
//				 const double D, const double H, const double B, const double T);
//
//std::list<double> verticalSpeeds(const Vect3& s, const Vect3& vo, const Vect3& vi,
//				 const double D, const double H, const double T);

}

#endif /* CRITICALVECTORS_H_ */
