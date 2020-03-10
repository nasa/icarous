/*
 * VectFuns.cpp
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Util.h" // NaN def
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "VectFuns.h"
#include "Constants.h"
#include "format.h"
#include <cmath>
#include <float.h>


namespace larcfm {


bool VectFuns::LoS(const Vect3& so, const Vect3& si, double D, double H) {
	Vect3 s = so.Sub(si);
	return s.x*s.x + s.y*s.y < D*D && std::abs(s.z) < H;
}

bool VectFuns::rightOfLine(const Vect2& so, const Vect2& vo, const Vect2& si) {
	return si.Sub(so).dot(vo.PerpR()) >= 0;
}

int VectFuns::rightOfLinePoints(const Vect2& a, const Vect2& b, const Vect2& p) {
	Vect2 AB = b.Sub(a);
	Vect2 AP = p.Sub(a);
	//The calculation below is the 2-D cross product.
	return Util::sign(AP.x*AB.y - AP.y*AB.x);
}

bool VectFuns::collinear(const Vect3& p0, const Vect3& p1, const Vect3& p2) {
	Vect3 v01 = p0.Sub(p1);
	Vect3 v02 = p1.Sub(p2);
	return v01.parallel(v02);
}

bool VectFuns::collinear(const Vect2& p0, const Vect2& p1, const Vect2& p2) {
	// area of triangle = 0? (same as det of sides = 0?)
	return  p1.Sub(p0).det(p2.Sub(p0)) == 0;
}

Vect2 VectFuns::midPoint(const Vect2& p0, const Vect2& p1) {
	return p0.Add(p1).Scal(0.5);
}

Vect3 VectFuns::midPoint(const Vect3& p0, const Vect3& p1) {
	return p0.Add(p1).Scal(0.5);
}

// f should be between 0 and 1 to interpolate
Vect3 VectFuns::interpolate(const Vect3& v1, const Vect3& v2, double f) {
	Vect3 dv = v2.Sub(v1);
	return v1.Add(dv.Scal(f));
}

// f should be between 0 and 1 to interpolate
Velocity VectFuns::interpolateVelocity(const Velocity& v1, const Velocity& v2, double f) {
        double newtrk = v1.trk() + f*(v2.trk() - v1.trk());
        double newgs = v1.gs() + f*(v2.gs() - v1.gs());
        double newvs = v1.vs() + f*(v2.vs() - v1.vs());
        return Velocity::mkTrkGsVs(newtrk,newgs,newvs);
}


// This appears to use the right-hand rule to determine it returns the inside or outside angle
double VectFuns::angle_between(const Vect2& v1, const Vect2& v2) {
	Vect2 VV1 = v1.Scal(1.0/v1.norm());
	Vect2 VV2 = v2.Scal(1.0/v2.norm());
	return Util::atan2_safe(VV2.y,VV2.x)-Util::atan2_safe(VV1.y,VV1.x);
}

double VectFuns::angle_between(const Vect2& a, const Vect2& b, const Vect2& c) {
	Vect2 A = b.Sub(a);
	Vect2 B = b.Sub(c);
	double d = (A.norm()*B.norm());
	if (d == 0.0) return M_PI;
	return Util::acos_safe(A.dot(B)/d);
}



bool VectFuns::divergentHorizGt(const Vect2& s, const Vect2& vo, const Vect2& vi, double minRelSpeed) {
	Vect2 v = vo.Sub(vi);
	bool rtn = s.dot(v) > 0 && v.norm() > minRelSpeed;
	return rtn;
}

bool VectFuns::divergentHorizGt(const Vect3& s, const Vect3& vo, const Vect3& vi, double minRelSpeed) {
   return divergentHorizGt(s.vect2(), vo.vect2(), vi.vect2(), minRelSpeed);
}

bool VectFuns::divergent(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi) {
	  return so.Sub(si).dot(vo.Sub(vi)) > 0;
}

bool VectFuns::divergent(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
	  return so.Sub(si).dot(vo.Sub(vi)) > 0;
}

double VectFuns::rateOfClosureHorizontal(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
	return -so.Sub(si).vect2().Hat().dot(vo.Sub(vi).vect2());
}

double VectFuns::rateOfClosureVertical(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
	return Util::sign(si.z-so.z)*(vo.z-vi.z);
}

// time of closest approach
double VectFuns::tau(const Vect3& s, const Vect3& vo, const Vect3& vi) {
	double rtn;
	Vect3 v = vo.Sub(vi);
	double nv = v.norm();
	if (Util::almost_equals(nv,0.0)) {
		rtn = std::numeric_limits<double>::max();        // pseudo infinity
	} else
		rtn = -s.dot(v)/(nv*nv);
	return rtn;
}// tau

double VectFuns::distAtTau(const Vect3& s, const Vect3& vo, const Vect3& vi, bool futureOnly) {
	double tau = VectFuns::tau(s,vo,vi);
	if (tau < 0 && futureOnly)
		return s.norm();                 // return distance now
	else {
		Vect3 v = vo.Sub(vi);
		Vect3 sAtTau = s.Add(v.Scal(tau));
		return sAtTau.norm();
	}
}

/**
 * Computes 2D intersection point of two lines, but also finds z component (projected by time from line 1)
 * @param s0 starting point of line 1
 * @param v0 direction vector for line 1
 * @param s1 starting point of line 2
 * @param v1 direction vector of line 2
 * @return Pair (2-dimensional point of intersection with 3D projection, relative time of intersection, relative to the so3)
 * If the lines are parallel, this returns the pair (0,NaN).
 */
