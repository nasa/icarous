/*
 * Velocity.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://research.nianet.org/fm-at-nia/ACCoRD
 *
 * NOTES: 
 * Track is True North/clockwise
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Velocity.h"
#include "Units.h"
#include "Util.h" // NaN def
#include "format.h"
#include "Constants.h"
#include "string_util.h"
#include <limits>
#include <vector>

namespace larcfm {

Velocity::Velocity(const double vx, const double vy, const double vz):
    				Vect3(vx,vy,vz) {}

Velocity::Velocity(): Vect3(0.0,0.0,0.0) {}

Velocity::Velocity(const Vect3& v3) : Vect3(v3.x,v3.y,v3.z) {};

Velocity Velocity::make(const Vect3& v) {
	return Velocity(v.x,v.y,v.z);
}

Velocity Velocity::make(const Vect2& v) {
	return Velocity(v.x,v.y,0.0);
}

Velocity Velocity::mkVxyz(const double vx, const double vy, const double vz) {
	return Velocity(vx,vy,vz);
}


Velocity Velocity::makeVxyz(const double vx, const double vy, const double vz) {
	return Velocity(Units::from("kn",vx),Units::from("kn",vy),Units::from("fpm",vz));
}


Velocity Velocity::makeVxyz(const double vx, const double vy, const std::string& uvxy,
		const double vz, const std::string& uvz) {
	return Velocity(Units::from(uvxy,vx),Units::from(uvxy, vy), Units::from(uvz,vz));
}

Velocity Velocity::mkTrkGsVs(const double trk, const double gs, const double vs){
	return Velocity(trkgs2vx(trk,gs),trkgs2vy(trk,gs),vs);
}

Velocity Velocity::makeTrkGsVs(const double trk, const double gs, const double vs) {
	return Velocity::mkTrkGsVs(Units::from("deg",trk), Units::from("kn",gs),Units::from("fpm",vs));
}


Velocity Velocity::makeTrkGsVs(const double trk, const std::string& utrk,
		const double gs, const std::string& ugs,
		const double vs, const std::string& uvs) {
	return mkTrkGsVs(Units::from(utrk,trk), Units::from(ugs,gs),Units::from(uvs,vs));
}


Velocity Velocity::mkVel(const Vect3& p1,const Vect3& p2, double speed) {
	Velocity rtn = make(p2.Sub(p1).Hat().Scal(speed));
	//fpln(" $$ mkVel: rtn = "+rtn.toString());
	return rtn;
}

double Velocity::track(const Vect3& p1,const Vect3& p2) {
	double rtn = Util::atan2_safe(p2.x-p1.x,p2.y-p1.y);
	return rtn;
}


Velocity Velocity::genVel(const Vect3& p1, const Vect3& p2, double dt) {
	return make(p2.Sub(p1).Scal(1/dt));
}

Velocity Velocity::mkAddTrk(double trk) const {
	double s = sin(trk);
	double c = cos(trk);
	return mkVxyz(x*c+y*s, -x*s+y*c, z);
}

double Velocity::trkgs2vx(double trk, double gs) {
	return gs * sin(trk);
}

double Velocity::trkgs2vy(double trk, double gs) {
	return gs * cos(trk);
}

Vect2 Velocity::trkgs2v(double trk, double gs) {
	return Vect2(trkgs2vx(trk,gs), trkgs2vy(trk,gs));
}

double Velocity::angle() const {
	return vect2().angle();
}

double Velocity::angle(const std::string& uangle) const {
	return Units::to(uangle,angle());
}

double Velocity::trk() const {
	return vect2().trk();
}

double Velocity::track(const std::string& utrk) const {
	return Units::to(utrk,trk());
}

/**
 * Compass angle in radians in the range [<code>0</code>, <code>2*Math.PI</code>).
 * Convention is clockwise with respect to north.
 *
 * @return the compass angle [rad]
 */
double Velocity::compassAngle() const {
	return vect2().compassAngle();
}

/**
 * Compass angle in explicit units in corresponding range [<code>0</code>, <code>2*Math.PI</code>).
 * Convention is clockwise with respect to north.
 *
 *  @param u the explicit units of compass angle
 *
 *  @return the compass angle [u]
 */
