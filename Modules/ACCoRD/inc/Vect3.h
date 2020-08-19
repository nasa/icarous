/*
 * Vect3.h
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 *
 * 3-D vectors.
 *
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef VECT3_H_
#define VECT3_H_

#include "Vect2.h"
#include <string>

namespace larcfm {

/**
 * 3-Dimensional mathematical vectors.
 */
class Vect3 {

private:

	static Vect3 makeInvalid3();

public:

	// This class should be immutable and therefore
	// data members should be "const."/ However, if
	// they were const, the there cannot be an
	// assignment operator interfers with the default
	// assignment operator.  Hopefully, no one
	// will take advantage of the non-constness of
	// these members.

	/** x coordinate */
	double x;
	/** y coordinate */
	double y;
	/** z coordinate */
	double z;

	/**
	 * Creates a new vector with coordinates (<code>x</code>,<code>y</code>).
	 *
	 * @param xx Real value
	 * @param yy Real value
	 * @param zz Real value
	 */
	Vect3(const double xx=0, const double yy=0, const double zz=0);

	/**
	 * Creates a new vector whose x and y components are v, and whose z component is vz.
	 *
	 * @param v a 2-D Vector
	 * @param vz the z component of the new vector
	 */
	Vect3(const Vect2& v, const double vz);

	static Vect3 mk(double x, double y, double z);

	/** Destructor */
	virtual ~Vect3() { }

	/**
	 * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>) in specified units.
	 *
	 * @param x  Real value [ux]
	 * @param ux Units x
	 * @param y  Real value [uy]
	 * @param uy Units y
	 * @param z  Real value [uz]
	 * @param uz Units z
	 * @return a new vector
	 */
	static Vect3 makeXYZ(double x, std::string ux, double y, std::string uy, double z, std::string uz);

	/**
	 * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>) in internal units.
	 *
	 * @param x Real value [internal units]
	 * @param y Real value [internal units]
	 * @param z Real value [internal units]
	 * @return a new vector
	 */
	static Vect3 mkXYZ(double x, double y, double z);

	/**
	 * Creates a new vector with coordinates (<code>x</code>,<code>y</code>,<code>z</code>) in internal units.
	 *
	 * @param x Real value [NM]
	 * @param y Real value [NM]
	 * @param z Real value [ft]
	 * @return a new vector
	 */
	static Vect3 make(double x, double y, double z);

	static Vect3 make(double x, const std::string& xunits, double y, const std::string& yunits, double z, const std::string& zunits);


	Vect3 mkX(double nx);

	Vect3 mkY(double ny);

	Vect3 mkZ(double nz);

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
	bool almostEquals(const Vect3& v) const;

	bool almostEquals2D(const Vect3& v, double horizEps) const;

	/**
	 * Checks if vectors are almost equal.
	 *
	 * @param v Vector
	 * @param maxUlps unit of least precision
	 *
	 * @return <code>true</code>, if <code>this</code> vector is almost equal
	 * to <code>v</code>.
	 */
	bool almostEquals(const Vect3& v, INT64FM maxUlps) const;

	bool within_epsilon(const Vect3& v2, double epsilon) const;

	/**
	 * Vector addition.
	 *
	 * @param v Vector
	 *
	 * @return the vector addition of <code>this</code> vector and <code>v</code>.
	 */
	Vect3  operator + (const Vect3& v) const;
	/**
	 * Vector subtraction.
	 *
	 * @param v Vector
	 *
	 * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
	 */
	Vect3  operator - (const Vect3& v) const;
	/**
	 * Vector negation.
	 *
	 * @return the vector negation of of <code>this</code> vector.
	 */
	Vect3  operator - () const;
	/**
	 * Scalar multiplication.
	 *
	 * @param k Real value
	 *
	 * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
	 */
	Vect3  operator * (const double k) const;

	/**
	 * Dot product.
	 *
	 * @param v Vector
	 *
	 * @return the dot product of <code>this</code> vector and <code>v</code>.
	 */
	double operator * (const Vect3& v) const; // Dot product

	/** Exact equality */
	bool operator == (const Vect3& v) const;

	/** Is any component not exactly equal */
	bool operator != (const Vect3& v) const;

	/**
	 * Scalar and addition multiplication. Compute: k*<code>this</code> + v
	 *
	 * @param k Real value
	 * @param v Vector
	 *
	 * @return the scalar multiplication <code>this</code> vector and <code>k</code>, followed by an
	 * addition to vector <code>v</code>.
	 */
	Vect3  ScalAdd(const double k, const Vect3& v) const;

	/**
	 * Addition and scalar multiplication.  Compute: this + k*<code>v</code>;
	 *
	 * @param k real value
	 * @param v vector
	 *
	 * @return the addition of <code>this</code> vector to <code>v</code> scaled by <code>k</code>.
	 */
	Vect3 AddScal(double k, const Vect3& v) const;


	/**
	 * Right perpendicular, z-component set to 0
	 *
	 * @return the right perpendicular of <code>this</code> vector, i.e., (<code>y</code>, <code>-x</code>).
	 */
	Vect3 PerpR() const;

