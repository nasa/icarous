/*
 * Vect3.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 *
 * 3-D vectors.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect3.h"
#include "Units.h"
#include <limits>
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include "Util.h" // NaN def

namespace larcfm {

Vect3::Vect3(const double xx, const double yy, const double zz) : x(xx), y(yy), z(zz) {
}

Vect3::Vect3(const Vect2&v, const double vz) : x(v.x), y(v.y), z(vz) {
}

Vect3 Vect3::makeXYZ(double x, std::string ux, double y, std::string uy, double z, std::string uz) {
	return Vect3(Units::from(ux,x),Units::from(uy,y),Units::from(uz,z));
}

Vect3 Vect3::mkXYZ(double x, double y, double z) {
	return Vect3(x,y,z);
}

Vect3 Vect3::mkX(double nx) {
	return Vect3(nx,y,z);
}
Vect3 Vect3::mkY(double ny) {
	return Vect3(x,ny,z);
}
Vect3 Vect3::mkZ(double nz) {
	return Vect3(x,y,nz);
}

bool Vect3::isZero() const {
	return x == 0.0 && y == 0.0 && z == 0.0;
}

bool Vect3::almostEquals(const Vect3& v) const {
	return Util::almost_equals(x,v.x) && Util::almost_equals(y,v.y) && Util::almost_equals(z,v.z);
}

bool Vect3::almostEquals(const Vect3& v, INT64FM maxUlps) const {
	return Util::almost_equals(x,v.x,maxUlps) && Util::almost_equals(y,v.y,maxUlps) && Util::almost_equals(z,v.z,maxUlps);
}

bool Vect3::within_epsilon(const Vect3& v2, double epsilon) const {
	if (std::abs(x - v2.x) > epsilon) return false;
	if (std::abs(y - v2.y) > epsilon) return false;
	if (std::abs(z - v2.z) > epsilon) return false;
	return true;
}


Vect3 Vect3::operator + (const Vect3& v) const {
	return this->Add(v);
}

Vect3 Vect3::operator - (const Vect3& v) const {
	return this->Sub(v);
}

Vect3 Vect3::operator - () const {
	return Vect3(-x,-y,-z);
}

Vect3 Vect3::operator * (const double k) const {
	return this->Scal(k);
}

double Vect3::operator * (const Vect3& v) const { // Dot product
	return dot(v.x,v.y,v.z);
}

bool Vect3::operator == (const Vect3& v) const {  // strict equality
	return x==v.x && y==v.y && z==v.z;
}

bool Vect3::operator != (const Vect3& v) const {  // strict disequality
	return x!=v.x || y!=v.y || z!=v.z;
}

Vect2 Vect3::vect2() const {
	return Vect2(x,y);
}

Vect3 Vect3::Hat() const {
	double n = norm();
	if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
		return ZERO();
	}
	return Vect3(x/n, y/n, z/n);
}

Vect3 Vect3::Hat2D() const {
	return Vect3(x,y,0.0).Hat();
}

Vect3 Vect3::cross(const Vect3& v) const {
	return Vect3(this->y*v.z - this->z*v.y, this->z*v.x - this->x*v.z, this->x*v.y - this->y*v.x);
}

bool Vect3:: parallel(const Vect3& v) const {
	return cross(v).almostEquals(Vect3::ZERO());
}

Vect3 Vect3::Add(const Vect3& v) const{
	return Vect3(x+v.x, y+v.y, z+v.z);
}

Vect3 Vect3::Sub(const Vect3& v) const {
	return Vect3(x-v.x,y-v.y,z-v.z);
}

Vect3 Vect3::Neg() const {
	return Vect3(-x,-y,-z);
}

Vect3 Vect3::Scal(double k) const {
	return Vect3(k*x,k*y,k*z);
}

Vect3 Vect3::ScalAdd(const double k, const Vect3& v) const {
	return Vect3(k*x+v.x, k*y+v.y, k*z+v.z);
}

Vect3 Vect3::AddScal(double k, const Vect3& v) const {
	return Vect3(x+k*v.x, y+k*v.y, z+k*v.z);
}

Vect3 Vect3::PerpR() const {
	return Vect3(y,-x, 0);
}

Vect3 Vect3::PerpL() const {
	return Vect3(-y,x, 0);
}

Vect3 Vect3::linear(const Vect3& v, double t) const {
	return Vect3(x+v.x*t, y+v.y*t, z+v.z*t);
}

Vect3 Vect3::linearByDist(double track, double d) const {
	double gs = 100;
	//Velocity v = Velocity::mkTrkGsVs(track,gs,0.0);
	Vect3 v = Vect3(gs*sin(track),gs*cos(track),0.0);
	double dt = d/gs;
	return linear(v,dt);
 }


double Vect3::dot(const double x, const double y, const double z) const {
	return this->x*x + this->y*y + this->z*z;
}

double Vect3::dot(const Vect3& v) const {
	return dot(v.x, v.y, v.z);
}

double Vect3::sqv() const {
	return dot(x,y,z);
}

double Vect3::norm() const {
	return sqrt_safe(sqv());
}

double Vect3::cyl_norm(const double d, const double h) const {
	return std::max(vect2().sqv()/sq(d),sq(z/h));
}

double Vect3::distanceH(const Vect3& w) const {
	Vect2 v = Vect2(x,y);
	return (v-w.vect2()).norm();
}
double Vect3::distanceV(const Vect3& w) const {
	return z - w.z;
}

std::string Vect3::toString() const {
	return toString(Constants::get_output_precision());
}

std::string Vect3::toString(int precision) const {
	return formatXYZ(precision,"(",", ",")");
}

std::string Vect3::toStringNP(const std::string& xunit, const std::string& yunit, const std::string& zunit, int prec) const {
	return FmPrecision(Units::to(xunit,x),prec)+", "+FmPrecision(Units::to(yunit,y),prec)+", " +
			FmPrecision(Units::to(zunit,z),prec);
}

std::string Vect3::formatXYZ(int prec, const std::string& pre, const std::string& mid, const std::string& post) const {
	return pre+FmPrecision(x,prec)+mid+FmPrecision(y,prec)+mid+FmPrecision(z,prec)+post;
}

std::string Vect3::toPVS(int prec) const {
	return "(# x:= "+FmPrecision(x,prec)+", y:= "+FmPrecision(y,prec)+", z:= "+FmPrecision(z,prec)+" #)";
}

/**
 * Returns true if the current vector has an "invalid" value
 */
bool Vect3::isInvalid() const {
	return x != x || y != y || z != z;
}

const Vect3& Vect3::ZERO() {
	static Vect3* v = new Vect3(0,0,0);
	return *v;
}

const Vect3& Vect3::INVALID() {
	static Vect3* v = new Vect3(NaN, NaN, NaN);
	return *v;
}

Vect3 Vect3::parse(const std::string& s) {
	std::vector<std::string> fields = split(s, Constants::wsPatternParens);
	while (fields.size() > 0 && equals(fields[0], "")) {
		fields.erase(fields.begin());
	}
	if (fields.size() == 3) {
		return Vect3(Util::parse_double(fields[0]),Util::parse_double(fields[1]),Util::parse_double(fields[2]));
	} else if (fields.size() == 6) {
		return Vect3(Units::from(Units::clean(fields[1]),Util::parse_double(fields[0])),
				Units::from(Units::clean(fields[3]),Util::parse_double(fields[2])),
				Units::from(Units::clean(fields[5]),Util::parse_double(fields[4])));
	}
	return Vect3::INVALID();
}

}
