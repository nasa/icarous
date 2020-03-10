/*
 * CDSI Core - The core algorithms for conflict detection between an
 * ownship state vector and a traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef CDSICORE_H_
#define CDSICORE_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "ErrorLog.h"
#include "Plan.h"
#include "Detection3D.h"
#include <string>
#include <vector>

#include "CDSSCore.h"

namespace larcfm {

/**
 * This class implements the core algorithms for conflict detection
 * between an ownship (modeled with a state vector) and a traffic
 * aircraft trajectory (modeled with a intent).  This class assumes
 * all inputs are in internal units.  As such, this class is most
 * appropriate to be part of a larger system, not as a stand-alone
 * conflict detection algorithm.  For that functionality, see the class
 * CDSI.<p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *
 * In addition, for each of these types of use, the aircraft can be
 * specified in two different coordinate systems: a Euclidean space
 * and a latitude and longitude reference frame. <p>
 */
class CDSICore : public ErrorReporter {

private:
  static CDSICore CDSICore_def;
  //    static const int SIZE = 10;

  //    int tin_size;
  std::vector<double> tin;
  std::vector<double> tout;
  std::vector<double> tca;
  std::vector<double> dist_tca;
  std::vector<int> segin;
  std::vector<int> segout;
  CDSSCore cdsscore;
  mutable ErrorLog error;

  void captureOutput(double t_base, int seg);
  void merge();

  //    static const bool allowVariableDistanceBuffer = true;

public:
  bool checkSmallTimes;

  //    /** Can the distance parameter change from leg to leg? */
  //    static bool allowVariableDistance() { return allowVariableDistanceBuffer; }

  //    /**
  //     * Create a new conflict detection (state information for the ownship
  //     * and intent information for the traffic) object with the given
  //     * parameters for the minimum horizontal distance and the minimum
  //     * vertical information.
  //     */
  //    CDSICore(double distance, double height);

  explicit CDSICore(Detection3D* cd);

  /**
   * Create a new conflict detection (state information for the ownship
   * and intent information for the traffic) object with the given
   * parameters for the minimum horizontal distance and the minimum
   * vertical information.
   */
  CDSICore();

  /**
   * Copy constructor
   */
  CDSICore(const CDSICore& cdsicore);

  /**
   * Destructor
   */
  ~CDSICore();

  CDSICore& operator= (const CDSICore& cdsicore);

  //    /** Returns the minimum horizontal separation distance */
  //    double getDistance() const;
  //
  //    /** Returns the minimum vertical separation distance */
  //    double getHeight() const;

  /**
   * Returns the conflict detection filter time.
   *
   * @return the conflict detection filter time seconds
   */
  double getFilterTime() const;

  //  /** Sets the minimum horizontal separation distance. The distance must be greater than 0. */
  //    void setDistance(double distance);
  //
  //  /** Sets the minimum vertical separation distance.  The height must be greater than 0.  */
  //    void setHeight(double height);

  /**
   * Sets the conflict detection filter time.
   *
   * @param cdfilter the conflict detection filter time in seconds.
   */
  void setFilterTime(double cdfilter);

  /** Returns the number of conflicts */
  int size() const;

  /** Returns if there were any conflicts */
  bool conflict() const;

