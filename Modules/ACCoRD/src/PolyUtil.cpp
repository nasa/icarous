/*
 * Copyright (c) 2011-2019 United States Government as represented by
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
#include "Triple.h"
#include "Constants.h"
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

SimplePoly PolyUtil::convexHull(const std::vector<Position>& pinlist, double bottom, double top) {
	const double MIN_H_DIST = Constants::get_horizontal_accuracy();

	std::vector<Triple<Position,double,double> > elems; // = std::vector<Triple<Position,int,double> >(); // vertex position, vertex index in original polygon, angle from origin point
	int origin = 0;
	if (pinlist.size() == 0) {
		return SimplePoly();
	}
	// remove redundant points
	std::vector<Position> plist;
	for (int i = 0; i < (int) pinlist.size(); i++) {
		bool found = false;
		for (int j = i+1; !found && j < (int) pinlist.size(); j++) {
			if (pinlist[i] == pinlist[j]) {
				found = true;
			}
		}
		if (!found) {
			plist.push_back(pinlist[i]);
		}
	}
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
			Velocity iv_i = po.initialVelocity(pi,100);
			//fpln(" $$$ convexHull: i = "+Fm0(i)+" iv_i = "+iv_i.toString());
			double val = Util::to_2pi(iv_i.trk());
			double dist = -po.distanceH(pi);
			//fpln(" $$$ convexHull: i = "+Fm0(i)+" iv_i = "+iv_i.toString() +" val = "+Fm6(val));
			elems.push_back(Triple<Position,double,double>(pi, val, dist));
		}
	}
	std::stable_sort(elems.begin(), elems.end(), SIComparator());
	elems.insert(elems.begin(),Triple<Position,double,double>(plist[origin],-DBL_MAX,-DBL_MAX));
	// delete any points that have a left-hand turn
	//fpln(" $$ convexHull: elems.size() = "+Fm0(elems.size()));
	for (int i = 0; i < (int) elems.size(); i++) {
	   //fpln(" $$$  "+Fm0(i)+"  elems[i].first = "+elems[i].first.toString());
	   //fpln(" $$$  "+Fm0(i)+"  elems[i].third = "+Units::str("deg",elems[i].third));
	}

	int i = 1;
	while (i < (int) elems.size()) {
		int j = i+1;
		if (j == (int) elems.size()) j = 0; // last point
		Position a = elems[i-1].first;
		Position b = elems[i].first;
		//fp(" $$$  "+Fm0(i)+"  b = "+b.toString());
		Position c = elems[j].first;
		double trk1 = a.finalVelocity(b, 100).trk();
		double trk2 = b.initialVelocity(c, 100).trk();
		//fpln(" "+Fm0(i)+"  trk1 = "+Units::str("deg",trk1)+" trk2 = "+Units::str("deg",trk2)+" turnDir = "+Fm0(Util::turnDir(trk1, trk2)));
		//fpln(" "+Fm0(i)+" "+bool2str(a.almostEquals(b))+" "+bool2str(b.almostEquals(c))+" "+bool2str(Util::turnDir(trk1, trk2) <= 0));
		if (a.almostEquals2D(b, MIN_H_DIST) || b.almostEquals2D(c, MIN_H_DIST) || Util::turnDir(trk1, trk2) <= 0) {
			//fpln(" $$ convexHull: remove i = "+Fm0(i));
			elems.erase(elems.begin()+i);
			if (i > 1) i--; // backtrack 1
		} else {
			i++;
		}
	}
	SimplePoly p2(bottom, top);
	std::reverse(elems.begin(), elems.end());
	for (i = 0; i < (int) elems.size(); i++) {
		p2.add(elems[i].first);
	}
	//fpln(" $$ convexHull: p2.size() = "+Fm0(p2.size()));
	return p2;
}

SimplePoly PolyUtil::convexHull(const SimplePoly& p) {
	return convexHull(p.getVertices(), p.getBottom(), p.getTop());
}


SimplePoly PolyUtil::convexHull(const std::vector<SimplePoly>& p) {
	std::vector<Position> ps; // = std::vector<Position>();
	double t = -DBL_MAX;
	double b = DBL_MAX;
	for (int i = 0; i < (int) p.size(); i++) {
		std::vector<Position> pts = p[i].getVertices();
		//fpln(" $$$ convexHull: pts = "+Fm0(pts.size()));
		ps.insert(ps.end(), pts.begin(), pts.end());
		t = Util::max(t, p[i].getTop());
		b = Util::min(b, p[i].getBottom());
	}
	return convexHull(ps, b, t);
}


SimplePoly PolyUtil::convexHull(const SimplePoly& p, double buffer) {
	std::vector<Position> ps; // = std::vector<Position>();
	for (int i = 0; i < p.size(); i++) {
		for (int j = 0; j < 360; j += 30) {
			Velocity v = Velocity::makeTrkGsVs(j, buffer, 0.0);
			Position pos = p.getVertex(i).linear(v, 1.0);
			ps.push_back(pos);
		}
	}
	return convexHull(ps,p.getBottom(), p.getTop());
}

Position PolyUtil::pushOut(SimplePoly& poly, const Position& p, double buffer) {
	Position cent = poly.centroid();
	double dist = cent.distanceH(p);
	Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
	return p.linear(v, buffer);
}

Position PolyUtil::pushOut(SimplePoly& poly, int i, double buffer) {
	Position p = poly.getVertex(i);
	//fpln(" $$$$ pushOut p = "+p.toString());
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





SimplePoly PolyUtil::simplify2(SimplePoly& p, double buffer) {
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
				p2.add(pushOut(p,idx,buffer));
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


SimplePoly PolyUtil::simplify(SimplePoly& p, double buffer) {
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
				p2.add(pushOut(p,idx,buffer));
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

SimplePoly PolyUtil::simplify(SimplePoly& p) {
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


SimplePoly PolyUtil::simplifyToSize(SimplePoly& p, int num) {
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
	std::vector<SimplePoly> alist; //  = std::vector<SimplePoly>();
	alist.push_back(sp);
	alist.push_back(sp.linear(v, timeAfter));
	alist.push_back(sp.linear(Velocity(v.Neg()), timeBefore));
	//fpln(" $$ stretchOverTime(SimplePoly): alist.size() = "+Fm0(alist.size()));
	SimplePoly rtn = convexHull(alist);
	//fpln(" $$ stretchOverTime(SimplePoly): rtn = "+rtn.toString());
	return rtn;
}

PolyPath PolyUtil::stretchOverTime(const PolyPath& pbase, double timeBefore, double timeAfter) {
	PolyPath pp(pbase);
	//fpln(" $$ stretchOverTime(PolyPath): timeAfter = "+Fm3(timeAfter));
	if (pp.getPathMode() == PolyPath::MORPHING) {
		pp.setPathMode(PolyPath::USER_VEL_FINITE);
		pp.addWarning("PolyUtil.stretchOverTime: Mode changed from MORPHING to USER_VEL_FINITE");
	}
	for (int i = 0; i < pp.size(); i++) {
		SimplePoly sp = pp.getPolyRef(i);  // TODO:  RWB IS THIS Ref OK?
		SimplePoly np = stretchOverTime(sp,pp.initialVelocity(i), timeBefore, timeAfter);
		pp.setPolygon(i, np);
	}
	//fpln(" $$ stretchOverTime(PolyPath): RETURN pp = "+pp.toString());
	return pp;
}


SimplePoly PolyUtil::bufferedConvexHull(SimplePoly& p, double hbuff, double vbuff) {
	//fpln("=========================================");
	std::vector<Position> points = std::vector<Position>();
	std::vector<Position> p_verts = p.getVertices();
	for (int i = 0; i < (int) p_verts.size(); i++) {
		Position pos = p_verts[i];
		//fpln(" $$ bufferedConvexHull: i = "+Fm0(i)+" pos = "+pos.toString());
		double ang = p.vertexAngle(i);
		double ang2 = ang/2.0;
		double trk1 = p.perpSide(i);
		Velocity v1 = Velocity::mkTrkGsVs(trk1, 1.0, 0.0);
		double trk3 = p.isClockwise() ?  (trk1 - Pi/2 + ang2) : (trk1 + Pi/2 - ang2);
		Velocity v3 = Velocity::mkTrkGsVs(trk3, 1.0, 0.0);
		points.push_back(pos.linear(v3, hbuff));
		double trk2;
		if (i == 0) {
			trk2 = p.perpSide(p_verts.size()-1);
		} else {
			trk2 = p.perpSide(i-1);
		}
		Velocity v2 = Velocity::mkTrkGsVs(trk2, 1.0, 0.0);
		Position pt1 = pos.linear(v1, hbuff);
		Position pt2 = pos.linear(v2, hbuff);
		//fpln(" $$ bufferedConvexHull: i="+Fm0(i)+" pt1="+pt1.toString()+" pgt2="+pt2.toString());
		//fpln(" $$ bufferedConvexHull: v1="+v1.toString()+" v2="+v2.toString()+" v3="+v3.toString());
		points.push_back(pt1);
		points.push_back(pt2);
	}
	SimplePoly ret = convexHull(points, p.getBottom()-vbuff, p.getTop()+vbuff);
	//fpln("=========================================");
	return ret;

}

PolyPath PolyUtil::bufferedConvexHull(const PolyPath& pbase, double hbuff, double vbuff) {
	PolyPath pp = PolyPath(pbase);
	if (pp.getPathMode() == PolyPath::MORPHING) {
		pp.makeVertexCountEqual();
//		pp.setPathMode(PolyPath::USER_VEL_FINITE);
	}
	for (int i = 0; i < pp.size(); i++) {
		SimplePoly sp = pp.getPolyRef(i);   // TODO: RWB is this ref ok?
		SimplePoly np = bufferedConvexHull(sp,hbuff, vbuff);
		pp.setPolygon(i, np);
	}
	return pp;
}

//bool PolyUtil::intersectsPolygon2D(const Position& so, const Velocity& vo, SimplePoly& sp, const Velocity& vp, double T, double incr) {
//	if (incr <= 0) return false;
//	BoundingRectangle br1 = BoundingRectangle();
//	br1.add(so);
//	br1.add(so.linear(vo, T));
//	BoundingRectangle br2 = sp.getBoundingRectangle();
//	br2.add(sp.linear(vp, T).getBoundingRectangle());
//	if (!br1.intersects(br2)) return false;
//	for (double t = 0; t <= T; t += incr) {
//		if (sp.linear(vp, t).contains2D(so.linear(vo, t))) return true;
//	}
//	return false;
//}
//
//bool PolyUtil::intersectsPolygon(const Position& so, const Velocity& vo, SimplePoly& sp, const Velocity& vp, double T, double incr) {
//	if (incr <= 0) return false;
//	BoundingRectangle br1 = BoundingRectangle();
//	br1.add(so);
//	br1.add(so.linear(vo, T));
//	BoundingRectangle br2 = sp.getBoundingRectangle();
//	br2.add(sp.linear(vp, T).getBoundingRectangle());
//	if (!br1.intersects(br2)) return false;
//	for (double t = 0; t <= T; t += incr) {
//		if (sp.linear(vp, t).contains(so.linear(vo, t))) return true;
//	}
//	return false;
//}


double PolyUtil::intersectsPolygon2D(const Plan& p, PolyPath& pp, double B, double T, double incr) {
	//fpln(" $$$ intersectsPolygon2D: ENTER pp.mode = "+pp.pathModeToString(pp.getPathMode()));
	double rtn = -1.0;
	if (incr <= 0) return rtn;
	BoundingRectangle br1 = p.getBoundBox().getBoundRect();
	double polyExtensionTm = std::max(0.0,p.getLastTime() - pp.getLastPolyTime());
	BoundingRectangle br2 = pp.getBoundingRectangle(polyExtensionTm);
	if (!br1.intersects(br2)) return rtn;
	double start = Util::max(B, Util::max(p.getFirstTime(), pp.getFirstTime()));
	double end = Util::min(T, Util::min(p.getLastTime(), pp.getLastTime()));
	//fpln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" start="+Fm0(start)+" end = "+Fm0(end));
	for (double t = start; t <= end; t += incr) {
		SimplePoly sp = pp.position(t);
		Position ac = p.position(t);
		if (sp.contains2D(ac)) {
			//f.pln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" t="+t+" LOSS");
			return t;
		} else {
			// f.pln(" $$ PolyUtil.intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" t="+t+" OK");
		}
	}
	//f.pln(" $$ PolyUtil::intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" B="+B+" T="+T+" CLEAR");
	return rtn;
}


Plan PolyUtil::reducePlanAgainstPolys(const Plan& pln, double gs, std::vector<PolyPath>& paths,
		                             double incr, bool leadInsPresent, const std::vector<PolyPath>& containment) {
	//fpln(" $$$$$ reducePlanAgainstPolys: pln = "+pln.toString());
	//fpln(" $$$$$ reducePlanAgainstPolys: gs = "+Fm2(gs)+" incr = "+Fm0(incr)+" leadInsPresent = "+bool2str(leadInsPresent));
	//fpln(" $$$$$ reducePlanAgainstPolys: paths = "+paths[0].toString());
	//fpln(" $$$$$ reducePlanAgainstPolys: ENTER paths.size() = "+Fm0(paths.size()));
	//fpln(" $$$$$ reducePlanAgainstPolys: ENTER containment.size() = "+Fm0(containment.size()));
	if (incr <= 0) return Plan();
	Plan curr = Plan(pln);
	Plan tmp;
	int lastsz = pln.size()+1;
	if (isPlanInConflictWx(curr, paths, curr.time(0), curr.getLastTime(), incr).first >= 0) { // no conflict
		///fpln(" !!! reducePlanAgainstPolys: WARNING: input plan is not conflict free!!!");
		curr.addWarning("PolyUtil::reducePlanAgainstPolys: WARNING: input plan is not conflict free!");
	}
	if (leadInsPresent && pln.size() > 3) {
		curr.setAltPreserve(1);
		curr.setAltPreserve(pln.size()-2);
	}
	//fpln("$$$$$ reducePlanAgainstPolys: BEGIN LOOP: lastsz="+Fm0(lastsz)+" currsz="+Fm0(curr.size()));
	//fpln(" $$$$$ curr = "+curr.toString());
	while (lastsz > curr.size()) {
		//fpln("$$$$$ reducePlanAgainstPolys:: lastsz="+Fm0(lastsz)+" currsz="+Fm0(curr.size()));
		lastsz = curr.size();
		int i = 1;
		while (i < curr.size()-1) {
			tmp = Plan(curr);
			double start =  tmp.time(i-1); // tmp.time(i-1);
			double end = tmp.getLastTime(); //  tmp.time(i+1);
			//fpln(" $$$$ i = "+Fm0(i)+" tmp.isAltPreserve(i) = "+bool2str(tmp.isAltPreserve(i)));
			if (tmp.isAltPreserve(i)) {
				i++;
			} else {
				tmp.remove(i);
				tmp = PlanUtil::mkGsConstant(tmp,gs);
				double tmConflict = isPlanInConflictWx(tmp, paths, start, end, incr).first;
				bool stay = isPlanContained(tmp, containment, start, end, incr);
				//fpln("  $$$$$$ reducePlanAgainstPolys: i="+Fm0(i)+" stay = "+bool2str(stay)+" tmConflict = "+Fm4(tmConflict));
				if (tmConflict < 0 && stay) {
					curr = tmp; // shrink current
				} else {
					i++; // advance index
				}
			}
		}
	};
	curr.removeAltPreserves();
	//fpln(" $$$$$ reducePlanAgainstPolys: EXIT: curr = "+curr.toString());
	return curr;
}

 Plan PolyUtil::reducePlanAgainstPolys(const Plan& pln, double gs, std::vector<PolyPath>& paths, double incr,
		bool leadInsPresent) {
	std::vector<PolyPath> containment;
	return reducePlanAgainstPolys(pln, gs, paths, incr, leadInsPresent, containment);
}


 std::pair<double,string> PolyUtil::isPlanInConflictWx(const Plan& plan, std::vector<PolyPath>& paths, double start, double end, double incr) {
    //fpln(" $$$ isPlanInConflictWx: ENTER paths.size() = "+Fm0(paths.size()));
	for (int i = 0; i < (int) paths.size(); i++) {
		//fpln(" $$$ isPlanInConflictWx: i = "+Fm0(i));
		double tmOfIntersection = intersectsPolygon2D(plan, paths[i], start, end, incr);
		if (tmOfIntersection >= 0) {
			return std::pair<double,string>(tmOfIntersection,paths[i].getID());
		}
	}
	return std::pair<double,string>(-1.0,"");
}

 std::pair<double,string>  PolyUtil::isPlanInConflictWx(const Plan& plan,
		 std::vector<PolyPath>& paths, double incr) {
	 return isPlanInConflictWx(plan,paths,incr,plan.getFirstTime());
}

 std::pair<double,string> PolyUtil::isPlanInConflictWx(const Plan& plan, std::vector<PolyPath>& paths, double incr, double fromTime) {
	if (paths.size() > 0) {
		for (int i = 0; i < (int) paths.size(); i++) {
			double tmOfIntersection = intersectsPolygon2D(plan, paths[i], fromTime, plan.getLastTime(), incr);
			if (tmOfIntersection >= 0) {
				return std::pair<double,string>(tmOfIntersection,paths[i].getID());
			}
		}
	}
	return std::pair<double,string>(-1.0,"");
}

/**
 * Returns time of intersection if the plan is NOT free of polygons from time start to time end.  This may miss intrusions of up to incr sec
 * so it may not detect
 * @param plan   plan to test
 * @param paths  set of polygons
 * @param start  start time of search
 * @param end    end time of search
 * @param incr   search increment
 * @return  time of intersection with a polygon if it occurs when search time interval, otherwise -1
 *          and polygon name that plan is in conflict with
 */
