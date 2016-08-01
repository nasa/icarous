/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;

import java.util.ArrayList;


/**
 * A bounding rectangle for a 3-dimensional polygon. The bounding rectangle is the smallest rectangle that encloses a set 
 * of points. A point is contained in the rectangle if it lies within its boundary or on one of its edges. 
 * Thus, the bounding rectangle includes its boundary.<p>
 * 
 * Special processing is conducted for LatLonAlt objects (including those emmbedded in Position objects.
 * The special processing includes (1) altitude information is captured as a bound, but not used
 * in the "contains" operation, and (2) proper handling when the longitude "wraps around" at -180/180.
 * This second special processing means that a bounding rectangle for LatLonAlt is limited to 180 degrees
 * (half the earth).  Very erratic results will come from a bounding rectangle that has both LatLonAlt points
 * and normal euclidean points.<p>
 * 
 * This probably should have been called BoundingBox, because it is three dimensional.
 */
public final class BoundingRectangle { 

	private double xMin;
	private double xMax;
	private double yMin;
	private double yMax;
	private double zMin;
	private double zMax;
	private double xCenter;  // only used for LatLonAlt objects

	/** Create a bounding rectangle with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
	public BoundingRectangle() {
		clear();
	}	
	
	/** Create a bounding rectangle with the list of given points.
	 */
	public BoundingRectangle(ArrayList<Vect2> vertices) {
		clear();
		for (int i = 0; i < vertices.size(); i++) {
			add(vertices.get(i).x,vertices.get(i).y);
		}
	}	

	/** Copy a bounding rectangle from an existing bounding rectangle
	 */
	public BoundingRectangle(BoundingRectangle br) {  //copy
		if (br != null) {
			xMin = br.xMin;
			xMax = br.xMax;
			yMin = br.yMin;
			yMax = br.yMax;
			zMin = br.zMin;
			zMax = br.zMax;
			xCenter = br.xCenter;
		} else {
			clear();
		}
	}

	/** Reset this bounding rectangle to a structure with invalid values.  When the
	 * first point is added, valid values are obtained.
	 */
	public void clear() {
		xMin = Double.MAX_VALUE;
		xMax = -Double.MAX_VALUE;
		yMin = Double.MAX_VALUE;
		yMax = -Double.MAX_VALUE;
		zMin = Double.MAX_VALUE;
		zMax = -Double.MAX_VALUE;
		
		xCenter = Double.MAX_VALUE; // initially I used NaN, but this was slow.
	}

	/**
	 * Add a point to this bounding rectangle.
	 * @param x
	 * @param y
	 */
	public void add(double x, double y) {
		//f.pln("min "+x+" "+xMin);
		xMin = Math.min(x,xMin);
		yMin = Math.min(y,yMin);
		xMax = Math.max(x,xMax);
		yMax = Math.max(y,yMax);
	}

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v
	 */
	public void add(Vect2 v) {
		if (v == null) return;
		add(v.x,v.y);
	}
	
	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param x
	 * @param y
	 * @param z
	 */
	public void add(double x, double y, double z) {
		add(x,y);
		zMin = Math.min(z,zMin);
		zMax = Math.max(z,zMax);
	}

	/**
	 * Add a point to this bounding rectangle.
	 * 
	 * @param v
	 */
	public void add(Vect3 v) {
		if (v == null) return;
		add(v.x,v.y,v.z);
	}
	
	/**
	 * Add another bounding rectangle to this bounding rectangle.
	 * 
	 * @param br the other bounding rectangle (this rest is not changed).
	 */
	public void add(BoundingRectangle br) {
		if (br == null) return;
		
		if (xCenter == Double.MAX_VALUE) {
			LatLonAlt ur = LatLonAlt.mk(br.getMaxY(), br.getMaxX(), br.getMaxZ());
			LatLonAlt ll = LatLonAlt.mk(br.getMinY(), br.getMinX(), br.getMinZ());
			add(ur);
			add(ll);
		} else {
			add(br.getMaxX(),br.getMaxY(),br.getMaxZ());
			add(br.getMinX(),br.getMinY(),br.getMinZ());
		}
	}
	
	private double fix_lon(double lon) {
		if (xCenter == Double.MAX_VALUE) {
			xCenter = lon;
		}
		
		if (Math.abs(lon - xCenter) > Math.PI) {
			if ((lon - xCenter > Math.PI)) {
				return -Math.PI + (lon - Math.PI);
			} else { 
				return Math.PI + (lon + Math.PI);
			}
		}
		return lon;
	}

	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param lla
	 */
	public void add(LatLonAlt lla) {
		if (lla == null) return;
		lla = lla.normalize();
		
		if (xCenter == Double.MAX_VALUE) {
			xCenter = lla.lon();
		}
		
		LatLonAlt nlla = denormalize(lla);
		add(nlla.lon(),nlla.lat(),nlla.alt());
	}
	
