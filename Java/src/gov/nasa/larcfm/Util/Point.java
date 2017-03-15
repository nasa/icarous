/*
 * Point.java 
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.Arrays;
import java.util.List;
import java.util.ArrayList;

/**
 * This class represents a point or a 3-dimensional vector.  This class
 * contains methods to make the printing of Vect3's be more appropriate for
 * a position (units of length), instead of the abstract Vect3 which has
 * no natural units.  
 *
 */
public final class Point extends Vect3 implements OutputList {

	/**
	 * Instantiates a new point in internal units.
	 * 
	 * @param x the x
	 * @param y the y
	 * @param z the z
	 */
	private /*protected*/ Point(double x, double y, double z) {
		super(x,y,z);
	} 

	/** A zero point */
	public final static Point ZERO = new Point(0.0,0.0,0.0);

	/** An invalid point.  Note that this is not necessarily equal to other invalid point -- use the isInvalid() test instead. */
	public final static Point INVALID = new Point(Double.NaN,Double.NaN,Double.NaN);

	/**
	 * New point from Vect3.
	 * 
	 * @param v the 3-D point vector (in internal units).
	 * 
	 * @return the point
	 */
	public static Point mk(Vect3 v) {
		return new Point(v.x,v.y,v.z);
	}     

	/**
	 * New point from Vect2 (setting the altitude to 0.0).
	 * 
	 * @param v the 2-D point vector (in internal units).
	 * @param alt the altitude of this point
	 * 
	 * @return the point
	 */
	public static Point mk(Vect2 v, double alt) {
		return new Point(v.x,v.y,alt);
	}     

	/**
	 * New point from Vect3.
	 * 
	 * @param v the 3-D point vector (in internal units).
	 * 
	 * @return the point
	 */
	public static Point mk(Vect2 v) {
		return mk(v,0.0);
	}     

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
	public static Point mk(double x, double y, double z) {
		return new Point(x,y,z);
	}     

	/**
	 * New point from Euclidean coordinates in conventional units.
	 *
	 * @param x the x-point [NM]
	 * @param y the y-point [NM]
	 * @param z the z-point [ft]
	 *
	 * @return the point
	 */
	public static Point make(double x, double y, double z) {
		return make(x,"NM",y,"NM",z,"ft");
	}     

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
	public static Point make(double x, String unitsX, double y, String unitsY, double z, String unitsZ) {
		return new Point(Units.from(unitsX,x),Units.from(unitsY,y),Units.from(unitsZ,z));
	}     

	/**
	 * New point from existing point, changing only the X
	 * @param X new X [internal units]
	 * @return new point
	 */
	public Point mkX(double X) {
		return mk(X, y, z);
	}

	/**
	 * New point from existing point, changing only the Y
	 * @param Y new Y [internal units]
	 * @return new point
	 */
	public Point mkY(double Y) {
		return mk(x, Y, z);
	}

	/**
	 * New point from existing point, changing only the Z
	 * @param Z new Z [internal units]
	 * @return new point
	 */
	public Point mkZ(double Z) {
		return mk(x, y, Z);
	}

	public Point Hat() {
		// This method means:
		//    return make(this.Hat());
		// but for efficiency, I am implementing it explicitly
		double n = norm();
		if ( n == 0.0) { // this is only checking the divide by zero case, so an exact comparison is correct.
			return this;
		}
		return mk(x / n, y / n, z / n);
	}

	public Point Neg() {
		return mk(-x, -y, -z);
	}

	public Point Add(Point v) {
		return mk(x+v.x, y+v.y, z+v.z);
	}

	public Point Sub(Point v) {
		return mk(x-v.x, y-v.y, z-v.z);
	}

	public Point Scal(double k) {
		//	    Vect3 u = new Vect3(x,y,z);
		//	    u.scal(k);
		//	    return u;
		return mk(k*x, k*y, k*z);
	}

