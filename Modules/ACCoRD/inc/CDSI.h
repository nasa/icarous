/*
 * CDSI - Conflict detection between an ownship state vector and a
 * traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 *
 *Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef CDSI_H_
#define CDSI_H_

#include "Vect3.h"
#include "ErrorLog.h"
#include "CDSICore.h"
#include "Plan.h"
#include "Detection3D.h"
#include "Detection3DAcceptor.h"

namespace larcfm {

  /** 
   * This class implements the algorithm for conflict detection
   * between an ownship (modeled with a state vector) and a traffic
   * aircraft trajectory (modeled with intent information).  <p>
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
  class CDSI : public ErrorReporter, Detection3DAcceptor {

  private:
    CDSICore core;
    mutable ErrorLog error;

    /** 
     * Create a new conflict detection (state information for the ownship
     * and intent information for the traffic) object.
     *
     * @param distance the minimum horizontal separation distance [nmi]
     * @param height the minimum vertical separation height [ft].
     */
    CDSI(double distance, double height);

  /** 
   * Create a new conflict detection (state information for the ownship
   * and intent information for the traffic) object.
   *
   * @param distance the minimum horizontal separation distance [nmi]
   * @param height the minimum vertical separation height [ft].
   */
    CDSI(Detection3D* cd);

    /** Returns the minimum horizontal separation distance in [m] */
    double getDistance() const;

    /** Returns the minimum vertical separation distance in [m] */
    double getHeight() const;

    /** Sets the minimum horizontal separation distance in [m] */
    void setDistance(double distance);

    /** Sets the minimum vertical separation distance in [m] */
    void setHeight(double height);


  public:

    CDSI();

    /**
     * Create a new CDSI conflict detection object using specified units.
     *
     * @param distance the minimum horizontal separation distance [dStr]
     * @param height the minimum vertical separation height [hStr].
     */
    static CDSI make(double distance, const std::string& dStr, double height, const std::string& hStr);

    static CDSI make(Detection3D* cd);

     /**
      * Create a new CDSI conflict detection object using internal units.
      *
      * @param distance the minimum horizontal separation distance [m]
      * @param height the minimum vertical separation height [m].
      */
     static CDSI mk(double distance, double height);


//    /** Returns the minimum horizontal separation distance in [nmi] */
//    double getDistance(const std::string& dStr) const;
//
//    /** Returns the minimum vertical separation distance in [ft] */
//    double getHeight(const std::string& dStr) const;


    /**
     * Returns the conflict detection filter time.
     * 
     * @return the conflict detection filter time seconds
     */
    double getFilterTime() const;


//    /** Sets the minimum horizontal separation distance in specified units */
//    void setDistance(double distance, const std::string& units);
//
//    /** Sets the minimum vertical separation distance in specified units*/
//    void setHeight(double height,  const std::string& units);


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
     * Returns the start time of the conflict in [s].  This value is in absolute time.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    double getTimeIn(int i) const;

    /** 
     * Returns the end time of the conflict in [s].  This value is in absolute time.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    double getTimeOut(int i) const;

    /** 
     * Returns the segment number of the entry into a conflict from the traffic aircraft's flight plan.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    int getSegmentIn(int i) const;

    /** 
     * Returns the segment number of the exit from a conflict from the traffic aircraft's flight plan.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    int getSegmentOut(int i) const;

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
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a cartesian reference
   * frame. <p>
   *
   * @param sx the x position of the state aircraft in [nmi]
   * @param sy the y position of the state aircraft in [nmi]
   * @param sz the z position of the state aircraft in [feet]
   * @param vx the x component of velocity of the state aircraft in [knot]
   * @param vy the y component of velocity of the state aircraft in [knot]
   * @param vz the z component of velocity of the state aircraft in [ft/min]
   * @param t0 the time, in [s], of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
   * @param horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
    bool detectionXYZ(double sx, double sy, double sz, 
		      double vx, double vy, double vz, double t0, double horizon, 
		      const Plan& intent, double startT, double endT);

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a latitude/longitude reference
   * frame. <p>
   *
   * @param lat the latitude of the state aircraft in [deg]
   * @param lon the longitude of the state aircraft in [deg]
   * @param alt the altitude of the state aircraft in [feet]
   * @param trk the track angle of the velocity of the state aircraft in [deg from true north]
   * @param gs the ground speed of velocity of the state aircraft in [knot]
   * @param vs the vertical speed of velocity of the state aircraft in [ft/min]
   * @param t0 the time, in [s], of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
   * @param horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
    bool detectionLL(double lat, double lon, double alt, 
		     double trk, double gs, double vs, double t0, double horizon,
		     const Plan& intent, double startT, double endT);

//  /**
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a cartesian reference
//   * frame. <p>
//   *
//   * @param sx the x position of the state aircraft in [nmi]
//   * @param sy the y position of the state aircraft in [nmi]
//   * @param sz the z position of the state aircraft in [feet]
//   * @param vx the x component of velocity of the state aircraft in [knot]
//   * @param vy the y component of velocity of the state aircraft in [knot]
//   * @param vz the z component of velocity of the state aircraft in [ft/min]
//   * @param t0 the time, in [s], of the state aircraft when located at (sx,sy,sz).  This can be 0.0 to represent "now"
//   * @param intent the flight plan of the intent aircraft
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//     static bool cdsi_xyz(double sx, double sy, double sz,
//            double vx, double vy, double vz, double t0,
//            const Plan& intent, double distance, double height, double startT, double endT);
//
//  /**
//   * Returns if there is a conflict between two aircraft: the state
//   * aircraft and the intent aircraft.  The state aircraft is
//   * assumed to move linearly from it position.  The intent aircraft
//   * is assumed to move according to the given flight plan.  Both
//   * aircraft are assumed to be represented in a latitude/longitude reference
//   * frame. <p>
//   *
//   * @param lat the latitude of the state aircraft in [deg]
//   * @param lon the longitude of the state aircraft in [deg]
//   * @param alt the altitude of the state aircraft in [feet]
//   * @param trk the track angle of the velocity of the state aircraft in [deg from true north]
//   * @param gs the ground speed of velocity of the state aircraft in [knot]
//   * @param vs the vertical speed of velocity of the state aircraft in [ft/min]
//   * @param t0 the time, in [s], of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
//   * @param intent the flight plan of the intent aircraft
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts relative to t0. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts relative to t0
//   * @return true if there is a conflict
//   */
//    static bool cdsi_ll(double lat, double lon, double alt,
//             double trk, double gs, double vs, double t0,
//             const Plan& intent, double distance, double height, double startT, double endT);
//
//
    /** Experimental.  You are responsible for deleting c after this call. */
    void setCoreDetectionPtr(const Detection3D* c);
    void setCoreDetectionRef(const Detection3D& c);

    Detection3D* getCoreDetectionPtr() const;
    Detection3D& getCoreDetectionRef() const;

    // ErrorReporter Interface Methods

    bool hasError() const {
      return error.hasError() || core.hasError();
    }
    bool hasMessage() const {
      return error.hasMessage() || core.hasMessage();
    }
    std::string getMessage() {
      return error.getMessage() + core.getMessage();
    }
    std::string getMessageNoClear() const {
      return error.getMessageNoClear() + core.getMessageNoClear();
    }
  };


}

#endif /* CDSI_H_ */