	/**
	 * Left perpendicular, z-component set to 0
	 *
	 * @return the left perpendicular of <code>this</code> vector, i.e., (<code>-y</code>, <code>x</code>).
	 */
	Vect3 PerpL() const;

	/**
	 * Calculates position after t time units in direction and magnitude of velocity v
	 * @param v    velocity
	 * @param t    time
	 * @return the new position
	 */
	Vect3 linear(const Vect3& v, double t) const;

	/** Calculates position after moving distance d in the direction "track"
	 * @param track   the direction
	 * @param d       distance
	 * @return the new position (horizontal only)
	 */
	Vect3 linearByDist2D(double track, double d) const;

	/**
	 * 2-Dimensional projection.
	 *
	 * @return the 2-dimensional projection of <code>this</code>.
	 */
	Vect2 vect2() const;

	/**
	 * Make a unit vector from the current vector.  If it is a zero vector, then a copy is returned.
	 * @return the unit vector
	 */
	Vect3 Hat() const ;

	/**
	 * Cross product.
	 *
	 * @param v Vector
	 *
	 * @return the cross product of <code>this</code> vector and <code>v</code>.
	 */
	Vect3 cross(const Vect3& v) const ;

	bool parallel(const Vect3& v) const ;

	/**
	 * Vector addition.
	 *
	 * @param v Vector
	 *
	 * @return the vector addition of <code>this</code> vector and <code>v</code>.
	 */
	Vect3  Add(const Vect3& v) const;

	/**
	 * Vector subtraction.
	 *
	 * @param v Vector
	 *
	 * @return the vector subtraction of <code>this</code> vector and <code>v</code>.
	 */
	Vect3  Sub(const Vect3& v) const;

	/**
	 * Vector negation.
	 *
	 * @return the vector negation of of <code>this</code> vector.
	 */
	Vect3  Neg() const;

	/**
	 * Scalar multiplication.
	 *
	 * @param k Real value
	 *
	 * @return the vector scalar multiplication of <code>this</code> vector and <code>k</code>.
	 */
	Vect3  Scal(double k) const;

	/**
	 * Dot product.
	 *
	 * @param x Real value
	 * @param y Real value
	 * @param z Real value
	 *
	 * @return the dot product of <code>this</code> vector and (<code>x</code>,<code>y</code>,<code>z</code>).
	 */
	double dot(const double x, const double y, const double z) const;

	/**
	 * Dot product.
	 *
	 * @param v Vector
	 *
	 * @return the dot product of <code>this</code> vector and <code>v</code>.
	 */
	double dot(const Vect3& v) const;

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

	double det2D(const Vect3& v) const;

	double dot2D(const Vect3& v) const;

	double sqv2D() const;

	double norm2D() const;

	Vect3 Hat2D() const;

	/**
	 * Cylindrical norm.
	 * @param d Radius of cylinder
	 * @param h Half-height of cylinder
	 * @return the cylindrical distance of <code>this</code>. The cylindrical distance is
	 * 1 when <code>this</code> is at the boundaries of the cylinder.
	 */
	double cyl_norm(const double d, const double h) const;

	/** The horizontal distance between this vector and the given vector, essentially same as v.Sub(w).vect2().norm()
	 * @param w vector
	 * @return horizontal distance */
	double distanceH(const Vect3& w) const;

	/** The vertical distance between this vector and the given vector, essentially same as v.z - w.z
	 * @param w vector
	 * @return vertical distance */
	double distanceV(const Vect3& w) const;

	/** A string representation of this vector */
	std::string toString() const;

	/** A string representation of this vector 
	 * @param precision number of digits of precision
	 * @return a string */
	std::string toString(int precision) const;

	/** A string representation of this vector */
	std::string toStringNP(const std::string& xunit, const std::string& yunit, const std::string& zunit) const;

	/** A string representation of this vector */
	std::string toStringNP(const std::string& xunit, const std::string& yunit, const std::string& zunit, int precision) const;

	std::string formatXYZ(const std::string& pre, const std::string& mid, const std::string& post) const;

	std::string formatXYZ(int prec, const std::string& pre, const std::string& mid, const std::string& post) const;

	std::string toPVS() const;

	std::string toPVS(int precision) const;

	/** 3-D time of closest point of approach
	 * if time is negative or velocities are parallel returns 0
	 * @param so position of one
	 * @param vo velocity of one
	 * @param si position of two
	 * @param vi velocity of two
	 * @return time of closest point of approach
	 */
	static double tcpa(const Vect3& so, const Vect3& vo, const Vect3& si, const Vect3& vi);

	/**
	 * Check if the current vector is valid
	 * @return true if the current vector has an "invalid" value
	 */
	bool isInvalid() const;

	/** Return a zero vector */
	static const Vect3& ZERO();

	/** An invalid vector, used for error reporting */
	static const Vect3& INVALID();

	static Vect3 parse(const std::string& s);

};


}

#endif /* VECT3_H_ */
