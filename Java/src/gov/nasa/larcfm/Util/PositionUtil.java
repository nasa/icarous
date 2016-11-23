/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

/**
 * GreatCircle and VectFuns functions lifted to Positions
 */
public final class PositionUtil {
	
	public static boolean collinear(Position p0, Position p1, Position p2) {
		if (p0.isLatLon()) {
			return GreatCircle.collinear(p0.lla(), p1.lla(), p2.lla());
		} else {
			return VectFuns.collinear(p0.vect2(), p1.vect2(), p2.vect2());
		}
	}
	
	// f should be between 0 and 1 to interpolate
	public static Position interpolate(Position v1, Position v2, double f) {
		if (v1.isLatLon()) {
			return new Position(GreatCircle.interpolate(v1.lla(), v2.lla(), f));
		} else {
			return new Position(VectFuns.interpolate(v1.point(), v2.point(), f));
		}
	}
	

	/**
	 * Return angle between P1-P2 and P2-P3
	 * @param p1 point 1
	 * @param p2 point 2 (intersection of two lines)
	 * @param p3 point 3
	 * @return angle between two lines
	 */
	public static double angle_between(Position p1, Position p2, Position p3) {
		if (p1.isLatLon()) {
			return GreatCircle.angle_between(p1.lla(), p2.lla(), p3.lla());
		} else {
			return VectFuns.angle_between(p1.vect2(), p2.vect2(), p3.vect2());
		}
	}
	

	public static Pair<Position,Double> intersection(Position so, Velocity vo, Position si, Velocity vi) {
		if (so.isLatLon()) {
			Pair<LatLonAlt, Double> p = GreatCircle.intersection(so.lla(), vo, si.lla(), vi, true);
			return Pair.make(new Position(p.first), p.second);
		} else {
			Pair<Vect3, Double> p = VectFuns.intersection(so.point(), vo, si.point(), vi);
			return Pair.make(new Position(p.first), p.second);
		}
	}


	public static Pair<Position,Double> intersection(Position so1, Position so2, double dto, Position si1, Position si2) {
		if (so1.isLatLon()) {
			Pair<LatLonAlt, Double> p = GreatCircle.intersectionAvgAlt(so1.lla(), so2.lla(), dto, si1.lla(), si2.lla());
			return Pair.make(new Position(p.first), p.second);
		} else {
			Pair<Vect3, Double> p = VectFuns.intersectionAvgZ(so1.point(), so2.point(), dto, si1.point(), si2.point());
			return Pair.make(new Position(p.first), p.second);
		}
	}
	
	public static Position closestPoint(Position a, Position b, Position x) {
		if (a.isLatLon()) {
			return new Position(GreatCircle.closest_point_circle(a.lla(), b.lla(), x.lla()));
		} else {
			return new Position(VectFuns.closestPoint(a.point(), b.point(), x.point()));
		}
	}
	
	
	public static Position closestPointOnSegment(Position a, Position b, Position x) {
		if (a.isLatLon()) {
			return new Position(GreatCircle.closest_point_segment(a.lla(), b.lla(), x.lla()));
		} else {
			return new Position(VectFuns.closestPointOnSegment(a.point(), b.point(), x.point()));
		}
	}

	public static Position behind(Position a, Position b, Position x) {
		if (a.isLatLon()) {
			return new Position(GreatCircle.closest_point_segment(a.lla(), b.lla(), x.lla()));
		} else {
			return new Position(VectFuns.closestPointOnSegment(a.point(), b.point(), x.point()));
		}
	}


	public static boolean behind(Position p1, Position p2, Velocity vo) {
		if (p1.isLatLon()) {
			return GreatCircle.behind(p1.lla(), p2.lla(), vo);
		} else {
			return VectFuns.behind(p1.vect2(), p2.vect2(), vo.vect2());
		}
	}
	
	public static int passingDirection(Position so, Velocity vo, Position si, Velocity vi) {
		if (so.isLatLon()) {
			return GreatCircle.passingDirection(so.lla(), vo, si.lla(), vi);
		} else {
			return VectFuns.passingDirection(so.point(), vo, si.point(), vi);
		}
		
	}
	


	public static int dirForBehind(Position so, Velocity vo, Position si, Velocity vi) {
		if (so.isLatLon()) {
			return GreatCircle.dirForBehind(so.lla(), vo, si.lla(), vi);
		} else {
			return VectFuns.dirForBehind(so.point(),vo,si.point(),vi);
		}
	}
	

}
