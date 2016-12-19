/*
 * CriticalVectors.h
 *
 * Contact: George Hagen
 * Organization: NASA/Langley Research Center
 *
 * This class computes the critical vectors for computing bands with traffic intent.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CRITICALVECTORSSI_H_
#define CRITICALVECTORSSI_H_

#include <list>
#include "Plan.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"

namespace larcfm {

class CriticalVectorsSI {
public:
  static std::list<Vect2> tracks(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
              const double D, const double H, const double B, const double T);

  static std::list<Vect2> groundSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
                    const double D, const double H, const double B, const double T);

  static std::list<double> verticalSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
                   const double D, const double H, const double B, const double T);


  static std::list<Vect2> tracksLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
                            const double D, const double H, const double B, const double T);

  static std::list<Vect2> groundSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
                                  const double D, const double H, const double B, const double T);

  static std::list<double> verticalSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
                                     const double D, const double H, const double B, const double T);
};

//std::list<Vect2> tracks(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
//			const double D, const double H, const double B, const double T);
//
//std::list<Vect2> groundSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
//			      const double D, const double H, const double B, const double T);
//
//std::list<double> verticalSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
//				 const double D, const double H, const double B, const double T);
//
//
//std::list<Vect2> tracksLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
//                          const double D, const double H, const double B, const double T);
//
//std::list<Vect2> groundSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
//                                const double D, const double H, const double B, const double T);
//
//std::list<double> verticalSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
//                                   const double D, const double H, const double B, const double T);
  

}

#endif /* CRITICALVECTORSSI_H_ */
