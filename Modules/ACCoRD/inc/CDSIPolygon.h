/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef CDSIPOLYGON_H
#define CDSIPOLYGON_H

#include "CDSSCore.h"
#include "EuclideanProjection.h"
#include "PolyPath.h"
#include "Projection.h"
#include "Position.h"
#include "SimplePoly.h"
#include "Vect3.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "Util.h"
#include "GreatCircle.h"
#include "Plan.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "DetectionPolygon.h"
#include "DetectionPolygonAcceptor.h"
#include "format.h"
#include <vector>
#include <string>

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
class CDSIPolygon : public ErrorReporter, DetectionPolygonAcceptor {

private:
  static CDSIPolygon def;

  std::vector<double> tin;
  std::vector<double> tout;
  std::vector<double> tca;
  std::vector<double> dist_tca;
  DetectionPolygon* cdss;
  ErrorLog error;

public:
  /**
   * Create a new conflict detection (state information for the ownship
   * and intent information for the traffic) object with the given
   * parameters for the minimum horizontal distance and the minimum
   * vertical information.
   */
  CDSIPolygon(DetectionPolygon* d);

  CDSIPolygon();


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
   * Returns an estimate of the time of closest approach.  This value is in absolute time
   * (not relative from a waypoint).  This point approximates the point where the two aircraft
   * are closest.  The definition of closest is not simple.  Specifically, space in the vertical
   * dimension counts more than space in the horizontal dimension: encroaching in the protected
   * zone 100 vertically is much more serious than encroaching 100 ft. horizontally.
   *
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double getTimeClosest(int i) const;

  /**
   * Returns the cylindrical distance at the time of closest approach.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double getDistanceClosest(int i) const;


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
  static bool cdsicore(const Position& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T);

  /**
   * Return true if the given position and velocity are in violation with the intent aircraft at time tm.
   * If tm is outisde the intent information, return false; 
   * @param so
   * @param vo
   * @param intent
   * @param tm
   * @return true if violation
   */
  bool violation(const Position& so, const Velocity& vo, const PolyPath& intent, double tm) const;

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
  bool detection(const Position& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T);


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
  bool detectionXYZ(Vect3 so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T);


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
  bool detectionLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T);


private:
  void captureOutput(double t_base, int conf, DetectionPolygon* cd);

  void merge();

public:
  std::string toString() const;

  // ErrorReporter Interface Methods

  bool hasError() const;

  bool hasMessage() const;

  std::string getMessage();

  std::string getMessageNoClear() const;

  void setCorePolygonDetectionPtr(const DetectionPolygon* d);
  void setCorePolygonDetectionRef(const DetectionPolygon& d);

  DetectionPolygon* getCorePolygonDetectionPtr() const;
  DetectionPolygon& getCorePolygonDetectionRef() const;

};

}

#endif
