/* SimpleProjection
 * 
 * Contact: George Hagen (george.hagen@nasa.gov)
 * 
 * project spherical coordinates (lat/lon) into a Euclidean frame and the inverse.
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

/**
 * This is an interface for local Euclidean projections around a given point.  This projection may be used to
 * transform geodesic coordinates (LatLonAlt objects) into this Euclidean frame, using the project() method.  Also points
 * within this frame, may be found in geodesic coordinates with the inverse() method.   As long as the points are
 * close to the projection point, the errors should be very small.
 * 
 * Euclidean positions will be with respect to a projection point (this becomes the origin of the new Euclidean space).
 * Note that this will often alter relative position altitude information as well!  If this is not desired, the projection should 
 * be made against a zero-altitude point.
 * 
 * EuclideanProjection objects should never be created directly.  Instead they should be retrieved via the Projection.createProjection()
 * method.
 * 
 */
public interface EuclideanProjection {

	/** Return a new EuclideanProjection with the given reference point */
	public EuclideanProjection makeNew(LatLonAlt lla);
	
	/** Return a new EuclideanProjection with the given reference point */
	public EuclideanProjection makeNew(double lat, double lon, double alt);
	
	/** 
	 * Given an ownship latitude and desired accuracy, what is the longest distance to a intersection this projection will support? [m] 
	 */
	public double conflictRange(double lat, double accuracy);
	
	/**
	 *  What is the maximum effective horizontal range of this projection? [m] 
	 */
	public double maxRange();
	
	/** Get the projection point for this projection */
	public LatLonAlt getProjectionPoint();
	
    /** Return a projection of a lat/lon(/alt) point in Euclidean 2-space */
	public Vect2 project2(LatLonAlt lla);
    
    /** Return a projection of a lat/lon(/alt) point in Euclidean 3-space */
    public Vect3 project(LatLonAlt lla);

    /** Return a LatLonAlt value corresponding to the given Euclidean position */
    public LatLonAlt inverse(Vect3 xyz);

    /** Return a LatLonAlt value corresponding to the given Euclidean position */
    public LatLonAlt inverse(Vect2 xy, double alt);

    /** Return a projection of a Position in Euclidean 3-space (if already in Euclidian coordinate, this is the identity function) */
	public Vect3 project(Position sip);
	
    /** Return a projection of a Position in Euclidean 3-space (if already in Euclidian coordinate, this is the identity function) */
	public Point projectPoint(Position sip);
	
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
    public Velocity projectVelocity(LatLonAlt lla, Velocity v);
    
    /** Given a velocity from a point in geodetic coordinates, return a projection of this velocity in Euclidean 3-space */
    public Velocity projectVelocity(Position ss, Velocity v);
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity.  If toLatLon is true, the velocity is projected 
     *  into the geodetic coordinate space, otherwise it acts as the identity function. 
     *  
     *   @param s -- the projected ??
     *  */ 
    public Velocity inverseVelocity(Vect3 s, Velocity v, boolean toLatLon);
    
    /** Given a velocity from a point, return a projection of this velocity and the point in Euclidean 3-space.  If the position is already in Euclidean coordinates, this acts as the identity function. */ 
    public Pair<Vect3,Velocity> project(Position p, Velocity v);
    
    /** Given a velocity from a point in Euclidean 3-space, return a projection of this velocity and the point.  If toLatLon is true, the point/velocity is projected into the geodetic coordinate space */ 
    public Pair<Position,Velocity> inverse(Vect3 p, Velocity v, boolean toLatLon);
    
  
}
