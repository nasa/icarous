
/*
 * Plan.cpp - -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 *  Authors: George Hagen,  Jeff Maddalon,  Rick Butler
 *
 */

#include "Plan.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "KinematicsPosition.h"
#include "PositionUtil.h"
#include "Kinematics.h"
#include "KinematicsDist.h"
#include "GsPlan.h"
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
#include "TcpData.h"

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool Plan::debug = false;

TcpData Plan::invalid_value = TcpData::makeInvalid();

const double Plan::minDt = GreatCircle::minDt;
const double Plan::nearlyZeroGs = 1E-10;
      double Plan::gsIn_0 = -1;
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
	data = fp.data;
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
		if (isTCP(j)) return false;
	}
	return true;
}

bool Plan::validIndex(int i) const {
	if (i >= 0 && i < (int) points.size()) return true;
	else return false;
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
		if (std::abs(time(i)-p.time(i)) > 0.0000001) {
			rtn = false;
			//fpln("almostEquals: time i = "+i+" does not match: "+getTime(i)+"  !=   "+p.getTime(i));
		}

	}
	return rtn;
}


bool Plan::almostEquals(const Plan& p, double epsilon_horiz, double epsilon_vert) const {
	bool rtn = true;
	for (int i = 0; i < size(); i++) { // Unchanged
		if (!point(i).almostEqualsPosition(p.point(i), epsilon_horiz, epsilon_vert)) {
			rtn = false;
			//fpln("almostEquals: point i = " + i + " does not match: " + point(i) + "  !=   " + p.point(i) + " epsilon_horiz = " + epsilon_horiz);
		}
		if (!(point(i).label() == p.point(i).label())) {
			//fpln("almostEquals: point i = " + i + " labels do not match.");
			return false;
		}
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
	data.clear();
	getMessage();
	bound.clear();
}

void Plan::clearVirtuals() {
	int i = 0;
	while (i < size()) {
		if (isVirtual(i)) {
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

bool Plan::isTimeInPlan(double t) const {
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
		if (!isVirtual(i)) return points[i].time();
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


void Plan::clearLabel(int ix) {
	NavPoint np = point(ix);
	TcpData  tcp = getTcpDataRef(ix);
	set(ix,np.makeLabel(""),tcp);
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

int Plan::getSegmentByDistance(int startix, double d) const {
	if (d < 0) return -1;
	double tdist = 0;
	int i = startix;
	while (tdist <= d && i < size()) {
		tdist += pathDistance(i);
		i++;
	}
	if (tdist > d && i <= size()) return i-1; // on segment i-1
	if (Util::within_epsilon(d, tdist, 0.01) && i == size()) return size()-1;
	return -1; // not found
}

int Plan::getSegmentByDistance(double d) const {
	return getSegmentByDistance(0,d);
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

double Plan::time(int i) const {
	if (i < 0 || i >= size()) {
		//      System.out.println("$$!! invalid point index of "+Fm0(i)+" in getTime()");
		addError("getTime: invalid point index of "+Fm0(i), i);
		return 0.0;
	}
	return points[i].time();
}

const Position Plan::getPos(int i) const {
	return point(i).position();
}


const NavPoint Plan::point(int i) const {
	if (i < 0 || i >= (int)points.size()) {
		addError("point: invalid index "+Fm0(i), i);
		return isLatLon() ? NavPoint::ZERO_LL() : NavPoint::ZERO_XYZ();
	}
	return points[i];
}

const std::pair<NavPoint,TcpData> Plan::get(int i) const {
	return std::pair<NavPoint,TcpData>(point(i) ,getTcpData(i));
}


TcpData& Plan::getTcpDataRef(int i) {
	if (i < 0 || i >= (int)data.size()) {
		addError("Plan.point: invalid index " + Fmi(i), i);
		invalid_value = TcpData::makeInvalid();
		return invalid_value;
	}
	return data[i];
}


TcpData Plan::getTcpData(int i) const {
	if (i < 0 || i >= (int)data.size()) {
		addError("Plan.point: invalid index " + Fmi(i), i);
		invalid_value = TcpData::makeInvalid();
		return invalid_value;
	}
	return data[i];
}

bool    Plan::isOriginal(int i)    const { return getTcpData(i).isOriginal();}
bool    Plan::isAltPreserve(int i) const { return getTcpData(i).isAltPreserve();}
double  Plan::signedRadius(int i)  const { return getTcpData(i).signedRadius(); }
double  Plan::turnRadius(int i)    const { return getTcpData(i).turnRadius();}
bool    Plan::isVirtual(int i)     const { return getTcpData(i).isVirtual();}
bool    Plan::hasSource(int i)     const { return getTcpData(i).hasSource();}

double  Plan::trkAccel(int i)      const {
	double rtn = 0.0;
	if (std::abs(signedRadius(i)) > 0) {
		rtn = gsOut(i)/signedRadius(i);
	}
	//fpln(" $$$ trkAccel: radiusSigned = "+Units::str("NM",radiusSigned)+" rtn = "+Units::str("deg/s",rtn));
	return rtn;
}


double  Plan::gsAccel(int i)      const { return getTcpData(i).gsAccel();}
double  Plan::vsAccel(int i)       const { return getTcpData(i).vsAccel();}


//Velocity  Plan::velocityInit(int i)  const { return getTcpDataRef(i).velocityInit();}

bool Plan::isTrkTCP(int i)      const {return getTcpData(i).isTrkTCP(); }
bool Plan::isBOT(int i)      const {return getTcpData(i).isBOT(); }
bool Plan::isEOT(int i)      const {return getTcpData(i).isEOT(); }
bool Plan::isGsTCP(int i)    const {return getTcpData(i).isGsTCP(); }
bool Plan::isBGS(int i)      const {return getTcpData(i).isBGS(); }
bool Plan::isEGS(int i)      const {return getTcpData(i).isEGS(); }
bool Plan::isVsTCP(int i)    const {return getTcpData(i).isVsTCP(); }
bool Plan::isBVS(int i)      const {return getTcpData(i).isBVS(); }
bool Plan::isEVS(int i)      const {return getTcpData(i).isEVS(); }
bool Plan::isBeginTCP(int i) const {return getTcpData(i).isBeginTCP(); }
bool Plan::isEndTCP(int i)   const {return getTcpData(i).isEndTCP(); }


bool    Plan::isTCP(int i) const { return getTcpData(i).isTCP();}


NavPoint Plan::sourceNavPoint(int i) const {return NavPoint(getTcpData(i).getSourcePosition(),getTcpData(i).getSourceTime());}

std::pair<NavPoint,TcpData> Plan::makeBOT(const NavPoint& src, Position p, double t,  double signedRadius, const Position& center, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBOT t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setBOT(signedRadius, center, linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEOT(const NavPoint& src, Position p, double t, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBOT t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEOT(linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEOTBOT(const NavPoint& src, Position p, double t,  double signedRadius, const Position& center, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBOT t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEOTBOT(signedRadius, center, linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);
}



std::pair<NavPoint,TcpData> Plan::makeBGS(const NavPoint& src, Position p, double t, double a, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBGS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setBGS(a,linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEGS(const NavPoint& src, Position p, double t, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBGS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEGS(linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEGSBGS(const NavPoint& src, Position p, double t, double a, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBGS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEGSBGS(a,linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);
}


std::pair<NavPoint,TcpData> Plan::makeBVS(const NavPoint& src, Position p, double t, double a, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setBVS(a,linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEVS(const NavPoint& src, Position p, double t, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEVS(linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEVSBVS(const NavPoint& src, Position p, double t, double a, int linearIndex) {
	//fpln(" $$$$$ NavPoint.makeBVS t = "+t+"   velocityIn = "+velocityIn);
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData::makeSource(src).setEVSBVS(a,linearIndex);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}


std::pair<NavPoint,TcpData> Plan::makeMidpoint(const NavPoint& src, TcpData& tcp, const Position& p, double t, int linearIndex) {
	NavPoint np = NavPoint(p,t);
	tcp = tcp.copy().setSource(src).setLinearIndex(linearIndex);
	return std::pair<NavPoint,TcpData>(np,tcp);
}




std::string Plan::getInfo(int i) const {
	if (i < 0 || i >= size()) {
		addError("getInfo: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return "";
	}
	return data[i].getInformation();
}

void Plan::setInfo(int i, const std::string& info) {
	if (i < 0 || i >= size()) {
		addError("setInfo: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
	}
	TcpData& d = data[i];
	d.setInformation(info);
	data[i]=d;
}

void Plan::setVirtual(int i) {
	if (i < 0 || i >= size()) {
		addError("setVirtual: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setVirtual();  //setType(TcpData::Virtual);
	data[i]=d;
}


void Plan::setOriginal(int i) {
	if (i < 0 || i >= size()) {
		addError("setOriginal: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setOriginal(); //setType(TcpData::Orig);
	data[i]=d;
}

void Plan::setAltPreserve(int i) {
	if (i < 0 || i >= size()) {
		addError("setAltPreserve: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setAltPreserve(); //setType(TcpData::AltPreserve);
	data[i]=d;
}

void Plan::setRadius(int i, double radius) {
	if (i < 0 || i >= size()) {
		addError("setRadius: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setRadiusSigned(radius);
	data[i]=d;
}

double Plan::getGsIn_0() const {
	return gsIn_0;
}

void Plan::setGsIn_0(double gsIn_0_d) {
	gsIn_0 = gsIn_0_d;
}


//Position Plan::turnCenter(int i) const {
//	if (i < 0 || i >= size()) {
//		addError("turnCenter: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
//		return Position::INVALID();
//	}
//	double R = signedRadius(i);
//	if (R != 0) {
//		Velocity vHat = data[i].getVelocityInit().mkAddTrk(Util::sign(R) * M_PI / 2).Hat2D();
//		Position pos = point(i).position();
//		return pos.linear(vHat, std::abs(R)).mkZ(pos.z());
//	}
//	return Position::INVALID();
//}

Position Plan::turnCenter(int i) const {
	if (i < 0 || i >= size()) {
		addError("turnCenter: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return Position::INVALID();
	}
	return getTcpData(i).turnCenter();
}

double Plan::sourceTime(int i) const {
	if (i < 0 || i >= size()) {
		addError("sourceTime: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return 0.0;
	}
	return data[i].getSourceTime();
}

void Plan::setSourceTime(int i, double time) {
	if (i < 0 || i >= size()) {
		addError("setSourceTime: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setSourceTime(time);
	data[i]=d;
}



Position Plan::sourcePosition(int i) const {
	if (i < 0 || i >= size()) {
		addError("sourcePosition: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return Position::INVALID();
	}
	return data[i].getSourcePosition();
}

void Plan::setSourcePosition(int i, const Position& pos) {
	if (i < 0 || i >= size()) {
		addError("setSourcePosition: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
		return;
	}
	TcpData& d = data[i];
	d.setSourcePosition(pos);
	//data[i]=d;
}

void Plan::resetSource(int i) {
	setSource(i,point(i));
}

void Plan::setSource(int i, const NavPoint& npi) {
	setSourceTime(i,npi.time());
	setSourcePosition(i,npi.position());
}


int Plan::linearIndex(int i) const {
	if (i < 0 || i >= size()) {
		addError("linearIndex: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
		return 0;
	}
	return data[i].getLinearIndex();
}

void Plan::setLinearIndex(int i, int index) {
	if (i < 0 || i >= size()) {
		addError("setLinearIndex: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setLinearIndex(index);
	//data[i]=d;
}



//void Plan::setVelocityInit(int i, const Velocity& vel) {
//	if (i < 0 || i >= size()) {
//		addError("setVelocityInit: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
//	}
//	TcpData& d = data[i];
//	d.setVelocityInit(vel);
//	//data[i]=d;
//}

void Plan::setGsAccel(int i, double accel) {
	if (i < 0 || i >= size()) {
		addError("setGsAccel: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setGsAccel(accel);
	//data[i]=d;
}


void Plan::setVsAccel(int i, double accel) {
	if (i < 0 || i >= size()) {
		addError("setVsAccel: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setVsAccel(accel);
}

void Plan::setBOT(int i, double signedRadius, Position center, int linearIndex) {
	if (i < 0 || i >= size()) {
		addError("setBOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBOT(signedRadius, center, linearIndex);
}

void Plan::setEOT(int i) {
	if (i < 0 || i >= size()) {
		addError("setEOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEOT( 0);
}

void Plan::setEOTBOT(int i, double signedRadius, Position center, int linearIndex) {
	if (i < 0 || i >= size()) {
		addError("setEOTBOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEOTBOT(signedRadius, center, linearIndex);
}

void Plan::setBGS(int i) {
	if (i < 0 || i >= size()) {
		addError("setBGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBGS(0.0,  0);
}

void Plan::setEGS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEGS( 0);
	//data[i]=d;
}

void Plan::setEGSBGS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEGSBGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEGSBGS(0.0,  0);
	//data[i]=d;
}

void Plan::setBVS(int i) {
	if (i < 0 || i >= size()) {
		addError("setBVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBVS(0.0,  0);
	//data[i]=d;
}

void Plan::setEVS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEVS( 0);
	//data[i]=d;
}

void Plan::setEVSBVS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEVSBVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEVSBVS(0.0,  0);
	//data[i]=d;
}



int Plan::addNavPoint(const NavPoint& p) {
	TcpData d = TcpData();
	d.setSource(p);
	return add(p, d);
}

int Plan::add(const Position& p, double time) {
	NavPoint np = NavPoint(p,time);
	return addNavPoint(np);
}


//int Plan::addFull(const NavPoint& p, double radiusSigned, double accel_gs, double accel_vs,
//		const NavPoint& source, int linearIndex,
//		const std::string& information) {
//
//	TcpData& d(source);
//	//d.ty = WayType  ty;
//	//d.tcp_trk = Trk_TCPType tcp_trk;
//	//d.tcp_gs = Gs_TCPType  tcp_gs;
//	//d.tcp_vs = Vs_TCPType  tcp_vs;
//	d.setRadiusSigned(radiusSigned);
//	d.setGsAccel(accel_gs);
//	d.setVsAccel(accel_vs);
////	d.setVelocityInit(velocityInit);
//	d.setLinearIndex(linearIndex);
//	d.setInformation(information);
//
//	return add(p,d);
//}


int Plan::add(const std::pair<NavPoint,TcpData>& p) {
	return add(p.first, p.second);
}

int Plan::add(const NavPoint& p2, const TcpData& d) {
	NavPoint p = p2;

	//  This condition can happen in Java, but not C++
	//
	//	TcpData& d;
	//	if (tcpdata == null) {
	//		d = new TcpData(); // use default values
	//	} else {
	//		d = new TcpData(tcpdata);
	//	}

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
//	if (d.isBeginTCP() && d.velocityInit().isInvalid()) {
//		addError("add: attempted to add begin TCP with invalid velocity",0);
//		fpln(" $$$$$ plan.add: attempted to add begin TCP with invalid velocity, time = "+Fm4(p.time()));
//		return -1;
//	}

	int i = getIndex(p.time());
	if (i >= 0) {
		//replace
		if ( ! isVirtual(i)) {
			if (getTcpData(i).mergeable(d)) {
				NavPoint np2 = point(i).appendLabel(p.label());
				TcpData np = getTcpData(i).mergeTCPInfo(d);
				points[i] = np2;
				data[i] = np;
			} else {
				addWarning("Attempt to merge a point at time "+Fm4(p.time())+" that already has an incompatible point, no point added.");
				//fpln(" $$$$$ plan.add Attempt to add a point at time "+fm4(p.time())+" that already has an incompatible point. ");
				return -(int)points.size()-1;
			}
		} else { // just replace the virtual
			//fpln(" $$ NavPoint.add: set at i = "+Fm0(i)+" p = "+p);
			points[i] = p;
			data[i] = d;
		}
	} else {
		//insert
		i = -i-1;  // where the point should be inserted
		insert(i, p, d);
	}

	bound.add(p.position());

	return i;
}




// Insert a point at position i
// Warning: i must be a valid index: 0..numPts-1
void Plan::insert(int i, const NavPoint& v, const TcpData& d) {
	int numPts = points.size();
	if (i < 0 || i > numPts) {
		addError("insert: Invalid index "+Fm0(i),i);
		return;
	}

	//System.arraycopy(points,i,points,i+1,numPts-i);
	if (i >= numPts) {
		points.push_back(v);
		data.push_back(d);
	} else {
		navPointIterator pai = points.begin();
		points.insert(pai+i,v);

		tcpDataIterator pai2 = data.begin();
		data.insert(pai2+i,d);
	}
}



//void Plan::insertWithTimeshift(int i, const NavPoint& v) {
//	double d0 = -1; // segment distance into point
//	double d1 = -1; // segment distance out of point
//	NavPoint v1 = v;
//	NavPoint np0 = closestPoint(v1.position()); // closest point, if in accel zone
//	//double dt0 = 0;
//	bool in_Accel = false;
//	bool onPlan = np0.distanceH(v) < Util::min(pathDistance()*0.001, 1.0);  // within 1 meter of plan (or 1/1000 of a short plan)
//	if (i > 0) {
//		d0 = point(i-1).distanceH(v);
//		if (inAccel(getTime(i-1))) {
//			in_Accel = true;
//			d0 = pathDistance(i-1,np0.time(),false);
//		}
//	}
//	if (i < size()) {
//		d1 = v.distanceH(point(i));
//		if (in_Accel && onPlan) {
//			d1 = pathDistanceFromTime(np0.time(),i);
//		}
//	}
//	if (d0 >= 0) {
//		if (onPlan) {
//			double d = d0+pathDistance(0,i-1);
//			//			double dd = pathDistance();
//			double t = timeFromDistance(d);
//			v1 = v.makeTime(t);
//		} else {
//			double t = getTime(i-1) + d0/gsOut(i-1,true);
//			v1 = v.makeTime(t);
//		}
//	}
//
//	insert(i,v1);
//
//	if (d1 >= 0) {
//		if (onPlan) {
//			double t1 = getTime(i+1);
//			double dd = pathDistance(0,i+1);
//			double t2 = timeFromDistance(dd);
//			double dt = t1 - t2;
//			timeshiftPlan(i+1,dt);
//		} else {
//			double dt = (v1.time() + d1/gsOut(i,true)) - getTime(i+1);
//			timeshiftPlan(i+1,dt);
//		}
//	}
//}


void Plan::remove(int i) {
	int numPts = points.size();
	if (i < 0 || i >= numPts) {
		addWarning("remove1: invalid index " +Fm0(i));
		return;
	}
	//System.arraycopy(points,i+1,points,i,numPts-i-1);
	navPointIterator pai = points.begin();
	points.erase(pai+i);

	tcpDataIterator pai2 = data.begin();
	data.erase(pai2+i);

	numPts = numPts - 1;
}

void Plan::remove(int i, int j) {
	if (i < 0 || j < i || j >= (int)points.size()) {
		addWarning("remove2: invalid index(s) "+Fm0(i)+" "+Fm0(j));
		return;
	}
	for (int k = j; k >= i; k--) {
		remove(k);
	}
}

int Plan::set(int i, const NavPoint& v, const TcpData& d) {
	if (i < 0 || i >= (int)points.size()) {
		addError("invalid index "+Fm0(i)+" in set()");
		return -1;
	}
	remove(i);
	return add(v,d);
}

int Plan::setNavPoint(int i, const NavPoint& v) {
	if (i < 0 || i >= (int)points.size()) {
		addError("invalid index "+Fm0(i)+" in set()");
		return -1;
	}
	TcpData tcp = getTcpData(i);
	remove(i);
	return add(v,tcp);
}

void Plan::setTime(int i, double t) {
	if (t < 0) {
		addError("setTime: invalid time "+Fm4(t),i);
		return;
	}
	NavPoint tempv = points[i].makeTime(t);
	TcpData  tcp = getTcpData(i);
	set(i, tempv, tcp);


}


void Plan::setAlt(int i, double alt) {
	if (i < 0 || i >= (int)points.size()) {
		addError("setTime: Invalid index "+Fm0(i),i);
		return;
	}
	NavPoint tempv = points[i].mkAlt(alt);
	remove(i);
	add(tempv, getTcpData(i));
}

Plan Plan::copyAndTimeShift(int start, double st) {
	Plan newKPC = Plan(name);
	if (start >= size() || st == 0.0) return *this;
	for (int j = 0; j < start; j++) {
		newKPC.add(get(j));
		//fpln("0 add newKPC.point("+j+") = "+newKPC.point(j));
	}
	double ft = 0.0; // time of point before start
	if (start > 0) {
		ft = time(start-1);
	}
	for (int i = start; i < size(); i++) {
		double t = time(i)+st; // adjusted time for this point
		//fpln(">>>> timeshiftPlan: t = "+ t+" ft = "+ft);
		if (t > ft && t >= 0.0) {
			// double newSourceTime = point(i).sourceTime() + st;
			std::pair<NavPoint, TcpData> pair = get(i);
			newKPC.add(pair.first.makeTime(t),pair.second); // .makeSourceTime(newSourceTime));
			//fpln(" add newKPC.point("+Fm0(i)+") = "+newKPC.point(i));
		} else {
			//fpln(">>>> copyTimeShift: do not add i = "+Fm0(i));
		}
	}
	return newKPC;
}



bool Plan::timeShiftPlan(int start, double dt) {
	if (ISNAN(dt) || start < 0 ) {
		//if (debug) Debug.halt();
		return false;
	}
	if (start >= size() || dt == 0.0) return true;
	if (dt < 0) {
		double ft = -1.0;
		if (start > 0) {
			ft = time(start-1);
			//			if (getTime(start)+dt <= ft+Constants::get_time_accuracy()) {
			//                // fpln("Plan.timeShiftPlan failed dt="+dt);
			//				return false;
			//			}

		}
		for (int i = start; i < size(); i++) {
//			if (preserveRTAs) { //  && point(i).isFixedTime()) {
//				break;
//			}
			double t = time(i)+dt;
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
			setTime(i, time(i)+dt);
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
		if (isTrkTCP(j)) {
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
		if (isTrkTCP(j)) {
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
		if (isGsTCP(j)) {
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
		if (isGsTCP(j)) {
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
		if (isVsTCP(j)) {
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
		if (isVsTCP(j)) {
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
		if (isBOT(j)) {
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
		if (isEOT(j)) {
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
		if (isEOT(j)) {
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
		if (isBOT(j)) {
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
		if (isBGS(j)) {
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
		if (isEGS(j)) {
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
		if (isEGS(j)) {
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
		if (isBGS(j)) {
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
		if (isBVS(j)) {
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
		if (isEVS(j)) {
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
		if (isEVS(j)) {
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
		if (isBVS(j)) {
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
		//fpln(" $$$$$$$$$$$$ prevTCP: points.get("+j+") = "+points.get(j).toStringFull());
		//			if (points.get(j).tcp_trk==Trk_TCPType.BOT || points.get(j).tcp_trk==Trk_TCPType.EOT || points.get(j).tcp_trk==Trk_TCPType.EOTBOT ) {
		if (isTrkTCP(j) || isGsTCP(j) || isVsTCP(j) ) {

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
		int ixBOT = prevBOT(getSegment(t)+1);
		NavPoint bot = points[ixBOT];//fixed
		//return bot.position().distanceH(bot.turnCenter());
		return getTcpData(ixBOT).turnRadius();
	} else {
		return -1.0;
	}
}

double Plan::trkAccelAtTime(double t) const {
	if (inTrkChange(t)) {
		int b = prevBOT(getSegment(t)+1);//fixed
		return trkAccel(b);
	} else {
		return 0.0;
	}
}


double Plan::gsAccelAtTime(double t) const {
	if (inGsChange(t)) {
		int b = prevBGS(getSegment(t)+1);//fixed
		return gsAccel(b);
	} else {
		return 0.0;
	}
}
double Plan::vsAccelAtTime(double t) const {
	if (inVsChange(t)) {
		int b = prevBVS(getSegment(t)+1);//fixed
		return vsAccel(b);
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
double Plan::timeFromDistance(double vo, double gsAccel, double dist) {
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
double Plan::timeFromDistanceWithinSeg(int seg, double rdist) const {
	if (seg < 0 || seg > size()-1 || rdist < 0 || rdist > pathDistance(seg)) return -1;
	double gs0 = initialVelocity(seg).gs();
	if (Util::almost_equals(gs0, 0.0)) return 0;
	if (inGsChange(time(seg))) {
		double a = gsAccel(prevBGS(seg+1));//fixed
		return timeFromDistance(gs0, a, rdist);
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
	return timeFromDistanceWithinSeg(seg,dd)+time(seg);
}


/** Return the absolute time that corresponds to the point that is path distance "dist" from startSeg
 *
 * @param dist  distance from start of plan
 * @return      time of point that is "rdist" distance from start of plan
 *
 */
double Plan::timeFromDistance(int startSeg, double dist) const {
	int seg = getSegmentByDistance(startSeg,dist);
	if (seg < 0 || seg > size() - 1) return -1;
	double pd = pathDistance(startSeg,seg);
	double distWithinLastSeg = dist - pd;
	//f.pln(" $$$$$$$ timeFromDistance: distWithinLastSeg = "+Units::str("ft",distWithinLastSeg));
	double tmWithin = timeFromDistanceWithinSeg(seg, distWithinLastSeg);
	//f.pln(" $$$$$$$ timeFromDistance: tmWithin = "+tmWithin+" time(seg) = "+time(seg));
	return tmWithin + time(seg);
}

Velocity Plan::velocityByDistance(double d) const {
	return velocity(timeFromDistance(d));
}


double Plan::trkOut(int seg, bool linear) const {
	//fpln(" $$$ trkOut: ENTER seg = "+seg+" linear = "+linear);
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
		Position center = turnCenter(ixBOT);
		double signedRad = signedRadius(ixBOT);
		int dir = Util::sign(signedRad);
		//fpln(" $$$ trkOut (inTurn): d = "+Units::str("ft",d)+"  signedRadius = "+Units::str("ft",signedRadius)+" ixBOT = "+ixBOT);
		double gsAt_d = 1000.0; // not used here -- don't use 0.0 because will lose track info
		Position so = point(ixBOT).position();
		vNew = KinematicsPosition::turnByDist2D(so, center, dir, d, gsAt_d).second;
		//fpln(" $$$ trkOut: vNew = "+vNew);
		return vNew.trk();
	} else {
		if (isLatLon()) {
			return GreatCircle::initial_course(point(seg).lla(), point(seg+1).lla());
		} else {
		    return point(seg).initialVelocity(point(seg+1)).trk();
		}
	}
}

double Plan::trkOut(int seg) const { return trkOut(seg,false); }

double Plan::trkIn(int seg) const { return trkFinal(seg-1, false); }


double Plan::trkFinal(int seg, bool linear) const {
	//fpln("$$$ trkFinal: ENTER: seg = "+seg+" linear = "+linear);
	if (seg < 0 || seg >= size()-1) {
		addError("trkFinal: invalid index "+Fm0(seg), 0);
		return -1;
	}
	NavPoint np1 = point(seg);
	if (inTrkChange(point(seg).time()) && ! linear) {
		int ixBOT = prevBOT(seg+1);
		double d = pathDistance(ixBOT,seg+1);
		double signedRad = signedRadius(ixBOT);
		int dir = Util::sign(signedRad);
		Position center = turnCenter(ixBOT);
		//fpln("$$$ trkFinal AA: d = "+Units::str("NM",d)+"  signedRadius = "+Units::str("ft",signedRadius)+" ixBOT = "+ixBOT);
		double gsAt_d = 1000.0; // not used here -- don't use 0.0 because will lose track info
		Position so = point(ixBOT).position();
		Velocity vFinal = KinematicsPosition::turnByDist2D(so, center, dir, d, gsAt_d).second;
		//fpln("$$$ trkFinal AA: seg = "+seg+" vFinal = "+vFinal);
		return vFinal.trk();
	} else {
		if (isLatLon()) {
			//double d = pathDistance(seg,seg+1);
			double trk = GreatCircle::final_course(point(seg).lla(), point(seg+1).lla());
			//fpln("$$$ trkFinal BB: seg = "+seg+" trk = "+Units::str("deg",trk));
			return trk;
		} else {
			Velocity vo = NavPoint::finalVelocity(point(seg), point(seg+1));
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
	double dt = time(j) - time(i);
	double a = 0.0;
	if (inGsChange(time(j-1)) && ! linear) {
		int ixBGS = prevBGS(i+1);//fixed
		a = gsAccel(ixBGS);
	}
	double rtn = dist/dt - 0.5*a*dt;
	//fpln(" $$>>>>>> gsOut: rtn = "+Units::str("kn",rtn,8)+" a = "+a+" seg = "+seg+" dt = "+f.Fm4(dt)+" dist = "+Units::str("ft",dist));
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
	double dt = time(j) - time(i);
	double a = 0.0;
	if (inGsChange(point(i).time()) && ! linear) {
		int ixBGS = prevBGS(i+1);//fixed
		a = gsAccel(ixBGS);
	}
	return dist/dt + 0.5*a*dt;
}

double Plan::gsIn(int seg, bool linear)  const {
	if (seg==0) return gsIn_0;
	return gsFinal(seg-1,linear);
}

double Plan::gsOut(int seg)  const { return gsOut(seg,false); }
double Plan::gsFinal(int seg) const { return gsFinal(seg,false); }
double Plan::gsIn(int seg)  const { return gsIn(seg,false); }



// ********** EXPERIMENTAL ****************
double Plan::gsAtTime(int seg, double gsAtSeg, double t, bool linear) const {
	//fpln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units::str("kn",gsAt,8));
	double gs;
	if (!linear && inGsChange(t)) {
		double dt = t - time(seg);
		int ixBGS = prevBGS(seg+1);//fixed
		double gsAcc = gsAccel(ixBGS);
		gs = gsAtSeg + dt*gsAcc;
		//fpln(" $$ gsAtTime A: gsAccel = "+gsAccel+" dt = "+f.Fm4(dt)+" seg = "+seg+" gs = "+Units::str("kn",gs,8));
	} else {
		gs = gsAtSeg;
		//fpln(" $$ gsAtTime B: seg = "+seg+" gs = "+Units::str("kn",gs,8));
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
		//fpln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units::str("kn",gsAt,8));
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
	double dt = time(j) - time(i);
	double a = 0.0;
	if (inVsChange(time(j-1)) && ! linear) {  // use getTime(j-1) rather than getTime(i) in case j-1 point is an EGS
		int ixBVS = prevBVS(i+1);//fixed
		a = vsAccel(ixBVS);
	}
	double rtn = dist/dt - 0.5*a*dt;
	//fpln(" $$>>>>>> vsOut: rtn = "+Units::str("fpm",rtn,8)+" a = "+a+" seg = "+seg+" dt = "+f.Fm4(dt)+" dist = "+Units::str("ft",dist));
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
	double dt = time(i+1) - time(i);
	double a = 0.0;
	if (inVsChange(point(i).time()) && ! linear) {
		int ixBvs = prevBVS(i+1);//fixed
		a = vsAccel(ixBvs);
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
	//fpln(" $$ vsAtTime: seg = "+seg+" vsAt = "+Units::str("kn",vsAt,8));
	double vs;
	if (!linear && inVsChange(t)) {
		double dt = t - time(seg);
		int ixBvs = prevBVS(seg+1);//fixed
		double vsAcc = vsAccel(ixBvs);
		vs = vsAtSeg + dt*vsAcc;
		//fpln(" $$ vsAtTime A: vsAccel = "+vsAccel+" dt = "+f.Fm4(dt)+" seg = "+seg+" vs = "+Units::str("kn",vs,8));
	} else {
		vs = vsAtSeg;
		//fpln(" $$ vsAtTime B: seg = "+seg+" vs = "+Units::str("kn",vs,8));
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
		//fpln(" $$ vsAtTime: seg = "+seg+" vsSeg = "+Units::str("fpm",vsSeg,8));
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
		double a = gsAccel(prevBGS(seg+1));//fixed
		distFromSo = gs0*dt + 0.5*a*dt*dt;
		//fpln(" $$$ distFromPointToTime(inGsChange A): dt = "+f.Fm2(dt)+" vo.gs() = "+Units::str("kn",gs0)+" distFromSo = "+Units::str("ft",distFromSo));
	} else {
		distFromSo = gs0*dt;
		//fpln(" $$$ distFromPointToTime(! inGsChange B): dt = "+f.Fm4(dt)+" gs0 = "+Units::str("kn",gs0)+" distFromSo = "+Units::str("ft",distFromSo));
	}
	return distFromSo;
}

/** Assumes seg = getSegment(t)
 *
 * @param t         time of interest
 * @param linear    If true, then interpret plan in a linear manner
 * @return          position and velocity at time t
 * @param gsAt_d    ground speed at time t
 * @return
 */
std::pair<Position, Velocity> Plan::advanceWithinSeg(int seg, double t, bool linear, double gsAt_d) const  {
	Position sNew;
	Velocity vNew;
	NavPoint np1 = point(seg);
    Position so = np1.position();
    std::pair<Position, Velocity> pv;
	if ( ! linear && inTrkChange(t)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		//fpln(" $$$ advanceWithinSeg: center = "+center);
		double signedRadius_d = signedRadius(ixPrevBOT);
		int dir = Util::sign(signedRadius_d);
		double distFromSo;
		bool method1 = true; // starting position could be either the current segment or the previous BOT
		if (method1) {
			distFromSo = distFromPointToTime(seg, t, linear); // starting position is the current segment
		} else {
			so = point(ixPrevBOT).position(); // // starting position is the previous BOT
			distFromSo = distFromPointToTime(ixPrevBOT, t, linear);
		}
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSo, gsAt_d);
	} else {
		NavPoint np2 = point(seg+1);
		Velocity vo = np1.initialVelocity(np2);
		double distFromSo = distFromPointToTime(seg, t, linear);
		pv = so.linearDist2D(vo.trk(), distFromSo, gsAt_d);
	}
	return pv;
}


/**
 *
 * @param seg
 * @param distFromSeg
 * @param linear
 * @param gsAt_d
 * @return
 */
std::pair<Position, Velocity> Plan::advanceDistanceWithinSeg2D(int seg, double distFromSeg, bool linear, double gsAt_d) const {
	NavPoint np1 = point(seg);
    Position so = np1.position();
    std::pair<Position,Velocity> pv;
    double tSeg = point(seg).time();
	if ( ! linear && inTrkChange(tSeg)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		//f.pln(" $$$ positionVelocity: center = "+center);
		double signedRadius_d = signedRadius(ixPrevBOT);
		int dir = Util::sign(signedRadius_d);
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSeg, gsAt_d);
		// f.pln(" $$ %%%% positionVelocity A: vNew("+f.Fm2(t)+") = "+vNew);
		// f.pln(" $$ %%%% positionVelocity A: sNew("+f.Fm2(t)+") = "+sNew);
	} else {
		NavPoint np2 = point(seg+1);
		Velocity vo = np1.initialVelocity(np2);
		pv = so.linearDist2D(vo.trk(), distFromSeg, gsAt_d);
	}
	return pv;
}


Position Plan::advanceDistanceWithinSeg2D(int seg, double distFromSeg, bool linear) const {
	NavPoint np1 = point(seg);
    Position so = np1.position();
    Position pv;
    double tSeg = point(seg).time();
	if ( ! linear && inTrkChange(tSeg)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		int dir = Util::sign(signedRadius(ixPrevBOT));
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSeg);
	} else {
		Velocity vo = np1.initialVelocity(point(seg+1));
		pv = so.linearDist2D(vo.trk(), distFromSeg);
	}
	return pv;
}

/** starting with point at seg advance "distanceFromSeg" in Plan
 * NOTE do not use non-positive value for gsAt_d
 *
 * @param seg
 * @param distFromSeg
 * @param linear
 * @param gsAt_d
 * @return
 */
std::pair<Position,int> Plan::advanceDistance2D(int seg, double distFromSeg, bool linear) const{
	double remainingDist = distFromSeg;
	for (int i = seg; i < size(); i++) {
		double pathDist_i = pathDistance(i);
		if (remainingDist < pathDist_i) {
			 Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
			 return std::pair<Position,int>(newPos,i);
		} else {
			remainingDist = remainingDist - pathDist_i;
		}
	}
	addWarning("advanceDistance:  distance exceeded length of plan!");
	int ixLast = size() - 1;
	return  std::pair<Position, int>(points[ixLast].position(), ixLast);
}

//std::pair<Position,int> Plan::advanceDistance(int seg, double distFromSeg, bool linear) const {
//	std::pair<Position,int> posSeg = advanceDistance2D(seg, distFromSeg, linear);
//	double t = timeFromDistance(seg, distFromSeg);
//	double alt = position(t).alt();
//	//double deltaAlt = posSeg.first.alt() - alt;
//	//f.pln(" $$$$ advanceDistance: deltaAlt = "+deltaAlt);
//	Position altPos = posSeg.first.mkAlt(alt);
//	return std::pair<Position,int>(altPos,posSeg.second);
//}

std::pair<Position,int> Plan::advanceDistance(int seg, double distFromSeg, bool linear) const {
	double remainingDist = distFromSeg;
	for (int i = seg; i < size(); i++) {
		double pathDist_i = pathDistance(i);
		if (remainingDist < pathDist_i) {
			Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
			double t = timeFromDistance(i, remainingDist);
			double alt = position(t).alt();
			Position altPos = newPos.mkAlt(alt);
			return std::pair<Position,int>(altPos,i);
		} else {
			remainingDist = remainingDist - pathDist_i;
		}
	}
	addWarning("advanceDistance: distance exceeded length of plan!");
	int ixLast = size() - 1;
	return  std::pair<Position, int>(points[ixLast].position(), ixLast);
}


/** Assumes seg = getSegment(t)
 *
 * @param t         time
 * @param linear    If true, then interpret plan in a linear manner
 * @return          position and velocity at time t
 * @param gsAt_d    ground speed at time t
 * @return
 */
std::pair<Position, Velocity> Plan::posVelWithinSeg(int seg, double t, bool linear, double gsAt_d) const {
	//Position sNew;
	//Velocity vNew;
	NavPoint np1 = point(seg);
    Position so = np1.position();
    std::pair<Position,Velocity> pv;
	if ( ! linear && inTrkChange(t)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		//f.pln(" $$$ positionVelocity: center = "+center);
		double signedRadius_d = signedRadius(ixPrevBOT);
		int dir = Util::sign(signedRadius_d);
		double distFromSo;
		bool method1 = true; // starting position could be either the current segment or the previous BOT
		if (method1) {
			distFromSo = distFromPointToTime(seg, t, linear); // starting position is the current segment
		} else {
			so = point(ixPrevBOT).position(); // // starting position is the previous BOT
			distFromSo = distFromPointToTime(ixPrevBOT, t, linear);
		}
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSo, gsAt_d);
		//sNew = tAtd.first;
		//vNew = tAtd.second;
		// f.pln(" $$ %%%% positionVelocity A: vNew("+f.Fm2(t)+") = "+vNew);
		// f.pln(" $$ %%%% positionVelocity A: sNew("+f.Fm2(t)+") = "+sNew);
	} else {
		NavPoint np2 = point(seg+1);
		Velocity vo = np1.initialVelocity(np2);
		double distFromSo = distFromPointToTime(seg, t, linear);
//			std::pair<Position, Velocity> pv = so.linearDist2D(vo, distFromSo);
//			vNew = pv.second.mkGs(gsAt_d);
		pv = so.linearDist2D(vo.trk(), distFromSo, gsAt_d);
		//sNew = pv.first;
		//vNew = pv.second;
	}
	return pv;
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
	//fpln("\n $$$$$ positionVelocity: ENTER t = "+t+" seg = "+seg);
	NavPoint np1 = point(seg);
	if (seg+1 > size()-1) {  // at Last Point
		Velocity v = finalVelocity(seg-1);
		//fpln("\n -----size = "+size()+" seg = "+seg+"\n $$$ accelZone: np1 = "+np1+" v = "+v);
		return std::pair<Position,Velocity>(np1.position(),v);
	}
	NavPoint np2 = point(seg+1);
	//fpln("\n ----------- seg = "+seg+"\n $$$ positionVelocity: np1 = "+np1);
	//fpln(" $$$ positionVelocity: np2 = "+np2);
	double gs0 = gsOut(seg,linear);
	//double gsAt_d = gsAtTime(t,linear);    // TODO; improve speed by passing in gs0 into gsAtTime
	double gsAt_d = gsAtTime(seg, gs0, t, linear);
	//fpln(" $$$ positionVelocity: seg = "+seg+" gs0 = "+Units::str("kn",gs0,4)+" gsAt_d = "+Units::str("kn",gsAt_d,4)); ;
	Position so = np1.position();
	std::pair<Position, Velocity> adv = advanceWithinSeg(seg, t, linear, gsAt_d);
	Position sNew = adv.first;
	Velocity vNew = adv.second;
	std::pair<double,double> altPair = interpolateAltVs(seg, t-time(seg), linear);
	sNew = sNew.mkAlt(altPair.first);
	vNew = vNew.mkVs(altPair.second);


//	if (inVsChange(t) & !linear) {
//		int ixBVS = prevBVS(seg+1);
//		NavPoint n1 = points[ixBVS];//fixed
//		Position soP = n1.position();
//		//double voPvs = n1.velocityInit().vs();
//		double voPvs = vsAtTime(n1.time(),linear);
//		std::pair<double,double> pv =  KinematicsPosition::vsAccelZonly(soP, voPvs, t-n1.time(), vsAccel(ixBVS));
//		sNew = sNew.mkAlt(pv.first);
//		vNew = vNew.mkVs(pv.second);                   // merge Vertical VS with horizontal components
//		//fpln(t+" $$$ positionVelocity(inVsChange) C: vNew = "+vNew);
//	} else {
//		if (seg < size()-1) {   // otherwise np2 is not a valid future point
//			double dt = t - np1.time();
//			double vZ = (np2.z() - np1.z())/(np2.time()-np1.time());
//			double sZ = np1.z() + vZ*dt;
//			//fpln(" $$$$$$$$ seg = "+seg+" dt = "+f.Fm2(dt)+" vZ = "+Units::str("fpm",vZ)+" sZ = "+Units::str("ft",sZ));
//			sNew = sNew.mkAlt(sZ);
//			vNew = vNew.mkVs(vZ);
//		}
//		//fpln(t+" $$$ positionVelocity(NOT inVsChange): vNew = "+vNew);
//	}
	//fpln(" $$ %%%% positionVelocity RETURN: sNew("+Fm2(t)+") = "+sNew.toString());
	//fpln(" $$ %%%% positionVelocity RETURN: vNew("+Fm2(t)+") = "+vNew.toString());
	return std::pair<Position,Velocity>(sNew,vNew);
}

std::pair<Position,Velocity> Plan::positionVelocity(double t) const {
	return positionVelocity(t,false);
}

std::pair<double,double> Plan::interpolateAltVs(int seg, double dt, bool linear) const {
	double vsAccel_d = 0.0;
	double tSeg = time(seg);
	if (inVsChange(tSeg) && !linear) {
		int ixBVS = prevBVS(seg+1);
		vsAccel_d = vsAccel(ixBVS);
	}
	double alt1 = point(seg).alt();
	double vsInit = vsOut(seg,linear);
	double newAlt = alt1 + vsInit*dt + 0.5 * vsAccel_d*dt*dt;; // interpolateAlt(vsInit, vsAccel,  alt1,  t1,  t);
	double newVs = vsInit + vsAccel_d*dt;
	return std::pair<double,double>(newAlt,newVs);
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
	return NavPoint::averageVelocity(points[i],points[i+1]);
}



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
	while (j < (int) size() && time(j) - time(i) < minDt) { // collapse next point(s) if very close
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
		return NavPoint::finalVelocity(npi, lastPt);
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
//		//fpln(" $$$ finalLinearVelocity: p1 = "+p1+" p2 = "+p2+" dt = "+dt+" v = "+v);
//	} else {
//		v = np.initialVelocity(points[j]);
//	}
//	//fpln(" $$ finalLinearVelocity: dt = "+(points.get(i+1).time()-points.get(i).time()));
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

//Velocity Plan::dtFinalVelocity(int i, bool linear) const {
//	if (i >= (int) size()) {   // there is no "final" velocity after the last point (in general.  it happens to work for Euclidean, but not lla)
//		addWarning("finalVelocity(int): Attempt to get a final velocity after the end of the Plan: "+Fm0(i));
//		return Velocity::INVALIDV();
//	}
//	if (i == (int) size()-1) {// || points[i].time() > getLastTime()-minDt) {
//		addWarning("finalVelocity(int): Attempt to get a final velocity at end of the Plan: "+Fm0(i));
//		return Velocity::ZEROV();
//	}
//	if (i < 0) {
//		addWarning("finalVelocity(int): Attempt to get a final velocity before beginning of the Plan: "+Fm0(i));
//		return Velocity::ZEROV();
//	}
//	//fpln(" ########## finalVelocity0:   np1="+points[i]+" np2="+points[i+1]);
//	double t2 = points[i+1].time();
//	return positionVelocity(t2-2.0*minDt).second;
//}
//


/**
 * calculate delta time for point i to make ground speed into it = gs
 *
 * @param i
 * @param gs
 * @return delta time
 */
double Plan::calcDtGsin(int i, double gs) const {
	if (i < 1 || i >= size()) {
		addError("calcTimeGSin: invalid index " + Fm0(i), 0); // must have a prev
		// wPt
		return -1;
	}
	if (gs <= 0) {
		addError("calcTimeGSIn: invalid gs=" + Fm2(gs), i);
		return -1;
	}
	if (inGsChange(points[i-1].time()) && !isBGS(i)) {
//		double dist = pathDistance(i - 1, i);
//		double initGs = gsOut(i - 1);
//		int ixBGS = prevBGS(i);
//		double gsAccel_d =gsAccel(ixBGS);
//		double deltaGs = gs - initGs;
//		double dt = deltaGs / gsAccel_d;
//		double acceldist = dt * (gs + initGs) / 2;
//		if (acceldist > dist) {
//			// fpln("#### calcTimeGSin: insufficient distance to achieve
//			// new ground speed");
//			addError("calcTimeGSin " + Fm0(i) + " insufficient distance to achieve new ground speed", i);
//			return -1;
//		}
//		dt = dt + (dist - acceldist) / gs;
		// fpln("#### calcTimeGSin: dist = "+Units::str("nm",dist)+" deltaGs
		// = "+Units::str("kn",deltaGs)+" dt = "+dt);
		addError(" calcDtGsin:  attempt to change point "+Fm0(i)+"'s time which is inside a ground speed acceleration!");
		return 0;
	} else {
		double dist = pathDistance(i - 1, i);
		double dt = dist / gs;
		// fpln("#### calcTimeGSin: i = "+i+" dist =  "+Units::str("nm",dist)+" dt = "+f.Fm4(dt)+" points.get(i-1).time() = "+points.get(i-1).time());
		return dt;
	}
}

/**
 * calculate time at a waypoint such that the ground speed into that
 * waypoint is "gs". If i or gs is invalid, this returns -1. If i is in a
 * turn, this returns the current point time.
 *
 * Note: parameter maxGsAccel is not used on a linear segment
 */
double Plan::calcTimeGSin(int i, double gs) const {
	return points[i - 1].time() + calcDtGsin(i, gs);
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
	TcpData tempv_tcp = data[i];
	if (preserve)
		tempv_tcp = tempv_tcp.setAltPreserve();
	set(i, tempv, tempv_tcp);
}


/** change the ground speed into ix to be gs -- all other ground speeds remain the same
 *
 * @param p    Plan of interest
 * @param ix   index
 * @param gs   new ground speed
 * @return     revised plan
 */
void Plan::mkGsIn(int ix, double gs) {
	if (ix > size() - 1) return;
	double tmIx = calcTimeGSin(ix,gs);
	timeShiftPlan(ix,tmIx - point(ix).time());
//	if (updateTCP) {
//		NavPoint np = point(ix);
//		if (isBeginTCP(ix)) {
//			//Velocity vin = velocityInit(ix);
//			Velocity vin = initialVelocity(ix);
//			//NavPt npNew = makeVelocityInit(np,vin.mkGs(gs));
//			TcpData tcp = getTcpData(ix); // .setVelocityInit(vin.mkGs(gs));
//			set(ix, np,tcp);
//		}
//	}
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
	//fpln("\n $$ makeGsOut: ix = "+Fm0(ix)+" dt = "+Fm4(dt)+" gs = "+Units::str("kn",gs));
	timeShiftPlan(ix+1,dt-time(ix+1));
	NavPoint np = point(ix);
	if (isBeginTCP(ix)) {
		//Velocity vin = velocityInit(ix);
		Velocity vin = initialVelocity(ix);
		double vs_out = vsOut(ix);
		vin = Velocity::mkTrkGsVs(vin.trk(),gs,vs_out);
		TcpData np_tcp = getTcpData(ix);
		//np_tcp = np_tcp.setVelocityInit(vin);
		//fpln(" $$ makeGsOut: vin = "+vin+" npNew = "+npNew.toStringFull());
		set(ix,np, np_tcp);
	}
	//fpln(" $$$$ makeGsOut: EXIT gsOut = "+Units::str("kn",gsOut(ix),6));
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
	Position p1 = points[i].position();
	Position p2 = points[i+1].position();
	if (!linear && inTrkChange(time(i))) {
		// if in a turn, figure the arc distance
		int ixBOT = prevBOT(i+1);
		//NavPoint bot = points[ixBOT];//fixed
		Position center = turnCenter(ixBOT);
		double R = turnRadius(ixBOT);
		double theta = PositionUtil::angle_between(p1,center,p2);
		return std::abs(theta*R);
	} else {
		// otherwise just use linear distance
		return p1.distanceH(p2);
	}
}

double Plan::pathDistance(int i, int j) const {
	return pathDistance(i,j, false);
}

double Plan::pathDistance(int i, int j, bool linear) const {
	if (i < 0) {
		i = 0;
	}
	if (j >= size()) {
		j = size()-1;
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
	//fpln("\n $$$ partialPathDistance: seg = "+seg);
	// if in a turn, figure the arc distance
	if (inTrkChange(t) && !linear) {
		int ixBOT = prevBOT(getSegment(t)+1);
		//NavPoint bot = points[ixBOT];//fixed
		double R = turnRadius(ixBOT);
		Position center = turnCenter(ixBOT);
		double alpha = PositionUtil::angle_between(currentPosition,center,point(seg+1).position());
		//fpln(" $$$$ alpha = "+Units::str("deg",alpha));
		double rtn = std::abs(alpha*R);
		//fpln(" $$$$+++++ partialPathDistance:  rtn = "+Units::str("nm",rtn));
		return rtn ;
	} else {
		// otherwise just use linear distance
		double rtn =  position(t).distanceH(point(seg+1).position());
		//fpln(" $$$$.... partialPathDistance: points.get(seg+1) = "+points.get(seg+1));
		//fpln(" $$$$.... partialPathDistance: rtn = "+Units::str("nm",rtn));
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



Position Plan::vertexFromTurnTcps(int ixBOT, int ixEOT, double altMid) const {
	//fpln(" $$ vertexFromTurnTcps: ixBOT = "+Fm0(ixBOT)+" ixEOT = "+Fm0(ixEOT));
	if (!validIndex(ixBOT) || !validIndex(ixEOT)) return Position::INVALID();
	double radius = turnRadius(ixBOT);
	Position center = turnCenter(ixBOT);
	//fpln(" $$ vertexFromTurnTcps: center = "+center.toString());
	Position botPos = getPos(ixBOT);
	Position eotPos = getPos(ixEOT);
	//fpln(" $$ vertexFromTurnTcps: botPos = "+botPos.toString()+" center = "+center.toString()+" eotPos = "+eotPos.toString());
	//fpln(" $$ vertexFromTurnTcps: theta = "+Units::str("deg",theta)+" distance = "+Units::str("NM",distance));
	double trkIn_d = trkOut(ixBOT);
	double trkOut_d = trkOut(ixEOT);
	int dir = Util::turnDir(trkIn_d,trkOut_d);
	//fpln(" $$ vertexFromTurnTcps: "+Units::str("deg",trkIn)+" "+Units::str("deg",trkOut)+" dir = "+dir);
	// ---------- recover altitude ---------
	if (altMid < 0) {
	   int ixMOT = findMOT(ixBOT,ixEOT);
	   if (ixMOT < 0) ixMOT = ixEOT-1;
	   altMid = point(ixMOT).alt();
	}
	return vertexFromTurnTcps(botPos, eotPos, radius, dir, center, altMid);

}

NavPoint Plan::vertexPointTurnTcps(int ixBOT, int ixEOT, double altMid) const {
	Position vertex = vertexFromTurnTcps(ixBOT, ixEOT, altMid);
	double gsIn_d = gsIn(ixBOT);
	Position botPos = getPos(ixBOT);
	double distToVertex = botPos.distanceH(vertex);
	double gsAccel_d = 0.0;
	double tmBOT = time(ixBOT);
	if (inGsChange(tmBOT)) {
		int ixBGS = prevBGS(ixBOT+1);
		gsAccel_d = gsAccel(ixBGS);
	}
	double dt = timeFromDistance(gsIn_d, gsAccel_d, distToVertex);
	return NavPoint(vertex,tmBOT+dt);
}

/**
 *
 * @param botPos   position of turn beginning
 * @param eotPos   position of end of turn
 * @param radius   radius of turn
 * @param dir      direction +1 = right, -1 = left
 * @param center   center position
 * @param altMid   altitude at middle of turn
 * @return         vertex of turn
 */
Position Plan::vertexFromTurnTcps(const Position& botPos, const Position& eotPos, double radius, int dir, const Position& center, double altMid) {
	double theta = PositionUtil::angle_between(botPos, center, eotPos);
	double cos_theta2 = cos(theta/2);
	double distance = 100.0;
	if (cos_theta2 != 0.0) {
		distance = radius/cos_theta2;
	}
	Position vertex;
	//f.pln(" $$ vertexFromTurnTcps: theta = "+Units.str("deg",theta)+" distance = "+Units.str("NM",distance));
	if (botPos.isLatLon()) {
		double centerToBOT_trk = GreatCircle::initial_course(center.lla(),botPos.lla());
		double cLineTrk = centerToBOT_trk + dir*theta/2;
		//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units.str("deg",centerToBOT_trk));
		vertex = Position(GreatCircle::linear_initial(center.lla(), cLineTrk, distance));
	} else {
		double centerToBOT_trk = Velocity::mkVel(center.point(), botPos.point(), 100.0).trk();
		//double centerToEOT_trk = Velocity.mkVel(center.point(), eotPos.point(), 100.0).trk();
		double cLineTrk = centerToBOT_trk + dir*theta/2;
		Vect3 sn = center.point().linearByDist2D(cLineTrk, distance);
		//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units.str("deg",centerToBOT_trk)+" cLineTrk = "+Units.str("deg",cLineTrk));
		vertex = Position(sn);
	}
	vertex = vertex.mkAlt(altMid);
	return vertex;
}

Position Plan::vertexFromTurnTcps(const Position& botPos, const Position& eotPos, double signedRadius, double trkInit, double altMid) {
	int dir = Util::sign(signedRadius);
	double radius = std::abs(signedRadius);
	Position center = KinematicsPosition::centerFromRadius(botPos, signedRadius, trkInit);
	return vertexFromTurnTcps(botPos, eotPos, radius, dir, center, altMid);
}


int Plan::findMOT(int ixBOT, int ixEOT) const {
	if (ixEOT == ixBOT +2) {
		//fpln(" $$ findMOT ("+ixBOT+","+ixEOT+"): AA return (ixBOT+1) = "+(ixBOT+1));
		return ixBOT+1;
	} else {
		int linIx = linearIndex(ixBOT);
		if (linIx >= 0) {
			for (int ixMOT = ixBOT; ixMOT < ixEOT; ixMOT++) {
				if (linearIndex(ixMOT) == linIx) {
					//fpln(" $$ findMOT ("+ixBOT+","+ixEOT+"): BB return via linearIndex = "+(ixMOT));
					return ixMOT;
				}
			}
		}
 	}
 	double pathDistBOT2EOT = pathDistance(ixBOT,ixEOT);
	double target = 0.49*pathDistBOT2EOT;
	for (int ixMOT = ixBOT; ixMOT < ixEOT; ixMOT++) {
		if (pathDistance(ixBOT,ixMOT) >= target) {
			//fpln(" $$ findMOT("+ixBOT+","+ixEOT+"): CC return via 1/2 pathDistance = "+(ixMOT));
			return ixMOT;
		}
	}
	//fpln(" $$ findMOT("+ixBOT+","+ixEOT+"): DD return last resort ixEOT-1 = "+(ixEOT-1));
	return ixEOT-1;
}

// Triple(distanceTo, altAt, gsIn)
std::vector<Quad<double,double,double, std::string> > Plan::buildDistList(int ixBOT, int ixEOT, double ratio) {
   	std::vector<Quad<double,double,double,std::string> > distList = std::vector<Quad<double,double,double,std::string> >();
   	for (int i = ixBOT; i < ixEOT; i++) {
   		double d_i = ratio*pathDistance(ixBOT,i);
   		double alt_i = point(i).alt();
   		double gsIn_i = gsOut(i);
   		std::string label_i = point(i).label();
   		Quad<double,double,double,std::string> trip_i = Quad<double,double,double,std::string>(d_i,alt_i,gsIn_i,label_i);
   		distList.push_back(trip_i);
   	}
	return distList;
}

std::vector<Quad<double,double,double,std::string> > Plan::buildDistList(int ixBOT, int ixEOT, const Position& vertex) {
 	double turnDist = pathDistance(ixBOT,ixEOT);
 	double vertexDist = point(ixBOT).position().distanceH(vertex) + point(ixEOT).position().distanceH(vertex);
 	double ratio = vertexDist/turnDist;
 	return buildDistList(ixBOT, ixEOT, ratio);
}



/**
 * Structurally revert TCP at ix: (does not depend upon source time or
 * source position!!) This private method assumes ix &gt; 0 AND ix &lt;
 * pln.size(). If ix is not a BOT, then nothing is done
 *
 * @param ixBOT    index of point to be reverted
 * @param addBackMidPoints
 *            if addBackMidPoints = true, then if there are extra points
 *            between the BOT and EOT, make sure they are moved to the
 *            correct place in the new linear sections. Do this by distance
 *            not time.
 * @param killNextGsTCPs
 *            if true, then if there is a BGS-EGS pair after the turn (i.e. creat by TrajGen), then
 *            remove these points
 * @param zVertex
 *            if non-negative, then assigned reverted vertex this altitude
 */
void Plan::structRevertTurnTCP(int ixBOT, bool addBackMidPoints, bool killNextGsTCPs) {
	// fpln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT = "+pln.point(ix).isBOT());
	if (!isBOT(ixBOT)) return;
	NavPoint BOT = point(ixBOT);
	int BOTlinIndex = getTcpData(ixBOT).getLinearIndex();
	std::string label = BOT.label();
	int ixEOT = nextEOT(ixBOT);
	double dist2Mid = pathDistance(ixBOT,ixEOT)/2.0;
	//fpln(" $$$ structRevertTurnTCP: dist2Mid = "+Units::str("ft",dist2Mid));
	bool linear = false;
	double tMid = timeFromDistance(ixBOT, dist2Mid);
	//fpln(" $$$ structRevertTurnTCP: timeFromDistance =  "+timeFromDistance(ixBOT, dist2Mid));
	Position posMid = position(tMid);
	double altMid = posMid.alt();
	//fpln(" $$$ structRevertTurnTCP: tMid = "+tMid+" altMid = "+Units::str("ft",altMid));
	double gsOutMid = velocity(tMid).gs();
	//fpln(" $$$ structRevertTurnTCP: gsOutMid = "+Units::str("kn",gsOutMid));
	NavPoint vertex =  vertexPointTurnTcps(ixBOT,ixEOT,altMid);
	//fpln(" $$$ structRevertTurnTCP: ixMOT = "+ixMOT+" vertex = "+vertex);
	//fpln(" $$$$$$$ ixBOT = "+ixBOT+" ixEOT = "+ixEOT);
	vector<Quad<double,double,double,std::string> > distList = buildDistList(ixBOT, ixEOT, vertex.position());
	// ======================== No Changes To Plan Before This Point ================================
	if (killNextGsTCPs & (ixEOT + 1 < size())) {  // remove BGS-EGS pair 1 sec after EOT (created by TrajGen)
		TcpData tcpAfter = getTcpData(ixEOT + 1);
		if (tcpAfter.isBGS() && (tcpAfter.getLinearIndex() == BOTlinIndex)) {
			int ixEGS = nextEGS(ixEOT);
			// fpln(" $$$$$ let's KILL TWO GS points AT  "+(ixEOT+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
			remove(ixEGS);
			remove(ixEOT + 1);
		}
	}
	double gsInEOT = gsIn(ixEOT);
	double gsOutEOT = gsOut(ixEOT);
	//double gsOutMOT = gsOut(ixMOT);
	double gsOutBOT = gsOut(ixBOT);
	// ========================= Kill all points between ix and ixEOT =============================
	for (int k = ixEOT-1; k > ixBOT; k--) {
		// fpln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k).tostd::stringFull());
		remove(k);
	}
	addNavPoint(vertex.makeLabel(label));
	double tmBOT = point(ixBOT).time();
	GsPlan gsp = GsPlan(tmBOT);
	if (addBackMidPoints) {
		bool vertexAdded = false;
		for (int i = 0; i < (int) distList.size(); i++) {  // reverse order to preserve indexes
			Quad<double,double,double,std::string> trip_i = distList[i];
			double d_i = trip_i.first;
			double alt_i = trip_i.second;
			double gsIn_i = trip_i.third;
			std::string label_i = trip_i.fourth;
			double deltaToMid = std::abs(d_i - dist2Mid);
			if (deltaToMid < 20) {  // close to where vertex will be (probably MOT)
				//fpln(" $$$$$$  GSP SKIP VERTEX i = "+i);
			} else if (!vertexAdded && d_i > deltaToMid) {
				gsp.add(vertex.position(),vertex.label(),gsOutMid);
				//fpln("  GSP ADD VERTEX i = "+i);
				vertexAdded = true;
			} else {
				linear = true;
				std::pair<Position, int> adv = advanceDistance2D(ixBOT, d_i, linear);
				Position pos_i = adv.first.mkAlt(alt_i);
				gsp.add(pos_i,label_i,gsIn_i);
			}
			//fpln(" $$ structRevertTurnTCP: gsp ADD i = "+i+"  gsIn_i_i = "+Units::str("kn",gsIn_i));
		}
		if (!vertexAdded) {
			//fpln("  GSP ADD VERTEX i AT END");
			gsp.add(vertex.position(),vertex.label(),gsOutMid);
		}

		//gsp.set(ixMOT-ixBOT,vertex.position(),vertex.label(),gsOutMid); // replace MOT with vertex
	} else {
		gsp.add(point(ixBOT).position(),"",gsOutBOT);
		gsp.add(vertex.position(),vertex.label(),gsOutMid);
	}
	//fpln(" $$$ structRevertTurnTCP: AFTER gsp = "+gsp);
	//fpln(" $$$$$$$$$$$$$$$$$$$$$$$ 00 structRevertTurnTCP: this = "+this);
	Plan pMids = gsp.linearPlan();
	ixEOT = nextEOT(ixBOT);   // EOT may now have new index
	getTcpDataRef(ixBOT).clearTrk();
	clearLabel(ixBOT);
	getTcpDataRef(ixEOT).clearEOT();
	//fpln(" $$$$$$$$$$$$$$$$$$$$$$$ ixEOT = "+ixEOT);
	//fpln(" $$$$$$$$$$$$$$$$$$$$$$$ 11 structRevertTurnTCP: this = "+this);
	// ======================== fix ground speeds ============================
	remove(ixBOT+1);   // remove vertex because it is now in the gsPlan
	for (int j = 1; j < pMids.size(); j++) {  // note we start with 1, i.e. do not add ixBOT
		addNavPoint(pMids.point(j));
	}
	mkGsIn(ixEOT, gsInEOT);
	mkGsOut(ixEOT, gsOutEOT);
	removeIfRedundant(ixEOT);    // remove this one first to preserve indexes
	removeIfRedundant(ixBOT);
	mergeClosePoints();
}


//void Plan::structRevertTurnTCP_OLD(int ix, bool addBackMidPoints, bool killNextGsTCPs) {
//	//fpln(" $$$$$ structRevertTurnTCP: ix = "+ix+" isBOT ="+pln.point(ix).isBOT()+" "+killNextGsTCPs);
//	//fpln(" $$$$$ structRevertTurnTCP: pln = "+pln);
//	if (isBOT(ix)) {
//		//fpln(" $$$$$ structRevertTurnTCP: ix = "+ix);
//		NavPoint BOT = point(ix);
//		int BOTlinIndex = getTcpData(ix).getLinearIndex();
//		double tBOT = BOT.time();
//		int ixEOT = nextEOT(ix);//fixed
//		NavPoint EOT = point(ixEOT);
//		double tEOT = EOT.time();
//		vector<std::pair<NavPoint,TcpData> > betweenPoints = vector<std::pair<NavPoint,TcpData> >(4);
//		vector<double> betweenPointDists = vector<double>(4);
//		if (addBackMidPoints) {  // add back mid points that are not TCPs
//			for (int j = ix+1; j < ixEOT; j++) {
//				std::pair<NavPoint,TcpData> np = get(j);
//				if ( ! isTCP(j)) {
//					//fpln(" >>>>> structRevertTurnTCP: SAVE MID point("+j+") = "+point(j).toStringFull());
//					betweenPoints.push_back(np);
//					double distance_j = pathDistance(ix,j);
//					betweenPointDists.push_back(distance_j);
//				}
//			}
//		}
//		Velocity vin;
//		if (ix == 0) vin = initialVelocity(ix);     // not sure if we should allow TCP as current point ??
//		else vin = finalVelocity(ix-1);
//		//fpln(" $$$$ structRevertTurnTCP: gsin = "+Units::str("kn",gsin,8));
//		Velocity vout = initialVelocity(ixEOT);
//
//
//		NavPoint vertex =  vertexPointTurnTcps(ix,ixEOT,-1);
//		//fpln(" $$$ structRevertTurnTCP: vertex = "+vertex.toString());
//
//
//		// ======================== No Changes To Plan Before This Point ================================
//		double gsInNext = vout.gs();
//		if (killNextGsTCPs & (ixEOT+1 < size())) {
//			NavPoint npAfter = point(ixEOT+1);
//			TcpData tcpAfter = getTcpData(ixEOT+1);        // compiler bug;  replace with point(ixEOT+1) to see it
//			if (tcpAfter.isBGS() && (tcpAfter.getLinearIndex() == BOTlinIndex) ) {
//			    int ixEGS = nextEGS(ixEOT);//fixed
//				vout = initialVelocity(ixEGS);
//				gsInNext = vout.gs();
//				//fpln(" $$$$$ let's KILL TWO GS points AT "+(ixEOT+1)+" and ixEGS = "+ixEGS+" dt = "+dt);
//				remove(ixEGS);
//				remove(ixEOT+1);
//			}
//		}
//		// fpln(" $$$$$ structRevertTurnTCP: ix = "+ix+" ixEOT = "+ixEOT);
//		// Kill all points between ix and ixEOT
//		for (int k = ixEOT; k >= ix; k--) {
//			//fpln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k).toStringFull());
//			remove(k);
//		}
//		//fpln(" $$$$ structRevertTurnTCP: ADD vertex = "+vertex);
//		int ixAdd = addNavPoint(vertex);
//		int ixNextPt = ixAdd+1;
//		// add back all removed points with revised position and time
//		if (addBackMidPoints) {
//			for (int i = 0; i < (int) betweenPointDists.size(); i++) {
//				double newTime = BOT.time() + betweenPointDists[i]/vin.gs();
//				Position newPosition = position(newTime);
//				std::pair<NavPoint,TcpData> savePt = betweenPoints[i];
//				NavPoint np = savePt.first.makePosition(newPosition).makeTime(newTime).mkAlt(savePt.first.alt());
//				add(np, savePt.second);
//				//fpln(" $$$$ structRevertTurnTCP: ADD BACK np = "+np);
//				ixNextPt++;
//			}
//		}
//		// fix ground speed after
//		//fpln(" $$$$ structRevertTurnTCP: ixNextPt = "+ixNextPt+" gsInNext = "+Units::str("kn", gsInNext));
//	    double tmNextSeg = time(ixNextPt);
//	    if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
//	    	int newNextSeg = getSegment(tmNextSeg);
//	    	double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
//	    	double dt2 = newNextSegTm - tmNextSeg;
//	    	//fpln(" $$$$$$$$ structRevertTurnTCP: dt2 = "+dt2);
//	    	timeshiftPlan(newNextSeg, dt2);
//	    }
//	    //fpln(" $$$$ structRevertTurnTCP: initialVelocity("+ixNextPt+") = "+initialVelocity(ixNextPt));
//		removeRedundantPoints(getIndex(tBOT),getIndex(tEOT));
//	}
//}


// will not remove first or last point
void Plan::removeRedundantPoints(int from, int to) {
//	double velEpsilon = 1.0;
	int ixLast = Util::min(size() - 2, to);
	int ixFirst = Util::max(1, from);
	for (int i = ixLast; i >= ixFirst; i--) {
//		NavPoint p = point(i);
//		Velocity vin = finalVelocity(i-1);
//		Velocity vout = initialVelocity(i);
//		if ( ! isTCP(i) && vin.within_epsilon(vout, velEpsilon)) { // 2.6)) { // see testAces3, testRandom for worst cases
//            //fpln(" $$$$$ removeRedundantPoints: REMOVE i = "+Fm0(i));
//			remove(i);
//		}
		removeIfRedundant(i);
	}
}

void Plan::removeRedundantPoints() {
     removeRedundantPoints(0,200000000);           // MAX_INT ??  MAXINTEGER ??
}

void Plan::removeIfRedundant(int ix,  bool trkF, bool gsF, bool vsF) {
	//f.pln(" $$$$$ removeIfRedundant: ENTER ix = "+ix);
	if (ix < 0 || ix >= size()-1) return;   // should not remove last point
	Velocity vin = finalVelocity(ix - 1);
	Velocity vout = initialVelocity(ix);
	//f.pln(" $$$$$ removeIfRedundant: i = "+i+" vin = "+vin+" vout = "+vout);
	double deltaTrk = Util::turnDelta(vin.trk(),vout.trk());
	double deltaGs = std::abs(vin.gs()-vout.gs());
	double deltaVs = std::abs(vin.vs()-vout.vs());
	//f.pln(" $$$$$ removeIfRedundant: ix = "+ix+" deltaTrk = "+ Units.str("deg",deltaTrk)+" deltaGs = "+Units.str("kn",deltaGs));
		if ( ! isTCP(ix)                                   &&
		 ( ! trkF || deltaTrk < Units::from("deg",1.0)) &&
		 ( ! gsF  || deltaGs < Units::from("kn",5.0))   &&
		 ( ! vsF  || deltaVs < Units::from("fpm",100.0))     ) {
		//f.pln(" $$$$$ removeIfRedundant: REMOVE i = "+ix);
		remove(ix);
	}
}


void Plan::removeIfRedundant(int ix) {
	bool trkF = true;
	bool gsF = true;
	bool vsF = false;
	removeIfRedundant(ix,trkF, gsF, vsF);
}


void Plan::removeIfVsConstant(int ix) {
	bool trkF = true;
	bool gsF = false;
	bool vsF = true;
	removeIfRedundant(ix,trkF, gsF, vsF);

}



void Plan::structRevertGsTCP(int ixBGS,  bool saveAccel) {
	if (isBGS(ixBGS)) {
		//fpln("\n\n $$$$>>>>>>>>>>>>>>>>>>>>>> structRevertGsTCP:  point("+Fm0(ix)+") = "+point(ix).toString());
		int ixEGS = nextEGS(ixBGS);
		//TcpData BGSTcp = getTcpData(ix);
		if (ixEGS < 0) {
			//fpln(" $$$$---------------------- structRevertGsTCP : ERROR nextEGSix = "+nextEGSix);
			addError(" structRevertGsTCP: Ill-formed BGS-EGS structure: no EGS found!");
			return;
		}
		double gsOutEGS = gsOut(ixEGS);
		double gsOutBGS = gsOut(ixBGS);
		//fpln(" $$$$---------------------- structRevertGsTCP AFTER: pln  = "+toString());			getTcpDataRef(ixEGS).clearEGS();
		TcpData& tcpEGS= getTcpDataRef(ixEGS);
		tcpEGS.clearEGS();
		TcpData& tcpBGS= getTcpDataRef(ixBGS);
		tcpBGS.clearBGS();
		if (!saveAccel) tcpBGS.setGsAccel(0.0);
		mkGsOut(ixBGS, gsOutBGS);       // to make removeIfVsConstant test legitimate
		removeIfVsConstant(ixEGS);
		//f.pln(" $$$ structRevertGsTCP: add newPoint = "+newPoint+" iNew =  "+iNew);
		mkGsOut(ixBGS, gsOutEGS);
	}
	return;
}

void Plan::revertGsTCPs() {
	int start = 0;
	bool saveAccel = false;
	revertGsTCPs(start, saveAccel);
}

void Plan:: revertGsTCPs(int start, bool saveAccel) {
	//fpln(" $$$ revertGsTCPs start = "+start+" size = "+size());
	if (start < 0) start = 0;
	for (int j = start; j < size(); j++) {
		//fpln(" $$$ REVERT GS AT j = "+j+" np_ix = "+point(j));
		structRevertGsTCP(j, saveAccel);
	}
}



// assumes ix > 0 AND ix < size()
double Plan::structRevertVsTCP(int ixBVS) {
	if (isBVS(ixBVS)) {
		NavPoint BVS = point(ixBVS);
		int nextEVSix = nextEVS(ixBVS);//fixed
		NavPoint EVS = point(nextEVSix);
		NavPoint pp = point(ixBVS - 1);
		double vsin = (BVS.z() - pp.z()) / (BVS.time() - pp.time());
		double dt = EVS.time() - BVS.time();
		double tVertex = BVS.time() + dt/2.0;
		double zVertex = BVS.z() + vsin*dt/2.0;
		Position pVertex = position(tVertex);
		std::string label = point(ixBVS).label();
		NavPoint vertex = NavPoint(pVertex.mkAlt(zVertex), tVertex).makeLabel(label);
		// f.pln(" $$$$ structRevertVsTCP: sourcePos = "+sourcePos+" vertex = "+vertex);
		for (int j = ixBVS+1; j < nextEVSix; j++) {
			double t = time(j);
			double newAlt;
			if (t < tVertex) {
				newAlt = interpolateAlts(t, BVS.time(), BVS.alt(), tVertex, zVertex);
			} else {  // if (t > tVertex) {
				newAlt = interpolateAlts(t, tVertex, zVertex, EVS.time(), EVS.alt());
			}
			NavPoint np_j = point(j).mkAlt(newAlt);
			TcpData  tcp_j = getTcpData(j);
			set(j,np_j,tcp_j);
		}
		getTcpDataRef(ixBVS).clearBVS();
		clearLabel(ixBVS);
		getTcpDataRef(nextEVSix).clearEVS();
		removeIfRedundant(nextEVSix);
		removeIfRedundant(ixBVS);
		int ixVertex = addNavPoint(vertex);
		setAltPreserve(ixVertex);
		mergeClosePoints();
		return zVertex;

	}
	return -1;
}

double Plan::interpolateAlts(double t, double t1, double alt1, double t2, double alt2) {
	if (t2 <= t1) return -1;
	if (t > t2) t = t2;
	if (t < t1) t = t1;
	double vs = (alt2-alt1)/(t2-t1);
	double dt = t-t1;
	double newAlt = alt1 + dt*vs;
	return newAlt;
}


void Plan::revertVsTCPs() {
	revertVsTCPs(0, size() - 1);
}

void Plan::revertVsTCPs(int start, int end) {
	// fpln(" $$## revertVsTCPs: start = "+start+" end = "+end);
	if (start < 0)
		start = 0;
	if (end > size() - 1)
		end = size() - 1;
	for (int j = end; j >= start; j--) {
		// fpln(" $$$ REVERT j = "+j+" np_ix = "+point(j));
		structRevertVsTCP(j);
	}
}


///** revert all TCPS back to its original linear point which have the same sourceTime as the point at index i
// *  if the point is a not a TCP do nothing.  Note that this function will timeshift the points after dSeg to regain
// *  original ground speed into the point after dSeg.  This function checks to make sure that the source position
// *  is reasonably close to the current position.  If not, it reverts to the current position.
// *
// * @param dSeg  The index of one of the TCPs created together that should be reverted
// * @return index of the reverted point
// */
//int Plan::revertGroupOfTCPs(int dSeg, bool checkSource) {
//	double maxDistH = Units::from("NM", 15.0);
//	double maxDistV = Units::from("ft", 5000.0);
//	if (dSeg < 0 || dSeg >= size()) {
//		addError(".. revertGroupOfTCPs: invalid index "+Fm0(dSeg), 0);
//		return -1;
//	}
//	NavPoint origDsegPt = point(dSeg);
//	if ( ! isTCP(dSeg)) {
//		//fpln(" $$ revertGroupOfTCPs: point "+dSeg+" is not a TCP, do nothing!");
//		return dSeg;
//	}
//	double sourceTm = getTcpData(dSeg).sourceTime();
//	//int dSeg = getSegment(sourceTm);
//	//fpln("\n $$$ revertGroupOfTCPs: point(dSeg).time = "+point(dSeg).time() +" sourceTm = "+sourceTm);
//	int firstInGroup = -1;
//	int lastInGroup = size()-1;
//	for (int j = 0; j < size(); j++) {
//		if (Constants::almost_equals_time(getTcpData(j).sourceTime(),sourceTm)) {
//			if (firstInGroup == -1) firstInGroup = j;
//			lastInGroup = j;
//		}
//	}
//	//fpln("  $$$ revertGroupOfTCPs: firstInGroup = "+firstInGroup+" lastInGroup = "+lastInGroup);
//	double gsInFirst = finalVelocity(firstInGroup-1).gs();
//	//fpln(" $$$ revertGroupOfTCPs: gsInFirst = "+Units::str("kn", gsInFirst));
//	int nextSeg = lastInGroup+1;
//	double tmNextSeg = getTime(nextSeg);
//	//fpln(" $$$ revertGroupOfTCPs: size = "+size()+" nextSeg = "+nextSeg+" tmNextSeg = "+tmNextSeg);
//	double gsInNext = finalVelocity(nextSeg-1).gs();
//	NavPoint revertedLinearPt = NavPoint(getTcpData(dSeg).sourcePosition(),getTcpData(dSeg).sourceTime());
//	int lastii = -1;
//	for (int ii = lastInGroup; ii >= firstInGroup; ii--) {
//		if (Constants::almost_equals_time(getTcpData(ii).sourceTime(),sourceTm)) {
//			//fpln(" $$$ remove point ii = "+ii+" point(i).time() = "+point(ii).time()+" point(i).sourceTime() = "+point(ii).sourceTime());
//			remove(ii);
//			lastii = ii;
//		}
//	}
//	// safety check in case there is a invalid source position
//	double distH = origDsegPt.distanceH(revertedLinearPt);
//	double distV = origDsegPt.distanceV(revertedLinearPt);
//	if (checkSource && (distH > maxDistH || distV > maxDistV)) {
//		//fpln(" $$$$ revertGroupOfTCPs: for dSeg = "+Fm0(dSeg)+" distH = "+Units::str("nm", distH));
//		//fpln(" $$$$ revertGroupOfTCPs: for dSeg = "+Fm0(dSeg)+" distV = "+Units::str("ft", distV));
//		revertedLinearPt = origDsegPt.makeNewPoint();
//	}
//	addNavPoint(revertedLinearPt);
//	// timeshift points
//	if (tmNextSeg > 0) { // if reverted last point, no need to timeshift points after dSeg
//		int newNextSeg = getSegment(tmNextSeg);
//		double newNextSegTm = linearCalcTimeGSin(newNextSeg, gsInNext);
//		double dt2 = newNextSegTm - tmNextSeg;
//		timeshiftPlan(newNextSeg, dt2);
//	}
//	// timeshift plan to regain original ground speed into the reverted point
//	if (firstInGroup < lastInGroup) {
//		int segNewLinearPt = getSegment(revertedLinearPt.time());
//		double newTm = linearCalcTimeGSin(segNewLinearPt, gsInFirst);
//		double dt = newTm - revertedLinearPt.time();
//		//fpln(" $$$ revertGroupOfTCPs: TIMESHIFT dt = "+dt);
//		timeshiftPlan(segNewLinearPt, dt);
//	}
//	//fpln(" $$$ revertGroupOfTCPs: lastii = "+lastii);
//	return lastii;
//}


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
		if (isBOT(i)) {
			int j1 = nextBOT(i);
			int j2 = nextEOT(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (isEOT(i)) {
			int j1 = prevBOT(i);
			int j2 = prevEOT(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		if (isBGS(i)) {
			int j1 = nextBGS(i);
			int j2 = nextEGS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (isEGS(i)) {
			int j1 = prevBGS(i);
			int j2 = prevEGS(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		if (isBVS(i)) {
			int j1 = nextBVS(i);
			int j2 = nextEVS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
		}
		if (isEVS(i)) {
			int j1 = prevBVS(i);
			int j2 = prevEVS(i);
			if (!(j1 >= 0 && j1 >= j2)) return i;
		}
		double tm_i = time(i);
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
		//			if ((isTurn() || isVSC()) && inGroundSpeedChange(np.time())) rtn = false;
		//			if (isGSC() && (inTurn(np.time()) || inVerticalSpeedChange(np.time()))) rtn = false;
		if (isBOT(i)) {
			int j1 = nextBOT(i);
			int j2 = nextEOT(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BOT at i "+Fm0(i)+" NOT FOLLOWED BY EOT!";
		}
		if (isEOT(i)) {
			int j1 = prevBOT(i);
			int j2 = prevEOT(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EOT at i "+Fm0(i)+" NOT PRECEEDED BY BOT!";
		}
		if (isBGS(i)) {
			int j1 = nextBGS(i);
			int j2 = nextEGS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BGS at i "+Fm0(i)+" NOT FOLLOWED BY EGS!";
		}
		if (isEGS(i)) {
			int j1 = prevBGS(i);
			int j2 = prevEGS(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EGS at i "+Fm0(i)+" NOT PRECEEDED BY BGS!";
		}
		if (isBVS(i)) {
			int j1 = nextBVS(i);
			int j2 = nextEVS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BVS at i "+Fm0(i)+" NOT FOLLOWED BY EVS!";
		}
		if (isEVS(i)) {
			int j1 = prevBVS(i);
			int j2 = prevEVS(i);
			if (!(j1 >= 0 && j1 >= j2)) return "EVS at i "+Fm0(i)+" NOT PRECEEDED BY BVS!";
		}

		if (inGsChange(np.time()) && inTrkChange(np.time())) {
			rtn = rtn + "  Overlap FAIL at i = "+Fm0(i);
		}
		//			fpln(" isWellFormed: i = "+i+" OK");e
	}
	return rtn;
}



bool Plan::isConsistent(double maxTrkDist, double maxGsDist, double maxVsDist, bool silent) const {
	bool rtn = true;
	if (!isWellFormed()) {
		error.addError("isWeakConsistent: not well formed");
		//fpln("  $$$ isConsistent: FAIL! not WellFormed!!");
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if (isBOT(i)) {
			if ( ! PlanUtil::turnConsistent(*this, i, maxTrkDist, silent)) {
				//error.addWarning("isWeakConsistent: turn "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
		if (isBGS(i)) {
			if ( ! PlanUtil::gsConsistent(*this, i, maxGsDist, silent)) {
				//error.addWarning("isWeakConsistent: GS "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
		if (isBVS(i)) {
			if ( ! PlanUtil::vsConsistent(*this, i, maxVsDist, vsAccel(i), silent)) {
				//error.addWarning("isWeakConsistent: VS "+Fm0(i)+" not consistent");
				rtn = false;
			}
		}
	}
    return rtn;
}

bool Plan::isConsistent() const {
	return isConsistent(false);
}


bool Plan::isConsistent(bool silent) const {
	return isConsistent(0.01,0.007,0.00001,silent);
}


bool Plan::isWeakConsistent(bool silent) const {
	return isConsistent(0.05,0.05,0.01,silent);
}

bool Plan::isWeakConsistent() const {
	bool silent = false;
	return isWeakConsistent(silent);
}


bool Plan::isVelocityContinuous() const {
    return isVelocityContinuous(true);
}


bool Plan::isVelocityContinuous(bool silent) const {
	//fpln(" $$isWeakVelocityContinuous: ENTER");
	for (int i = 0; i < size(); i++) {
		if (i > 0) {
			if (isTCP(i)) {
				if (!PlanUtil::isTrkContinuous(*this, i, Units::from("deg", 5.0), silent)) return false;
				if (!PlanUtil::isGsContinuous(*this, i, Units::from("kn", 10), silent)) return false;
				if (!PlanUtil::isVsContinuous(*this, i, Units::from("fpm", 100), silent)) return false;
			}
		}
	}
    return true;
}

bool Plan::isWeakVelocityContinuous(bool silent) const {
	for (int i = 0; i < size(); i++) {
		if (i > 0) {
			if (isTCP(i)) {
				if (!PlanUtil::isTrkContinuous(*this, i, Units::from("deg", 10.0), silent)) return false;
				if (!PlanUtil::isGsContinuous(*this, i, Units::from("kn", 20), silent)) return false;
				if (!PlanUtil::isVsContinuous(*this, i, Units::from("fpm", 300), silent)) return false;
			}
		}
	}
    return true;
}


bool Plan::isFlyable() const {
	fpln(" isFlyable: ENTER");
	bool silent = false;
	return isFlyable(silent);
}


/**
 * This returns true if the entire plan is "sound"
 */
bool Plan::isFlyable(bool silent) const {
	return isConsistent(silent) && isVelocityContinuous(silent);;
}

/**
 * This returns true if the entire plan is "sound"
 */
bool Plan::isWeakFlyable(bool silent) const {
	return isWeakConsistent(silent) && isWeakVelocityContinuous(silent);
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
	double t1 = time(seg);
	double dt = time(seg+1)-t1;
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
			double a = vsAccel(prevBVS(seg+1));
			double tm = KinematicsDist::gsTimeConstantAccelFromDist(vs1, a, p.alt()-np.alt());
			ret = NavPoint(position(tm),tm);
		} else {
			double vtot = std::abs(np.alt()-np2.alt());
			double frac = std::abs(np.alt()-p.alt())/vtot;
			double tm = dt*frac-t1;
			ret = NavPoint(position(tm),tm);
		}
	} else if (inTrkChange(t1)) {
		int ixBOT = prevBOT(getSegment(t1)+1);
		//NavPoint bot = points[ixBOT];//fixed
		Position center = turnCenter(ixBOT);
		double endD = pathDistance(seg);
		double d2 = ProjectedKinematics::closestDistOnTurn(np.position(), initialVelocity(seg), getTcpData(ixBOT).turnRadius(), Util::sign(signedRadius(ixBOT)), center, p, endD);
		if (Util::almost_equals(d2, 0.0)) {
			ret = np;
		} else if (Util::almost_equals(d2, endD)) {
			ret = np2;
		} else {
			double segDt = timeFromDistanceWithinSeg(seg, d2);
			ret = NavPoint(position(t1+segDt), t1+segDt);
		}

	} else if (isLatLon()) {
		LatLonAlt lla = GreatCircle::closest_point_segment(points[seg].lla(), points[seg+1].lla(), p.lla());
		d = GreatCircle::distance(points[seg].lla(), lla);
		double segDt = timeFromDistanceWithinSeg(seg, d);
//		double frac = d/pathDistance(seg);
//		ret = NavPoint(Position(lla), t1 + frac*dt);
		ret = NavPoint(position(t1+segDt), t1+ segDt);
	} else {
		Vect3 cp = VectFuns::closestPointOnSegment(points[seg].point(), points[seg+1].point(), p.point());
		d = points[seg].point().distanceH(cp);
		double segDt = timeFromDistanceWithinSeg(seg, d);
//		double frac = d/pathDistance(seg);
//		ret = NavPoint(position(t1 + frac*dt), t1 + frac*dt);
		ret = NavPoint(position(t1+segDt), t1+ segDt);
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
	return isTCP(ix) || inAccel(time(ix));
}



Plan Plan::planFromState(const std::string& id, const Position& pos, const Velocity& v, double startTime, double endTime) {
	Plan p(id);
	if (endTime <= startTime) {
		return p;
	}
	NavPoint np = NavPoint(pos, startTime);
	//np = np.makeMutability(NavPoint::Fixed, NavPoint::Fixed, NavPoint::Fixed);
	p.addNavPoint(np);
	p.addNavPoint(np.linear(v, endTime-startTime));
	return p;
}

Plan Plan::copyWithIndex() const {
	Plan lpc = Plan(name,note);
	for (int j = 0; j < size(); j++) {
		lpc.add(point(j), getTcpData(j).setSource(point(j)).setLinearIndex(j));
	}
	return lpc;
}


Plan Plan::cut(int firstIx, int lastIx) const {
	Plan lpc = Plan(name,note);
	for (int i = firstIx; i <= lastIx; i++) {
		std::pair<NavPoint,TcpData> np = get(i);
		lpc.add(np);
	}
	return lpc;
}


void  Plan::mergeClosePoints(double minDt) {
	//fpln(" $$$$$$ mergeClosePoints "+getName()+" minDt  = "+minDt);
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
				else if (!isBeginTCP(i) && isBeginTCP(i+1)) ixDelete = i;
				// save attributes of "ixDelete"
				NavPoint npDelete2 = point(ixDelete);
				TcpData npDelete = getTcpData(ixDelete);
				remove(ixDelete);
				// the index of the remaining point is "i"
				//NavPoint newNpi = point(i).mergeTCPInfo(npDelete);
				TcpData newData = getTcpData(i).mergeTCPInfo(npDelete);
				NavPoint newNpi = point(i);
				newNpi = newNpi.appendLabel(npDelete2.label());
				set(i,newNpi,newData);
				//fpln(" $$$$$ mergeClosePoints: DELETE point ixDelete = "+ixDelete);
			}
		}
		//NavPoint npi = point(i);
		//set(i,npi.makeLinearIndex(i));
	}
}

void  Plan::mergeClosePoints() {
	Plan::mergeClosePoints(minDt);
}


std::vector<int> Plan::findLinearIndex(int ix) const {
	std::vector<int> al;
	for (int i = 0; i < size(); i++) {
		int linearIx = getTcpData(i).getLinearIndex();
		if (linearIx == ix)
			al.push_back(i);
	}
	return al;
}


std::string Plan::toString() const {
	return toStringFull(false);
}

std::string Plan::toStringFull(bool showSource) const {
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
			sb << "Waypoint "+Fm0(j)+": " << toStringFull(j,showSource);
			sb << endl;
		}
	}
	return sb.str();
}

std::string Plan::toStringFull(int i) const {
	return toStringFull(i,false);
}

std::string Plan::toStringFull() const {
	return toStringFull(false);
}


std::string Plan::toStringFull(int i, bool showSource) const {
	return toStringFull(point(i), getTcpData(i), showSource);
}

std::string Plan::toStringFull(const NavPoint& p, const TcpData& d) {
	return toStringFull(p,d,false);
}

std::string Plan::toStringFull(const NavPoint& p, const TcpData& d, bool showSource) {
	//StringBuffer sb = new StringBuffer(100);
	std::stringstream sb;

	sb << "[(";
	if (p.isLatLon()) sb << "LL: ";
	sb << p.toStringShort(4);
	sb << "), ";
	sb << d.getTypeString();
	if (d.isTrkTCP()) {
		sb << ", " << d.getTrkTypeString();
//		if (d.isBOT()) {
//			sb << " accTrk = " << Fm4(Units::to("deg/s", d.trkAccel()));
//		}
	}
	if (d.isGsTCP()) {
		sb << ", " << d.getGsTypeString();
		if (d.isBGS()) {
			sb << " accGs = " << Fm4(Units::to("m/s^2", d.getGsAccel()));
		}
	}
	if (d.isVsTCP()) {
		sb << ", " << d.getVsTypeString();
		if (d.isBVS()) {
			sb << " accVs = " << Fm4(Units::to("m/s^2", d.getVsAccel()));
		}
	}
//	if ( ! d.getVelocityInit().isInvalid()) sb << " vin = " << d.getVelocityInit().toStringUnits();
	if (d.getRadiusSigned() != 0.0) {
		sb << " sgnRadius = " << Fm4(Units::to("NM", d.getRadiusSigned()));
		Position center = d.turnCenter();
		if (!center.isInvalid()) sb << " center = " << center.toString2D(4);
	}
	if (showSource && d.getSourceTime() >= 0) {
		sb << " srcTime = " << Fm2(d.getSourceTime());
		NavPoint src = d.sourceNavPoint();
		if (!src.isInvalid()) {
			sb << " srcPos = " << src.toString();
		}
	}
    sb << "<" << Fm0(d.getLinearIndex()) << ">";
	sb << "]";
	sb << " " << p.label() << " " << d.getInformation();
	return sb.str();
}

std::vector<std::string> Plan::toStringList(int i, int precision, bool tcp) const {
	std::vector<std::string> ret;// name is (0)
	ret.push_back(name);
	std::vector<std::string> sl = toStringList(point(i), getTcpData(i), precision, tcp);
	ret.insert(ret.end(), sl.begin(), sl.end());
	return ret;
}


std::vector<std::string> Plan::toStringList(const NavPoint& p, const TcpData& d, int precision, bool tcp) {
	std::vector<std::string> ret;// name is normally (0)
	std::vector<std::string> vec = p.position().toStringList(precision);
	ret.insert(ret.end(), vec.begin(),vec.end());
	ret.push_back(FmPrecision(p.time(),precision)); // time (4)
	if (tcp) {
		ret.push_back(d.getTypeString()); // type (string) (5)
		//vec = d.getVelocityInit().toStringList(precision);
		Velocity vv = Velocity::ZERO();
		vec = vv.toStringList(precision);
		//ret.insert(ret.end(),vec.begin(),vec.end()); // vin (6-8) DO NOT CHANGE THIS -- POLYGONS EXPECT VEL TO BE HERE
		ret.push_back(d.getTrkTypeString()); // tcp trk (string) (9)
		ret.push_back(d.getGsTypeString()); // tcp gs (string) (11)
		ret.push_back(d.getVsTypeString()); // tcp vs (string) (13)
		ret.push_back(FmPrecision(Units::to("NM", d.getRadiusSigned()), precision)); // radius (15)
		//ret.push_back(FmPrecision(Units::to("deg/s",d.trkAccel()),precision)); // trk accel (10)
		ret.push_back(FmPrecision(Units::to("m/s^2",d.getGsAccel()),precision)); // gs accel (12)
		ret.push_back(FmPrecision(Units::to("m/s^2",d.getVsAccel()),precision)); // vs accel (14)
		vec = d.getSourcePosition().toStringList(precision);
		ret.insert(ret.end(),vec.begin(),vec.end()); // source position (16-18)
		ret.push_back(FmPrecision(d.getSourceTime(),precision)); // source time (19)

		vec = d.turnCenter().toStringList(precision);
		ret.insert(ret.end(),vec.begin(),vec.end()); // turn Center (16-18)
		if (d.getInformation().size() > 0) {
			ret.push_back(d.getInformation()); // label (string) (20)
		} else {
			ret.push_back("-");
		}
		if (p.label().size() > 0) {
			ret.push_back(p.label()); // label (string) (20)
		} else {
			ret.push_back("-");
		}
	} else { // no tcp
		std::string fl = TcpData::fullLabel(p,d);
		if (fl.size() > 0) {
			ret.push_back(fl); // label (string) (5)
		} else {
			ret.push_back("-");
		}
	}
	return ret;
}

std::string Plan::toStringGs() const {
	return toStringV(1);
}

std::string Plan::toStringV(int velField) const {
	std::ostringstream sb;
	sb << " Plan for aircraft: ";
	sb << name;
	if (error.hasMessage()) {
		sb << " error.message = " + error.getMessageNoClear();
	}
	sb << std::endl;
	if (note.length() > 0) {
		sb <<"Note=" << note << std::endl;
	}
	if (size() == 0) {
		sb << "<empty>";
	} else {
		for (int j = 0; j < size(); j++) {
			sb << "Waypoint " << j << ": " << point(j).toString();
			sb << " TCP:" << getTcpData(j).tcpTypeString();
			sb << " linearIndex = " << getTcpData(j).getLinearIndex();
			if (j < size()) {
				if (velField == 0)
					sb << "  TRK: " <<  Units::str("deg", trkOut(j), 4);
				if (velField == 1) {
					if (j > 0)
						sb << ",  GSin: " + Units::str("kn", gsFinal(j - 1), 4);
					else
						sb << ",  GSin: -------------";
					if (j < size() - 1)
						sb << ",  GSout: " + Units::str("kn", gsOut(j), 4);
					else
						sb << ",  GSout: -------------";
					// sb.append(", GSaccel:
					// "+Units.str("m/s^2",point(j).gsAccel(),4));
				}
				if (velField == 2) {
					if (isAltPreserve(j))
						sb << ", *AltPreserve*";
					sb << ",  vsOut: " + Units::str("fpm", vsOut(j), 4);
				}
			}
			sb << std::endl;
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
		if (includeVirtuals || isVirtual(i)) {
			if (name.size() == 0) {
				sb << "Aircraft";
			} else {
				sb << name;
			}
			sb << ", ";
			//sb << (point(i).toOutput(precision,tcpColumns));
			sb << list2str(toStringList(point(i), getTcpData(i),precision,tcpColumns),",");


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

