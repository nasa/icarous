/*
 * CDSS.h
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

#ifndef CDSS_H_
#define CDSS_H_

#include "Velocity.h"
#include "CD3D.h"
#include "CDCylinder.h"
#include "string_util.h"

namespace larcfm {

/**
 * The Class CDSS.  This is a dedicated class that only uses the CD3D (CDCylinder) detection algorithm.
 * The more general form is CDSSCore, which may utilize arbitrary Detection3D detection algorithms.
 */
class CDSS {

public:

  /**
   * Create a new state-based conflict detection object using internal units.
   *
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   * @param cdfilter the cdfilter
   */
  CDSS(const double distance, const double height, const double cdfilter);

  CDSS();

  /**
   * Create a new state-based conflict detection object using specified units.
   * 
   * @param distance the minimum horizontal separation distance [ud]
   * @param ud the specified units of distance
   * @param height the minimum vertical separation height [uh]
   * @param uh the specified units of height
   * @param cdfilter the cdfilter
   */
  CDSS(const double distance, const std::string& ud,
      const double height, const std::string& uh,
      const double cdfilter);

  // needed because of pointer
  /** Copy constructor */
  CDSS(const CDSS& cdss);

  ~CDSS();

  // needed because of pointer
  CDSS& operator=(const CDSS& cdss);

  /**
   * Create a new state-based conflict detection object using specified units.
   *
   * @param distance the minimum horizontal separation distance [dStr]
   * @param height the minimum vertical separation height [hStr].
   */
  static CDSS make(double distance, const std::string& dStr, double height, const std::string& hStr);

  /**
   * Create a new state-based conflict detection object using internal units.
   *
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  static CDSS mk(double distance, double height);


  /**
   * Returns the minimum horizontal separation distance in internal units [m].
   *
   * @return the distance
   */
  double getDistance() const;

  /**
   * Returns the minimum horizontal separation distance in specified units.
   * 
   * @param ud the specified units of distance
   * 
   * @return the distance in specified units [ud]
   */
  double getDistance(const std::string& ud) const;

  /**
   * Returns the minimum vertical separation distance in internal units.
   *
   * @return the height in internal units [m]
   */
  double getHeight() const;

  /**
   * Returns the minimum vertical separation distance in specified units.
   * 
   * @param uh the specified units of height
   * 
   * @return the height in specified units [uh]
   */
  double getHeight(const std::string& uh) const;

  /**
   * Returns the time horizon in internal units.
   *  
   * @return the time horizon [s]
   */
  double getTimeHorizon() const;

  /**
   * Returns the time horizon in specified units.
   * 
   * @param ut the specified units of time 
   * 
   * @return the time horizon in specified units [ut]
   */
  double getTimeHorizon(const std::string& ut) const;

  /**
   * Returns the conflict detection filter time.
   *
   * @return the conflict detection filter time seconds
   */
  double getFilterTime() const;

  /**
   * Sets the minimum horizontal separation distance in internal units.
   *
   * @param distance the distance in internal units [m]
   */
  void setDistance(const double distance);

  /**
   * Sets the minimum horizontal separation distance in specified units.
   * 
   * @param distance the distance in specified units [ud]
   * @param ud the specified units of distance
   */
  void setDistance(const double distance, const std::string& ud);

  /**
   * Sets the minimum vertical separation distance in internal units.
   *
   * @param height the height in internal units [m]
   */
  void setHeight(const double height);

  /**
   * Sets the minimum vertical separation distance in specified units.
   * 
   * @param height the height in specified units [uh]
   * @param uh the specified units of time of height
   */
  void setHeight(const double height, const std::string& uh);

  /**
   * Sets the time horizon in internal units.
   * 
   * @param time the time horizon in internal units [s].
   */
  void setTimeHorizon(const double time);

  /**
   * Sets the time horizon in specified units.
   * 
   * @param time the time horizon in specified units [ut].
   * @param ut the specified units of time
   */
  void setTimeHorizon(const double time, const std::string& ut);

  /**
   * Sets the conflict detection filter time.
   * 
   * @param cdfilter the conflict detection filter time in seconds.
   */
  void setFilterTime(const double cdfilter);

  double getFilter() const;


