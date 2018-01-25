/*
 * Vect2.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 *
 * 2-D vectors.
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include "Units.h"
#include <limits>
#include "string_util.h"
#include "format.h"
#include "Constants.h"
#include "Util.h" // NaN def

namespace larcfm {

Vect2::Vect2(const double xx, const double yy) : x(xx), y(yy) {}

bool Vect2::isZero() const {
	return x == 0.0 && y == 0.0;
}

bool Vect2::almostEquals(const Vect2& v) const {
	return Util::almost_equals(x,v.x) && Util::almost_equals(y,v.y);
}

bool Vect2::almostEquals(const Vect2& v, INT64FM maxUlps) const {
	return Util::almost_equals(x,v.x,maxUlps) && Util::almost_equals(y,v.y,maxUlps);
}

Vect2 Vect2::operator + (const Vect2& v) const {
	return this->Add(v);
}

Vect2 Vect2::operator - (const Vect2& v) const {
	return this->Sub(v);
}

Vect2 Vect2::operator - () const {
	return Vect2(-x,-y);
}

Vect2 Vect2::operator * (const double k) const {
	return this->Scal(k);
}

double Vect2::operator * (const Vect2& v) const { // Dot product
	return dot(v.x,v.y);
}

bool Vect2::operator == (const Vect2& v) const {  // strict equality
	return x==v.x && y==v.y;
}

bool Vect2::operator != (const Vect2& v) const {  // strict disequality
	return x!=v.x || y!=v.y;
}

Vect2 Vect2::Hat() const {
	//Vect2 v = new Vect2(this);
	//v.hat();
	//return v;
	double n = norm();
	if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
		return ZERO();
	}
	return Vect2(x/n, y/n);

//	double sq = sqv();
//	if (sq == 0.0) {
//		return ZERO();
//	}
//	double n_inv = Util::Q_rsqrt(sq);
//	return Vect2(x*n_inv, y*n_inv);
}

Vect2 Vect2::Add(const Vect2& v) const {
	//    Vect2 u = new Vect2(x,y);
	//    u.add(v);
	//    return u;
	return Vect2(x+v.x,y+v.y);
}

Vect2 Vect2::Sub(const Vect2& v) const {
	//    Vect2 u = new Vect2(x,y);
	//    u.sub(v);
	//    return u;
	return Vect2(x-v.x,y-v.y);
}

Vect2 Vect2::Neg() const {
	return Vect2(-x,-y);
}

Vect2 Vect2::Scal(double k) const {
	//    Vect2 u = new Vect2(x,y);
	//    u.scal(k);
	//    return u;
	return Vect2(k*x,k*y);
}

Vect2 Vect2::ScalAdd(double k, const Vect2& v) const {
	//    Vect2 u = new Vect2(x,y);
	//    u.scal(k);
	//    u.add(v);
	//    return u;
	return Vect2(k*x+v.x,k*y+v.y);
}

Vect2 Vect2::AddScal(double k, const Vect2& v) const {
	//Vect2 u = new Vect2(x,y);
	//    Vect2 vv = new Vect2(v.x,v.y);
	//    vv.scal(k);
	//    vv.add(this);
	//    return vv;
	return Vect2(x+k*v.x,y+k*v.y);
}

/**
 * Right perpendicular.
 *
 * @return the right perpendicular of <code>this</code> vector, i.e., (<code>y</code>, <code>-x</code>).
 */
Vect2 Vect2::PerpR() const {
	return Vect2(y,-x);
}

/**
 * Left perpendicular.
 *
 * @return the left perpendicular of <code>this</code> vector, i.e., (<code>-y</code>, <code>x</code>).
 */
Vect2 Vect2::PerpL() const {
	return Vect2(-y,x);
}

/**
 * Calculates position after t time units in direction and magnitude of velocity v
 * @param v    velocity
 * @param t    time
 * @return the new position
 */
Vect2 Vect2::linear(const Vect2& v, double t) const{
	return Vect2(x + v.x*t,y + v.y*t);
}

double Vect2::distance(const Vect2& s) const {
	return Util::sqrt_safe(Util::sq(s.x - x) + Util::sq(s.y - y));
}

double Vect2::dot(const double x, const double y) const {
	return this->x*x + this->y*y;
}

double Vect2::dot(const Vect2& v) const {
	return dot(v.x,v.y);
}

double Vect2::det(const Vect2& v) const {
	return det(v.x,v.y);
}

double Vect2::det(const double x, const double y) const {
	return this->x*y - this->y*x;
}

double Vect2::sqv() const {
	//This implementation
	//return dot(x,y);
	//was replaced by this implementation
	return x*x+y*y;
	// for performance reasons.
}

double Vect2::norm() const {
	return sqrt_safe(sqv());
}

