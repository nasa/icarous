/*
 * Copyright (c) 2014-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.Arrays;

/**
 * Euclidean space representation of polygon with height info.
 * This extends Vect3 (temporarily) in order to take advantage to polymorphism of detection algorithms.
 * If treated as a normal Vect3 point, this ignores the polygon information.
 * Only functions that are aware of Poly3Ds will have access to its full definition.
 */
public class Poly3D { // extends Point {

	Poly2D p2d = Poly2D.ZERO;
	double top = 0;
	double bottom = 0;
//	private boolean centroidDefined = false;
	Vect3 cpos = Vect3.ZERO; // hide the final version!

//	void calcCentroid() {
//		cpos = new Vect3(p2d.planarCentroid(), (top-bottom)/2);
//		centroidDefined = true;
//	}
	
	public Poly3D() {
		//super(x, y, z);
		p2d = new Poly2D();
	}

	public Poly3D(Poly2D v, double b, double t) {
		//super(v.x, v.y, (t+b)/2.0);
		p2d = new Poly2D(v);
		top = t;
		bottom = b;
	}

//	public Poly3D(Vect3 v) {
//		//super(v.x,v.y,v.z);
//		p2d = new Poly2D();
//	}
	
	public Poly3D(Poly3D p) {
		//super(p.x,p.y,p.z);
		p2d = new Poly2D(p.p2d);
		top = p.top;
		bottom = p.bottom;
//		centroidDefined = p.centroidDefined;
		cpos = p.cpos;
	}

	public Poly2D poly2D() {
		return p2d;
	}

	public void addVertex(Vect2 v) {
//		centroidDefined = false;
		p2d.addVertex(v);		
	}

//	public void setVertex(int i, Vect2 v) {
//		centroidDefined = false;
//		p2d.setVertex(i,v);
//	}
//
	
	public Vect2 getVertex(int i) {
		return p2d.getVertex(i);
	}

	public int size() {
		return p2d.size();
	}

	public double getTop() {
		return top;
	}

	public void setTop(double t) {
//		centroidDefined = false;
		top = t;
	}

	public double getBottom() {
		return bottom;
	}

	public void setBottom(double b) {
//		centroidDefined = false;
		bottom = b;
	}

	public Vect3 centroid() {
//		if (!centroidDefined) calcCentroid();
		return new Vect3(p2d.centroid(), (top-bottom)/2);
	}

	public Vect3 averagePoint() {
		return new Vect3(p2d.averagePoint(), (top-bottom)/2);
	}
	
	public String toString() {
		return /*super.toString()+" "+*/ p2d+" bot="+bottom+" top="+top;
	}

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	public boolean contains(Vect3 v) {
		if (v.z > top || v.z < bottom) return false;
		return p2d.contains(v.vect2());
	}

	/**
	 * Return true if p is a subset of this polygon.
	 * This uses standard raycasting checks for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	public boolean contains(Poly3D p) {
		if (p.top > top || p.bottom < bottom) return false;
		return p2d.contains(p.poly2D());
	}
	
	
	public double distanceFromEdge(Vect3 v3) {
			Vect3 cl = VectFuns.closestPointOnSegment(new Vect3(getVertex(size()-1),bottom), new Vect3(getVertex(0), bottom), v3);
			double dist = v3.distanceH(cl);
			for (int i = 0; i < size()-1; i++) {
				cl = VectFuns.closestPointOnSegment(new Vect3(getVertex(i),bottom), new Vect3(getVertex(i+1),bottom), v3);
				dist = Util.min(v3.distanceH(cl), dist);				
			}
			return dist;
	}
	
	public Poly3D reverseOrder() {
		return new Poly3D(p2d.reverseOrder(), bottom, top);
	}
	
//	public String toString(String xunit, String yunit, String zunit) {
//		return /*super.toStringUnits(xunit,yunit,zunit)+" "+*/ p2d.toString(xunit,yunit)+" bot="+Units.to(zunit, bottom)+" top="+Units.to(zunit, top);
//	}

//	/** 
//	 * This parses a space or comma-separated string as a Vect3 (an inverse to the toString method).  If three 
//	 * bare values are present, then it is interpreted as internal units.
//	 * If there are 3 value/unit pairs then each values is interpreted wrt the appropriate unit.  If the string 
//	 * cannot be parsed, an INVALID value is returned. 
//	 * */
//	public static aPolygon3D parse(String str) {
//		String[] fields = str.split(Constants.wsPatternParens);
//		if (fields[0].equals("")) {
//			fields = Arrays.copyOfRange(fields,1,fields.length);
//		}
//		try {
//			if (fields.length == 3) {
//				return new aPolygon3D(Double.parseDouble(fields[0]),Double.parseDouble(fields[1]),Double.parseDouble(fields[2]));
//			} else if (fields.length == 6) {
//				return new aPolygon3D(Units.from(Units.clean(fields[1]),Double.parseDouble(fields[0])),
//						Units.from(Units.clean(fields[3]),Double.parseDouble(fields[2])),
//						Units.from(Units.clean(fields[5]),Double.parseDouble(fields[4])));
//			}
//		} catch (Exception e) {}
//		return aPolygon3D.INVALID;
//	}
//

	
}
