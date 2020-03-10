/*
 * Point.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "Point.h"
#include "Units.h"
#include "Constants.h"
#include "format.h"
#include "Vect2.h"
#include "Vect3.h"
#include "string_util.h"
#include "Util.h" // NaN def
#include <string>


using namespace larcfm;

Point::Point(double x, double y, double z) : Vect3(x,y,z) {
} 


const Point& Point::ZEROP() {
	static Point* p = new Point(0,0,0);
	return *p;
}

const Point& Point::INVALIDP() {
	static Point* p = new Point(NaN, NaN, NaN);
	return *p;
}



Point Point::mk(const Vect3& v) {
	return Point(v.x,v.y,v.z);
}     

Point Point::mk(const Vect2& v, double alt) {
	return Point(v.x,v.y,alt);
}     

Point Point::mk(double x, double y, double z) {
	return Point(x,y,z);
}     

Point Point::make(double x, double y, double z) {
	return make(x,"NM",y,"NM",z,"ft");
}     

Point Point::make(double x, const std::string& unitsX, double y, const std::string& unitsY, double z, const std::string& unitsZ) {
	return Point(Units::from(unitsX,x),Units::from(unitsY,y),Units::from(unitsZ,z));
}     

Point Point::mkX(double X) const {
	return mk(X, y, z);
}

Point Point::mkY(double Y) const {
	return mk(x, Y, z);
}

Point Point::mkZ(double Z) const {
	return mk(x, y, Z);
}

Point Point::Hat() const {
	// This method means:
	//    return make(this.Hat());
	// but for efficiency, I am implementing it explicitly
	double n = norm();
	if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
		return *this;
	}
	return mk(x / n, y / n, z / n);
}

Point Point::Neg() const {
	return mk(-x, -y, -z);
}

Point Point::Add(const Point& v) const {
	return mk(x+v.x, y+v.y, z+v.z);
}

Point Point::Sub(const Point& v) const {
	return mk(x-v.x, y-v.y, z-v.z);
}

Point Point::Scal(double k) const {
	//	    Vect3 u(x,y,z);
	//	    u.scal(k);
	//	    return u;
	return mk(k*x, k*y, k*z);
}

Point Point::linear(const Velocity& v, double t) const {
	return mk(x + v.x*t,y + v.y*t,z +v.z*t);
}

std::string Point::toString() const {
	fpln("Point internal"+Fm8(x)+" "+Fm8(y));
	return toString(Constants::get_output_precision(),"NM","NM","ft");
}

std::string Point::toString(int prec, const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const {
	return "("+FmPrecision(Units::to(xUnits, x),prec)+", "+FmPrecision(Units::to(yUnits, y),prec)+", "+FmPrecision(Units::to(zUnits, z),prec)+")";
}

std::string Point::toStringUnits(const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const {
	return "("+Units::str(xUnits, x)+", "+Units::str(yUnits, y)+", "+Units::str(zUnits, z)+")";
}

std::vector<std::string> Point::toStringList() const {
	std::vector<std::string> ret(3);
	ret.push_back(to_string(Units::to("NM", x)));
	ret.push_back(to_string(Units::to("NM", y)));
	ret.push_back(to_string(Units::to("ft", z)));
	return ret;
}

std::string Point::toStringNP() const {
	return toStringNP(15, "NM","NM","ft");
}

std::string Point::toStringNP(int precision) const {
	return toStringNP(precision,"NM","NM","ft");
}

std::string Point::toStringNP(int precision, const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const {
	return FmPrecision(Units::to(xUnits, x), precision)+", "+FmPrecision(Units::to(yUnits, y), precision)+", "+FmPrecision(Units::to(zUnits, z), precision);
}

// static Point parse(std::string str) {
// 	String[] fields = str.split(Constants::wsPatternParens);
// 	try {
// 		if (fields.length == 3) {
// 			return make(
// 					Double.parseDouble(fields[0]),
// 					Double.parseDouble(fields[1]),
// 					Double.parseDouble(fields[2]));
// 		} else if (fields.length == 6) {
// 			return make(
// 					Double.parseDouble(fields[0]), Units.clean(fields[1]),
// 					Double.parseDouble(fields[2]), Units.clean(fields[3]),
// 					Double.parseDouble(fields[4]), Units.clean(fields[5]));
// 		}
// 	} catch (Exception e) {}
// 	return make(Vect3.INVALID);	
// }


Point Point::parse(const std::string& s) {
	std::vector<std::string> fields = split(s, Constants::wsPatternParens);
	while (fields.size() > 0 && equals(fields[0], "")) {
		fields.erase(fields.begin());
	}
	if (fields.size() == 3) {
		return make(Util::parse_double(fields[0]),Util::parse_double(fields[1]),Util::parse_double(fields[2]));
	} else if (fields.size() == 6) {
		return make(Util::parse_double(fields[0]),Units::clean(fields[1]),
				Util::parse_double(fields[2]),Units::clean(fields[3]),
				Util::parse_double(fields[4]),Units::clean(fields[5]));
	}
	return mk(Vect3::INVALID());
}
