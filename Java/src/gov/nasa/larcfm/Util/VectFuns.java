/*
 * VectFuns.java 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;

public final class VectFuns {

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
	public static boolean LoS(Vect3 so, Vect3 si, double D, double H) {
		Vect3 s = so.Sub(si);
		//return s.vect2().sqv() < Util.sq(D) && Math.abs(s.z) < H;
		return s.x*s.x + s.y*s.y < D*D && Math.abs(s.z) < H;
	}

	/**
	 * Returns true if si is on the right side of the line (so,vo)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si traffic aircraft position
	 * @return true if is to the right of line (so,vo)
	 */
	public static boolean rightOfLine(Vect2 so, Vect2 vo, Vect2 si) {
		return si.Sub(so).dot(vo.PerpR()) >= 0;
	}

	/**
	 * Return if point p is to the right or left of the line from A to B
	 * @param a point A
	 * @param b point B
	 * @param p point P
	 * @return 1 if to the right or collinear, -1 if to the left.
	 */	
	public static int rightOfLinePoints(Vect2 a, Vect2 b, Vect2 p) {
		Vect2 AB = b.Sub(a);
		Vect2 AP = p.Sub(a);
		//The calculation below is the 2-D cross product.
		return Util.sign(AP.x()*AB.y() - AP.y()*AB.x());
	}


	public static boolean collinear(Vect2 p0, Vect2 p1, Vect2 p2) {
		// area of triangle = 0? (same as det of sides = 0?)
		return  p1.Sub(p0).det(p2.Sub(p0)) == 0;
	}

	public static boolean collinear(Vect3 p0, Vect3 p1, Vect3 p2) {
		Vect3 v01 = p0.Sub(p1);
		Vect3 v02 = p1.Sub(p2);
		return v01.parallel(v02);
	}

	public static Vect2 midPoint(Vect2 p1, Vect2 p2) {
		return p1.Add(p2).Scal(0.5);
	}

	public static Vect3 midPoint(Vect3 v1, Vect3 v2) {
		return v1.Add(v2).Scal(0.5);
	}

	// f should be between 0 and 1 to interpolate
	public static Vect3 interpolate(Vect3 v1, Vect3 v2, double f) {
		Vect3 dv = v2.Sub(v1);
		return v1.Add(dv.Scal(f));
	}

	// f should be between 0 and 1 to interpolate
	public static Velocity interpolateVelocity(Velocity v1, Velocity v2, double f) {
		double newtrk = v1.trk() + f*(v2.trk() - v1.trk());
		double newgs = v1.gs() + f*(v2.gs() - v1.gs());
		double newvs = v1.vs() + f*(v2.vs() - v1.vs());
		return Velocity.mkTrkGsVs(newtrk,newgs,newvs);
	}


	// This appears to use the right-hand rule to determine it returns the inside or outside angle
	public static double angle_between(Vect2 v1, Vect2 v2) {
		Vect2 VV1 = v1.Scal(1.0/v1.norm());
		Vect2 VV2 = v2.Scal(1.0/v2.norm());
		return Util.atan2_safe(VV2.y,VV2.x)-Util.atan2_safe(VV1.y,VV1.x);
	}

	/**
	 * Return angle between P1-P2 and P2-P3
	 * @param a point 1
	 * @param b point 2 (intersection of two lines)
	 * @param c point 3
	 * @return angle between two lines
	 */	
	public static double angle_between(Vect2 a, Vect2 b, Vect2 c) {
		Vect2 A = b.Sub(a);
		Vect2 B = b.Sub(c);
		return Util.acos_safe(A.dot(B)/(A.norm()*B.norm()));
	}


	/**
	 * determines if divergent and relative speed is greater than a specified minimum relative speed
	 * 
	 * @param s  relative position of ownship with respect to traffic
	 * @param vo initial velocity of ownship
	 * @param vi initial velocity of traffic
	 * @param minRelSpeed  the desired minimum relative speed
	 * @return   true iff divergent and relative speed is greater than a specified minimum relative speed
	 */
	public static boolean  divergentHorizGt(Vect2 s, Vect2 vo, Vect2 vi, double minRelSpeed) {
		Vect2 v = (vo.Sub(vi));
		boolean rtn = s.dot(v) > 0 && v.norm() > minRelSpeed;
		return rtn;
	}

	public static boolean  divergentHorizGt(Vect3 s, Vect3 vo, Vect3 vi, double minRelSpeed) {
		return divergentHorizGt(s.vect2(), vo.vect2(), vi.vect2(), minRelSpeed);
	}

	//	public static boolean  divergent(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
	//		Vect3 s = (so.Sub(si));
	//		Vect3 v = (vo.Sub(vi));
	//		boolean rtn = s.dot(v) > 0;
	//		return rtn;
	//	}


	/**
	 * Return if two aircraft in the given state are divergent in the horizontal plane
	 */
	public static boolean divergent(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
		return so.Sub(si).dot(vo.Sub(vi)) > 0;
	}


	/**
	 * Return if two aircraft in the given state are divergent in a 3D sense
	 */
	public static boolean divergent(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
		return so.Sub(si).dot(vo.Sub(vi)) > 0;
	}

	//	  /**
	//	   * Return the horizontal rate of closure of two aircraft in the given state
	//	   */
	//	  public static double rateOfClosureHorizontal(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
	//		  return -so.Sub(si).vect2().Hat().dot(vo.Sub(vi).vect2());
	//	  }
	//
	//	  /**
	//	   * Return the vertical rate of closure of two aircraft in the given state
	//	   */
	//	  public static double rateOfClosureVertical(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
	//		  return Util.sign(si.z-so.z)*(vo.z-vi.z);
	//	  }


	/** time of closest approach, if parallel return Double.MAX_VALUE
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @return     time of closest approach (can be negative)
	 */
	public static double tau(Vect3 s, Vect3 vo, Vect3 vi) {
		double rtn;
		Vect3 v = vo.Sub(vi);
		double nv = v.norm();
		if (Util.almost_equals(nv,0.0)) {
			rtn = Double.MAX_VALUE;                    // pseudo infinity
		} else
			rtn = -s.dot(v)/(nv*nv);
		return rtn;
	}// tau

	/**  distance at time of closest approach
	 * @param s    relative position of aircraft
	 * @param vo   velocity of ownship
	 * @param vi   velocity of traffic
	 * @param futureOnly  if true then in divergent cases use distance now  
	 * @return     distance at time of closest approach
	 */
	public static double distAtTau(Vect3 s, Vect3 vo, Vect3 vi, boolean futureOnly) {   
		double tau = tau(s,vo,vi);
		if (tau < 0 && futureOnly) 
			return s.norm();                 // return distance now
		else {
			Vect3 v = vo.Sub(vi);
			Vect3 sAtTau = s.Add(v.Scal(tau));
			return sAtTau.norm();
		}
	}


	/** Q function (relative tangent point) 
	 * @param s point outside circle with origin center
	 * @param D radius of circle
	 * @param eps direction (+/-1)
	 * @return point on origin-centered circle that is tangent and passes through s, or INVALID if s is within the circle
	 */
	public static Vect2 Q(Vect2 s, double D, int eps) {
		double sq_s = s.sqv();
		double sq_D = Util.sq(D);
		double delta = sq_s-sq_D;
		if (delta >= 0) { 
			double alpha = sq_D/sq_s;
			double beta  = D*Util.sqrt_safe(delta)/sq_s;
			return new Vect2(alpha*s.x+eps*beta*s.y,
					alpha*s.y-eps*beta*s.x);   
		}
		return Vect2.INVALID;
	}

	/**
	 * Computes 2D intersection point of two lines, but also finds z component (projected by time from line o)
	 * This z-component is constrained to be within the z components of the defining points.
	 * @param so3 starting point of line o
	 * @param vo3 direction vector for line o
	 * @param si3 starting point of line i
	 * @param vi3 direction vector of line i
	 * @return Pair (2-dimensional point of intersection, relative time of intersection, relative to the so3)
	 * Note the intersection may be in the past (i.e. negative time)
	 * If the lines are 2-D parallel, this returns the pair (0,NaN).
	 */
	public static Pair<Vect3,Double> intersection(Vect3 so3, Velocity vo3, Vect3 si3, Velocity vi3) {
		Vect2 so = so3.vect2();
		Vect2 vo = vo3.vect2();
		Vect2 si = si3.vect2();
		Vect2 vi = vi3.vect2();
		Vect2 ds = si.Sub(so);
		if (vo.det(vi) == 0) {
			//f.pln(" $$$ intersection: lines are parallel vo="+vo+" vi="+vi);
			return new Pair<Vect3,Double>(Vect3.ZERO, Double.NaN);
		}
		double tt = ds.det(vi)/vo.det(vi);
		//f.pln(" $$$ intersection: tt = "+tt);
		Vect3 intersec = so3.Add(vo3.Scal(tt));
		double nZ = intersec.z();
		double maxZ = Util.max(so3.z,si3.z);
		double minZ = Util.min(so3.z,si3.z);			
		if (nZ > maxZ) nZ = maxZ;
		if (nZ < minZ) nZ = minZ;	
		//f.pln("intersection nz="+nZ);		
		return new Pair<Vect3,Double>(intersec.mkZ(nZ),tt);
	}

	/**
	 * Intersection of 2 (2D) lines
	 * @param so current position on line 1 
	 * @param vo velocity of so along line 1
	 * @param si current position on line 2
	 * @param vi velocity of si on line 2
	 * @return position and time along line 1, or (0,NaN) if there is no intersection.
	 */
	public static Pair<Vect2,Double> intersection(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
		Vect2 ds = si.Sub(so);
		if (vo.det(vi) == 0) {
			return new Pair<Vect2,Double>(Vect2.ZERO, Double.NaN);
		}
		double tt = ds.det(vi)/vo.det(vi);
		Vect2 intersec = so.Add(vo.Scal(tt));
		return new Pair<Vect2,Double>(intersec,tt);
	}

	// returns intersection point and time of intersection relative to position so1
	// for time return value, it assumes that aircraft travels from so1 to so2 in dto seconds and from si1 to si2 in dti seconds
	// a negative time indicates that the intersection occurred in the past (relative to directions of travel of so1)
	/**
	 * Given two lines defined by so, so2 and si, si2 return the intersection point 
	 * Calculate altitude of intersection using the average of the altitudes of the two closests points to the
	 * intersection.
	 * 
	 * @param so1     first point of line A 
	 * @param so2    second point of line A 
	 * @param dto    the delta time between point so and point so2.
	 * @param si1     first point of line B
	 * @param si2    second point of line B 
	 * @return a pair: intersection point and the delta time from point "so" to the intersection, can be negative if intersect
	 *                 point is in the past
	 *                if intersection point is invalid then the returned delta time is -1
	 */

	public static double distanceH(Vect3 soA, Vect3 soB) {
		return soA.Sub(soB).vect2().norm();
	}


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
	public static Pair<Vect3,Double> intersectionAvgZ(Vect3 so1, Vect3 so2, double dto, Vect3 si1, Vect3 si2) {
		Velocity vo3 = Velocity.genVel(so1, so2, dto); //along line 1
		Velocity vi3 = Velocity.genVel(si1, si2, dto); //along line 2     // its ok to use any time here,  all times are relative to so
		Pair<Vect3,Double> iP = intersection(so1,vo3,si1,vi3); // 2d intersection along line 1 (includes line 1 altitude)
		Vect3 interSec = iP.first;
		double do1 = distanceH(so1,interSec);
		double do2 = distanceH(so2,interSec);
		double alt_o = so1.z();                   // chose z from end point of line 1 closest to interSec
		if (do2 < do1) alt_o = so2.z();
		double di1 = distanceH(si1,interSec);
		double di2 = distanceH(si2,interSec);
		double alt_i = si1.z();                   //  chose z from end point of line 2 closest to interSec
		if (di2 < di1) alt_i = si2.z();
		double nZ = (alt_o + alt_i)/2.0;       
		//f.pln("intersection nz="+nZ);		
		return new Pair<Vect3,Double>(interSec.mkZ(nZ),iP.second); 
	}

	public static Pair<Vect2,Double> intersection(Vect2 so1, Vect2 so2, double dto, Vect2 si1, Vect2 si2) {
		Vect2 vo = so2.Sub(so1).Scal(1/dto);
		Vect2 vi = si2.Sub(si1).Scal(1/dto);
		return intersection(so1,vo,si1,vi);
	}

	/**
	 * returns the perpendicular time and distance between line defined by s,v and point q.
	 * @param  s point on line
	 * @param  v velocity vector of line
	 * @param  q a point not on the line
	 * 
	 * @return time to perpendicular location on line (relative to s) and perpendicular distance
	 */
	public static Pair<Double,Double> distPerp(Vect2 s, Vect2 v, Vect2 q) {
		double tp = q.Sub(s).dot(v)/v.sqv();
		double dist = s.Add(v.Scal(tp)).Sub(q).norm();
		return new Pair<Double,Double>(tp,dist);
	}

	// horizontal only
	public static Pair<Double,Double> distPerp(Vect3 s, Vect3 v, Vect3 q) {
		return distPerp(s.vect2(),v.vect2(),q.vect2());
	}

	/**
	 * Return the closest point along line a-b to point so
	 * EXPERIMENTAL
	 */
	public static Vect3 closestPoint3(Vect3 a, Vect3 b, Vect3 so) {
		// translate a to origin, then project so onto the line defined by ab, then translate back to a
		// this is closest point in 3-space
		Vect3 ab = b.Sub(a);
		return ab.Scal(so.Sub(a).dot(ab)/ab.dot(ab)).Add(a);
	}

	/**
	 * Return the closest (preference to horizontal) point along line a-b to point so
	 * EXPERIMENTAL
	 */
	public static Vect3 closestPoint(Vect3 a, Vect3 b, Vect3 so) {
		if (a.almostEquals(b)) return Vect3.INVALID;
		Vect2 c = closestPoint(a.vect2(), b.vect2(), so.vect2());
		Vect3 v = b.Sub(a);
		double d1 = v.vect2().norm();
		double d2 = c.Sub(a.vect2()).norm();
		double d3 = c.Sub(b.vect2()).norm();
		double f = d2/d1;
		if (d3 > d1 && d3 > d2) { // negative direction
			f = -f; 
		}
		return a.AddScal(f, v);

		//		Vect3 v = a.Sub(b).PerpL().Hat2D(); // perpendicular vector to line
		//		Vect3 s2 = so.AddScal(100, v);
		//		Pair<Vect3, Double> i = intersectionAvgZ(a,b,100,so,s2);
		//		// need to fix altitude to be along the a-b line
		//		return interpolate(a,b,i.second/100.0);
	}

	/**
	 * Return the closest (horizontal) point along line a-b to point so
	 * EXPERIMENTAL
	 */
	public static Vect2 closestPoint(Vect2 a, Vect2 b, Vect2 so) {
		// translate a to origin, then project so onto the line defined by ab, then translate back to a
		Vect2 ab = b.Sub(a);
		return ab.Scal(so.Sub(a).dot(ab)/ab.dot(ab)).Add(a);
		//		if (collinear(a,b,so)) return so;
		//		Vect2 v = a.Sub(b).PerpL().Hat(); // perpendicular vector to line
		//		Vect2 s2 = so.AddScal(100, v);
		//		Vect2 cp = intersection(so,s2,100,a,b).first;
		//		return cp;
	}

	/**
	 * Return the closest (horizontal) point on segment a-b to point so
	 * EXPERIMENTAL
	 */
	public static Vect3 closestPointOnSegment(Vect3 a, Vect3 b, Vect3 so) {
		Vect3 i = closestPoint(a,b,so);
		double d1 = a.distanceH(b);
		double d2 = a.distanceH(i);
		double d3 = b.distanceH(i);
		if (d2 <= d1 && d3 <= d1) {
			return i;
		} else if (d2 < d3) {
			return a;
		} else {
			return b;
		}
	}

	public static Vect2 closestPointOnSegment(Vect2 a, Vect2 b, Vect2 so) {
		Vect2 i = closestPoint(a,b,so);
		// a, b, i are all on line
		double d1 = a.distance(b);
		double d2 = a.distance(i);
		double d3 = b.distance(i);
		if (d2 <= d1 && d3 <= d1) { // i is between a and b
			return i;
		} else if (d2 < d3) {
			return a;
		} else {
			return b;
		}
	}

	public static double distanceToSegment(Vect2 a, Vect2 b, Vect2 so) {
		return so.distance(closestPointOnSegment(a,b,so));
	}

	/**
	 * Computes 2D intersection point of two lines, but also finds z component (projected by time from line 1)
	 * @param s0 starting point of line 1
	 * @param v0 direction vector for line 1
	 * @param s1 starting point of line 2
	 * @param v1 direction vector of line 2
	 * @return time the OWNSHIP (so3) will reach the point.  Note that the intruder (si3) may have already passed this point.
	 * If the lines are parallel, this returns NaN.
	 */
	public static double  timeOfIntersection(Vect3 so3, Velocity vo3, Vect3 si3, Velocity vi3) {
		Vect2 so = so3.vect2();
		Vect2 vo = vo3.vect2();
		Vect2 si = si3.vect2();
		Vect2 vi = vi3.vect2();
		Vect2 ds = si.Sub(so);
		if (vo.det(vi) == 0) {
			//f.pln(" $$$ intersection: lines are parallel");
			return Double.NaN;
		}
		double tt = ds.det(vi)/vo.det(vi);
		//f.pln(" $$$ intersection: tt = "+tt);
		return tt;
	}


	/**
	 * Rotate p around origin.
	 * @param p point to move
	 * @param angle angle of rotation (positive is clockwise)
	 * @return new position for p
	 */
	public static Vect2 rotate(Vect2 p, double angle) {
		double x1 = Math.cos(angle)*p.x + Math.sin(angle)*p.y;
		double y1 = -Math.sin(angle)*p.x + Math.cos(angle)*p.y;
		return new Vect2(x1,y1);
		//double[][] r2 = {{Math.cos(angle),Math.sin(angle)},{-Math.sin(angle),Math.cos(angle)}};
		//Matrix2d r = new Matrix2d(r2);
		//Matrix2d p2 = new Matrix2d(p.Sub(origin));
		//Vect2 p3 = r.mult(p2).vect2();
		//return p3.Add(origin);
	}

	/**
	 * Return the distance at which si, traveling in direction vi, will intersect with segment AB (inclusive)
	 * @param si
	 * @param vi
	 * @param a
	 * @param b
	 * @return distance of intersection, or negative if none (
	 */
	public static double intersectSegment(Vect2 si, Vect2 vi, Vect2 a, Vect2 b) {
		double theta = vi.trk();
		// rotate segment so vi is "straight up" and si is at the origin
		Vect2 A = rotate(a.Sub(si), -theta);
		Vect2 B = rotate(b.Sub(si), -theta);
		if ((A.x >= 0 && B.x <= 0) || (A.x <= 0 && B.x >= 0)) {
			if (A.x == B.x) {
				if (A.y >= 0 || B.y >= 0) {
					return Util.max(0.0, Util.min(A.y, B.y)); // first point of intersection
				}
			} else if (A.y == B.y) {
				if (A.y >= 0) {
					return A.y;
				}
			} else if (A.y >= si.y || B.y >= si.y) {
				double m = (B.x-A.x)/(B.y-A.y);
				double y0 = A.y-m*A.x;
				if (y0 >= 0) {
					return y0;
				}
			}
		}
		return -1.0;
	}


	/**
	 * Returns true if x is "behind" so , that is, x is within the region behind the perpendicular line to vo through so.  
	 */
	public static boolean behind(Vect2 x, Vect2 so, Vect2 vo) {
		return Util.turnDelta(vo.trk(), x.Sub(so).trk()) > Math.PI/2.0;
	}

	/**
	 * Returns values indicating whether the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
	 * @param so ownship position
	 * @param vo ownship velocity
	 * @param si intruder position
	 * @param vi intruder velocity
	 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if divergent or parallel
	 */
	public static int passingDirection(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
		double toi = timeOfIntersection(so,vo,si,vi);
		double tii = timeOfIntersection(si,vi,so,vo); // these values may have opposite sign!
		//f.pln("toi="+toi);
		//f.pln("int = "+	intersection(so,vo,si,vi));
		if (Double.isNaN(toi) || toi < 0 || tii < 0) return 0;
		Vect3 so3 = so.linear(vo, toi);
		Vect3 si3 = si.linear(vi, toi);
		//f.pln("so3="+so3);		
		//f.pln("si3="+si3);		
		if (behind(so3.vect2(), si3.vect2(), vi.vect2())) return -1;
		return 1;
	}


	//	public static int dirForBehind(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
	//		if (divergent(so,vo,si,vi)) return 0;				
	//		double sdetvi = so.Sub(si).det(vi);
	//		double toi = 0.0;
	//		if (sdetvi != 0.0) toi = -vo.det(vi)/sdetvi;
	//		Vect2 nso = so.AddScal(toi,vo);
	//		Vect2 nsi = si.AddScal(toi,vi);
	//		int ahead = Util.sign(nso.Sub(nsi).dot(vi)); // Are we ahead of intruder at crossing pt
	//		int onRight = Util.sign(nsi.Sub(nso).det(vo)); // Are we ahead of intruder at crossing pt
	//		return ahead*onRight;
	//	}

	public static int dirForBehind(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
		if (divergent(so,vo,si,vi)) return 0;
		return (rightOfLine(si, vi, so) ? -1 : 1);
	}

	public static int dirForBehind(Vect3 so, Velocity vo, Vect3 si, Velocity vi) {
		return dirForBehind(so.vect2(),vo.vect2(),si.vect2(),vi.vect2());	
	}



	//	public String strPair(Pair<Vect3,Velocity> pp) {
	//		return " "+pp.first.toStringNP()+" "+pp.second.toStringNP();
	//	}



	/**
	 * Calculate the normal (perpendicular vector) of a plane defined by 3 points.  This is not necessarily a unit vector.
	 * @param a point 1
	 * @param b point 2 
	 * @param c point 3
	 * @return
	 */
	public static Vect3 planeNormal(Vect3 a, Vect3 b, Vect3 c) {
		Vect3 ab = a.Sub(b);
		Vect3 ac = a.Sub(c);
		return ab.cross(ac);

	}

	/**
	 * Return the side (+1 right, -1 left) of the ray s(point),v(velocity) that point p is on.
	 * If value is zero, p is on the line (given no floating point issues)
	 */
	public static int onSide(Vect2 s, Vect2 v, Vect2 p) {
		return Util.sign(p.Sub(s).det(v));
	}


	/**
	 * Return the interior point where the lines between two circles' tangents intersect (from Wolfram Mathworld).
	 * A line tangent to both circles will pass through this point
	 * @param c1 center of circle 1
	 * @param r1 radius of circle 1
	 * @param c2 center of circle 2
	 * @param r2 radius of circle 2
	 * @return interior center of similitude.  This version will test if the circles overlap and return INVALID in that case. 
	 */
	public static Vect2 internalCenterOfSimilitude(Vect2 c1, double r1, Vect2 c2, double r2) {
		if (c1.distance(c2) < r1+r2) return Vect2.INVALID;
		return c1.Scal(r1).Add(c2.Scal(r2)).Scal(1/(r1+r2));
	}

	/**
	 * Return the interior point where the lines between two circles' tangents intersect, if they have different radii (from Wolfram Mathworld)
	 * A line tangent to both circles will pass through this point
	 * @param c1 center of circle 1
	 * @param r1 radius of circle 1
	 * @param c2 center of circle 2
	 * @param r2 radius of circle 2
	 * @return exterior center of similitude.  This version will test if the circles overlap and return INVALID in that case.  It will also return INVALID if they have the same radii. 
	 */
	public static Vect2 externalCenterOfSimilitude(Vect2 c1, double r1, Vect2 c2, double r2) {
		if (Util.almost_equals(r1,  r2) || c1.distance(c2) < r1+r2) return Vect2.INVALID;
		return c2.Scal(r1).Sub(c1.Scal(r2)).Scal(1/(r2-r1));
	}

	/**
	 * Return a list of segments that are tangent to two circles
	 * @param c1 center of circle 1
	 * @param r1 radius of circle 1
	 * @param c2 center of circle 2
	 * @param r2 radius of circle 2
	 * @return endpoints of segments (from c1 to c2) tangent to both circles.  This list will be empty if the circles overlap.
	 */
	public static ArrayList<Pair<Vect2,Vect2>> tangentSegments(Vect2 c1, double r1, Vect2 c2, double r2) {
		ArrayList<Pair<Vect2,Vect2>> ret = new ArrayList<Pair<Vect2,Vect2>>();
		Vect2 icos = internalCenterOfSimilitude(c1,r1,c2,r2);
		if (icos.isInvalid()) return ret; // overlaps, nothing to do
		Vect2 ecos = externalCenterOfSimilitude(c1,r1,c2,r2);
		if (Util.almost_equals(r1, r2)) { // parallel
			Vect2 t1 = c1.AddScal(r1, c1.Sub(c2).PerpL().Hat());
			Vect2 t2 = c2.AddScal(r2, c1.Sub(c2).PerpL().Hat());
			Vect2 t3 = c1.AddScal(r1, c1.Sub(c2).PerpR().Hat());
			Vect2 t4 = c2.AddScal(r2, c1.Sub(c2).PerpR().Hat());
			ret.add(Pair.make(t1, t2));
			ret.add(Pair.make(t3, t4));
		} else { // converging external
			Vect2 s1 = ecos.Sub(c1);
			Vect2 s2 = ecos.Sub(c2);
			Vect2 t1 = Q(s1, r1, +1).Add(c1);
			Vect2 t2 = Q(s1, r1, -1).Add(c1);
			Vect2 t3 = Q(s2, r2, +1).Add(c2);
			Vect2 t4 = Q(s2, r2, -1).Add(c2);
			ret.add(Pair.make(t1, t3));
			ret.add(Pair.make(t2, t4));
		}
		// internal
		Vect2 s1 = icos.Sub(c1);
		Vect2 s2 = icos.Sub(c2);
		Vect2 t1 = Q(s1, r1, +1).Add(c1);
		Vect2 t2 = Q(s1, r1, -1).Add(c1);
		Vect2 t3 = Q(s2, r2, +1).Add(c2);
		Vect2 t4 = Q(s2, r2, -1).Add(c2);
		ret.add(Pair.make(t1, t3));
		ret.add(Pair.make(t2, t4));
		return ret;
	}

}
