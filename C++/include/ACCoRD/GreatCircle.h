/* 
 * GreatCircle
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
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
   * <li>All course angles (i.e., desired track angles) are in radians clockwise
   * from true north.
   * <li>When the returned value is in lat/long and it is a Vect2, latitude is in
   * the "x" position and longitude is in the "y" position.
   * </ul>
   */
  class GreatCircle {
  public:

	static const double minDt; // small number, about machine


	/**
	 * Convert an angle in degrees/minutes/seconds into internal units
	 * (radians). The flag indicates if this angle is north (latitude) or east
	 * (longitude).  <p>
	 * 
	 * If the degrees is negative (representing 
	 * south or west), then the flag is ignored.
	 */
    static double decimal_angle(double degrees, double minutes, double seconds, bool north_east);


	/**
	 * Convert the distance (in internal length units) across a sphere at a
	 * height of h (in internal length units) above surface of the (spherical)
	 * Earth into an angle.
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
   * Convert the given angle into a distance across a
   * (spherical) Earth at height above the surface of h.
   */
  static double distance_from_angle(double angle, double h);

  /**
   * Compute the great circle distance in radians between the two points. The
   * calculation applies to any sphere, not just a spherical Earth. The
   * current implementation uses the haversine formula.
   */
  static double angular_distance(double lat1, double lon1, double lat2, double lon2);

  /**
   * Compute the great circle distance in radians between the two points. The
   * calculation applies to any sphere, not just a spherical Earth. The
   * current implementation uses the haversine formula.
   */
  static double angular_distance(const LatLonAlt& p1, const LatLonAlt& p2);

  /**
   * Compute the great circle distance between the two given points. The
   * calculation assumes the Earth is a sphere
   */
  static double distance(double lat1, double lon1, double lat2, double lon2);

	/**
	 * Compute the great circle distance between the two given points. The
	 * calculation assumes the Earth is a sphere. This ignores the altitudes.
	 */
  static double distance(const LatLonAlt& p1, const LatLonAlt& p2);

	/**
	 * Determines if two points are close to each other, see
	 * Constants.get_horizontal_accuracy().
	 */
  static bool almost_equals(double lat1, double lon1, double lat2, double lon2);

  /**
   * Determines if two points are close to each other, where 'close'
   * is defined by the distance parameter given in meters.
   */							  
  static bool almost_equals(double lat1, double lon1, double lat2,
			       double lon2, double epsilon);
	
  /**
   * The initial true course (course relative to true north) at lat/long #1 on
   * the great circle route from lat/long #1 to lat/long #2. The value is in
   * internal units of angles (radians), and is a compass angle [0..2*Pi]: clockwise
   * from true north.
   * <p>
   * 
   * Usage Note: If lat/long #1 and #2 are close to each other, then the
   * initial course may become unstable. In the extreme case when lat/long #1
   * equals lat/long #2, then the initial course is undefined.
   */
  static double initial_course(double lat1, double lon1, double lat2, double lon2);

  /**
   * The initial true course (course relative to true north) at point #1 on
   * the great circle route from point #1 to point #2. The value is in
   * internal units of angles (radians), and is a compass angle [0..2*Pi]: clockwise
   * from true north.
   * <p>
   * 
   * Usage Note: If point #1 and #2 are close to each other, then the
   * initial course may become unstable. In the extreme case when point #1
   * equals point #2, then the initial course is undefined.
   */
  static double initial_course(LatLonAlt p1, LatLonAlt p2);

  static double final_course(LatLonAlt p1, LatLonAlt p2);

  /**
   * A representative course (course relative to true north) for the entire
   * arc on the great circle route from lat/long #1 to lat/long #2. The value
   * is in internal units of angles (radians), and is a compass angle [0..2*Pi]:
   * clockwise from true north. This is currently calculated as the initial
   * course from the midpoint of the arc to its endpoint.
   */
  static double representative_course(double lat1, double lon1,
				      double lat2, double lon2);
  
  static double representative_course(const LatLonAlt& p1, const LatLonAlt& p2);

  
  /**
   * Find the position (latitude, longitude, and altitude) of a point on the great circle from
   * point #1 to point #2 as a fraction of the distance between the two
   * points. If the fraction is 0.0 then point #1 is returned, if the
   * fraction is 1.0 then point #2 is returned. If a fraction less than
   * zero or greater than one is used, then this function will extrapolate
   * along the great circle.
   * <p>
   * 
   * Usage Notes:
   * <ul>
   * <li>The return value r has r.x as latitude and r.y as
   * longitude. This is different than in the Vect4 class.
   * <li>Behavior of this function is undefined if the two points are
   * antipodal (i.e. lat1+lat2=0 and abs(lon1-lon2)=pi) because a unique great
   * circle line is undefined (there are infinitely many of them).
   * <li>if lat/long #1 is almost the same as #2, then #1 is returned 
   * </ul>
   */
  static LatLonAlt interpolate(const LatLonAlt& p1, const LatLonAlt& p2, double f);
  
  // This is a fast but crude way of interpolating between relatively close geodesic points
  static LatLonAlt interpolateEst(const LatLonAlt& p1, const LatLonAlt& p2, double f);

  /**
   * Find a point on the great circle route from point #1 to point #2,
   * traveling at the given velocity (only ground speed and vertical speed, not 
   * track angle) for the given amount of time. If
   * points #1 and #2 are essentially the same, then the direction between
   * these two points is undefined, so the first point is returned.
   * <p>
   * 
   * This calculation ignores altitude. Small errors (typically less than
   * 0.5%) will be introduced at typical aircraft altitudes.
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
   * for the given amount of time.  This calculation
   * follows the rhumb line (loxodrome or line of constant track).<p>
   *
   * Modern aircraft (and most ships) usually travel great circles not rhumb lines,
   * therefore linear_initial() is usually the preferred over this function.<p>
   *
   * At "normal" latitudes, rhumb lines are usually within a few percent of the
   * great circle route.  However, near the poles the behavior of rhumb lines is
   * not intuitive: if the destination is a point near the pole, then the rhumb line
   * may spiral around the pole to get to the destination.  In fact, if you maintain
   * a constant track angle along a rhumb line for a long enough distance, gradually
   * the line will spiral in towards one of the poles.
   * <p>
   * 
   * Rhumb lines are not defined at the exact north and south poles, therefore if
   * the origin or destination is precisely at a pole, this function will choose a
   * point near the pole.<p>
   *
   * This calculation is approximate: small errors (typically less than
   * 0.5%) will be introduced at typical aircraft altitudes.
   */
  static LatLonAlt linear_rhumb(const LatLonAlt& s, const Velocity& v, double t);

  /**
   * Find a point from the given lat/lon at an angle of 'track' at a distance of 'dist'.
   * This calculation follows the rhumb line (loxodrome or line of constant track).<p>
   *
   * Modern aircraft (and most ships) usually travel great circles not rhumb lines,
   * therefore linear_initial() is usually the preferred over this function.<p>
   *
   * At "normal" latitudes, rhumb lines are usually within a few percent of the
   * great circle route.  However, near the poles the behavior of rhumb lines is
   * not intuitive: if the destination is a point near the pole, then the rhumb line
   * may spiral around the pole to get to the destination.  In fact, if you maintain
   * a constant track angle along a rhumb line for a long enough distance, gradually
   * the line will spiral in towards one of the poles.
   * <p>
   * 
   * Rhumb lines are not defined at the exact north and south poles, therefore if
   * the origin or destination is precisely at a pole, this function will choose a
   * point near the pole.<p>
   *
   * This calculation is approximate: small errors (typically less than
   * 0.5%) will be introduced at typical aircraft altitudes.
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

private:
	static bool gauss_check(double a, double b, double c, double A, double B, double C);
public:

	/**
	 * Find a point from the given lat/lon when traveling along the great circle with
	 * the given initial velocity for the given amount of time.  
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than
	 * 0.5%) will be introduced at typical aircraft altitudes.
	 */
  static LatLonAlt linear_initial(const LatLonAlt& s, const Velocity& v, double t);

	/**
	 * Find a point from the given lat/lon at an angle of 'track' at a distance
	 * of 'dist'. This calculation follows the great circle.
	 * <p>
	 * 
	 * This calculation is approximate: small errors (typically less than 0.5%)
	 * will be introduced at typical aircraft altitudes.<p>
	 * 
	 * @param s     a position
	 * @param track the second point to define the great circle
	 * @param dist  distance from point #1 [m]
     * @return a new position that is distance d from point #1
	 * 
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
     * This returns the point on the great circle that running through p1 and p2 that is closest to point x.
     * This uses Napier's rules for right spherical triangles for the non-collinear case. 
     * The altitude of the output is the same as x.<p>
     * If p1 and p2 are the same point, then every great circle runs through them, thus x is on one of these great circles.  In this case, x will be returned.  
     * This assumes any 2 points will be within 90 degrees of each other (angular distance).
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
     * This uses Napier's rules for right spherical triangles for the non-collinear case.
     * This assumes any 2 points will be within 90 degrees of each other (angular distance).
     * @param p1 the starting point of the great circle
     * @param p2 another point on the great circle
     * @param x point to determine closest segment point to.
     * @return the LatLonAlt point on the segment that is closest (horizontally) to x
     */
    static LatLonAlt closest_point_segment(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& x);
    
    /**
     * Given two great circles defined by a1,a2 and b1,b2, return the intersection poin that is closest a1.  Use LatLonAlt.antipode() to get the other value.
     * This assumes that the arc distance between a1,a2 < 90 and b1,b2 < 90
     * This returns an INVALID value if both segments are collinear
     * EXPERIMENTAL
     */
    static LatLonAlt intersection(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2);

    static std::pair<LatLonAlt,double> intersection(const LatLonAlt& so, const LatLonAlt& so2, double dto, const LatLonAlt& si, const LatLonAlt& si2);

    static std::pair<LatLonAlt,double> intersection(const LatLonAlt& so, const Velocity& vo, const LatLonAlt& si, const Velocity& vi, bool checkBehind);

    /**
     * Given two great circles defined by so, so2 and si, si2 return the acute angle between them at the
     * intersection point.  This is the same as the dihedral angle, or angle between the two GC planes.
     * Note this may not be the same angle as the one projected into the Euclidean (unless the projection point is the intersection point),
     * and will generally not be the same as the (non-projected) track angle difference between them.
     */
    static double angleBetween(const LatLonAlt& a1, const LatLonAlt& a2, const LatLonAlt& b1, const LatLonAlt& b2);

	/**
	 * Return angle between great circles
	 * @param a point on gc1
	 * @param b intersection of gc1 and gc2
	 * @param c point on gc2
	 * @return
	 */
    static double angle_between(const LatLonAlt& a, const LatLonAlt& b, const LatLonAlt& c);

    /**
     * Return true if x is "behind" ll, considering its current direction of travel, v.
     * "Behind" here refers to the hemisphere aft of ll.
     * That is, x is within the region behind the perpendicular line to v through ll.
     * @param ll aircraft position
     * @param v aircraft velocity
     * @param x intruder positino
     * @return
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
	 */
  static Velocity velocity_final(const LatLonAlt& p1, const LatLonAlt& p2, double t);


    /**
     * Transforms a lat/lon position to a point on in R3 (on a sphere)
     * This is an Earth-Centered, Earth-Fixed translation (assuming earth-surface altitude).
     * From Wikipedia http://en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
     * We take a standard radius of the earth as defined in GreatCircle, and treat altitude as 0. 
     * @param lat Latitude
     * @param lon Longitude
     * @return point in R3 on surface of the earth
     */
  static Vect3 spherical2xyz(double lat, double lon);

    /**
     * Transforms a R3 position on the earth surface into lat/lon coordinates
     * This is an Earth-Centered, Earth-Fixed translation (assuming earth-surface altitude).
     * From Wikipedia http://en.wikipedia.org/wiki/Curvilinear_coordinates (contents apparently moved to Geodetic datum entry)
     * We take a standard radius of the earth as defined in GreatCircle, and treat altitude as 0. 
     * @param v position in R3, with ECEF origin
     * @return LatLonAlt point on surface of the earth (zero altitude)
     */
  static LatLonAlt xyz2spherical(const Vect3& v) ;



private:

  /**
   * Convert the distance (in internal length units) across the surface of the
   * (spherical) Earth into an angle.
   */
  static double angle_from_distance(double distance);

  };

}

#endif /* GREATCIRCLE_H_ */