std::pair<Vect3,double> VectFuns::intersection(const Vect3& so3, const Velocity& vo3, const Vect3& si3, const Velocity& vi3) {
	Vect2 so = so3.vect2();
	Vect2 vo = vo3.vect2();
	Vect2 si = si3.vect2();
	Vect2 vi = vi3.vect2();
	Vect2 ds = si.Sub(so);
	if (vo.det(vi) == 0) {
		//f.pln(" $$$ intersection: lines are parallel");
		return std::pair<Vect3,double>(Vect3::ZERO(),  NaN);
	}
	double tt = ds.det(vi)/vo.det(vi);
	//f.pln(" $$$ intersection: tt = "+tt);
	Vect3 intersec = so3.Add(vo3.Scal(tt));
	double nZ = intersec.z;
	double maxZ = Util::max(so3.z,si3.z);
	double minZ = Util::min(so3.z,si3.z);
	if (nZ > maxZ) nZ = maxZ;
	if (nZ < minZ) nZ = minZ;
	return std::pair<Vect3,double>(intersec.mkZ(nZ),tt);
}

std::pair<Vect2,double> VectFuns::intersection2D(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi) {
	Vect2 ds = si.Sub(so);
	if (vo.det(vi) == 0) {
		//f.pln(" $$$ intersection: lines are parallel");
		return std::pair<Vect2,double>(Vect2::ZERO(),  NaN);
	}
	double tt = ds.det(vi)/vo.det(vi);
	Vect2 intersec = so.Add(vo.Scal(tt));
	return std::pair<Vect2,double>(intersec,tt);
}


double VectFuns::distanceH(const Vect3& soA, const Vect3& soB) {
	return soA.Sub(soB).vect2().norm();
}

std::pair<Vect3,double> VectFuns::intersectionAvgZ(const Vect3& so1, const Vect3& so2, double dto, const Vect3& si1, const Vect3& si2) {
	Velocity vo3 = Velocity::genVel(so1, so2, dto);
	Velocity vi3 = Velocity::genVel(si1, si2, dto);      // its ok to use any time here,  all times are relative to so
	std::pair<Vect3,double> iP = intersection(so1,vo3,si1,vi3);
	Vect3 interSec = iP.first;
			double do1 = distanceH(so1,interSec);
			double do2 = distanceH(so2,interSec);
			double alt_o = so1.z;
			if (do2 < do1) alt_o = so2.z;
			double di1 = distanceH(si1,interSec);
			double di2 = distanceH(si2,interSec);
			double alt_i = si1.z;
			if (di2 < di1) alt_i = si2.z;
			double nZ = (alt_o + alt_i)/2.0;
	        return std::pair<Vect3,double>(interSec.mkZ(nZ),iP.second);
}

