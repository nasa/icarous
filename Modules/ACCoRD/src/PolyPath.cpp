/*
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "PolyPath.h"
#include "Position.h"
#include "NavPoint.h"
#include "Velocity.h"
#include "SimplePoly.h"
#include "SimpleMovingPoly.h"
#include "Plan.h"
#include "Util.h"
#include "Constants.h"
#include "BoundingRectangle.h"
#include "Triple.h"
#include "format.h"
#include "string_util.h"
#include <vector>
#include <string>
#include <cfloat>
#include <sstream>
#include <limits>

#include "Debug.h"


namespace larcfm {

using std::vector;
using std::string;
using std::endl;

PolyPath::PolyPath() : error("PolyPath") {
	label = "";
	mode = AVG_VEL;
	modeSet = false;
}

PolyPath::PolyPath(const std::string& n) : error("PolyPath") {
	label = n;
	mode = AVG_VEL;
	modeSet = false;
}

PolyPath::PolyPath(const std::string& n, PathMode m) : error("PolyPath") {
	label = n;
	mode = m;
	modeSet = true;
}


/**
 * Make a PolyPlan for a non-dynamic area p.
 * @param p
 */
PolyPath::PolyPath(const std::string& n, const SimplePoly& p) : error("PolyPath") {
	label = n;
	polyList.push_back(p);
	times.push_back(0.0);
	vlist.push_back(Velocity::INVALIDV());
	mode = AVG_VEL;
	modeSet = true;
}

PolyPath::PolyPath(const std::string& n, const SimplePoly& p, double t) : error("PolyPath") {
	label = n;
	polyList.push_back(p);
	times.push_back(t);
	vlist.push_back(Velocity::INVALIDV());
	mode = AVG_VEL;
	modeSet = true;
}

PolyPath::PolyPath(const std::string& n, const SimplePoly& p, const Velocity& v, double t) : error("PolyPath") {
	label = n;
	polyList.push_back(p);
	times.push_back(t);
	vlist.push_back(v);
	mode = USER_VEL;
	modeSet = true;
}


PolyPath::PolyPath(const std::string& n, const vector<SimplePoly>& ps, const vector<double>& ts) : error("PolyPath") {
	label = n;
	if (ps.size() == ts.size()){
		for (int i=0; i < (signed)ps.size(); i++) {
			addPolygon(ps[i],ts[i]);
		}
	}
	mode = AVG_VEL;
	modeSet = true;
}

//// deep copy
//PolyPath::PolyPath(const PolyPath& p) : error("PolyPath") {
//	error = p.error;
//	name = p.name;
//	//	morphingPolys = p.morphingPolys;
////	fpln("\n $$$$$$$$ PolyPath(p) Constructor: name = "+p.getName());
//	for (int i = 0; i < p.size(); i++) {
////		fpln(" $$$$$$$$ PolyPath(p) Constructor: addPolygon i = "+Fm0(i)+" p.times[i] = "+Fm2(p.times[i]));
//		addPolygon(p.polyList[i].copy(), p.vlist[i], p.times[i]);
//	}
//	mode = p.mode;
//	modeSet = p.modeSet;
////	fpln("$$$$$$$$ PolyPath(p) Constructor: name = "+p.getName()+"  DONE");
//}



PolyPath PolyPath::pathFromState(const std::string& n, const SimplePoly& p, const Velocity& v, double tstart, double tend) {
	SimplePoly ep = p.linear(v, tend-tstart);
	PolyPath pp = PolyPath(n,p,tstart);
	pp.addPolygon(ep, tend);
	pp.setID(n);
	pp.mode = AVG_VEL;
	pp.modeSet = true;
	return pp;
}

PolyPath PolyPath::pathFromState(const std::string& n, const MovingPolygon3D& p, EuclideanProjection proj, double tstart, double tend) {
	SimplePoly sp = SimplePoly::make(p.position(0), proj);
	SimplePoly ep = SimplePoly::make(p.position(tend-tstart), proj);
	PolyPath pp = PolyPath(n,sp,tstart);
	pp.addPolygon(ep, tstart+tend);
	pp.mode = AVG_VEL;
	pp.modeSet = true;
	if (sp.size() < p.size()) {
		pp.error.addWarning("pathFromState: "+pp.getID()+" attempted to add "+Fm0(p.size()-sp.size())+" invalid or duplicate vertices, ignored");
	}
	return pp;
}


