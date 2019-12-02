/* 
 * GreatCircle
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef GREATCIRCLE_H_
#define GREATCIRCLE_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Constants.h"
#include "Util.h"
#include "Units.h"
#include <string>
#include "Velocity.h"
#include "LatLonAlt.h"
#include "Triple.h"


namespace larcfm {

/**
 * This class contains common formulas used for Great Circle calculations. All
 * of these calculations assume a spherical earth. Much of this is based on the
 * Aviation Formulary (v1.44) by Ed Williams.
 * <p>
 * 
 * Notes:
 * <ul>
 * <li>The Earth does not rotate. This may change.
 * <li>Positive latitude is north and positive longitude is east.
 * <li>All course angles (i.e., desired chord angles) are in radians clockwise
 * from true north.
 * <li>When the returned value is in lat/long and it is a Vect2, latitude is in
 * the "x" position and longitude is in the "y" position.
 * <li>For any of these calculations that rely on a radius for the earth, the
 * value used is GreatCircle.spherical_earth_radius.
 * <li>Great circles cannot be defined by antipodal points
 * </ul>
 */
  class GreatCircle {
  public:

	static const double minDt; // small number, about machine


	/**
	 * Convert the distance (in internal length units) across a sphere at a
	 * height of h (in internal length units) above surface of the (spherical)
	 * Earth into an angle.
	 * 
	 * @param distance distance [m]
	 * @param h height above surface of spherical Earth
	 * @return angular distance [radian]
	 */
    static double angle_from_distance(double distance, double h);


  /**
   * The radius of a spherical "Earth" assuming 1 nautical mile is 1852 meters
   * (as defined by various international committees) and that one nautical
   * mile is equal to one minute of arc (traditional definition of a nautical
   * mile). This value lies between the major and minor axis as defined by
   * WGS84.
   */
  static const double spherical_earth_radius;

	/**
	 * Convert the given angle into a distance across a (spherical) Earth at
	 * height above the surface of h.
	 * 
	 * @param angle angular distance [radian]
	 * @param h height above surface of spherical Earth
	 * @return linear distance [m]
	 */
  static double distance_from_angle(double angle, double h);

	/**
	 * Compute the great circle distance in radians between the two points. The
	 * calculation applies to any sphere, not just a spherical Earth. The
	 * current implementation uses the haversine formula.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return angular distance
	 */
  static double angular_distance(double lat1, double lon1, double lat2, double lon2);

	/**
	 * Compute the great circle distance in radians between the two points. The
	 * calculation applies to any sphere, not just a spherical Earth. The
	 * current implementation uses the haversine formula.
	 * 
	 * @param p1 one point
	 * @param p2 another point
	 * @return angular distance
	 */
  static double angular_distance(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Compute the great circle distance between the two given points. The
	 * calculation assumes the Earth is a sphere
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return distance
	 */
  static double distance(double lat1, double lon1, double lat2, double lon2);

	/**
	 * Compute the great circle distance between the two given points. The
	 * calculation assumes the Earth is a sphere. This ignores the altitudes.
	 * 
	 * @param p1 one point
	 * @param p2 another point
	 * @return angular distance
	 */
  static double distance(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Determines if two points are close to each other, see
	 * Constants.get_horizontal_accuracy().
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return true, if almost equals
	 */
  static bool almost_equals(double lat1, double lon1, double lat2, double lon2);

	/**
	 * Determines if two points are close to each other, where 'close'
	 * is defined by the distance value given in meters.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @param epsilon maximum difference
	 * @return true, if almost equals
	 */							  
  static bool almost_equals(double lat1, double lon1, double lat2,
			       double lon2, double epsilon);

  static bool almostEquals(const LatLonAlt& b, const LatLonAlt& v);
	/** Are these two LatLonAlt almost equal, where 'almost' is defined by the given distances [m]
	 * 
	 * @param b LatLonAlt object
	 * @param a LatLonAlt object
	 * @param horizEps allowed difference in horizontal dimension
	 * @param vertEps allowed difference in vertical dimension
	 * @return true if the two are almost equals
	 */
  static bool almostEquals(const LatLonAlt& b, const LatLonAlt& a, double horizEps, double vertEps);
  static bool almostEquals2D(const LatLonAlt& b, const LatLonAlt& v, double horizEps);
	
	/**
	 * The initial true course (course relative to true north) at lat/long #1 on
	 * the great circle route from lat/long #1 to lat/long #2. The value is in
	 * internal units of angles (radians), and is a compass angle [0..2*Pi]:
	 * clockwise from true north.
	 * <p>
	 * 
	 * Usage Note: If lat/long #1 and #2 are close to each other, then the
	 * initial course may become unstable. In the extreme case when lat/long #1
	 * equals lat/long #2, then the initial course is undefined.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return initial course
	 */
  static double initial_course(double lat1, double lon1, double lat2, double lon2);

	/**
	 * The initial true course (course relative to true north) at point #1 on
	 * the great circle route from point #1 to point #2. The value is in
	 * internal units of angles (radians), and is a compass angle [0..2*Pi]:
	 * clockwise from true north.
	 * <p>
	 * 
	 * Usage Note: If point #1 and #2 are close to each other, then the initial
	 * course may become unstable. In the extreme case when point #1 equals
	 * point #2, then the initial course is undefined.
	 * 
	 * @param p1 a point
	 * @param p2 another point
	 * @return initial course
	 */
  static double initial_course(const LatLonAlt& p1, const LatLonAlt& p2);

  static double final_course(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * A representative course (course relative to true north) for the entire
	 * arc on the great circle route from lat/long #1 to lat/long #2. The value
	 * is in internal units of angles (radians), and is a compass angle
	 * [0..2*Pi]: clockwise from true north. This is currently calculated as the
	 * initial course from the midpoint of the arc to its endpoint.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return representative course
	 */
  static double representative_course(double lat1, double lon1,
				      double lat2, double lon2);
  
  static double representative_course(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Find the maximum latitude of the great circle defined by the
	 * two points.
	 *
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return maximum latitude
	 */
	static double max_latitude_gc(double lat1, double lon1, double lat2, double lon2) ;

  private:
	static double max_latitude_gc_course(double lat1, double lon1, double lat2, double lon2, double trk) ;

  public:
	static double max_latitude_gc(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Find the minimum latitude of the great circle defined by the
	 * two points.
	 *
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return minimum latitude
	 */
	static double min_latitude_gc(double lat1, double lon1, double lat2, double lon2);

  private:
	static double min_latitude_gc_course(double lat1, double lon1, double lat2, double lon2, double trk);

  public:
	static double min_latitude_gc(const LatLonAlt& p1, const LatLonAlt& p2);

	// Given a great circle defined by Point 1 and 2, find the longitude of where it
	// crosses the latitude defined by lat3.
	//
	// from Aviation Formulary
	// longitude sign is reversed from the formulary!
	static double lonCross(double lat1, double lon1, double lat2, double lon2, double lat3);


  
	/**
	 * Find the maximum (northern-most) latitude of the line segment defined by the two points
	 * along a great circle path.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return maximum latitude
	 */
	static double max_latitude(double lat1, double lon1, double lat2, double lon2);

	static double max_latitude(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Find the minimum (southern-most) latitude of the line segment defined by the two points
	 * along a great circle path.
	 * 
	 * @param lat1 latitude of point 1
	 * @param lon1 longitude of point 1
	 * @param lat2 latitude of point 2
	 * @param lon2 longitude of point 2
	 * @return minimum latitude
	 */
	static double min_latitude(double lat1, double lon1, double lat2, double lon2);

	static double min_latitude(const LatLonAlt& p1, const LatLonAlt& p2);


	/**
	 * Find the position (latitude, longitude, and altitude) of a point on the
	 * great circle from point #1 to point #2 as a fraction of the distance
	 * between the two points. If the fraction is 0.0 then point #1 is returned,
	 * if the fraction is 1.0 then point #2 is returned. If a fraction less than
	 * zero or greater than one is used, then this function will extrapolate
	 * along the great circle.
	 * <p>
	 * 
	 * Usage Notes:
	 * <ul>
	 * <li>The return value r has r.x as latitude and r.y as longitude. This is
	 * different than in the Vect4 class.
	 * <li>Behavior of this function is undefined if the two points are
	 * antipodal (i.e. lat1+lat2=0 and abs(lon1-lon2)=pi) because a unique great
	 * circle line is undefined (there are infinitely many of them).
	 * <li>if lat/long #1 is almost the same as #2, then #1 is returned
	 * </ul>
	 * 
	 * @param p1 point #1
	 * @param p2 point #1
	 * @param f decimal fraction
	 * @return a new point between p1 and p2
	 */
  static LatLonAlt interpolate(const LatLonAlt& p1, const LatLonAlt& p2, double f);
  
  // This is a fast but crude way of interpolating between relatively close geodesic points
	/**
	 * This is a fast but crude way of interpolating between relatively close geodesic points
	 * 
	 * @param p1 point #1
	 * @param p2 point #1
	 * @param f decimal fraction
	 * @return a new point between p1 and p2
	 */
  static LatLonAlt interpolateEst(const LatLonAlt& p1, const LatLonAlt& p2, double f);

	/**
	 * Find a point on the great circle route from point #1 to point #2,
	 * traveling at the given velocity (only ground speed and vertical speed,
	 * not track angle) for the given amount of time. If points #1 and #2 are
	 * essentially the same, then the direction between these two points is
	 * undefined, so the first point is returned.
	 * <p>
	 * 
	 * This calculation ignores altitude. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 * 
	 * @param p1 a point
	 * @param p2 another point
	 * @param v velocity
	 * @param t time
	 * @return end point of a linear extrapolation
	 */
  static LatLonAlt linear_gcgs(const LatLonAlt& p1, const LatLonAlt& p2, const Velocity& v, double t);

	/**
	 * Return a new location on the great circle path from p1 to p2 that is
	 * distance d from p1
	 * 
	 * @param p1   the first point to define the great circle
	 * @param p2   the second point to define the great circle
	 * @param d    distance from point #1 [m]
     * @return a new position that is distance d from point #1
	 */
  static LatLonAlt linear_gc(LatLonAlt p1, LatLonAlt p2, double d);


	/**
	 * Find a point from the given lat/lon when traveling at the given velocity
	 * for the given amount of time. This calculation follows the rhumb line
	 * (loxodrome or line of constant track).
	 * <p>
	 * 
	 * Modern aircraft (and most ships) usually travel great circles not rhumb
	 * lines, therefore linear_initial() is usually the preferred over this
	 * function.
	 * <p>
	 * 
	 * At "normal" latitudes, rhumb lines are usually within a few percent of
	 * the great circle route. However, near the poles the behavior of rhumb
	 * lines is not intuitive: if the destination is a point near the pole, then
	 * the rhumb line may spiral around the pole to get to the destination. In
	 * fact, if you maintain a constant track angle along a rhumb line for a
	 * long enough distance, gradually the line will spiral in towards one of
	 * the poles.
	 * <p>
	 * 
	 * Rhumb lines are not defined at the exact north and south poles, therefore
	 * if the origin or destination is precisely at a pole, this function will
	 * choose a point near the pole.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 * 
	 * @param s position
	 * @param v velocity
	 * @param t time
	 * @return linear extrapolation along a rhumb line
	 */
  static LatLonAlt linear_rhumb(const LatLonAlt& s, const Velocity& v, double t);

	/**
	 * Find a point from the given lat/lon at an angle of 'track' at a distance
	 * of 'dist'. This calculation follows the rhumb line (loxodrome or line of
	 * constant track).
	 * <p>
	 * 
	 * Modern aircraft (and most ships) usually travel great circles not rhumb
	 * lines, therefore linear_initial() is usually preferred over this
	 * function.
	 * <p>
	 * 
	 * At "normal" latitudes, rhumb lines are usually within a few percent of
	 * the great circle route. However, near the poles the behavior of rhumb
	 * lines is not intuitive: if the destination is a point near the pole, then
	 * the rhumb line may spiral around the pole to get to the destination. In
	 * fact, if you maintain a constant track angle along a rhumb line for a
	 * long enough distance, gradually the line will spiral in towards one of
	 * the poles.
	 * <p>
	 * 
	 * Rhumb lines are not defined at the exact north and south poles, therefore
	 * if the origin or destination is precisely at a pole, this function will
	 * choose a point near the pole.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 * 
	 * @param s position
	 * @param track track angle
	 * @param dist distance
	 * @return linear extrapolation along a rhumb line
	 */
  static LatLonAlt linear_rhumb(const LatLonAlt& s, double track, double dist);


	/**
	 * Solve the spherical triangle when one has a side (in angular distance), another side, and an angle between sides.
	 * The angle is <b>not</b> between the sides.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 *
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  A firstSolution == true
	 * will return a smaller angle, B, than firstSolution == false.
	 *
	 * @param b one side (in angular distance)
	 * @param a another side (in angular distance)
	 * @param A the angle opposite the side a
	 * @param firstSolution select which solution to use
	 * @return a Triple of angles B and C, and the side c.
	 */
	static Triple<double,double,double> side_side_angle(double b, double a, double A, bool firstSolution);

	/**
	 * Solve the spherical triangle when one has a side (in angular distance), and two angles.
	 * The side is <b>not</b> between the angles.  The sides are labeled a, b, and c.  The angles are labelled A, B, and
	 * C.  Side a is opposite angle A, and so forth.<p>
	 *
	 * Given these constraints, in some cases two solutions are possible.  To
	 * get one solution set the parameter firstSolution to true, to get the other set firstSolution to false.  A firstSolution == true
	 * will return a smaller side, b, than firstSolution == false.
	 *
	 * @param a one side (in angular distance)
	 * @param A the angle opposite the side a
	 * @param B another angle
	 * @param firstSolution select which solution to use
	 * @return a Triple of side b, angle C, and the side c.
	 */
	static Triple<double,double,double> side_angle_angle(double a, double A, double B, bool firstSolution);

	/**
	 * This implements the spherical cosine rule to complete a triangle on the unit sphere
	 * @param a side a (angular distance)
	 * @param C angle between sides a and b
	 * @param b side b (angular distance)
	 * @return triple of A,B,c (angle opposite a, angle opposite b, side opposite C)
	 */
	static Triple<double,double,double> side_angle_side(double a, double C, double b);

	/**
	 * This implements the supplemental (polar triangle) spherical cosine rule to complete a triangle on the unit sphere
	 * @param A angle A
	 * @param c side between A and B (angular distance
	 * @param B angle B
	 * @return triple of a,b,C (side opposite A, side opposite B, angle opposite c)
	 */
	static Triple<double,double,double> angle_side_angle(double A, double c, double B);

	static LatLonAlt intersectionSegment(double T, const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const LatLonAlt& si2);

	/**  EXPERIMENTAL
	 * Given two great circle segments defined by a1,a2 and b1,b2, return the intersection point that is closest a1.  
	 * This assumes that the arc distance between a1,a2 &lt; 90 and b1,b2 &lt; 90
	 * The altitude of the return value is equal to a1.alt()
	 * This returns an INVALID value if both segments are collinear
	 * 
	 * @param so  starting point of segment [so,so2]
	 * @param so2 ending point of segment [so,so2]
	 * @param si  starting point of segment [si,si2]
	 * @param si2 ending point of segment [si,si2]
	 * @return the point that intersects the two "great circle" segments
	 */
	static LatLonAlt intersectSegments(const LatLonAlt& so, const LatLonAlt& so2, const LatLonAlt& si, const LatLonAlt& si2);


private:
	static bool gauss_check(double a, double b, double c, double A, double B, double C);
public:

	/**
	 * Find a point from the given lat/lon ('s') when traveling along the great circle
	 * with the given initial velocity for the given amount of time.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.
	 * 
	 * @param s a position
	 * @param v velocity
	 * @param t time
	 * @return position that is t seconds from s going velocity v
	 */
  static LatLonAlt linear_initial(const LatLonAlt& s, const Velocity& v, double t);

	/**
	 * Find a point from the given lat/lon ('s') with an initial 'track' angle at a distance
	 * of 'dist'. This calculation follows the great circle.
	 * <p>
	 * 
	 * Note: this method does not compute an accurate altitude<p>
	 * 
	 * 
	 * @param s     a position
	 * @param track the second point to define the great circle
	 * @param dist  distance from point #1 over the surface of the Earth [m]
	 * @return a new position that is distance d from point #1
	 */
  static LatLonAlt linear_initial(const LatLonAlt& s, double track, double dist);

  /**
   * This function forms a great circle from p1 to p2, then computes
   * the shortest distance of another point (offCircle) to the great circle.  This is the
   * cross track distance. A positive
   * value means offCircle is to the right of the path from p1 to p2.  A
   * negative value means offCircle is to the left of the path from p1 to p2.<p>
   *
   * @param p1 the starting point of the great circle
   * @param p2 another point on the great circle
   * @param offCircle the point to measure the cross track distance
   * @return the cross track distance [m]
   */
  static double cross_track_distance(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& offCircle);

    /**
     * Determines if the three points are on the same great circle.
     * @param p1 One point
     * @param p2 Second point
     * @param p3 Third point
     * @return true, if the three points are collinear
     */
    static bool collinear(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3);

	/**
	 * This returns the point on the great circle running through p1 and p2 that is closest to point x.
	 * The altitude of the output is the same as x.<p>
	 * If p1 and p2 are the same point, then every great circle runs through them, thus x is on one of these great circles.  In this case, x will be returned.  
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
    static LatLonAlt closest_point_circle(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x);

  private:
    static LatLonAlt closest_point_circle(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x, double a, double b, double c, double A, double B, double C);
  public:
    
	/**
	 * This returns the point on the great circle segment running through p1 and p2 that is closest to point x.
	 * This will return either p1 or p2 if the actual closest point is outside the segment.
	 * @param p1 the starting point of the great circle
	 * @param p2 another point on the great circle
	 * @param x point to determine closest segment point to.
	 * @return the LatLonAlt point on the segment that is closest (horizontally) to x
	 */
    static LatLonAlt closest_point_segment(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x);
    
	/**
	 * Given two great circles defined by a1,a2 and b1,b2, return the intersection point that is closest a1.  Use LatLonAlt.antipode() to get the other value.
	 * This assumes that the arc distance between a1,a2 &lt; 90 and b1,b2 &lt; 90
	 * The altitude of the return value is equal to a1.alt()
	 * This returns an INVALID value if both segments are collinear
	 * 
	 * @param a1 point #1 to form great circle #1
	 * @param a2 point #2 to form great circle #1
	 * @param b1 point #1 to form great circle #2
	 * @param b2 point #2 to form great circle #2
	 * @return the point that intersects the two great circles
	 */
    static LatLonAlt intersection(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2);

    static std::pair<LatLonAlt,double> intersectionExtrapAlt(const LatLonAlt& so, const LatLonAlt& so2, double dto, const LatLonAlt& si, const LatLonAlt& si2);

    static std::pair<LatLonAlt,double>  intersectionAvgAlt(const LatLonAlt& so, const LatLonAlt& so2, double dto, const LatLonAlt& si, const LatLonAlt& si2);


    static std::pair<LatLonAlt,double> intersection(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi);

	/**
	 * Given two great circles defined by so, so2 and si, si2 return the angle between them at the intersection point.  
	 * This is the same as the dihedral angle, or angle between the two GC planes. 
	 * Note this may not be the same angle as the one projected into the Euclidean (unless the projection point is the intersection point). 
	 * and will generally not be the same as the (non-projected) track angle difference between them (though that can be very close).
	 * This will always return a value between 0 and PI.
	 * 
	 * Note: If a1 and b1 or if a2 and b2 are the "closest" to the intersection point (and both a's are on the same side of the 
	 * intersection point, similarly for both b's), this will return the angle "between" them, the one you
	 * would go through to make the shortest turn from a2 to b2 (or from b2 to a2), which may or may not be acute.  
	 * 
	 * @param a1 one point on the first great circle
	 * @param a2 second point on the first great circle
	 * @param b1 one point on the second great circle
	 * @param b2 second point on the second great circle
	 * @return angle between two great circles
	 */
    static double angleBetween(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2);

	/**
	 * Return angle between great circles (old version, uses spherical trig)
	 * @param a point on gc1
	 * @param b intersection of gc1 and gc2
	 * @param c point on gc2
	 * @return angle between the two great circles
	 */
    static double angle_betweenOLD(const LatLonAlt& a, const LatLonAlt& b, const LatLonAlt& c);

	/**
	 * Return the turn angle between great circles (this will return a value between 0 and PI)
	 * (uses coordinate transformation)
	 * @param a point on gc1
	 * @param b intersection of gc1 and gc2
	 * @param c point on gc2
	 * @return magnitude of angle between the two great circles, from a-b to b-c
	 */
    static double angle_between(const LatLonAlt& a, const LatLonAlt& b, const LatLonAlt& c);

	/**
	 * Return the turn angle between two great circles, measured in the indicated direction.  This can return a value larger than PI.
	 * @param a first point
	 * @param b turn point
	 * @param c last point
	 * @param dir +1 is right (clockwise), -1 is left (counterclockwise)
	 * @return Value of angle of turn from a-b to b-c
	 */
    static double angle_between(const LatLonAlt& a, const LatLonAlt& b, const LatLonAlt& c, int dir);


	/**
	 * Return true if x is "behind" ll, considering its current direction of travel, v.
	 * "Behind" here refers to the hemisphere aft of ll.
	 * That is, x is within the region behind the perpendicular line to v through ll.
	 * @param ll aircraft position
	 * @param v aircraft velocity
	 * @param x intruder position
	 * @return true, if x is behind ll
	 */
    static bool behind(const LatLonAlt& x, const LatLonAlt& ll, const Velocity& v) ;

   	/**
   	 * Returns values describing if the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
   	 * @param so ownship position
   	 * @param vo ownship velocity
   	 * @param si intruder position
   	 * @param vi intruder velocity
   	 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if collinear or parallel or closest intersection is behind you
   	 */
    static int passingDirection(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi) ;

    static int dirForBehind(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi);


  
	/**
	 * Estimate the velocity on the great circle from lat/lon #1 to lat/lon #2
	 * with the given amount of time. Essentially, the velocity at the mid point
	 * between lat/lon #1 and lat/lon #2. If points #1 and #2 are essentially
	 * the same (about 1 meter apart), then a zero vector is returned. Also if
	 * the absolute value of time is less than 1 [ms], then a zero vector is
	 * returned.
	 * <p>
	 * 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.<p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 * 
	 * @param p1 a point
	 * @param p2 another point
	 * @param t time
	 * @return average velocity
	 */
  static Velocity velocity_average(const LatLonAlt& p1, const LatLonAlt& p2, double t);


	/**
	 * Estimate the velocity on the great circle from lat/lon #1 to lat/lon #2
	 * with the given speed. 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.<p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 * 
	 * @param s1 a point
	 * @param s2 another point
	 * @param speed speed between point
	 * @return average velocity
	 */
  static Velocity velocity_average_speed(const LatLonAlt& s1, const LatLonAlt& s2, double speed);


	/**
	 * Compute the initial velocity on the great circle from lat/lon #1 to
	 * lat/lon #2 with the given amount of time. If points #1 and #2 are
	 * essentially the same (about 1 meter apart), then a zero vector is
	 * returned. Also if the absolute value of time is less than 1 [ms], then a
	 * zero vector is returned.
	 * <p>
	 * 
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.
	 * <p>
	 * 
	 * This calculation ignores altitude when calculating great circle distance.
	 * Small errors (typically less than 0.5%) will be introduced at typical
	 * aircraft altitudes.
	 * 
	 * @param p1 point 1
	 * @param p2 point 2
	 * @param t time
	 * @return velocity from point 1 to point 2, taking time t
	 */
  static Velocity velocity_initial(const LatLonAlt& p1, const LatLonAlt& p2, double t);

	/**
	 * Estimate the final velocity on the great circle from lat/lon #1 to
	 * lat/lon #2 with the given amount of time. The track angle of the velocity
	 * is the course from point #1 to #2 roughly at point #2. If points #1 and
	 * #2 are essentially the same (about 1 meter apart), then a zero vector is
	 * returned. Also if the absolute value of time is less than 1 [ms], then a
	 * zero vector is returned.
	 * <p>
	 *  
	 * If the time is negative, then the velocity is along the great circle
	 * formed by #1 and #2, but in the opposite direction from #2.
	 * <p>
	 * 
	 * This is an estimate of the velocity. This calculation ignores altitude
	 * when calculating great circle distance. Small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 * 
	 * @param p1 a point
	 * @param p2 another point
	 * @param t time
	 * @return final velocity 
	 */
  static Velocity velocity_final(const LatLonAlt& p1, const LatLonAlt& p2, double t);


	/**
	 * Transforms a lat/lon position to a point in R3 (on a sphere)
	 * This is an Earth-Centered, Earth-Fixed translation (assuming earth-surface altitude).
	 * From Wikipedia: en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
	 * 
	 * The x-axis intersects the sphere of the earth at 0 latitude (the equator) and 0 longitude (Greenwich). 
	 * 	
	 * @param lat latitude
	 * @param lon longitude
	 * @return point in R3 on surface of the earth (zero altitude)
	 */
  static Vect3 spherical2xyz(double lat, double lon);

	/**
	 * Transforms a lat/lon position to a point in R3 (on a sphere)
	 * This is an Earth-Centered, Earth-Fixed translation (ECEF, assuming earth-surface altitude).
	 * From Wikipedia: en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
	 * 
	 * The x-axis intersects the sphere of the earth at 0 latitude (the equator) and 0 longitude (Greenwich). 
	 * 	
	 * @param lla lattitude/longitude point
	 * @return point in R3 on surface of the earth (zero altitude)
	 */
  static Vect3 spherical2xyz(const LatLonAlt& lla);

	/**
	 * Transforms a R3 position on the earth surface into lat/lon coordinates
	 * This is an Earth-Centered, Earth-Fixed translation (ECEF, assuming earth-surface altitude).
	 * From Wikipedia: en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
	 * We take a standard radius of the earth as defined in GreatCircle, and treat altitude as 0. 
	 * @param v position in R3, with ECEF origin
	 * @return LatLonAlt point on surface of the earth (zero altitude)
	 */
  static LatLonAlt xyz2spherical(const Vect3& v) ;


  	/**
	 * Return the straight-line chord distance (through a spherical earth) from 
	 * two points on the surface of the earth. 
	 * 
	 * @param lat1 latitude of first point
	 * @param lon1 longitude of first point
	 * @param lat2 latitude of second point
	 * @param lon2 longitude of second point
	 * @return the chord distance
	 */
  static double chord_distance(double lat1, double lon1, double lat2, double lon2);
 
	/**
	 * Return the chord distance (through the earth) corresponding to a given surface distance (at the nominal earth radius).
	 * This is the distance of a direct line between two surface points.
	 * @param surface_dist distance across surface
	 * @return chord distance
	 */
	static double chord_distance(double surface_dist);
	
	/**
	 * Return the surface distance (at the nominal earth radius) corresponding to a given chord distance (through the earth).
	 * @param chord_distance cordal distance
	 * @return surface distance
	 */
	static double surface_distance(double chord_distance);
	
	static double to_chordal_radius(double surface_radius);

	static double to_surface_radius(double chord_radius);



		/**
	 * EXPERIMENTAL
	 * Given a small circle, rotate a point
	 * @param so point on circle
	 * @param center center of circle
	 * @param angle angle of rotation around center (positive is clockwise)
	 * @return another position on the circle
	 */
	static LatLonAlt small_circle_rotation(const LatLonAlt& so, const LatLonAlt& center, double angle);

	/**
	 * Accurately calculate the linear distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcAngle angular (radian) length of the arc.  This is the angle between two great circles that intersect at the small circle's center.
	 * @return linear distance of the small circle arc
	 * Note: A 100 km radius turn over 60 degrees produces about 4.3 m error.
	 */
	static double small_circle_arc_length(double radius, double arcAngle);

	/**
	 * Accurately calculate the angular distance of an arc on a small circle (turn) on the sphere.
	 * @param radius along-surface radius of small circle
	 * @param arcLength linear (m) length of the arc.  This is the along-line length of the arc.
	 * @return Angular distance of the arc around the small circle (from 0 o 2pi)
	 * Note: A 100 km radius turn over 100 km of turn produces about 0.0024 degrees of error.
	 */
	static double small_circle_arc_angle(double radius, double arcLength);


private:

	/**
	 * Convert the distance (in internal length units) across the <b>surface</b> of the
	 * (spherical) Earth into an angle.
	 */
  static double angle_from_distance(double distance);

  };

}

#endif /* GREATCIRCLE_H_ */
