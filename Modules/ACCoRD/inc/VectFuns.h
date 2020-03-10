/*
 * VectFuns.h
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef VECTFUNS_H
#define VECTFUNS_H

#include "Vect3.h"
#include "Velocity.h"
#include "Triple.h"


namespace larcfm {

class VectFuns {
public:

	/**
	 * Returns true if aircraft are in loss of separation at time 0.
	 *
	 * @param so the relative position of the ownship aircraft
	 * @param si the relative position of the traffic aircraft
	 * @param D the minimum horizontal distance
	 * @param H the minimum vertical distance
	 *
	 * @return true, if aircraft are in loss of separation
	 */
	static bool LoS(const Vect3& so, const Vect3& si, double D, double H);


	/**
	 * Returns true if si is on the right side of the line (so,vo)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si traffic aircraft position
	 * @return true if is to the right of line (so,vo)
	 */
	static bool rightOfLine(const Vect2& so, const Vect2& vo, const Vect2& si);

	/**
	 * Return if point p is to the right or left of the line from A to B
	 * @param a point A
	 * @param b point B
	 * @param p point P
	 * @return 1 if to the right or collinear, -1 if to the left.
	 */
	static int rightOfLinePoints(const Vect2& a, const Vect2& b, const Vect2& p);

	static bool collinear(const Vect3& p0, const Vect3& p1, const Vect3& p2);

	static bool collinear(const Vect2& p0, const Vect2& p1, const Vect2& p2);

	static Vect2 midPoint(const Vect2& p0, const Vect2& p1);

	static Vect3 midPoint(const Vect3& p0, const Vect3& p1);

	static Vect3 interpolate(const Vect3& v1, const Vect3& v2, double f);

	// f should be between 0 and 1 to interpolate
	static Velocity interpolateVelocity(const Velocity& v1, const Velocity& v2, double f);

	static double angle_between(const Vect2& v1, const Vect2& v2);

	static double angle_between(const Vect2& v1, const Vect2& v2, const Vect2& v3);

	/**
	 * determines if divergent and relative speed is greater than a specified minimum relative speed
	 *
	 * @param s  relative position of ownship with respect to traffic
	 * @param vo initial velocity of ownship
	 * @param vi initial velocity of traffic
	 * @param minRelSpeed  the desired minimum relative speed
	 * @return   true iff divergent and relative speed is greater than a specified minimum relative speed
	 */
	static bool divergentHorizGt(const Vect2& s, const Vect2& vo, const Vect2& vi, double minRelSpeed);
	static bool divergentHorizGt(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed);

	/**
	 * Return if two aircraft in the given state are divergent in the horizontal plane
	 */
	static bool divergent(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi);


	/**
	 * Return if two aircraft in the given state are divergent in a 3D sense
	 * 
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si intruder position
	 * @param vi intruder velocity
	 * @return true, if divergent
	 */
	static bool divergent(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi);


	/**
	 * Return the horizontal rate of closure of two aircraft in the given state
	 * @param so position of first aircraft
	 * @param vo velocity of first aircraft
	 * @param si position of second aircraft
	 * @param vi velocity of second aircraft
	 * @return rate of closure
	 */
	static double rateOfClosureHorizontal(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) ;

	/**
	 * Return the vertical rate of closure of two aircraft in the given state
	 * @param so position of first aircraft
	 * @param vo velocity of first aircraft
	 * @param si position of second aircraft
	 * @param vi velocity of second aircraft
	 * @return rate of closure
	 */
	static double rateOfClosureVertical(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi);

	/** time of closest approach, if parallel return Double.MAX_VALUE
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @return     time of closest approach  (can be negative)
	 */
	static double tau(const Vect3& s, const Vect3& vo, const Vect3& vi);

	/**
	 * distance at time of closest approach
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @param futureOnly  if true then in divergent cases use distance now
	 * @return     distance at time of closest approach
	 */

	static double distAtTau(const Vect3& s, const Vect3& vo, const Vect3& vi, bool futureOnly);
	

	/**
	 * Computes 2D intersection point of two lines, but also finds z component (projected by time from line 1)
	 * @param s0 starting point of line 1
	 * @param v0 direction vector for line 1
	 * @param s1 starting point of line 2
	 * @param v1 direction vector of line 2
	 * @return Pair (2-dimensional point of intersection with 3D projection, relative time of intersection, relative to the so3)
	 * If the lines are parallel, this returns the pair (0,NaN).
	 */
	static std::pair<Vect3,double> intersection(const Vect3& so3, const Velocity& vo3, const Vect3& si3, const Velocity& vi3);

	static std::pair<Vect2,double> intersection2D(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi);

	 /**
     * Calculate the horizontal distance between two points.
     * 
     * @param soA   point A 
     * @param soB   point B 
     * @return horizontal distance between points.
     */
	static double distanceH(const Vect3& soA, const Vect3& soB);

	/**
	 * Computes 2D intersection point of two lines, but also finds z component as average of the closest end point of each line)
	 * This z-component is constrained to be within the z components of the defining points.
	 * @param so1 starting point of line o
	 * @param so2 ending point of line o
	 * @param si1 starting point of line i
	 * @param vi2 ending point of line i
	 * @return Pair (2-dimensional point of intersection, relative time of intersection, relative to the so1)
	 * This includes the average altitude between the *endpoints* closest to the point of intersection
	 * Note the intersection may be in the past (i.e. negative time)
	 * If the lines are parallel, this returns the pair (0,NaN).
	 */
	static std::pair<Vect3,double> intersectionAvgZ(const Vect3& so1, const Vect3& so2, double dto, const Vect3& si1, const Vect3& si2);

	static std::pair<Vect2,double> intersection2D(const Vect2& so1, const Vect2& so2, double dto, const Vect2& si1, const Vect2& si2);

	static std::pair<Vect2,double> intersectSegments(const Vect2& so, const Vect2& so2, const Vect2& si, const Vect2& si2);

	/**
	 * Return the closest point on the line a-b to point so as a 3D norm
	 * EXPERIMENTAL
	 * 
	 * @param a a point to define a line 
	 * @param b another point to define a line
	 * @param so a point
	 * @return the closest point
	 */
	static Vect3 closestPoint3(const Vect3& a, const Vect3& b, const Vect3& so);

	static Vect3 closestPoint(const Vect3& a, const Vect3& b, const Vect3& so);

	/**
	 * Return the closest (horizontal) point along line a-b to point so
	 * EXPERIMENTAL
	 * 
     * @param a a point to define a line 
	 * @param b another point to define a line
	 * @param so a point
	 * @return the closest point
	 */
	static Vect2 closestPoint(const Vect2& a, const Vect2& b, const Vect2& so);

	static Vect3 closestPointOnSegment(const Vect3& a, const Vect3& b, const Vect3& so);

	static Vect2 closestPointOnSegment(const Vect2& a, const Vect2& b, const Vect2& so);

	static Vect3 closestPointOnSegment3(const Vect3& a, const Vect3& b, const Vect3& so);

	static std::pair<Vect3,double> closestPointOnSegment3_extended(const Vect3& a, const Vect3& b, const Vect3& so);

	static double distanceToSegment(const Vect2& a, const Vect2& b, const Vect2& so);

	/**
	 * Computes 2D intersection point of two infinite lines
	 * @param so3 starting point of line 1
	 * @param vo3 direction vector for line 1
	 * @param si3 starting point of line 2
	 * @param vi3 direction vector of line 2
	 * @return time the OWNSHIP (so3) will reach the point.  Note that the intruder (si3) may have already passed this point.
	 * If the lines are parallel, this returns NaN.
	 */
	static double timeOfIntersection(const Vect3& so3, const Velocity& vo3, const Vect3& si3, const Velocity& vi3);

	/**
	 * Returns true if x is "behind" so , that is, x is within the region behind the perpendicular line to vo through so.  
	 * 
	 * @param x   a position
	 * @param so  aircraft position
	 * @param vo  aircraft velocity
	 * @return true, if x is behind so
	 */
    static bool behind(const Vect2& x, const Vect2& so, const Vect2& vo);

   /**
	 * Returns values indicating whether the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si intruder position
	 * @param vi intruder velocity
	 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if divergent or parallel
	 */
	static int passingDirection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi);


	static int dirForBehind(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi) ;

	static int dirForBehind(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) ;


};

}

#endif //VECTFUNS_H
