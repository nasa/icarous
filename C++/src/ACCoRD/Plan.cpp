
/*
 * Plan.cpp - -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 *  Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 *
 */

#include "Plan.h"
//#include "UnitSymbols.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "KinematicsPosition.h"
#include "PositionUtil.h"
#include "Kinematics.h"
#include "KinematicsDist.h"
#include "Velocity.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>
#include <float.h>
#include "PlanUtil.h"
#include "VectFuns.h"

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool Plan::debug = false;

const double Plan::minDt = GreatCircle::minDt;
//const double Plan::revertGsTurnConnectionTime = 5.0;


std::string Plan::specPre() {
	return "$";
}

Plan::Plan() : error("Plan") {
	name = "";
	note = "";
	init();
}

Plan::Plan(const std::string& name) : error("Plan") {
	this->name = name;
	this->note = "";
	init();
}

Plan::Plan(const std::string& name, const std::string& note) : error("Plan") {
	this->name = name;
	this->note = note;
	init();
}


void Plan::init() {
	error.setConsoleOutput(debug); // debug ON!
	errorLocation = -1;
}

Plan::Plan(const Plan& fp) : error("Plan") {
	points = fp.points;
	name = fp.name;
	note = fp.note;
	error = fp.error;
	errorLocation = fp.errorLocation;
	debug = fp.debug;
	bound = BoundingRectangle(fp.bound);
}

Plan::~Plan() {
	// no pointers to delete
}

bool Plan::operator ==(const Plan& o) const {
	Plan fp = (Plan) o;

	unsigned int numPts = points.size();
	if (numPts != fp.points.size()) {
		return false;
	}

	bool r = true;

	for (unsigned int j = 0; j < numPts; j++) {
		//r = r && points[j].equals(fp.points[j]);
		r = r && points[j] == fp.points[j];
	}
	return r && /*latlon == fp.latlon && */ equalsIgnoreCase(name,fp.name);
}

bool Plan::operator != (const Plan& fp) const {
	return !(*this == fp);
}

bool Plan::isLinear() const {
	for (int j = 0; j < (int) points.size(); j++) {
		if (points[j].isTCP()) return false;
	}
	return true;
}

bool Plan::equals(const Plan& fp){
	//if (!getClass().equals(fp.getClass())) return false;
	if (points.size() != fp.points.size()) {
		return false;
	}
	bool r = true;
	for (int j = 0; j < (int) points.size(); j++) {
		r = r && points[j].equals(fp.points[j]);
	}
	return r && (name == fp.name); // && type_p == fp.type_p; //  && numTCPs == fp.numTCPs;
}

bool Plan::almostEquals(const Plan& p) const {
	bool rtn = true;
	for (int i = 0; i < size(); i++) {                // Unchanged
		if (!point(i).almostEqualsPosition(p.point(i))) {
			rtn = false;
			//fpln("plans_almost_equal: point i = "+Fm0(i)+" does not match");
		}
		if (std::abs(getTime(i)-p.getTime(i)) > 0.0000001) {
			rtn = false;
			//fpln("almostEquals: time i = "+i+" does not match: "+getTime(i)+"  !=   "+p.getTime(i));
		}
		//		if ( point(i).label() != p.point(i).label()) {
		//			//fpln("almostEquals: point i = "+i+" labels do not match.");
		//			return false;
		//		}

	}
	return rtn;
}

void Plan::setDebug(bool d) {
	debug = d;
	error.setConsoleOutput(debug);
}
void Plan::setStaticDebug(bool d) {
	debug = d;
}

void Plan::clear() {
	points.clear();
	getMessage();
	bound.clear();
}

void Plan::clearVirtuals() {
	int i = 0;
	while (i < size()) {
		if (points[i].isVirtual()) {
			navPointIterator pai = points.begin();
			points.erase(pai+i);
		} else {
			i++;
		}
	}
}

int Plan::size() const {
	return points.size();
}

/** Get an approximation of the bounding rectangle around this plan */
BoundingRectangle Plan::getBound() const {
	return bound;
}


bool Plan::isLatLon() const {
	if (points.size() > 0) {
		return points[0].isLatLon();
	} else {
		return false;
	}
}

bool Plan::isLatLon(bool latlon) const {
	if (points.size() > 0) {
		return isLatLon() == latlon;
	} else {
		return true;
	}
}

const std::string& Plan::getName() const {
	return name;
}

void Plan::setName(const std::string& s) {
	name = s;
}

const std::string& Plan::getNote() const {
	return note;
}

void Plan::setNote(const std::string& s) {
	note = s;
}


double Plan::getFirstTime() const {
	if (size() == 0) {
		addError("getFirstTime: Empty plan", 0);
		return 0.0;
	}
	return points[0].time();
}

double Plan::getLastTime() const {
	if (size() == 0) {
		addError("getLastTime: Empty plan", 0);
		return 0.0;
	}
	return points[size()-1].time();
}

bool Plan::timeInPlan(double t) const {
	return getFirstTime() <= t && t <= getLastTime();
}


NavPoint Plan::getLastPoint() const {
	if (size() == 0) {
		addError("getLastTime: Empty plan", 0);
		return NavPoint::INVALID();
	}
	return points[size()-1];
}

double  Plan::getFirstRealTime() const {
	int i = 0;
	while (i < size()) {
		if (!points[i].isVirtual()) return points[i].time();
		i++;
	}
	addError("getFirstRealTime: all points are virtual or plan empty", 0);
	return 0.0;
}

int Plan::nextPtOrEnd(int startWp) const {
	int rtn = points.size()-1;
	if (startWp < (int)points.size()-1) rtn = startWp+1;
	return rtn;
}


int Plan::getIndex(double tm) const {
	int numPts = points.size();
	if (numPts == 0) {
		return -1;
	}
	return indexSearch(tm, 0, numPts-1);
}

int Plan::indexSearch(double tm, int i1, int i2) const {
	if (i1 > i2) return -i2-2;
	if (tm == points[i1].time()) return i1;
	if (tm == points[i2].time()) return i2;
	if (tm < points[i1].time()) return -i1-1;
	if (tm > points[i2].time()) return -i2-2;
	int mid = (i2-i1)/2 + i1;
	if (tm == points[mid].time()) return mid;
	if (tm < points[mid].time()) {
		return indexSearch(tm, i1+1, mid-1);
	} else {
		return indexSearch(tm, mid+1, i2-1);
	}
}


int Plan::getSegment(double tm) const {
	int i = getIndex(tm);
	if (i == -1) return -1;
	if (i >= 0) return i;
	if (-i-2 == size()-1) return -1;
	return -i-2;
}

int Plan::getSegmentByDistance(double d) const {
	if (d < 0) return -1;
	double tdist = 0;
	int i = 0;
	while (tdist <= d && i < size()) {
		tdist += pathDistance(i);
		i++;
	}
	if (tdist > d && i <= size()) return i-1; // on segment i-1
	if (Util::within_epsilon(d, tdist, 0.01) && i == size()) return size()-1;
	return -1; // not found
}

int Plan::getNearestIndex(double tm) const {
	int p = getIndex(tm);
	int numPts = points.size();
	if (p<0) {
		if (p <= -numPts-1) {
			p = numPts-1;
		} else if (p == -1) {
			p = 0;
		} else {
			double dt1 = std::abs(tm-points[-p-1].time());
			double dt2 = std::abs(tm-points[-p-2].time());
			if (dt2 < dt1)
				p = -p-2;
			else
				p = -p-1;
		}
	}
	return p;
}

double Plan::getTime(int i) const {
	if (i < 0 || i >= size()) {
		//      System.out.println("$$!! invalid point index of "+Fm0(i)+" in getTime()");
		addError("getTime: invalid point index of "+Fm0(i), i);
		return 0.0;
	}
	return points[i].time();
}

const NavPoint Plan::point(int i) const {
	if (i < 0 || i >= (int)points.size()) {
		addError("point: invalid index "+Fm0(i), i);
		return isLatLon() ? NavPoint::ZERO_LL() : NavPoint::ZERO_XYZ();
	}
	return points[i];
}

int Plan::add(const NavPoint& p) {
	// I don't believe (p == NULL) is possible in C++
	if (p.isInvalid()) {
		addError("add: Attempt to add invalid NavPoint",0);
		return -1;
	}
	int numPts = points.size();

	if (p.isInvalid()) {
		addError("add: Attempt to add invalid NavPoint",0);
		return -1;
	}
#ifndef _MSC_VER
	if (!std::isfinite(p.time())) {
		addError("add: Attempt to add infinite NavPoint",0);
		return -1;
	}
#endif
	if (p.time() < 0) {
		addError("Attempt to add a point at negative time, time = "+Fm4(p.time())+" in add()",0);
		return -numPts-1;
	}
	if ( ! isLatLon(p.isLatLon())) {
		addError("Attempted to add NavPoint with wrong geometry.",0);
		return -1;
	}
	if (p.isBeginTCP() && p.velocityInit().isInvalid()) {
		addError("add: attempted to add begin TCP with invalid velocity",0);
		fpln(" $$$$$ plan.add: attempted to add begin TCP with invalid velocity, time = "+Fm4(p.time()));
		return -1;
	}

	int i = getIndex(p.time());
	if (i >= 0) {
		//replace
		if ( ! point(i).isVirtual()) {
			if (point(i).mergeable(p)) {
				NavPoint np = point(i).mergeTCPInfo(p);
				points[i] = np;
			} else {
				addWarning("Attempt to merge a point at time "+Fm4(p.time())+" that already has an incompatible point, no point added.");
				//f.pln(" $$$$$ plan.add Attempt to add a point at time "+fm4(p.time())+" that already has an incompatible point. ");
				return -(int)points.size()-1;
			}
		} else { // just replace the virtual
			//f.pln(" $$ NavPoint.add: set at i = "+Fm0(i)+" p = "+p);
			points[i] = p;
		}
	} else {
		//insert
		i = -i-1;  // where the point should be inserted
		insert(i, p);
	}

	bound.add(p.position());

	return i;
}




// Insert a point at position i
// Warning: i must be a valid index: 0..numPts-1
void Plan::insert(int i, const NavPoint& v) {
	int numPts = points.size();
	if (i < 0 || i > numPts) {
		addError("insert: Invalid index "+Fm0(i),i);
		return;
	}

	//System.arraycopy(points,i,points,i+1,numPts-i);
	if (i >= numPts) {
		points.push_back(v);
	} else {
		navPointIterator pai = points.begin();
		points.insert(pai+i,v);
	}
}


void Plan::insertWithTimeshift(int i, const NavPoint& v) {
	double d0 = -1; // segment distance into point
	double d1 = -1; // segment distance out of point
	NavPoint v1 = v;
	NavPoint np0 = closestPoint(v1.position()); // closest point, if in accel zone
	//double dt0 = 0;
	bool in_Accel = false;
	bool onPlan = np0.distanceH(v) < Util::min(pathDistance()*0.001, 1.0);  // within 1 meter of plan (or 1/1000 of a short plan)
	if (i > 0) {
		d0 = point(i-1).distanceH(v);
		if (inAccel(getTime(i-1))) {
			in_Accel = true;
			d0 = pathDistance(i-1,np0.time(),false);
		}
	}
	if (i < size()) {
		d1 = v.distanceH(point(i));
		if (in_Accel && onPlan) {
			d1 = pathDistanceFromTime(np0.time(),i);
		}
	}
	if (d0 >= 0) {
		if (onPlan) {
			double d = d0+pathDistance(0,i-1);
			//			double dd = pathDistance();
			double t = timeFromDistance(d);
			v1 = v.makeTime(t);
		} else {
			double t = getTime(i-1) + d0/gsOut(i-1,true);
			v1 = v.makeTime(t);
		}
	}

	insert(i,v1);

	if (d1 >= 0) {
		if (onPlan) {
			double t1 = getTime(i+1);
			double dd = pathDistance(0,i+1);
			double t2 = timeFromDistance(dd);
			double dt = t1 - t2;
			timeshiftPlan(i+1,dt);
		} else {
			double dt = (v1.time() + d1/gsOut(i,true)) - getTime(i+1);
			timeshiftPlan(i+1,dt);
		}
	}
}


void Plan::remove(int i) {
	int numPts = points.size();
	if (i < 0 || i >= numPts) {
		addWarning("remove1: invalid index " +Fm0(i));
		return;
	}
	//System.arraycopy(points,i+1,points,i,numPts-i-1);
	navPointIterator pai = points.begin();
	points.erase(pai+i);

	numPts = numPts - 1;
}