void PolyPath::addPolygon(const SimplePoly& ps, double ts) {
    //fpln(" $$$$$$$$$ addPolygon: ps.size() = "+Fm0(ps.size())); // +" =??"+Fm0(polyList[0].size()));
	unsigned int i = 0;
	if (mode == MORPHING && times.size() > 0 && ps.size() != polyList[0].size()) { // make sure new polygon is the same size
		fpln("$$$$ addPolygon: Poly size does not match existing list for MORPHING mode!");
		error.addError("addPolygon: Poly size does not match existing list for MORPHING mode!");
		return;
	}
	if (isUserVel()) {
		fpln("$$$$ addPolygon(p,t): current USER_VEL* mode requires velocity information");
		error.addWarning("addPolygon(p,t): current USER_VEL* mode requires velocity information");
		return;
	}
	while(i < times.size() && times[i] < ts) {
		i++;
	}
	polyList.insert(polyList.begin()+i,ps);
	times.insert(times.begin()+i,ts);
	vlist.insert(vlist.begin()+i,Velocity::INVALIDV());
	if (!modeSet) {
		mode = AVG_VEL;
		modeSet = true;
	}
}


void PolyPath::addPolygon(const SimplePoly& ps, const Velocity& v, double ts) {
	unsigned int i = 0;
	//fpln(" $$$ "+getName()+" times.size() = "+Fm0(times.size())+" isUserVel() = "+bool2str(isUserVel()));
	if (times.size() > 0 && !isUserVel()) {
		//fpln(" $$$ "+getName()+".addPolygon(p,v,t): previous polygons did not have velocity information, it will be ignored here");
		error.addWarning(getID()+".addPolygon(p,v,t): previous polygons did not have velocity information, it will be ignored here");
		if (mode == MORPHING && ps.size() != polyList[0].size()) {
			error.addError("addPolygon: Poly size does not match existing list for MORPHING mode!");
			return;
		}
	} else if (!modeSet) {
		mode = USER_VEL;
		modeSet = true;
	}
	while(i < times.size() && times[i] < ts) {
		i++;
	}
	polyList.insert(polyList.begin()+i,ps);
	times.insert(times.begin()+i,ts);
	vlist.insert(vlist.begin()+i,v);
	//fpln(" $$$$ PolyPath.addPolygon: "+toStringShort());
}

// deep copy
PolyPath PolyPath::copy() const {
	PolyPath pp;
	pp.error = error;
	pp.label = label;
	//	morphingPolys = p.morphingPolys;
	//fpln("\n $$$$$$$$ PolyPath.copy(): name = "+pp.getName());
	for (int i = 0; i < size(); i++) {
		//fpln(" $$$$$$$$ PolyPath.copy(): addPolygon i = "+Fm0(i)+" p.times[i] = "+Fm2(times[i]));
		pp.addPolygon(polyList[i].copy(), vlist[i], times[i]);
	}
	pp.mode = mode;
	pp.modeSet = modeSet;
	//fpln("$$$$$$$$ PolyPath.copy(): name = "+pp.getName()+"  DONE");
	return pp;
}

// builds a Plan based on this PolyPath
// this will probably change to an arraylist in the future
Triple<Plan,double,double> PolyPath::buildPlan() const {
	Plan p = Plan(label);
	if (size() < 1) return Triple<Plan,double,double>(p, 0.0, 0.0);

	double maxH = 0;
	double maxD = 0;
	for (int i = 0; i < (int) times.size(); i++) {
		SimplePoly poly = polyList[i];
		if (poly.size() < 2) {
			// TODO: needed for vstrat to function properly, check this
			return Triple<Plan,double,double>(p, 0.0, 0.0);
		}
		double time = times[i];
		NavPoint np = NavPoint(poly.boundingCircleCenter(),time);
		p.addNavPoint(np);
		if (std::abs(poly.getTop()-poly.getBottom()) > maxH) {
			maxH = std::abs(poly.getTop()-poly.getBottom());
		}
		if (poly.boundingCircleRadius() > maxD) {
			maxD = poly.boundingCircleRadius();
		}
	}

	// static poly or USER_VEL
	if (times.size() == 1) {
		SimplePoly poly = polyList[0];
		NavPoint np = NavPoint(poly.boundingCircleCenter(),DBL_MAX);
		if (isUserVel()) {
			if (vlist[0].isInvalid()) {
				error.addError("USER_VEL path has invalid velocity");
			} else {
				np = NavPoint(poly.boundingCircleCenter().linear(vlist[0], 36000), times[0]+36000);
			}
		}
		p.addNavPoint(np);
	}
	return Triple<Plan,double,double>(p, maxD, maxH/2);
}

