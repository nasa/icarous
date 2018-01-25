/*
 * ENUProjection.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "OrthographicProjection.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Util.h"
#include "Units.h"
#include "Vect2.h"
//#include "UnitSymbols.h"
#include "Constants.h"
#include <stdexcept>


namespace larcfm {
using std::string;
using std::cout;
using std::endl;
using std::runtime_error;

// Static functions

Vect2 OrthographicProjection::spherical2xy(double lat, double lon) const {
	double c = std::sin(llaRef.lat())*std::sin(lat)+std::cos(llaRef.lat())*std::cos(lat)*std::cos(lon-llaRef.lon());
	if (c < 0) return Vect2::INVALID();

	double r = GreatCircle::spherical_earth_radius;
	double x = r*std::cos(lat)*std::sin(lon-llaRef.lon());
	double y = r*(std::cos(llaRef.lat())*std::sin(lat)-std::sin(llaRef.lat())*std::cos(lat)*std::cos(lon-llaRef.lon()));
	return Vect2(x,y);
}

LatLonAlt OrthographicProjection::xy2spherical(double x, double y, double alt) const {
	double r = GreatCircle::spherical_earth_radius;
	double p = Util::sqrt_safe(Util::sq(x)+Util::sq(y));
	if (Util::almost_equals(p, 0.0)) return llaRef.mkAlt(alt);
	double c = Util::asin_safe(p/r);
	double lat = Util::asin_safe(std::cos(c)*std::sin(llaRef.lat()) + y*std::sin(c)*std::cos(llaRef.lat())/p);
	double lon = llaRef.lon()+Util::atan2_safe(x*std::sin(c), p*std::cos(c)*std::cos(llaRef.lat())-y*std::sin(c)*std::sin(llaRef.lat()));
	return LatLonAlt::mk(lat, lon, alt);

}



// OrthographicProjection

OrthographicProjection::OrthographicProjection() {
	projAlt = 0;
	llaRef = LatLonAlt::ZERO();
}

OrthographicProjection::OrthographicProjection(const LatLonAlt& lla) {
	projAlt = lla.alt();
	llaRef = lla;
}

OrthographicProjection::OrthographicProjection(double lat, double lon, double alt) {
	projAlt = alt;
	llaRef = LatLonAlt::mk(lat, lon, alt);
}

OrthographicProjection OrthographicProjection::makeNew(const LatLonAlt& lla) const {
	return OrthographicProjection(lla);
}

OrthographicProjection OrthographicProjection::makeNew(double lat, double lon, double alt) const {
	return OrthographicProjection(lat, lon, alt);
}

double OrthographicProjection::conflictRange(double lat, double accuracy) const {
	return Units::from("NM", std::floor(243.0*std::pow(Units::to("NM",std::ceil(accuracy)),1.0/3.0)));
}

double OrthographicProjection::maxRange() const{
	return Units::from("NM", 3400);
}

LatLonAlt OrthographicProjection::getProjectionPoint() const {
	return llaRef;
}

Vect2 OrthographicProjection::project2(const LatLonAlt& lla) const {
	return spherical2xy(lla.lat(),lla.lon());
}

Vect3 OrthographicProjection::project(const LatLonAlt& lla) const {
	return Vect3(project2(lla),lla.alt() - projAlt);
}

Vect3 OrthographicProjection::project(const Position& sip) const {
	Vect3 si;
	if (sip.isLatLon()) {
		si = project(sip.lla());
	} else {
		si = sip.point();
	}
	return si;
}

Point OrthographicProjection::projectPoint(const Position& sip) const {
	return Point::mk(project(sip));
}

LatLonAlt OrthographicProjection::inverse(const Vect2& xy, double alt) const {
	return xy2spherical(xy.x, xy.y, alt+projAlt);
}

LatLonAlt OrthographicProjection::inverse(const Vect3& xyz) const {
	return inverse(xyz.vect2(), xyz.z);
}

Velocity OrthographicProjection::projectVelocity(const LatLonAlt& lla, const Velocity& v) const {
	double timeStep = 10.0;
	LatLonAlt ll2 = GreatCircle::linear_initial(lla,v,timeStep);
	Vect3 se = project(lla);
	Vect3 s2 = project(ll2);
	Vect3 vn = s2.Sub(se).Scal(1/timeStep);
	return Velocity::make(vn);
}

Velocity OrthographicProjection::projectVelocity(const Position& ss, const Velocity& v) const {
	if (ss.isLatLon()) {
		return projectVelocity(ss.lla(),v);
	} else {
		return v;
	}
}

// The user needs to keep track of whether to translate back (i.e. whether original was LatLon())
Velocity OrthographicProjection::inverseVelocity(const Vect3& s, const Velocity& v, bool toLatLon) const {
	if (toLatLon) {
		double timeStep = 10.0;
		Vect3 s2 = s.linear(v,timeStep);
		LatLonAlt lla1 = inverse(s);
		LatLonAlt lla2 = inverse(s2);
		Velocity nv = GreatCircle::velocity_initial(lla1,lla2,timeStep);
		return nv;
	} else {
		return v;
	}
}

std::pair<Vect3,Velocity> OrthographicProjection::project(const Position& p, const Velocity& v) const {
	return std::pair<Vect3,Velocity>(project(p),projectVelocity(p,v));
}

std::pair<Position,Velocity> OrthographicProjection::inverse(const Vect3& p, const Velocity& v, bool toLatLon) const {
	if (toLatLon) {
		return std::pair<Position,Velocity>(Position(inverse(p)),inverseVelocity(p,v,true));
	} else {
		return std::pair<Position,Velocity>(Position(p),v);
	}
}


}

