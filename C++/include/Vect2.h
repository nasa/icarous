/*
 * Vect2.h
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 *
 * 2-D vectors.
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef VECT2_H_
#define VECT2_H_

#include "Util.h"
#include <string>

namespace larcfm {

/**
 * 2-Dimensional mathematical vectors.
 */
class Vect2 {

public:

	// This class should be immutable and therefore
	// data members should be "const."/ However, if
	// they were const, the there cannot be an
	// assignment operator interfers with the default
	// assignment operator.  Hopefully, no one
	// will take advantage of the non-constness of
	// these members.

	/** The x-component */
	double x;
	/** The y-component */
	double y;

	/**
	 * Creates a new vector with coordinates (<code>x</code>,<code>y</code>).
	 *
	 * @param xx Real value
	 * @param yy Real value
	 */
	Vect2(const double xx=0.0, const double yy=0.0);

	/** Destructor */
	virtual ~Vect2() { }

	/**
	 * Checks if vector is zero.
	 *
	 * @return <code>true</code>, if <code>this</code> vector is zero.
	 */
	bool isZero() const;
	/**
	 * Checks if vectors are almost equal.
	 *
	 * @param v Vector
	 *
	 * @return <code>true</code>, if <code>this</code> vector is almost equal
	 * to <code>v</code>.
	 */
	bool almostEquals(const Vect2& v) const;

	/**
	 * Checks if vectors are almost equal.
	 *
	 * @param v Vector
	 * @param maxUlps unit of least precision
	 *
	 * @return <code>true</code>, if <code>this</code> vector is almost equal
	 * to <code>v</code>.
	 */
	bool almostEquals(const Vect2& v, INT64FM maxUlps) const;

	/**
	 * Vector addition.
	 *
	 * @param v Vector
	 *
	 * @return the vector addition of <code>this</code> vector and <code>v</code>.
	 */
	Vect2  operator + (const Vect2& v) const;
	/**
	 * Vector subtraction.
	 *
	 * @param v Vector
	 *
	 * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
	 */
	Vect2  operator - (const Vect2& v) const;
	/**
	 * Vector negation.
	 *
	 * @return the vector negation of of <code>this</code> vector.
	 */
	Vect2  operator - () const;
	/**
	 * Scalar multiplication.
	 *
	 * @param k Real value
	 *
	 * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
	 */
	Vect2  operator * (const double k) const;
	/**
	 * Dot product.
	 *
	 * @param v Vector
	 *
	 * @return the dot product of <code>this</code> vector and <code>v</code>.
	 */
	double operator * (const Vect2& v) const; // Dot product

	/** Exact equality */
	bool operator == (const Vect2& v) const;
	/** Is any component not exactly equal */
	bool operator != (const Vect2& v) const;

	/**
	 * Make a unit vector from the current vector. If it is a zero vector, then a copy is returned.
	 * @return the unit vector
	 */
	Vect2 Hat() const;

	/**
	 * Vector addition.
	 *
	 * @param v Vector
	 *
	 * @return the vector addition of <code>this</code> vector and <code>v</code>.
	 */
	Vect2  Add(const Vect2& v) const;

	/**
	 * Vector subtraction.
	 *
	 * @param v Vector
	 *
	 * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
	 */
	Vect2  Sub(const Vect2& v) const;

	/**
	 * Vector negation.
	 *
	 * @return the vector negation of of <code>this</code> vector.
	 */
	Vect2  Neg() const;

	/**
	 * Scalar multiplication.
	 *
	 * @param k Real value
	 *
	 * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
	 */
	Vect2  Scal(double k) const;

	/**
	 * Scalar and addition multiplication.  Compute: k*<code>this</code> + v
	 *
	 * @param k real value
	 * @param v vector
	 *
	 * @return the scalar multiplication <code>this</code> vector and <code>k</code>, followed by an
	 * addition to vector <code>v</code>.
	 */
	Vect2  ScalAdd(const double k, const Vect2& v) const;

	/**
	 * Addition and scalar multiplication.  Compute: this + k*<code>v</code>;
	 *
	 * @param k real value
	 * @param v vector
	 *
	 * @return the addition of <code>this</code> vector to <code>v</code> scaled by <code>k</code>.
	 */
	Vect2 AddScal(double k, const Vect2& v) const;



	/**
	 * Right perpendicular.
	 *
	 * @return the right perpendicular of <code>this</code> vector, i.e., (<code>y</code>, <code>-x</code>).
	 */
	Vect2 PerpR() const;

	/**
	 * Left perpendicular.
	 *
	 * @return the left perpendicular of <code>this</code> vector, i.e., (<code>-y</code>, <code>x</code>).
	 */
	Vect2 PerpL() const;


	/**
	 * Calculates position after t time units in direction and magnitude of velocity v
	 * @param v    velocity
	 * @param t    time
	 * @return the new position
	 */
	Vect2 linear(const Vect2& v, double t) const;

	double distance(const Vect2& s) const;