bool PolyUtil::isPlanContained(const Plan& plan, const std::vector<PolyPath>& paths, double B, double T, double incr) {
	if (paths.size() == 0) return true;
	//        f.pln(" $$$ staysWithinPolygon2D: incr = "+incr+" pp = "+pp);
	if (incr <= 0) return true;
	double start = std::max(B, plan.getFirstTime());
	double end = std::min(T, plan.getLastTime());
	//		f.pln(" $$ PolyUtil.intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" start="+start+" end="+end+" CLEAR");
	for (double t = start; t <= end; t += incr) {
		Position ac = plan.position(t);
		bool ok = false;
		for (int j = 0; !ok && j < (int) paths.size(); j++) {
			PolyPath pp = paths[j];
			if (t < pp.getFirstTime() || t > pp.getLastTime()) continue;
			SimplePoly sp = pp.position(t);
			if (sp.contains2D(ac)) {
				ok = true;
			} else {
				// f.pln(" $$ PolyUtil.staysWithinPolygon2D p="+p.getName()+" path="+pp.getName()+" t="+t+" OK");
			}
		}
		if (!ok) return false;
	}
	//f.pln(" $$ PolyUtil.staysWithinPolygon2D p="+p.getName()+" path="+pp.getName()+" B="+B+" T="+T+" CLEAR");
	return true;
}



double PolyUtil::calculateWxExitTime(const Plan& plan, std::vector<PolyPath>& paths, double incr, double entryTime) {
	double lastTm = plan.getLastTime();
	for (double exTm = entryTime; exTm <= lastTm; exTm = exTm + incr) {
		double tm = isPlanInConflictWx(plan, paths, incr, exTm).first;
		if (tm < 0) return exTm;
	}
	return lastTm;
}


} // namespace 
