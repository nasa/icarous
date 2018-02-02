/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TurnGeneration_H
#define TurnGeneration_H

#include "Units.h"
#include "Triple.h"
#include "Quad.h"
#include "Tuple5.h"
#include "Tuple6.h"
#include "ErrorLog.h"
#include "NavPoint.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"

namespace larcfm {
  
/**
 *  TurnGeneration provides methods to generate the beginning, middle, and end of turn (BOTs, MOTs, and EOTs).
 *  This class finds solutions that are
 *  based on spherical trigonometry without having to use projections (the original versions of these methods
 *  in TrajGen used Euclidean projections).   
 */
class TurnGeneration {

public:
  static Tuple5<NavPoint,NavPoint,NavPoint, int, Position> turnGenerator(const NavPoint& np1, const NavPoint& np2, int linearIndex, const NavPoint& np3, double radius);
  static Tuple6<Position,Position,Position,int,double,Position> turnGenerator(const Position& np1, const Position& np2, const Position& np3, double radius);
 	
	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal Components Only)
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param radius Radius of the turn
	 * @return BOT,MOT,EOT positions, and arclength from BOT to EOT   
	 * NOTE: The altitudes are not computed!!
	 * velocity in metadata is set correctly for BOT, with track values modified to match the MOT and EOT points
	 */
  static Tuple6<Vect2,Vect2,Vect2,int,double,Vect2> turnGeneratorEucl(const Vect2& pt1, const Vect2& pt2, const Vect2& pt3, double radius);
 
	
	/**
	 * Return the BOT and EOT trajectory change points for a turn (Horizontal Components Only)
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param radius Radius of the turn
	 * @return BOT,EOT pair   
	 * NOTE: The altitudes are NOT computed
	 * velocity in metadata is set correctly for BOT, with track values modified to match the MOT and EOT points
	 */
  static Tuple6<Vect3,Vect3,Vect3,int,double,Vect3> turnGeneratorEucl(const Vect3& p1, const Vect3& p2, const Vect3& p3, double radius);
 
	/**
	 * Return the BOT, MOT, and EOT TCPs for a turn. 
	 * @param np1 Start point of the leg before the turn
	 * @param np2 Vertex of the turn
	 * @param np3 End point of the leg after the turn
	 * @param R Radius of the turn
	 * @return BOT,MOT,EOT triple.   
	 * NOTE.   MOT.alt = np.alt()
	 * velocity in metadata is set correctly for BOT, with track values modified to match the MOT and EOT points
	 */
  static Quad<NavPoint,NavPoint,NavPoint,int> turnGeneratorProjected(const NavPoint& np1, const NavPoint& np2, int linearIndex, const NavPoint& np3, double R);
 	
	/**
	 * does NOT calculate an accurate altitude
	 * 
	 * @param p2
	 * @param trkIn
	 * @param trkOut
	 * @param radius
	 * @return returns BOT, MOT, EOT and distance from BOT to p2 = distance from EOT to p2
	 */
  static Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,int,double,LatLonAlt> turnGeneratorLLA(const LatLonAlt& p2, double trkIn, double trkOut, double radius);
 	
	/**
	 * Altitudes are not set.
	 * @param p1
	 * @param p2
	 * @param p3
	 * @param radius
	 * @return
	 */
  static Tuple6<LatLonAlt,LatLonAlt,LatLonAlt,int,double,LatLonAlt> turnGeneratorLLA(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius);
 	
	/**
	 * Altitudes are not set.
	 * 
	 * @param p1
	 * @param p2
	 * @param p3
	 * @param radius as a distance, not an angle
	 * @return
	 */
  static Triple<LatLonAlt,LatLonAlt,LatLonAlt> turnGeneratorLLA_Alt2(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius);
 	

	


private:	
  static double arcLengthLLA(const LatLonAlt& bot, const LatLonAlt& intercept, double radius);
	/**
	 * Rprime takes the turnRadius (great circle distance) and returns another distance.
	 *  Given a point c on the sphere (center of turn, for instance), and given another point
	 *  p on the sphere that has radius TurnRad (great circle radius), this functions
	 *  returns the distance (closest) between c and the line through the origin and p.
	 * @param TurnRad
	 * @return new radius
	 */
  static double Rprime(double TurnRad);
	/**
	 * returns a unit normal vector to the plane containing p, q, and the origin, in the direction of r.
	 * @param p Vect3
	 * @param q Vect3
	 * @param r Vect3
	 * @return Vect3
	 */
  static Vect3 normalvect(const Vect3& p, const Vect3& q, const Vect3& r);
	/**
	 * Plane1 is set of all points Rprime(TurnRad) distance from the plane containing p,q,0, in the direction of r
	 * Plane2 is set of all points Rprime(TurnRad) distance from the plane containing q,r,0, in the direction of p
	 * This function computes one point that is on the line of intersection between these two planes.
	 * @param TurnRad
	 * @param p
	 * @param q
	 * @param r
	 * @return Vect3 on the line
	 */
  static Vect3 point_on_line(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r);
  static Vect3 turnCenterxyz(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r);
  static Vect3 BOT(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r);
  static Vect3 EOT(double TurnRad, const Vect3& p, const Vect3& q, const Vect3& r);
	/**
	 * p,q,r are assumed to be on the surface of a sphere. This gives 1 for a right turn and -1 for a left turn.
	 * @param p point
	 * @param q point
	 * @param r point
	 * @return -1 or 1
	 */
  static int turnDirection(const Vect3& p, const Vect3& q, const Vect3& r);
  	

public:
  static Tuple5<LatLonAlt,LatLonAlt,LatLonAlt,int,double> turnGeneratorLLA_Alt3(const LatLonAlt& p1, const LatLonAlt& p2, const LatLonAlt& p3, double radius);
  	
	
};
}

#endif // TurnGeneration_H