// Angle in (-pi,pi]
double Vect2::angle() const {
	return atan2_safe(y,x);
}

// Track is clockwise angle in (-pi,pi] wrt north,
// i.e., track = pi/2 - angle()
double Vect2::trk() const {
	return atan2_safe(x,y);
}

// Compass angle = track in [0,2pi)
double Vect2::compassAngle() const {
	return to_2pi(trk());
}

// Compass angle = track in [0,2pi)
double Vect2::compassAngle(const Vect2& v2) const {
	return (v2-*this).compassAngle();
}

Vect2 Vect2::mkTrkGs(double trk, double gs) {
	return Vect2(gs*sin(trk),gs*cos(trk));
}

double Vect2::sqRel(const Vect2& v) const {
	return sq(x-v.x) + sq(y-v.y);
}

bool Vect2::leq(const Vect2& v, const Vect2& vo) const {
	return sqRel(vo) <= v.sqRel(vo);
}

// a portable way to test for IEEE NaN values
bool Vect2::isInvalid() const {
	return x != x || y != y;
}

std::string Vect2::toString() const {
	return toString(Constants::get_output_precision());
}

std::string Vect2::toString(int precision) const {
  return formatXY(precision,"(",", ",")");
}

std::string Vect2::toStringNP(const std::string& xunit, const std::string& yunit, int prec) const {
	return FmPrecision(Units::to(xunit,x),prec)+", "+FmPrecision(Units::to(yunit,y),prec);
}

std::string Vect2::formatXY(int prec, const std::string& pre, const std::string& mid, const std::string& post) const {
	return  pre+FmPrecision(x,prec)+mid+FmPrecision(y,prec)+post;
}

std::string Vect2::toPVS(int prec) const {
	return "(# x:= "+FmPrecision(x,prec)+", y:= "+FmPrecision(y,prec)+" #)";
}

/**
 * Return actual time of closest point approach (return negative infinity if parallel)
 */
double Vect2::actual_tcpa (const Vect2& so, const Vect2& vo, const Vect2& si,  const Vect2& vi){
	double rtn;
	Vect2 s = so - si;
	Vect2 v = vo - vi;
	double nv = v.norm();
	if (nv > 0) {
		rtn = -s.dot(v)/(nv*nv);
	} else {
		rtn = NINFINITY;;
	}
	return rtn;
}

/**
 * Return time to closest point approach
 * if time is negative or velocities are parallel returns 0
 */
double Vect2::tcpa (const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi){
	double t;
	Vect2 s = so.Sub(si);
	Vect2 v = vo.Sub(vi);
	double nv = v.sqv();
	if (nv > 0)
		t = Util::max(0.0,-s.dot(v)/nv);
	else
		t = 0;
	return t;
}

/**
 * Return distance at time of closest point of approach
 **/
double Vect2::dcpa(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi) {
	double t = tcpa(so,vo,si,vi);
	Vect2 s = so.Sub(si);
	Vect2 v = vo.Sub(vi);
	Vect2 st = s.AddScal(t,v);
	return st.norm();
}

/**
 * returns the perpendicular distance between line defined vy s,v and point q.
 * @param s
 * @param v
 * @param q
 */
double Vect2::distPerp(const Vect2& s, const Vect2& v, const Vect2& q) {
	double tp = q.Sub(s).dot(v)/v.sqv();
	return s.Add(v.Scal(tp)).Sub(q).norm();

}

/**
 * returns the perpendicular distance between line defined vy s,v and point q.
 * @param s
 * @param v
 * @param q
 */
double Vect2::distAlong(const Vect2& s, const Vect2& v, const Vect2& q) {
	double tp = q.Sub(s).dot(v)/v.sqv();
	//f.pln(" $$$ distAlong: tp = "+tp);
	return Util::sign(tp)*v.Scal(tp).norm();

}

Vect2 Vect2::intersect_pt(const Vect2& s0, const Vect2& v0, const Vect2& s1, const Vect2& v1) {
	if (Util::almost_equals(v0.det(v1),0.0)) {
		//fpln(" $$$$$$$$ ERROR $$$$$$$$$");
		return Vect2::INVALID();
	} else {
		Vect2 delta = s1.Sub(s0);
		double ss = delta.det(v1)/v0.det(v1);
		return s0.Add(v0.Scal(ss));
	}
}

bool Vect2::within_epsilon(const Vect2& v2, double epsilon) const {
	if (std::abs(x - v2.x) > epsilon) return false;
	if (std::abs(y - v2.y) > epsilon) return false;
	return true;
}


const Vect2& Vect2::ZERO() {
	static Vect2 *v = new Vect2(0, 0);
	return *v;
}

const Vect2& Vect2::INVALID() {
	static Vect2 *v = new Vect2(NaN, NaN);
	return *v;
}



}
