/*
 * CDSSCore.h
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef CDSSCORE_H_
#define CDSSCORE_H_

#include "Velocity.h"
#include "CDCylinder.h"
#include "Detection3D.h"
#include "string_util.h"
#include "EuclideanProjection.h"
#include "Detection3DAcceptor.h"

namespace larcfm {

/**
 * The Class CDSSCore.
 */
class CDSSCore : Detection3DAcceptor {
private:
  /** The conflict filter time */
  double filter;

  /** Time of (cylindrical) closest approach *. */
  double tca;

  double dtca;

  double t_in;

  double t_out;

  /** Aircraft relative position at time of closest approach. */
  Vect3 stca;

  /** CD3D object */
  Detection3D* cd;

  void init(double d, double h, double f, Detection3D* c);

public:

  CDSSCore(Detection3D* cd, double cdfilter);

  // needed because of pointer
  /** Copy constructor */
  CDSSCore(const CDSSCore& cdss);

  CDSSCore();

  ~CDSSCore();

  // needed because of pointer
  CDSSCore& operator=(const CDSSCore& cdss);

  static CDSSCore make(Detection3D& cd, double cdfilter, const std::string& tunit);

  /**
   * Returns the conflict detection filter time.
   *
   * @return the conflict detection filter time seconds
   */
  double getFilterTime() const;

  /**
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  void setFilterTime(double cdfilter);

  /**
   * Was there a detected conflict?
   */
  bool conflict() const;

  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo velocity of ownship
   * @param si the position of the intruder
   * @param vi velocity of intruder
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  bool violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const;

  bool violation(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip, const EuclideanProjection& proj) const;

  bool violation(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip) const;

  /**
   * Determines if two aircraft are in conflict in a given lookahead time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   *
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param D  The horizontal size of the protection zone
   * @param H  The vertical size of the protection zone
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   *
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double T) const;

  /**
   * Detects a conflict that lasts more than filter time within time horizon 
   * and computes the time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * 
   * @return true, if there is a conflict that last more than the filter time and is in the interval [0,infinity].
   */
  bool detectionEver(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi);

  /**
   * Detects a conflict that lasts more than filter time in a given lookahead time 
   * interval and computes the time interval of conflict (internal units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   * 
   * @return true, if there is a conflict that last more than the filter time in the interval [B,T].
   * NOTE: The timeIn and timeOut values are NOT truncated by the B and T values.
   * NOTE: the timeOut value is truncated by TimeHorizon
   */
  bool detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T);

  /** Time horizon. TimeHorizon < 0  means an infinite time horizon.  This parameter affects timeOut()!
   *  Note>  This is NOT the lookahead time!  The lookahead time is provided as a method parameter.
   *  Instead, this is intended to be a hard limit on the duration that state information is valid, for
   *  example if there is a TCP in the future.
   *  Effectively, a non-negative TimeHorizon will cut short detection at that point, even if the
   *  lookahead time is longer.
   */
  bool detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double timeHorizon);

  /**
   * Detects a conflict that lasts more than filter time in a given lookahead time
   * interval and computes the time interval of conflict (specified units).
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in specified units [ut] (B >= 0)
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [B,T].
   */
  bool detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, const std::string& ut);

  /**
   * Detects a conflict that lasts more than filter time within a given lookahed time 
   * and computes the time interval of conflict (internal units). If timeIn() == 0, after this function is called then aircraft 
   * is in loss of separation.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  the lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */
  bool detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      double T);

  /**
   * Detects a conflict that lasts more than filter time within a given lookahead time
   * and computes the time interval of conflict (specified units). If timeIn() == 0, after this function is called then aircraft 
   * is in loss of separation.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  the lookahead time in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,T].
   */
  bool detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      double T, const std::string& ut);

  /**
   * Duration of conflict in internal units. 
   * 
   * @return the duration of conflict. Zero means that there is not conflict.
   */
  double conflictDuration() const;

  /**
   * Duration of conflict in specified units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the duration of conflict in explicit units [ut]. Zero  means that 
   * there is no conflict. 
   */
  double conflictDuration(const std::string& ut) const;

  /**
   * Time to loss of separation in internal units.
   * 
   * @return the time to loss of separation. Note that this is a relative time.
   */
  double getTimeIn() const;

  /**
   * Time to loss of separation in explicit units.
   *
   * @param ut the explicit units of time
   *
   * @return the time to loss of separation in explicit units [ut].
   * If time is negative then there is no conflict.
   */
  double getTimeIn(const std::string& ut) const;

  /**
   * Time to exit from loss of separation in internal units.
   * 
   * @return the time to exit out of loss of separation. 
   *  Note that this is a relative time.
   */
  double getTimeOut() const;

  /**
   * Time to recovery of loss of separation in explicit units.
   * 
   * @param ut the explicit units of time
   * 
   * @return the time to recovery of loss of separation in explicit units [ut].
   */
  double getTimeOut(const std::string& ut) const;

  /**
   * Time of closest approach in internal units.
   * 
   * @return the cylindrical time of closest approach [s].
   */
  double timeOfClosestApproach() const;

  /**
   * Time of (cylindrical) closest approach in explicit units.
   *
   * @param ut the explicit units of time
   *
   * @return the time of (cylindrical) closest approach in explicit units [ut]
   */
  double timeOfClosestApproach(const std::string& ut) const;

  /**
   * Cylindrical distance at time of closest approach.
   *
   * @return the cylindrical distance at time of closest approach. This distance normalizes
   * horizontal and vertical distances. Therefore, it is unitless. It has the property that
   * the value is less than 1 if and only if the aircraft are in loss of separation. The value is 1
   * if the ownship is at the boundary of the intruder's protected zone.
   */
  double distanceAtCriticalTime() const;

  /**
   * Relative position at time of closest approach (internal units).
   *
   * @return the relative position of the ownship with respect to the intruder
   * at time of closest approach.
   *
   */
  Vect3 relativePositionAtTCA() const;

  std::string toString() const;

  /** The makes an internal copy of c.  You are responsible for deleting the original c after this call. */
  void setCoreDetectionPtr(const Detection3D* c);

  /** This makes an internal copy of c.  You are responsible for cleaning up the original c after this call -- local or temporary variables are fine to use. */
  void setCoreDetectionRef(const Detection3D& c);

  /** This returns a pointer to this object's core Detection3D object.  Do not delete it! */
  Detection3D* getCoreDetectionPtr() const;

  /** This returns a reference to this object's core Detection3D object. */
  Detection3D& getCoreDetectionRef() const;
};

}

#endif /* CDSSCORE_H_ */
