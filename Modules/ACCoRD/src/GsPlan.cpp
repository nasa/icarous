/*
 * Copyright (c) 2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "GsPlan.h"
#include "Plan.h"
#include "Position.h"
#include "NavPoint.h"
#include "format.h"
#include "string_util.h"
#include "TrajGen.h"
#include "PlanUtil.h"
#include "Units.h"
#include "Velocity.h"
#include <string>
#include <vector>

namespace larcfm {


GsPlan::GsPlan() {

}

GsPlan::GsPlan(double stTime) {
	id = "";
	starttime = stTime;
}


GsPlan::GsPlan(const std::string& s) {
	id = s;
	starttime = 0.0;
}

GsPlan::GsPlan(const Plan& lpc, int start, int end) {
	if (start < 0) start = 0;
	if (end >= lpc.size()) end = lpc.size()-1;
	id = lpc.getName();
	for (int i = start; i <= end ; i++) {
		NavPoint np = lpc.point(i);
		double gsOut_i = lpc.gsOut(i);
		//f.pln(" $$$ GsPlan: i = "+i+"  "+np.tcpTypeString()+" gsOut_i() = "+Units.str("kn",gsOut_i));
		add(np.position(),np.label(),gsOut_i);
	}
	starttime = lpc.getFirstTime();
}


GsPlan::GsPlan(const Plan& lpc) {
	int start = 0;
	int end = lpc.size()-1;
	id = lpc.getName();
	for (int i = start; i <= end ; i++) {
		NavPoint np = lpc.point(i);
		double gsOut_i = lpc.gsOut(i);
		//f.pln(" $$$ GsPlan: i = "+i+"  "+np.tcpTypeString()+" gsOut_i() = "+Units.str("kn",gsOut_i));
		add(np.position(),np.label(),gsOut_i);
	}
	starttime = lpc.getFirstTime();
}


GsPlan::GsPlan(const GsPlan& gsp) {
	rt = Route(gsp.rt);
	id = gsp.id;
	gsOuts  = gsp.gsOuts;
	starttime = gsp.starttime;
}

GsPlan::GsPlan(const Route& r, const std::string& name, double start, double gsAll) {
	rt = r;
	id = name;
	for (int j = 0; j < rt.size(); j++) {
		gsOuts.push_back(gsAll);
	}
	starttime = start;
}

GsPlan GsPlan::makeGsPlanConstant(const GsPlan& gsp, double gsNew) {
	GsPlan gspNew = GsPlan(gsp);
	for (int j = 0; j < gsp.size(); j++) {
		gspNew.setGs(j,gsNew);
	}
	return gspNew;
}


GsPlan GsPlan::mkGsPlanBankAngle(const Plan& lpc, double bankAngle) {
	Route rt = Route::mkRouteBankAngle(lpc,bankAngle);
	GsPlan gsp = GsPlan(rt, lpc.getName(), lpc.getFirstTime(), 0.0);
	fpln(Fm0(lpc.size())+" "+Fm0(gsp.size()));
	for (int j = 0; j < rt.size(); j++) {
		gsp.gsOuts[j] = lpc.gsOut(j);
	}
	return gsp;
}



int GsPlan::size() const {
	return rt.size();
}


std::string GsPlan::getName() const {
	return id;
}

void GsPlan::setName(const std::string& s) {
	id = s;
}

double GsPlan::gs(int i) const {
	return gsOuts[i];
}

Route GsPlan::route() const {
	return rt;
}

void GsPlan::add(const Position& pos, const std::string& label, double gsOut, double rad) {
	rt.add(pos, label, rad);
	gsOuts.push_back(gsOut);
}

void GsPlan::set(int ix, const Position& pos, const std::string& label, double gsOut) {
	//f.pln(" $$###>>>>> GsPlan.set: ix = "+ix+" ps = "+pos+" "+label+" gsin = "+Units.str("kn",gsOut));
	double radius = 0.0;
	rt.set(ix, pos, label, radius);
	gsOuts[ix] = gsOut;
}

void GsPlan::add(const Position& pos, std::string label, double gsOut) {
	double radius = 0.0;
	rt.add(pos, label, radius);
	gsOuts.push_back(gsOut);
}

void GsPlan::add(const Position& pos, const std::string& label) {
	rt.add(pos, label, 0.0);
	if (gsOuts.size() > 0) {
		gsOuts.push_back(gsOuts[gsOuts.size()-1]);
	} else {
		gsOuts.push_back(-1.0);
	}
}

Position GsPlan::position(int i) const {
	return rt.position(i);
}

Position GsPlan::last() const{
	return rt.position(size()-1);
}

std::string GsPlan::name(int i)const {
	return rt.name(i);
}

double GsPlan::radius(int i)const {
	return rt.radius(i);
}


void GsPlan::setRadius(int i, double rad) {
	if (i < 0 || i >= size()) {
		fpln(" $$$ ERROR: Route.setName: index out of range");
	} else {
		rt.setRadius(i,rad);
	}
}

std::vector<double> GsPlan::getGsInits() const {
	return gsOuts;
}

void GsPlan::add(const GsPlan& p, int ix) {
	rt.add(p.position(ix),p.name(ix),p.radius(ix));
	gsOuts.push_back(p.gsOuts[ix]);
	//f.pln(" $$ GsPlan add "+p.names.get(ix));
}

void GsPlan::addAll(const GsPlan& p) {
	rt.addAll(p.rt);
	gsOuts.insert(gsOuts.end(), p.gsOuts.begin(), p.gsOuts.end());
}

GsPlan GsPlan::append(const GsPlan& p2) {
	GsPlan rtn = GsPlan(*this);
	rtn.addAll(p2);
	//f.pln(" $$ GsPlan:  append: rtn.size() = "+rtn.size());
	return rtn;
}

void GsPlan::remove(int i) {
	rt.remove(i);
	gsOuts.erase(gsOuts.begin()+i);
}


void GsPlan::setGs(int i, double gsin) {
     gsOuts[i] = gsin;
}

double GsPlan::startTime() const {
	return starttime;
}

void GsPlan::setStartTime(double startTime) {
	starttime = startTime;
}

int GsPlan::findName(const std::string& nm) const {
	return rt.findName(nm);
}

double GsPlan::pathDistance(int i, int j, bool linear) const {
	return rt.pathDistance(i, j, linear);
}

double GsPlan::ETA(bool linear) const {
	double tot = starttime;
	double mygs = 1.0;
	for (int i = 0; i < size()-1; i++) {
		if (gs(i) > 0.0) {
			mygs = gs(i);
		}
		tot += pathDistance(i, i+1, linear) / mygs;
	}
	return tot;
}

Plan GsPlan::linearPlan() const {
	Plan lpc;
	if (rt.size() < 1) return lpc;
	double lastT = starttime;
	Position lastNp = position(0);
	//f.pln(" $$$ GsPlan.linearPlan: lastNp = "+lastNp);
	lpc.addNavPoint(NavPoint(lastNp,starttime).makeLabel(name(0)));
	for (int i = 1; i < rt.size(); i++) {
		Position np = position(i);
		double pathDist = np.distanceH(lastNp);
		double gs_i = gsOuts[i-1];
		double t = lastT + pathDist/gs_i;
		//f.pln(" $$$ linearPlan: gs = "+Units.str("kn",gs_i)+" t = "+t);
		NavPoint nvp = NavPoint(np,t).makeLabel(name(i));
		//f.pln(" $$$ GsPlan.linearPlan: i = "+i+" nvp = "+nvp);
		TcpData tcp = TcpData::makeSource(nvp).setRadiusSigned(radius(i));
		//NavPt nvPair= new NavPt(nvp,tcp);
		if (name(i)==rt.virtualName) tcp = tcp.setVirtual();
		//f.pln(" $$$$$ GsPlan.linearPlan: nvp = "+nvp.toStringFull());
		lpc.add(nvp,tcp);
		lastT = t;
		lastNp = np;
	}
	return lpc;
}

int GsPlan::closestSegment(const Position& pos) const {
	Plan pl = linearPlan();
	double t = pl.closestPoint(pos).time();
	return pl.getSegment(t);
}


Position GsPlan::positionFromDistance(double dist, double defaultBank, bool linear) const {
	Plan p = linearPlan();
	double starttime = p.getFirstTime();
	if (!linear) {
		p = TrajGen::generateTurnTCPs(p, defaultBank);
	}
	return PlanUtil::advanceDistance(p, starttime, dist, false).first;
}

Velocity GsPlan::velocityFromDistance(double dist, double defaultBank, bool linear) const {
	Plan p = linearPlan();
	if (!linear) {
		p = TrajGen::generateTurnTCPs(p, defaultBank);
	}
	return p.velocityByDistance(dist);
}


bool GsPlan::equals(const GsPlan& fp) const {
	if (starttime != fp.starttime) return false;
	for (int i = 0; i < fp.size(); i++) {                // Unchanged
		if (position(i) != fp.position(i)) return false;
		if (! larcfm::equals(name(i),fp.name(i))) return false;
		if (gs(i) != fp.gs(i)) return false;
	}
	return true;
}

bool GsPlan::almostEquals(const GsPlan& p) const {
	bool rtn = true;
	for (int i = 0; i < size(); i++) {                // Unchanged
		if (!position(i).almostEquals(p.position(i))) {
			rtn = false;
			fpln("almostEquals: point i = "+Fm0(i)+" does not match: "+position(i).toString()+"  !=   "+p.position(i).toString());
		}

		if (! larcfm::equals(name(i),p.name(i))) {
			fpln("almostEquals: name i = "+Fm0(i)+" does not match: "+name(i)+"  !=   "+p.name(i));
			rtn = false;
		}
	}
	return rtn;
}


std::string GsPlan::toString() const {
	std::string rtn = "GsPlan size = "+Fm0(rt.size())+"\n";
	for (int i = 0; i < rt.size(); i++) {
		rtn += " "+Fm0(i)+" "+position(i).toString()+" "+name(i);
		if (radius(i) != 0.0) rtn += " radius ="+Fm3(radius(i));
		rtn += " gsInit = "+Units::str("kn",gsOuts[i]);
		rtn += "\n";
	}
	return rtn;
}




}