bool PolyPath::isLatLon() const {
	return polyList.size() > 0 && polyList[0].isLatLon();
}

bool PolyPath::isStatic() const {
	return polyList.size() == 1 && !isUserVel();
}

bool PolyPath::isContinuing() const {
	return polyList.size() == 1 || mode == USER_VEL || mode == USER_VEL_EVER;
}

PolyPath::PathMode PolyPath::getPathMode() const {
	return mode;
}

bool PolyPath::isUserVel() const {
	return mode == USER_VEL || mode == USER_VEL_FINITE || mode == USER_VEL_EVER;
}


void PolyPath::setPathMode(PathMode m) {
	if (m == MORPHING) {
		for (int i = 1; i < size(); i++) {
			if (polyList[i].size() != polyList[0].size()) {
				fpln("PolyPathMode.setPathMode: "+getID()+": polygon "+Fm0(i)+" sizes are not consistent, cannot switch to MORPHING mode");
				error.addError("PolyPathMode.setPathMode: "+getID()+": polygon "+Fm0(i)+" sizes are not consistent, cannot switch to MORPHING mode");
				return;
			}
		}
	}

	bool wasUserVel = isUserVel();
	bool willBeUserVel = (m == USER_VEL || m == USER_VEL_FINITE || m == USER_VEL_EVER);

	// we need to fix the velocities if it did not used to be a userVel mode, and will be afterwards
	// other cases either keep the same stored velocities or ignore them

	if (willBeUserVel && !wasUserVel) {
		for (int i = 0; i < size(); i++) {
			vlist[i] = initialVelocity(i);
		}
	}

	mode = m;
}



string PolyPath::getID() const {
	return label;
}

void PolyPath::setID(const string& n) {
	label = n;
}

string PolyPath::getNote() const {
	return note;
}

void PolyPath::setNote(const string& n) {
	note = n;
}

int PolyPath::size() const {
	return polyList.size();
}


void PolyPath::remove(int n) {
	polyList.erase(polyList.begin()+n);
	times.erase(times.begin()+n);
}

double PolyPath::getFirstTime() const {
		if (times.size() > 0) {
			if (mode == USER_VEL_EVER || isStatic()) {
				return 0.0;
			} else {
				return times[0];
			}
		}
		error.addError("getFirstTime: empty path");
		return -1.0; // error
}

double PolyPath::getLastTime() const {
	if (isContinuing()) return std::numeric_limits<double>::infinity();
	if (times.size() > 1) return times[times.size()-1];
	return 0.0;
}

/**
 * Returns the time of the last polygon in the path
 */
double PolyPath::getLastPolyTime() const {
	if (times.size() > 1) return times[times.size()-1];
	return 0.0;
}


/**
 * Convert the polygons in this path so that all have the same number of vertices.
 *
 */
void PolyPath::makeVertexCountEqual() {
	int sz = 0;
	for (int i = 0; i < size(); i++) {
		sz = Util::max(sz, polyList[i].size());
	}
	for (int i = 0; i < size(); i++) {
		polyList[i].increaseVertices(sz);
	}
}



SimplePoly PolyPath::interpolate(double time) const {
	//fpln("PolyPath.interpolate: mode="+pathModeToString(mode)+" time="+Fm0(time));
	if (isStatic()) {
		return polyList[0];
	}

	if (times.size() == 0 || time < getFirstTime() || time > getLastTime()) {
		error.addError("interpolate: time out of bounds");
		return SimplePoly();
	}

	if (time == getLastTime()) {
		if (mode == USER_VEL_FINITE && size() > 1) {
			Velocity v = vlist[size()-2];
			double dt = time - times[size()-2];
			return polyList[size()-2].linear(v, dt);
		}
		return polyList[times.size()-1];
	}

	int i = 0;
	double t1 = times[0];
	double t2 = t1;
	for (int j = 0; j < (signed)times.size()-1; j++) {
		t1 = times[j];
		t2 = times[j+1];
		if (time == t1) return polyList[j];
		if (time == t2) return polyList[j+1];
		if (t1 < time && time < t2) {
			i = j;
			break;
		} else if (time > t2) {
			// fill in in case we go past the end
			i = j+1;
			t1 = t2;
		}
	}

	double dt = time-t1;

	SimplePoly poly1 = polyList[i];
	SimplePoly newPoly = SimplePoly();
	int sz = poly1.size();
	//fpln(" $$$ PolyPath::interpolate: poly1="+poly1.toString());
	if (mode == MORPHING) {
		SimplePoly poly2 = polyList[i+1];
		//fpln("1");
		for (int j = 0; j < sz; j++) {
			Position n1 = poly1.getVertex(j);
			Position n2 = poly2.getVertex(j);
			Velocity v = n1.initialVelocity(n2, t2-t1);
			Position p = n1.linear(v,dt);
			newPoly.add(p);
			newPoly.setBottom(poly1.getBottom()+v.vs()*dt);
			newPoly.setTop(poly1.getTop()+v.vs()*dt);
		}
	} else {
		//fpln("2");
		int seg = getSegment(time);
        //f.pln("PolyPath.interpolate "+name+" seg="+seg);
		Velocity v = vlist[seg];
		if (mode == AVG_VEL) {
			//fpln("3");
			SimplePoly poly2 = polyList[i+1];
			double vs = (poly2.getBottom() - poly1.getBottom())/(t2-t1);
			v = poly1.averagePoint().initialVelocity(poly2.averagePoint(), t2 - t1).mkVs(vs);
		}
		if (v.isZero()) {
			newPoly = poly1.copy();
		} else {
			newPoly = poly1.linear(v, dt);
		}
	}
	//fpln(" $$$ PolyPath::interpolate: newPoly="+newPoly.toString());
	return newPoly;
}

