/*
 * SimpleProjection.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef SIMPLEPROJECTION_H_
#define SIMPLEPROJECTION_H_

#include "LatLonAlt.h"
#include "Vect3.h"
#include "Velocity.h"
#include "Position.h"
#include "Util.h"
#include "Point.h"

namespace larcfm {

/**
 * This class creates a local Euclidean projection around a given point.  This projection may be used to
 * transform geodesic coordinates (LatLonAlt objects) into this Euclidean frame, using the project() method.  Also points
 * within this frame, may be found in geodesic coordinates with the inverse() method.   As long as the points are
 * close to the projection point, the errors will be very small.
 * 
 * This projection has special code for when positions are near the poles.
 *  
 * Note: projection objects should never be made directly, and instead should be retrieved via Projection.getProjection()
 * 
 */
  class SimpleProjection {
  private:
    double projLat;
    double projLon;
    double projAlt;
    bool projNorth;
 
  public:

    /** Default constructor. */
    SimpleProjection();

    /** Create a projection around the given reference point. */
    SimpleProjection(const LatLonAlt& lla);
    
    /** Create a projection around the given reference point. */
    SimpleProjection(double lat, double lon, double alt);
    
    /** Destructor */
    ~SimpleProjection() {}
    
    /** Return a new projection with the given reference point */
    SimpleProjection makeNew(const LatLonAlt& lla) const;
    
    /** Return a new projection with the given reference point */
    SimpleProjection makeNew(double lat, double lon, double alt) const;
    
	/** 
	 * Given an ownship latitude and desired accuracy, what is the longest distance to conflict this projection will support? [m] 
	 */
    double conflictRange(double latitude, double accuracy) const;
    
	/**
	 *  What is the maximum effective horizontal range of this projection? [m] 
	 */
    double maxRange() const;

	/** Get the projection point for this projection */
	LatLonAlt getProjectionPoint() const;

    /** Is the reference point near a pole? */    
    bool isPolar() const;
    
    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
    Vect2 project2(const LatLonAlt& lla) const;
    
    /** Return a projection of a lat/lon(/alt) point in Euclidean 3-space */
    Vect3 project(const LatLonAlt& lla) const;
    
    /** Return a projection of a Position in Euclidean 3-space (if already in Euclidian coordinate, this is the identity function) */
	Vect3 project(const Position& sip) const;

	Point projectPoint(const Position& sip) const;

    /** Return a LatLonAlt value corresponding to the given Euclidean position */
    LatLonAlt inverse(const Vect2& xy, double alt) const;
    
    /** Return a LatLonAlt value corresponding to the given Euclidean position */
    LatLonAlt inverse(const Vect3& xyz) const; 
    
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
    Velocity projectVelocity(const LatLonAlt& lla, const Velocity& v) const;
    
    /** Given a velocity from a point, return a projection of this velocity in Euclidean 3-space  (if already in Euclidian coordinate, this is the identity function) */
    Velocity projectVelocity(const Position& ss, const Velocity& v) const;
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected into the geodetic coordinate space */ 
    Velocity inverseVelocity(const Vect3& s, const Velocity& v, bool toLatLon) const;
    
    /** Given a velocity from a point, return a projection of this velocity and the point in Euclidean 3-space.  If the position is already in Euclidean coordinates, this acts as the idenitty function. */ 
    std::pair<Vect3,Velocity> project(const Position& p, const Velocity& v) const;
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity and the point.  If toLatLon is true, the point/velocity is projected into the geodetic coordinate space */ 
    std::pair<Position,Velocity> inverse(const Vect3& p, const Velocity& v, bool toLatLon) const;
    
    /** String representation */
    std::string toString() const { return "SimpleProjection "+to_string(projLat)+" "+to_string(projLon)+" "+to_string(projAlt);}
  
    /**
     * This method performs a particular projection from a spherical Earth
     * latitude/longitude coordinate system to Euclidean (XY) one. Lat/long #0
     * are placed at the origin of the XY coordinate system. The returned value
     * is an XY position of point #1 relative to point #0.
     * <p>
     *
     * This projection has the property that the Euclidean distance in the XY
     * frame is equal to the great circle distance in the lat/lon frame. In
     * addition, the course from the origin to the returned x,y point is the
     * same as the course at the mid point on the great circle arc.
     * <p>
     *
     * This transform has a symmetric correspondence, that is, it doesn't matter
     * which point is the origin: <tt>projectXY(lat0,lon0,lat1,lon1) =
     * -projectXY(lat1,lon1,lat0,lon0)</tt>
     */
    static Vect2 projectXY(double lat0, double lon0, double lat1, double lon1);
    
    /**
     * This method performs a particular projection from a spherical Earth
     * latitude/longitude coordinate system to Euclidean (XYZ) one. Lat/long #0
     * are placed at the origin of the Euclidean coordinate system. The returned value
     * is the position of point #1 relative to point #0.
     * <p>
     *
     * This projection has the property that the Euclidean distance in the XY
     * frame is equal to the great circle distance in the lat/lon frame. In
     * addition, the course from the origin to the returned x,y point is the
     * same as the course at the mid point on the great circle arc.
     * <p>
     *
     * This transform has a symmetric correspondence, that is, it doesn't matter
     * which point is the origin: <tt>projectXY(lat0,lon0,lat1,lon1) =
     * -projectXY(lat1,lon1,lat0,lon0)</tt>
     */
    static Vect3 projectXYZ(const LatLonAlt& p0, const LatLonAlt& p1);

    /** Return a projection with the pole as a reference point. */
    static Vect2 polar_xy(const LatLonAlt& lla, bool north);

    /** Invert a projection, using the pole as a reference point. */
    static LatLonAlt polar_inverse(const Vect2& v, double alt, bool north);

  };

  /**
   * \deprecated {Used SimpleProjection:: version.}
   * Return a projection with the pole as a reference point. */
  Vect2 polar_xy(const LatLonAlt& lla, bool north);
  
  /**
   * \deprecated {Used SimpleProjection:: version.}
   * Invert a projection, using the pole as a reference point. */
  LatLonAlt polar_inverse(const Vect2& v, double alt, bool north);
  
}


#endif /* SIMPLEPROJECTION_H_ */
