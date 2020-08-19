/*
 * Projection.h
 *
 * Holding area for universal projection information.  All projection objects should be retrieved using these functions.
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PROJECTION_H_
#define PROJECTION_H_

#include "EuclideanProjection.h"
#include "LatLonAlt.h"
#include <string>

 /*
  * WARNING! The string "EuclideanProjection" is the name of a MACRO
  * representing the actual type of the chosen projection.
  *
  * Anything starting with this string will be replaced with SOMETHING
  * ELSE at compile time.  You may not notice a difference.  At first.
  *
  */

 namespace larcfm {

/**
 * A static holding class for universal projection (Spherical Earth to Euclidean plane) information.  
 * All projection objects should be retrieved from this class.<p>
 * 
 * Remember that the given projection point becomes the origin of the Euclidean space, so to preserve absolute altitude (above 
 * sea level) information in the Euclidean space, it may be necessary to make the projection point's altitude zero.
 */
class Projection {
   private:
	   static EuclideanProjection projection;
	   static ProjectionType ptype;
   public:
	  /**
	   * Returns a new projection for the current type with the given reference point.
	   * 
	   * Note that in the projected Euclidean frame, if two points are made using the 
	   * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	   * altitudes may have changed (the reference point is subtracted).  If you need 
	   * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	   * the reference point should have a zero altitude.
	   * 
	   * @param lat latitude of reference point
	   * @param lon longitude of reference point
	   * @param alt altitude of reference point
	   * @return new projection
	   */
	   static EuclideanProjection createProjection(double lat, double lon, double alt);
	  /**
	   * Returns a new projection for the current type with the given reference point.
	   * 
	   * Note that in the projected Euclidean frame, if two points are made using the 
	   * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	   * altitudes may have changed (the reference point is subtracted).  If you need 
	   * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	   * the reference point should have a zero altitude.
	   * 
	   * For example, if you call p2 = Projection.createProjection(p1).project(p1), p2 will 
	   * have an altitude of zero.  If you instead call p2 = Projection.createProjection(p1.zeroAlt(0)).project(p1),
	   * p2 will have the same altitude as p1.
	   * 
	   * @param lla reference point
	   * @return new projection
	   */
	   static EuclideanProjection createProjection(const LatLonAlt& lla);

	  /**
	   * Returns a new projection for the current type with the given reference point.
	   * This will return an altitude-preserving projection against the given Position if it is lat/lon.
	   * If it is Euclidean, the projection will be against the LatLonAlt.ZERO point.
	   * 
	   * Note that in the projected Euclidean frame, if two points are made using the 
	   * same projection, their _relative_ altitudes will be consistent, but their _absolute_ 
	   * altitudes may have changed (the reference point is subtracted).  If you need 
	   * to have the same absolute altitude values in the geodetic and Euclidean frames, 
	   * the reference point should have a zero altitude.
	   * 
	   * For example, if you call p2 = Projection.createProjection(p1).project(p1), p2 will 
	   * have an altitude of zero.  If you instead call p2 = Projection.createProjection(p1.zeroAlt(0)).project(p1),
	   * p2 will have the same altitude as p1.
	   * 
	   * @param pos reference point
	   * @return new projection
	   */
	  static EuclideanProjection createProjection(const Position& pos);

	  /**
	   * Geodetic projections into the Euclidean frame, for various reasons, tend to lose accuracy over long distances
	   * or when close to the poles.  This can be countered by examining trajectories as shorter segments at a time.  
	   * This is already done in Detector and Stratway, but not in any other tools.  For CDII, it is best
	   * to break up the ownship's plan in this way.  For CDSI and IntentBands, it is better to break up the traffic
	   * in this way.
	   * 
	   * This return an estimate on the suggested maximum segment size, depending on the current projection.
	   * 
	   * @param lat - latitude  [rad]
	   * @param accuracy - desired accuracy (allowable error) [m]
	   * @return the maximum length of a trajectory segment at the given latitude that preserves the desired accuracy.
	   */
	   static double projectionConflictRange(double lat, double accuracy);

	  /**
	   * This is a range about which the projection will completely break down and start producing nonsensical answers.
	   * Attempting to use the projection at ranges greater than this is an error state (at ranges less than this but greater
	   * than the conflictRange, it may still be unacceptably inaccurate, however).
	   * 
	   * @return maximum range for the projection (in meters).	
	   */
	   static double projectionMaxRange();


	   /**
	    * Set the projection to a new type.  This is a global change.
	   * 
	   * @param t type of projection
	   */
	   static void setProjectionType(ProjectionType t);

	  /**
	   * Given a string representation of a ProjectionType, return the ProjectionType
	   * 
	   * @param s name of projection type
	   * @return projection type
	   */
	   static ProjectionType getProjectionTypeFromString(std::string s);

	  /**
	   * Return the current ProjectionType
	   * 
	   * @return type of projection
	   */
	   static ProjectionType getProjectionType();
   };


   /**
    * \deprecated {Use Projection:: version.}
    * Returns a new projection for the current type with the given reference point.
    */
   EuclideanProjection getProjection(double lat, double lon, double alt);
   /**
    * \deprecated {Use Projection:: version.}
    * Returns a new projection for the current type with the given reference point.
    */
   EuclideanProjection getProjection(const LatLonAlt& lla);

   /**
    * \deprecated {Use Projection:: version.}
    * Geodetic projections into the Euclidean frame, for various reasons, tend to lose accuracy over long distances
    * or when close to the poles.  This can be countered by examining shorter segments at a time.  
    * This is already done in Detector and Stratway, but not in any other tools.  For CDII, it is best
    * to break up the ownship's plan in this way.  For CDSI and IntentBands, it is better to break up the traffic
    * in this way.
    * 
    * This return an estimate on the suggested maximum segment size, depending on the current projection.
    * 
    * @param lat - latitude  [radians]
    * @param accuracy - desired accuracy (allowable error) [m]
    * @return the maximum length of a trajectory segment at the given latitude that preserves the desired accuracy.
    */
   double projectionConflictRange(double lat, double accuracy);

   /**
    * \deprecated {Use Projection:: version.}
    * This is a range about which the projection will completely break down and start producing nonsensical answers.
    * Attempting to use the projection at ranges greater than this is an error state (at ranges less than this but greater
    * than the conflictRange, it may still be unacceptably inaccurate, however).
    * 
    * @return maximum range for the projection
    */
   double projectionMaxRange();
     

   /**
    * \deprecated {Use Projection:: version.}
    * Set the projection to a new type.  This is a global change.
    */
   void setProjectionType(ProjectionType t);

   /**
    * \deprecated {Use Projection:: version.}
    * Given a string representation of a ProjectionType, return the ProjectionType
    */
   ProjectionType getProjectionTypeFromString(std::string s);

   /**
    * \deprecated {Use Projection:: version.}
    * Return the current ProjectionType
    */
   ProjectionType getProjectionType();

 }
 
#endif /* PROJECTION_H_ */
