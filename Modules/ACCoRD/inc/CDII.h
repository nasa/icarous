/*
 * CDII - conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef CDII_H_
#define CDII_H_

#include <string>
#include "Vect3.h"
#include "Vect2.h"
#include "ErrorLog.h"
#include "Plan.h"
#include "CDIICore.h"
#include "Detection3DAcceptor.h"

namespace larcfm {

/**
 * This class implements the algorithm for conflict detection between
 * an ownship and a traffic aircraft, where the trajectories of both
 * aircraft are modeled with intent information.  <p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *
 * When using the object method, all method getTimeIn(), etc, assume
 * that the detection() method has been called first.<p>
 *
 *  Usage:
 *     CDII.detection(ownship,traffic,B,T)
 *
 *     If there are long legs in the ownship Plan, then it is prudent to call
 *
 *        PlanUtil.interpolateVirtuals(ownship, Units.from("NM",0.1) ,0 , Tinfinity);
 *
 *     before the detection call.
 *
 */
  class CDII : public ErrorReporter, Detection3DAcceptor {
  
  private :
    CDIICore cdii_core;
    mutable ErrorLog error;

    /**
      * Create a new conflict detection (intent information for both the ownship
      * and traffic) object.
      *
      * @param distance the minimum horizontal separation distance [m]
      * @param height the minimum vertical separation height [m].
      */
     CDII(double distance, double height);

  /** Returns the minimum horizontal separation distance in [m] */
     double getDistance() const;

  /** Returns the minimum vertical separation distance in [m] */
     double getHeight() const;

  /** Sets the minimum horizontal separation distance in [m] */
     void setDistance(double distance);

  /** Sets the minimum vertical separation distance in [m] */
     void setHeight(double height);

 
  public:


     /**
       * Create a new conflict detection (intent information for both the ownship
       * and traffic) object.
       */
    CDII();

  /**
   * Factory method for cylindrical detection
   * @param distance
   * @param dUnits
   * @param height
   * @param hUnits
   * @return CDII object
   */
    static CDII make(double distance, const std::string& dUnits, double height, const std::string& hUnits);

  /**
   * Factory method for cylindrical detection
   * @param distance
   * @param dUnits
   * @param height
   * @param hUnits
   * @return CDII object
   */
    static CDII mk(double distance, double height);

    CDIICore const & getCore() const;

  /** Returns the minimum horizontal separation distance in the given units */
    double getDistance(const std::string& units) const;

  /** Returns the minimum vertical separation distance in the given units */
    double getHeight(const std::string& units) const;

  /** Sets the minimum horizontal separation distance in the specified units */
    void setDistance(double distance, const std::string& units);

  /** Sets the minimum vertical separation distance in the specified units */
    void setHeight(double height, const std::string& units);
  
  /**
   * Returns the conflict detection filter time.
   * 
   * @return the conflict detection filter time seconds
   */
    double getFilterTime();

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

    //
    // Per conflict information
    //

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
     * Returns the segment number of the entry into a conflict from the ownship aircraft's flight plan.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    int getSegmentIn(int i) const;

    /** 
     * Returns the segment number of the exit from a conflict from the ownship aircraft's flight plan.
     * If there was not a conflict, then this value is meaningless.
     * @param i the i-th conflict, must be between 0..size()-1
     */
    int getSegmentOut(int i) const;

    double getCriticalTime(int i) const;

    double getDistanceAtCriticalTime(int i) const;

    /**
     * Is there a conflict at any time in the interval from start to
     * end (inclusive). This method assumes that the detection()
     * method has been called first.
     *
     * @param start the time to begin looking for conflicts [s]
     * @param end the time to end looking for conflicts [s]
     * @return true if there is a conflict
     */
    bool conflictBetween(double start, double end) const;

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts (absolute time)
   * @return true if there is a conflict
   * 
   * Note: If the aircraft are in loss of separation at time startT, 
   * the time in reported for that conflict will be startT. 
   */
    bool detection(const Plan& ownship, const Plan& traffic, double startT, double endT,  bool interpolateVirtuals);
    bool detection(const Plan& ownship, const Plan& traffic, double startT, double endT);

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  This version can be less efficient than the normal detection() call,
   * but all reported conflict time in and time out values will not be constrained to be within the
   * lookahead times used.<p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
   * @param endT the time, in [s], to end looking for conflicts (absolute time)
   * @return true if there is a conflict
   * 
   * Note: All time in and time out data reported will be accurate within the length of the given plans. 
   */
    bool detectionExtended(const Plan& ownship, const Plan& traffic, double startT, double endT,  bool interpolateVirtuals);
    bool detectionExtended(const Plan& ownship, const Plan& traffic, double startT, double endT);

    /** Experimental.  You are responsible for deleting c after this call. */
    void setCoreDetectionPtr(const Detection3D* c);
    void setCoreDetectionRef(const Detection3D& c);
    Detection3D* getCoreDetectionPtr() const;
    Detection3D& getCoreDetectionRef() const;

    // ErrorReporter Interface Methods

    bool hasError() const {
      return error.hasError() || cdii_core.hasError();
    }
    bool hasMessage() const {
      return error.hasMessage() || cdii_core.hasMessage();
    }
    std::string getMessage() {
      return error.getMessage() + cdii_core.getMessage();
    }
    std::string getMessageNoClear() const {
      return error.getMessageNoClear() + cdii_core.getMessageNoClear();
    }

//  /**
//   * Returns if there is a conflict between two aircraft: the ownship and
//   * the traffic aircraft.  <p>
//   *
//   * @param ownship the trajectory intent of the ownship
//   * @param traffic the trajectory intent of the traffic
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts
//   * @return true if there is a conflict
//   */
//     static bool cdii(const Plan& ownship, const Plan& traffic, double distance, double height, double startT, double endT);


  };

//  /**
//   * \deprecated {Use CDII::cdii version.}
//   * Returns if there is a conflict between two aircraft: the ownship and
//   * the traffic aircraft.  <p>
//   *
//   * @param ownship the trajectory intent of the ownship
//   * @param traffic the trajectory intent of the traffic
//   * @param distance the minimum horizontal separation distance in [nmi]
//   * @param height the minimum vertical separation distance in [ft]
//   * @param startT the time, in [s], to start looking for conflicts. This can be 0.0.
//   * @param endT the time, in [s], to end looking for conflicts
//   * @returns true if there is a conflict
//   */
//    bool cdii(const Plan& ownship, const Plan& traffic, double distance, double height, double startT, double endT);


}

#endif /* CDII_H_ */