	/**
	 * Calculates position after t time units in direction and magnitude of velocity v
	 * @param v    velocity
	 * @param t    time
	 * @return the new position
	 */
	public Point linear(Velocity v, double t) {
		return mk(x + v.x*t,y + v.y*t,z +v.z*t);
	}

	// 
	// output methods
	//

	/** String representation of the point in [NM, NM, ft] */
	public String toString() {
		return toString(Constants.get_output_precision(),"NM","NM","ft");
	}

	/**
	 * String representation of the point in the given units
	 * @param prec precision (0-15)
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	public String toString(int prec, String xUnits, String yUnits, String zUnits) {
		return "("+f.FmPrecision(Units.to(xUnits, x()),prec)+", "+f.FmPrecision(Units.to(yUnits, y()),prec)+", "+f.FmPrecision(Units.to(zUnits, z()),prec)+")";
	}

	/** String representation with the given units 
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	public String toStringUnits(String xUnits, String yUnits, String zUnits) {
		return "("+Units.str(xUnits, x())+", "+Units.str(yUnits, y())+", "+Units.str(zUnits, z())+")";
	}

	/**
	 * Return an array of string representing each value of the point in the units [NM,NM,ft]
	 * @return array of strings
	 */
	public List<String> toStringList() {
		ArrayList<String> ret = new ArrayList<String>(3);
		ret.add(Double.toString(Units.to("NM", x())));
		ret.add(Double.toString(Units.to("NM", y())));
		ret.add(Double.toString(Units.to("ft", z())));	
		return ret;
	}

	/**
	 * Return an array of string representing each value of the point in the units [NM,NM,ft]
	 * @return array of strings
	 */
	public List<String> toStringList(int precision) {
		ArrayList<String> ret = new ArrayList<String>(3);
		ret.add(f.FmPrecision(Units.to("NM", x()), precision));
		ret.add(f.FmPrecision(Units.to("NM", y()), precision));
		ret.add(f.FmPrecision(Units.to("ft", z()), precision));	
		return ret;
	}

	/** String representation, 4 decimal places, without parentheses 
	 * @return a string representing this point
	 * */
	public String toStringNP() {
		return toStringNP(15,"NM","NM","ft");
	}

	/**
	 * String representation, with user-specified precision
	 * @param precision precision (0-15)
	 * @return a string representing this point
	 */
	public String toStringNP(int precision) {
		return toStringNP(precision,"NM","NM","ft");
	}

	/**
	 * String representation, with user-specified precision
	 * @param precision precision (0-15)
	 * @param xUnits units for the x value
	 * @param yUnits units for the y value
	 * @param zUnits units for the z value
	 * @return a string representing this point
	 */
	public String toStringNP(int precision, String xUnits, String yUnits, String zUnits) {
		return f.FmPrecision(Units.to(xUnits, x()), precision)+", "+f.FmPrecision(Units.to(yUnits, y()), precision)+", "+f.FmPrecision(Units.to(zUnits, z()), precision);	
	}

	/** 
	 * This parses a space or comma-separated string as a Point (an inverse to the toString 
	 * method).  If three bare values are present, then it is interpreted as the default units for 
	 * a point: [NM,NM,ft].  If there are 3 value/unit pairs then each values is interpreted with regard 
	 * to the appropriate unit.  If the string cannot be parsed, an INVALID value is
	 * returned. 
	 */
	public static Point parse(String str) {
		String[] fields = str.split(Constants.wsPatternParens);
		if (fields[0].equals("")) {
			fields = Arrays.copyOfRange(fields,1,fields.length);
		}
		try {
			if (fields.length == 3) {
				return make(
						Double.parseDouble(fields[0]),
						Double.parseDouble(fields[1]),
						Double.parseDouble(fields[2]));
			} else if (fields.length == 6) {
				return make(
						Double.parseDouble(fields[0]), Units.clean(fields[1]),
						Double.parseDouble(fields[2]), Units.clean(fields[3]),
						Double.parseDouble(fields[4]), Units.clean(fields[5]));
			}
		} catch (Exception e) {}
		return mk(Vect3.INVALID);	
	}

}
