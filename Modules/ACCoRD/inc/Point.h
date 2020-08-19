/*
 * Point.h
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2020 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef POINT_H_
#define POINT_H_

#include "Vect2.h"
#include "Vect3.h"
#include "Velocity.h"
#include "OutputList.h"
#include <string>

namespace larcfm {

/**
 * This class represents a point or a 3-dimensional vector.  This class
 * contains methods to make the printing of Vect3's be more appropriate for
 * a position (units of length), instead of the abstract Vect3 which has
 * no natural units.  
 *
 */
class Point: public Vect3, OutputList {

public:
	/**
	 * Instantiates a new point in internal units.
	 * 
	 * @param x the x
	 * @param y the y
	 * @param z the z
	 */
	Point(double x, double y, double z);


	/** Return a zero point vector */
	static const Point& ZEROP();

	/** An invalid point, used for error reporting */
	static const Point& INVALIDP();

	/**
	 * New point from Vect3.
	 * 
	 * @param v the 3-D point vector (in internal units).
	 * 
	 * @return the point
	 */
	static Point mk(const Vect3& v);

	/**
	 * New point from Vect2 (setting the altitude to 0.0).
	 * 
	 * @param v the 2-D point vector (in internal units).
	 * @param alt the altitude in internal units
	 * 
	 * @return the point
	 */
	static Point mk(const Vect2& v, double alt = 0.0);

	/**
	 * New point from Euclidean coordinates in internal units.
	 * 
	 * @param x the x-point [internal]
	 * @param y the y-point [internal]
	 * @param z the z-point [internal]
	 * 
	 * @return the point
	 * 
	 */
	static Point mk(double x, double y, double z);

	/**
	 * New point from Euclidean coordinates in conventional units.
	 *
	 * @param x the x-point [NM]
	 * @param y the y-point [NM]
	 * @param z the z-point [ft]
	 *
	 * @return the point
	 */
	static Point make(double x, double y, double z);

	/**
	 * New point from Euclidean coordinates in the given units.
	 *
	 * @param x the x-point
	 * @param unitsX the units for x 
	 * @param y the y-point 
	 * @param unitsY the units for y
	 * @param z the z-point 
	 * @param unitsZ the units for z
	 *
	 * @return the point
	 */
	static Point make(double x, const std::string& unitsX, double y, const std::string& unitsY, double z, const std::string& unitsZ);

	/**
	 * New point from existing point, changing only the X
	 * @param X new X [internal units]
	 * @return new point
	 */
	Point mkX(double X) const;

	/**
	 * New point from existing point, changing only the Y
	 * @param Y new Y [internal units]
	 * @return new point
	 */
	Point mkY(double Y) const;

	/**
	 * New point from existing point, changing only the Z
	 * @param Z new Z [internal units]
	 * @return new point
	 */
	Point mkZ(double Z) const;

	Point Hat() const;

	Point Neg() const;

	Point Add(const Point& v) const;

	Point Sub(const Point& v) const;

	Point Scal(double k) const;

	/**
	 * Calculates position after t time units in direction and magnitude of velocity v
	 * @param v    velocity
	 * @param t    time
	 * @return the new position
	 */
	Point linear(const Velocity& v, double t) const;

	// 
	// output methods
	//

	/** String representation of the point in [NM, NM, ft] */
	std::string toString() const;

	/**
	 * String representation of the point in the given units
	 * @param prec precision (0-15)
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	std::string toString(int prec, const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const;

	/** String representation with the given units 
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	std::string toStringUnits(const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const;

	/**
	 * Return an array of string representing each value of the point in the units [NM,NM,ft]
	 * @return array of strings
	 */
	std::vector<std::string> toStringList() const;

	/** String representation, 4 decimal places, without parentheses 
	 * @return a string representing this point
	 * */
	std::string toStringNP() const;

	/**
	 * String representation, with user-specified precision
	 * @param precision precision (0-15)
	 * @return a string representing this point
	 */
	std::string toStringNP(int precision) const;

	/**
	 * String representation, with user-specified precision
	 * @param precision precision (0-15)
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	std::string toStringNP(int precision, const std::string& xUnits, const std::string& yUnits, const std::string& zUnits) const;

	/** 
	 * This parses a space or comma-separated string as a Point (an inverse to the toString 
	 * method).  If three bare values are present, then it is interpreted as the default units for 
	 * a point: [NM,NM,ft].  If there are 3 value/unit pairs then each values is interpreted with regard 
	 * to the appropriate unit.  If the string cannot be parsed, an INVALID value is
	 * returned. 
	 * 
	 * @param str string to parse
	 * @return point
	 */
	static Point parse(const std::string& str);

};
}


#endif /* POINT_H_ */
