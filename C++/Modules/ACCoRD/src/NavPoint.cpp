/*
 * NavPoint.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "NavPoint.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Constants.h"
#include "format.h"
#include "Util.h" // NaN definition
#include "string_util.h"
#include <stdexcept>
#include <algorithm>

namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;

static const bool NavPoint_DEBUG = false;

const NavPoint& NavPoint::ZERO_LL() {
	static NavPoint* v = new NavPoint(Position::ZERO_LL(), 0.0);
	return *v;
}
const NavPoint& NavPoint::ZERO_XYZ() {
	static NavPoint* v = new NavPoint(Position::ZERO_XYZ(), 0.0);
	return *v;
}
const NavPoint& NavPoint::INVALID() {
	static NavPoint* v = new NavPoint(Position::INVALID(), NaN);
	return *v;
}


NavPoint::NavPoint() :
    				p(Position::ZERO_LL()),
					t(0.0),
					label_s("")
					//ty(Orig),
					//tcp_trk(NONE),
					//tcp_gs(NONEg),
					//tcp_vs(NONEv),
					//sourcePosition_p(p),
					//sourceTime_d(t),
					//accel_d(0.0),
					//sgnRadius(0.0),		      // signed radius
					//accel_gs(0.0),            // signed gs-acceleration value
					//accel_vs(0.0),            // signed vs-acceleration value
					//velocityInit_v(Velocity::INVALIDV()),
                    //linearIndex_i(-1)
{ }

NavPoint::NavPoint(const Position& pp, double tt) :
    				p(pp),
					t(tt),
					label_s("")
					//ty(Orig),
					//tcp_trk(NONE),
					//tcp_gs(NONEg),
					//tcp_vs(NONEv),
					//sourcePosition_p(p),
					//sourceTime_d(t),
					//sgnRadius(0.0),           // signed radius
					//accel_gs(0.0),            // signed gs-acceleration value
					//accel_vs(0.0),            // signed vs-acceleration value
					//velocityInit_v(Velocity::INVALIDV()),
					//linearIndex_i(-1)
{ }

NavPoint::NavPoint(const Position& pp, double tt, const string& llabel) :
    				p(pp),
					t(tt),
					label_s(llabel)
					//ty(Orig),
					//tcp_trk(NONE),
					//tcp_gs(NONEg),
					//tcp_vs(NONEv),
					//sourcePosition_p(p),
					//sourceTime_d(t),
					//sgnRadius(0.0),           // signed radius
					//accel_gs(0.0),            // signed gs-acceleration value
					//accel_vs(0.0),            // signed vs-acceleration value
					//velocityInit_v(Velocity::INVALIDV()),
					//linearIndex_i(-1)
{ }


NavPoint NavPoint::makeLatLonAlt(double lat, double lon, double alt, double t) {
	return NavPoint(Position::makeLatLonAlt(lat,lon,alt), t);
}

NavPoint NavPoint::makeXYZ(double x, double y, double z, double t) {
	return NavPoint(Position::makeXYZ(x, y, z), t);
}

bool NavPoint::isInvalid() const {
	return p.isInvalid() || t != t;
}




bool NavPoint::almostEquals(const NavPoint& v) const {
	return Constants::almost_equals_time(t,v.t)
	&& p.almostEquals(v.p);
}

bool NavPoint::almostEqualsPosition(const NavPoint& v) const {
	return p.almostEquals(v.p);
}

bool NavPoint::almostEqualsPosition(const NavPoint& v, double epsilon_horiz, double epsilon_vert) const {
	return p.almostEquals(v.p, epsilon_horiz, epsilon_vert);
}


bool NavPoint::operator == (const NavPoint& v) const {  // strict equality
	return p == v.p && t==v.t && label_s == v.label_s;
}

bool NavPoint::equals(const NavPoint& v) const {  // strict equality
	return *this == v;
}

bool NavPoint::operator != (const NavPoint& v) const {  // strict disequality
	return !(*this == v); //p!=v.p || t!=v.t || ty != v.ty; //  || mut != v.mut;
}

Vect2 NavPoint::vect2() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in vect2()");
	}
	return p.vect2();
}

Point NavPoint::point() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in vect3()");
	}
	return p.point();
}

const LatLonAlt& NavPoint::lla() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lla()");
	}
	return p.lla();
}

const Position& NavPoint::position() const {
	return p;
}

double NavPoint::x() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in x()");
	}
	return p.x();
}

double NavPoint::y() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in y()");
	}
	return p.y();
}

double NavPoint::z() const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in z()");
	}
	return p.z();
}

double NavPoint::lat() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lat()");
	}
	return p.lat();
}

double NavPoint::lon() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in lon()");
	}
	return p.lon();
}

double NavPoint::alt() const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in alt()");
	}
	return p.alt();
}


double NavPoint::latitude() const {
	return p.latitude();
}

double NavPoint::longitude() const {
	return p.longitude();
}

double NavPoint::altitude() const {
	return p.altitude();
}


double NavPoint::xCoordinate() const {
	return p.xCoordinate();
}

double NavPoint::yCoordinate() const {
	return p.yCoordinate();
}

double NavPoint::zCoordinate() const {
	return p.zCoordinate();
}

double NavPoint::time() const {
	return t;
}

const std::string& NavPoint::label() const {
	return label_s;
}

bool NavPoint::isNameSet() const {
	return label_s != "";
}

bool NavPoint::isLatLon() const {
	return p.isLatLon();
}


const NavPoint NavPoint::copy(const Position& p) const {
	return NavPoint(p, this->t, this->label_s); //, this->tcp_trk, this->tcp_gs, this->tcp_vs,
}

const NavPoint NavPoint::mkLat(double lat) const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeLat()");
	}
	return copy(p.mkLat(lat));
}

const NavPoint NavPoint::mkX(double x) const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeX()");
	}
	return copy(p.mkX(x));
}

const NavPoint NavPoint::mkLon(double lon) const {
	if (NavPoint_DEBUG && ! p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeLon()");
	}
	return copy(p.mkLon(lon));
}

const NavPoint NavPoint::mkY(double y) const {
	if (NavPoint_DEBUG && p.isLatLon()) {
		throw runtime_error("Incorrect geometry in makeY()");
	}
	return copy(p.mkY(y));
}

const NavPoint NavPoint::mkAlt(double alt) const {
	return copy(p.mkAlt(alt));
}

const NavPoint NavPoint::mkZ(double z) const {
	return copy(p.mkZ(z));
}


const NavPoint NavPoint::makeTime(double time) const {
	return NavPoint(this->p, time, this->label_s);
}

const NavPoint NavPoint::makeLabel(const std::string& label) const {
	return NavPoint(this->p, this->t, label);
}

const NavPoint NavPoint::appendLabel(const std::string& label) const {
	return NavPoint(this->p, this->t, this->label_s+label);
}


const NavPoint NavPoint::makeMovedFrom(const NavPoint& o) const {
	return NavPoint(this->p, this->t, o.label_s);
}

const NavPoint NavPoint::makePosition(const Position& p) const {
	return NavPoint(p, this->t, this->label_s);
}

Velocity NavPoint::initialVelocity(const NavPoint& s1, const NavPoint& s2) {
	if (NavPoint_DEBUG && s1.isLatLon() != s2.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s2.time() - s1.time();
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_initial(s1.p.lla(), s2.p.lla(), dt);
		} else {
			return Velocity::make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
		}
	} else {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_initial(s2.p.lla(), s1.p.lla(), -dt);
		} else {
			return Velocity::make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
		}
	}
}

  
Velocity NavPoint::initialVelocity(const NavPoint& s) const {
  return initialVelocity(*this, s);
}

Velocity NavPoint::finalVelocity(const NavPoint& s1, const NavPoint& s2) {
	if (NavPoint_DEBUG && s1.isLatLon() != s2.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s2.time() - s1.time();
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_final(s1.p.lla(), s2.p.lla(), dt);
		} else {
			return Velocity::make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
		}
	} else {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_final(s2.p.lla(), s1.p.lla(), -dt);
		} else {
			return Velocity::make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
		}
	}
}


double NavPoint::verticalSpeed(const NavPoint& s) const {
	return (s.alt()-this->alt())/(s.time()-this->time());
}


Velocity NavPoint::averageVelocity(const NavPoint& s1, const NavPoint& s2) {
	if (NavPoint_DEBUG && s1.isLatLon() != s2.isLatLon()) {
		throw runtime_error("Incompatible geometries in velocity()");
	}
	double dt = s2.time() - s1.time();
	if (dt == 0) {
		return Velocity::ZEROV();
	} else if (dt > 0) {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_average(s1.p.lla(), s2.p.lla(), dt);
		} else {
			return Velocity::make((s2.p.point().Sub(s1.p.point())).Scal(1.0/dt));
		}
	} else {
		if (s2.isLatLon()) {
			return GreatCircle::velocity_average(s2.p.lla(), s1.p.lla(), -dt);
		} else {
			return Velocity::make((s1.p.point().Sub(s2.p.point())).Scal(1.0/-dt));
		}
	}
}


const NavPoint NavPoint::linear(const Velocity& v, const double time) const {
	Position newPos = p.linear(v,time);
	return NavPoint(newPos,t+time);
}


const NavPoint NavPoint::linearEst(const Velocity& vo, double tm) const {
	return NavPoint(p.linearEst(vo,tm),t+tm);
}


const NavPoint NavPoint::interpolate(const NavPoint& np, const double time) const {
	Velocity v = initialVelocity(np);
	Position newPos = p.linear(v,time-t);
	return NavPoint(newPos,time);
}


double NavPoint::distanceH(const NavPoint& np2) const {
	return np2.position().distanceH(position());
}

/** Vertical distance */
double NavPoint::distanceV(const NavPoint& np2) const {
	return np2.position().distanceV(position());
}



std::string NavPoint::toStringShort() const {
	return "hello";
	//return toString(Constants::get_output_precision());
}

std::string NavPoint::toStringShort(int precision) const {
	return p.toStringNP(precision) + ", " + FmPrecision(t,precision);
}

std::string NavPoint::toString() const {
	return toString(Constants::get_output_precision());
}

std::string NavPoint::toString(int precision) const {
	return p.toStringNP(precision) + ", " + FmPrecision(t,precision) + " " + label_s;
}



} // namespace