Velocity PolyPath::initialVelocity(int n)  {
	if (times.size() == 1 && !isUserVel()) {
		return Velocity::ZEROV();
	}
	if (n < 0  || n >= size()) {
		error.addError("velocity requested for out-of-bounds index!");
		return Velocity::INVALIDV();
	}
	if (isUserVel()) {
		return vlist[n];
	}

	if (n >=0 && n < (signed)polyList.size()-1) {
		Position n1 = polyList[n].averagePoint().mkAlt(polyList[n].getBottom());
		Position n2 = polyList[n+1].averagePoint().mkAlt(polyList[n+1].getBottom());
		return n1.initialVelocity(n2, times[n+1]-times[n]);
	}
	return Velocity::ZEROV();
}

Velocity PolyPath::initialVertexVelocity(int vert, int n)  {
	if (mode != MORPHING) {
		return initialVelocity(n);
	}
	if (times.size() == 1) {
		return Velocity::ZEROV();
	}
	if (n < 0  || n >= size()) {
		error.addError("vertex velocity requested for out-of-bounds index!");
		return Velocity::INVALIDV();
	}
	if (vert < 0  || vert >= polyList[0].size()) {
		error.addError("vertex velocity requested for out-of-bounds vertex!");
		return Velocity::INVALIDV();
	}
	if (n >= 0 && n < (int) polyList.size()-1) {
		Position n1 = Position(polyList[n].getVertex(vert));
		Position n2 = Position(polyList[n+1].getVertex(vert));
		double dt = times[n+1]-times[n];
		return n1.initialVelocity(n2, dt);
	}
	return Velocity::ZEROV();
}


int PolyPath::getIndex(double time) const {
	if (polyList.size() == 1 && mode != USER_VEL && mode != USER_VEL_FINITE) return 0;
	if (time > times[times.size()-1]) {
		return -(int)times.size()-1;
	}
	for (int i = 0; i < (signed)polyList.size(); i++) {
		if (times[i] == time) {
			return i;
		} else if (times[i] > time) {
			return -(i+1);
		}
	}
	return -1;
}


SimplePoly& PolyPath::getPolyRef(int n) {
	if (times.size() == 1) return polyList[0];
	if (n >=0 && n < (signed)times.size()) {
		return polyList[n];
	}
	error.addError("getPolyRef index out of bounds");
	return polyList[0];
}

SimplePoly PolyPath::getPoly(int n) const {
	if (times.size() == 1) return polyList[0];
	if (n >=0 && n < (signed)times.size()) {
		return polyList[n];
	}
	error.addError("getPoly index out of bounds");
	return polyList[0];
}

double PolyPath::getTime(int n) const {
	if (times.size() == 1) return times[0];
	if (n >=0 && n < (signed)times.size()) {
		return times[n];
	}
	return 0;
}

/**
 * Sets the time t for a given step n.  The order of times must be preserved.
 */
void PolyPath::setTime(int n, double t) {
	if (t >= 0 && n >= 0 && n < size()) {
		if ((n > 0 && n < size()-1 && t > times[n-1] && t < times[n+1]) ||
				(n > 0 && t > times[n-1]) || (n <size()-1 && t < times[n+1]) ||
				size() == 1) {
			times[n] = t;
		}
	}
}