std::pair<Vect2,double> VectFuns::intersection2D(const Vect2& so1, const Vect2& so2, double dto, const Vect2& si1, const Vect2& si2) {
	Vect2 vo = so2.Sub(so1).Scal(1/dto);
	Vect2 vi = si2.Sub(si1).Scal(1/dto);      // its ok to use any time here,  all times are relative to so
	return intersection2D(so1,vo,si1,vi);
}


std::pair<Vect2,double> VectFuns::intersectSegments(const Vect2& so, const Vect2& so2, const Vect2& si, const Vect2& si2) {
	Vect2 vo = so2.Sub(so);
	Vect2 vi = si2.Sub(si);
	std::pair<Vect2,double> int2D = intersection2D(so,vo,si,vi);
	double fractDist = int2D.second;
	if (int2D.first.isInvalid()) return int2D;
	if (fractDist < 0 || fractDist > 1.0) return std::pair<Vect2,double>(Vect2::INVALID(), fractDist);
	Vect2 w = so.Sub(si);
	double D = vo.det(vi);
	double tI = vo.det(w)/D;
	if (tI < 0 || tI > 1) return std::pair<Vect2,double>(Vect2::INVALID(), tI);
	return int2D;
}

Vect3 VectFuns::closestPoint3(const Vect3& a, const Vect3& b, const Vect3& so) {
	Vect3 ab = b.Sub(a);
	return ab.Scal(so.Sub(a).dot(ab)/ab.dot(ab)).Add(a);
}


Vect3 VectFuns::closestPoint(const Vect3& a, const Vect3& b, const Vect3& so) {
	if (a.almostEquals(b)) return Vect3::INVALID();
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


//	Vect3 v = a.Sub(b).PerpL().Hat2D(); // perpendicular vector to line
//	Vect3 s2 = so.AddScal(100, v);
//	std::pair<Vect3, double> i = intersectionAvgZ(a,b,100,so,s2);
//	// need to fix altitude to be along the a-b line
//	return interpolate(a,b,i.second/100.0);
}

Vect2 VectFuns::closestPoint(const Vect2& a, const Vect2& b, const Vect2& so) {
	// translate a to origin, then project so onto the line defined by ab, then translate back to a
	Vect2 ab = b.Sub(a);
	return ab.Scal(so.Sub(a).dot(ab)/ab.dot(ab)).Add(a);
//	if (collinear(a,b,so)) return so;
//	Vect2 v = a.Sub(b).PerpL().Hat(); // perpendicular vector to line
//	Vect2 s2 = so.AddScal(100, v);
//	Vect2 cp = intersection(so,s2,100,a,b).first;
//	return cp;
}


