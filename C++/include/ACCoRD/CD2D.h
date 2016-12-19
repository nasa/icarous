/*
 * CD2D.h
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

#ifndef CD2D_H_
#define CD2D_H_

#include "Consts.h"
#include "Horizontal.h"
#include "LossData.h"

namespace larcfm {

/**
 * CD2D is an algorithm for 2-D conflict *detection*.<p>
 *
 * All units in this file are *internal*:
 * <ul>
 * <li> Units of distance are denoted [d]
 * <li> Units of time are denoted     [t]
 * <li> Units of speed are denoted    [d/t]
 * </ul>
 *
 * REMARK: X points to East, Y points to North. 
 *
 * Naming Convention<br>
 *   The intruder is fixed at the origin of the coordinate system.
 *<ul>
 *<li>   D  : Diameter of the protected zone [d]
 *<li>   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *<li>   T  : Upper bound of lookahead time interval [t] (B < T)
 *<li>   s  : Relative 2-D position of the ownship [d,d]
 *<li>   vo : Ownship velocity vector [d/t,d/t]
 *<li>   vi : Traffic velocity vector [d/t,d/t]
 *</ul>
 * 
 * Functions <br>
 *<ul>
 *<li> violation : Check for 2-D loss of separation
 *<li> detection : 2-D conflict detection with calculation of conflict interval
 *<li> cd2d      : Check for predicted conflict
 *</ul>
 * 
 */
class CD2D {
public:

  /**
   * detection(s,vo,vi,D,B,T) computes the horizontal conflict time interval
   * (t_in,t_out) in [B,T], where 0 <= B < T
   */
  static LossData detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D, const double B, const double T);

  /**
   * detection(s,vo,vi,D,T) computes the horizontal conflict time interval
   * (t_in,t_out) in [0,T], where 0 <= B < T.
   */
  static LossData detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D, const double T);

  /**
   * detection(s,vo,vi,D) computes the horizontal conflict time interval
   * (t_in,t_out) in [0,...).
   */
  static LossData detection(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D);

  /**
   * violation(s,D) IFF there is an horizontal loss of separation at current time
   */
  static bool violation(const Vect2& s, const double D);

  /**
   * cd2d(s,vo,vi,D,B,T) IFF there is an horizontal conflict in the time
   * interval [B,T], where 0 <= B < T.
   */
  static bool cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D, const double B, const double T);

  /**
   * cd2d(s,vo,vi,D,T) IFF there is an horizontal conflict in the time
   * interval [0,T], where T > 0.
   */
  static bool cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D, const double T);

  /**
   * cd2d(s,vo,vi,D) IFF there is an horizontal conflict in the time
   * interval [0,...).
   */
  static bool cd2d(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double D);

  /**
   * Returns the time to horizontal closest point of approach for s,vo,vi during the
   * interval [B,T], where 0 <= B < T.
   */
  static double tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double B, const double T);

  /**
   * Returns the time to horizontal closest point of approach for s,vo,vi, for
   * interval [0,T], where T > 0.
   */
  static double tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi,
      const double T);

  /**
   * Returns the time to horizontal closest point of approach for s,vo,vi, for
   * interval [0,...).
   */
  static double tcpa(const Vect2& s, const Vect2& vo, const Vect2& vi);

};

}

#endif /* CD2D_H_ */