/**
 * Set the polygon at index i to be a copy of p
 */
void PolyPath::setPolygon(int i, const SimplePoly& p) {
	if (i < 0 || i >= (int) (int) times.size()) {
		error.addError("setPolygon: index out of bounds");
	} else {
		polyList[i] = p;
	}
}

/**
 * Set the user velocity at index i to be v
 * This only has effect if isCalculatedVelocities() is false.
 */
void PolyPath::setVelocity(int i, const Velocity& v) {
	if (i < 0 || i >= (int) times.size()) {
		error.addError("setVelocity: index out of bounds");
	} else {
		vlist[i] = v;
	}
}


// returns true if the polypath follows expected behaviors (i.e. increasing times and same size for each poly)
// sets error messages if otherwise
bool PolyPath::validate() const {
	double lastTime = -100;
	if (polyList.size() != times.size()) {
		error.addError("PolyPath.validate "+label+": poly list different size from times list");
		return false;
	}
	if (polyList.size() != vlist.size()) {
		error.addError("PolyPath.validate "+label+": poly list "+Fm0((int)polyList.size())+" different size from velocity list "+Fm0((int)vlist.size()));
		return false;
	}
	for (int i = 0 ; i < (signed)polyList.size(); i++) {
		if (mode == MORPHING && polyList[0].size() != polyList[i].size()) {
			error.addError("PolyPath.validate "+label+": Mode MORPHING step "+Fm0(i)+" size != step 0 size");
			return false;
		}
		if (isUserVel() && vlist[i].isInvalid()) {
			error.addError("PolyPath.validate "+label+": Mode USER_VEL step "+Fm0(i)+" has invalid velocity");
		}
		if (times[i] < lastTime) {
			error.addError("PolyPath.validate "+label+": Times are not increasing from "+Fm4(lastTime)+" to "+Fm4(times[i])+" "+Fm0(i));
			return false;
		}
		lastTime = times[i];
		if (!polyList[i].validate(&error)) {
			error.addError("PolyPath.validate "+label+": Key polygon at time "+Fm1(lastTime)+" is invalid!");
			return false;
		}
	}
	return true;
}


PolyPath PolyPath::truncate(double t)  {
	if (t >= getLastTime()) return copy();
	PolyPath path = PolyPath(label);
	if (t <= getFirstTime()) return path;

	SimplePoly end = interpolate(t);
	for (int i = 0; i < size() && getTime(i) < t; i++) {
		if (isUserVel()) {
			path.addPolygon(getPoly(i), initialVelocity(i), getTime(i));
		} else {
			path.addPolygon(getPoly(i), getTime(i));
		}
	}
	if (isUserVel()) {
		path.addPolygon(end, velocity(t), t);
		path.setPathMode(USER_VEL_FINITE);
	} else {
		path.addPolygon(end, t);
		path.setPathMode(mode);
	}
	return path;
}

// this does not check poly sizes
void PolyPath::addVertex(const Position& p, double bottom, double top, double time) {
	int index = -1;
	for (int i = 0; i < (signed)polyList.size(); i++) {
		if (Util::almost_equals(times[i],time)) index = i;
	}

	SimplePoly poly = SimplePoly();
	if (index < 0) {
		index = 0;
		while(index < (signed)times.size() && times[index] < time) {
			index++;
		}
		polyList.insert(polyList.begin()+index,poly);
		times.insert(times.begin()+index,time);
		vlist.insert(vlist.begin()+index,Velocity::INVALIDV());
	}
	if (!polyList[index].add(p)) {
		error.addWarning("addPoint: attempted to add duplicate vertex, ignored");
	}
	polyList[index].setTop(top);
	polyList[index].setBottom(bottom);

}

void PolyPath::addVertex(int n, const Position& p) {
	SimplePoly poly = polyList[n];
	Position cent = poly.centroid();
	Velocity v = cent.initialVelocity(p, 100);
	for (int i = 0; i < size(); i++) {
		poly = polyList[i];
		Position pp = poly.centroid().linear(v, 100);
		if (!poly.add(pp)) {
			error.addWarning("addVertex: attempted to add duplicate vertex, ignored");
		}
	}
}


void PolyPath::removeVertex(int vert) {
	for (int i = 0; i < size(); i++) {
		polyList[i].remove(vert);
	}
}

//GUI function, unused in C++
void PolyPath::translate(Vect3 off) {
	for (int i = 0; i < (signed)polyList.size(); i++) {
		polyList[i].translate(off);
	}
}