  /**
   * Determines if two aircraft are in loss of separation.
   * This function DOES NOT compute time interval of conflict.
   * This function uses the CD3D definition of loss of separation -- for a more general solution, use lossOfSeparation(Vect3,Velocity,Velocity)
   * 
   * @param s the relative position of the aircraft
   * 
   * @return true, if the aircraft are in loss of separation.
   */
  bool lossOfSeparation(const Vect3& s) const;


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
  bool lossOfSeparation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const;



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
  bool lossOfSeparation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double d, double h) const;



  /**
   * Determines if two aircraft are in conflict ever within time horizon.
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * 
   * @return true, if the aircraft are in conflict in the interval [0,time_horizon].
   */
  bool conflictEver(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const;

  /**
   * Determines if two aircraft are in conflict in a given lookahead time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * This function uses the CD3D definition of "conflict" -- for a more general solution, use the one of the other conflict() or detection() methods.
   *
   * @param s the relative position of the aircraft
   * @param vo the velocity of the ownship
   * @param vi the velocity of the intruder
   * @param D  The horizontal size of the protection zone
   * @param H  The vertical size of the protection zone
   * @param T  upper bound of lookahead time interval in internal units [s] (T > 0)
   *
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  static bool conflict(const Vect3& s, const Velocity& vo, const Velocity& vi, double D, double H, double T);

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
  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double T) const;

  /**
   * Determines if two aircraft are in conflict in a given lookahed time interval (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder 
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in internal units [s] (B >= 0)
   * @param T  upper bound of lookahead time interval in internal units [s] (T > B)
   * 
   * @return true, if the aircraft are in conflict in the interval [B,T].
   */

  bool conflictBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double B, const double T) const;

  /**
   * Determines if two aircraft are in conflict in a given lookahead time internal (specified
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder 
   * @param vi the velocity of the intruder
   * @param B  lower bound of lookahead time interval in specified units [ut] (B >= 0)
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if the aircraft are in conflict in the interval [B,T].
   */
  bool conflictBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double B, const double T, const std::string& ut) const;

  /**
   * Determines if two aircraft are in conflict within a given lookahed time (internal
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  lookahead time in internal units [s] (T > 0)
   * 
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double T) const;

  /**
   * Determines if two aircraft are in conflict within a given lookahead time (specified
   * units).
   * This function DOES NOT compute time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * @param T  upper bound of lookahead time interval in specified units [ut] (T > 0)
   * @param ut specified units of time
   * 
   * @return true, if the aircraft are in conflict in the interval [0,T].
   */
  bool conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double T, const std::string& ut) const;

  /**
   * Detects a conflict that lasts more than filter time within time horizon 
   * and computes the time interval of conflict.
   * 
   * @param so the position of the ownship
   * @param vo the velocity of the ownship
   * @param si the position of the intruder
   * @param vi the velocity of the intruder
   * 
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [0,time_horizon].
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
   * @return true, if there is a conflict that last more than the filter time
   * in the interval [B,T].
   */
  bool detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double B, const double T);

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
  bool detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi,
      const double B, const double T, const std::string& ut);

  /**
   * Detects a conflict that lasts more than filter time within a given lookahed time 
   * and computes the time interval of conflict (internal units).
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
      const double T);

  /**
   * Detects a conflict that lasts more than filter time within a given lookahead time
   * and computes the time interval of conflict (specified units).
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
      const double T, const std::string& ut);

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
   * @return the duration of conflict in explicit units [ut]. Zero means that there is not conflict.
   */
  double conflictDuration(const std::string& ut) const;

  /** EXPERIMENTAL STATIC TIME INTO LOS (assumes infinite lookahead time), undefined if not conflict
   *
   * @param s the relative position of the aircraft
   * @param vo the ownship's velocity
   * @param vi the intruder's velocity
   * @param D the minimum horizontal distance
   * @param H the minimum vertical distance
   *
   * @return time to enter loss of separation
   */
  static double timeIntoLoS(const Vect3& s, const Vect3& vo, const Vect3& vi, double D, double H);

  /**
   * Was there a detected conflict?
   */
  bool conflict() const;

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
   * Time to recovery of loss of separation in internal units.
   * 
   * @return the time to recovery of loss of separation. Note that this is a relative time.
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
  double cylindricalDistanceAtTCA() const;

  /**
   * Relative position at time of closest approach (internal units).
   *
   * @return the relative position of the ownship with respect to the intruder
   * at time of closest approach.
   *
   */
  Vect3 relativePositionAtTCA() const;

  /**
   * Vertical distance at time of closest approach (internal units).
   *
   * @return the vertical distance at time of closest approach.
   *
   */
  double verticalDistanceAtTCA() const;

  /**
   * Vertical distance at time of closest approach (explicit units).
   *
   * @param ud the explicit units of distance
   *
   * @return the vertical distance at time of closest approach.
   *
   */
  double verticalDistanceAtTCA(const std::string& ud) const;

  /**
   * Horizontal distance at time of closest approach (internal units).
   *
   * @return the horizontal distance at time of closest approach.
   */
  double horizontalDistanceAtTCA() const;

  /**
   * Horizontal distance at time of closest approach (explicit units).
   *
   * @param ud the explicit units of distance
   *
   * @return the horizontal distance at time of closest approach.
   */
  double horizontalDistanceAtTCA(const std::string& ud) const;

  std::string toString();

private:
  /** The minimum horizontal distance */
  double D;

  /** The minimum vertical distance */
  double H;

  /** Time horizon. TimeHorizon < 0  means an infinite time horizon */
  double TimeHorizon;

  /** The conflict filter time */
  double filter;

  /** Time of (cylindrical) closest approach *. */
  double tca;

  double t_in;

  double t_out;

  /** Aircraft relative position at time of closest approach. */
  Vect3 stca;

  /** CD3D object */
  CDCylinder cd;

  void init(const double d, const double h, const double f);

};

}

#endif /* CDSS_H_ */