void Plan::remove(int i, int j) {
	if (i < 0 || j < i || j >= (int)points.size()) {
		addWarning("remove2: invalid index(s) "+Fm0(i)+" "+Fm0(j));
		return;
	}
	//navPointIterator pai = points.begin();
	for (int k = j; k >= i; k--) {
		remove(k);
	}
}

int Plan::set(int i, const NavPoint& v) {
	if (i < 0 || i >= (int)points.size()) {
		addError("invalid index "+Fm0(i)+" in set()");
		return -1;
	}
	//std::cout << "set 1 " << toOutput(0,8,false,false) << endl;
	//std::cout << "set 2 " << v.toOutput(8,false) << endl;
	remove(i);
	//std::cout << "set 3 " << toOutput(0,8,false,false) << endl;
	//std::cout << "set 4 " << v.toOutput(8,false) << endl;
	return add(v);
}

void Plan::setTime(int i, double t) {
	if (i < 0 || i >= (int)points.size()) {
		addError("setTime: Invalid index "+Fm0(i),0);
		return;
	}
	if (t < 0) {
		addError("setTime: invalid time "+Fm4(t),i);
		return;
	}
	NavPoint tempv = points[i].makeTime(t);
	remove(i);
	add(tempv);
}


void Plan::setAlt(int i, double alt) {
	if (i < 0 || i >= (int)points.size()) {
		addError("setTime: Invalid index "+Fm0(i),i);
		return;
	}
	NavPoint tempv = points[i].mkAlt(alt);
	remove(i);
	add(tempv);
}

Plan Plan::copyAndTimeShift(int start, double st) {
	Plan newKPC = Plan(name);
	if (start >= size() || st == 0.0) return *this;
	for (int j = 0; j < start; j++) {
		newKPC.add(point(j));
		//fpln("0 add newKPC.point("+j+") = "+newKPC.point(j));
	}
	double ft = 0.0; // time of point before start
	if (start > 0) {
		ft = getTime(start-1);
	}
	for (int i = start; i < size(); i++) {
		double t = getTime(i)+st; // adjusted time for this point
		//fpln(">>>> timeshiftPlan: t = "+ t+" ft = "+ft);
		if (t > ft && t >= 0.0) {
			// double newSourceTime = point(i).sourceTime() + st;
			newKPC.add(point(i).makeTime(t)); // .makeSourceTime(newSourceTime));
			//fpln(" add newKPC.point("+Fm0(i)+") = "+newKPC.point(i));
		} else {
			//fpln(">>>> copyTimeShift: do not add i = "+Fm0(i));
		}
	}
	return newKPC;
}



bool Plan::timeshiftPlan(int start, double dt) {
	if (ISNAN(dt) || start < 0 ) {
		//if (debug) Debug.halt();
		return false;
	}
	if (start >= size() || dt == 0.0) return true;
	if (dt < 0) {
		double ft = -1.0;
		if (start > 0) {
			ft = getTime(start-1);
			//			if (getTime(start)+dt <= ft+Constants::get_time_accuracy()) {
			//                // f.pln("Plan.timeShiftPlan failed dt="+dt);
			//				return false;
			//			}

		}
		for (int i = start; i < size(); i++) {
//			if (preserveRTAs) { //  && point(i).isFixedTime()) {
//				break;
//			}
			double t = getTime(i)+dt;
			if (t > ft && t >= 0.0) {
				setTime(i,t);
			} else {
				remove(i);
				i--;
			}
		}
	} else {
//		int end = size()-1;
//		if (preserveRTAs) {
//			int i = start;
//			while (i < end) { // && !point(i+1).isFixedTime()) {
//				i++;
//			}
//			end = i;
//		}
		for (int i = size()-1; i >= start; i--) {
			setTime(i, getTime(i)+dt);
		}
	}
	return true;
}

//bool Plan:: timeshiftPlan(int start, double st) {
//	return timeshiftPlan(start,st,false);
//}


