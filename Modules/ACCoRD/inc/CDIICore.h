/*
 * CDIICore - the core algorithm for conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#ifndef CDIICORE_H_
#define CDIICORE_H_
 
#include "Vect2.h"
#include "Vect3.h"
#include "LatLonAlt.h"
#include "Velocity.h"
#include "CDSICore.h"
#include "Plan.h"
#include "Detection3D.h"
#include "format.h"
#include <string>
#include <vector>


namespace larcfm {

  /**
   * This class implements the core algorithms for conflict detection
   * between an ownship and a traffic aircraft trajectory, where both
   * trajectories include intent information.  This class assumes all
   * inputs are in internal units.  As such, this class is most
   * appropriate to be part of a larger system, not as a stand-alone
   * conflict detection algorithm.  For that functionality, see the
   * class CDII.<p>
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
   */
  class CDIICore : public ErrorReporter {  

  private:
//    static const int SIZE = 20;

//    int tin_size;
    std::vector<double> tin;
    std::vector<double> tout;
    std::vector<double> tcpa;
    std::vector<double> dist_tca;
    std::vector<int> segin;
    std::vector<int> segout;
    CDSICore cdsicore;
    mutable ErrorLog error;

  public:
//    /**
//     * Create a new conflict detection (intent information for both the ownship
//     * and traffic) object.
//     *
//     * @param distance the minimum horizontal separation distance
//     * @param height the minimum vertical separation height
//     */
//    CDIICore(double distance, double height);

	/**
	 * Create a new conflict detection (intent information for both the ownship
	 * and traffic) object.
	 *
	 * @param distance
	 *            the minimum horizontal separation distance
	 * @param height
	 *            the minimum vertical separation height
	 */
    CDIICore(Detection3D* cd);


    /**
     * Copy constructor
     */
    CDIICore(const CDIICore& cdiicore);

    CDIICore();

    /**
     * Destructor
     */
    ~CDIICore();

    CDIICore& operator= (const CDIICore& cdiicore);

//    /** Returns the minimum horizontal separation distance */
//    double getDistance() const;
//
//    /** Returns the minimum vertical separation distance */
//    double getHeight() const;
//
//    /** Sets the minimum horizontal separation distance */
//    void setDistance(double distance);
//
//    /** Sets the minimum vertical separation distance */
//    void setHeight(double height);
  
    /** Returns the number of conflicts */
    int size() const;

    /** Returns if there were any conflicts.  */
    bool conflict() const;


	/**
	 * Returns the conflict detection filter time.
	 * 
	 * @return the conflict detection filter time
	 */
    double getFilterTime() ;

    /**
     * Sets the conflict detection filter time.
     *
     * @param cdfilter  the conflict detection filter time.
     */
    void setFilterTime(double cdfilter);


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
     * Returns the distance index at the time of closest approach. 
     * @param i the i-th conflict, must be between 0..size()-1
     */
    double getDistanceClosest(int i) const;

    /**
     * Is there a conflict at any time in the interval from start to
     * end (inclusive).  This method assumes that the detection()
     * method has been called first.
     *
     * @param start the time to begin looking for conflicts
     * @param end the time to end looking for conflicts
     * @return true if there is a conflict
     */
    bool conflictBetween(double start, double end) const;

	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts (no time
	 *            information will be returned prior to the start of the ownship
	 *            segment containing B)
	 * @param T
	 *            the absolute time to end looking for conflicts
	 * @return true if there is a conflict
	 */
    bool detection(const Plan& ownship, const Plan& traffic, double B, double T);

	/**
	 * This version calculates the "true" time in and time out for all conflicts
	 * that overlap with [B,T]. It is less efficient than the normal detection()
	 * algorithm and should only be called if accurate time in information is
	 * necessary when B might be within a loss of separation region.
	 */
    bool detectionExtended(const Plan& ownship, const Plan& traffic, double B, double T);

    bool hasError() const {
      return error.hasError() || cdsicore.hasError();
    }
    bool hasMessage() const {
      return error.hasMessage() || cdsicore.hasError();
    }
    std::string getMessage() {
      return error.getMessage() + cdsicore.getMessage();
    }
    std::string getMessageNoClear() const {
      return error.getMessageNoClear() + cdsicore.getMessageNoClear();
    }



   /**
     * Returns if there is a conflict between two aircraft: the ownship and
     * the traffic aircraft.  <p>
     *
     * @param ownship the trajectory intent of the ownship
     * @param traffic the trajectory intent of the traffic
     * @param D the minimum horizontal separation distance
     * @param H the minimum vertical separation distance
     * @param B the absolute time to start looking for conflicts
     * @param T the absolute time to end looking for conflicts
     * @return true if there is a conflict
     */
    static bool cdiicore(const Plan& ownship, const Plan& traffic, Detection3D* cd, double B, double T);

	/**
	 * Return true if there is a violation between two aircraft at time tm. If
	 * tm is outside either of the plans' times, this will return false.
	 * 
	 * @param ownship
	 * @param traffic
	 * @param tm
	 * @return true if there is a violation
	 */
    bool violation(const Plan& ownship, const Plan& traffic, double tm) const;


    /** Experimental.  You are responsible for deleting c after this call. */
    void setCoreDetectionPtr(const Detection3D* c);
    void setCoreDetectionRef(const Detection3D& c);
    /** Experimental. Reference to core detection object. */
    Detection3D* getCoreDetectionPtr() const;
    Detection3D& getCoreDetectionRef() const;

    std::string toString() const;

  private:
    static CDIICore CDIICore_def;
	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 * 
	 * This will not return any timing information prior to the owhship segment
	 * containing B.
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts.
	 * @param T
	 *            the absolute time to end looking for conflicts.
	 * @return true if there is a conflict
	 */
    bool detectionXYZ(const Plan& ownship, const Plan& traffic, double B, double T);
	/**
	 * Returns if there is a conflict between two aircraft: the ownship and the
	 * traffic aircraft.
	 * <p>
	 * 
	 * This will not return any timing information prior to the ownship segment
	 * containing B.
	 *
	 * @param ownship
	 *            the trajectory intent of the ownship
	 * @param traffic
	 *            the trajectory intent of the traffic
	 * @param B
	 *            the absolute time to start looking for conflicts.
	 * @param T
	 *            the absolute time to end looking for conflicts.
	 * @return true if there is a conflict
	 */
    bool detectionLL(const Plan& ownship, const Plan& traffic, double B, double T);

    void captureOutput(const CDSICore& cd3d, int seg);
    void merge();
  };
}

#endif /* CDIICORE_H_ */