	/**
	 * Dot product.
	 *
	 * @param x Real value
	 * @param y Real value
	 *
	 * @return the dot product of <code>this</code> vector and (<code>x</code>,<code>y</code>).
	 */
	double dot(const double x, const double y) const;
	/**
	 * Dot product.
	 *
	 * @param v Vector
	 *
	 * @return the dot product of <code>this</code> vector and <code>v</code>.
	 */
	double dot(const Vect2& v) const;
	/**
	 * Determinant.
	 *
	 * @param x Real value
	 * @param y Real value
	 *
	 * @return the determinant of <code>this</code> vector and (<code>x</code>,<code>y</code>).
	 */
	double det(const double x, const double y) const;
	/**
	 * Determinant.
	 *
	 * @param v Vector
	 *
	 * @return the determinant of <code>this</code> vector and <code>v</code>.
	 */
	double det(const Vect2& v) const;
	/**
	 * Square.
	 *
	 * @return the dot product of <code>this</code> vector with itself.
	 */
	double sqv() const;
	/**
	 * Norm.
	 *
	 * @return the norm of of <code>this</code> vector.
	 */
	double norm() const;

	/**
	 * Angle.
	 *
	 * @return the angle of <code>this</code> vector in the range
	 * (-<code>pi</code>, <code>pi</code>]. Convention is counter-clockwise
	 * with respect to east.
	 */
	double angle() const;

	/**
	 * Track angle.
	 *
	 * @return the track angle of <code>this</code> vector in the range
	 * (-<code>pi</code>, <code>pi</code>]. The track angle is the clockwise angle
	 * with respect to north, i.e., <code>v.track() = pi/2 - v.angle()</code>.
	 */
	double track() const;

	/**
	 * Compass angle.
	 *
	 * @return the track angle of <code>this</code> vector in the range
	 * [<code>0</code>, <code>2*pi</code>). Convention is clockwise with respect to north.
	 */
	double compassAngle() const;


	/**
	 * Compass angle from this position to v2
	 *
	 * @return the track angle of <code>v2-this</code> vector in the range
	 * [<code>0</code>, <code>2*Math.PI</code>). Convention is clockwise with respect to north.
	 */
	double compassAngle(const Vect2& v2) const;


	/**
	 * Construct a Vect2 from a track angle and ground speed
	 *
	 * @param trk    track angle
	 * @param gs     ground speed
	 *
	 */
	static Vect2 mkTrkGs(double trk, double gs);

	/**
	 * Relative less or equal.
	 *
	 * @param v  Vector
	 * @param vo Vector
	 *
	 * @return <code>true</code>, if the norm of <code>Sub(vo)</code> is less or equal than
	 * the norm of <code>v.Sub(vo)</code>.
	 */
	bool   leq(const Vect2& v, const Vect2& vo) const;

	/** Returns true if the current vector has an "invalid" value */
	bool isInvalid() const;

	/** Return a string representing this vector */
	std::string toString() const;

	/** A string representation of this vector */
	std::string toString(int precision) const;

	/** A string representation of this vector */
	std::string toStringNP(const std::string& xunit, const std::string& yunit, int precision) const;

	std::string formatXY(int prec, const std::string& pre, const std::string& mid, const std::string& post) const;

	std::string toPVS(int prec) const;

	/**
	 * Return actual time of closest point approach (return negative infinity if parallel)
	 */
	static double actual_tcpa (const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi);

	/** time to closest point of approach
	 * if time is negative or velocities are parallel returns 0
	 */
	static double tcpa (const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi);

	/** distance at time of closest point of approach
	 **/
	static double dcpa(const Vect2& so, const Vect2& vo, const Vect2& si, const Vect2& vi);

	/**
	 * returns the perpendicular distance between line defined by s,v and point q.
	 * @param s point on line
	 * @param v direction vector of line
	 * @param q a point not on the line
	 */
	static double distPerp(const Vect2& s, const Vect2& v, const Vect2& q);

	/**
	 * returns the longitudinal distance between line defined by s,v and point q.
	 * @param s point on line
	 * @param v direction vector of line
	 * @param q a point not on the line
	 */
	static double distAlong(const Vect2& s, const Vect2& v, const Vect2& q);


	/**
	 * Computes intersection point of two lines
	 * @param s0 starting point of line 1
	 * @param v0 direction vector for line 1
	 * @param s1 starting point of line 2
	 * @param v1 direction vector of line 2
	 * @return 2-dimensional point of intersection, or an invalid point if they are parallel
	 */
	static Vect2 intersect_pt(const Vect2& s0, const Vect2& v0, const Vect2& s1, const Vect2& v1);




	/** Return a zero vector */
	static const Vect2 ZERO;

	/** Return an "invalid" vector */
	static const Vect2& INVALID();

	//    static Vect2& makeInvalid();

	bool within_epsilon(const Vect2& v2, double epsilon) const;


private:
	double sqRel(const Vect2& v) const;

};


}

#endif /* VECT2_H_ */
