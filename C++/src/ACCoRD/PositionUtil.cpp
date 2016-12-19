/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "PositionUtil.h"
#include "Position.h"
#include "Velocity.h"
#include "Vect3.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "VectFuns.h"

namespace larcfm {

	bool PositionUtil::collinear(const Position & p0, const Position & p1, const Position & p2) {
		if (p0.isLatLon()) {
			return GreatCircle::collinear(p0.lla(), p1.lla(), p2.lla());
		} else {
			return VectFuns::collinear(p0.vect2(), p1.vect2(), p2.vect2());
		}
	}
	
	// f should be between 0 and 1 to interpolate
	Position PositionUtil::interpolate(const Position & v1, const Position & v2, double f) {
		if (v1.isLatLon()) {
			return  Position(GreatCircle::interpolate(v1.lla(), v2.lla(), f));
		} else {
			return  Position(VectFuns::interpolate(v1.point(), v2.point(), f));
		}
	}
	

	/**
	 * Return angle between P1-P2 and P2-P3
	 * @param p1 point 1
	 * @param p2 point 2 (intersection of two lines)
	 * @param p3 point 3
	 * @return angle between two lines
	 */
	double PositionUtil::angle_between(const Position & p1, const Position & p2, const Position & p3) {
		if (p1.isLatLon()) {
			return GreatCircle::angle_between(p1.lla(), p2.lla(), p3.lla());
		} else {
			return VectFuns::angle_between(p1.vect2(), p2.vect2(), p3.vect2());
		}
	}
	

	std::pair<Position,double> PositionUtil::intersection(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi) {
		if (so.isLatLon()) {
			std::pair<LatLonAlt, double> p = GreatCircle::intersection(so.lla(), vo, si.lla(), vi, true);
			return std::pair<Position,double>( Position(p.first), p.second);
		} else {
			std::pair<Vect3, double> p = VectFuns::intersection(so.point(), vo, si.point(), vi);
			return std::pair<Position,double>( Position(p.first), p.second);
		}
	}


	std::pair<Position,double> PositionUtil::intersection(const Position & so1, const Position & so2, double dto, const Position & si1, const Position & si2) {
		if (so1.isLatLon()) {
			std::pair<LatLonAlt, double> p = GreatCircle::intersectionAvgAlt(so1.lla(), so2.lla(), dto, si1.lla(), si2.lla());
			return std::pair<Position,double>( Position(p.first), p.second);
		} else {
			std::pair<Vect3, double> p = VectFuns::intersectionAvgZ(so1.point(), so2.point(), dto, si1.point(), si2.point());
			return std::pair<Position,double>( Position(p.first), p.second);
		}
	}
	
	Position PositionUtil::closestPoint(const Position & a, const Position & b, const Position & x) {
		if (a.isLatLon()) {
			return  Position(GreatCircle::closest_point_circle(a.lla(), b.lla(), x.lla()));
		} else {
			return  Position(VectFuns::closestPoint(a.point(), b.point(), x.point()));
		}
	}
	
	
	Position PositionUtil::closestPointOnSegment(const Position & a, const Position & b, const Position & x) {
		if (a.isLatLon()) {
			return  Position(GreatCircle::closest_point_segment(a.lla(), b.lla(), x.lla()));
		} else {
			return  Position(VectFuns::closestPointOnSegment(a.point(), b.point(), x.point()));
		}
	}

	Position PositionUtil::behind(const Position & a, const Position & b, const Position & x) {
		if (a.isLatLon()) {
			return  Position(GreatCircle::closest_point_segment(a.lla(), b.lla(), x.lla()));
		} else {
			return  Position(VectFuns::closestPointOnSegment(a.point(), b.point(), x.point()));
		}
	}


	bool PositionUtil::behind(const Position & p1, const Position & p2, const Velocity & vo) {
		if (p1.isLatLon()) {
			return GreatCircle::behind(p1.lla(), p2.lla(), vo);
		} else {
			return VectFuns::behind(p1.vect2(), p2.vect2(), vo.vect2());
		}
	}
	
	int PositionUtil::passingDirection(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi) {
		if (so.isLatLon()) {
			return GreatCircle::passingDirection(so.lla(), vo, si.lla(), vi);
		} else {
			return VectFuns::passingDirection(so.point(), vo, si.point(), vi);
		}
		
	}
	


	int PositionUtil::dirForBehind(const Position & so, const Velocity & vo, const Position & si, const Velocity & vi) {
		if (so.isLatLon()) {
			return GreatCircle::dirForBehind(so.lla(), vo, si.lla(), vi);
		} else {
			return VectFuns::dirForBehind(so.point(),vo,si.point(),vi);
		}
	}
	

}//namespace
