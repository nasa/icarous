/* A Lat/Lon/Alt position
 * 
 * Authors:  George Hagen              NASA Langley Research Center
 *           Ricky Butler              NASA Langley Research Center
 *           Jeff Maddalon             NASA Langley Research Center
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "LatLonAlt.h"
#include "Units.h"
#include "GreatCircle.h"
#include "string_util.h"
#include "format.h"
#include <limits>

namespace larcfm {

LatLonAlt::LatLonAlt(double x, double y, double z) :
    				lati(x),
					longi(y),
					alti(z) {
}

LatLonAlt::LatLonAlt() :
    				lati(0),
					longi(0),
					alti(0) {
}

LatLonAlt& LatLonAlt::operator= (const LatLonAlt& lla) {
	lati = lla.lati;
	longi = lla.longi;
	alti = lla.alti;

	return *this;
}

bool LatLonAlt::operator == (const LatLonAlt& v) const {
	return lati == v.lati && longi == v.longi && alti == v.alti;
}

bool LatLonAlt::operator != (const LatLonAlt& v) const {
	return lati != v.lati || longi != v.longi || alti != v.alti;
}

bool LatLonAlt::equals(const LatLonAlt& a) const {
	return *this == a;
}

bool LatLonAlt::equals2D(const LatLonAlt& lla) const {
	return (lati == lla.lati) && (longi == lla.longi);
}

double LatLonAlt::decimal_angle(double degrees, double minutes, double seconds, bool north_east) {
	return ((north_east) ? 1.0 : -1.0) * Units::from("deg", (degrees + minutes / 60.0 + seconds / 3600.0));
}

double LatLonAlt::latitude() const {
	return to_180(Units::to("deg", lati));
}

double LatLonAlt::longitude() const {
	return to_180(Units::to("deg", longi));
}

double LatLonAlt::altitude() const {
	return Units::to("ft", alti);
}

double LatLonAlt::lat() const {
	return lati;
}
double LatLonAlt::lon() const {
	return longi;
}
double LatLonAlt::alt() const {
	return alti;
}

double LatLonAlt::distanceH(const LatLonAlt& lla2) const {
	return GreatCircle::distance(*this,lla2);
}

const LatLonAlt LatLonAlt::make() {
	return LatLonAlt(0.0, 0.0, 0.0);
}

const LatLonAlt LatLonAlt::mk(const LatLonAlt& v) {
	return LatLonAlt(v.lat(),
			v.lon(),
			v.alt());
}

const LatLonAlt LatLonAlt::make(double lat, double lon, double alt){
	return LatLonAlt(Units::from("deg", lat),
			Units::from("deg", lon),
			Units::from("ft", alt));
}

const LatLonAlt LatLonAlt::make(double lat, std::string lat_unit, double lon, std::string lon_unit,
		double alt, std::string alt_unit) {
	return LatLonAlt(Units::from(lat_unit, lat),
			Units::from(lon_unit, lon),
			Units::from(alt_unit, alt));
}

const LatLonAlt LatLonAlt::mk(double lat, double lon, double alt) {
	return LatLonAlt(lat, lon, alt);
}

const LatLonAlt LatLonAlt::mkAlt(double alt) const {
	return LatLonAlt(lati, longi, alt);
}

const LatLonAlt LatLonAlt::makeAlt(double alt) const {
	return LatLonAlt(lati, longi, Units::from("ft",alt));
}

const LatLonAlt LatLonAlt::zeroAlt() const {
	return LatLonAlt(lati, longi, 0.0);
}

bool LatLonAlt::isInvalid() const {
	return lati != lati || longi != longi || alti != alti;
}

//const LatLonAlt LatLonAlt::linear(const Velocity& v, double time) {
//	LatLonAlt current = LatLonAlt(lati, longi, alti);
//	if (time == 0 || v.isZero()) {
//		return current;
//	} else {
//		return GreatCircle::linear_initial(current,v,time);
//	}
//}


const LatLonAlt LatLonAlt::linearEst(double dn, double de) const {
	//f.pln(" lat = "+Units.str("deg",lati)+" lon = "+Units.str("deg",longi));
	double R = GreatCircle::spherical_earth_radius; //6378137;                   // diameter earth in meters
	double nLat = lati + dn/R;
	double nLon = longi + de/(R*cos(lati));
	//f.pln(" nLat = "+Units.str("deg",nLat)+" nLon = "+Units.str("deg",nLon));
	return LatLonAlt::mk(nLat,nLon,alti);
}

const LatLonAlt LatLonAlt::linearEst(const Velocity& vo, double tm)  const{
	double dn = vo.Scal(tm).y;
	double de = vo.Scal(tm).x;
	double nAlt = alti + vo.z*tm;
	return linearEst(dn,de).mkAlt(nAlt);
}


std::string LatLonAlt::toString() const {
	std::stringstream temp;
	temp << Units::str("deg",lat()) << ", "
			<< Units::str("deg",lon()) << ", "
			<< Units::str("ft",alt());
	return temp.str();
}

std::string LatLonAlt::toString(int precision) const {
  return toString("deg","deg","ft",precision);
}

std::string LatLonAlt::toString(const std::string& latunit, const std::string& lonunit, const std::string& zunit) const {
	return toString(latunit,lonunit,zunit,Constants::get_output_precision());
}

std::string LatLonAlt::toString(const std::string& latunit, const std::string& lonunit, const std::string& zunit, int precision) const {
	std::stringstream temp;
	temp <<  FmPrecision(Units::to(latunit,lati),precision) << ", " << FmPrecision(Units::to(lonunit,longi),precision) << ", " << FmPrecision(Units::to(zunit,alti),precision);
	return temp.str();
}

const LatLonAlt& LatLonAlt::ZERO() {
	static LatLonAlt* v = new LatLonAlt(0,0,0);
	return *v;
}

const LatLonAlt& LatLonAlt::INVALID(){
	static LatLonAlt* v = new LatLonAlt(std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN(), std::numeric_limits<double>::quiet_NaN());
	return *v;
}


const LatLonAlt LatLonAlt::parse(const std::string& str) {
	std::vector<std::string> fields = split(str, Constants::wsPatternParens);
	while (fields.size() > 0 && (fields[0] == "")) {
		fields.erase(fields.begin());
	}
	if (fields.size() == 3) {
		return LatLonAlt::make(Util::parse_double(fields[0]),Util::parse_double(fields[1]),Util::parse_double(fields[2]));
	} else if (fields.size() == 6) {
		return LatLonAlt::mk(Units::from(Units::clean(fields[1]),Util::parse_double(fields[0])),
				Units::from(Units::clean(fields[3]),Util::parse_double(fields[2])),
				Units::from(Units::clean(fields[5]),Util::parse_double(fields[4])));
	}
	return LatLonAlt::INVALID();
}


LatLonAlt LatLonAlt::normalize(double lat, double lon, double alt) {
	double nlat, nlon;
	nlon = Util::to_pi(lon);
	lat = Util::to_pi(lat);
	nlat = Util::to_pi2_cont(lat);
	if (lat != nlat) {
		nlon = Util::to_pi(nlon + Pi);
	}
	return LatLonAlt::mk(nlat, nlon, alt);
}

LatLonAlt LatLonAlt::normalize(double lat, double lon) {
	return normalize(lat, lon, 0.0);
}

LatLonAlt LatLonAlt::normalize() const {
	return normalize(lat(), lon(), alt());
}


bool LatLonAlt::isWest(const LatLonAlt& a) const {
	// this uses the same calculations as Util.clockwise:
	return Util::clockwise(a.lon(), lon());
}

double LatLonAlt::track(const LatLonAlt& p) const {
	LatLonAlt ThisLatLonAlt = mk(lati,longi,alti);
	return GreatCircle::initial_course(ThisLatLonAlt,p);
}


}