Vect3 VectFuns::closestPointOnSegment(const Vect3& a, const Vect3& b, const Vect3& so) {
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

Vect2 VectFuns::closestPointOnSegment(const Vect2& a, const Vect2& b, const Vect2& so) {
	Vect2 i = closestPoint(a,b,so);
	double d1 = a.distance(b);
	double d2 = a.distance(i);
	double d3 = b.distance(i);
	if (d2 <= d1 && d3 <= d1) {
		return i;
	} else if (d2 < d3) {
		return a;
	} else {
		return b;
	}
}

Vect3 VectFuns::closestPointOnSegment3(const Vect3& a, const Vect3& b, const Vect3& so) {
	Vect3 i = closestPoint3(a,b,so);
	double d1 = a.Sub(b).norm();
	double d2 = a.Sub(i).norm();
	double d3 = b.Sub(i).norm();
	if (d2 <= d1 && d3 <= d1) {
		return i;
	} else if (d2 < d3) {
		return a;
	} else {
		return b;
	}
}

std::pair<Vect3,double> VectFuns::closestPointOnSegment3_extended(const Vect3& a, const Vect3& b, const Vect3& so) {
	Vect3 i = closestPoint3(a,b,so);
	if (a.almostEquals(b)) return std::pair<Vect3,double>(a,  0.0);
	double d1 = a.Sub(b).norm();
	double d2 = a.Sub(i).norm();
	double d3 = b.Sub(i).norm();
	if (d2 <= d1 && d3 <= d1) {
		double r = d2/d1;
		return std::pair<Vect3,double>(i, r);
	} else if (d2 < d3) {
		return std::pair<Vect3,double>(a, 0.0);
	} else {
		return std::pair<Vect3,double>(b, 1.0);
	}
}



/**
 * Computes 2D intersection point of two lines, but also finds z component (projected by time from line 1)
 * @param s0 starting point of line 1
 * @param v0 direction vector for line 1
 * @param s1 starting point of line 2
 * @param v1 direction vector of line 2
 * @return Pair (2-dimensional point of intersection with 3D projection, relative time of intersection, relative to the so3)
 * If the lines are parallel, this returns the pair (0,NaN).
 */
double  VectFuns::timeOfIntersection(const Vect3& so3, const Velocity& vo3, const Vect3& si3, const Velocity& vi3) {
	Vect2 so = so3.vect2();
	Vect2 vo = vo3.vect2();
	Vect2 si = si3.vect2();
	Vect2 vi = vi3.vect2();
	Vect2 ds = si.Sub(so);
	if (vo.det(vi) == 0) {
		//f.pln(" $$$ intersection: lines are parallel");
		return NaN;
	}
	double tt = ds.det(vi)/vo.det(vi);
	//f.pln(" $$$ intersection: tt = "+tt);
	return tt;
}


/**
 * Returns true if x is "behind" so , that is, x is within the region behind the perpendicular line to vo through so.
 */
bool VectFuns::behind(const Vect2& x, const Vect2& so, const Vect2& vo) {
	return Util::turnDelta(vo.trk(), x.Sub(so).trk()) > M_PI/2.0;
}

/**
 * Returns values indicating whether the ownship state will pass in front of or behind the intruder (from a horizontal perspective)
 * @param so ownship position
 * @param vo ownship velocity
 * @param si intruder position
 * @param vi intruder velocity
 * @return 1 if ownship will pass in front (or collide, from a horizontal sense), -1 if ownship will pass behind, 0 if divergent or parallel
 */
int VectFuns::passingDirection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
	double toi = timeOfIntersection(so,vo,si,vi);
	double tii = timeOfIntersection(si,vi,so,vo); // these values may have opposite sign!
//fpln("toi="+toi);
//fpln("int = "+	intersection(so,vo,si,vi));
	if (ISNAN(toi) || toi < 0 || tii < 0) return 0;
	Vect3 so3 = so.linear(vo, toi);
	Vect3 si3 = si.linear(vi, toi);
//fpln("so3="+so3);
//fpln("si3="+si3);
	if (behind(so3.vect2(), si3.vect2(), vi.vect2())) return -1;
	return 1;
}


//	static int dirForBehind(Vect2 so, Vect2 vo, Vect2 si, Vect2 vi) {
//		if (divergent(so,vo,si,vi)) return 0;
//		double sdetvi = so.Sub(si).det(vi);
//		double toi = 0.0;
//		if (sdetvi != 0.0) toi = -vo.det(vi)/sdetvi;
//		Vect2 nso = so.AddScal(toi,vo);
//		Vect2 nsi = si.AddScal(toi,vi);
//		int ahead = Util::sign(nso.Sub(nsi).dot(vi)); // Are we ahead of intruder at crossing pt
//		int onRight = Util::sign(nsi.Sub(nso).det(vo)); // Are we ahead of intruder at crossing pt
//		return ahead*onRight;
//	}

int VectFuns::dirForBehind(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi) {
	if (divergent(so,vo,si,vi)) return 0;
	return (rightOfLine(si, vi, so) ? -1 : 1);
}

int VectFuns::dirForBehind(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
     return dirForBehind(so.vect2(),vo.vect2(),si.vect2(),vi.vect2());
}





}
