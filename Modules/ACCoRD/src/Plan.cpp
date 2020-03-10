
/*
 * Plan.cpp - -- the primary data structure for storing trajectories, both linear and kinematic
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2019 United States Government as represented by
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
#include "Tuple5.h"
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
#include "TrajGen.h"
#include "Debug.h"
#include "ParameterData.h"

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::vector;

bool   Plan::debug = false;
bool   Plan::newConsistencyAlg = true;
double Plan::MIN_TRK_DELTA_GEN = Units::from("deg", 1);
double Plan::MIN_GS_DELTA_GEN = Units::from("kn", 10);
double Plan::MIN_VS_DELTA_GEN = Units::from("fpm", 200);
std::string Plan::manualRadius = ".<manRadius>.";
std::string Plan::manualGsAccel = ".<manGsAccel>.";
std::string Plan::manualVsAccel = ".<manVsAccel>.";
const std::string Plan::noteParamStart = ":PARAM_START:";
const std::string Plan::noteParamEnd = ":PARAM_END:";


TcpData Plan::invalid_value = TcpData::makeInvalid();

const double Plan::minDt = GreatCircle::minDt;
const double Plan::nearlyZeroGs = 1E-4;                   // used in GsConsistent
const double em6DegtoMeter = 0.15;

//double Plan::gsIn_0 = -1;



std::string Plan::specPre() {
	return "$";
}

Plan::Plan() : 
	label(""),
	error("Plan"),
	note("") {
	init();
}

Plan::Plan(const std::string& planname) : 
	label(planname),
	error("Plan"),
	note("") {
	init();
}

Plan::Plan(const std::string& planname, const std::string& plannote) : 
	label(planname),
	error("Plan"),
	note(plannote) {
	init();
}


void Plan::init() {
	error.setConsoleOutput(debug); // debug ON!
	errorLocation = -1;
}

Plan::Plan(const Plan& fp) : 
	label(fp.label),
	points(fp.points),
	data(fp.data),
	error(fp.error),
	errorLocation(fp.errorLocation),
	note(fp.note),
	//debug(fp.debug),
	bound(BoundingBox(fp.bound)) {
// 	points = fp.points;
// 	data = fp.data;
// 	name = fp.name;
// 	note = fp.note;
// 	error = fp.error;
// 	errorLocation = fp.errorLocation;
 	debug = fp.debug;
// 	bound = BoundingBox(fp.bound);
}

Plan::~Plan() {
	// no pointers to delete
}

bool Plan::operator ==(const Plan& o) const {
	Plan fp = (Plan) o;

	size_t numPts = points.size();
	if (numPts != fp.points.size()) {
		return false;
	}

	bool r = true;

	for (size_t j = 0; j < numPts; j++) {
		//r = r && points[j].equals(fp.points[j]);
		r = r && points[j] == fp.points[j];
	}
	return r && /*latlon == fp.latlon && */ equalsIgnoreCase(label,fp.label);
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
	return r && (label == fp.label); // && type_p == fp.type_p; //  && numTCPs == fp.numTCPs;
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
		if (!(point(i).name() == p.point(i).name())) {
			//fpln("almostEquals: point i = " + i + " names do not match.");
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
	return static_cast<int>(points.size());
}

