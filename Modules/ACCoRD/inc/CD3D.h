/*
 * CD3D.h 
 * Release: ACCoRD++-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * CD3D is an algorithm for 3-D conflict *detection*.
 *
 * Unit Convention
 * ---------------
 * All units in this file are *internal*:
 * - Units of distance are denoted [d]
 * - Units of time are denoted     [t]
 * - Units of speed are denoted    [d/t]
 *
 * REMARK: X points to East, Y points to North. 
 *
 * Naming Convention
 * -----------------
 *   The intruder is fixed at the origin of the coordinate system.
 * 
 *   D  : Diameter of the protected zone [d]
 *   H  : Height of the protected zone [d]
 *   B  : Lower bound of lookahed time interval [t] (B >= 0)
 *   T  : Upper bound of lookahead time interval [t] (B < T)
 *   s  : Relative 3-D position of the ownship [d,d,d]
 *   vo : Ownship velocity vector [d/t,d/t,d/t]
 *   vi : Traffic velocity vector [d/t,d/t,d/t]
 * 
 * Functions
 * ---------
 * LoS : Check for 3-D loss of separation
 * detection : 3-D conflict detection with calculation of conflict interval 
 * cd3d      : Check for predicted conflict
 * 
 * Global variables (modified by detection)
 * ----------------
 * t_in  : Time to loss of separation
 * t_out : Time to recovery from loss of separation
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CD3D_H_
#define CD3D_H_

#include "CD2D.h"
#include "Vertical.h"
#include "Detection3D.h"
#include "LossData.h"

namespace larcfm {

class CD3D {


public:

  /**
   * Returns true is aircraft are in loss of separation (LoS) at time 0.
   *
   * @param s the relative position of the aircraft
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return true, if aircraft are in loss of separation
   */
  static bool LoS(const Vect3& s, const double D, const double H);

  /**
   * Returns true is aircraft are in loss of separation at time 0.
   *
   * @param so the relative position of the ownship aircraft
   * @param si the relative position of the traffic aircraft
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return true, if aircraft are in loss of separation
   */
  static bool lossOfSep(const Vect3& so, const Vect3& si, const double D, const double H);


  /**
   * Computes the conflict time interval in [B,T].
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time (B < T)
   * 
   * @return true, if the conflict time interval (t_in,t_out) is in [B,T].
   * The returned t_in and t_out values are truncated to be within [B,T]
   *
   */
  static LossData detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double B, const double T);

  /**
   * Computes the actual conflict times 
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return true, if there is a conflict time interval (t_in,t_out)
   */
  static LossData detectionActual(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H);

  /**
   * Determines if there is a conflict in the time interval [B,T]
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time  (B < T)
   * 
   * @return true, if there is a conflict in the time interval [B,T].
   */
  static bool cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double B, const double T);

  /**
   * Determines if there is a conflict in the time interval [0,T]
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the upper bound of the lookahead time (T > 0)
   * 
   * @return true, if there is a conflict in the time interval [0,T].
   */
  static bool cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double T);

  /**
   * Determines if there is a conflict in the time interval [0,...)
   *
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return true, if there is a conflict in the time interval [0,...)
   */
  static bool cd3d(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H);


  /**
   * Computes the time to cylindrical closest point of approach for the interval [0,...).
   * 
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * 
   * @return the time to cylindrical closest point of approach for the interval [0,...).
   */
  static double tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const double D, const double H);

  /**
   * Computes the time to cylindrical closest point of approach for the interval [B,T].
   *
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param B the the lower bound of the lookahead time (B >= 0)
   * @param T the upper bound of the lookahead time  (B < T)
   *
   * @return the time to cylindrical closest point of approach for the interval [B,T].
   */
  static double tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const double D, const double H, const double B, const double T);

  /**
   * Computes the time to cylindrical closest point of approach for the interval [0,T].
   *
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   * @param T the upper bound of the lookahead time  (T > 0)
   *
   * @return the time to cylindrical closest point of approach for the interval [0,T].
   */
  static double tccpa(const Vect3& s, const Vect3& vo, const Vect3& vi,
      const double D, const double H, const double T);


  };
  

}

#endif /* CD3D_H_ */