void PolyPath::timeshift(double shift) {
	for (int i = 0; i < (signed)size(); i++) {
		times[i] = times[i]+shift;
	}
	while (times[0] < 0.0) {
		times.erase(times.begin());
		polyList.erase(polyList.begin());
		vlist.erase(vlist.begin());
	}
}

int PolyPath::getSegment(double tm) const {
	int i = getIndex(tm);
	if (i == -1) {
		if (mode == USER_VEL_EVER) return 0;
		return -1;
	}
	if (i >= 0) return i;
	if (-i-1 == size()) {
		if (!isContinuing()) {
			return -1;
		} else {
			return size()-1; // continuing use last segment
		}
	}
	return -i-2;
}

/**
 * Interpolate the poly at the given time
 * If time is outside the path's limit, return null.
 * @param time
 * @return
 */
SimplePoly PolyPath::position(double time) const {
	//fpln(" $$$$ PolyPath::position: time = "+Fm0(time)+" "+Fm0(getFirstTime())+" "+Fm0(getLastTime()));
	if (times.size() > 1 && (time < getFirstTime() || time > getLastTime())) {
		//fpln("position requested for out-of-bounds time!"+Fm0(time));
		error.addError("position requested for out-of-bounds time!");
		return SimplePoly();
	}

	return interpolate(time);
}



Velocity PolyPath::velocity(double time)  {
	if (isStatic()) {
		return Velocity::ZEROV();
	}
	if (time < getFirstTime() || time > getLastTime()) {
		error.addError("velocity requested for out-of-bounds time!");
		return Velocity::INVALIDV();
	}
	int i = getSegment(time);
	if (isUserVel()) {
		return vlist[i];
	}
	if (Util::within_epsilon(time, getLastTime(), Constants::get_time_accuracy())) {
		return finalVelocity(size()-2);
	}
	if (isLatLon()) {
		if (isContinuing()) {
			Velocity v = vlist[i];
			double dt = time-getTime(i);
			if (Util::within_epsilon(dt, Constants::get_time_accuracy())) {
				return v;
			}
			Position p1 = polyList[i].getVertex(0);
			Position p2 = p1.linear(v, dt);
			return Velocity(p2.initialVelocity(p1, dt).Neg());
		} else if (i < size()-1){
			Position p1 = polyList[i].averagePoint();
			Position p2 = polyList[i+1].averagePoint();
			double t1 = times[i];
			double t2 = times[i+1];
			Velocity v = p1.initialVelocity(p2, t2-t1);
			Position p3 = p1.linear(v, time-t1);
			return p3.initialVelocity(p2, t2-time);
		}
	}
	return initialVelocity(i);
}

Velocity PolyPath::vertexVelocity(int vert, double time)  {
	if (mode != MORPHING) {
		return velocity(time);
	}
	if (times.size() == 1) {
		return Velocity::ZEROV();
	}
	if (time < getFirstTime() || time > getLastTime()) {
		error.addError("vertex velocity requested for out-of-bounds time!");
		return Velocity::INVALIDV();
	}
	if (vert < 0 || vert >= getPoly(0).size()) {
		error.addError("vertex velocity requested for out-of-bounds index!");
		return Velocity::INVALIDV();
	}
	if (Util::within_epsilon(time, getLastTime(), Constants::get_time_accuracy())) {
		return finalVertexVelocity(vert, size()-2);
	}
	int i = getSegment(time);
	if (isLatLon()) {
		//		if (isContinuing()) {
		//			Velocity v = vlist[i];
		//			double dt = time-getTime(i);
		//			if (Util::within_epsilon(dt, Constants::get_time_accuracy())) {
		//				return v;
		//			}
		//			Position p1 = polyList[i].getVertex(vert);
		//			Position p2 = p1.linear(v, dt);
		//			return p2.initialVelocity(p1, dt).Neg();
		//		} else {
		Position p1 = polyList[i].getVertex(vert);
		Position p2 = polyList[i+1].getVertex(vert);
		double t1 = times[i];
		double t2 = times[i+1];
		Velocity v = p1.initialVelocity(p2, t2-t1);
		Position p3 = p1.linear(v, time-t1);
		return p3.initialVelocity(p2, t2-time);
		//		}
	} else {
		return initialVertexVelocity(vert,i);
	}
}