	/**
	 * Add a point to this bounding rectangle.
	 * Note that when adding LatLonAlt points, great circle paths may fall outside the defined bounding rectangle!
	 * 
	 * @param p
	 */
	public void add(Position p) {
		if (p == null) return;
		if (p.isLatLon()) {
			add(p.lla()); //p.lon(),Util.to_pi2_cont(p.lat()),p.alt());
		} else {
			add(p.x(),p.y(),p.z());
		}
	}
	
	/**
	 * Return a LatLonAlt object that is consistent with this bounding rectangle.  This LatLonAlt object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * @param lla a LatLonAlt object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	public LatLonAlt denormalize(LatLonAlt lla) {
		lla = lla.normalize();
		return LatLonAlt.mk(lla.lat(),fix_lon(lla.lon()),lla.alt());
	}

	/**
	 * Return a Position object that is consistent with this bounding rectangle.  If this bounding
	 * rectangle contains latitude/longitude points, then the returned Position object
	 * may have a longitude greater than 180 degrees or less than -180 degrees.
	 * 
	 * @param p a Position object
	 * @return a LatLonAlt object with (possibly) non-standard longitude.
	 */
	public Position denormalize(Position p) {
		if (p.isLatLon()) {
			return new Position(denormalize(p.lla()));
		} else {
			return p;
		}
	}

	/**
	 * Determines if the given point is within the bounding rectangle. A point on
	 * the edges of this rectangle is also within the rectangle.
	 * 
	 * @param x the x coordinate of the point in question
	 * @param y the y coordinate of the point in question
	 * @return true if the point is within the bounding rectangle or on any of its
	 *         edges.
	 */
	public boolean contains(double x,  double y) {
		//f.pln("x>=xMin: "+(x>=xMin));
		//f.pln("x<=xMax: "+(x<=xMax));
		//f.pln("y>=yMin: "+(y>=yMin));
		//f.pln("y<=yMax: "+(y<=yMax));
		return (x >= xMin && x <= xMax && y >= yMin && y <= yMax);
	}

	public boolean contains(Vect2 v) {
		return contains(v.x,v.y);
	}
	
	/** Is this Vect3 in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param v Vect3
	 * @return true if the point is contained
	 */
	public boolean contains(Vect3 v) {
		return contains(v.x,v.y);
	}
	
	/** Is this position in the contained area?  (using only the X and Y coordinates)
	 * 
	 * @param p a position
	 * @return true if the point is contained
	 */
	public boolean contains(Position p) {
		if (p.isLatLon()) {
			return contains(p.lla());
		} else {
			return contains(p.x(), p.y());
		}
	}
	
	/** Is this LatLonAlt in the contained area?  (using only the latitude and longitude coordinates)
	 * 
	 * @param lla a position
	 * @return true if the point is contained
	 */
	public boolean contains(LatLonAlt lla) {
		return contains(fix_lon(lla.lon()), lla.lat());
	}
	
	/**
	 * Does the given rectangle share any points with this rectangle?  Only two 
	 * dimensions are used.
	 * 
	 * @param rect the BoundingRectangle to be tested for intersection
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	public boolean intersects(BoundingRectangle rect) {
		return intersects(rect, 0.0);
	}

	/**
	 * Return true if any point in rect is within buffer of any point in this bounding rectangle. Only two dimensions are used
	 * @return true, if the supplied BoundingRectangle intersects this one
	 */
	public boolean intersects(BoundingRectangle rect, double buffer) {
		return ! (xMax+buffer < rect.getMinX() || xMin-buffer > rect.getMaxX() || yMax+buffer < rect.getMinY() || yMin-buffer > rect.getMaxY());
//		if (xMax+buffer < rect.getMinX() || xMin-buffer > rect.getMaxX() || yMax+buffer < rect.getMinY() || yMin-buffer > rect.getMaxY()) {
//		return false;
//	}
//	return true;
	}

	/**
	 * @return left bound
	 */
	public double getMinX() {
		return xMin;
	}

	/**
	 * @return right bound
	 */
	public double getMaxX() {
		return xMax;
	}

	/**
	 * @return bottom bound
	 */
	public double getMinY() {
		return yMin;
	}

	/**
	 * @return top bound
	 */
	public double getMaxY() {
		return yMax;
	}

	/**
	 * @return min Z bound
	 */
	public double getMinZ() {
		return zMin;
	}

	/**
	 * @return max Z bound
	 */
	public double getMaxZ() {
		return zMax;
	}
	
	public String toString() {
		String rtn = "<"+xMin+" "+xMax+" "+yMin+" "+yMax+">";
		return rtn;			
	}

}