double Velocity::compassAngle(const std::string& u) const {
	return Units::to(u,compassAngle());
}

double Velocity::gs() const {
	return sqrt_safe(x*x+y*y); //vect2().norm();
}

double Velocity::groundSpeed(const std::string& ugs) const {
	return Units::to(ugs,gs());
}

double Velocity::vs() const {
	return z;
}

double Velocity::verticalSpeed(const std::string& uvs) const {
	return Units::to(uvs,z);
}

bool Velocity::compare(const Velocity& v, double maxTrk, double maxGs, double maxVs) {
	if (Util::turnDelta(v.trk(),trk()) > maxTrk) return false;
	if (std::abs(v.gs() - gs()) > maxGs) return false;
	if (std::abs(v.vs() - vs()) > maxVs) return false;
	return true;
}

bool Velocity::compare(const Velocity& v, double horizDelta, double vertDelta) {
	return std::abs(z-v.z) <= vertDelta && vect2().Sub(v.vect2()).norm() <= horizDelta;
}


const Velocity& Velocity::ZEROV() {
	static Velocity* v = new Velocity(0,0,0);
	return *v;
}

const Velocity& Velocity::INVALIDV() {
	static Velocity* v = new Velocity(NaN, NaN, NaN);
	return *v;
}

/**
 * New velocity from existing velocity, changing only the track
 * @param trk track angle [rad]
 * @return new velocity
 */
Velocity Velocity::mkTrk(double trk) const {
	return mkTrkGsVs(trk, gs(), vs());
}

/**
 * New velocity from existing velocity, changing only the track
 * @param trk track angle [u]
 * @param u units
 * @return new velocity
 */
Velocity Velocity::mkTrk(double trk, std::string u) const {
	return mkTrk(Units::from(u,trk));
}

/**
 * New velocity from existing velocity, changing only the ground speed
 * @param gs [m/s]
 * @return
 */
Velocity Velocity::mkGs(double gs_d) const {
	  if (gs_d < 0) return INVALIDV();
	//return mkTrkGsVs(trk(), gs, vs());    // optimzation due to Aaron Dutle
	double gs0 = gs();
	if (gs0 > 0.0) {
		double scal = gs_d/gs0;
		return mkVxyz(x*scal, y*scal, vs());
	} else {
		return mkVxyz(0.0,gs_d,vs());
	}
}

/**
 * New velocity from existing velocity, changing only the ground speed
 * @param gs [u]
 * @param u unit
 * @return
 */
Velocity Velocity::mkGs(double gs_d, std::string u) const {
	return mkGs(Units::from(u,gs_d));
}

/**
}
 * New velocity from existing velocity, changing only the vertical speed
 * @param vs [m/s]
 * @return
 */
Velocity Velocity::mkVs(double vs) const {
	return mkVxyz(x, y, vs);
}

/**
 * New velocity from existing velocity, changing only the vertical speed
 * @param vs [u]
 * @param u units
 * @return
 */
Velocity Velocity::mkVs(double vs, std::string u) const {
	return mkVs(Units::from(u,vs));
}

Velocity Velocity::NegV() const {
	return Velocity::make(Neg());
}

Velocity Velocity::zeroSmallVs(double threshold) const {
	if (std::abs(z) < threshold) return mkVxyz(x,y,0.0);
	return mkVxyz(x,y,z);
}


Velocity Velocity::parseXYZ(const std::string& str) {
	return Velocity::make(Vect3::parse(str));
}


/** This parses a space or comma-separated string as a Trk/Gs/Vs Velocity (an inverse to the toString method).  If three bare values are
 * present, then it is interpreted as degrees/knots/fpm. If there are 3 value/unit pairs then each values is
 * interpreted wrt the appropriate unit.  If the string cannot be parsed, an INVALID value is
 * returned. */
