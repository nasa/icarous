/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "PolyUtil.h"
#include "SimplePoly.h"
#include "PolyPath.h"
#include "Velocity.h"
#include "Position.h"
#include "Plan.h"
#include "PlanUtil.h"
#include <vector>
#include <string>
#include <cmath>
#include <algorithm>
#include <limits>
#include <float.h>

namespace larcfm {

using std::vector;
using std::string;
using std::endl;





SimplePoly PolyUtil::convexHull(const std::vector<Position>& plist, double bottom, double top) {
	std::vector<Triple<Position,int,double> > elems = std::vector<Triple<Position,int,double> >(); // vertex position, vertex index in original polygon, angle from origin point
	int origin = 0;
	// pick origin point
	for (int i = 1; i < (int) plist.size(); i++) {
		Position po = plist[origin];
		Position pi = plist[i];
		if (pi.y() > po.y() || (pi.y() == po.y() && pi.isWest(po))) {
			origin = i;
		}
	}
	// order all other points relative to origin
	for (int i = 0; i < (int) plist.size(); i++) {
		if (i != origin) {
			Position po = plist[origin];
			Position pi = plist[i];
			double val = Util::to_2pi(po.initialVelocity(pi,100).trk());
			elems.push_back(Triple<Position,int,double>(pi, i, val));
		}
	}
	std::sort(elems.begin(), elems.end(), SIComparator());
	elems.insert(elems.begin(),Triple<Position,int,double>(plist[origin],origin,std::numeric_limits<double>::infinity()));
	// delete any points that have a left-hand turn
	int i = 1;
	while (i < (int) elems.size()) {
		int j = i+1;
		if (j == (int) elems.size()) j = 0; // last point
		Position a = elems[i-1].first;
		Position b = elems[i].first;
		Position c = elems[j].first;
		double trk1 = a.finalVelocity(b, 100).trk();
		double trk2 = b.initialVelocity(c, 100).trk();
		if (a.almostEquals(b) || b.almostEquals(c) || Util::turnDir(trk1, trk2) <= 0) {
			elems.erase(elems.begin()+i);
			if (i > 1) i--; // backtrack 1
		} else {
			i++;
		}
	}
	SimplePoly p2 = SimplePoly(bottom, top);
	for (i = 0; i < (int) elems.size(); i++) {
		p2.addVertex(elems[i].first);
	}
	return p2;
}

SimplePoly PolyUtil::convexHull(const SimplePoly& p) {
	return convexHull(p.getVertices(), p.getBottom(), p.getTop());
}


SimplePoly PolyUtil::convexHull(const std::vector<SimplePoly>& p) {
	std::vector<Position> ps = std::vector<Position>();
	double t = -DBL_MAX;
	double b = DBL_MAX;
	for (int i = 0; i < (int) p.size(); i++) {
		std::vector<Position> pts = p[i].getVertices();
		ps.insert(ps.end(), pts.begin(), pts.end());
		t = Util::max(t, p[i].getTop());
		b = Util::min(b, p[i].getBottom());
	}
	return convexHull(ps, b, t);
}


SimplePoly PolyUtil::convexHull(const SimplePoly& p, double buffer) {
	std::vector<Position> ps = std::vector<Position>();
	for (int i = 0; i < p.size(); i++) {
		for (int j = 0; j < 360; j += 30) {
			Velocity v = Velocity::makeTrkGsVs(j, buffer, 0.0);
			Position pos = p.getVertex(i).linear(v, 1.0);
			ps.push_back(pos);
		}
	}
	return convexHull(ps,p.getBottom(), p.getTop());
}

Position PolyUtil::pushOut(const SimplePoly& poly, const Position& p, double buffer) {
	Position cent = poly.centroid();
	double dist = cent.distanceH(p);
	Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
	return p.linear(v, buffer);
}

Position PolyUtil::pushOut(const SimplePoly& poly, int i, double buffer) {
	Position p = poly.getVertex(i);
	Position cent = poly.centroid();
	double dist = cent.distanceH(p);
	Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
	int h = i-1;
	if (h < 0) {
		h = poly.size()-1;
	}
	int j = i+1;
	if (j >= poly.size()) {
		j = 0;
	}
	if (!poly.isClockwise()) {
		int tmp = h;
		h = j;
		j = tmp;
	}
	double trk1 = p.initialVelocity(poly.getVertex(j), 100).trk();
	double trk2 = p.initialVelocity(poly.getVertex(h), 100).trk();
	v.mkTrk((trk1+trk2)/2 + Pi);
	return p.linear(v, buffer);
}





SimplePoly PolyUtil::simplify2(const SimplePoly& p, double buffer) {
	if (p.size() <= 3) {
		fpln("Simplify size too small");
		return p.copy();
	}
	//std::vector<SimplePoly> fails = std::vector<SimplePoly>();
	Position c = p.centroid();
	int i = 3;
	while (i <= 90 && 360.0/i < p.size()) {
		// divide up into angular segments and find max point in each segment
		double incr = 2*Pi/i;
		SimplePoly p2 = SimplePoly(p.getBottom(),p.getTop());
		for (int j = 0; j < i; j++) {
			int idx = p.maxInRange(c, j*incr, (j+1)*incr);
			if (idx >= 0) {
				//					p2.addVertex(pushOut(points[idx],buffer));
				p2.addVertex(pushOut(p,idx,buffer));
			}
		}
		// remove points that are probably from non-convex points

		// check containment of original
		bool ok = true;
		for (int j = 0; j < p.size(); j++) {
			if (!p2.contains2D(p.getVertex(j))) {
				ok = false;
				//fails.add(p2);
				fpln("SimplePoly simplify i="+Fm0(i)+" does not contain "+Fm0(j)+" "+p2.getVertex(j).toString());
				break;
			}
		}
		if (ok) {
			//				f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
			return p2;
		}
		if (i >= 45) {
			i = i*2;
		} else if (i >= 20) {
			i = i + 5;
		} else {
			i++;
		}
	}
	fpln("SimplePoly.Simplify failed.");

	return p.copy();
}


SimplePoly PolyUtil::simplify(const SimplePoly& p, double buffer) {
	if (p.size() <= 3) {
//			f.pln("Simplify size too small");
		return p.copy();
	}
	//	std::vector<SimplePoly> fails = std::vector<SimplePoly>();
	Position c = p.centroid();
	int i = 3;
//f.pln("Simplify i="+i+" size="+p.size());
	while (i <= 90 && i < p.size()) {
//f.pln("Simplify i="+i);
		// divide up into angular segments and find max point in each segment
		double incr = 2*Pi/i;
		SimplePoly p2 = SimplePoly(p.getBottom(),p.getTop());
		for (int j = 0; j < i; j++) {
			int idx = p.maxInRange(c, j*incr, (j+1)*incr);
			if (idx >= 0) {
				//						p2.addVertex(pushOut(points[idx],buffer));
				p2.addVertex(pushOut(p,idx,buffer));
			}
		}
		// remove points that are probably from non-convex points

		// check containment of original
		bool ok = true;
		for (int j = 0; j < p.size(); j++) {
			if (!p2.contains2D(p.getVertex(j))) {
				ok = false;
				//	fails.add(p2);
//					f.pln("SimplePoly simplify i="+i+" does not contain "+j+" "+p2.getVertex(j));
				break;
			}
		}
		if (ok) {
//				f.pln("SimplePoly simplify from="+p.size()+" to="+p2.size());
			return p2;
		}
		if (i >= 45) {
			i = i*2;
		} else if (i >= 20) {
			i = i + 5;
		} else {
			i++;
		}
	}

	return p.copy();
}

SimplePoly PolyUtil::simplify(const SimplePoly& p) {
	bool done = false;
	SimplePoly p2 = p.copy();
//	int step = 0;

	ErrorLog error = ErrorLog("SimplePoly");

	while (!done) {
		done = true;
		int i = 0;
		while (i < p2.size() && p2.size() > 3) {
			int h = i-1;
			int j = i+1;
			if (h < 0) h = p2.size()-1;
			if (j == p2.size()) j = 0;
			double trk1 = p2.getVertex(h).finalVelocity(p2.getVertex(i), 100).trk();
			double trk2 = p2.getVertex(i).initialVelocity(p2.getVertex(j),100).trk();
			if (Util::clockwise(trk1, trk2) != p.isClockwise()) {
				p2.remove(i);
				done = false;
			}
			i++;
			if (p.isClockwise() != p2.isClockwise()) fpln("CLOCKWISE MISMATCH");
		}
		//DebugSupport.dumpSimplePoly(p2, "step"+(step++));
	}
	return p2;
}


SimplePoly PolyUtil::simplifyToSize(const SimplePoly& p, int num) {
	if (p.size() <= num) return p.copy();
	SimplePoly tmp = simplify(p, 0.0);
	double incr = p.maxRadius()*0.1;
	double buff = 0.0;
	int count = 0;
	while (tmp.size() > num && count < 3) {
		buff += incr;
		tmp = simplify(p, buff);
		count++;
	}
	//			f.pln("SimplePoly simplifyToSize count="+count+"  buff="+buff+"  incr="+incr+" from="+size()+" to="+tmp.size());
	return tmp;
}



SimplePoly PolyUtil::stretchOverTime(const SimplePoly& sp, const Velocity& v, double timeBefore, double timeAfter) {
	std::vector<SimplePoly> alist = std::vector<SimplePoly>();
	alist.push_back(sp);
	alist.push_back(sp.linear(v, timeAfter));
	alist.push_back(sp.linear(v.Neg(), timeBefore));
	return convexHull(alist);
}

PolyPath PolyUtil::stretchOverTime(const PolyPath& pbase, double timeBefore, double timeAfter) {
	PolyPath pp = PolyPath(pbase);
	if (pp.getPathMode() == PolyPath::MORPHING) {
		pp.setPathMode(PolyPath::USER_VEL_FINITE);
	}
	for (int i = 0; i < pp.size(); i++) {
		SimplePoly sp = pp.getPolyRef(i);
		SimplePoly np = stretchOverTime(sp,pp.initialVelocity(i), timeBefore, timeAfter);
		pp.setPolygon(i, np);
	}
	return pp;
}


SimplePoly PolyUtil::bufferedConvexHull(const SimplePoly& p, double hbuff, double vbuff) {
	std::vector<Position> points = std::vector<Position>();
	std::vector<Position> p_points = p.getVertices();
	for (int i = 0; i < (int) p_points.size(); i++) {
		Position pos = p_points[i];
		double ang = p.vertexAngle(i);
		double ang2 = ang/2.0;
		double trk1 = p.perpSide(i);
		Velocity v1 = Velocity::mkTrkGsVs(trk1, 1.0, 0.0);
		double trk3 = p.isClockwise() ?  (trk1 - Pi/2 + ang2) : (trk1 + Pi/2 - ang2);
		Velocity v3 = Velocity::mkTrkGsVs(trk3, 1.0, 0.0);
		points.push_back(pos.linear(v3, hbuff));

			double trk2;
			if (i == 0) {
				trk2 = p.perpSide(p_points.size()-1);
			} else {
				trk2 = p.perpSide(i-1);
			}
			Velocity v2 = Velocity::mkTrkGsVs(trk2, 1.0, 0.0);
			points.push_back(pos.linear(v1, hbuff));
			points.push_back(pos.linear(v2, hbuff));
	}
	return convexHull(points, p.getBottom()-vbuff, p.getTop()+vbuff);
}

PolyPath PolyUtil::bufferedConvexHull(const PolyPath& pbase, double hbuff, double vbuff) {
	PolyPath pp = PolyPath(pbase);
	if (pp.getPathMode() == PolyPath::MORPHING) {
		pp.setPathMode(PolyPath::USER_VEL_FINITE);
	}
	for (int i = 0; i < pp.size(); i++) {
		SimplePoly sp = pp.getPolyRef(i);
		SimplePoly np = bufferedConvexHull(sp,hbuff, vbuff);
		pp.setPolygon(i, np);
	}
	return pp;
}

bool PolyUtil::intersectsPolygon2D(const Position& so, const Velocity& vo, const SimplePoly& sp, const Velocity& vp, double T, double incr) {
	if (incr <= 0) return false;
	BoundingRectangle br1 = BoundingRectangle();
	br1.add(so);
	br1.add(so.linear(vo, T));
	BoundingRectangle br2 = sp.getBoundingRectangle();
	br2.add(sp.linear(vp, T).getBoundingRectangle());
	if (!br1.intersects(br2)) return false;
	for (double t = 0; t <= T; t += incr) {
		if (sp.linear(vp, t).contains2D(so.linear(vo, t))) return true;
	}
	return false;
}

bool PolyUtil::intersectsPolygon(const Position& so, const Velocity& vo, const SimplePoly& sp, const Velocity& vp, double T, double incr) {
	if (incr <= 0) return false;
	BoundingRectangle br1 = BoundingRectangle();
	br1.add(so);
	br1.add(so.linear(vo, T));
	BoundingRectangle br2 = sp.getBoundingRectangle();
	br2.add(sp.linear(vp, T).getBoundingRectangle());
	if (!br1.intersects(br2)) return false;
	for (double t = 0; t <= T; t += incr) {
		if (sp.linear(vp, t).contains(so.linear(vo, t))) return true;
	}
	return false;
}


std::pair<double,string> PolyUtil::intersectsPolygon2D(const Plan& p, const PolyPath& pp, double B, double T, double incr) {
	std::pair<double,string> rtn = std::pair<double,string>(-1.0,"");
	if (incr <= 0) return rtn;
	BoundingRectangle br1 = p.getBound();
	BoundingRectangle br2 = pp.getBoundingRectangle();
	if (!br1.intersects(br2)) return rtn;
	double start = Util::max(B, Util::max(p.getFirstTime(), pp.getFirstTime()));
	double end = Util::min(T, Util::min(p.getLastTime(), pp.getLastTime()));
	//f.pln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" start="+start+" end="+end+" CLEAR");
	for (double t = start; t <= end; t += incr) {
		SimplePoly sp = pp.position(t);
		Position ac = p.position(t);
		if (sp.contains2D(ac)) {
			//f.pln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" t="+t+" LOSS");
			rtn = std::pair<double,string>(t,pp.getName());
			return rtn;
		}
	}
	//f.pln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" B="+B+" T="+T+" CLEAR");
	return rtn;
}


Plan PolyUtil::reducePlanAgainstPolys(const Plan& p, const std::vector<PolyPath>& paths, double incr) {
	double gs = p.initialVelocity(0).gs();
	return reducePlanAgainstPolys(p,gs,paths,incr);
}

Plan PolyUtil::reducePlanAgainstPolys(const Plan& plan, double gs, const std::vector<PolyPath>& paths, double incr) {
	if (incr <= 0) return Plan();
	Plan curr = Plan(plan);
	Plan tmp;
	int lastsz = plan.size()+1;
	while (lastsz > curr.size()) {
//f.pln("lastsz="+lastsz+" currsz="+curr.size());
		lastsz = curr.size();
		int i = 1;
		while (i < curr.size()-1) {
			tmp = Plan(curr);
			double start = tmp.time(i-1);
			double end = tmp.time(i+1);
			tmp.remove(i);
			tmp = PlanUtil::linearMakeGSConstant(tmp,gs);
			if (isPlanInConflictWx(tmp, paths, start, end, incr).first < 0) {
				curr = tmp; // shrink current
			} else {
				i++; // advance index
			}
		}
	};
	return curr;
}

std::pair<double,string> PolyUtil::isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double start, double end, double incr) {
	for (int i = 0; i < (int) paths.size(); i++) {
		std::pair<double,string> ip2D = intersectsPolygon2D(plan, paths[i], start, end, incr);
		double tmOfIntersection = ip2D.first;
		if (tmOfIntersection >= 0) {
			return ip2D;
		}
	}
	return std::pair<double,string>(-1.0,"");
}

std::pair<double,string>  PolyUtil::isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double incr) {
	 return isPlanInConflictWx(plan,paths,incr,plan.getFirstTime());
}

std::pair<double,string> PolyUtil::isPlanInConflictWx(const Plan& plan, const std::vector<PolyPath>& paths, double incr, double fromTime) {
	if (paths.size() > 0) {
		for (int i = 0; i < (int) paths.size(); i++) {
			std::pair<double,string> ip2D = intersectsPolygon2D(plan, paths[i], fromTime, plan.getLastTime(), incr);
			double tmOfIntersection = ip2D.first;
			if (tmOfIntersection >= 0) {
				return ip2D;
			}
		}
	}
	return std::pair<double,string>(-1.0,"");
}


double PolyUtil::calculateWxExitTime(const Plan& plan, const std::vector<PolyPath>& paths, double incr, double entryTime) {
	double lastTm = plan.getLastTime();
	for (double exTm = entryTime; exTm <= lastTm; exTm = exTm + incr) {
		double tm = isPlanInConflictWx(plan, paths, incr, exTm).first;
		if (tm < 0) return exTm;
	}
	return lastTm;
}


} // namespace 
