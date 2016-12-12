/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "GeneralState.h"
#include "Position.h"
#include "Velocity.h"
#include "SimpleMovingPoly.h"
#include "SimplePoly.h"
#include "Units.h"
#include "EuclideanProjection.h"
#include <limits>
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include "Util.h" // NaN def

namespace larcfm {


GeneralState::GeneralState(){
	id = "";
	pointDefined = false;
	polyDefined = false;
	containment = false;
	state = General3DState::INVALID;
	t = 0.0;
}


GeneralState::GeneralState(const std::string& name, const Position& s, const Velocity& v, double time){
	id = name;
	si = s;
	vi = v;
	pointDefined = true;
	polyDefined = false;
	state = General3DState::INVALID;

	t = time;
	containment = false;
}

GeneralState::GeneralState(const std::string& name, const SimpleMovingPoly& p, double time) {
	id = name;
	pointDefined = false;
	polyDefined = true;
	mp = p;
	t = time;
	containment = false;
	state = General3DState::INVALID;
}

GeneralState::GeneralState(const std::string& name, const SimpleMovingPoly& p, double time, bool cont){
	id = name;
	pointDefined = false;
	polyDefined = true;
	mp = p;
	t = time;
	containment = cont;
	state = General3DState::INVALID;
}

GeneralState::GeneralState(const std::string& name, const SimplePoly& p, const Velocity& v, double time, bool cont){
	id = name;
	pointDefined = false;
	polyDefined = true;
	mp =  SimpleMovingPoly(p,v);
	t = time;
	containment = cont;
	state = General3DState::INVALID;
}


GeneralState::GeneralState(const GeneralState& g){
	id = g.id;
	si = g.si;
	vi = g.vi;
	pointDefined = g.pointDefined;
	polyDefined = g.polyDefined;

	if (polyDefined) {
		mp = g.mp.copy();
	}
	t = g.t;
	containment = g.containment;
	state = g.state;
}

const GeneralState GeneralState::INVALID = GeneralState();

GeneralState GeneralState::make(const General3DState& g, const std::string& name, double time){
	if (g.hasPointMass()) {
		return  GeneralState(name,  Position(g.getVect3()), g.getVelocity(), time);
	} else if (g.hasPolygon()) {
		return  GeneralState(name, SimpleMovingPoly::make(g.getPolygon()), time);
	} else {
		return INVALID;
	}
}

GeneralState GeneralState::make(const General3DState& g, const EuclideanProjection& proj, const std::string& name, double time) {
	if (g.hasPointMass()) {
		std::pair<Position,Velocity> pr = proj.inverse(g.getVect3(), g.getVelocity(), true);
		return  GeneralState(name, pr.first, pr.second, time);
	} else if (g.hasPolygon()) {
		return  GeneralState(name, SimpleMovingPoly::make(g.getPolygon(),proj), time);
	} else {
		return INVALID;
	}
}


//double GeneralState::distanceH(const Position& p) const{
//	if (hasPointMass()) {
//		return si.distanceH(p);
//	} else if (hasPolygon()) {
//		return mp.position(0).distanceFromEdge(p);
//	} else {
//		return -1.0;
//	}
//}
//
//double GeneralState::distanceV(const Position& p) const{
//	if (hasPointMass()) {
//		return si.distanceV(p);
//	} else if (hasPolygon()) {
//		return mp.position(0).distanceV(p);
//	} else {
//		return -1.0;
//	}
//}

Position GeneralState::representativePosition() const{
	if (hasPointMass()) {
		return si;
	} else if (hasPolygon()) {
		return mp.position(0).averagePoint();
	} else {
		return Position::INVALID();
	}
}

Velocity GeneralState::representativeVelocity() const{
	if (hasPointMass()) {
		return vi;
	} else if (hasPolygon()) {
		return mp.averageVelocity();
	} else {
		return Velocity::INVALID();
	}
}
std::string GeneralState::getName() const{
	return id;
}

GeneralState GeneralState::copy() const{
	GeneralState gs;
	gs.pointDefined = pointDefined;
	gs.polyDefined = polyDefined;
	gs.si = si;
	gs.vi = vi;
	gs.mp = mp;
	gs.id = id;
	gs.t = t;
	return gs;
}

bool GeneralState::hasPointMass() const{
	return pointDefined;
}

bool GeneralState::hasPolygon() const{
	return polyDefined;
}

Position GeneralState::getPosition() const{
	return si;
}

Velocity GeneralState::getVelocity() const{
	return vi;
}

SimpleMovingPoly GeneralState::getPolygon() const{
	return mp;
}

double GeneralState::getTime() const{
	return t;
}

bool GeneralState::isContainment() const{
	return hasPolygon() && containment;
}

General3DState GeneralState::get3DState(const EuclideanProjection& proj) const{
	if (pointDefined) {
		if (si.isLatLon()) {
			state = General3DState(proj.project(si), proj.projectVelocity(si, vi));;
			return state;
		} else {
			state =  General3DState(si.point(), vi);;
			return state;
		}
	} else if (polyDefined) {
		state =  General3DState(mp.getMovingPolygon(proj), containment);
		return state;
	} else {
		return General3DState::INVALID;
	}
}


General3DState GeneralState::getLast3DState() const{
	return state;
}

General3DState GeneralState::pred(double tm) const{
	double dt = tm-t;
	return state.linear(dt);
}

GeneralState GeneralState::linearPred(double tm) const{
	double dt = tm-t;
	if (hasPointMass()) {
		Position s2 = si.linear(vi, dt);
		return  GeneralState(id, s2, si.finalVelocity(s2, dt), t);
	} else if (hasPolygon()) {
		return  GeneralState(id, mp.linear(dt), t, containment);
	} else {
		return GeneralState::INVALID;
	}
}

GeneralState GeneralState::linear(double dt) const{
	if (hasPointMass()) {
		Position s2 = si.linear(vi, dt);
		return  GeneralState(id, s2, si.finalVelocity(s2, dt), t+dt);
	} else if (hasPolygon()) {
		return  GeneralState(id, mp.linear(dt), t+dt, containment);
	} else {
		return GeneralState::INVALID;
	}
}

bool GeneralState::checkLatLon(bool ll) const{
	if (!pointDefined && !polyDefined) {
		return true;
	}
	return ll == (polyDefined ? mp.isLatLon() : si.isLatLon());
}

bool GeneralState::isLatLon() const{
	return checkLatLon(true);
}

bool GeneralState::isInvalid() const{
	return !pointDefined && !polyDefined;
}

std::string GeneralState::toString() const{
	std::string s = "State="+id+" t="+Fm2(t)+":";
	if (hasPointMass()) {
		s = s + "s="+si.toString()+" v="+vi.toString();
	} else if (hasPolygon()) {
		s = s + mp.toString();
	} else {
		s = s + "INVALID";
	}
	return s;
}

//std::string GeneralState::toOutput(int precision) const {
//		if (hasPointMass()) {
//			return id+","+si.toStringNP(precision)+","+vi.toStringNP(precision)+","+t+",-";
//		} else if (hasPolygon()) {
//			SimplePoly poly = mp.poly;
//			std::string s = "";
//			for (int j = 0; j < poly.size(); j++) {
//				std::vector<std::string> ret;
//				ret.push_back(id);  // name is (0)
//				ret.addAll(poly.getVertex(j).toStringList(precision)); //vertex 1-3
//				ret.addAll(mp.vlist[j].toStringList()); // vel 5-7
//				ret.push_back(f.FmPrecision(t,precision)); // time 8
//				ret.push_back(f.FmPrecision(Units.to("ft", poly.getTop()),precision));
//				s += f.list2str(ret,", ")+"\n";
//			}
//			return s;
//		} else {
//			return "";
//		}
//	}


}