Velocity Velocity::parse(const std::string& str) {
	Vect3 v3 = Vect3::parse(str);
	std::vector<std::string> fields = split(str, Constants::wsPatternParens);
	if (fields.size() == 3) {
		return Velocity::makeTrkGsVs(v3.x, v3.y, v3.z);
		//		} else if (fields.length == 6 && !fields[1].substring(0,3).equalsIgnoreCase("deg") && !fields[1].substring(0,3).equalsIgnoreCase("rad")) {
		//			return parseXYZ(str);
	}
	return Velocity::mkTrkGsVs(v3.x, v3.y, v3.z);
}

std::string Velocity::toUnitTest() const {
	return "Velocity::mkTrkGsVs("  +Fm6(trk())+", "+Fm6(gs())+", "+Fm6(vs())+"); ";
}

std::string Velocity::toString() const {
	return toString(Constants::get_output_precision());
}

std::string Velocity::toString(int prec) const {
	 return"("+Units::str("deg",compassAngle(),prec)+", "+Units::str("knot",gs(),prec)+", "+Units::str("fpm",vs(),prec)+")";
}

std::string Velocity::toStringUnits() const {
	return toStringUnits("deg","knot","fpm");
}

std::string Velocity::toStringUnits(const std::string& trkUnits, const std::string& gsUnits, const std::string& vsUnits) const {
	return  "("+Units::str(trkUnits,compassAngle())+", "+ Units::str(gsUnits,gs())+", "+ Units::str(vsUnits,vs())+")";
}

std::string Velocity::toStringXYZ() const {
	return toStringXYZ(Constants::get_output_precision());
}

std::string Velocity::toStringXYZ(int prec) const {
	return "("+FmPrecision(Units::to("knot", x),prec)+", "+FmPrecision(Units::to("knot", y),prec)+", "+FmPrecision(Units::to("fpm", z),prec)+")";
}

std::vector<std::string> Velocity::toStringList() const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else {
		ret.push_back(Fm12(Units::to("deg", compassAngle())));
		ret.push_back(Fm12(Units::to("knot", gs())));
		ret.push_back(Fm12(Units::to("fpm", vs())));
	}
	return ret;
}

std::vector<std::string> Velocity::toStringList(int precision) const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else {
		ret.push_back(FmPrecision(Units::to("deg", compassAngle()),precision));
		ret.push_back(FmPrecision(Units::to("knot", gs()),precision));
		ret.push_back(FmPrecision(Units::to("fpm", vs()),precision));
	}
	return ret;
}

std::vector<std::string> Velocity::toStringXYZList() const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else {
		ret.push_back(Fm12(Units::to("knot", x)));
		ret.push_back(Fm12(Units::to("knot", y)));
		ret.push_back(Fm12(Units::to("fpm", z)));
	}
	return ret;
}

std::vector<std::string> Velocity::toStringXYZList(int precision) const {
	std::vector<std::string> ret;
	if (isInvalid()) {
		ret.push_back("-");
		ret.push_back("-");
		ret.push_back("-");
	} else {
		ret.push_back(FmPrecision(Units::to("knot", x),precision));
		ret.push_back(FmPrecision(Units::to("knot", y),precision));
		ret.push_back(FmPrecision(Units::to("fpm", z),precision));
	}
	return ret;
}

std::string Velocity::toStringNP() const {
	return toStringNP(Constants::get_output_precision());
}

std::string Velocity::toStringNP(int precision) const {
	return FmPrecision(Units::to("deg", compassAngle()), precision)+", "+FmPrecision(Units::to("knot", gs()),precision)+", "+FmPrecision(Units::to("fpm", vs()),precision);
}

std::string Velocity::toStringNP(const std::string& utrk, const std::string& ugs, const std::string& uvs, int precision) const {
    return FmPrecision(Units::to(utrk, compassAngle()), precision)+", "+FmPrecision(Units::to(ugs, gs()), precision)+", "+FmPrecision(Units::to(uvs, vs()), precision);
}


//************************************************
// deprecated functions:

double trkgs2vx(double trk, double gs) {
	return gs * sin(trk);
}

double trkgs2vy(double trk, double gs) {
	return gs * cos(trk);
}

Vect2 trkgs2v(double trk, double gs) {
	return Vect2(trkgs2vx(trk,gs), trkgs2vy(trk,gs));
}

}