Velocity PolyPath::finalVelocity(int i)  {
	if (i < 0 || i >= size()-1) {
		error.addError("final velocity index out of bounds!");
		return Velocity::INVALIDV();
	}
	Position p1 = polyList[i].averagePoint();
	Position p2 = polyList[i+1].averagePoint();
	return Velocity::make(p2.initialVelocity(p1, times[i+1]-times[i]).Neg());
}

Velocity PolyPath::finalVertexVelocity(int vert, int i)  {
	if (mode != MORPHING) {
		return finalVelocity(i);
	}
	if (i < 0 || i >= size()-1) {
		error.addError("final vertex velocity index out of bounds!");
		return Velocity::INVALIDV();
	}
	if (vert < 0 || vert >= getPoly(0).size()) {
		error.addError("vertex velocity requested for out-of-bounds index!");
		return Velocity::INVALIDV();
	}
	Position p1 = polyList[i].getVertex(vert);
	Position p2 = polyList[i+1].getVertex(vert);
	return Velocity(p2.initialVelocity(p1, times[i+1]-times[i]).Neg());
}


MovingPolygon3D PolyPath::getInitialMovingPolygon(int i, const EuclideanProjection& proj)  {
	if (i < 0 || i >= size()-1 || times[i] >= getLastTime()) {
		error.addError("getMovingPolygon index out of bounds");
		return MovingPolygon3D();
	}
	Poly3D p3d = polyList[i].poly3D(proj);
	double dt = DBL_MAX;
	if (i < size()-1) {
		dt = times[i+1] - times[i];
	}
	if (mode == MORPHING) {
		std::vector<Velocity> vvlist;
		for (int j = 0; j < p3d.size(); j++) {
			vvlist.push_back(initialVertexVelocity(j,i));
		}
		return MovingPolygon3D(p3d,vvlist,dt);
	} else {
		Velocity v = initialVelocity(i);
		return MovingPolygon3D(p3d,v,dt);

	}
}

MovingPolygon3D PolyPath::getMovingPolygon(double time, const EuclideanProjection& proj)  {
	if (time < getFirstTime() || time >= getLastTime()) {
		error.addError("getMovingPolygon time out of bounds");
		return MovingPolygon3D();
	}
	Poly3D p3d = position(time).poly3D(proj);
	int seg = getSegment(time);
	double dt = DBL_MAX;
	if (seg < size()-1) {
		dt = getTime(seg+1) - time;
	}
	//fpln("PolyPath.getMovingPolygon "+Fm4(time)+" seg="+Fm0(seg)+" dt="+Fm4(dt)+" p3d="+p3d.toString());
	if (mode == MORPHING) {
		std::vector<Velocity> vvlist;
		for (int i = 0; i < p3d.size(); i++) {
			vvlist.push_back(vertexVelocity(i,time));
		}
		//fpln("PolyPath.getMovingPolygon p3d="+p3d.toString()+" v="+v.toString()+" dt="+Fm1(dt));
		return MovingPolygon3D(p3d,vvlist,dt);
	} else {
		Velocity v = velocity(time);
		return MovingPolygon3D(p3d,v,dt);
	}

}


bool PolyPath::isContainment() const {
	return containment;
}

void PolyPath::setContainment(bool b) {
	containment = b;
}



string PolyPath::toString()  {
	string s = "PolyPath: " + label +" mode="+pathModeToString(mode)+" size() = "+Fm0(size())+  "\n";
	for (int i = 0; i < (signed)size(); i++) {
		s = s + Fm2(times[i])+"s : "+polyList[i].toString() + " v="+initialVelocity(i).toString()+"\n";
	}
	return s;
}


string PolyPath::toStringShort() {
	string rtn = "PATH: size = "+Fm0(size())+"\n";
	for (int i = 0; i < (int) polyList.size(); i++) {
		SimplePoly sp = polyList[i];
		rtn = rtn + " "+Fm2(times[i])+" SimplePoly: bottom = "+Fm1(sp.getBottom())
				  +" top = "+Fm1(sp.getTop())+" sp.size = "+Fm0(sp.size())+"\n";
	}
	return rtn;
}




string PolyPath::pathModeToString(PathMode m) {
	if (m == AVG_VEL) return "AVG_VEL";
	if (m == USER_VEL) return "USER_VEL";
	if (m == USER_VEL_FINITE) return "USER_VEL_FINITE";
	if (m == USER_VEL_EVER) return "USER_VEL_EVER";
	return "MORPHING";
}