  /**
   * Returns the start time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double getTimeIn(int i) const;

  /**
   * Returns the end time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double getTimeOut(int i) const;

  /**
   * Returns the segment number in the traffic aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  int getSegmentIn(int i) const;

  /**
   * Returns the segment number in the traffic aircraft's flight plan.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  int getSegmentOut(int i) const;

  double getCriticalTime(int i) const;

  double getDistanceAtCriticalTime(int i) const;

  //  private:
  // double seg_lh_top(const Plan& flp, double t0, int j, double B, double T) const;
  // double seg_lh_bottom(const Plan& flp, double t0, int j, double B, double T) const;

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan. <p>
   *
   * @param so the Position of the state aircraft
   * @param vo the velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
   * @param state_horizon the largest time where the state aircraft's position can be predicted.
   * @param intent the flight plan of the intent aircraft
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  static bool cdsicore(const Position& so, const Velocity& vo, double t0, double state_horizon,
      const Plan& intent, double D, double H, double B, double T);

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan. <p>
   *
   * @param so the position of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool detection(const Position& so, const Velocity& vo, double t0, double state_horizon,
      const Plan& intent, double B, double T);

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a Euclidean reference
   * frame at the time t0. <p>
   *
   * @param so the position of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (so.x,so.y,so.z).  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool detectionXYZ(const Vect3& so, const Velocity& vo, double t0, double state_horizon,
      const Plan& intent, double B, double T);

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a latitude/longitude reference
   * frame. <p>
   *
   * @param so the latitude/longitude/altitude of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool detectionLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon,
      const Plan& intent, double B, double T);

  bool hasError() const {
    return error.hasError();
  }
  bool hasMessage() const {
    return error.hasMessage();
  }
  std::string getMessage() {
    return error.getMessage();
  }
  std::string getMessageNoClear() const {
    return error.getMessageNoClear();
  }

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a cartesian reference
   * frame. <p>
   *
   * @param so the position of the state aircraft
   * @param vo the velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
   * @param state_horizon the largest time where the state aircraft's position can be predicted.
   * @param intent the flight plan of the intent aircraft
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  static bool cdsicore_xyz(const Vect3& so, const Velocity& vo, Detection3D* cd, double t0, double state_horizon,
      const Plan& intent, double B, double T);

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a latitude/longitude reference
   * frame. <p>
   *
   * @param state the latitude/longitude/altitude of the state aircraft
   * @param vo the velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
   * @param state_horizon the largest time where the state aircraft's position can be predicted.
   * @param intent the flight plan of the intent aircraft
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  static bool cdsicore_ll(const LatLonAlt& state, Velocity vo, Detection3D* cd, double t0, double state_horizon,
      const Plan& intent, double B, double T);


  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan. <p>
   *
   * @param so the Position of the state aircraft
   * @param vo the velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
   * @param state_horizon the largest time where the state aircraft's position can be predicted.
   * @param intent the flight plan of the intent aircraft
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  static bool cdsicore(const Position& so, const Velocity& vo, Detection3D* cd, double t0, double state_horizon, const Plan& intent, double B, double T);

  /**
   * Return true if the given position and velocity are in violation with the intent aircraft at time tm.
   * If tm is outisde the intent information, return false;
   * @param so
   * @param vo
   * @param intent
   * @param tm
   * @return true if violation
   */
  bool violation(const Position& so, const Velocity& vo, const Plan& intent, double tm) const;


  /**
   * EXPERIMENTAL
   */
  bool conflictXYZ(const Vect3& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent, double B, double T) const ;


  /**
   * EXPERIMENTAL
   */
  bool conflictLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent, double B, double T) const ;

  std::string toString() const;


  /** Experimental.  You are responsible for deleting c after this call. */
  void setCoreDetectionPtr(const Detection3D* c);
  void setCoreDetectionRef(const Detection3D& c);

  /** Experimental. Reference to core detection object. */
  Detection3D* getCoreDetectionPtr() const;
  Detection3D& getCoreDetectionRef() const;

};




//  /**
//   * \deprecated {Use CDSICore:: version.}
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a cartesian reference
//   * frame. <p>
//   *
//   * @param so the position of the state aircraft
//   * @param vo the velocity of the state aircraft
//   * @param t0 the time of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
//   * @param state_horizon the largest time where the state aircraft's position can be predicted.
//   * @param intent the flight plan of the intent aircraft
//   * @param D the minimum horizontal separation distance
//   * @param H the minimum vertical separation distance
//   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
//   * @param T the time to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//  bool cdsicore_xyz(const Vect3& so, const Velocity& vo, double t0, double state_horizon,
//		    const Plan& intent, double D, double H, double B, double T);
//
// /**
//   * \deprecated {Use CDSICore:: version.}
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a latitude/longitude reference
//   * frame. <p>
//   *
//   * @param state the latitude/longitude/altitude of the state aircraft
//   * @param vo the velocity of the state aircraft
//   * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
//   * @param state_horizon the largest time where the state aircraft's position can be predicted.
//   * @param intent the flight plan of the intent aircraft
//   * @param D the minimum horizontal separation distance
//   * @param H the minimum vertical separation distance
//   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
//   * @param T the time to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//  bool cdsicore_ll(const LatLonAlt& state, Velocity vo, double t0, double state_horizon,
//		   const Plan& intent, double D, double H, double B, double T);


}


#endif /* CDSICORE_H_ */