/** Get an approximation of the bounding rectangle around this plan */
BoundingBox Plan::getBoundBox() const {
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

const std::string& Plan::getID() const {
	return label;
}

void Plan::setID(const std::string& s) {
	label = s;
}


const std::string Plan::getName(int i) const {
	if (i < 0 || i >= size()) return "";
	else return point(i).name();
}

void Plan::setName(int i, const std::string& s) {
	if (i < 0 || i >= size()) {
		addError(" $$ setPointName: illegal index",i);
		return;
	}
	NavPoint np = point(i).makeName(s);
	points[i] = np;
}


const std::string& Plan::getNote() const {
	return note;
}

void Plan::setNote(const std::string& s) {
	note = s;
}

void Plan::appendNote(const std::string& s) {
	note += s;
}

void Plan::includeParameters(ParameterData pd) {
	size_t i1 = note.find(noteParamStart);
	size_t i2 = note.find(noteParamEnd);
	if (i2 > i1) note = note.substr(0,i1)+note.substr(i2+noteParamEnd.length()); // remove old parameters
	if (pd.size() > 0) {
		note += noteParamStart+pd.toParameterList(";")+noteParamEnd;
	}
}

/**
 * Retrieve parameters stored in the plan's note field, if any.
 * @return ParameterData containing stored parameters, possibly empty.
 */
ParameterData Plan::extractParameters() const {
	ParameterData pd = ParameterData();
	//int i1 = note.find(noteParamStart);
	//int i2 = note.find(noteParamEnd);
	//if (i1 >= 0 && i2 > noteParamStart.length()) {
	size_t i1 = note.find(noteParamStart);
	size_t i2 = note.find(noteParamEnd);
	if (i1 != string::npos && i2 != string::npos && i2 > noteParamStart.length()) {
		int start = i1+noteParamStart.length();
		//fpln(" $$$$ extractParameters: start = "+Fm0(start)+" i2 = "+Fm0(i2));
		std::string s = note.substr(start,  i2-start);
		pd.parseParameterList(";", s);
	}
	return pd;
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
	int rtn = static_cast<int>(points.size())-1;
	if (startWp < rtn) rtn = startWp+1;
	return rtn;
}



/**
 * Return the index of first point that has a name equal to the given
 * string -1 if there are no matches.
 *
 * @param startIx   start with this index
 * @param name     String to match
 * @param withWrap  if true, go through whole list
 */
int Plan::findName(const std::string& name, int startIx, bool withWrap) {
	if (startIx >= (int) points.size()) {
		if (withWrap)
			startIx = 0;
		else
			return -1;
	}
	for (int i = startIx; i < (int) points.size(); i++) {
		NavPoint np = points[i];
		if (np.name() == name)
			return i;
	}
	if (withWrap) {
		for (int i = 0; i < startIx; i++) {
			NavPoint np = points[i];
			if (np.name() == name)
				return i;
		}
	}
	return -1;
}

/**
 * Return the index of first point that has a name equal to the given
 * string, return -1 if there are no matches.
 *
 * @param name    String to match
 */
int Plan::findName(const std::string& name) const {
	if (! isLinear()) {  // debug RWB-MOT
		//f.pln(" $$ findName: name = "+name);
		//Debug.printCallingMethod();
	}
	for (int i = 0; i < (int) points.size(); i++) {
		NavPoint np = points[i];
		if (np.name() == name)
			return i;
	}
	return -1;
}

void Plan::clearName(int ix) {
	NavPoint np = point(ix);
	TcpData  tcp = getTcpDataRef(ix);
	set(ix,np.makeName(""),tcp);
}

std::string Plan::getInfo(int i) const {
	if (i < 0 || i >= size()) {
		addError("getInfo: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return "";
	}
	return data[i].getInformation();
}


int Plan::findInfo(const std::string& info, int startIx, bool withWrap) const {
	if (startIx >= (int) points.size()) {
		if (withWrap)
			startIx = 0;
		else
			return -1;
	}
	for (int i = startIx; i < (int) points.size(); i++) {
		TcpData tcp = getTcpData(i);
		if (tcp.getInformation() == info)
			return i;
	}
	if (withWrap) {
		for (int i = 0; i < startIx; i++) {
			TcpData tcp = getTcpData(i);
			if (tcp.getInformation() == info)
				return i;
		}
	}
	return -1;
}

int Plan::findInfo(const std::string& info, bool exact) const {
	for (int i = 0; i < (int) points.size(); i++) {
		TcpData tcp = getTcpData(i);
		if ((exact && tcp.getInformation() == info) || tcp.getInformation().find(info)!=std::string::npos)
			return i;
	}
	return -1;
}

int Plan::findInfo(const std::string& info) const {
	bool exact = false;
	return findInfo(info,exact);
}


void Plan::setInfo(int i, const std::string& info) {
	if (i < 0 || i >= size()) {
		addError("setInfo: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
	} else {
		TcpData& d = data[i];
		d.setInformation(info);
		data[i]=d;
	}
}

void Plan::appendInfo(int i, const std::string& info) {
	if (i < 0 || i >= size()) {
		addError("appendInfo: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
	} else {
		TcpData d = data[i];
		d.setInformation(d.getInformation()+info);
		data[i] = d;
	}
}


void Plan::clearInfo(int ix) {
	setInfo(ix,"");
}



int Plan::getIndex(double tm) const {
	int numPts = static_cast<int>(points.size());
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
	while (tdist < d && i < size()) {
		tdist += pathDistance(i);
		i++;
	}
	if (tdist == d) return i;
	if (tdist > d && i <= size()) return i-1; // on segment i-1
	if (Util::within_epsilon(d, tdist, 0.01) && i == size()) return size()-1;
	return -1; // not found
}

int Plan::getSegmentByDistance(double d) const {
	return getSegmentByDistance(0,d);
}


int Plan::getNearestIndex(double tm) const {
	int p = getIndex(tm);
	int numPts = static_cast<int>(points.size());
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
	return points[i].position();
}

double Plan::alt(int i) const {
	return points[i].alt();
}


const NavPoint Plan::point(int i) const {
	if (i < 0 || i >= (int)points.size()) {
		addError("point: invalid index "+Fm0(i), i);
		//fpln(" $$$ point: invalid index "+Fm0(i));
		//Debug::halt();
		return NavPoint::INVALID();
	}
	return points[i];
}

const std::pair<NavPoint,TcpData> Plan::get(int i) const {
	return std::pair<NavPoint,TcpData>(point(i) ,getTcpData(i));
}


TcpData& Plan::getTcpDataRef(int i) {
	if (i < 0 || i >= (int)data.size()) {
		addError("Plan.getTcpDataRef: invalid index " + Fmi(i), i);
		invalid_value = TcpData::makeInvalid();
		return invalid_value;
	}
	return data[i];
}


TcpData Plan::getTcpData(int i) const {
	if (i < 0 || i >= (int)data.size()) {
		addError("Plan.getTcpData: invalid index " + Fmi(i), i);
		invalid_value = TcpData::makeInvalid();
		return invalid_value;
	}
	return data[i];
}

double  Plan::signedRadius(int i) const {   
	return getTcpData(i).getRadiusSigned();
}

double Plan::vertexRadius(int i) const {
	return std::abs(signedRadius(i));
}
  
double Plan::calcRadius(int i) const {
	if ( ! isBOT(i)) {
		addError(" calcRadius: attempt to access chordal radius from non BOT! at i = "+Fm0(i)+"!");
	}
	Position BOT = point(i).position();
	Position center = turnCenter(i);
	return center.distanceH(BOT);
}


int  Plan::turnDir(int i) const {
	TcpData tcp_i = getTcpData(i);
	if (! tcp_i.isTrkTCP()) {
        addError("turnDir: Attempt to get turn direction from a non BOT pint!");
	}
	return tcp_i.turnDir();

}

bool    Plan::isOriginal(int i)    const { return getTcpData(i).isOriginal();}
bool    Plan::isAltPreserve(int i) const { return getTcpData(i).isAltPreserve();}
bool    Plan::isVirtual(int i)     const { return getTcpData(i).isVirtual();}

double  Plan::gsAccel(int i)      const { return getTcpData(i).getGsAccel();}
double  Plan::vsAccel(int i)       const { return getTcpData(i).getVsAccel();}


bool Plan::isTrkTCP(int i)   const {return getTcpData(i).isTrkTCP(); }
bool Plan::isBOT(int i)      const {return getTcpData(i).isBOT(); }
bool Plan::isMOT(int i) const {
	return getTcpData(i).isMOT();
}



bool Plan::isEOT(int i)      const {return getTcpData(i).isEOT(); }
bool Plan::isGsTCP(int i)    const {return getTcpData(i).isGsTCP(); }
bool Plan::isBGS(int i)      const {return getTcpData(i).isBGS(); }
bool Plan::isEGS(int i)      const {return getTcpData(i).isEGS(); }
bool Plan::isVsTCP(int i)    const {return getTcpData(i).isVsTCP(); }
bool Plan::isBVS(int i)      const {return getTcpData(i).isBVS(); }
bool Plan::isEVS(int i)      const {return getTcpData(i).isEVS(); }
bool Plan::isBeginTCP(int i) const {return getTcpData(i).isBeginTCP(); }
bool Plan::isEndTCP(int i)   const {return getTcpData(i).isEndTCP(); }


bool Plan::isTCP(int i) const { return getTcpData(i).isTCP();}

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

void Plan::setVertexRadius(int i, double radius) {
	if (i < 0 || i >= size()) {
		addError("setRadius: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setRadiusSigned(radius);
	data[i]=d;
}

//double Plan::getGsIn_0() const {
//	return gsIn_0;
//}
//
//void Plan::setGsIn_0(double gsIn_0_d) {
//	gsIn_0 = gsIn_0_d;
//}


Position Plan::turnCenter(int i) const {
	if (i < 0 || i >= size()) {
		addError("turnCenter: invalid point index of " + Fmi(i) + " size=" + Fmi(size()));
		return Position::INVALID();
	}
	return getTcpData(i).turnCenter();
}

double Plan::getGsAccel(int i) {
	return data[i].getGsAccel();
}

double Plan::getVsAccel(int i) {
	return data[i].getVsAccel();
}


void Plan::setGsAccel(int i, double accel) {
	if (i < 0 || i >= size()) {
		addError("setGsAccel: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setGsAccel(accel);
}


void Plan::setVsAccel(int i, double accel) {
	if (i < 0 || i >= size()) {
		addError("setVsAccel: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setVsAccel(accel);
}

void Plan::setBOT(int i, double signedRadius, Position center) {
	if (i < 0 || i >= size()) {
		addError("setBOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBOT(signedRadius, center);
}

void Plan::setEOT(int i) {
	if (i < 0 || i >= size()) {
		addError("setEOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEOT();
}

void Plan::addBOT(int i, double signedRadius, Position center) {
	TcpData& d = getTcpDataRef(i);
	d.addBOT(signedRadius, center);
}

void Plan::addEOT(int i) {
	TcpData& d = getTcpDataRef(i);
	d.addEOT();
}

void Plan::setEOTBOT(int i, double signedRadius, Position center) {
	if (i < 0 || i >= size()) {
		addError("setEOTBOT: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEOTBOT(signedRadius, center);
}

void Plan::setMOT(int i) {
	getTcpDataRef(i).setMOT(true);
}
void Plan::clearMOT(int i) {
	getTcpDataRef(i).setMOT(false);
}

void Plan::clearBOT(int ix) {
	getTcpDataRef(ix).clearBOT();
}

void Plan::clearEOT(int ix) {
	getTcpDataRef(ix).clearEOT();
}


void Plan::setBGS(int i, double acc) {
	if (i < 0 || i >= size()) {
		addError("setBGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBGS(acc);
}

void Plan::setEGS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEGS();
}

void Plan::addEGS(int i) {
	TcpData& d = getTcpDataRef(i);
	d.addEGS();
}


void Plan::clearEGS(int ix) {
	getTcpDataRef(ix).clearEGS();
}


void Plan::setEGSBGS(int i, double acc) {
	if (i < 0 || i >= size()) {
		addError("setEGSBGS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEGSBGS(acc);

}

void Plan::clearBGS(int ix) {
	getTcpDataRef(ix).clearBGS();
}

void Plan::setBVS(int i, double acc) {
	if (i < 0 || i >= size()) {
		addError("setBVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setBVS(acc);
}

void Plan::addEVS(int i) {
	TcpData& d = getTcpDataRef(i);
	d.addEVS();
}


void Plan::setEVS(int i) {
	if (i < 0 || i >= size()) {
		addError("setEVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEVS();
}

void Plan::setEVSBVS(int i, double acc) {
	if (i < 0 || i >= size()) {
		addError("setEVSBVS: invalid point index of "+Fmi(i)+" size="+Fmi(size()));
	}
	TcpData& d = data[i];
	d.setEVSBVS(acc);
}

void Plan::clearBVS(int ix) {
	getTcpDataRef(ix).clearBVS();
}


void Plan::clearEVS(int ix) {
	getTcpDataRef(ix).clearEVS();
}




int Plan::addNavPoint(const NavPoint& p) {
	TcpData d = TcpData();
	//d.setSource(p);
	return add(p, d);
}

int Plan::add(const Position& p, double time) {
	NavPoint np = NavPoint(p,time);
	return addNavPoint(np);
}


int Plan::add(const std::pair<NavPoint,TcpData>& p) {
	return add(p.first, p.second);
}

int Plan::add(const NavPoint& p2, const TcpData& d) {
	NavPoint p = p2;
	if (p.isInvalid()) {
		addError("add: Attempt to add invalid NavPoint",0);
		return -1;
	}
	int numPts = static_cast<int>(points.size());

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

	int i = getIndex(p.time());
	if (i >= 0) {
		//replace
		if ( ! isVirtual(i)) {
			if (getTcpData(i).mergeable(d)) {
				NavPoint np2 = point(i).appendName(p.name());
				TcpData np = getTcpData(i).mergeTCPData(d);
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
	int numPts = static_cast<int>(points.size());
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

/**
 * Insert a navpoint at the indicated distance from plan start.
 * @param d distance (can be negative)
 * @return index of new point, or -1 if there is an error.
 */
int Plan::insertByDistance(double d) {
	double t = timeFromDistance(d);
	if (t < 0) return -1;
	Position p = position(t);
	return add(p,t);
}

/**
 * Insert a navpoint at the indicated distance from the given index point.  Negative distances are before the given point.
 * @param d distance
 * @return index of new point, or -1 if there is an error.
 */
int Plan::insertByDistance(int i, double d) {
	if (i < 0 || i >= size()) return -1;
	double d1 = pathDistance(0,i);
	double d2 = d1+d;
	return insertByDistance(d2);
}



void Plan::remove(int i) {
	int numPts = static_cast<int>(points.size());
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

/**
 * Attempt to replace the i-th point's tcp data  with the given data. The navpoint from the
 * old point is retained.  If successful,
 * this returns the index of the new point. This method
 * returns -1 and sets an error if the given index is out of bounds.
 *
 * @param i  the index of the point to be replaced
 * @param v  the new TcpData to replace it with
 * @return the actual index of the new point
 */
int Plan::setTcpData(int i, TcpData v) {
	if (i < 0 || i >= (int) points.size()) {
		addError("setTcpData: invalid index " + Fmi(i), i);
		return -1;
	}
	NavPoint np = point(i);
	remove(i);
	return add(np, v);
}


void Plan::setTime(int i, double t) {
	if (t < 0) {
		addError("setTime: invalid time "+Fm4(t),i);
		return;
	}
	if (i < 0 || i >= size()) {
		addError("setTime: invalid index "+Fm0(i));
		return;
	}
	NavPoint tempv = points[i].makeTime(t);
	TcpData  tcp = getTcpData(i);
	set(i, tempv, tcp);


}

void Plan:: setTimeInPlace(int i, double t) {
	points[i] = points[i].makeTime(t);
}


void Plan::setAlt(int i, double alt) {
	if (i < 0 || i >= (int)points.size()) {
		addError("setTime: Invalid index "+Fm0(i),i);
		return;
	}
	NavPoint tempv = points[i].mkAlt(alt);
	setNavPoint(i, tempv);
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
		for (int i = size()-1; i >= start; i--) {
			setTime(i, time(i)+dt);
		}
	}
	return true;
}


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
	if (current < 0 || current > size()) {
		addWarning("prevEOT invalid starting index "+Fm0(current));
		return -1;
	}
	for (int j = current-1; j >= 0; j--) {
		if (isEOT(j)) {
			return j;
		}
	}
	return -1;

}


int Plan::nextEOT(int current) const {
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
	if (current < -1 || current > size()-1) {
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

int Plan::nextBGS(int current) const {
	if (current < -1 || current > size()-1) {
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

int Plan::prevTRK(int current) const {
	for (int j = current - 1; j >= 0; j--) {
		if (isBOT(j)|| isEOT(j)) return j;
	}
	return -1;
}

int Plan::prevGS(int current) const {
	for (int j = current - 1; j >= 0; j--) {
		if (isBGS(j)|| isEGS(j)) return j;
	}
	return -1;
}

int Plan::prevVS(int current) const {
	for (int j = current - 1; j >= 0; j--) {
		if (isBVS(j)|| isEVS(j)) return j;
	}
	return -1;
}

int Plan::prevBVS(int current) const {
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
	if (current < -1 || current > size()-1) {
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
		if (isTrkTCP(j) || isGsTCP(j) || isVsTCP(j) ) {

			return j;
		}
	}
	return -1;
}


int Plan::nextTCP(int current) const {
	if (current < 0 || current > size()) {
		addWarning("nextTCP invalid starting index " + Fm0(current));
		return -1;
	}
	for (int j = current + 1; j < size(); j++) {
		// f.pln(" $$$ prevTCP: points.get("+j+") = "+points.get(j).toStringFull());
		if (isTrkTCP(j) || isGsTCP(j) || isVsTCP(j)) {
			return j;
		}
	}
	return -1;
}

bool Plan::inTrkChange(double t) const { //fixed
	int i = getSegment(t);
	return inTrkAccel(i);
}


bool Plan::inGsChange(double t) const { //fixed
	int i = getSegment(t);
	return inGsAccel(i);
}


bool Plan::inVsChange(double t) const { //fixed
	int i = getSegment(t);
	return inVsAccel(i);
}


//bool Plan::inTrkChangeOLD(double t) const { //fixed
//	int i = getSegment(t);
//	int j1 = prevBOT(i+1);
//	int j2 = prevEOT(i+1);
//	return j1 >= 0 && j1 >= j2;
//}
//
//
//bool Plan::inGsChangeOLD(double t) const { //fixed
//	int i = getSegment(t);
//	int j1 = prevBGS(i+1);
//	int j2 = prevEGS(i+1);
//	return j1 >= 0 && j1 >= j2;
//}
//
//
//bool Plan::inVsChangeOLD(double t) const { //fixed
//	int i = getSegment(t);
//	int j1 = prevBVS(i+1);
//	int j2 = prevEVS(i+1);
//	return j1 >= 0 && j1 >= j2;
//}

bool Plan::inTrkAccel(int ix) const {
	int j1 = prevTRK(ix+1);
	if (j1 < 0) return false;
	if (isBOT(j1)) return true;
	return false;
}
bool Plan::inGsAccel(int ix) const {
	int j1 = prevGS(ix+1);
	if (j1 < 0) return false;
	if (isBGS(j1)) return true;
	return false;
}
bool Plan::inVsAccel(int ix) const {
	int j1 = prevVS(ix+1);
	if (j1 < 0) return false;
	if (isBVS(j1)) return true;
	return false;
}




//bool Plan::inTrkChangeByDistance(double d) const { //fixed
//	int i = getSegmentByDistance(d);
//	int j1 = prevBOT(i+1);
//	int j2 = prevEOT(i+1);
//	return j1 >= 0 && j1 >= j2;
//}
//
//
//bool Plan::inGsChangeByDistance(double d) const { //fixed
//	int i = getSegmentByDistance(d);
//	int j1 = prevBGS(i+1);
//	int j2 = prevEGS(i+1);
//	return j1 >= 0 && j1 >= j2;
//}
//
//
//bool Plan::inVsChangeByDistance(double d) const { //fixed
//	int i = getSegmentByDistance(d);
//	int j1 = prevBVS(i+1);
//	int j2 = prevEVS(i+1);
//	return j1 >= 0 && j1 >= j2;
//}

double Plan::turnRadiusAtTime(double t) const {
	if (inTrkChange(t)) {
		int ixBOT = prevBOT(getSegment(t)+1);
		NavPoint bot = points[ixBOT];//fixed
		return std::abs(getTcpData(ixBOT).getRadiusSigned());
	} else {
		return -1.0;
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



Position Plan::position(double t, bool linear) const {
	return positionVelocity(t, linear).first;
}

Position Plan::position(double t) const {
	return positionVelocity(t, false).first;
}

NavPoint Plan::navPt(double t) const {
	return NavPoint(position(t),t);
}


/**
 * Estimate the initial velocity at the given time for this aircraft.
 * A time before the beginning of the plan returns a zero velocity.
 */
Velocity Plan::velocity(double tm, bool linear) const {
	return positionVelocity(tm, linear).second;
}

/**
 * Estimate the initial velocity at the given time for this aircraft.
 * A time before the beginning of the plan returns a zero velocity.
 */
Velocity Plan::velocity(double tm) const {
	return positionVelocity(tm, false).second;
}


/**
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
	if (ISNAN(t1)) return t1;
	double t2 = Util::root(0.5*gsAccel, vo, -dist, -1);
	double dt = t1 < 0 ? t2 :  (t2 < 0 ? t1 : Util::min(t1, t2));
	return dt;
}


double Plan::timeFromDistanceWithinSeg(int seg, double rdist) const {
	if (seg < 0 || seg >= size()-1 || rdist < 0 || rdist > pathDistance(seg)) {
        addError("timeFromDistanceWithinSeg: seg="+Fm0(seg)+" rdist="+Fm2(rdist)+" not in segment");
		return -1;
	}
	double gs0 =gsOut(seg);
	double gs1 = gsFinal(seg);
	if (Util::almost_equals(gs0, 0.0) && Util::almost_equals(gs1, 0.0)) return 0;
	if (inGsChange(time(seg))) {
		double a = gsAccel(prevBGS(seg+1));//fixed
		return timeFromDistance(gs0, a, rdist);
	} else {
		return rdist/gs0;
	}
}

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
	double tmWithin = timeFromDistanceWithinSeg(seg, distWithinLastSeg);
	return tmWithin + time(seg);
}


std::pair<double,double> Plan::timeFromHeight(int seg, double height) const {
	if (seg < 0 || seg >= size()-1) {
		return std::pair<double,double>(-1.0, -1.0); // bad parameters
	}
	double alt0 = getPos(seg).alt();
	double alt1 = getPos(seg+1).alt();
	double t0 = time(seg);
	double dt = time(seg+1) - t0;
	double vs = vsOut(seg);
	double a = vsAccel(seg);
	if (Util::almost_equals(a,  0.0)) {
		if ((height < alt0 && height < alt1) || (height > alt0 && height > alt1)) {
			return std::pair<double,double>(-1.0, -1.0); // out of range
		}
		if (Util::almost_equals(height,  alt0)) {
			return std::pair<double,double>(t0, -1.0); // at start
		}
		if (Util::almost_equals(vs,  0.0) && !Util::almost_equals(height, alt0)) {
			return std::pair<double,double>(-1.0, -1.0); // out of range
		}
		double t = (height-alt0)/vs;
		if (t < 0.0 || t > dt) {
			return std::pair<double,double>(-1.0, -1.0); // out of range
		}
		return std::pair<double,double>(t+t0, -1.0);
	} else { // in accel zone
		double t1 = Util::root(-.5*a, vs, (alt0-height), 1);
		double t2 = Util::root(-.5*a, vs, (alt0-height), -1);
		// various error conditions:
		if (t1 < 0.0 || t1 > dt) {
			t1 = -1.0;
		} else {
			t1 = t1+t0;
		}
		if (t2 < 0.0 || t2 > dt) {
			t2 = -1.0;
		} else {
			t2 = t2+t0;
		}
		if (t1 < 0.0 || (t1 >= 0.0 && t2 >= 0.0 && t2 < t1)) { // swap if the first is bad or if both good and t2 > t1
			double tmp = t1;
			t1 = t2;
			t2 = tmp;
		}
		return std::pair<double,double>(t1,  t2);
	}

}



Velocity Plan::velocityFromDistance(double d) const {
	return velocity(timeFromDistance(d));
}


double Plan::trkOut(int seg, bool linear) const {
	//fpln(" $$$ trkOut: ENTER seg = "+seg+" linear = "+linear);
	if (seg < 0 || seg > size()-1) {
		addError("trkOut: invalid index "+Fm0(seg), 0);
		return NaN;
	}
	if (seg == size()-1) {
		return trkFinal(seg-1,linear);
	}
	if (inTrkChange(point(seg).time()) && !linear) {
		int ixBOT = prevBOT(seg + 1);
		return trkInTurn(seg, ixBOT);
	} else {
		if (isLatLon()) {
			return GreatCircle::initial_course(point(seg).lla(), point(seg+1).lla());
		} else {
			return point(seg).initialVelocity(point(seg+1)).trk();
		}
	}
}

double Plan::trkInTurn(int ix, int ixBOT) const {
	double finalTrk;
	Position pos = point(ix).position();
	int dir = turnDir(ixBOT);
	Position center = turnCenter(ixBOT);
	if (pos.isLatLon()) {
		double final_course = GreatCircle::final_course(center.lla(),pos.lla());
		finalTrk = final_course + dir*M_PI/2;
	} else {
		double trkFromCenter = Velocity::track(center.vect3(), pos.vect3());
		double nTrk = trkFromCenter;
		finalTrk = nTrk + dir*M_PI/2;
	}
	return finalTrk;
}

double Plan::trkOut(int seg) const { return trkOut(seg,false); }

double Plan::trkIn(int seg) const { return trkFinal(seg-1, false); }

double Plan::trkDelta(int i) const {
	return Util::turnDelta(trkIn(i),trkOut(i));
}

double Plan::defTrkOut(int seg) const {
	if (seg > size() - 1) return NaN;
	int ix = seg;
	for (ix = seg; ix < size(); ix++) {
		double gsOut_ix = gsOut(ix);
		if (! Util::almost_equals(gsOut_ix,0.0,PRECISION5)) break;
	}
	if (ix >= size()) ix = size()-1;
	return trkOut(ix);

}

double Plan::defTrkIn(int seg) const {
	if (seg > size() - 1) return NaN;
	int ix = seg;
	for (ix = seg; ix > 0; ix--) {
		double gsIn_ix = gsIn(ix);
		if (! Util::almost_equals(gsIn_ix,0.0,PRECISION5)) break;
	}
	if (ix == 0) return trkOut(ix);
	return trkIn(ix);
}


double Plan::trkFinal(int seg, bool linear) const {
	double rtn = NaN;
	//fpln("$$$ trkFinal: ENTER: seg = "+seg+" linear = "+linear);
	if (seg < 0 || seg >= size()-1) {
		addError("trkFinal: invalid index "+Fm0(seg), 0);
		return rtn;
	}
	NavPoint np1 = point(seg);
	if (inTrkChange(point(seg).time()) && ! linear) {
		int ixBOT = prevBOT(seg+1);
		return trkInTurn(seg+1, ixBOT);
	} else {
		if (isLatLon()) {
			double trk = GreatCircle::final_course(point(seg).lla(), point(seg+1).lla());
			rtn = trk;
		} else {
			Velocity vo = NavPoint::finalVelocity(point(seg), point(seg+1));
			rtn = vo.trk();
		}
	}
	return Util::to_2pi(rtn);

}


double Plan::gsOutCalc(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("gsOut: invalid index "+Fm0(i), 0);
		return NaN;
	}
	if (i == size()-1) return gsFinal(i-1);
	int j = i+1;
	//	while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip next point(s) if very close
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
	return rtn;
}

double Plan::gsOutCalc(int i) const {
	return gsOutCalc(i,false);
}


double Plan::gsOut(int i, bool linear) const {
	double rtn = gsOutCalc(i,linear);
	if (rtn < 0) {  // to guard against -1.2E-12 calculations, see GsConsistent
		rtn = 0.0;
	}
	return rtn;
}

double Plan::gsFinalCalc(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("gsFinal: invalid index "+Fm0(i), 0);
		return NaN;
	}
	int j = i+1;
	//	while (i > 0 && getTime(j) - getTime(i) < minDt) { // skip next point(s) if very close
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

double Plan::gsFinal(int i, bool linear) const {
	double rtn = gsFinalCalc(i,linear);
	if (rtn < 0) {
		rtn = 0.0;
	}
    return rtn;
}

double Plan::gsIn(int seg, bool linear)  const {
	if (seg<0) {
		//if (Debug::FAIL_FAST) return NaN;
		//else
			return gsOut(0);   // This is usually what you want
	}
	return gsFinal(seg-1,linear);
}

double Plan::gsInCalc(int i)  const {
	if (i < 0) {
		//if (Debug::FAIL_FAST) return NaN;
		//else
			return gsOut(0);   // This is usually what you want
	}
	bool linear = false;
	return gsFinalCalc(i-1,linear);
}


double Plan::gsOut(int seg)  const { return gsOut(seg,false); }
double Plan::gsFinal(int seg) const { return gsFinal(seg,false); }
double Plan::gsIn(int seg)  const { return gsIn(seg,false); }
















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
		gs = NaN;
	} else {
		double gsSeg = gsOut(seg, linear);
		//fpln(" $$ gsAtTime: seg = "+seg+" gsAt = "+Units::str("kn",gsAt,8));
		gs = gsAtTime(seg, gsSeg, t, linear);
	}
	return gs;
}

double Plan::gsAtTime(double t) const {
	bool linear = false;
	return gsAtTime(t, linear);
}

double Plan::vsOut(int i, bool linear) const {
	if (i < 0 || i > size()-1) {
		addError("vsOut: invalid index "+Fm0(i), 0);
		return NaN;
	}
	if (i == size()-1) return vsFinal(i-1);
	int j = i+1;
	//	while (j < size()-1 && getTime(j) - getTime(i) < minDt) { // skip next point(s) if very close
	//		j++;
	//	}
	double dist = alt(j) - alt(i);
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
		return NaN;
	}
	int j = i+1;
	//	while (i > 0 && getTime(j) - getTime(i) < minDt) { // collapse next point(s) if very close
	//		i--;
	//	}
	double dist = alt(j) - alt(i);
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
		vs = NaN;
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
	if (dt < minDt) return 0.0; // special case, ignore very short times, otherwise can mess up calcs below -- GEH
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
std::pair<Position, Velocity> Plan::posVelWithinSeg2D(int seg, double t, bool linear, double gsAt_d) const  {
	Position sNew;
	Velocity vNew;
	NavPoint np1 = point(seg);
	Position so = np1.position();
	std::pair<Position, Velocity> pv;
	if ( ! linear && inTrkChange(t)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		//fpln(" $$$ advanceWithinSeg: center = "+center);
		int dir = turnDir(ixPrevBOT);
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


std::pair<Position, Velocity> Plan::advanceDistanceWithinSeg2D(int seg, double distFromSeg, bool linear, double gsAt_d) const {
	NavPoint np1 = point(seg);
	Position so = np1.position();
	std::pair<Position,Velocity> pv;
	double tSeg = point(seg).time();
	if ( ! linear && inTrkChange(tSeg)) {
		int ixPrevBOT = prevBOT(seg + 1);
		Position center = turnCenter(ixPrevBOT);
		//f.pln(" $$$ positionVelocity: center = "+center);
		int dir = turnDir(ixPrevBOT);
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSeg, gsAt_d);
		// f.pln(" $$ %%%% positionVelocity A: vNew("+f.Fm2(t)+") = "+vNew);
		// f.pln(" $$ %%%% positionVelocity A: sNew("+f.Fm2(t)+") = "+sNew);
	} else {
		double track = trkOut(seg,linear);
		pv = so.linearDist2D(track, distFromSeg, gsAt_d);
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
		int dir = turnDir(ixPrevBOT);
		pv = KinematicsPosition::turnByDist2D(so, center, dir, distFromSeg);
	} else {
		double track = trkOut(seg,linear);
		pv = so.linearDist2D(track, distFromSeg);
	}
	return pv;
}

std::pair<Position,int> Plan::advanceDistance2D(int seg, double distFromSeg, bool linear) const{
	double remainingDist = distFromSeg;
	for (int i = seg; i < size(); i++) {
		double pathDist_i = pathDistance(i,linear);
		if (remainingDist < pathDist_i) {
			Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
			return std::pair<Position,int>(newPos,i);
		} else {
			remainingDist = remainingDist - pathDist_i;
		}
	}
	addError("advanceDistance:  distance exceeded length of plan!");
	int ixLast = size() - 1;
	return  std::pair<Position, int>(points[ixLast].position(), ixLast);
}

std::pair<Position,int> Plan::advanceDistance(int ix, double distFromSeg, bool linear) const {
	if (distFromSeg == 0.0) { // SAVE TIME
		return std::pair<Position,int>(point(ix).position(),ix);
	}
	double remainingDist = distFromSeg;
	for (int i = ix; i < size(); i++) {
		double pathDist_i = pathDistance(i);
		if (remainingDist < pathDist_i) {
			Position newPos = advanceDistanceWithinSeg2D(i, remainingDist, linear);
			double t = timeFromDistance(i, remainingDist);
			//double alt = position(t).alt();
			double alt = interpolateAltVs(i, t-time(i), linear).first;
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
		int dir = turnDir(ixPrevBOT);
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
	if (t < getFirstTime() || ISNAN(t) || t > getLastTime()) {
		return std::pair<Position,Velocity>(Position::INVALID(), Velocity::INVALIDV());
	}
	if (size() == 1) {
		return std::pair<Position,Velocity>(points[0].position(), Velocity::INVALIDV());
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
	double gs0 = gsOut(seg,linear);
	double gsAt_d = gsAtTime(seg, gs0, t, linear);
	Position so = np1.position();
	std::pair<Position, Velocity> adv = posVelWithinSeg2D(seg, t, linear, gsAt_d);
	Position sNew = adv.first;
	Velocity vNew = adv.second;
	std::pair<double,double> altPair = interpolateAltVs(seg, t-time(seg), linear);
	sNew = sNew.mkAlt(altPair.first);
	vNew = vNew.mkVs(altPair.second);
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

std::pair<double,double> Plan::altitudeVs(double t, bool linear) const {
	int seg = getSegment(t);
	double dt = t-time(seg);
	return interpolateAltVs(seg, dt, linear);
}




Velocity Plan::averageVelocity(int i, bool linear) const {
	if (i >= size()-1) {   // there is no velocity after the last point
		addWarning("averageVelocity(int): Attempt to get an averge velocity after the end of the Plan: "+Fm0(i));
		return Velocity::INVALIDV();
	}
	if (i < 0) {
		addWarning("averageVelocity(int): Attempt to get an average velocity before beginning of the Plan: "+Fm0(i));
		return Velocity::INVALIDV();
	}
	Velocity v = NavPoint::averageVelocity(points[i],points[i+1]);

	if (!linear) {
		double dt = time(i+1) - time(i);
		double gs = pathDistance(i)/dt;
		v = v.mkGs(gs);
	}
	return v;
}

Velocity Plan::averageVelocity(int i) const {
	return averageVelocity(i, true);
}


Velocity Plan::initialVelocity(int i) const {
	return initialVelocity(i, false);
}

Velocity Plan::initialVelocity(int i, bool linear) const {
	//if (size() < 2) return Velocity::INVALIDV();
	return Velocity::mkTrkGsVs(trkOut(i,linear), gsOut(i,linear), vsOut(i,linear));
}


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

/**
 * calculate delta time for point i to make ground speed into it = gs
 *
 * @param i
 * @param gs
 * @return delta time
 */
double Plan::calcDtGsIn(int ix, double gs) const {
	double d = pathDistance(ix-1);
	double dt;
	if (Util::almost_equals(gs,0.0,PRECISION13)) {
		if (Util::almost_equals(d,0.0,PRECISION13)) { // hover
			dt = 0.0;
			addWarning("calcDtGsIn: setting ground speed to zero on hover segment does not change segment time");
		} else {
			dt = NaN;
			addError("calcDtGsIn: setting ground speed to zero is impossible on non-zero lengthed segment");
		}
	} else if (inGsChange(points[ix-1].time())) {
		int ixBGS = prevBGS(ix);
		double a_gs = gsAccel(ixBGS);
		dt = Util::root(0.5*a_gs, -gs, d, -1);
	} else {
		dt = d/gs;
	}
    return dt;
}

/**
 * calculate time at a point such that the ground speed into that
 * point is "gs". If i or gs is invalid, this returns -1. If i is in a
 * turn, this returns the current point time.
 *
 * Note: parameter maxGsAccel is not used on a linear segment
 */
double Plan::calcTimeGsIn(int i, double gs) const {
	return points[i - 1].time() + calcDtGsIn(i, gs);
}

void Plan::setTimeGsIn(int i, double gs) {
	double newT = calcTimeGsIn(i,gs);
	setTime(i,newT);
}


/**
 * Change the ground speed into ix to be gs -- all other ground speeds remain the same in the linear case
 * Note:  If ix is in an acceleration zone, this will also affect gsOut(ix-1)
 *
 * @param ix  index
 * @param gs  new ground speed
 */
bool Plan::mkGsIn(int ix, double gs) {
	if (ix < 0 || ix > size() - 1) return false;
	double dt = calcDtGsIn(ix,gs);
	if (ISNAN(dt)) {
		addWarning(" mkGsIn: could not make ground speed = "+Units::str("kn",gs)+" at ix = "+Fm0(ix));
		return false;
	}
	if (Util::almost_equals(dt,0.0,PRECISION13)) return true;  // prevents removal of hover segment
	double shift_t =  dt - (time(ix) - time(ix-1));  // change
	timeShiftPlan(ix, shift_t);
	return true;
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
bool Plan::mkGsOut(int ix, double gs) {
	if (ix < 0 || ix >= size() - 1) return false;
//	double a = 0.0;
	double d = pathDistance(ix);
	double dt;
	double a = 0;
	bool inGsAccel_b = inGsAccel(ix);
	if (inGsAccel_b) {
		int ixBGS = prevBGS(ix+1);
		if (ixBGS >= 0) a = gsAccel(ixBGS);
	}
	if (Util::almost_equals(gs,0.0,PRECISION13) && a <= 0.0) {
		if (Util::almost_equals(d,0.0,PRECISION13)) { // hover
			dt = 0.0;
			addWarning("mkGsOut: setting ground speed to zero on hover segment does not change segment time");
		} else {
			dt = NaN;
			addError("mkGsOut: setting ground speed to zero is impossible on non-zero lengthed segment");
		}
	} else if (inGsAccel_b) {
		//int ixBGS = prevBGS(ix+1);
		//double a = gsAccel(ixBGS);
		dt = Util::root(0.5*a,gs,-d,1);
	} else {
		dt = d/gs;
	}
	if (ISNAN(dt)) {
		addWarning(" mkGsOut: could not make ground speed = "+Units::str("kn",gs)+" at ix = "+Fm0(ix));
		return false;
	}
	if (Util::almost_equals(dt,0.0,PRECISION13)) return true;  // prevents removal of hover segment
    double shift_t = time(ix) + dt - time(ix+1);
	timeShiftPlan(ix + 1, shift_t);
	return true;
}

void Plan::mkGsConstant(int wp1, int wp2, double gs) {
	//f.pln("$$ mkGsConstant:  wp1 = "+wp1+" wp2 = "+wp2+ " gs = "+gs);
	if (gs <= 0.0) {
		addError("PlanUtil.mkGsConstant: cannot accept gs <= 0");
		return;
	}
	if (wp1 < 0) wp1 = 0;
	if (wp2 >= size()) wp2 = size()-1;
	if (wp1 >= wp2) return;
	fixBGS_EGS(wp1,wp2);
	for (int j = wp2-1; j >= wp1; j--) {
		mkGsOut(j,gs);
	}
}

void Plan::mkGsConstant(double gs) {
	mkGsConstant(0, size()-1, gs);
}

void Plan::fixBGS_EGS(int wp1, int wp2) {
	bool inGsChange1 = inGsChange(points[wp1].time());
	bool inGsChange2 = inGsChange(points[wp2].time());
	//f.pln(" $$ mkGsConstant: inGsChange1 = "+inGsChange1+" inGsChange2 = "+inGsChange2);
	int ixBGS1 = -1;
	int ixEGS1 = -1;
	int ixBGS2 = -1;
	if (inGsChange2) ixBGS2 = prevBGS(wp2+1);
	if (inGsChange1) {
		ixBGS1 = prevBGS(wp1+1);
		ixEGS1 = nextEGS(wp1);
	}
	for (int jj = wp1; jj <= wp2; jj++) {
		if (jj < wp2) clearBGS(jj);
		if (jj > wp1) clearEGS(jj);
	}
	//f.pln(" $$$ mkGsConstant: ixEGS1 = "+ixEGS1);
	if (ixEGS1 >= 0 && ixBGS1 < wp1) {
		setEGS(wp1);
	}
	if (ixBGS2 >= 0 && ixBGS2 < wp2) {
		setBGS(wp2,gsAccel(ixBGS2));
	}
}

void Plan::mkAlt(int ix, double newAlt) {
	NavPoint np = point(ix).mkAlt(newAlt);
	TcpData  tcp = getTcpData(ix);
	remove(ix);
	add(np,tcp);
}



void Plan::setAltVSin(int i, double vs, bool preserve) {
	if (i <= 0)
		return;
	double dt = point(i).time() - point(i - 1).time();
	double newAlt = point(i - 1).alt() + dt * vs;
	NavPoint tempv = points[i].mkAlt(newAlt);
	TcpData tcp = data[i];
	if (preserve)
		tcp = tcp.setAltPreserve();
	set(i, tempv, tcp);
}

void Plan::removeAltPreserves() {
	for (int i = 0; i < size(); i++) {
		TcpData tcp = getTcpData(i);
		if (tcp.isAltPreserve()) {
			tcp = tcp.setOriginal();
		    setTcpData(i,tcp);
		}
	}
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
	double rtn = 0.0;
	if (i < 0 || i+1 >= size()) {
		return 0;
	}
	Position p1 = points[i].position();
	Position p2 = points[i+1].position();
	if (!linear && inTrkChange(time(i))) {
		// if in a turn, figure the arc distance
		int ixBOT = prevBOT(i+1);
		Position center = turnCenter(ixBOT);
		double R; //  = chordalRadius(ixBOT);
		if (KinematicsLatLon::chordalSemantics) {
			R = signedRadius(ixBOT);
		} else {
			R = calcRadius(ixBOT);
		}
		double theta = PositionUtil::angle_between(p1,center,p2);
		rtn = std::abs(theta*R);
	} else {
		// otherwise just use linear distance
		rtn = p1.distanceH(p2);
	}
	return rtn;
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
	// if in a turn, figure the arc distance
	if (inTrkChange(t) && !linear) {
		int ixBOT = prevBOT(getSegment(t)+1);
		//NavPoint bot = points[ixBOT];//fixed
		double R = signedRadius(ixBOT);
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
double Plan::pathDistanceToPoint(double t, int j) const {
	int i = getSegment(t);
	double dist_i_j = pathDistance(i+1,j);
	double dist_part = partialPathDistance(t,false);
	if (j >= i) {
		return dist_i_j + dist_part;
	} else {
		return dist_i_j - dist_part;
	}
}

double Plan::pathDistanceFromTime(double t) const {
	if (t <= getFirstTime()) return 0;
	if (t >= getLastTime()) return pathDistance();
	int seg = getSegment(t);
	return pathDistance(0,seg)+distFromPointToTime(seg, t, false);
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
		total = total + vertDistance(jj);
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
	double radius = std::abs(signedRadius(ixBOT));
	Position center = turnCenter(ixBOT);
	Position botPos = getPos(ixBOT);
	Position eotPos = getPos(ixEOT);
	double trkIn_d = trkOut(ixBOT);
	double trkOut_d = trkOut(ixEOT);
	int dir = Util::turnDir(trkIn_d,trkOut_d);
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
	double gsIn_d;
	if (ixBOT == 0) gsIn_d = gsOut(ixBOT);
	else gsIn_d = gsIn(ixBOT);
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
	//f.pln(" $$ vertexFromTurnTcps: theta = "+Units::str("deg",theta)+" distance = "+Units::str("NM",distance));
	if (botPos.isLatLon()) {
		double centerToBOT_trk = GreatCircle::initial_course(center.lla(),botPos.lla());
		double cLineTrk = centerToBOT_trk + dir*theta/2;
		//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units::str("deg",centerToBOT_trk));
		vertex = Position(GreatCircle::linear_initial(center.lla(), cLineTrk, distance));
	} else {
		double centerToBOT_trk = Velocity::track(center.vect3(), botPos.vect3());
		double cLineTrk = centerToBOT_trk + dir*theta/2;
		Vect3 sn = center.vect3().linearByDist2D(cLineTrk, distance);
		//f.pln(" $$ vertexFromTurnTcps: centerToBOT_trk = "+Units::str("deg",centerToBOT_trk)+" cLineTrk = "+Units::str("deg",cLineTrk));
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

int Plan::findMOT(int ixBOT) const {
    int ixEOT = nextEOT(ixBOT);
    return findMOT(ixBOT,ixEOT);
}

int Plan::findMOT(int ixBOT, int ixEOT) const {
	if (ixEOT <= ixBOT+1) return -1;  // No MOT possible
	if (ixEOT == ixBOT +2) {
		//fpln(" $$ findMOT ("+ixBOT+","+ixEOT+"): AA return (ixBOT+1) = "+(ixBOT+1));
		return ixBOT+1;
	}
	for (int ix = ixBOT+1; ix < ixEOT; ix++) {
        if (isMOT(ix)) return ix;
		}
	// Leave the following in case MOTflag is lacking
	double pathDistBOT2EOT = pathDistance(ixBOT,ixEOT);
	double target = 0.49*pathDistBOT2EOT;
	for (int ix = ixBOT+1; ix < ixEOT; ix++) {
		if (pathDistance(ixBOT,ix) >= target) {
			//fpln(" $$ findMOT("+ixBOT+","+ixEOT+"): CC return via 1/2 pathDistance = "+(ixMOT));
			return ix;
		}
	}
	//fpln(" $$ findMOT("+ixBOT+","+ixEOT+"): DD return last resort ixEOT-1 = "+(ixEOT-1));
	return ixEOT-1;
}


void Plan::revertGsTCPs() {
	int start = 0;
	for (int j = start; j < size(); j++) {
		//fpln(" $$$ REVERT GS AT j = "+j+" np_ix = "+point(j));
		revertGsTCP(j);
	}
}

void Plan::revertGsTCP(int ixBGS) {
	//fpln(" $$ structRevertGsTCP: ENTER this = "+toStringTrk());
	if (isBGS(ixBGS)) {
		//fpln("\n\n $$$$>>>>>>>>>>>>>>>>>>>>>> structRevertGsTCP:  point("+Fm0(ixBGS)+") = "+point(ixBGS).toString());
		int ixEGS = nextEGS(ixBGS);
		//f.pln(" $$ structRevertGsTCP: ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
		if (ixEGS < 0) {
			//f.pln(" $$$$---------------------- structRevertGsTCP : ERROR nextEGSix = "+nextEGSix);
			addError(" structRevertGsTCP: Ill-formed BGS-EGS structure: no EGS found!");
		}
		// store sequence of ground speeds within BGS - EGS
		double gsInBGS = gsIn(ixBGS);
		double gsOutEGS = gsOut(ixEGS);
		//f.pln(" $$$ structRevertGsTCP: gsOutEGS = "+Units.str("kn",gsOutEGS));
		TcpData& tcpEGS= getTcpDataRef(ixEGS);
		tcpEGS.clearEGS();
		TcpData& tcpBGS= getTcpDataRef(ixBGS);
		tcpBGS.clearBGS();
		//if (!saveAccel) tcpBGS.setGsAccel(0.0);
		std::string infoBGS = tcpBGS.getInformation();
		if (contains(infoBGS,Plan::manualGsAccel)) { // leave gsAccel data in Point ixBGS
			setInfo(ixBGS,replace(infoBGS,Plan::manualGsAccel,""));
		} else {
			tcpBGS.setGsAccel(0.0);
		}
		//fpln(" $$>>>>>>>>>>>>>>>>>> structRevertGsTCP: ixBGS = "+Fm0(ixBGS)+" this = "+toStringTrk());
		if (Util::almost_equals(gsOutEGS,0.0,PRECISION5)) {
			//fpln(" $$$ structRevertGsTCP: NEAR-0: gsOutEGS = "+Units::str("kn",gsOutEGS));
			setGsAccel(ixBGS,tcpBGS.getGsAccel());
			for (int i = ixBGS; i < ixEGS; i++) {
				mkGsOut(i,gsInBGS);
			}
			std::string name = getName(ixBGS);
			std::string info = getInfo(ixBGS)+getInfo(ixEGS);
			setName(ixEGS,name);
			setInfo(ixEGS,info);
			setName(ixBGS,"");
			setInfo(ixBGS,"");
			removeIfVsConstant(ixBGS);
		} else {
			setGsAccel(ixBGS,tcpBGS.getGsAccel());
			for (int i = ixBGS; i < ixEGS; i++) {
				mkGsOut(i,gsOutEGS);
			}
			//f.pln(" $$$$ structRevertGsTCP: AT ixBGS = "+ixBGS+" make gsOutBGS = "+Units.str("kn",gsOutBGS));
			bool trkF = true;
			bool gsF = false;
			bool vsF = true;
			removeIfRedundant(ixEGS, trkF, gsF, vsF);
			//f.pln(" $$$$ structRevertGsTCP: AT ixBGS = "+ixBGS+" make gsOut = gsOutEGS = "+Units.str("kn",gsOutEGS));
		}
	}
}

void Plan::crudeRevertGsTCP(int ixBGS) {
	if (isBGS(ixBGS)) {
		TcpData& tcpBGS = getTcpDataRef(ixBGS);
		tcpBGS.clearBGS();
		int ixEGS = nextEGS(ixBGS);
		//f.pln(" $$>>>> structRevertGsTCP: ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
		if (ixEGS < 0)	return;
		TcpData& tcpEGS = getTcpDataRef(ixEGS);
		tcpEGS.clearEGS();
		bool trkF = true;
		bool gsF = false;
		bool vsF = false;  // rely on AltPreserve
		removeIfRedundant(ixEGS, trkF, gsF, vsF);
	}

}



void Plan::revertVsTCPs() {
	int start = 0;
	int end = size()-1;
	if (start < 0)
		start = 0;
	if (end > size() - 1)
		end = size() - 1;
	for (int j = end; j >= start; j--) {
		//fpln(" $$$ REVERT VS TCP at j = "+Fm0(j)+" np_ix = "+point(j).toString());
		revertVsTCP(j);
	}
}


void Plan::repairPlan() {
	if (size() == 0) return;
	if (isEOT(0)) getTcpDataRef(0).clearEOT();
	if (isEGS(0)) getTcpDataRef(0).clearEGS();
	if (isEVS(0)) getTcpDataRef(0).clearEVS();
	if (size() > 1) {
		int lastIx = size()-1;
		if (isBOT(lastIx)) getTcpDataRef(lastIx).clearBOT();
		if (isBGS(lastIx)) getTcpDataRef(lastIx).clearBGS();
		if (isBVS(lastIx)) getTcpDataRef(lastIx).clearBVS();
	}
	// remove all end TCPs with no preceding Begin TCPs
	for (int i = 0; i < size(); i++) {
		if (isEOT(i) && (prevBOT(i) < 0 || prevBOT(i) < prevEOT(i))) {
			getTcpDataRef(i).clearEOT();
		}
		if (isEGS(i) && (prevBGS(i) < 0 || prevBGS(i) < prevEGS(i))) {
			getTcpDataRef(i).clearEGS();
		}
		if (isEVS(i) && (prevBVS(i) < 0 || prevBVS(i) < prevEVS(i))) {
			getTcpDataRef(i).clearEVS();
		}
	}
	if (inTrkChange(time(size()-1)) ) {
		//f.pln(" $$$$ cleanPlan: "+name+" last point is in TRK accel!!!");
		getTcpDataRef(size()-1).setEOT();
	}
	if (inGsChange(time(size()-1)) ) {
		//f.pln(" $$$$ cleanPlan: "+name+" last point is in GS accel!!!");
		getTcpDataRef(size()-1).setEGS();
	}
	if (inVsChange(time(size()-1)) ) {
		//f.pln(" $$$$ cleanPlan: "+name+" last point is in VS accel!!!");
		getTcpDataRef(size()-1).setEVS();
	}
}

// supply missing MOTs
void Plan::repairMOTs() {
	for (int i = 0; i < size(); i++) {
		if (isBOT(i)) {
			repairMOT(i);
		}
	}
}

/** supply missing MOTs
 *
 * @param ixBOT         index of a BOT
 */
void Plan::repairMOT(int ixBOT) {
	int ixEOT = nextEOT(ixBOT);
	if (ixEOT >= 0) {
		int ixMOT = findMOT(ixBOT);
		//f.pln(" $$$$ ixBOT = "+ixBOT+" ixMOT = "+ixMOT);
		if (ixMOT < 0) {
			double pathD = pathDistance(ixBOT,ixEOT);
			double halfDist = pathD/2.0;
			double tMOT = timeFromDistance(ixBOT, halfDist);
			Position MOTpos = position(tMOT);
			NavPoint MOT(MOTpos,tMOT);
			ixMOT = addNavPoint(MOT);
			setMOT(ixMOT);
		}
	}
}



void Plan::repairGsContinuity(int ixBGS, int ixEGS, double vo, double vf, double maxGsAccel) {
	getMessage();   // clear previous error messages
	double D = pathDistance(ixBGS,ixEGS);
	if (D > 0) {
		double delV = vf - vo;
		double newT = 2*D/(vo+vf);
		double newA = delV/newT;
		if (std::abs(newA) > std::abs(maxGsAccel)) {
			addError("fixGs_continuity: calculated gs acceleration exceeds maximum value a = "+Fm2(newA)+" maxGsAccel = "+Fm2(maxGsAccel));
			//fpln("fixGs_continuity: calculated gs acceleration exceeds maximum value a = "+a+" maxGsAccel = "+maxGsAccel);
		}
		//f.pln(" $$ fixGs_continuity: a = "+a+" dt = "+dt);
		setGsAccel(ixBGS,newA);
		repairGsContInside(ixBGS,ixEGS,vo);
	} else {
		addWarning("Cannot fix continuity at ixBGS = "+Fm0(ixBGS)+" because D = "+Fm1(D));
	}
}

void Plan::repairGsContinuity(int ixBGS, int ixEGS, double maxGsAccel) {
	double gsIn_d;
	if (ixBGS == 0) gsIn_d = gsOut(ixBGS);
	else gsIn_d = gsIn(ixBGS);
	double gsOut_d = gsOut(ixEGS);
	//f.pln(" $$$$$ fixGs_continuity("+ixBGS+","+ixEGS+")): gsIn("+ixBGS+") = "+Units::str("kn",gsIn)+" gsOut("+ixEGS+")  = "+Units::str("kn",gsOut));
	repairGsContinuity(ixBGS, ixEGS, gsIn_d, gsOut_d, maxGsAccel);
}


void Plan::repairGsContinuity(double maxGsAccel) {
	for (int ii = 0; ii < size(); ii++) {
		if (isBGS(ii)) {
			int ixBGS = prevBGS(ii+1);
			int ixEGS = nextEGS(ixBGS);
			repairGsContinuity(ixBGS,ixEGS,maxGsAccel);
		}
	}
}


bool Plan::repairGsContInside(int ixBGS, int ixEGS, double v0) {
	//f.pln(" $$$ fixGsContInside: ixBGS = "+ixBGS+" ixEGS = "+ixEGS);
	double a = gsAccel(ixBGS);
	bool rtn = true;
	//f.pln("$$$$$>>> fixGsContInside:  ixBGS = "+ixBGS+" lastIxInGsAccel = "+lastIxInGsAccel);
	double tBGS = time(ixBGS);
	double gsOutEGS = gsOut(ixEGS);
	if (ixEGS + 1 < size()) {
		timeShiftPlan(ixEGS+1,1E10);
	}
	for (int j = ixBGS+1; j <= ixEGS; j++) {
		double d_j = pathDistance(ixBGS,j);
		double t_j = tBGS + Plan::timeFromDistance(v0, a, d_j);
		if (ISNAN(t_j)) {
			addError("fixGsContInside: timeFromDistance generated NAN");
			//f.pln(" $$$$$$ PlanUtil.fixGsContInside: j = "+j+"  aBGS = "+a+" d_j = "+d_j+" t_j = "+t_j);
			rtn = false;
			//Debug.halt();
		} else {
			setTimeInPlace(j,t_j);
			//fpln(" $$$ fixGsContInside: set time of j = "+j+" to "+t_j);
		}
	}
	if (ixEGS+1 < size()) mkGsOut(ixEGS,gsOutEGS);                       // undo timeshift
	return rtn; // success

}


/**
 * Attempt to fix (gs) acceleration inconsistencies in this plan by adjusting accel values and/or point times.
 * @return new plan with fixed inconsistencies, or null if unfixable.  This new plan may have warnings (informational, possibly not concerning) or errors (possibly concerning).
 */
 void Plan::repairGsConsistency() {
	for (int i = 0; i < size(); i++) {
			//p.repairGsInNeg(i);
		if (isBGS(i)) repairGsConsistencyAt(i);
		repairNegGsOut(i);
		repairNegGsIn(i);
	}
	//mergeClosePoints();
}

 void Plan::repairGsConsistencyAt(int ixBGS) {
	//f.pln(" $$ repairGsConsistencyAt: ENTER ixBGS = "+ixBGS);
	if (ixBGS < 0 || ixBGS > size() || ! isBGS(ixBGS)) return;
	int ixEGS = nextEGS(ixBGS);
	if (ixEGS >= 0) {
		if ( ! checkBGS(ixBGS)) {
			double d = 	pathDistance(ixBGS,ixEGS);
			double dt = time(ixEGS) - time(ixBGS);
			double a = gsAccel(ixBGS);
			double newAccel;
			if (a >= 0) {
				newAccel = 2*d/(dt*dt) - 1E-5;
			} else {
				newAccel = -2*d/(dt*dt) + 1E-5;
			}
			//f.pln(" $$>>>>>>>>>>>>>  repairGsConsistencyAt: at ixBGS = "+ixBGS+" CHANGE a = "+a+" TO    newAccel = "+newAccel);
			setGsAccel(ixBGS,newAccel);
		}
	}
}


/** Repair a negative GsIn value at ix
 *
 * @param ix
 */
 void Plan::repairNegGsIn(int ix) {
	if (ix < 1 || ix >= size()) return;
	if (gsFinalCalc(ix-1,false) >= 0) return;
	double dist = pathDistance(ix-1, ix, false);
	double dt = time(ix) - time(ix-1);
	if (isBGS(ix-1)) {
		double a = -2.0*dist / (dt * dt);
		//f.pln(" $$ repairNegGsIn: gsInCalc("+ix+") = "+gsInCalc(ix)+" change a from "+getGsAccel(ix-1)+" TO a = "+a);
		setGsAccel(ix-1,a);
	} else {
		int ixBGS = prevBGS(ix);
		double a = getGsAccel(ixBGS);
		//f.pln(" $$ repairNegGsIn: a = "+a+" dist = "+dist);
		double newT = time(ix-1) + std::sqrt(2.0*dist/std::abs(a));
		//f.pln(" $$ repairNegGsIn: gsInCalc("+ix+") = "+gsInCalc(ix)+" change time from "+time(ix)+" to "+newT);
		setTime(ix,newT);
	}
}


/** Repair a negative GsOut value at ix
 *
 * @param ix
 */

 void Plan::repairNegGsOut(int ix) {
	if (ix < 1 || ix >= size()) return;
	if (gsOutCalc(ix) < 0) {
		int ixBGS = prevBGS(ix+1);
		if (ixBGS >= 0) {
			double a = getGsAccel(ixBGS);
			double dist = pathDistance(ix, ix+1, false);
			//f.pln(" $$ repairNegGsOut: a = "+a+" dist = "+dist);
			double newT = time(ix) + std::sqrt(2.0*dist/std::abs(a));
			//f.pln(" $$ repairNegGsOut: gsOutCalc("+ix+") = "+gsOutCalc(ix)+" change time("+(ix+1)+") from "+time(ix+1)+" to "+newT);
			setTime(ix+1,newT);
		}
	}
}





 /**
  * Structurally revert Turn TCP at ix. This method assumes ix &gt; 0 AND ix &lt;
  * pln.size(). If ix is not a BOT, then nothing is done
  *
  * NOTE:  Assumes BGS_EGS and BVS-EVS TCPs have already been reverted
  *
  * @param ixBOT    index of point to be reverted
  */
 void Plan::revertTurnTCP(int ixBOT) { // , bool killNextGsTCPs) {
	 if (!isBOT(ixBOT)) return;
	 //f.pln("\n\n $$$$$ structRevertTurnTCP: ixBOT = "+ixBOT+" isBOT = "+isBOT(ixBOT)+" "+killNextGsTCPs);
	 NavPoint BOT = point(ixBOT);
	 std::string name = "";
	 int ix_EOT = nextEOT(ixBOT);
	 int ixMOT = findMOT(ixBOT,ix_EOT);
	 if (ixMOT >= 0) {                     // in the rare cases there is no MOT
		 NavPoint MOT = point(ixMOT);
		 name = MOT.name();
	 }
	 double radius = std::abs(getTcpData(ixBOT).getRadiusSigned());
	 int ixEOT = nextEOT(ixBOT);
	 double dist2Mid = pathDistance(ixBOT,ixEOT)/2.0;
	 //f.pln(" $$$ structRevertTurnTCP: ixBOT = "+ixBOT+" info = "+info+" dist2Mid = "+Units.str("ft",dist2Mid));
	 bool linear = false;
	 double tMid = timeFromDistance(ixBOT, dist2Mid);
	 //f.pln(" $$$ structRevertTurnTCP: timeFromDistance =  "+timeFromDistance(ixBOT, dist2Mid));
	 Position posMid = position(tMid);
	 double altMid = posMid.alt();
	 NavPoint vertex = vertexPointTurnTcps(ixBOT,ixEOT,altMid).makeName(name);
	if (vertex.isInvalid()) {
		fpln("$$$ structRevertTurnTCP: ixBOT = "+Fm0(ixBOT)+" calculated vertex is invalid!");
	}
	double turnDist = pathDistance(ixBOT,ixEOT);
	double vertexDist = point(ixBOT).position().distanceH(vertex.position()) + point(ixEOT).position().distanceH(vertex.position());
	double ratio = vertexDist/turnDist;
	Plan turnCut = cut(ixBOT,ixEOT);
	//f.pln(" $$$$ turnCut = "+turnCut.toStringGs());
	// ======================== No Changes To Plan Before This Point ================================
	// ========================= REMOVE all points between ixBOT and ixEOT =============================
    bool vertexShouldBeAltPreserve = false;
	for (int k = ixEOT-1; k > ixBOT; k--) {  // remove (ixBOT,ixEOT)
		//f.pln(" $$$$ structRevertTurnTCP: remove point k = "+k+" "+point(k)+" getInfo = "+getInfo(k));
		if (isAltPreserve(k)) vertexShouldBeAltPreserve = true;
		remove(k);
	}
	//f.pln(" $$$ name = "+name+" info = "+info);
	timeShiftPlan(ixBOT+1,10000);                // this will be corrected later: revertedTurn.mkGsOut(i,gsOut_i)
	int ixVert = addNavPoint(vertex);             // Need to add vertex so new distances can be calculated
	std::string infoBOT = getInfo(ixBOT);
	if (contains(infoBOT, Plan::manualRadius)) {
	   setVertexRadius(ixVert, radius);              // Sets vertex radius
	   setInfo(ixBOT,replace(infoBOT,Plan::manualRadius,""));
	}
	double lastTm = turnCut.time(0);
	Plan revertedTurn = Plan();
	for (int i = 0; i < turnCut.size(); i++) {   // do not process EOT
		double d_i = ratio*turnCut.pathDistance(0,i);
		double alt_i = turnCut.alt(i);
		std::string name_i = turnCut.point(i).name();
		std::string info_i = turnCut.getInfo(i);
		//f.pln(" $$>>>>>> structRevertTurnTCP:  i = "+i+" info_i = "+info_i+" d_i = "+Units.str("NM",d_i));
		if (turnCut.isMOT(i)) {
			NavPoint pos_i = NavPoint(vertex.position(),lastTm+100*i).makeName(name);
			revertedTurn.addNavPoint(pos_i);
			if (TcpData::motFlagInInfo) info_i = replace(info_i,TcpData::MOTflag,"");
			revertedTurn.clearMOT(i);
		} else {
			linear = true;
			std::pair<Position, int> adv = advanceDistance2D(ixBOT, d_i, linear);
			Position pos_i = adv.first.mkAlt(alt_i);
			//f.pln(" $$ structRevertTurnTCP: ADD i = "+i+" info_i = "+info_i);
			NavPoint rev_i = NavPoint(pos_i,lastTm+100*i).makeName(name_i);
			revertedTurn.addNavPoint(rev_i);
		}
		revertedTurn.setInfo(i,info_i);
		lastTm = revertedTurn.time(i);
		//f.pln(" $$ structRevertTurnTCP: i = "+i+"  gsIn_i_i = "+Units.str("kn",gsIn_i));
	}
	//fpln(" $$ structRevertTurnTCP: revertedTurn = "+revertedTurn.toString());
	for (int i = 0; i < turnCut.size(); i++) {   // transfer speeds to reverted turn
		double gsOut_i = turnCut.gsOut(i);
		revertedTurn.mkGsOut(i,gsOut_i);
	}
	ixEOT = nextEOT(ixBOT);   // EOT may now have new index
	//f.pln(" $$  AFTER revertedTurn = "+revertedTurn.toStringGs());
	getTcpDataRef(ixBOT).clearBOT();
	clearName(ixBOT);
	getTcpDataRef(ixEOT).clearEOT();
	remove(ixBOT+1);   // remove vertex because it is in the pMids list
	for (int j = 1; j < revertedTurn.size()-1; j++) {  // note we start with 1, i.e. do not add ixBOT
		int ix = addNavPoint(revertedTurn.point(j));
		//f.pln(" $$$ structRevertTurnTCP: ix = "+ix+" point(+"+j+") = "+revertedTurn.point(j));
		setInfo(ix,revertedTurn.getInfo(j));
		double gsOut_j = turnCut.gsOut(j);
		mkGsOut(ix,gsOut_j);
		// save manual settings for GS and VS acceleration
		double gsAccel_j = turnCut.getGsAccel(j);
		if (gsAccel_j > 0) setGsAccel(ix,gsAccel_j);
		double vsAccel_j = turnCut.getVsAccel(j);
		if (std::abs(vsAccel_j) > 0) setVsAccel(ix,vsAccel_j);
	}
	if (vertexShouldBeAltPreserve) {
	    int ix_mot = findMOT(ixBOT,ixEOT);
	    if (ix_mot >=0) setAltPreserve(ix_mot);
	}
	for (int  jj  = ixBOT + revertedTurn.size()-1; jj >= ixBOT; jj--) {
		removeIfRedundant(jj);
	}
	mergeClosePoints();
	//f.pln(" $$$ structRevertTurnTCP: AFTER merge: this = "+this);
}

void Plan::revertTurnTCPs() {
	//f.pln(" $$$ revertGsTCPs start = "+start+" size = "+size());
	for (int j = 0; j < size(); j++) {
		//bool killNextGsTCPs = false;
		revertTurnTCP(j); // , killNextGsTCPs);
	}
}



// assumes ix > 0 AND ix < size()
int Plan::revertVsTCP(int ixBVS) {
	if (isBVS(ixBVS)) {
		//fpln(" $$ revertVsTCP: ENTER ixBVS = "+Fm0(ixBVS)+" this = "+toStringVs());
		NavPoint BVS = point(ixBVS);
		int ixEVS = nextEVS(ixBVS);
		if (ixEVS < 0) {
			addError(" $$$ revertVsTCP: Plan "+getID()+" is not well-formed at ix = "+Fm0(ixBVS));
			return ixBVS;
		}
		NavPoint EVS = point(ixEVS);
		double vsin;
		if (ixBVS == 0) vsin = vsOut(ixBVS);
		else vsin = vsIn(ixBVS);
		double dt = EVS.time() - BVS.time();
		double tVertex = BVS.time() + dt/2.0;
		double zVertex = BVS.z() + vsin*dt/2.0;
		Position pVertex = position(tVertex);
		std::string info = getInfo(ixBVS)+getInfo(ixEVS); // NOTE: ATS sometimes puts $TOD in EVS rather than BVS
		std::string name = point(ixBVS).name();
		if (ixEVS == size()-1) {       // if BVS name is empty, use name in EVS
			if (name=="") {           // move name of BVS/EVS to reverted point
				name = getName(ixEVS);
				setName(ixEVS,"");
			} else {
				setName(ixBVS,"");
			}
		}
		NavPoint vertex = NavPoint(pVertex.mkAlt(zVertex), tVertex).makeName(name);
		//fpln(" $$$$ revertVsTCP: ixBVS = "+Fm0(ixBVS)+" vertex = "+vertex.toString()+" info = "+info);
		for (int j = ixBVS+1; j < ixEVS; j++) {
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
		std::string infoBVS = getInfo(ixBVS);
		//fpln(" $$$$ revertVsTCP: ixBVS = "+Fm0(ixBVS)+" infoBVS = "+infoBVS);
		double vsAccel_bvs;
		if (contains(infoBVS,Plan::manualVsAccel)) {
			//setInfo(ixBVS,replace(infoBVS,Plan::manualVsAccel,""));
			vsAccel_bvs = vsAccel(ixBVS);
		} else {
			vsAccel_bvs = 0;
		}
		//fpln(" $$$$ revertVsTCP: ixBVS = "+Fm0(ixBVS)+" vsAccel_bvs = "+Fm2(vsAccel_bvs));
		getTcpDataRef(ixBVS).clearBVS();
		clearName(ixBVS);
		clearInfo(ixBVS);
		getTcpDataRef(ixEVS).clearEVS();
		removeIfRedundant(ixEVS);
		removeIfRedundant(ixBVS);
		int ixVertex = addNavPoint(vertex);
		//fpln(" $$$$ revertVsTCP: APPEND at ixVertex = "+Fm0(ixVertex)+" info = "+info);
		appendInfo(ixVertex,replace(info,Plan::manualVsAccel,""));
		setAltPreserve(ixVertex);
		//fpln(" $$$$ revertVsTCP: ixVertex = "+Fm0(ixVertex)+" vsAccel_bvs = "+Fm2(vsAccel_bvs));
		setVsAccel(ixBVS,vsAccel_bvs);   // save old vsAccel in linear plan
		mergeClosePoints();
		//fpln(" $$$$ revertVsTCP: EXIT "+toStringGs());
		return ixVertex;
	}
	return -1;
}

double Plan::inferredVsIn(int ixBVS) {
	double vin = 0.0;
	if (isBVS(ixBVS)) {
         double a = vsAccel(ixBVS);
         int ixEVS = nextEVS(ixBVS);
         if (ixEVS >= 0) {
     		double dz = alt(ixEVS) - alt(ixBVS);
     		double dt = time(ixEVS) - time(ixBVS);
            vin = (dz - 0.5*a*dt*dt)/dt;
         } else {
		     addError("inferedVsIn: attempted on an inconsistent plan");
         }
	} else {
	     addError("inferedVsIn: attempt on non BVS point!");
	}
	return vin;
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


std::pair<int,std::string> Plan::wellFormed(bool strongMOT) const{
	if (size() < 2) return std::pair<int,std::string>(0,"Plan must have at least two points!");
	double lastTm = -1;
	for (int i = 0; i < size(); i++) {
		NavPoint np = point(i);
		if (isBOT(i)) {
			int j1 = nextBOT(i);
			int j2 = nextEOT(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return std::pair<int,std::string>(i, "BOT at i "+Fm0(i)+" NOT FOLLOWED BY EOT!");
			int ixMOT = findMOT(i);
			if (ixMOT < 0 || ixMOT >= j2)
				return std::pair<int,std::string>(i, " NO MOT IN TURN in turn at i = "+Fm0(i)+"!");
			else if (strongMOT && ! isMOT(ixMOT)) return std::pair<int,std::string>(i, "at i = "+Fm0(i)+" midPoint inside [BOT,EOT] not marked as MOT");
		}
		if (isEOT(i)) {
			int j1 = prevBOT(i);
			int j2 = prevEOT(i);
			if (!(j1 >= 0 && j1 >= j2)) return std::pair<int,std::string>(i, "EOT at i "+Fm0(i)+" NOT PRECEEDED BY BOT!");
		}
		if (isBGS(i)) {
			int j1 = nextBGS(i);
			int j2 = nextEGS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return std::pair<int,std::string>(i, "BGS at i "+Fm0(i)+" NOT FOLLOWED BY EGS!");
		}
		if (isEGS(i)) {
			int j1 = prevBGS(i);
			int j2 = prevEGS(i);
			if (!(j1 >= 0 && j1 >= j2)) return std::pair<int,std::string>(i, "EGS at i "+Fm0(i)+" NOT PRECEEDED BY BGS!");
		}
		if (isBVS(i)) {
			int j1 = nextBVS(i);
			int j2 = nextEVS(i);
			if (j2 < 0 || (j1 > 0 && j1 < j2)) return std::pair<int,std::string>(i, "BVS at i "+Fm0(i)+" NOT FOLLOWED BY EVS!");
		}
		if (isEVS(i)) {
			int j1 = prevBVS(i);
			int j2 = prevEVS(i);
			if (!(j1 >= 0 && j1 >= j2)) return std::pair<int,std::string>(i, "EVS at i "+Fm0(i)+" NOT PRECEEDED BY BVS!");
		}
		double tm_i = time(i);
		if (i > 0) {
			if (time(i) <= time(i-1)) {
				return std::pair<int,std::string>(i, "Point times at i = "+Fm0(i)+" are out of order!");
			}
			double dt = std::abs(tm_i-lastTm);
			if (dt < minDt) {
				return std::pair<int,std::string>(i, "Delta time into i = "+Fm0(i)+" is less than minDt = "+Fm12(minDt));
			}
			lastTm = tm_i;
		}
	}
	return std::pair<int,std::string>(-1, "");


}

bool Plan::isWellFormed() const {
	//if (size() == 0) return true;
	return indexWellFormed() < 0;
}


/**
 * This returns -1 if the entire plan is "well formed", i.e. all acceleration zones have a matching beginning and end point.
 * Returns a nonnegative value to indicate the problem point
 */
int Plan::indexWellFormed() const {
	bool strongMOT = false;
	return wellFormed(strongMOT).first;
}

//	if (size() < 2) return 0;
//	double lastTm = -1;
//	for (int i = 0; i < size(); i++) {
//		NavPoint np = point(i);
//		if (isBOT(i)) {
//			int j1 = nextBOT(i);
//			int j2 = nextEOT(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
//			int ixMOT = findMOT(i);
//			if (ixMOT < 0 || ixMOT >= j2) return i;
//		}
//		if (isEOT(i)) {
//			int j1 = prevBOT(i);
//			int j2 = prevEOT(i);
//			if (!(j1 >= 0 && j1 >= j2)) return i;
//		}
//		if (isBGS(i)) {
//			int j1 = nextBGS(i);
//			int j2 = nextEGS(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
//		}
//		if (isEGS(i)) {
//			int j1 = prevBGS(i);
//			int j2 = prevEGS(i);
//			if (!(j1 >= 0 && j1 >= j2)) return i;
//		}
//		if (isBVS(i)) {
//			int j1 = nextBVS(i);
//			int j2 = nextEVS(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return i;
//		}
//		if (isEVS(i)) {
//			int j1 = prevBVS(i);
//			int j2 = prevEVS(i);
//			if (!(j1 >= 0 && j1 >= j2)) return i;
//		}
//		double tm_i = time(i);
//		if (i > 0) {
//			double dt = std::abs(tm_i-lastTm);
//			if (dt < minDt) {
//				fpln("$$ isWellFormed: Delta time into i = "+Fm0(i)+" is less than minDt = "+Fm8(minDt));
//				return i;
//			}
//			lastTm = tm_i;
//		}
//
//
//	}
//	return -1;
//}


/**
 * This returns a string representing which part of the plan is not
 * "well formed", i.e. all acceleration zones have a matching beginning and end point.
 */
std::string Plan::strWellFormed() const {
	bool strongMOT = true;
	return wellFormed(strongMOT).second;

}
//	std::string rtn = "";
//	double lastTm = -1;
//	for (int i = 0; i < size(); i++) {
//		NavPoint np = point(i);
//		// not well formed if GSC overlaps with other accel zones
//		//			if ((isTurn() || isVSC()) && inGroundSpeedChange(np.time())) rtn = false;
//		//			if (isGSC() && (inTurn(np.time()) || inVerticalSpeedChange(np.time()))) rtn = false;
//		if (isBOT(i)) {
//			int j1 = nextBOT(i);
//			int j2 = nextEOT(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BOT at i "+Fm0(i)+" NOT FOLLOWED BY EOT!";
//			int ixMOT = findMOT(i);
//			if (ixMOT < 0 || ixMOT >= j2)
//				return " NO MOT IN TURN in turn at i = "+Fm0(i)+"!";
//			else if (! isMOT(ixMOT)) return "at i = "+Fm0(i)+" midPoint inside [BOT,EOT] not marked as MOT";
//		}
//		if (isEOT(i)) {
//			int j1 = prevBOT(i);
//			int j2 = prevEOT(i);
//			if (!(j1 >= 0 && j1 >= j2)) return "EOT at i "+Fm0(i)+" NOT PRECEEDED BY BOT!";
//		}
//		if (isBGS(i)) {
//			int j1 = nextBGS(i);
//			int j2 = nextEGS(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BGS at i "+Fm0(i)+" NOT FOLLOWED BY EGS!";
//		}
//		if (isEGS(i)) {
//			int j1 = prevBGS(i);
//			int j2 = prevEGS(i);
//			if (!(j1 >= 0 && j1 >= j2)) return "EGS at i "+Fm0(i)+" NOT PRECEEDED BY BGS!";
//		}
//		if (isBVS(i)) {
//			int j1 = nextBVS(i);
//			int j2 = nextEVS(i);
//			if (j2 < 0 || (j1 > 0 && j1 < j2)) return "BVS at i "+Fm0(i)+" NOT FOLLOWED BY EVS!";
//		}
//		if (isEVS(i)) {
//			int j1 = prevBVS(i);
//			int j2 = prevEVS(i);
//			if (!(j1 >= 0 && j1 >= j2)) return "EVS at i "+Fm0(i)+" NOT PRECEEDED BY BVS!";
//		}
//
////		if (inGsChange(np.time()) && inTrkChange(np.time())) {
////			rtn = rtn + "  Overlap FAIL at i = "+Fm0(i);
////		}
//		double tm_i = time(i);
//		if (i > 0) {
//			double dt = std::abs(tm_i-lastTm);
//			if (dt < minDt) {
//				return "Delta time into i = "+Fm0(i)+" is less than minDt = "+Fm12(minDt);
//			}
//			lastTm = tm_i;
//		}
//	}
//	return rtn;
//}

int Plan::findBot(int i) const {
	if (inTrkChange(point(i).time())) {
		int ixBOT = prevBOT(i+1);
		return ixBOT;
	}
	return -1;
}


bool Plan::isGsConsistent(int ix, double distEpsilon, bool silent, double nearZeroGsValue ) const {
	bool linear = false;
	double gsOut_ix = gsOutCalc(ix,linear);
	if (gsOut_ix < -nearZeroGsValue ) {
		if ( ! silent) {
			fpln(" >>> isGsConsistent"+getID()+": GS NEGATIVE! gsOut ("+Fm0(ix)+") = "+Units::str("kn",gsOut_ix));
		}
		return false;
	}
	if (ix > 0) {
		double gsFinal_ix = gsInCalc(ix);
		//f.pln(" $$ isGsConsistent: at i = "+ixBGS+" gsIn = "+Units.str("kn",gsFinal_ixM1,15));
		if (gsFinal_ix < -nearZeroGsValue) {
			if ( ! silent ) fpln(" >>> isGsConsistent"+getID()+": GS NEGATIVE! gsIn("+Fm0(ix-1)+") = "+Units::str("kn",gsFinal_ix,15));
			return false;
		}
	}
	if ( ! isBGS(ix))  return true;
	bool rtn = true;
	if (newConsistencyAlg) {
		rtn = checkBGS(ix,silent);		//f.pln(" $$$$$$$$$$$$$$$$$$$ isGsConsistent: rtn = "+rtn);
	} else  {
		NavPoint BGS = point(ix);
		int ixEGS = nextEGS(ix);
		if (ixEGS < 0) {
			if ( ! silent ) fpln(" >>> isGsConsistent("+getID()+"): ix = "+Fm0(ix)+" ixEGS = "+Fm0(ixEGS));
			return false; // plan not well-formed
		}
		NavPoint EGS = point(ixEGS);
		double gsOutBGS = gsOut(ix);
		rtn = true;
		double dt = EGS.time() - BGS.time();
		double aBGS = gsAccel(ix);
		double ds = gsOutBGS*dt + 0.5*aBGS*dt*dt;
		double distH = 	pathDistance(ix,ixEGS);
		double absDiff = std::abs(ds-distH);
		if (absDiff > distEpsilon) {
			if (!silent) {
				fpln(" >>> isConsistent"+getID()+": GS FAIL! at i = "+Fm0(ix)+" GSC section fails test! difference = "+Units::str("ft",absDiff,8));
			}
			rtn = false;
		}
	}
	return rtn;
}

bool Plan::checkBGS(int ixBGS) const {
    return checkBGS(ixBGS,true);
}

bool Plan::checkBGS(int ixBGS, bool silent) const {
	bool ok = true;
	int ixEGS = nextEGS(ixBGS);
	if (ixEGS >= 0) {
		double dt = time(ixEGS) - time(ixBGS);
		double d = 	pathDistance(ixBGS,ixEGS);
		double a = gsAccel(ixBGS);
        // need v0 = d/t - 0.5*a*t >= 0 and vf = d/t + 0.5*a*t>= 0
		double epsilon = 1E-7;  // for floating point errors
		if (a >= 0) {
			ok = d*(1+epsilon) + epsilon/100.0 > 0.5*a*dt*dt;
		} else {
			ok = d*(1+epsilon) + epsilon/100.0 > -0.5*a*dt*dt;
		}
		if (!ok && ! silent ) {
			fpln(" >>> isGsConsistent("+getID()+"): GS FAIL! at i = "+Fm0(ixBGS)+" GSC section fails for a = "+Fm1(a)+" d = "+Fm1(d)+" >??  0.5*|a|*dt*dt = "+Fm1(0.5*std::abs(a)*dt*dt));
		}
		//f.pln(" $$$$$$ checkBGS: ixBGS = "+ixBGS+" dt = "+dt+" a = "+a+" d = "+d+" >??  0.5*|a|*dt*dt = "+(0.5*Math.abs(a)*dt*dt)+" ok = "+ok);
	}
    return ok;
}


/**
 * This returns true if the entire plan produces reasonable accelerations.
 * If the plan has instantaneous "jumps," it is not consistent.
 * @param silent use true to ensure limited console messages
 * @return true if consistent
 */
bool Plan::isGsConsistent(bool silent) {
	for (int i = 0; i < size(); i++) {
	   if (!isGsConsistent(i, 0.001, silent, nearlyZeroGs)) return false;
	}
	return true;
}


bool Plan::isVsConsistent(int ix, double distEpsilon, bool silent) const {
	if ( ! isBVS(ix)) {
		return true;
	}
	NavPoint VSCBegin = point(ix);
	int ixEVS = nextEVS(ix);
	if (ixEVS < 0) return false; // plan not well-formed
	NavPoint VSCEnd = point(ixEVS);
	double a = vsAccel(ix);
	double dt = VSCEnd.time() - VSCBegin.time();
	double ds = vsOut(ix)*dt + 0.5*a*dt*dt;
	double deltaAlt = VSCEnd.alt() - VSCBegin.alt();
	double absDiff = std::abs(ds-deltaAlt);
	if (absDiff > distEpsilon) {
		if ( ! silent) {
			fpln(" >>> isVsConsistent"+getID()+": VS FAIL!  at i = "+Fm0(ix)+" VSC Section fails Test! absDiff = "+Units::str("ft",absDiff,8));
		}
		return false;
	}
	return true;
}


bool Plan::isTurnConsistent(int i, double distH_Epsilon, bool silent) const {
	int ixBOT = findBot(i);
	if (ixBOT < 0) return true;  // i is not in a turn, so yes it is consistent
	int ixEOT = nextEOT(i);
	if (ixEOT < 0) return false; // plan is not well-formed
	bool rtn = true;
	Position center = turnCenter(ixBOT);
	if (center.isInvalid()) {
		if ( ! silent) fpln(" >>> isTurnConsistent"+getID()+": "+getID()+" at i = +"+Fm0(i)+" turn center is invalid!");
		rtn = false;
	} else {
		//double turnRadius = p.getTcpData(ixBOT).turnRadius(); // unsigned radius
		Position BOT = getPos(ixBOT);
		double calcRadius = BOT.distanceH(center);
		bool containsMOTflag = false;
		for (int ix = ixBOT; ix < ixEOT; ix++) {
			if (isMOT(ix)) containsMOTflag = true;
			double distanceFromCenter = point(ix).position().distanceH(center);
			if (std::abs(distanceFromCenter - calcRadius) > distH_Epsilon) {
				if ( ! silent) {
					fpln(" >>> isTurnConsistent: "+getID()+" POINT OFF CIRCLE at ix = "+Fmi(ix)+" deltaRadius = "+Units::str("NM",(distanceFromCenter - calcRadius)));
				}
				rtn = false;
			}
		}
		if (!containsMOTflag) {
			if ( ! silent) {
				fpln(" >>> isTurnConsistent: WARNING "+getID()+" does not have MOT in turn at "+Fm0(ixBOT));
				//rtn = false;
			}
		}

	}
	return rtn;
}


bool Plan::isTurnConsistent(bool silent) const {
	bool rtn = true;
	if ( ! isWellFormed()) {
		if (!silent) {
			fpln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
		}
		error.addError("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if (isBOT(i)) {
			rtn = isTurnConsistent(i, em6DegtoMeter, silent);
		}
	}
	return rtn;
}


bool Plan::isVsConsistent(bool silent) const {
	bool rtn = true;
	if (!isWellFormed()) {
		if (!silent) {
			fpln("  >>> isConsistent"+getID()+": FAIL! not WellFormed!! " + strWellFormed());
		}
		error.addError("  >>> isConsistent"+getID()+": FAIL! not WellFormed!! " + strWellFormed());
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if (isBVS(i)) {
			if ( ! isVsConsistent(i, 0.001, silent)) {
				// error.addWarning("isConsistent fail: "+i+" Not vs
				// consistent!");
				rtn = false;
			}
		}
	}
	return rtn;
}


bool Plan::isConsistent(double maxTrkDist, double maxGsDist, double maxVsDist, bool silent, double nearZeroGsValue) const {
	bool rtn = true;
	if (!isWellFormed()) {
		if ( ! silent) fpln("  >>> isConsistent FAIL! not WellFormed!! " + strWellFormed());
		error.addError("isConsistent"+getID()+": not well formed");
		return false;
	}
	for (int i = 0; i < size(); i++) {
		if ( ! isTurnConsistent(i, maxTrkDist, silent)) {
			error.addWarning("isConsistent: turn "+Fm0(i)+" not consistent");
			if ( ! silent) fpln("isConsistent: turn " + Fm0(i) + " not consistent");
			rtn = false;
		}
		if ( ! isGsConsistent(i, maxGsDist, silent, nearZeroGsValue)) {
			error.addWarning("isConsistent: GS "+Fm0(i)+" not consistent");
			if ( ! silent) fpln("isConsistent: GS " + Fm0(i) + " not consistent");
			rtn = false;
		}
		if ( ! isVsConsistent(i, maxVsDist, silent)) {
			error.addWarning("isConsistent: VS "+Fm0(i)+" not consistent");
			if ( ! silent) fpln("isConsistent: VS " + Fm0(i) + " not consistent");
			rtn = false;
		}
	}
	return rtn;
}

bool Plan::isConsistent() const {
	bool silent = true;
	return isConsistent(silent);
}


bool Plan::isConsistent(bool silent) const {
	return isConsistent(em6DegtoMeter,0.007,0.001,silent,nearlyZeroGs);
}


bool Plan::isWeakConsistent(bool silent) const {
	return isConsistent(em6DegtoMeter,0.05,0.1,silent, 100*nearlyZeroGs);
}

bool Plan::isWeakConsistent() const {
	bool silent = true;
	return isWeakConsistent(silent);
}



bool Plan::isTrkContinuous(int i, double trkEpsilon, bool silent) const {
	double gsIn_i = gsIn(i);
	double gsOut_i = gsOut(i);
	if (Util::almost_equals(gsIn_i,0.0,PRECISION5) || Util::almost_equals(gsOut_i,0.0,PRECISION5)) return true;
	bool rtn = true;
	double trkDel = trkDelta(i);
	if (! isTrkTCP(i) && (trkDel < Plan::MIN_TRK_DELTA_GEN || gsOut(i) < 1E-10)) return true;
	if ( std::abs(trkDel) > trkEpsilon) {
		if (!silent) fpln(" $$ isTrkContinuous"+getID()+": FAIL trkDelta ("+Fm0(i)+") = "+Units::str("deg",trkDel));
		rtn = false;
	}
	return rtn;
}

bool Plan::isTrkContinuous(int i, bool silent) const {
	return isTrkContinuous(i,MIN_TRK_DELTA_GEN,silent);
}

bool Plan::isTrkContinuous(bool silent) const {
	for (int i = 1; i < size()-1; i++) {
      if (! isTrkContinuous(i,silent)) return false;
	}
	return true;
}

bool Plan::isGsContinuous(int i, double gsEpsilon, bool silent) const {
	bool rtn = true;
	double gsDelta = std::abs(gsOut(i) - gsIn(i));
	if (! isGsTCP(i) && gsDelta < Plan::MIN_GS_DELTA_GEN) return true;
	if (gsDelta > gsEpsilon) {
		if (!silent) {
			fpln(" $$ isGsContinuous"+getID()+": FAIL gsIn ("+Fm0(i)+") = "+Units::str("kn",gsIn(i))+"  gsOut ("+Fm0(i)+") = "+Units::str("kn",gsOut(i))+" gsDelta = "+Units::str("kn",gsDelta));
		}
		rtn = false;
	}
	return rtn;
}

bool Plan::isGsContinuous(int i, bool silent)  const{
	return isGsContinuous(i,MIN_GS_DELTA_GEN,silent);
}

bool Plan::isGsContinuous(bool silent)  const {
	for (int i = 1; i < size()-1; i++) {
      if (!isGsContinuous(i,silent)) return false;
	}
	return true;
}

bool Plan::isVsContinuous(int i, double velEpsilon, bool silent) const {
	bool rtn = true;
	double vsDelta = std::abs(vsIn(i) - vsOut(i));
	if (! isVsTCP(i) && vsDelta < Plan::MIN_VS_DELTA_GEN) return true;
	if (vsDelta > velEpsilon) {
		if (!silent) {
			fpln(" $$ isVsContinuous"+getID()+": FAIL vsIn ("+Fm0(i)+") = "+Units::str("fpm",vsIn(i))+" vsOut ("+Fm0(i)+") = "+Units::str("fpm",vsOut(i))+" vsDelta  = "+Units::str("fpm",vsDelta));
		}
		rtn = false;
	}
	return rtn;
}

bool Plan::isVsContinuous(int i, bool silent)  const{
	return isVsContinuous(i,MIN_VS_DELTA_GEN,silent);
}

bool Plan::isVsContinuous(bool silent)  const{
	for (int i = 1; i < size(); i++) {
      if (!isVsContinuous(i,silent)) return false;
	}
	return true;
}



bool Plan::isVelocityContinuous() const {
	return isVelocityContinuous(true);
}


bool Plan::isVelocityContinuous(bool silent) const {
	for (int i = 1; i < size(); i++) {
//		if (!isTrkContinuous(i, Units::from("deg", 2.0), silent)) return false;
//		if (!isGsContinuous(i, Units::from("kn", 10), silent)) return false;
//		if (!isVsContinuous(i, Units::from("fpm", 100), silent)) return false;
		if (!isTrkContinuous(i, MIN_TRK_DELTA_GEN, silent)) return false;    // TODO: SWITCH TO THESE PARAMS
		if (!isGsContinuous(i, MIN_GS_DELTA_GEN, silent)) return false;
		if (!isVsContinuous(i, MIN_VS_DELTA_GEN, silent)) return false;
	}
	return true;
}

bool Plan::isWeakVelocityContinuous(bool silent) const {
	for (int i = 1; i < size(); i++) {
		if (!isTrkContinuous(i, Units::from("deg", 5.0), silent)) return false;
		if (!isGsContinuous(i, Units::from("kn", 20), silent)) return false;
		if (!isVsContinuous(i, Units::from("fpm", 300), silent)) return false;
	}
	return true;
}



bool Plan::isFlyable() const {
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

bool Plan::isWeakFlyable() {
	bool silent = true;
	return isWeakFlyable(silent);
}


std::pair<NavPoint,TcpData> Plan::makeBOT(Position p, double t,  double signedRadius, const Position& center) {
	NavPoint np(p,t);
	TcpData tcp = TcpData().setBOT(signedRadius, center);
	return  std::pair<NavPoint,TcpData>(np,tcp);
}

std::pair<NavPoint,TcpData> Plan::makeEOT(Position p, double t) {
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData().setEOT();
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEOTBOT( Position p, double t,  double signedRadius, const Position& center) {
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData().setEOTBOT(signedRadius, center);
	return  std::pair<NavPoint,TcpData>(np,tcp);
}



std::pair<NavPoint,TcpData> Plan::makeBGS(const std::string& name,  Position p, double t, double a) {
	NavPoint np = NavPoint(p,t).makeName(name);
	TcpData tcp = TcpData().setBGS(a);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEGS( Position p, double t) {
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData().setEGS();
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEGSBGS(const std::string& name, Position p, double t, double a) {
	NavPoint np = NavPoint(p,t).makeName(name);
	TcpData tcp = TcpData().setEGSBGS(a);
	return  std::pair<NavPoint,TcpData>(np,tcp);
}


std::pair<NavPoint,TcpData> Plan::makeBVS(const std::string& name,  Position p, double t, double a) {
	NavPoint np = NavPoint(p,t).makeName(name);
	TcpData tcp = TcpData().setBVS(a);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEVS( Position p, double t) {
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData().setEVS();
	return  std::pair<NavPoint,TcpData>(np,tcp);

}

std::pair<NavPoint,TcpData> Plan::makeEVSBVS( Position p, double t, double a) {
	NavPoint np = NavPoint(p,t);
	TcpData tcp = TcpData().setEVSBVS(a);
	return  std::pair<NavPoint,TcpData>(np,tcp);

}


void Plan::fix() {
	if (!isWellFormed()) {
		fpln(" Plan.fix has not been ported yet -- used only in Watch!");
	}
}


// experimental
NavPoint Plan::closestPointHoriz(int seg, const Position& p) const {
	if (seg < 0 || seg >= size()-1) {
		addError("closestPositionHoriz: invalid index");
		return NavPoint::INVALID();
	}
	double d = 0;
	double t1 = time(seg);
	double dt = time(seg+1)-t1;
	NavPoint np = points[seg];
	NavPoint np2 = points[seg+1];
	if (np.distanceH(np2) < 0.001 && np.distanceV(np2) < 0.001) { // almost same point
		return np;
	}
	NavPoint ret;
	// vertical case is special
	if (pathDistance(seg) <= 0.0 || (Util::almost_equals(initialVelocity(seg).gs(), 0.0) && Util::almost_equals(initialVelocity(seg+1).gs(), 0.0))) {
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
		Position center = turnCenter(ixBOT);
		double endD = pathDistance(seg);
		double d2 = ProjectedKinematics::closestDistOnTurn(np.position(), initialVelocity(seg), std::abs(signedRadius(ixBOT)), turnDir(ixBOT), p, endD);
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
		ret = NavPoint(position(t1+segDt), t1+ segDt);
	} else {
		Vect3 cp = VectFuns::closestPointOnSegment(points[seg].vect3(), points[seg+1].vect3(), p.vect3());
		d = points[seg].vect3().distanceH(cp);
		double segDt = timeFromDistanceWithinSeg(seg, d);
		ret = NavPoint(position(t1+segDt), t1+ segDt);
	}
	return ret;
}


NavPoint Plan::closestPoint(const Position& p) const {
	return closestPoint(0,points.size()-1, p, true, 0.0);
}

NavPoint Plan::closestPointHoriz(const Position& p) const {
	return closestPoint(0,points.size()-1, p, false, Units::from("ft", 100));
}

NavPoint Plan::closestPoint(int start, int end, const Position& p, bool horizOnly, double maxHdist) const {
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


std::pair<Vect3,double> Plan::closestPoint3D(int seg, const Vect3& v0) const {
	if (seg < 0 || seg >= size() - 1) {
		addError("closestPoint3D: invalid index");
		return std::pair<Vect3,double>(Vect3::INVALID(), NaN);
	}
	Position p1 = getPos(seg);
	Position p2 = getPos(seg+1);
	Vect3 v1 = p1.vect3();
	Vect3 v2 = p2.vect3();
	if (isLatLon()) {
		v1 = GreatCircle::spherical2xyz(p1.lla());
		v2 = GreatCircle::spherical2xyz(p2.lla());
	}

	std::pair<Vect3,double> closest3 = VectFuns::closestPointOnSegment3_extended(v1, v2, v0);
	return closest3;
}

/**
 * Return the closest geometric point on a plan to a reference point, as measured as a Euclidean 3D norm from each linear segment.
 * (Kinematic turns are not accounted for in this distance calculation.)
 * @param seg
 * @param p reference point
 * @return
 */
NavPoint Plan::closestPoint3D(const Position& p) const {
	double mindist = DBL_MAX;
	double ratio = 0.0;
	int minseg = -1;
	Vect3 closest = Vect3::INVALID();
	Vect3 p3 = p.vect3();
	if (isLatLon()) {
		p3 = GreatCircle::spherical2xyz(p.lla());
	}

	for (int i = 0; i < size()-1; i++) {
		std::pair<Vect3,double> pvd = closestPoint3D(i, p3);
		double d = pvd.first.Sub(p3).norm();
		if (d < mindist) {
			mindist = d;
			closest = pvd.first;
			minseg = i;
			ratio = pvd.second;
		}
	}
	Position pos = Position(closest);
	if (isLatLon()) {
		pos = Position(GreatCircle::xyz2spherical(closest));
	}
	double dist = pathDistance(minseg)*ratio;
	double time = timeFromDistance(minseg, dist);
	double gs0 = gsOut(minseg);
	double gs1 = gsFinal(minseg);
	if (Util::within_epsilon(gs0, 0.0, 0.1) && Util::within_epsilon(gs1, 0.0, 0.1)) {
		double height = pos.alt();
		std::pair<double,double> times = timeFromHeight(minseg, height);
		if (times.first >= 0.0) {
			time = times.first;
		}
	}
	NavPoint np = NavPoint(position(time),time);

	return np;
}


bool Plan::inAccel(double t) const {
	return  inTrkChange(t) || inGsChange(t) || inVsChange(t);
}

bool Plan::inAccelZone(int ix) const {
	return inAccel(time(ix));
}



Plan Plan::planFromState(const std::string& id, const Position& pos, const Velocity& v, double startTime, double endTime) {
	Plan p(id);
	if (endTime <= startTime) {
		return p;
	}
	NavPoint np = NavPoint(pos, startTime);
	p.addNavPoint(np);
	p.addNavPoint(np.linear(v, endTime-startTime));
	return p;
}

Plan Plan::copy() const {
	Plan lpc = Plan(label,note);
	for (int j = 0; j < size(); j++) {
		lpc.add(get(j));
	}
	return lpc;
}

Plan Plan::cut(int firstIx, int lastIx) const {
	Plan lpc = Plan(label,note);
	for (int i = firstIx; i <= lastIx; i++) {
		std::pair<NavPoint,TcpData> np = get(i);
		lpc.add(np);
	}
	return lpc;
}

double Plan::getMIN_GS_DELTA_GEN() const {
	return MIN_GS_DELTA_GEN;
}

void Plan::setMIN_GS_DELTA_GEN(double minGsDeltaGen) {
	MIN_GS_DELTA_GEN = minGsDeltaGen;
}

double Plan::getMIN_TRK_DELTA_GEN() const {
	return MIN_TRK_DELTA_GEN;
}

void Plan::setMIN_TRK_DELTA_GEN(double minTrkDeltaGen) {
	MIN_TRK_DELTA_GEN = minTrkDeltaGen;
}

double Plan::getMIN_VS_DELTA_GEN() const {
	return MIN_VS_DELTA_GEN;
}

void Plan::setMIN_VS_DELTA_GEN(double minVsDeltaGen) {
	MIN_VS_DELTA_GEN = minVsDeltaGen;
}

void Plan::setMinDeltaGen(double trkDelta, double gsDelta, double vsDelta) {
	MIN_TRK_DELTA_GEN = trkDelta;   // minimum track delta that will result in a BOT-EOT generation
	MIN_GS_DELTA_GEN = gsDelta;    // minimum GS delta that will result in a BGS-EGS generation
	MIN_VS_DELTA_GEN = vsDelta;  // minimum VS delta that will result in a BVS-EVS generation
}



void Plan::setMinDeltaGen_BackToDefaults() {
	MIN_TRK_DELTA_GEN = Units::from("deg", 1);   // minimum track delta that will result in a BOT-EOT generation
	MIN_GS_DELTA_GEN = Units::from("kn", 10);    // minimum GS delta that will result in a BGS-EGS generation
	MIN_VS_DELTA_GEN = Units::from("fpm", 200);  // minimum VS delta that will result in a BVS-EVS generation
}



int  Plan::mergeClosePoints(int i, double minDt) {
	int rtn = -1;
	if (i >= size() - 1) return -1;  // nothing to do
	if (minDt > 0) {
		double deltaTm = point(i+1).time() - point(i).time();
		//fpln(" $$$$$$ mergeClosePoints: i = "+Fm0(i)+" deltaTm = "+Fm12(deltaTm));
		if (deltaTm < minDt) {
			int ixDelete = i+1;
			if (i == 0) ixDelete = 1;
			else if (!isBeginTCP(i) && isBeginTCP(i+1)) ixDelete = i;
			// save attributes of "ixDelete"
			NavPoint npDelete2 = point(ixDelete);
			TcpData tcp_i = getTcpData(i);
			TcpData tcp_ip1 = getTcpData(i+1);
			remove(ixDelete);
			rtn = ixDelete;
			// check for a very small accel region that is being removed
			if (tcp_i.isBOT() && tcp_ip1.isEOT()) {
				tcp_i.clearBOT();
				tcp_ip1.clearEOT();
			}
			if (tcp_i.isBGS() && tcp_ip1.isEGS()) {
				tcp_i.clearBGS();
				tcp_ip1.clearEGS();
			}
			if (tcp_i.isBVS() && tcp_ip1.isEVS()) {
				tcp_i.clearBVS();
				tcp_ip1.clearEVS();
			}
			TcpData mergeTCPData = tcp_i.mergeTCPData(tcp_ip1);
			NavPoint newNpi = point(i);
			newNpi = newNpi.appendName(npDelete2.name());
			//fpln(" $$$$$$ mergeClosePoints: i = "+Fm0(i)+" ixDelete = "+Fm0(ixDelete)+" tcp_ip1 = "+tcp_ip1.toString()+" getTcpData(i) = "+getTcpData(i).toString());
			//fpln(" $$$$$$ mergeClosePoints: i = "+Fm0(i)+" ixDelete = "+Fm0(ixDelete)+" mergeTCPData = "+mergeTCPData.toString());
			set(i,newNpi,mergeTCPData);
		}
	}
	return rtn;
}

void  Plan::mergeClosePoints(double minDt) {
	for (int i = size()-2; i >= 0; i--) {
		mergeClosePoints(i,minDt);
	}
}


void  Plan::mergeClosePoints() {
	Plan::mergeClosePoints(minDt);
}

int Plan::mergeClosePointsByDist(int j, double minDist) {
	int rtn = j;
	double dist_j = point(j).distanceH(point(j-1));
	//fpln(" $$ mergeClosePointsByDist j = "+Fm0(j)+" dist_j = "+Units::str("m",dist_j,12));
	if (dist_j < minDist) {                // prevent hovers within a turn
		TcpData mergeTCPData = getTcpData(j).mergeTCPData(getTcpData(j-1));
		NavPoint newNpi = point(j).appendName(point(j-1).name());
		//fpln(" $$ mergeClosePointsByDist: j = "+Fm0(j)+" newNpi = "+newNpi.toString()+" mergeTCPData = "+mergeTCPData.toString());
		set(j, newNpi, mergeTCPData);
		//f.pln(" $$$$$$ mergeClosePointsByDist: toStringPoint(j) = "+toStringPoint(j));
		remove(j-1);
        rtn = j-1;
	}
	return rtn;
}



// will not remove first or last point
void Plan::removeRedundantPoints(int from, int to) {
	//	double velEpsilon = 1.0;
	int ixLast = Util::min(size() - 2, to);
	int ixFirst = Util::max(1, from);
	for (int i = ixLast; i >= ixFirst; i--) {
		removeIfRedundant(i);
	}
}

void Plan::removeRedundantPoints() {
	removeRedundantPoints(0,200000000);           // MAX_INT ??  MAXINTEGER ??
}

int Plan::removeIfRedundant(int ix,  bool trkF, bool gsF, bool vsF) {
	double minTrk = Units::from("deg",1.0);
	double minGs = Units::from("kn",5.0);
	double minVs = Units::from("fpm",100.0);
	bool repair = true;
	return removeIfRedundant(ix,trkF, gsF, vsF, minTrk, minGs, minVs, repair);
}

int Plan::removeIfRedundant(int ix,  bool trkF, bool gsF, bool vsF, double minTrk, double minGs, double minVs, bool repair) {
	//fpln(" $$$$$ removeIfRedundant: ENTER ix = "+Fm0(ix)+" "+bool2str(isTCP(ix))+" "+bool2str(isAltPreserve(ix))+" info ="+getInfo(ix));
	if (ix <= 0 || ix >= size()-1) return -1;   // should not remove first or last point
	if (isTCP(ix)) return -1;
	if (isAltPreserve(ix)) return -1;
	if (isMOT(ix)) return -1;           // should not remove AltPreserve point
	if (! larcfm::equals(getInfo(ix),"")) return -1;
	if (! larcfm::equals(getName(ix),"")) return -1;
	Velocity vin = finalVelocity(ix - 1);
	Velocity vout = initialVelocity(ix);
	double deltaTrk = Util::turnDelta(vin.trk(),vout.trk());
	double deltaGs = std::abs(vin.gs()-vout.gs());
	double deltaVs = std::abs(vin.vs()-vout.vs());
	//fpln(" $$$$$ removeIfRedundant: ix = "+Fm0(ix)+" deltaTrk = "+Units::str("deg",deltaTrk)+" deltaGs ="+Units::str("kn",deltaGs)+" deltaVs = "+Units::str("fpm",deltaVs));
	if (
			( ! trkF || deltaTrk <= minTrk) &&
			( ! gsF  || deltaGs <= minGs)   &&
			( ! vsF  || deltaVs <= minVs)     ) {
		if (repair) remove(ix);
		return ix;
	}
	return -1;
}



int Plan::removeIfRedundant(int ix) {
	bool trkF = true;
	bool gsF = true;
	bool vsF = false;
	return removeIfRedundant(ix,trkF, gsF, vsF);
}


int Plan::removeIfVsConstant(int ix) {
	bool trkF = true;
	bool gsF = false;
	bool vsF = true;
	return removeIfRedundant(ix,trkF, gsF, vsF);
}



/**
 * Remove records of deleted points and make all remaining points "original"
 * @param fp
 */
void Plan::cleanPlan() {
	mergeClosePoints(Plan::minDt);
	for (int i = 0; i < size(); i++) {
		TcpData tcp = getTcpData(i).setOriginal();    // get rid of AltPreserves
		set(i,point(i), tcp);
	}
}


  std::string Plan::toString() const {
    return toStringVelocity(6);
}


std::string Plan::toStringPoint(int i) const{
  NavPoint p = point(i);
  TcpData d = getTcpData(i);
	std::stringstream sb;
	sb << d.getTypeString();
	//if (d.isTrkTCP()) {
	//	sb << ", " << d.getTrkTypeString();
	//}
	if (d.isGsTCP()) {
	  //sb << ", " << d.getGsTypeString();
		if (d.isBGS()) {
			sb << " accGs = " << Fm4(Units::to("m/s^2", d.getGsAccel()));
		}
	}
	if (d.isVsTCP()) {
	  //sb << ", " << d.getVsTypeString();
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
	//sb << "]";
	//sb << " " << p.name() << " " << d.getInformation();
	return sb.str();
}

std::vector<std::string> Plan::toStringList(int i, int precision, bool tcp) const {
	std::vector<std::string> ret;// name is (0)
	ret.push_back(label);
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
		Velocity vv = Velocity::ZEROV();
		//vec = vv.toStringList(precision);
		ret.push_back(d.getTrkTypeString()); // tcp trk (string) (9)
		ret.push_back(d.getGsTypeString()); // tcp gs (string) (11)
		ret.push_back(d.getVsTypeString()); // tcp vs (string) (13)
		ret.push_back(FmPrecision(Units::to("NM", d.getRadiusSigned()), precision)); // radius (15)
		ret.push_back(FmPrecision(Units::to("m/s^2",d.getGsAccel()),precision)); // gs accel (12)
		ret.push_back(FmPrecision(Units::to("m/s^2",d.getVsAccel()),precision)); // vs accel (14)
		vec = d.turnCenter().toStringList(precision);
		ret.insert(ret.end(),vec.begin(),vec.end()); // turn Center (16-18)
		if (d.getInformation().size() > 0) {
			ret.push_back(d.getInformation()); // name (string) (20)
		} else {
			ret.push_back("-");
		}
		if (p.name().size() > 0) {
			ret.push_back(p.name()); // name (string) (20)
		} else {
			ret.push_back("-");
		}
	} else { // no tcp
		std::string fl = TcpData::fullName(p,d);
		if (fl.size() > 0) {
			ret.push_back(fl); // name (string) (5)
		} else {
			ret.push_back("-");
		}
	}
	return ret;
}

std::string Plan::toStringTrk() const {
	return toStringVelocity(1);
}

std::string Plan::toStringGs() const {
	return toStringVelocity(2);
}

std::string Plan::toStringVs() const {
	return toStringVelocity(3);
}

  std::string Plan::toStringVelocity(int velocityDisplay) const {
	std::ostringstream sb;
	int precision;
		if (isLatLon()) {
		  sb << "Geodesic Plan: ";
			precision = 6;
		} else {
		  sb << "Euclidean Plan: ";
			precision = 4;
		}
		if (velocityDisplay == 6) {
			precision = 4;
		}

		//sb << " Plan for aircraft: ";
	sb << label;
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
			//std::string motFlag = "";
			//if (! TcpData::motFlagInInfo && isMOT(j)) motFlag = ".<MOT>.";
			sb << padLeft(Fmi(j),3) << ": " << padRight(point(j).name()+" "+getInfo(j),22);
			sb << " ";
			sb << padLeft(FmPrecision(point(j).time(),2),12) << " " << padRight("("+point(j).position().toStringNP(precision)+")",39);
			TcpData tcp = getTcpData(j);
			//sb << " " << getTcpData(j).tcpTypeString();
			if (tcp.isBOT()) {
		    	if (tcp.isEOT()) sb << " EOTBOT";
		    	else sb << " BOT";
		    	sb << padRight("("+FmPrecision(Units::to("NM",tcp.getRadiusSigned()),3)+") ",8);
		    } else if (tcp.isEOT()) sb << " EOT       ";
			//fpln("$$ toStringVelocity: j = "+Fm0(j)+" tcp = "+tcp.toString());
			if (!TcpData::motFlagInInfo && tcp.isMOT()) {sb <<" MOT";}
		    if (tcp.isBGS()) {
		    	if (tcp.isEGS()) sb << " EGSBGS";
		    	else sb << " BGS";
		    	sb << padRight("("+FmPrecision(Units::to("m/s^2",gsAccel(j)),3)+") ",8);
		    } else if (tcp.isEGS()) sb << " EGS       ";
		    if (tcp.isBVS()) {
		    	if (tcp.isEVS()) sb << " EVSBVS";
		    	else sb << " BVS";
		    	sb << "("+FmPrecision(Units::to("m/s^2",vsAccel(j)),3)+") ";
		    } else if (tcp.isEVS()) sb << " EVS       ";
		    if (! tcp.isTCP()) sb << "            ";
		    //if (vsAccel(j) != 0) sb << "<vsAccel:"+FmPrecision(Units::to("m/s^2",vsAccel(j)),3)+">";
		    if (j < size()) {
				if (velocityDisplay == 1) {
					if (j > 0)
					  sb << padRight("TrkIn: " + Units::str("deg", trkIn(j), 4),15);
					else
					  sb << padRight("TrkIn: -------------",15);
					if (j < size() - 1)
					  sb << padRight("TrkOut: " + Units::str("deg", trkOut(j), 4),15);
					else
					  sb << padRight("TrkOut: -------------",15);
					if (!Util::almost_equals(signedRadius(j),0.0))
					  sb << padRight("R: "+Units::str("NM", signedRadius(j), 4),15);
				}
				if (velocityDisplay == 2) {
					if (j > 0)
					  sb << padRight("GSin: " + Units::str("kn", gsIn(j), 4),20);
					else
					  sb << padRight("GSin: -------------",20);
					if (j < size() - 1)
					  sb << padRight("GSout: " + Units::str("kn", gsOut(j), 4),20);
					else
					  sb << padRight("GSout: -------------",20);
					// sb << ", GSaccel: "+Units::str("m/s^2",point(j).gsAccel(),4);
				}
				if (velocityDisplay == 3) {
					//sb << ", VSaccel: "+Units::str("m/s^2",vsAccel(j),4));
					if (j > 0)
					  sb << padRight("VSin: " + Units::str("fpm", vsIn(j), 4),25);
					else
					  sb << padRight("VSin: -------------",25);
					if (j < size() - 1)
					  sb << padRight("vsOut: " + Units::str("fpm", vsOut(j), 4),25);
					else
					  sb << padRight("vSout: ---------------",25);
				}
				if (velocityDisplay == 4) {
					sb << "  pathDist " + Units::str("NM", pathDistance(0,j), 4);
				}
				if (velocityDisplay == 6) {
				  sb << toStringPoint(j);
				}
				//if (isAltPreserve(j)) sb << " *AltPreserve*";
			}
			//if (vertexRadius(j) != 0) {
		    	//sb << ("<radius:"+FmPrecision(Units::to("NM",vertexRadius(j)),3)+">");
			//if (isBOT(j) && turnCenter(j).isInvalid()) sb << (" center = ***NAN***");
			//}
			//if (gsAccel(j) != 0) sb << ("<gsAccel:"+FmPrecision(Units::to("m/s^2",gsAccel(j)),3)+">");
			//if (vsAccel(j) != 0) sb << ("<vsAccel:"+FmPrecision(Units::to("m/s^2",vsAccel(j)),3)+">");
			//sb << "\t\t info = "+getInfo(j);
			sb << std::endl;
			}
	}
	return sb.str();
}


	std::string Plan::toStringProfile() const {
		std::ostringstream sb;
		if (error.hasMessage()) {
			sb << std::endl;
			sb << "    error.message = ";
			sb << error.getMessageNoClear();
			sb << std::endl;
		} else {
			sb << std::endl;			
		}
		sb << "              INFO           Time(s)        Alt(ft)    TrkOut(deg)      GsOut(kn)     VsOut(fpm)           dt      pathDist(NM)      ";
		sb << std::endl;
		sb << "         --------------    ---------    -----------    -----------     ----------     ----------        -------    -----------";
		sb << std::endl;
		for (int j = 0; j < size()-1; j++) {
			sb << padLeft(Fmi(j),3);
			sb << ":";	
			std::string virtFlag = isVirtual(j) ? "*VIRTUAL*" : "";
			std::string motFlag = "";
			if (! TcpData::motFlagInInfo && isMOT(j)) motFlag = "<MOT>";
		    sb << padLeft(getInfo(j)+virtFlag+motFlag,20);
			sb << padLeft(Fm2(Units::to("s", time(j))), 12);
			sb << padLeft(Fm2(Units::to("ft", alt(j))), 15);
			sb << padLeft(Fm2(Units::to("deg", trkOut(j))), 15);
			sb << padLeft(Fm2(Units::to("kn", gsOut(j))), 15);
			sb << padLeft(Fm2(Units::to("fpm", vsOut(j))), 15);
			if (j < size()-1) sb << padLeft(Fm2(time(j+1)-time(j)),15);
			sb << padLeft(Fm3(Units::to("NM",pathDistance(j))),15);			
			if (isTCP(j)) sb << padLeft(getTcpData(j).toStringTcpType(),10);
			
			
			sb << std::endl;
		}
		if (size() > 0) {
			int lastIx = (size()-1);
			sb << padLeft(Fmi(lastIx),3);
			sb << ":";
			sb << padLeft(getInfo(lastIx),20);
			sb << padLeft(Fm2(Units::to("s", time(lastIx))), 12);
			sb << padLeft(Fm2(Units::to("ft", alt(lastIx))), 15);
			sb << padLeft("------         ------",75);

			sb << std::endl;
		}
		sb << padLeft("",96);
		sb << padLeft(Fm3(getLastTime()-getFirstTime()),15);			
		sb << padLeft(Fm3(Units::to("NM",pathDistance())),15);			

		return sb.str();
	}



//std::string Plan::getOutputHeader(bool tcpcolumns) const {
//	std::string s = "";
//	s += "Name, ";
//	if (isLatLon()) {
//		s += "Lat, Lon, Alt";
//	} else {
//		s += "SX SY SZ";
//	}
//	s += ", Time, ";
//	if (tcpcolumns) {
//		s += "type, trk, gs, vs, tcp_trk, accel_trk, tcp_gs, accel_gs, tcp_vs, accel_vs, radius, ";
//		if (isLatLon()) {
//			s += "src_lat, src_lon, src_alt, ";
//		} else {
//			s += "src_x, src_y, src_z, ";
//		}
//		s += "src_time, ";
//	}
//	s += "name";
//	return s;
//}


std::ostream& operator << (std::ostream& os, const Plan &f) {
	Plan n(f);
	os << n.toString();
	return os;
}


}