PolyPath::PathMode PolyPath::parsePathMode(const string& s) {
	if (equalsIgnoreCase(s,"AVG_VEL")) return AVG_VEL;
	if (equalsIgnoreCase(s,"USER_VEL")) return USER_VEL;
	if (equalsIgnoreCase(s,"USER_VEL_FINITE")) return USER_VEL_FINITE;
	if (equalsIgnoreCase(s,"USER_VEL_EVER")) return USER_VEL_EVER;
	return MORPHING;
}

// this outputs name, x y z1 z2 time fixed fields (or lat lon alt)
string PolyPath::toOutput()  {
	return toOutput(Constants::get_output_precision(), false);
}

string PolyPath::toOutput(int precision, bool tcpColumns)  {
	std::stringstream sb;
	for (int i = 0; i < (signed)times.size(); i++) {
		SimplePoly poly = polyList[i];
		for (int j = 0; j < poly.size(); j++) {
			std::vector<std::string> ret = toStringList(i,j,precision,0,tcpColumns);
			sb << list2str(ret, ", ") << endl;
		}
	}

	return sb.str();
}

std::vector<std::string> PolyPath::toStringList(int i, int j, int precision, int latLonExtraPrecision, bool tcpColumns)  {
	SimplePoly poly = polyList[i];
	std::vector<std::string> ret;
	ret.push_back(label);  // name is (0)
	std::vector<std::string> vertices = poly.getVertex(j).toStringList(precision,latLonExtraPrecision,false);
	ret.insert(ret.end(), vertices.begin(), vertices.end()); //vertex 1-3
	ret.push_back(FmPrecision(times[i],precision)); // time 4
	if (tcpColumns) {
		int start = 4;
		for (int k = start; k < TcpData::TCP_OUTPUT_COLUMNS; k++) {
			ret.push_back("-"); // label
		}
	} else {
		ret.push_back("-"); // label
	}
	ret.push_back(FmPrecision(Units::to("ft", poly.getTop()),precision));
	if (isUserVel()) {
		std::vector<std::string> vels = initialVelocity(i).toStringList();
		ret.insert(ret.end(), vels.begin(), vels.end()); // vel 6-8
	}
	return ret;
}

SimpleMovingPoly PolyPath::getSimpleMovingPoly(int i)  {
	SimplePoly poly = getPoly(i);
	if (mode == PolyPath::MORPHING) {
		std::vector<Velocity> vvlist;
		for (int j = 0; j < poly.size(); j++) {
			vvlist.push_back(initialVertexVelocity(j,i));
		}
		return SimpleMovingPoly(poly,vvlist);
	} else {
		return SimpleMovingPoly(poly,initialVelocity(i));
	}
}

SimpleMovingPoly PolyPath::getSimpleMovingPoly(double t)  {
	SimplePoly poly = interpolate(t);
	if (mode == PolyPath::MORPHING) {
		std::vector<Velocity> vvlist;
		for (int j = 0; j < poly.size(); j++) {
			vvlist.push_back(vertexVelocity(j,t));
		}
		return SimpleMovingPoly(poly,vvlist);
	} else {
		return SimpleMovingPoly(poly,velocity(t));
	}
}



bool PolyPath::contains (const Position& p, double t) const {
	return t >= getFirstTime() && t <= getLastTime() && position(t).contains(p);
}

bool PolyPath::contains2D (const Position& p, double t) const {
	return t >= getFirstTime() && t <= getLastTime() && position(t).contains2D(p);
}



BoundingRectangle PolyPath::getBoundingRectangle(double user_vel_time_extension)  {
	//fpln(" $$$ getBoundingRectangle: ENTER mode = "+pathModeToString(mode)+" size = "+Fm0(polyList.size()));
	BoundingRectangle br;
	if (polyList.size() == 0) return br;
	for (int i = 0; i < (int) polyList.size()-1; i++) {
		br.add(polyList[i].getBoundingRectangle());
	}
	if ((mode == USER_VEL || mode == USER_VEL_EVER) && !vlist[size()-1].isZero()) {
		// add last poly
		br.add(polyList[size()-1].getBoundingRectangle());
		// add poly 20 hours from now
		br.add(position(times[size()-1]+user_vel_time_extension).getBoundingRectangle());
	} else if (mode == USER_VEL_FINITE) {
		// do not add last poly
		br.add(position(getLastTime()).getBoundingRectangle());
	} else {
		// add last poly
		br.add(polyList[size()-1].getBoundingRectangle());
	}
	//fpln(" $$$ getBoundingRectangle: H");
	return br;
}

BoundingRectangle PolyPath::getBoundingRectangle()  {
    return getBoundingRectangle(72000);
}


} //namespace