int Plan::prevTrkTCP(int current) const {
	if (current < 0 || current > size()) {
		addWarning("prevTrkTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isTrkTCP()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextTrkTCP(int current) const {
	if (current < 0 || current > size()-1) {
		addWarning("nextTrkTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < size(); j++) {
		if (points[j].isTrkTCP()) {
			return j;
		}
	}
	return -1;
}


int Plan::prevGsTCP(int current) const {
	if (current < 0 || current > size()) {
		addWarning("prevGsTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isGsTCP()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextGsTCP(int current) const {
	if (current < 0 || current > size()-1) {
		addWarning("nextGsTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < size(); j++) {
		if (points[j].isGsTCP()) {
			return j;
		}
	}
	return -1;
}



int Plan::prevVsTCP(int current) const {
	if (current < 0 || current > size()) {
		addWarning("prevVsTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isVsTCP()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextVsTCP(int current) const {
	if (current < 0 || current > size()-1) {
		addWarning("nextVsTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < size(); j++) {
		if (points[j].isVsTCP()) {
			return j;
		}
	}
	return -1;
}



int Plan::prevBOT(int current) const {
	//return prevTrkTCP(current);
	if (current < 0 || current > size()) {
		addWarning("prevBOT invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >= 0; j--) {
		//			if (points.get(j).tcp_trk==Trk_TCPType.BOT || points.get(j).tcp_trk==Trk_TCPType.EOTBOT ) {
		if (points[j].isBOT()) {
			return j;
		}
	}
	return -1;

}

int Plan::prevEOT(int current) const {
	//return prevTrkTCP(current);
	if (current < 0 || current > size()) {
		addWarning("prevEOT invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >= 0; j--) {
		//			if (points.get(j).tcp_trk==Trk_TCPType.BOT || points.get(j).tcp_trk==Trk_TCPType.EOTBOT ) {
		if (points[j].isEOT()) {
			return j;
		}
	}
	return -1;

}


int Plan::nextEOT(int current) const {
	//return nextTrkTCP(current,NavPoint::EOT,NavPoint::EOTBOT);
	if (current < 0 || current > size()-1) {
		addWarning("nextEOT invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isEOT()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextBOT(int current) const {
	//return nextTrkTCP(current,NavPoint::EOT,NavPoint::EOTBOT);
	if (current < 0 || current > size()-1) {
		addWarning("nextBOT invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isBOT()) {
			return j;
		}
	}
	return -1;
}


int Plan::prevBGS(int current) const {
	//return prevGsTCP(current,NavPoint::BGS,NavPoint::EGSBGS);
	if (current < 0 || current > size()) {
		addWarning("prevBGS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isBGS()) {
			return j;
		}
	}
	return -1;
}

int Plan::prevEGS(int current) const {
	//return prevGsTCP(current,NavPoint::BGS,NavPoint::EGSBGS);
	if (current < 0 || current > size()) {
		addWarning("prevEGS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isEGS()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextEGS(int current) const{
	//return nextGsTCP(current,NavPoint::EGS,NavPoint::EGSBGS);
	if (current < 0 || current > size()-1) {
		addWarning("nextEGS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isEGS()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextBGS(int current) const{
	//return nextGsTCP(current,NavPoint::EGS,NavPoint::EGSBGS);
	if (current < 0 || current > size()-1) {
		addWarning("nextBGS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isBGS()) {
			return j;
		}
	}
	return -1;
}


int Plan::prevBVS(int current) const {
	//return prevVsTCP(current,NavPoint::BVS,NavPoint::EVSBVS);
	if (current < 0 || current > size()) {
		addWarning("prevBVS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isBVS()) {
			return j;
		}
	}
	return -1;
}

int Plan::prevEVS(int current) const {
	//return prevVsTCP(current,NavPoint::BVS,NavPoint::EVSBVS);
	if (current < 0 || current > size()) {
		addWarning("prevEVS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		if (points[j].isEVS()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextEVS(int current) const {
	//return nextVsTCP(current,NavPoint::EVS,NavPoint::EVSBVS);
	if (current < 0 || current > size()-1) {
		addWarning("nextEVS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isEVS()) {
			return j;
		}
	}
	return -1;
}

int Plan::nextBVS(int current) const {
	//return nextVsTCP(current,NavPoint::EVS,NavPoint::EVSBVS);
	if (current < 0 || current > size()-1) {
		addWarning("nextBVS invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current+1; j < (int)points.size(); j++) {
		if (points[j].isBVS()) {
			return j;
		}
	}
	return -1;
}


int Plan::prevTCP(int current) const {
	if (current < 0 || current > size()) {
		addWarning("prevTCP invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >=0; j--) {
		//f.pln(" $$$$$$$$$$$$ prevTCP: points.get("+j+") = "+points.get(j).toStringFull());
		//			if (points.get(j).tcp_trk==Trk_TCPType.BOT || points.get(j).tcp_trk==Trk_TCPType.EOT || points.get(j).tcp_trk==Trk_TCPType.EOTBOT ) {
		if (points[j].isTrkTCP() || points[j].isGsTCP() || points[j].isVsTCP() ) {
			return j;
		}
	}
	return -1;
}



bool Plan::inTrkChange(double t) const { //fixed
	//return inTrkChange(getSegment(t));
	int i = getSegment(t);
	int j1 = prevBOT(i+1);
	int j2 = prevEOT(i+1);
	return j1 >= 0 && j1 >= j2;
}

bool Plan::inGsChange(double t) const { //fixed
	//return inGsChange(getSegment(t));
	int i = getSegment(t);
	int j1 = prevBGS(i+1);
	int j2 = prevEGS(i+1);
	return j1 >= 0 && j1 >= j2;
}


bool Plan::inVsChange(double t) const { //fixed
	//return inVsChange(getSegment(t));
	int i = getSegment(t);
	int j1 = prevBVS(i+1);
	int j2 = prevEVS(i+1);
	return j1 >= 0 && j1 >= j2;
}


bool Plan::inTrkChangeByDistance(double d) const { //fixed
	//return inTrkChange(getSegment(t));
	int i = getSegmentByDistance(d);
	int j1 = prevBOT(i+1);
	int j2 = prevEOT(i+1);
	return j1 >= 0 && j1 >= j2;
}


bool Plan::inGsChangeByDistance(double d) const { //fixed
	//return inGsChange(getSegment(t));
	int i = getSegmentByDistance(d);
	int j1 = prevBGS(i+1);
	int j2 = prevEGS(i+1);
	return j1 >= 0 && j1 >= j2;
}


bool Plan::inVsChangeByDistance(double d) const { //fixed
	//return inVsChange(getSegment(t));
	int i = getSegmentByDistance(d);
	int j1 = prevBVS(i+1);
	int j2 = prevEVS(i+1);
	return j1 >= 0 && j1 >= j2;
}

double Plan::turnRadiusAtTime(double t) const {
	if (inTrkChange(t)) {
		NavPoint bot = points[prevBOT(getSegment(t)+1)];//fixed
		//return bot.position().distanceH(bot.turnCenter());
		return bot.turnRadius();
	} else {
		return -1.0;
	}
}

double Plan::trkAccelAtTime(double t) const {
	if (inTrkChange(t)) {
		int b = prevBOT(getSegment(t)+1);//fixed
		return point(b).trkAccel();
	} else {
		return 0.0;
	}
}


double Plan::gsAccelAtTime(double t) const {
	if (inGsChange(t)) {
		int b = prevBGS(getSegment(t)+1);//fixed
		return point(b).gsAccel();
	} else {
		return 0.0;
	}
}
double Plan::vsAccelAtTime(double t) const {
	if (inVsChange(t)) {
		int b = prevBVS(getSegment(t)+1);//fixed
		return point(b).vsAccel();
	} else {
		return 0.0;
	}
}

//double Plan::calcVertAccel(int i) const {
//	if (i < 1 || i+1 > size()-1) return 0;
//	double acalc = (initialVelocity(i+1).vs() - finalVelocity(i-1).vs())/(point(i+1).time() - point(i).time());
//	if (point(i).isBVS()) {
//		double dt = getTime(nextEVS(i)) - point(i).time();//fixed
//		acalc = (initialVelocity(nextEVS(i)).vs() - finalVelocity(i-1).vs())/dt;//fixed
//	}
//	//fpln(" calcVertAccel:   acalc = "+acalc);
//	return acalc;
//}
//
//double Plan::calcGsAccel(int i) const {
//	if (i < 1 || i+1 > size()-1) return 0;
//	double acalc = (initialVelocity(i+1).gs() - finalVelocity(i-1).gs())/(point(i+1).time() - point(i).time());
//	//fpln(" calcVertAccel:   acalc = "+acalc);
//	return acalc;
//}
//
//Position Plan::positionOLD(double t) const {
//	return positionOLD(t, false);
//}
//
//Position Plan::positionOLD(double t, bool linear) const {
//	int seg = getSegment(t);
//	//fpln(" $$$$$ position: seg = "+Fm0(seg));
//	if (seg < 0) {
//		addError("position: time out of bounds "+Fm2(t)+" / "+Fm2(getFirstTime())+" to "+Fm2(getLastTime()),0);
//		return Position::INVALID();
//	}
//	NavPoint np1 = point(seg);
//	if (seg == size()-1 || np1.time() == t) {
//		//fpln(" $$$$$ position: return EARLY!"+points[seg).position());
//		return np1.position();
//	}
//	if (linear) {
//		double t1 = point(seg).time();
//		return point(seg).position().linear(initialVelocity(seg),t-t1);
//	}
//	return accelZone(Velocity::ZEROV(), t, t).first;
//}


// ******** EXPERIMENTAL ***************
Position Plan::position(double t, bool linear) const {
	return positionVelocity(t, linear).first;
}

// ******** EXPERIMENTAL ***************
Position Plan::position(double t) const{
	return positionVelocity(t, false).first;
}


/**  *********** EXPERIMENTAL **********
 * Estimate the initial velocity at the given time for this aircraft.
 * A time before the beginning of the plan returns a zero velocity.
 */
Velocity Plan::velocity(double tm, bool linear) const {
	return positionVelocity(tm, linear).second;
}

/**  *********** EXPERIMENTAL **********
 * Estimate the initial velocity at the given time for this aircraft.
 * A time before the beginning of the plan returns a zero velocity.
 */
Velocity Plan::velocity(double tm) const {
	return positionVelocity(tm, false).second;
}


/** EXPERIMENTAL
 * time required to cover distance "dist" if initial speed is "vo" and acceleration is "gsAccel"
 *
 * @param gsAccel
 * @param vo
 * @param dist
 * @return
 */
double Plan::timeFromGs(double vo, double gsAccel, double dist) {
	//double vo = initialVelocity(seg).gs();
	//double a = point(prevBGS(seg)).gsAccel();
	double t1 = Util::root(0.5*gsAccel, vo, -dist, 1);
	double t2 = Util::root(0.5*gsAccel, vo, -dist, -1);
	double dt = ISNAN(t1) || t1 < 0 ? t2 : (ISNAN(t2) || t2 < 0 ? t1 : Util::min(t1, t2));
	return dt;
}


// experimental - map path distance (in this segment) to relative time (in this segment)
// if there is a gs0=0 segment, return the time of the start of the segment
// return -1 on out of bounds input
double Plan::timeFromDistance(int seg, double rdist) const {
	if (seg < 0 || seg > size()-1 || rdist < 0 || rdist > pathDistance(seg)) return -1;
	double gs0 = initialVelocity(seg).gs();
	if (Util::almost_equals(gs0, 0.0)) return 0;
	if (inGsChange(getTime(seg))) {
		double a = point(prevBGS(seg+1)).gsAccel();//fixed
		return timeFromGs(gs0, a, rdist);
	} else {
		return rdist/gs0;
	}
}

// experimental -- map total path distance to absolute time
// return -1 on out of bounds input
double Plan::timeFromDistance(double dist) const {
	int seg = getSegmentByDistance(dist);
	if (seg < 0 || seg > size()-1) return -1;
	double dd = dist-pathDistance(0,seg);
	return timeFromDistance(seg,dd)+getTime(seg);
}


double Plan::trkOut(int seg, bool linear) const {
	//f.pln(" $$$ trkOut: ENTER seg = "+seg+" linear = "+linear);
	if (seg < 0 || seg > size()-1) {
		addError("trkOut: invalid index "+Fm0(seg), 0);
		return -1;
	}
	if (seg == size()-1) {
		return trkFinal(seg-1,linear);
	}
	Velocity vNew;
	if (inTrkChange(point(seg).time()) && ! linear) {
		int ixBOT = prevBOT(seg+1);
		double d = pathDistance(ixBOT,seg);
		Position center = point(ixBOT).turnCenter();
		double signedRadius = point(ixBOT).signedRadius();
		int dir = Util::sign(signedRadius);
		//f.pln(" $$$ trkOut (inTurn): d = "+Units.str("ft",d)+"  signedRadius = "+Units.str("ft",signedRadius)+" ixBOT = "+ixBOT);
		double gsAt_d = 1000.0; // not used here -- don't use 0.0 because will lose track info
		Position so = point(ixBOT).position();
		vNew = KinematicsPosition::turnByDist(so, center, dir, d, gsAt_d).second;
		//f.pln(" $$$ trkOut: vNew = "+vNew);
		return vNew.trk();
	} else {
		//NavPoint np2 = point(seg+1);
		//vNew = linearVelocityOut(seg);
		vNew = point(seg).initialVelocity(point(seg+1));
		//f.pln(" $$$ trkOut (linear): seg = "+seg+" vNew = "+vNew);
        return vNew.trk();
	}
}

double Plan::trkOut(int seg) const { return trkOut(seg,false); }


double Plan::trkFinal(int seg, bool linear) const {
	//f.pln("$$$ trkFinal: ENTER: seg = "+seg+" linear = "+linear);
	if (seg < 0 || seg >= size()-1) {
		addError("trkFinal: invalid index "+Fm0(seg), 0);
		return -1;
	}
	NavPoint np1 = point(seg);
	if (inTrkChange(point(seg).time()) && ! linear) {
		int ixBOT = prevBOT(seg+1);
		double d = pathDistance(ixBOT,seg+1);
		double signedRadius = point(ixBOT).signedRadius();
		int dir = Util::sign(signedRadius);
		Position center = point(ixBOT).turnCenter();
		//f.pln("$$$ trkFinal AA: d = "+Units.str("NM",d)+"  signedRadius = "+Units.str("ft",signedRadius)+" ixBOT = "+ixBOT);
		double gsAt_d = 1000.0; // not used here -- don't use 0.0 because will lose track info
		Position so = point(ixBOT).position();
		Velocity vFinal = KinematicsPosition::turnByDist(so, center, dir, d, gsAt_d).second;
		//f.pln("$$$ trkFinal AA: seg = "+seg+" vFinal = "+vFinal);
		return vFinal.trk();
	} else {
		if (isLatLon()) {
			//double d = pathDistance(seg,seg+1);
			double trk = GreatCircle::final_course(point(seg).lla(), point(seg+1).lla());
			//f.pln("$$$ trkFinal BB: seg = "+seg+" trk = "+Units.str("deg",trk));
			return trk;
		} else {
			Velocity vo = point(seg).finalVelocity(point(seg+1));
			//f.pln("$$$ trkFinal CC: seg = "+seg+" vo = "+vo);
			return vo.trk();
		}
	}
}


double Plan::gsOut(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("gsOut: invalid index "+Fm0(i), 0);
		return -1;
	}
	if (i == size()-1) return gsFinal(i-1);
	int j = i+1;
//	while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // collapse next point(s) if very close
//		j++;
//	}
	double dist = pathDistance(i,j,linear);
	double dt = getTime(j) - getTime(i);
	double a = 0.0;
	if (inGsChange(getTime(j-1)) && ! linear) {
		int ixBGS = prevBGS(i+1);//fixed
		a = point(ixBGS).gsAccel();
	}
	double rtn = dist/dt - 0.5*a*dt;
	//f.pln(" $$>>>>>> gsOut: rtn = "+Units.str("kn",rtn,8)+" a = "+a+" seg = "+seg+" dt = "+f.Fm4(dt)+" dist = "+Units.str("ft",dist));
	if (rtn <= 0) {
		//fpln(" ### gsOut: has encountered an ill-structured plan resulting in a negative ground speed!!");
		rtn = 0.000001;
	}
	return rtn;
}

double Plan::gsFinal(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("gsFinal: invalid index "+Fm0(i), 0);
		return -1;
	}
	int j = i+1;
//	while (i > 0 && getTime(j) - getTime(i) < minDt) { // collapse next point(s) if very close
//		i--;
//	}
	double dist = pathDistance(i,j,linear);
	double dt = getTime(j) - getTime(i);
	double a = 0.0;
	if (inGsChange(point(i).time()) && ! linear) {
		int ixBGS = prevBGS(i+1);//fixed
		a = point(ixBGS).gsAccel();
	}
	return dist/dt + 0.5*a*dt;
}

double Plan::gsIn(int seg, bool linear)  const {
	return gsFinal(seg-1,linear);
}

double Plan::gsOut(int seg)  const { return gsOut(seg,false); }
double Plan::gsFinal(int seg) const { return gsFinal(seg,false); }
double Plan::gsIn(int seg)  const { return gsIn(seg,false); }


//// ********** EXPERIMENTAL (towards turn/gs overlap) ****************
//double Plan::gsAtSeg(int seg, bool linear) const {
//	NavPoint np1 = point(seg);
//	double gs;
//	if (!linear && np1.isBeginTCP() && !np1.isBVS()) {    // TODO: do not use vertical begins -- is that best?
//		gs = np1.velocityInit().gs();
//        //fpln(" $$ gsAtSeg A: seg = "+Fm0(seg)+" gs = "+Units::str("kn",gs,8));
//	} else {
//		if (seg >= size()-1) {
//           gs = linearVelocityOut(size()-1).gs();
//           //fpln(" $$ gsAtSeg B: seg = "+Fm0(seg)+" gs = "+Units::str("kn",gs,8));
//		} else {
//			if (! linear && inGsChange(np1.time())) {
//				int ixBGS = prevBGS(seg);
//				double dist = pathDistance(ixBGS,seg);
//				double dt = point(seg).time() - point(ixBGS).time();
//				double gsAccel = point(ixBGS).gsAccel();
//				gs = dist/dt + 0.5*gsAccel*dt;
//               //fpln(" $$ gsAtSeg C: seg = "+Fm0(seg)+" gs = "+Units::str("kn",gs,8));
//			} else { // This may be inaccurate due to poor placement of vertical points in a turn
//				double dist = pathDistance(seg,linear);
//				double dt = point(seg+1).time() - point(seg).time();
//				gs = dist/dt;
//               //fpln(" $$ gsAtSeg D:seg = "+Fm0(seg)+" dt = "+Fm4(dt)+" dist = "+Units::str("ft",dist)+" gs = "+Units::str("kn",gs,8));
//			}
//		}
//	}
//    //fpln(" $$ gsAtSeg return: seg = "+Fm0(seg)+" gs = "+Units::str("kn",gs,4));
//	return gs;
//}

// ********** EXPERIMENTAL ****************
double Plan::gsAtTime(int seg, double gsAtSeg, double t, bool linear) const {
	//f.pln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units.str("kn",gsAt,8));
	double gs;
	if (!linear && inGsChange(t)) {
		double dt = t - getTime(seg);
		int ixBGS = prevBGS(seg+1);//fixed
		double gsAccel = point(ixBGS).gsAccel();
		gs = gsAtSeg + dt*gsAccel;
		//f.pln(" $$ gsAtTime A: gsAccel = "+gsAccel+" dt = "+f.Fm4(dt)+" seg = "+seg+" gs = "+Units.str("kn",gs,8));
	} else {
		gs = gsAtSeg;
		//f.pln(" $$ gsAtTime B: seg = "+seg+" gs = "+Units.str("kn",gs,8));
	}
	return gs;
}

double Plan::gsAtTime(double t, bool linear) const {
	double gs;
	int seg = getSegment(t);
	if (seg < 0) {
		gs = -1;
	} else {
		double gsSeg = gsOut(seg, linear);
		//f.pln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units.str("kn",gsAt,8));
		gs = gsAtTime(seg, gsSeg, t, linear);
	}
	return gs;
}



double Plan::vsOut(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("vsOut: invalid index "+Fm0(i), 0);
		return -1;
	}
	if (i == size()-1) return vsFinal(i-1);
	int j = i+1;
//	while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip next point(s) if very close
//		j++;
//	}
	double dist = point(j).alt() - point(i).alt();
	double dt = getTime(j) - getTime(i);
	double a = 0.0;
	if (inVsChange(getTime(j-1)) && ! linear) {  // use getTime(j-1) rather than getTime(i) in case j-1 point is an EGS
		int ixBVS = prevBVS(i+1);//fixed
		a = point(ixBVS).vsAccel();
	}
	double rtn = dist/dt - 0.5*a*dt;
	//f.pln(" $$>>>>>> vsOut: rtn = "+Units.str("fpm",rtn,8)+" a = "+a+" seg = "+seg+" dt = "+f.Fm4(dt)+" dist = "+Units.str("ft",dist));
	return rtn;
}

double Plan::vsFinal(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("vsFinal: invalid index "+Fm0(i), 0);
		return -1;
	}
	int j = i+1;
//	while (i > 0 && getTime(j) - getTime(i) < minDt) { // collapse next point(s) if very close
//		i--;
//	}
	double dist = point(j).alt() - point(i).alt();
	double dt = getTime(i+1) - getTime(i);
	double a = 0.0;
	if (inVsChange(point(i).time()) && ! linear) {
		int ixBvs = prevBVS(i+1);//fixed
		a = point(ixBvs).vsAccel();
	}
	return dist/dt + 0.5*a*dt;
}

double Plan::vsIn(int seg, bool linear)const {
	return vsFinal(seg-1,linear);
}

double Plan::vsOut(int seg) const { return vsOut(seg,false); }
double Plan::vsFinal(int seg) const { return vsFinal(seg,false); }
double Plan::vsIn(int seg) const { return vsIn(seg,false); }


//	Velocity VelocityOut(int seg, bool linear) {
//		return Velocity.mkTrkGsVs(trkOut(seg,linear),gsOut(seg,linear),vsOut(seg,linear));
//	}




double Plan::vsAtTime(int seg, double vsAtSeg, double t, bool linear) const {
	//f.pln(" $$ vsAtTime: seg = "+seg+" vsAt = "+Units.str("kn",vsAt,8));
	double vs;
	if (!linear && inVsChange(t)) {
		double dt = t - getTime(seg);
		int ixBvs = prevBVS(seg+1);//fixed
		double vsAccel = point(ixBvs).vsAccel();
		vs = vsAtSeg + dt*vsAccel;
		//f.pln(" $$ vsAtTime A: vsAccel = "+vsAccel+" dt = "+f.Fm4(dt)+" seg = "+seg+" vs = "+Units.str("kn",vs,8));
	} else {
		vs = vsAtSeg;
		//f.pln(" $$ vsAtTime B: seg = "+seg+" vs = "+Units.str("kn",vs,8));
	}
	return vs;
}

double Plan::vsAtTime(double t, bool linear) const {
	double vs;
	int seg = getSegment(t);
	if (seg < 0) {
		vs = -1;
	} else {
		double vsSeg = vsOut(seg, linear);
		//f.pln(" $$ vsAtTime: seg = "+seg+" vsSeg = "+Units.str("fpm",vsSeg,8));
		vs = vsAtTime(seg, vsSeg, t, linear);
	}
	return vs;
}

double Plan::vsAtTime(double t) const {
	bool linear = false;
	return vsAtTime(t, linear);
}

/** Calculate the distance from the Navpoint at "seq" to plan position at time "t"
 *
 * @param seg
 * @param t
 * @param linear
 * @return
 */
double Plan::distFromPointToTime(int seg, double t, bool linear) const {
	NavPoint np1 = point(seg);
	double distFromSo = 0;
	double gs0 = gsOut(seg,linear);
	double dt = t - np1.time();
	if (inGsChange(t) && ! linear) {
		double gsAccel = point(prevBGS(seg+1)).gsAccel();//fixed
		distFromSo = gs0*dt + 0.5*gsAccel*dt*dt;
		//f.pln(" $$$ positionVelocity(inGsChange A): dt = "+f.Fm2(dt)+" vo.gs() = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
	} else {
		distFromSo = gs0*dt;
		//f.pln(" $$$ positionVelocity(! inGsChange B): dt = "+f.Fm4(dt)+" gs0 = "+Units.str("kn",gs0)+" distFromSo = "+Units.str("ft",distFromSo));
	}
	return distFromSo;
}



/** Compute position and velocity at time t
 *
 * Note that the calculation proceeds in steps.  First, the 2D path is determined.  This gives a final position
 * and final track.   Then ground speed is computed.  Finally vertical speed is computed.
 *
 * @param t    time of interest
 * @return
 */
std::pair<Position,Velocity> Plan::positionVelocity(double t, bool linear) const {
	if (t < getFirstTime() || ISNAN(t) ) {
		return std::pair<Position,Velocity>(Position::INVALID(), Velocity::ZEROV());
	}
	int seg = getSegment(t);
	//f.pln("\n $$$$$ positionVelocity: ENTER t = "+t+" seg = "+seg);
	Position sNew;
	Velocity vNew;
	NavPoint np1 = point(seg);
	if (seg+1 > size()-1) {  // at Last Point
		Velocity v = finalVelocity(seg-1);
		//f.pln("\n -----size = "+size()+" seg = "+seg+"\n $$$ accelZone: np1 = "+np1+" v = "+v);
		return std::pair<Position,Velocity>(np1.position(),v);
	}
	NavPoint np2 = point(seg+1);
	//f.pln("\n ----------- seg = "+seg+"\n $$$ positionVelocity: np1 = "+np1);
	//f.pln(" $$$ positionVelocity: np2 = "+np2);
	double gs0 = gsOut(seg,linear);
	//double gsAt_d = gsAtTime(t,linear);    // TODO; improve speed by passing in gs0 into gsAtTime
	double gsAt_d = gsAtTime(seg, gs0, t, linear);
	//f.pln(" $$$ positionVelocity: seg = "+seg+" gs0 = "+Units.str("kn",gs0,4)+" gsAt_d = "+Units.str("kn",gsAt_d,4)); ;
	Position so = np1.position();
	//f.pln(t+" $$$ positionVelocity: seg = "+seg+" t = "+f.Fm2(t)+" positionVelocity: so = "+so+" gs0 = "+Units.str("kn",gs0));
	double distFromSo = distFromPointToTime(seg, t, linear);
	if (inTrkChange(t) & !linear) {
		int ixPrevBOT = prevBOT(seg+1);//fixed
		Position center = point(ixPrevBOT).turnCenter();
		double signedRadius = point(ixPrevBOT).signedRadius();
		//f.pln(t+" $$$ positionVelocity: signedRadius = "+Units.str("ft",signedRadius,4));
		int dir = Util::sign(signedRadius);
		std::pair<Position,Velocity> tAtd = KinematicsPosition::turnByDist(so, center, dir, distFromSo, gsAt_d);
		sNew = tAtd.first;
		vNew = tAtd.second;
		//fpln(" $$ %%%% positionVelocity A: vNew("+Fm2(t)+") = "+vNew.toString());
		//fpln(" $$ %%%% positionVelocity A: sNew("+Fm2(t)+") = "+sNew.toString());
	} else {
		Velocity vo = np1.initialVelocity(np2);
		//f.pln(" $$ %%%% positionVelocity B1: t = "+t+" seg = "+seg+"  distFromSo = "+Units.str("ft",distFromSo)+" np1 = "+np1);
		std::pair<Position,Velocity> pv = so.linearDist(vo, distFromSo);
		sNew = pv.first;
		vNew = pv.second.mkGs(gsAt_d);
		//fpln(" $$ %%%% positionVelocity B2: vNew("+Fm2(t)+") = "+vNew.toString());
	}
	if (inVsChange(t) & !linear) {
		NavPoint n1 = points[prevBVS(seg+1)];//fixed
		Position soP = n1.position();
		//double voPvs = n1.velocityInit().vs();
		double voPvs = vsAtTime(n1.time(),linear);
		std::pair<double,double> pv =  KinematicsPosition::vsAccelZonly(soP, voPvs, t-n1.time(), n1.vsAccel());
		sNew = sNew.mkAlt(pv.first);
		vNew = vNew.mkVs(pv.second);                   // merge Vertical VS with horizontal components
		//f.pln(t+" $$$ positionVelocity(inVsChange) C: vNew = "+vNew);
	} else {
		if (seg < size()-1) {   // otherwise np2 is not a valid future point
			double dt = t - np1.time();
			double vZ = (np2.z() - np1.z())/(np2.time()-np1.time());
			double sZ = np1.z() + vZ*dt;
			//f.pln(" $$$$$$$$ seg = "+seg+" dt = "+f.Fm2(dt)+" vZ = "+Units.str("fpm",vZ)+" sZ = "+Units.str("ft",sZ));
			sNew = sNew.mkAlt(sZ);
			vNew = vNew.mkVs(vZ);
		}
		//f.pln(t+" $$$ positionVelocity(NOT inVsChange): vNew = "+vNew);
	}
	//fpln(" $$ %%%% positionVelocity RETURN: sNew("+Fm2(t)+") = "+sNew.toString());
	//fpln(" $$ %%%% positionVelocity RETURN: vNew("+Fm2(t)+") = "+vNew.toString());
	return std::pair<Position,Velocity>(sNew,vNew);
}

std::pair<Position,Velocity> Plan::positionVelocity(double t) const {
	return positionVelocity(t,false);
}


//// v is the linear (estimate) velocity from point i (before t1) to point i+1 (after t1)
//// t1 is the time of interest
//// t2 is the time from which the velocity should be calculated (t1=t2 for initial/current velocity, t2=time of point i+1 for final velocity)
//std::pair<Position,Velocity> Plan::accelZone(const Velocity& v_linear, double t1, double t2) const {
//	int seg = getSegment(t1);
//	//fpln(" ENTER ##########>>>>>> accelZone  t1 = "+Fm2(t1)+" t2 = "+Fm2(t2)+"  seg = "+Fm0(seg));
//	NavPoint np1 = point(seg);
//	if (seg+1 > size()-1) {
//		fpln("accelZone: EARLY EXIT: size = "+Fm0(size())+" seg = "+Fm0(seg));
//		return std::pair<Position,Velocity>(np1.position(), v_linear);
//	}
//	NavPoint np2 = point(seg+1);
//	Position p = np1.position().linear(np1.initialVelocity(np2),t1-np1.time());
//	Velocity v = v_linear;
//	if (inTrkChange(t1)) {
//		NavPoint n1 = points[prevBOT(getSegment(t1))];
//		Position so = n1.position();
//		Velocity vo = n1.velocityIn();
//		//fpln(" $$##########>>>>>> accelZone:  seg = "+Fm0(seg)+" n1 = "+n1.toStringFull()+" dt = "+Fm2(t2-n1.time()));
//		std::pair<Position,Velocity> pv = KinematicsPosition::turnOmega(so, vo, t2-n1.time(), n1.trkAccel());
//		p = pv.first.mkAlt(p.alt()); // need to treat altitude separately
//		v = pv.second.mkVs(v_linear.vs());
//		//fpln(" $$########## finalVelocity: inTurn adjustment: seg = "+Fm0(seg)+" t1 = "+Fm2(t1)+" t2 = "+Fm2(t2)+" v = "+v.toString());
//	} else if (inGsChange(t1)) {
//		NavPoint n1 = points[prevBGS(getSegment(t1))];
//		Position so = n1.position();
//		Velocity vo = n1.velocityIn();
//		std::pair<Position,Velocity> pv = KinematicsPosition::gsAccel(so, vo, t2-n1.time(), n1.gsAccel());
//		p = pv.first.mkAlt(p.alt()); // need to treat altitude separately
//		v = pv.second.mkVs(v_linear.vs());
//		//fpln(" $$########## finalVelocity: inGSC adjustment: v = "+v.toString());
//	}
//	if (inVsChange(t1)) {
//		NavPoint n1 = points[prevBVS(getSegment(t1))];
//		Position so = n1.position();
//		Velocity vo = n1.velocityIn();
//		std::pair<Position,Velocity> pv =  KinematicsPosition::vsAccel(so, vo, t2-n1.time(), n1.vsAccel());
//		Velocity v2 = pv.second;
//		p = p.mkAlt(pv.first.alt());
//		v = v.mkVs(v2.vs());
//		//fpln(" $$########## finalVelocity: inVSC adjustment in seg = "+seg+" dt = "+(t2-n1.time())+" n1.accel() = "+n1.accel());
//		//fpln(" $$########## finalVelocity: inVSC adjustment in seg = "+seg+" FROM v = "+v+" TO v2 = "+v2);
//	}
//	//fpln(" $$$$ ########## accelZone:  RETURN v = "+v.toString());
//	return std::pair<Position,Velocity>(p,v);
//}
//
//
//Velocity Plan::velocityOLD(double tm) const {
//	return velocityOLD(tm, false);
//}
//
//Velocity Plan::velocityOLD(double tm, bool linear) const {
//	//fpln(" $$$$ Plan.initialVelocity: ENTER with tm = "+Fm0(tm));
//	if (tm < getFirstTime()) {
//		return Velocity::ZEROV();
//	}
//	if (tm > getLastTime()) {
//		addError("velocity: time "+Fm2(tm)+" is not in plan!", size()-1);
//		return Velocity::INVALIDV();
//	}
//	//fpln(" $$$$ ---------------------- Plan.velocity: ENTER velocityCalc Portion");
//	if (tm > getLastTime()-minDt) { // if almost at last time, move back a tiny bit
//		tm = getLastTime()-minDt;
//	}
//	int i = getSegment(tm);
//	int j = i+1;
//	NavPoint nextPt = points[j];
//	while (j < (int) points.size() && nextPt.time() - points[i].time() < minDt) {
//		nextPt = points[++j];
//	}
//	//  this generates an erroneous value if the time is very nearly to the next point
//	if (nextPt.time() - tm < minDt) {
//		tm = Util::max(getFirstTime(), nextPt.time() - 2.0*minDt);
//	}
//	NavPoint np = NavPoint(positionOLD(tm,linear), tm);
//	Velocity v = np.initialVelocity(nextPt);
//	//fpln("Plan i="+Fm0(i)+" np="+np.toString()+" nextPt="+nextPt.toString());
//	// also can erroneously produce a zero velocity if very near next point, use the final velocity then
//	if (v.isZero() && !nextPt.position().almostEquals(points[i].position())) {
//		v = finalVelocity(i);
//	}
//	//fpln(" $$$$ ----------------------------- Plan.velocity: BEFORE v = "+v.toString() );
//	if (linear) return v;
//	v = accelZone(v,tm,tm).second;
//	//fpln(" $$$$ ------------------------------Plan.velocity: AFTER v = "+v.toString() );
//	return v;
//}





Velocity Plan::averageVelocity(int i) const {
	//fpln(" $$$$$>>>>>>>>>>>>>>>>>>>... averageVelocity i = "+Fm0(i));
	if (i >= size()-1) {   // there is no velocity after the last point
		addWarning("averageVelocity(int): Attempt to get an averge velocity after the end of the Plan: "+Fm0(i));
		return Velocity::ZEROV();
	}
	if (i < 0) {
		addWarning("averageVelocity(int): Attempt to get an average velocity before beginning of the Plan: "+Fm0(i));
		return Velocity::ZEROV();
	}
	//	double t1 = points[i].time();
	//	double t2 = points[i].time();
	//	// this is the velocity at the halfway point between the two points
	//	return velocity((t2-t1)/2.0+t1);
	return points[i].averageVelocity(points[i+1]);
}



// estimate the velocity from point i to point i+1 (at point i).  If extend is true this may have a value at the last point, otherwise not.
Velocity Plan::initialVelocity(int i) const {
	return initialVelocity(i, false);
}

//Velocity Plan::initialVelocity(int i, bool linear) const {
//	//fpln("\n $$$$ Plan.initialVelocity: ENTER with i = "+Fm0(i)+" "+getName()+" linear = "+Fm0(linear));
//	Velocity rtn =  Velocity::ZEROV();
//	//	if (size() > 1 && i == size()-1) {
//	//		return finalVelocity(size()-2);
//	//	}
//	if (i > size()-1) {   // there is no velocity after the last point
//		addWarning("initialVelocity(int): Attempt to get an initial velocity after the end of the Plan: "+Fm0(i));
//		return Velocity::ZEROV();
//	}
//	if (i < 0) {
//		addWarning("initialVelocity(int): Attempt to get an initial velocity before beginning of the Plan: "+Fm0(i));
//		return Velocity::ZEROV();
//	}
//	if (size() == 1) {
//		return Velocity::ZEROV();
//	}
//	NavPoint np = points[i];
//	double t = np.time();
//	if (linear) {
//		//return np.initialVelocity(points[i+1]);
//		rtn = linearVelocityOut(i);
//		//fpln(Fm0(i)+" $$$$$ initialVelocity0: ******************* linear, rtn = "+rtn.toString());
//	} else {
//		if (np.isBOT()) { // || np.isBGS() || np.isBVS()) {
//			rtn = np.velocityInit();
//			//fpln(Fm0(i)+" $$$$$ initialVelocity0:  rtn = np.velocityIn("+Fm0(i)+") = "+ np.velocityIn().toString());
//		} else if (inTrkChange(t) || inGsChange(t) || inVsChange(t) || (np.isTCP() && i == (int) points.size()-1)) { // special case to also handle last point being in accel zone
//			rtn = velocity(t);
//			//fpln(Fm0(i)+" $$$$$ initialVelocity B:  rtn = velocity("+Fm2(t)+") = "+ rtn.toString());
//			//		} else if (i > 0 && i == points.size()-1) {
//			//			rtn = finalVelocity(i-1);
//		} else {
//			//rtn =  np.initialVelocity(points[i+1]);
//			rtn = linearVelocityOut(i);
//			//fpln(Fm0(i)+" $$$$$ initialVelocity2: rtn = getDtVelocity = "+ rtn.toString());
//		}
//	}
//	//fpln(" $$#### initialVelocity("+Fm0(i)+") rtn = "+rtn.toString());
//	return rtn;
//}

Velocity Plan::initialVelocity(int i, bool linear) const {
		return Velocity::mkTrkGsVs(trkOut(i,linear), gsOut(i,linear), vsOut(i,linear));
}

/** This function computes the velocity at point i in a strictly linear manner.  If i is an
 *  inner point it uses the next point to construct the tangent.  if the next point is less than
 *  dt ahead in time, then it finds the next point that is at least minDt ahead in time.
 *
 *  If it is called with the last point in the plan it looks backward for a point.   In this case
 *  it uses final velocity on the previous leg.
 *
 * @param i
 * @return initial velocity at point i
 */
Velocity Plan::linearVelocityOut(int i) const {
	Velocity rtn;
	int j = i+1;
	while (j < (int) size() && getTime(j) - getTime(i) < minDt) { // collapse next point(s) if very close
		j++;
	}
	if (j >= (int) size()) { // special case, back up a bit
		//Position p = position(getLastTime()-10*minDt);
		//return p.initialVelocity(points[size()-1].position(), 10*minDt);
		NavPoint lastPt = points[size()-1];
		while (i > 0 && lastPt.time() - points[i].time() < minDt) {
			i--;
		}
		NavPoint npi = points[i];
		return npi.finalVelocity(lastPt);
	}
	rtn = points[i].initialVelocity(points[j]);
	return rtn;
}

//Velocity Plan::finalLinearVelocity(int ii) const {
//	int i = ii;
//	Velocity v;
//	int j = i+1; //goal point for velocity in
//	while(i > 0 && points[j].time()-points[i].time() < minDt) {
//		i--;
//	}
//	if (i == 0 && points[j].time()-points[i].time() < minDt) {
//		while (j < size()-1 && points[j].time()-points[i].time() < minDt) {
//			j++;
//		}
//		v = points[i].initialVelocity(points[j]);
//	}
//	NavPoint np = points[i];
//	if (isLatLon()) {
//		LatLonAlt p1 = np.lla();
//		LatLonAlt p2 = point(j).lla();
//		double dt = points[j].time()-np.time();
//		v = GreatCircle::velocity_final(p1,p2,dt);
//		//f.pln(" $$$ finalLinearVelocity: p1 = "+p1+" p2 = "+p2+" dt = "+dt+" v = "+v);
//	} else {
//		v = np.initialVelocity(points[j]);
//	}
//	//f.pln(" $$ finalLinearVelocity: dt = "+(points.get(i+1).time()-points.get(i).time()));
//	return v;
//}

// estimate the velocity from point i to point i+1 (at point i+1).
Velocity Plan::finalVelocity(int i) const {
	return finalVelocity(i, false);
}


Velocity Plan::finalVelocity(int i, bool linear) const {
	if (i >= size()) {
		addWarning("finalVelocity(int): Attempt to get a final velocity after the end of the Plan: "+Fm0(i));
		//Debug.halt();
		return Velocity::INVALIDV();
	}
	if (i == (int) size()-1) {// || points[i].time() > getLastTime()-minDt) {
		addWarning("finalVelocity(int): Attempt to get a final velocity at end of the Plan: "+Fm0(i));
		return Velocity::ZEROV();
	}
	return Velocity::mkTrkGsVs(trkFinal(i,linear), gsFinal(i,linear), vsFinal(i,linear));
}

Velocity Plan::dtFinalVelocity(int i, bool linear) const {
	if (i >= (int) size()) {   // there is no "final" velocity after the last point (in general.  it happens to work for Euclidean, but not lla)
		addWarning("finalVelocity(int): Attempt to get a final velocity after the end of the Plan: "+Fm0(i));
		return Velocity::INVALIDV();
	}
	if (i == (int) size()-1) {// || points[i].time() > getLastTime()-minDt) {
		addWarning("finalVelocity(int): Attempt to get a final velocity at end of the Plan: "+Fm0(i));
		return Velocity::ZEROV();
	}
	if (i < 0) {
		addWarning("finalVelocity(int): Attempt to get a final velocity before beginning of the Plan: "+Fm0(i));
		return Velocity::ZEROV();
	}
	//fpln(" ########## finalVelocity0:   np1="+points[i]+" np2="+points[i+1]);
	double t2 = points[i+1].time();
	return positionVelocity(t2-2.0*minDt).second;
}


//double Plan::calcVertAccel(int i) {
//	if (i < 1 || i+1 > size()-1) return 0;
//	double acalc = (initialVelocity(i+1).vs() - finalVelocity(i-1).vs())/(point(i+1).time() - point(i).time());
//	if (point(i).isBVS()) {
//		double dt = getTime(nextEVS(i)) - point(i).time();
//		acalc = (initialVelocity(nextEVS(i)).vs() - finalVelocity(i-1).vs())/dt;
//
//	}
//	return acalc;
//}
//
//double Plan::calcGsAccel(int i) {
//	if (i < 1 || i+1 > size()-1) return 0;
//	double acalc = (initialVelocity(i+1).gs() - finalVelocity(i-1).gs())/(point(i+1).time() - point(i).time());
//	//f.pln(" calcGsAccel: $$$$ vin = "+finalVelocity(i-1)+" vout = "+initialVelocity(i+1)+" dt = "+(point(i+1).time() - point(i).time()));
//	return acalc;
//}

double Plan::calcTimeGSin(int i, double gs) const {
	if (i < 1 || i >= size()) {
		addError("calcTimeGSin: invalid index "+Fm0(i), 0); // must have a prev wPt
		return -1;
	}
	if (gs <= 0) {
		addError("calcTimeGSIn: invalid gs="+Fm2(gs), i);
		return -1;
	}
	if (inGsChange(points[i].time())) {
		double dist = pathDistance(i-1,i);
		Velocity v = initialVelocity(i-1);
		int ixBGS = prevBGS(i);
		double gsAccel = point(ixBGS).gsAccel();
		double dt = (gs - v.gs())/gsAccel;
		//fpln("#### calcTimeGSin: dist = "+Units::str("nm",dist)+" dt = "+dt);
		double acceldist = dt*(gs+v.gs())/2;
		if (acceldist > dist) {
			//fpln("#### calcTimeGSin: insufficient distance to achive new ground speed");
			addError("calcTimeGSin "+Fm0(i)+" insufficient distance to achieve new ground speed",i);
			return -1;
		}
		return points[i-1].time() + dt + (dist-acceldist)/gs;
	} else {
		double dist = pathDistance(i-1,i);
		double dt = dist/gs;
		//fpln("#### calcTimeGSin: dist = "+Units::str("nm",dist)+" dt = "+dt+" points[i-1).time() = "+points[i-1).time());
		return points[i-1].time() + dt;
	}
}

// return time needed at waypoint i in order for ground speed in to be gs
double Plan::linearCalcTimeGSin(int i, double gs) const {
	if (i > 0 && i < size()) {
		double d = point(i-1).distanceH(point(i));
		//fpln(" $$$$ calcTimeGSin: d = "+Units::str("nm",d));
		return point(i-1).time() + d/gs;
	} else {
		addError("linearCalcTimeGSin(2): index "+Fm0(i)+" out of bounds");
		return -1;
	}
}

void Plan::setTimeGSin(int i, double gs) {
	double newT = calcTimeGSin(i,gs);
	setTime(i,newT);
}


void Plan::setAltVSin(int i, double vs, bool preserve) {
	if (i <= 0)
		return;
	double dt = point(i).time() - point(i - 1).time();
	double newAlt = point(i - 1).alt() + dt * vs;
	NavPoint tempv = points[i].mkAlt(newAlt);
	if (preserve)
		tempv = tempv.makeAltPreserve();
	set(i, tempv);
}


/** change the ground speed into ix to be gs -- all other ground speeds remain the same
 *
 * @param p    Plan of interest
 * @param ix   index
 * @param gs   new ground speed
 * @return     revised plan
 */
void Plan::mkGsInto(int ix, double gs) {
	if (ix > size() - 1) return;
	double tmIx = calcTimeGSin(ix,gs);
	timeshiftPlan(ix,tmIx - point(ix).time());
}



/**
 *  Change the ground speed at ix to be gs -- all other ground speeds remain the same
 *  NOTE:  This assumes that there are no BVS - EVS segments in the area
 *
 *  Note: If point is a begin TCP, we need to update the velocityIn
 * @param p    Plan of interest
 * @param ix   index
 * @param gs   new ground speed
 * @return     revised plan
 */
void Plan::mkGsOut(int ix, double gs) {
	if (ix >= size() - 1) return;
	double dt = calcTimeGSin(ix+1,gs);
	//f.pln("\n $$ makeGsOut: ix = "+ix+" dt = "+f.Fm4(dt)+" gs = "+Units.str("kn",gs));
	timeshiftPlan(ix+1,dt-getTime(ix+1));
	NavPoint np = point(ix);
	if (np.isBeginTCP()) {
		Velocity vin = np.velocityInit();
		double vs_out = vsOut(ix);
		vin = Velocity::mkTrkGsVs(vin.trk(),gs,vs_out);
		NavPoint npNew = np.makeVelocityInit(vin);
		//f.pln(" $$ makeGsOut: vin = "+vin+" npNew = "+npNew.toStringFull());
		set(ix,npNew);
	}
}


bool Plan::isVelocityContinuous() const {
	for (int i = 0; i < size(); i++) {
		if (i > 0) {  //RWB
			if (point(i).isTCP()) {  //RWB
				if (!finalVelocity(i-1).compare(initialVelocity(i), Units::from("deg",10.0), Units::from("kn",20), Units::from("fpm",100))) { // see testAces3, testRandom for worst cases
					//fpln(" $$$ isConsistent: FAIL! continuity: finalVelocity("+(i-1)+") = "+finalVelocity(i-1).toString4NP()
					//		+" != initialVelocity("+Fm0(i)+") = "+initialVelocity(i).toString4NP());  //RWB
					return false;  //RWB
				} //RWB
			}
		}
	}
	return true;
}


double Plan::pathDistance() const {
	int i = 0;
	int j = size()-1;
	if (j <= 0) return 0;
	return pathDistance(i,j);
}

double Plan::pathDistance(int i) const {
	return pathDistance(i, false);
}


double Plan::pathDistance(int i, bool linear) const {
	if (i < 0 || i+1 >= size()) {
		return 0;
	}
	// if in a turn, figure the arc distance
	NavPoint p1 = points[i];
	double tt = p1.time();
	if (!linear && inTrkChange(tt)) {
		// if in a turn, figure the arc distance
		NavPoint p2 = points[i+1];
		NavPoint bot = points[prevBOT(i+1)];//fixed
		Position center = bot.turnCenter();
		double R = bot.turnRadius();
		double theta = PositionUtil::angle_between(p1.position(),center,p2.position());
		return std::abs(theta*R);
	} else {
		// otherwise just use linear distance
		return points[i].position().distanceH(points[i+1].position());
	}
}

double Plan::pathDistance(int i, int j) const {
	return pathDistance(i,j, false);
}

double Plan::pathDistance(int i, int j, bool linear) const {
	if (i < 0) {
		i = 0;
	}
	if (j > size()) {
		j = size();
	}
	double total = 0.0;
	for (int jj = i; jj < j; jj++) {
		total = total + pathDistance(jj, linear);
	}
	return total;
}

double Plan::partialPathDistance(double t, bool linear) const {
	if (t < getFirstTime() || t >= getLastTime()) {
		return 0.0;
	}
	int seg = getSegment(t);
	Position currentPosition = position(t);
	//f.pln("\n $$$ partialPathDistance: seg = "+seg);
	// if in a turn, figure the arc distance
	if (inTrkChange(t) && !linear) {
		NavPoint bot = point(prevBOT(getSegment(t)+1));//fixed
		double R = bot.turnRadius();
		Position center = bot.turnCenter();
		//double distAB = points.get(i).position().distanceH(points.get(i+1).position());
		//double alpha = 2*(Math.asin(distAB/(2*R)));
		//double dt = points.get(seg+1).time() - t;
		//double alpha = points.get(seg).trkAccel()*dt;
		double alpha = PositionUtil::angle_between(currentPosition,center,point(seg+1).position());
		//f.pln(" $$$$ alpha = "+Units.str("deg",alpha));
		double rtn = std::abs(alpha*R);
		//f.pln(" $$$$+++++ partialPathDistance:  rtn = "+Units.str("nm",rtn));
		return rtn ;
	} else {
		// otherwise just use linear distance
		double rtn =  position(t).distanceH(point(seg+1).position());
		//f.pln(" $$$$.... partialPathDistance: points.get(seg+1) = "+points.get(seg+1));
		//f.pln(" $$$$.... partialPathDistance: rtn = "+Units.str("nm",rtn));
		return rtn;
	}



}

/** calculate path distance from the current position at time t to point j
 *
 * @param t     current time
 * @param j     next point
 * @return      path distance
 */
double Plan::pathDistanceFromTime(double t, int j) const {
	int i = getSegment(t);
	double dist_i_j = pathDistance(i+1,j);
	double dist_part = partialPathDistance(t,false);
	if (j >= i) {
		return dist_i_j + dist_part;
	} else {
		return dist_i_j - dist_part;
	}
}

double Plan::vertDistance(int i) const {
	//fpln(" $$$$$$ pathDistance: i = "+Fm0(i)+" points = "+points);
	if (i < 0 || i+1 > size()) {
		return 0;
	}
	return points[i+1].position().z() - points[i].position().z();
}

/**
 * Find the cumulative horizontal (curved) path distance between points i and j [meters].
 */
double Plan::vertDistance(int i, int j) const {
	if (i < 0) {
		i = 0;
	}
	if (j > size()) {
		j = size();
	}
	double total = 0.0;
	for (int jj = i; jj < j; jj++) {
		//System.out.println("pathDistance "+jj+" "+legDist);
		total = total + vertDistance(jj);
		//System.out.println("jj="+jj+" total="+total);
	}
	return total;
}

double Plan::averageGroundSpeed() const {
	if (size() < 2) return 0.0;
	return pathDistance() / (getLastTime() - getFirstTime());
}


// calculate vertical speed from point i to point i+1 (at point i).
double Plan::verticalSpeed(int i) const {
	if (i < 0 || i >= size()-1) {   // there is no velocity after the last point
		addWarning("verticalSpeed: Attempt to get a vertical speed outside of the Plan: "+Fm0(i));
		return 0;
	}
	return points[i].verticalSpeed(points[i+1]);
}


void Plan::structRevertTurnTCP(int ix, bool addBackMidPoints, bool killNextGsTCPs, double zVertex) {
	//fpln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT ="+pln.point(ix).isBOT()+" "+killNextGsTCPs);
	//fpln(" $$$$$ structRevertTurnTCP: pln = "+pln);
	if (point(ix).isBOT()) {
		//fpln(" $$$$$ structRevertTurnTCP: ix = "+ix);
		NavPoint BOT = point(ix);
		int BOTlinIndex = BOT.linearIndex();
		double tBOT = BOT.time();
		int nextEOTix = nextEOT(ix);//fixed
		NavPoint EOT = point(nextEOTix);
		double tEOT = EOT.time();
		vector<NavPoint> betweenPoints = vector<NavPoint>(4);
		vector<double> betweenPointDists = vector<double>(4);
		if (addBackMidPoints) {  // add back mid points that are not TCPs
			for (int j = ix+1; j < nextEOTix; j++) {
				NavPoint np = point(j);
				if ( ! np.isTCP()) {
					//fpln(" >>>>> structRevertTurnTCP: SAVE MID point("+j+") = "+point(j).toStringFull());
					betweenPoints.push_back(np);
					double distance_j = pathDistance(ix,j);
					betweenPointDists.push_back(distance_j);
				}
			}
		}
		Velocity vin;
		if (ix == 0) {
			//vin = point(ix).velocityInit();     // not sure if we should allow TCP as current point ??
			vin = initialVelocity(0);
		}
		else vin = finalVelocity(ix-1);
		double gsin = vin.gs();
		//fpln(" $$$$ structRevertTurnTCP: gsin = "+Units::str("kn",gsin,8));
		Velocity vout = initialVelocity(nextEOTix);
		std::pair<Position,double> interSec = Position::intersection(BOT.position(), vin, EOT.position(), vout);
		double distToIntersec = interSec.first.distanceH(BOT.position());
		double tmInterSec = tBOT + distToIntersec/gsin;
		NavPoint vertex;
		if (tmInterSec >= tEOT) { // use BOT position, if vertex angle is too small
			 double tMid = (tBOT+tEOT)/2.0;
			 Position posMid = position(tMid);
			 //fpln(" $$$$ structRevertTurnTCP: use BOT position, if vertex angle is too small posMid = "+posMid);
		     vertex = NavPoint(posMid,tMid);
		} else {
			vertex = NavPoint(interSec.first,tmInterSec);
		}
		if (vertex.isInvalid()) {
			addError(" structRevertTurnTCP: reversion of point "+Fm0(ix)+" failed!");
			return;
		}
        if (zVertex >= 0) {
			vertex = vertex.mkAlt(zVertex);          // a better value for altitude obtained from previous internal BVS-EVS pair
		}
		// ======================== No Changes To Plan Before This Point ================================
		double gsInNext = vout.gs();
		if (killNextGsTCPs & (nextEOTix+1 < size())) {
			NavPoint npAfter = point(nextEOTix+1);
			//double dt = getTime(nextEOTix+1) - getTime(nextEOTix);
			//fpln(" $$$$ structRevertTurnTCP: npAfter = "+npAfter.toStringFull()+" dt = "+dt);
			//if (npAfter.isBGS() && dt < revertGsTurnConnectionTime) {  // note that makeKinematicPlan always makes it 1 sec after the EOT
			if (npAfter.isBGS() && (npAfter.linearIndex() == BOTlinIndex) ) {
			    int ixEGS = nextEGS(nextEOTix);//fixed
				vout = initialVelocity(ixEGS);
				gsInNext = vout.gs();
				//fpln(" $$$$$ let's KILL TWO GS points AT "+(nextEOTix+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
				//fpln(" $$$$$ KILL "+point(ixEGS).toStringFull());
				//fpln(" $$$$$ KILL "+point(nextEOTix+1).toStringFull());
				remove(ixEGS);
				remove(nextEOTix+1);
			}
		}
		// Kill all points between ix and nextEOTix
		// fpln(" $$$$$ structRevertTurnTCP: ix = "+ix+" nextEOTix = "+nextEOTix);
		for (int k = nextEOTix; k >= ix; k--) {
			//fpln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k).toStringFull());
			remove(k);
		}
		//fpln(" $$$$ structRevertTurnTCP: ADD vertex = "+vertex);
		int ixAdd = add(vertex);
		int ixNextPt = ixAdd+1;
		// add back all removed points with revised position and time
		if (addBackMidPoints) {
			for (int i = 0; i < (int) betweenPointDists.size(); i++) {
				double newTime = BOT.time() + betweenPointDists[i]/vin.gs();
				Position newPosition = position(newTime);
				NavPoint savePt = betweenPoints[i];
				NavPoint np = savePt.makePosition(newPosition).makeTime(newTime).mkAlt(savePt.alt());
				add(np);
				//fpln(" $$$$ structRevertTurnTCP: ADD BACK np = "+np);
				ixNextPt++;
			}
		}
		// fix ground speed after
		//fpln(" $$$$ structRevertTurnTCP: ixNextPt = "+ixNextPt+" gsInNext = "+Units::str("kn", gsInNext));
	    double tmNextSeg = getTime(ixNextPt);
	    if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
	    	int newNextSeg = getSegment(tmNextSeg);
	    	double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
	    	double dt2 = newNextSegTm - tmNextSeg;
	    	//fpln(" $$$$$$$$ structRevertTurnTCP: dt2 = "+dt2);
	    	timeshiftPlan(newNextSeg, dt2);
	    }
	    //fpln(" $$$$ structRevertTurnTCP: initialVelocity("+ixNextPt+") = "+initialVelocity(ixNextPt));
		removeRedundantPoints(getIndex(tBOT),getIndex(tEOT));
	}
}


// will not remove first or last point
void Plan::removeRedundantPoints(int from, int to) {
	double velEpsilon = 1.0;
	int ixLast = Util::min(size() - 2, to);
	int ixFirst = Util::max(1, from);
	for (int i = ixLast; i >= ixFirst; i--) {
		NavPoint p = point(i);
		Velocity vin = finalVelocity(i-1);
		Velocity vout = initialVelocity(i);
		if (!p.isTCP() && vin.within_epsilon(vout, velEpsilon)) { // 2.6)) { // see testAces3, testRandom for worst cases
            //fpln(" $$$$$ removeRedundantPoints: REMOVE i = "+Fm0(i));
			remove(i);
		}
	}
}

void Plan::removeRedundantPoints() {
     removeRedundantPoints(0,200000000);           // MAX_INT ??  MAXINTEGER ??
}


void Plan::structRevertGsTCP(int ix) {
	if (point(ix).isBGS()) {
		//NavPoint BGS = point(ix);
		int nextEGSix = nextEGS(ix); //fixed
		NavPoint BGS = point(ix);
		double gsOutEGS = gsOut(nextEGSix);
		NavPoint newPoint = BGS.makeNewPoint();
		remove(nextEGSix);
		remove(ix);
		int iNew = add(newPoint);
		if (iNew >= 0) {
			mkGsOut(iNew, gsOutEGS);
		}
		add(newPoint);
	}
	return; // ix;
}

// assumes ix > 0 AND ix < size()
double Plan::structRevertVsTCP(int ix) {
	if (point(ix).isBVS()) {
		NavPoint BVS = point(ix);
		int nextEVSix = nextEVS(ix);//fixed
//		if (nextEVSix == size()-1) {
//			fpln(" $$$$ structRevertVerticalTCP: ERROR EVS cannot be last point! ");
//		}
		NavPoint EVS = point(nextEVSix);
        NavPoint pp = point(ix-1);
		//NavPoint qq = point(nextEVSix+1);
		double vsin = (BVS.z() - pp.z())/(BVS.time() - pp.time());
		double dt = EVS.time() - BVS.time();
		double tVertex = BVS.time() + dt/2.0;
		double zVertex = BVS.z() + vsin*dt/2.0;
		Position pVertex = position(tVertex);
		NavPoint vertex = NavPoint(pVertex.mkAlt(zVertex),tVertex);
		remove(nextEVSix);
		remove(ix);
		add(vertex);
		return zVertex;
	}
	return -1;
}


/** revert all TCPS back to its original linear point which have the same sourceTime as the point at index i
 *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after dSeg to regain
 *  original ground speed into the point after dSeg.  This function checks to make sure that the source position
 *  is reasonably close to the current position.  If not, it reverts to the current position.
 *
 * @param dSeg  The index of one of the TCPs created together that should be reverted
 * @return index of the reverted point
 */
int Plan::revertGroupOfTCPs(int dSeg, bool checkSource) {
	double maxDistH = Units::from("nm", 15.0);
	double maxDistV = Units::from("ft", 5000.0);
	if (dSeg < 0 || dSeg >= size()) {
		addError(".. revertGroupOfTCPs: invalid index "+Fm0(dSeg), 0);
		return -1;
	}
	NavPoint origDsegPt = point(dSeg);
	if (!origDsegPt.isTCP()) {
		//fpln(" $$ revertGroupOfTCPs: point "+dSeg+" is not a TCP, do nothing!");
		return dSeg;
	}
	double sourceTm = origDsegPt.sourceTime();
	//int dSeg = getSegment(sourceTm);
	//fpln("\n $$$ revertGroupOfTCPs: point(dSeg).time = "+point(dSeg).time() +" sourceTm = "+sourceTm);
	int firstInGroup = -1;
	int lastInGroup = size()-1;
	for (int j = 0; j < size(); j++) {
		if (Constants::almost_equals_time(point(j).sourceTime(),sourceTm)) {
			if (firstInGroup == -1) firstInGroup = j;
			lastInGroup = j;
		}
	}
	//fpln("  $$$ revertGroupOfTCPs: firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
	double gsInFirst = finalVelocity(firstInGroup-1).gs();
	//fpln(" $$$ revertGroupOfTCPs: gsInFirst = "+Units::str("kn", gsInFirst));
	int nextSeg = lastInGroup+1;
	double tmNextSeg = getTime(nextSeg);
	//fpln(" $$$ revertGroupOfTCPs: size = "+size()+" nextSeg = "+nextSeg+" tmNextSeg = "+tmNextSeg);
	double gsInNext = finalVelocity(nextSeg-1).gs();
	NavPoint revertedLinearPt = NavPoint(point(dSeg).sourcePosition(),point(dSeg).sourceTime());
	int lastii = -1;
	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
		if (Constants::almost_equals_time(point(ii).sourceTime(),sourceTm)) {
			//fpln(" $$$ remove point ii = "+ii+" point(i).time() = "+point(ii).time()+" point(i).sourceTime() = "+point(ii).sourceTime());
			remove(ii);
			lastii = ii;
		}
	}
	// safety check in case there is a invalid source position
	double distH = origDsegPt.distanceH(revertedLinearPt);
	double distV = origDsegPt.distanceV(revertedLinearPt);
	if (checkSource && (distH > maxDistH || distV > maxDistV)) {
		//fpln(" $$$$ revertGroupOfTCPs: for dSeg = "+Fm0(dSeg)+" distH = "+Units::str("nm", distH));
		//fpln(" $$$$ revertGroupOfTCPs: for dSeg = "+Fm0(dSeg)+" distV = "+Units::str("ft", distV));
		revertedLinearPt = origDsegPt.makeNewPoint();
	}
	add(revertedLinearPt);
	// timeshift points
	if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
		int newNextSeg = getSegment(tmNextSeg);
		double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
		double dt2 = newNextSegTm - tmNextSeg;
		timeshiftPlan(newNextSeg, dt2);
	}
	// timeshift plan to regain original ground speed into the reverted point
	if (firstInGroup < lastInGroup) {
		int segNewLinearPt = getSegment(revertedLinearPt.time());
		double newTm = linearCalcTimeGSin(segNewLinearPt, gsInFirst);
		double dt = newTm - revertedLinearPt.time();
		//fpln(" $$$ revertGroupOfTCPs: TIMESHIFT dt = "+dt);
		timeshiftPlan(segNewLinearPt, dt);
	}
	//fpln(" $$$ revertGroupOfTCPs: lastii = "+lastii);
	return lastii;
}


bool Plan::isWellFormed() const {
	return indexWellFormed() < 0;
}



/**
 * This returns -1 if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and end point.
 * Returns a nonnegative value to indicate the problem point
 */
int Plan::indexWellFormed() const {
	double lastTm = -1;
	for (int i = 0; i < size(); i++) {
		NavPoint np = point(i);
		if (np.isBOT()) {
			int j1 = nextBOT(i);
			int j2 = nextEOT(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (np.isEOT()) {
			int j1 = prevBOT(i);
			int j2 = prevEOT(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		if (np.isBGS()) {
			int j1 = nextBGS(i);
			int j2 = nextEGS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (np.isEGS()) {
			int j1 = prevBGS(i);
			int j2 = prevEGS(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		if (np.isBVS()) {
			int j1 = nextBVS(i);
			int j2 = nextEVS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (np.isEVS()) {
			int j1 = prevBVS(i);
			int j2 = prevEVS(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		double tm_i = getTime(i);
		if (i > 0) {
			double dt = std::abs(tm_i-lastTm);
			if (dt < minDt) {
				fpln("$$ isWellFormed: Delta time into i = "+Fm0(i)+" is less than minDt = "+Fm8(minDt));
				return i;
			}
			lastTm = tm_i;
		}


	}
	return -1;
}


/**
 * This returns a string representing which part of the plan is not
 * "well formed", i.e. all acceleration zones have a matching beginning and end point.
 */
std::string Plan::strWellFormed() const {
	std::string rtn = "";
	for (int i = 0; i < size(); i++) {
		NavPoint np = point(i);
		// not well formed if GSC overlaps with other accel zones
		//			if ((np.isTurn() || np.isVSC()) && inGroundSpeedChange(np.time())) rtn = false;
		//			if (np.isGSC() && (inTurn(np.time()) || inVerticalSpeedChange(np.time()))) rtn = false;
		if (np.isBOT()) {
			int j1 = nextBOT(i);
			int j2 = nextEOT(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BOT at i "+Fm0(i)+" NOT FOLLOWED BY EOT!";
		}
		if (np.isEOT()) {
			int j1 = prevBOT(i);
			int j2 = prevEOT(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EOT at i "+Fm0(i)+" NOT PRECEEDED BY BOT!";
		}
		if (np.isBGS()) {
			int j1 = nextBGS(i);
			int j2 = nextEGS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BGS at i "+Fm0(i)+" NOT FOLLOWED BY EGS!";
		}
		if (np.isEGS()) {
			int j1 = prevBGS(i);
			int j2 = prevEGS(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EGS at i "+Fm0(i)+" NOT PRECEEDED BY BGS!";
		}
		if (np.isBVS()) {
			int j1 = nextBVS(i);
			int j2 = nextEVS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BVS at i "+Fm0(i)+" NOT FOLLOWED BY EVS!";
		}
		if (np.isEVS()) {
			int j1 = prevBVS(i);
			int j2 = prevEVS(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EVS at i "+Fm0(i)+" NOT PRECEEDED BY BVS!";
		}

		if (inGsChange(np.time()) && inTrkChange(np.time())) {
			rtn = rtn + "  Overlap FAIL at i = "+Fm0(i);
		}
		//			f.pln(" isWellFormed: i = "+i+" OK");e
	}
	return rtn;
}





///**
// * This returns true if the entire plan is "consistent", i.e. all acceleration zones have a matching beginning and end point.
// */
//bool Plan::isWellFormed() const {
//	//fpln(" isWellFormed: size() = "+size());
//	for (int i = 0; i < size(); i++) {
//		NavPoint np = point(i);
//		if (np.isBOT()) {
//			int j = nextTrkTCP(i);
//			if (j < 0 || !point(j).isEOT()) return false;
//		}
//		if (np.isEOT()) {
//			int j = prevTrkTCP(i);
//			if (j < 0 || !point(j).isBOT()) return false;
//		}
//		if (np.isBGS()) {
//			int j = nextGsTCP(i);
//			if (j < 0 || !point(j).isEGS()) return false;
//		}
//		if (np.isEGS()) {
//			int j = prevGsTCP(i);
//			if (j < 0 || !point(j).isBGS()) return false;
//		}
//		if (np.isBVS()) {
//			int j = nextVsTCP(i);
//			if (j < 0 || !point(j).isEVS()) return false;
//		}
//		if (np.isEVS()) {
//			int j = prevVsTCP(i);
//			if (j < 0 || !point(j).isBVS()) return false;
//		}
//		if (inGsChange(np.time()) && inTrkChange(np.time())) return false;
//	}
//	return true;
//}
//
/////**
//// * This returns true if the entire plan is "wellFormed", i.e. all acceleration zones have a matching beginning and end point.
//// */
//std::string Plan::strWellFormed() const {
//	//	//f.pln(" isWellFormed: size() = "+size());
//	std::string rtn = "";
//	for (int i = 0; i < size(); i++) {
//		NavPoint np = point(i);
//		// not well formed if GSC overlaps with other accel zones
//		//			if ((np.isTurn() || np.isVSC()) && inGroundSpeedChange(np.time())) rtn = false;
//		//			if (np.isGSC() && (inTurn(np.time()) || inVerticalSpeedChange(np.time()))) rtn = false;
//		if (np.isBOT()) {
//			int j = nextTrkTCP(i);
//			if (j < 0 || !point(j).isEOT()) return "BOT at i "+Fm0(i)+" NOT FOLLOWED BY EOT!";
//		}
//		if (np.isEOT()) {
//			int j = prevTrkTCP(i);
//			if (j < 0 || !point(j).isBOT()) return "EOT at i "+Fm0(i)+" NOT PRECEEDED BY BOT!";
//		}
//		if (np.isBGS()) {
//			int j = nextGsTCP(i);
//			if (j < 0 || !point(j).isEGS()) return "BGS at i "+Fm0(i)+" NOT FOLLOWED BY EGS!";
//		}
//		if (np.isEGS()) {
//			int j = prevGsTCP(i);
//			if (j < 0 || !point(j).isBGS()) return "EGS at i "+Fm0(i)+" NOT PRECEEDED BY BGS!";
//		}
//		if (np.isBVS()) {
//			int j = nextVsTCP(i);
//			if (j < 0 || !point(j).isEVS()) return "BVS at i "+Fm0(i)+" NOT FOLLOWED BY EVS!";
//		}
//		if (np.isEVS()) {
//			int j = prevVsTCP(i);
//			if (j < 0 || !point(j).isBVS()) return "EVS at i "+Fm0(i)+" NOT PRECEEDED BY BVS!";
//		}
//		//		if (inGsChange(np.time()) && inTrkChange(np.time())) {
//		//				rtn = rtn + "  Overlap FAIL at i = "+Fm0(i);
//		//		}
//		//			f.pln(" isWellFormed: i = "+i+" OK");e
//	}
//	return rtn;
//
//}



bool Plan::isConsistent() const {
	return isConsistent(false);
}


/**
 * This returns true if the entire plan is "sound"
 */
bool Plan::isConsistent(bool silent) const {
	//fpln(" $$$ isConsistent: ENTER type = "+Fm0(type_p));
	bool rtn = true;
	if (!isWellFormed()) {
		//fpln("  $$$ isConsistent: FAIL! not WellFormed!!");
		if ( ! silent) {
			fpln("  >>> isConsistent FAIL! not WellFormed!! "+strWellFormed());
		}
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if (point(i).isBGS()) {
			if ( ! PlanUtil::gsConsistent(*this, i, 0.00005, 0.07, silent))
				rtn = false;
		}
		if (point(i).isBVS()) {
			if ( ! PlanUtil::vsConsistent(*this, i, 0.00001, 0.00001, silent))
				rtn = false;
		}
		if (point(i).isBOT()) {
			bool useProjection = false;
			if ( ! PlanUtil::turnConsistent(*this, i, 0.5, 1.1, 1.2,  silent, useProjection))
				rtn = false;
		}
		if (i > 0) {
			if (point(i).isTCP()) {
				if (! PlanUtil::velocityContinuous(*this, i, 2.6, silent)) rtn = false;
			}
		}
	}
	//fpln("  $$$ isConsistent: rtn = "+rtn );
	return rtn;
}

/**
 * This returns true if the entire plan is "sound"
 */
bool Plan::isWeakConsistent(bool silent, bool useProjection) const {
	//fpln(" $$$ isConsistent: ENTER type = "+Fm0(type_p));
	bool rtn = true;
	if (!isWellFormed()) {
		error.addError("isWeakConsistent: not well formed");
		//fpln("  $$$ isConsistent: FAIL! not WellFormed!!");
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if (point(i).isBGS()) {
			if ( ! PlanUtil::gsConsistent(*this, i, 0.2, 0.1, silent)){
				error.addWarning("isWeakConsistent: GS "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
		if (point(i).isBVS()) {
			if ( ! PlanUtil::vsConsistent(*this, i, 0.001, 0.05, silent)) {
				error.addWarning("isWeakConsistent: VS "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
		if (point(i).isBOT()) {
			if ( ! PlanUtil::turnConsistent(*this, i, 0.1, 0.5, 1.2,  silent, useProjection)) {
				error.addWarning("isWeakConsistent: turn "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
		if (i > 0) {
			if (point(i).isTCP()) {
				if (! PlanUtil::velocityContinuous(*this, i, 5.0, silent)) {
					error.addWarning("isWeakConsistent: velocity "+Fm0(i)+" not consistent");
					rtn = false;
				}
			}
		}
	}
	//fpln("  $$$ isConsistent: rtn = "+rtn );
	return rtn;
}

bool Plan::isWeakConsistent() const {
	bool silent = false;
	bool useProjection = false;
	return isWeakConsistent(silent,useProjection);
}

void Plan::fix() {
	if (!isWellFormed()) {
		fpln(" Plan.fix has not been ported yet -- used only in Watch!");
		//		for (int i = 0; i < size(); i++) {
		//			NavPoint np = point(i);
		//			if ((np.isBGS() && nextEGS(i) < 0) ||
		//					(np.isEGS() && prev_BGS(np.time()) < 0) ||
		//					(np.isBOT() && nextEOT(i) < 0) ||
		//					(np.isEOT() && prev_BOT(np.time()) < 0) ||
		//					(np.isBVS() && nextEVS(i) < 0) ||
		//					(np.isEVS() && prev_BVS(np.time()) < 0)
		//			)
		//			{
		//				set(i, np.makeTCPClear());
		//			}
		//		}
	}
}


// experimental
NavPoint Plan::closestPointHoriz(int seg, const Position& p) {
	if (seg < 0 || seg >= size()-1) {
		addError("closestPositionHoriz: invalid index");
		return NavPoint::INVALID();
	}
	double d = 0;
	double t1 = getTime(seg);
	double dt = getTime(seg+1)-t1;
	NavPoint np = points[seg];
	NavPoint np2 = points[seg+1];
	if (pathDistance(seg) <= 0.0) {
		//fpln("closestPositionHoriz: patDistance for seg "+seg+" is zero");
		//fpln(toString());
		return np;
	}
	NavPoint ret;
	// vertical case is special
	if (Util::almost_equals(initialVelocity(seg).gs(), 0.0) && Util::almost_equals(initialVelocity(seg+1).gs(), 0.0)) {
		if ((p.alt() <= np.alt() && np.alt() <= np2.alt()) || (p.alt() >= np.alt() && np.alt() >= np2.alt())) {
			ret = np;
		} else if ((p.alt() <= np2.alt() && np2.alt() <= np.alt()) || (p.alt() >= np2.alt() && np2.alt() >= np.alt())) {
			ret = np2;
		} else if (inVsChange(t1)) {
			double vs1 = initialVelocity(seg).vs();
			double a = point(prevBVS(seg+1)).vsAccel();
			double tm = KinematicsDist::gsTimeConstantAccelFromDist(vs1, a, p.alt()-np.alt());
			ret = NavPoint(position(tm),tm);
		} else {
			double vtot = std::abs(np.alt()-np2.alt());
			double frac = std::abs(np.alt()-p.alt())/vtot;
			double tm = dt*frac-t1;
			ret = NavPoint(position(tm),tm);
		}
	} else if (inTrkChange(t1)) {
		NavPoint bot = points[prevBOT(getSegment(t1)+1)];//fixed
		Position center = bot.turnCenter();
		double endD = pathDistance(seg);
		double d2 = ProjectedKinematics::closestDistOnTurn(np.position(), initialVelocity(seg), bot.turnRadius(), Util::sign(bot.signedRadius()), center, p, endD);
		if (Util::almost_equals(d2, 0.0)) {
			ret = np;
		} else if (Util::almost_equals(d2, endD)) {
			ret = np2;
		} else {
			double segDt = timeFromDistance(seg, d2);
			ret = NavPoint(position(t1+segDt), t1+segDt);
		}

	} else if (isLatLon()) {
		LatLonAlt lla = GreatCircle::closest_point_segment(points[seg].lla(), points[seg+1].lla(), p.lla());
		d = GreatCircle::distance(points[seg].lla(), lla);
		double segDt = timeFromDistance(seg, d);
//		double frac = d/pathDistance(seg);
//		ret = NavPoint(Position(lla), t1 + frac*dt);
		ret = NavPoint(Position(lla), t1+ segDt);
	} else {
		Vect3 cp = VectFuns::closestPointOnSegment(points[seg].point(), points[seg+1].point(), p.point());
		d = points[seg].point().distanceH(cp);
		double segDt = timeFromDistance(seg, d);
//		double frac = d/pathDistance(seg);
//		ret = NavPoint(position(t1 + frac*dt), t1 + frac*dt);
		ret = NavPoint(Position(cp), t1+ segDt);
	}
	//	if (inGsChange(t1)) {
	//		NavPoint bgsc = points[prevBGS(getSegment(t1))];
	//		double t2 = Kinematics::gsAccelToDist(initialVelocity(seg).gs(), np.distanceH(ret), bgsc.gsAccel()).second;
	//		if (t2 >= 0) {
	//			ret = ret.makeTime(np.time()+t2);
	//		}
	//	}
	return ret;
}


NavPoint Plan::closestPoint(const Position& p) {
	return closestPoint(0,points.size()-1, p, true, 0.0);
}

NavPoint Plan::closestPointHoriz(const Position& p) {
	return closestPoint(0,points.size()-1, p, false, Units::from("ft", 100));
}

NavPoint Plan::closestPoint(int start, int end, const Position& p, bool horizOnly, double maxHdist) {
	double hdist = DBL_MAX;
	double vdist = DBL_MAX;
	NavPoint closest = NavPoint::INVALID();
	for (int i = start; i < end; i++) {
		NavPoint np = closestPointHoriz(i,p);
		double dh = np.position().distanceH(p);
		double dv = np.position().distanceV(p);
		if (dh < hdist || (!horizOnly && Util::within_epsilon(dh, hdist, maxHdist) && dv < vdist)) {
			hdist = dh;
			vdist = dv;
			closest = np;
		}
	}
	if (getFirstTime() > closest.time()) {
		fpln("closestPoint(iip)");
	}
	return closest;
}



bool Plan::inAccel(double t) const {
	return  inTrkChange(t) || inGsChange(t) || inVsChange(t);
}

bool Plan::inAccelZone(int ix) const {
	return point(ix).isTCP() || inAccel(getTime(ix));
}



Plan Plan::planFromState(const std::string& id, const Position& pos, const Velocity& v, double startTime, double endTime) {
	Plan p(id);
	if (endTime <= startTime) {
		return p;
	}
	NavPoint np = NavPoint(pos, startTime);
	//np = np.makeMutability(NavPoint::Fixed, NavPoint::Fixed, NavPoint::Fixed);
	p.add(np);
	p.add(np.linear(v, endTime-startTime));
	return p;
}

Plan Plan::copyWithIndex() const {
	Plan lpc = Plan(name,note);
	for (int j = 0; j < size(); j++) {
		lpc.add(point(j).makeLinearIndex(j));
	}
	return lpc;
}


Plan Plan::cut(int firstIx, int lastIx) const {
	Plan lpc = Plan(name,note);
	for (int i = firstIx; i <= lastIx; i++) {
		NavPoint np = point(i);
		lpc.add(np);
	}
	return lpc;
}


void  Plan::mergeClosePoints(double minDt) {
	//f.pln(" $$$$$$ mergeClosePoints "+getName()+" minDt  = "+minDt);
	for (int i = size()-2; i >= 0; i--) {
		if (minDt > 0) {
			NavPoint npi = point(i);
			NavPoint npip1 = point(i+1);
			double ti = npi.time();
			double tip1 = npip1.time();
			if (tip1-ti < minDt) {
				int ixDelete = i+1;
				if (i == size()-1) ixDelete = i;
				else if (i == 0) ixDelete = 1;
				else if (!npi.isBeginTCP() && npip1.isBeginTCP()) ixDelete = i;
				// save attributes of "ixDelete"
				NavPoint npDelete = point(ixDelete);
				remove(ixDelete);
				// the index of the remaining point is "i"
				NavPoint newNpi = point(i).mergeTCPInfo(npDelete);
				set(i,newNpi);
				//f.pln(" $$$$$ mergeClosePoints: DELETE point ixDelete = "+ixDelete);
			}
		}
		//NavPoint npi = point(i);
		//set(i,npi.makeLinearIndex(i));
	}

}



std::string Plan::toString() const {
	std::ostringstream sb;
	//sb << planTypeName() << " Plan for aircraft: ";
	sb << " Plan for aircraft: ";

	sb << (name);
	if (error.hasMessage()) {
		sb << " error.message = " << error.getMessageNoClear();
	}
	sb << endl;
	if (note.length() > 0) {
		sb << "Note=" << note << endl;
	}

	if (size() == 0) {
		sb << ("<empty>");
	} else {
		for (int j = 0; j < size(); j++) {
			sb << "Waypoint "+Fm0(j)+": " << (point(j).toStringFull());
			sb << endl;
		}
	}
	return sb.str();
}


std::string Plan::getOutputHeader(bool tcpcolumns) const {
	std::string s = "";
	s += "Name, ";
	if (isLatLon()) {
		s += "Lat, Lon, Alt";
	} else {
		s += "SX SY SZ";
	}
	s += ", Time, ";
	if (tcpcolumns) {
		s += "type, trk, gs, vs, tcp_trk, accel_trk, tcp_gs, accel_gs, tcp_vs, accel_vs, radius, ";
		if (isLatLon()) {
			s += "src_lat, src_lon, src_alt, ";
		} else {
			s += "src_x, src_y, src_z, ";
		}
		s += "src_time, ";
	}
	s += "Label";
	return s;
}

std::string Plan::toOutput() const {
	return toOutput(0, Constants::get_output_precision(), true, false);
}

std::string Plan::toOutputMin() const {
	return toOutput(0, Constants::get_output_precision(), false, false);
}

std::string Plan::toOutput(int extraspace, int precision, bool tcpColumns, bool includeVirtuals) const {
	//StringBuffer sb = new StringBuffer(100);
	std::ostringstream sb;
	// add existing velocity stuff here
	for (int i = 0; i < size(); i++) {
		if (includeVirtuals || !point(i).isVirtual()) {
			if (name.size() == 0) {
				sb << "Aircraft";
			} else {
				sb << name;
			}
			sb << ", ";
			sb << (point(i).toOutput(precision,tcpColumns));
			for (int j = 0; j < extraspace; j++) {
				sb << ", -";
			}
			sb << endl;
		}
	}
	return sb.str();
}

std::ostream& operator << (std::ostream& os, const Plan &f) {
	Plan n(f);
	os << n.toString();
	return os;
}

}

