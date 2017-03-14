/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

package gov.nasa.larcfm.Util;


import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;

/**
 * A basic polygon that describes a volume.  This volume has a flat bottom and top 
 * (specified as altitude values).  Points describe the cross-section area vertices in
 * a consistently clockwise (or consistently counter-clockwise) manner.  The cross-section
 * need not be convex, but an "out of order" listing of the vertices will result in edges 
 * that cross, and will cause several calculations to fail (with no warning).
 * 
 * A SimplePoly sets the altitude for all its points to be the _bottom_ altitude,
 * while the top is stored elsewhere as a single value.  The exact position for "top"
 * vertices is computed on demand.
 * 
 * The cross-section must be a simple polygon, that is it allows for non-convex areas, but
 * vertices and edges may not overlap or cross.  Vertices may be ordered in either a clockwise
 * or counterclockwise manner.
 * 
 * (A vertex-complete polygon allows for vertices and edges to overlap but not cross, while
 * a general polygon allows for edges to cross.)
 * 
 * Point indices are based on the order they are added.
 * 
 * Note: polygon support is experimental and the interface is subject to change!
 *
 */
public class SimplePoly {
	ArrayList<Position> points = new ArrayList<Position>(6);
	boolean bottomTopSet = false;
	double top = 0;
	double bottom = 0;

	boolean boundingCircleDefined = false;
	boolean centroidDefined = false;
	boolean averagePointDefined = false;

	Position cPos;
	Position bPos;
	Position aPos;
	
	double clockwiseSum = 0; // sum of all angles in order (measured in a clockwise manner), if (approx) 2PI then it is clockwise, , if (approx) -2PI, then counterclockwise; 

	/**
	 * Constructs an empty SimplePoly.
	 */
	public SimplePoly() {
		cPos = Position.ZERO_LL;
	}

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	public SimplePoly(double b, double t) {
		bottomTopSet = true;
		top = t;
		bottom = b;
		cPos = Position.ZERO_LL;
	}

	/**
	 * Constructor for a SimplePoly with predefined top and bottom altitudes.
	 * 
	 * @param b Bottom altitude
	 * @param t Top Altitude
	 */
	public SimplePoly(double b, double t, String units) {
		bottomTopSet = true;
		top = Units.from(units,t);
		bottom = Units.from(units,b);
		cPos = Position.ZERO_LL;
	}


	/**
	 * Create a deep copy of a SimplePoly
	 * 
	 * @param p Source poly.
	 */
	public SimplePoly(SimplePoly p) {
		bottomTopSet = p.bottomTopSet;
		bottom = p.bottom;
		top = p.top;
		cPos = p.cPos;
		for(int i = 0; i < p.size(); i++) {
			addVertex(p.getVertex(i));
		}
	}

	public boolean equals(SimplePoly p) {
		boolean ret = bottom == p.bottom && top == p.top;
		if (size() != p.size()) return false;
		for (int i = 0; i < size(); i++) {
			if (!getVertex(i).equals(p.getVertex(i))) return false; 
		}
		return ret;
	}

	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates .
	 */
	public static SimplePoly make(Poly3D p3) {
		SimplePoly sp = new SimplePoly();
		sp.setBottom(p3.getBottom());
		sp.setTop(p3.getTop());
		for(int i = 0; i < p3.size(); i++) {
			Position v = new Position(new Vect3(p3.getVertex(i),p3.getBottom()));
			sp.addVertex(v);
		}
		return sp;
	}


	/**
	 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates .
	 */
	public static SimplePoly make(Poly3D p3, EuclideanProjection proj) {
		SimplePoly sp = new SimplePoly();
		sp.setBottom(p3.getBottom());
		sp.setTop(p3.getTop());
		for(int i = 0; i < p3.size(); i++) {
			Position v = new Position(proj.inverse(p3.getVertex(i), p3.getBottom()));
			sp.addVertex(v);
		}
		return sp;
	}


	/**
	 *  
	 * @return True if this SimplePoly contains any geodetic points.
	 */
	public boolean isLatLon() {
		return points.size() > 0 && points.get(0).isLatLon();
	}

	/**
	 * 
	 * @return Number of points in this SimplePoly
	 */
	public int size() {
		return points.size();
	}

	// standard formulas for area and centroid
	/**
	 * Return the area (in m^2 or rad^2) of this SimplePoly.
	 */
	// note: this may fail over poles!
	private double signedArea(double dx, double dy) {
		double a = 0;
		points.add(points.get(0));
		double xorig = points.get(0).x();
		for (int i = 0; i < points.size()-1; i++) {
			double x0 = points.get(i).x()-dx;
			double x1 = points.get(i+1).x()-dx;
			double y0 = points.get(i).y()-dy;
			double y1 = points.get(i+1).y()-dy;
			// correct for date line wrap around
			if (isLatLon()) {
				if (x0-xorig > Math.PI) {
					x0 = x0 - 2*Math.PI; 
				} else if (x0-xorig < -Math.PI) {
					x0 = x0 + 2*Math.PI;
				}
				if (x1-xorig > Math.PI) {
					x1 = x1 - 2*Math.PI; 
				} else if (x1-xorig < -Math.PI) {
					x1 = x1 + 2*Math.PI;
				}
			}
			a = a + (x0*y1 - x1*y0);
		}
		points.remove(points.size()-1);
		return 0.5*a;
	}


	// note: this may fail over poles!
	private void calcCentroid() {
		centroidDefined = true;
		if (size() < 2) {
			cPos = Position.INVALID;
			return;
		}
		// there can be numeric problems with these calculations if sides are small (multiplying small numbers loses precision)
		// shift points to around the origin (and then back) in an attempt to alleviate these.
		Position bbcent = getBoundingRectangle().centerPos();
		double dx = bbcent.x();
		double dy = bbcent.y();
		double a = signedArea(dx,dy);
		// if a point or line, use old method
		if (a == 0) {
			Vect2 v2 = new Vect2(0,0);
			for (int i = 0; i < points.size(); i++) {
				v2 = v2.Add(points.get(i).vect2());
			}
			v2 = v2.Scal(1.0/points.size());
			double z = (top+bottom)/2.0;
			if (points.get(0).isLatLon()) {
				LatLonAlt lla = LatLonAlt.mk(v2.y(), v2.x(), z);
				cPos = new Position(lla);
			} else {
				cPos = new Position(Point.mk(v2,z));
			}
		} else {
			double x = 0;
			double y = 0;
			points.add(points.get(0));
			double xorig = points.get(0).x();
			for (int i = 0; i < points.size()-1; i++) {
				double x0 = points.get(i).x()-dx;
				double x1 = points.get(i+1).x()-dx;
				// correct for date line wrap around
				if (isLatLon()) {
					if (x0-xorig > Math.PI) {
						x0 = x0 - 2*Math.PI; 
					} else if (x0-xorig < -Math.PI) {
						x0 = x0 + 2*Math.PI;
					}
					if (x1-xorig > Math.PI) {
						x1 = x1 - 2*Math.PI; 
					} else if (x1-xorig < -Math.PI) {
						x1 = x1 + 2*Math.PI;
					}
				}
				double y0 = points.get(i).y()-dy;
				double y1 = points.get(i+1).y()-dy;
				x = x + (x0+x1)*(x0*y1-x1*y0);
				y = y + (y0+y1)*(x0*y1-x1*y0);
			}
			points.remove(points.size()-1);
			x = x/(6*a)+dx;
			y = y/(6*a)+dy;
			cPos = points.get(0).mkX(x).mkY(y).mkZ((top+bottom)/2);
			if (isLatLon()) {
				cPos = new Position(LatLonAlt.normalize(cPos.lat(), cPos.lon(), cPos.alt()));
			}
		}
		if (!getBoundingRectangle().contains(cPos)) {
			// in latlon with small polygons this sometimes returns an incorrect value
//			f.pln("WARNING SimplePoly.calcCentroid has encountered a numeric error.  Returning averagePoint instead. "+boundingCircleRadius());
//			cPos = averagePoint();
			cPos = getBoundingRectangle().centerPos();
		}
	}

	//	@SuppressWarnings("unused")
	//	// if we have a more accurate bounding circle than one based on the centroid
	//	private void calcBoundingCircleCenter() {
	//		boundingCircleDefined = true;
	//		double maxD = 0.0;
	//		Position p1 = points.get(0);
	//		Position p2 = points.get(0);
	//		for (int i = 0; i < points.size(); i++) {
	//			Position p3 = points.get(i);
	//			Position p4 = maxDistPair(points.get(i));
	//			double d = p3.distanceH(p4);
	//			if (d > maxD) {
	//				maxD = d;
	//				p1 = p3;
	//				p2 = p4;
	//			}
	//		}
	//		double r = maxD/2;
	//		double f = 0.5;
	//		if (p1.isLatLon()) {
	//			bPos = new Position(GreatCircle.interpolate(p1.lla(),p2.lla(),f));
	//		} else {
	//			NavPoint np1 = new NavPoint(p1,0);
	//			NavPoint np2 = new NavPoint(p2,1);
	//			bPos = np1.linear(np1.initialVelocity(np2), 0.5).position();
	//		}
	//		
	//		// now move along the segment to the farthest from  the current cPos
	//		p2 = maxDistPair(bPos);
	//		double d = p2.distanceH(bPos);
	//		if (d > r) {
	//			f = (d - r)/d;	// fraction of distance along new segment
	//			if (p2.isLatLon()) {
	//				bPos = new Position(GreatCircle.interpolate(bPos.lla(),p2.lla(),f));
	//			} else {
	//				NavPoint np1 = new NavPoint(bPos,0);
	//				NavPoint np2 = new NavPoint(p2,1);
	//				bPos = np1.linear(np1.initialVelocity(np2), 0.5).position();
	//			}
	//		}
	//	}

	/**
	 * Return this centroid of this volume.
	 * Note: if sides are small (less than about 10^-5 in internal units), there may be errors in the centroid calculations
	 * @return The centroid position of this volume.
	 */
	public Position centroid() {
		if (!centroidDefined) {
			calcCentroid();
		}
		return cPos;
	}

	private Position avgPos(List<Position> points, List<Double> wgts) {
		// there can be numeric problems with these calculations if sides are small (multiplying small numbers loses precision)
		// shift points to around the origin (and then back) in an attempt to alleviate these.
		Position bbcent = getBoundingRectangle().centerPos();
		double dx = bbcent.x();
		double dy = bbcent.y();
		double x = 0;
		double y = 0;
		double xorig = points.get(0).x()-dx;
		double tot = 0;
		for (int i = 0; i < points.size(); i++) {
			double x0 = points.get(i).x()-dx;
			double y0 = points.get(i).y()-dy;
			// correct for date line wrap around
			if (isLatLon()) {
				if (x0-xorig > Math.PI) {
					x0 = x0 - 2*Math.PI; 
				} else if (x0-xorig < -Math.PI) {
					x0 = x0 + 2*Math.PI;
				}
			}
			double w = 1.0;
			if (wgts != null && wgts.size() == points.size()) {
				w = wgts.get(i);
			}
			x += x0 * w;
			y += y0 * w;
			tot += w;
		}
		tot = tot == 0.0 ? 1.0 : tot;
		if (isLatLon()) {
			x = Util.to_pi(x);
		}
		Position myaPos = points.get(0).mkX(dx+x/tot).mkY(dy+y/tot);
		return myaPos;
	}

	// note: this may fail over poles!
	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid!  This will, however, have the nice property of having
	 * a constant velocity when dealing with a morphing polygon. 
	 */
	public Position averagePoint() {
		if (!averagePointDefined) {
			aPos = avgPos(points, null).mkZ((top+bottom)/2);
			averagePointDefined = true;
		}	
		return aPos;
	}


	// returns the center of a bounding circle
	// calculates it and radius, if necessary 
	/**
	 * Returns the center of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
	 */
	public Position boundingCircleCenter() {
		return centroid();
		//		if (!boundingCircleDefined) {
		//			calcBoundingCircleCenter();
		//		}
		//		return bPos;
	}

	/** Returns true if this polygon is convex */
	public boolean isConvex() {
		boolean ret = true;
		if (points.size() > 2) {
			double a1 = points.get(points.size()-1).track(points.get(0));
			double a2 = points.get(0).track(points.get(1));
			int pdir = Util.turnDir(a1, a2); 
			for (int i = 0; i < points.size()-2; i++) {
				a1 = a2;
				a2 = points.get(i+1).track(points.get(i+2));
				int dir = Util.turnDir(a1, a2);;
				ret = ret && (pdir == 0 || dir == pdir);
				if (dir != 0) pdir = dir;
			}
		}
		return ret;
	}

	/**
	 * Returns the radius of a circle completely containing this SimplePoly.  This is not necessarily the minimal bounding circle.
	 */
	public double boundingCircleRadius() {
		return maxRadius();
		//		Position c = boundingCircleCenter();
		//		Position p = maxDistPair(c);
		//		return c.distanceH(p);
	}

	private Position maxDistPair(Position p) {
		double maxD = 0.0;
		Position p2 = p;
		for (int i = 0; i < points.size(); i++) {
			double d = points.get(i).distanceH(p);
			if (d > maxD) {
				p2 = points.get(i);
				maxD = d;
			}
		}
		return p2;
	}

	private int maxDistIdx(Position p) {
		double maxD = 0.0;
		int idx = -1;
		for (int i = 0; i < points.size(); i++) {
			double d = points.get(i).distanceH(p);
			if (d > maxD) {
				idx = i;
				maxD = d;
			}
		}
		return idx;		
	}

	/** 
	 * Returns the max horizontal distance between any vertex and the centroid
	 */
	public double maxRadius() {
		Position c = centroid();
		Position p = maxDistPair(c);
		//f.pln("SimplePoly,maxRadius "+this);
		//f.pln("SimplePoly,maxRadius c="+c+" p="+p+" dist="+Units.str("nmi", c.distanceH(p)));
		//Thread.dumpStack();
		//f.pln("---");
		return c.distanceH(p);
	}

	public double apBoundingRadius() {
		Position c = averagePoint();
		Position p = maxDistPair(c);
		return c.distanceH(p);
	}


	/** 
	 * Add a new point to the SimplePoly.  Points should be added in a consistently clockwise or consistently counter-clockwise manner. 
	 * This currently does NOT set the Z component of the point (unless it is the first point)
	 * Returns false if an error is detected (duplicate or invalid vertex), true otherwise. 
	 */
	public boolean addVertex(Position p){
		if (p == null || p.isInvalid()){ //|| points.contains(p)) {
			// error -- no duplicate points allowed
			//f.pln("SimplePoly.addVertex error: bad point "+p);			
			//			Debug.halt();
			return false;
		}
		clockwiseSum = 0;
		//		if (points.size() > 0 && (points.get(0).distanceH(p) < Constants.get_horizontal_accuracy() || points.get(points.size()-1).distanceH(p) < Constants.get_horizontal_accuracy())) {
		//			// error -- too close
		////f.pln("SimplePoly.addVertex error: too close "+p+" d1="+points.get(0).distanceH(p)+" d2="+points.get(points.size()-1).distanceH(p));			
		//			return false;
		//		}
		centroidDefined = false;
		boundingCircleDefined = false;
		averagePointDefined = false;
		if (points.size() < 1 && !bottomTopSet) {
			top = bottom = p.alt();
			bottomTopSet = true;
		}
		points.add(p.mkAlt(bottom));
		return true;
	}

	/**
	 * Remove a point from this SimplePoly.
	 * @param n Index (in order added) of the point to be removed.
	 */
	public void remove(int n) {
		centroidDefined = false;
		boundingCircleDefined = false;
		averagePointDefined = false;
		clockwiseSum = 0;
		if (n >= 0 && n < points.size()) {
			points.remove(n);
		} 
	}

	/**
	 * Change the position of a point already added to the SimplePoly, indicated by its index. 
	 * This currently does NOT set the Z component of the point.
	 * Returns false if an invalid vertex is detected, true otherwise 
	 */
	public boolean setVertex(int n, Position p) {
		if (p == null || p.isInvalid() || points.contains(p.mkAlt(bottom))) {
			// error -- no duplicate points allowed
			return false;
		}
		clockwiseSum = 0;
		centroidDefined = false;
		boundingCircleDefined = false;
		averagePointDefined = false;
		if (n >= 0 && n < points.size()) {
			points.set(n,p.mkAlt(bottom));
		} 
		return true;
	}


	/**
	 * Sets the top altitude of this SimplePoly.
	 * @param t New top altitude.
	 */
	public void setTop(double t) {
		centroidDefined = false;
		boundingCircleDefined = false;
		averagePointDefined = false;
		bottomTopSet = true;
		top = t;
	}

	/**
	 * Return the top altitude of this SimplePoly.
	 */
	public double getTop() {
		return top;
	}

	/**
	 * Sets the bottom altitude of this SimplePoly.
	 * @param b New bottom altitude.
	 */
	public void setBottom(double b) {
		centroidDefined = false;
		boundingCircleDefined = false;
		averagePointDefined = false;
		bottomTopSet = true;
		bottom = b;
		for (int i = 0; i < points.size(); i++) {
			points.set(i,points.get(i).mkAlt(bottom));
		}
	}

	/**
	 * Return the bottom altitude of this SimplePoly.
	 */
	public double getBottom() {
		return bottom;
	}

	/**
	 * Returns the position of the (bottom) point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 */
	public Position getVertex(int n) {
		if (n >= 0 && n < points.size()) {
			return points.get(n);
		} 
		return centroid();
	}

	public ArrayList<Position> getVertices() {
		return points;
	}
	
	/**
	 * Returns the position of the top point with index n.
	 * If n is not a valid index, this returns the centroid position.
	 */
	public Position getTopPoint(int n) {
		if (n >= 0 && n < points.size()) {
			return points.get(n).mkAlt(top);
		} 
		return centroid();	   
	}

	/**
	 * Returns a deep copy of this SimplPoly.
	 */
	public SimplePoly copy() {
		SimplePoly r = new SimplePoly(bottom,top);
		for(int i = 0; i < points.size(); i++) {
			//f.pln("copy "+i+" src point="+points.get(i));
			r.addVertex(points.get(i));
		}
		return r;
	}

	public boolean isClockwise() {
		if (Util.almost_equals(clockwiseSum, 0.0)) {
			for (int i = 0; i < points.size(); i++) {
				int h = i-1;
				int j = i+1;
				if (h < 0) h = points.size()-1;
				if (j == points.size()) j = 0;
				double trk1 = points.get(h).finalVelocity(points.get(i), 100).trk();
				double trk2 = points.get(i).initialVelocity(points.get(j), 100).trk();
				double angle = Util.to_pi(Util.turnDelta(trk1, trk2, true));
				clockwiseSum += angle;
			}
		}
		return clockwiseSum > 0.0;
	}

	/**
	 * closest horizontal distance from p to an edge.  
	 * @param p
	 * @return
	 */
	public double distanceFromEdge(Position p) {
		if (p.isLatLon()) {
			LatLonAlt ll = p.lla();
			LatLonAlt cl = GreatCircle.closest_point_segment(getVertex(size()-1).lla(), getVertex(0).lla(), ll);
			double dist = GreatCircle.distance(ll, cl);
			for (int i = 0; i < size()-1; i++) {
				cl = GreatCircle.closest_point_segment(getVertex(i).lla(), getVertex(i+1).lla(), ll);
				dist = Util.min(GreatCircle.distance(ll, cl), dist);				
			}
			return dist;
		} else {
			Vect3 v3 = p.point();
			Vect3 cl = VectFuns.closestPointOnSegment(getVertex(size()-1).point(), getVertex(0).point(), v3);
			double dist = v3.distanceH(cl);
			for (int i = 0; i < size()-1; i++) {
				cl = VectFuns.closestPointOnSegment(getVertex(i).point(), getVertex(i+1).point(), v3);
				dist = Util.min(v3.distanceH(cl), dist);				
			}
			return dist;
		}
	}

	public double distanceV(Position p) {
		if (p.isLatLon()) {
			return Util.min(Math.abs(p.alt()-top), Math.abs(p.alt()-bottom));
		} else {
			return Util.min(Math.abs(p.z()-top), Math.abs(p.z()-bottom));
		}

	}

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	public boolean contains(Position p) {
		EuclideanProjection proj = Projection.createProjection(p);
		Poly3D poly = poly3D(proj);
		if (p.isLatLon()) {
			return poly.contains(Vect3.ZERO);
		} else {
			return poly.contains(p.point());
		}
	}

	/**
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 */
	public boolean contains2D(Position p) {
		EuclideanProjection proj = Projection.createProjection(p);
		Poly2D poly = poly3D(proj).poly2D();
		if (p.isLatLon()) {
			return poly.contains(Vect2.ZERO);
		} else {
			return poly.contains(p.vect2());
		}
	}

	/**
	 * This moves the SimplePoly by the amount determined by the given (Euclidean) offset.
	 * @param off offset
	 */
	public void translate(Vect3 off) {
		Position c = centroid();
		if (isLatLon()) {
			EuclideanProjection proj = Projection.createProjection(c.lla());
			for (int i = 0; i < size(); i++) {
				setVertex(i, new Position(proj.project(points.get(i))));
			}
		}
		// translate
		for (int i = 0; i < size(); i++) {
			Position p = points.get(i);
			setVertex(i, p.mkX(p.x()+off.x()).mkY(p.y()+off.y()));
		}
		// shift back to latlon, if necessary
		if (isLatLon()) {
			EuclideanProjection proj = Projection.createProjection(c.lla());
			for (int i = 0; i < size(); i++) {
				setVertex(i, new Position(proj.inverse(points.get(i).point())));
			}
		}
		setTop(top+off.z());
		setBottom(bottom+off.z());
	}
	//	
	//	/** return a Poly3D version of this */
	//	public Poly3D poly3D(EuclideanProjection proj) {
	//		Poly3D p3;
	//		if (isLatLon()) {
	//			p3 = new Poly3D(proj.project(centroid()));
	//			for (Position p : points) {
	//				p3.addVertex(proj.project(p).vect2());
	//			}
	//		} else {
	//			p3 = new Poly3D(centroid().point());
	//			for (Position p : points) {
	//				p3.addVertex(p.point().vect2());
	//			}
	//		}
	//		p3.setBottom(getBottom());
	//		p3.setTop(getTop());
	//		return p3;
	//	}

	public SimplePoly linear(Velocity v, double t) {
		SimplePoly newPoly = new SimplePoly();
		int sz = size();
		for (int j = 0; j < sz; j++) {
			Position p = getVertex(j).linear(v,t);
			//f.pln("linear vj="+getVertex(j)+" v="+v+" t="+t+" p="+p);			  
			Position pt = getTopPoint(j).linear(v,t);
			newPoly.addVertex(p);
			newPoly.setBottom(p.z());
			newPoly.setTop(pt.z());
		}
		return newPoly;

	}

	/** return a aPolygon3D version of this.  proj is ignored if this is Euclidean */
	public Poly3D poly3D(EuclideanProjection proj) {
		Poly3D p3;
		if (isLatLon()) {
			p3 = new Poly3D();
			for (Position p : points) {
				//f.pln("SimplePoly.poly3D "+p+" -> "+proj.project(p));				
				p3.addVertex(proj.project(p).vect2());
			}
		} else {
			p3 = new Poly3D();
			for (Position p : points) {
				p3.addVertex(p.point().vect2());
			}
		}
		p3.setBottom(getBottom());
		p3.setTop(getTop());
		return p3;
	}



	public SimplePoly buildContainingPoly(int K) {
		SimplePoly s = new SimplePoly();
		for (int i = 0; i < size(); i = i + K) {
			s.addVertex(getVertex(i));
		}
		//f.pln(" $$ buildContainingPoly: s.size = "+s.size());
		ArrayList<Integer> lv = new ArrayList<Integer>(10);
		for (int i = 0; i < size(); i++) {
			if (i % K != 0) { 
				Position v = getVertex(i);
				if (!s.contains(v)) {
					//s.addVertex(v)
					lv.add(i);
				}
			}
		}	
		//f.pln(" $$ buildContainingPoly: lv.size = "+lv.size());

		SimplePoly q = new SimplePoly();
		for (int i = 0; i < size(); i++) {
			if (i % K == 0 || lv.contains(i)) {
				q.addVertex(getVertex(i));            	
			}
		}
		return q;
	}

	//	private class SRComparator implements Comparator<Pair<Position,Integer>> {
	//		@Override
	//		public int compare(Pair<Position,Integer> o1, Pair<Position,Integer> o2) {
	//			double d1 = centroid().distanceH(o1.first);
	//			double d2 = centroid().distanceH(o2.first);
	//			return Double.compare(d2, d1); // largest to smallest
	//		}
	//	}
	//
	//	private class SIComparator implements Comparator<Pair<Position,Integer>> {
	//		@Override
	//		public int compare(Pair<Position,Integer> o1, Pair<Position,Integer> o2) {
	//			return Integer.compare(o1.second, o2.second);
	//		}
	//	}

	//	private Position pushOut(Position p, double buffer) {
	//		Position cent = centroid();
	//		double dist = cent.distanceH(p);
	//		Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
	//		return p.linear(v, buffer);
	//	}
	//
	//	private Position pushOut(int i, double buffer) {
	//		Position p = getVertex(i);
	//		Position cent = centroid();
	//		double dist = cent.distanceH(p);
	//		Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
	//		int h = i-1;
	//		if (h < 0) {
	//			h = size()-1;
	//		}
	//		int j = i+1;
	//		if (j >= size()) {
	//			j = 0;
	//		}
	//		if (!isClockwise()) {
	//			int tmp = h;
	//			h = j;
	//			j = tmp;
	//		}
	//		double trk1 = p.initialVelocity(getVertex(j), 100).trk();
	//		double trk2 = p.initialVelocity(getVertex(h), 100).trk();
	//		v.mkTrk((trk1+trk2)/2 + Math.PI);
	//		return p.linear(v, buffer);
	//	}

	//	private SimplePoly mkSimp(double top, ArrayList<Pair<Position,Integer>> vs) {
	//		SimplePoly p2 = new SimplePoly();
	//		Collections.sort(vs,new SIComparator());
	//		for(int i = 0; i < vs.size(); i++) {
	//			p2.points.add(vs.get(i).first);
	//		}
	//		p2.setTop(top);
	//		return p2;
	//	}

	//	/**
	//	 * This returns a convex polygon that completely contains the current polygon, hopefully with fewer vertices.
	//	 * Buffer is an extra space
	//	 * @return
	//	 */
	//	public SimplePoly simplify2(double buffer) {
	//		if (size() < 3) return copy();
	//		Position cent = centroid();
	//		ArrayList<Pair<Position,Integer>> sortedVerts = new ArrayList<Pair<Position,Integer>>();
	//		for (int i = 0; i < points.size(); i++) {
	//			sortedVerts.add(Pair.make(points.get(i), i));
	//		}
	//		Collections.sort(sortedVerts,new SRComparator());
	//		ArrayList<Pair<Position,Integer>> sortedVerts2 = new ArrayList<Pair<Position,Integer>>();
	//		sortedVerts2.add(Pair.make(pushOut(sortedVerts.get(0).first,buffer), sortedVerts.get(0).second));
	//		sortedVerts2.add(Pair.make(pushOut(sortedVerts.get(1).first,buffer), sortedVerts.get(1).second));
	//		sortedVerts2.add(Pair.make(pushOut(sortedVerts.get(2).first,buffer), sortedVerts.get(2).second));
	//		SimplePoly p2 = mkSimp(top,sortedVerts2);
	//		for (int i = 3; i < sortedVerts.size(); i++) {
	//			if (!p2.contains(sortedVerts.get(i).first)) {
	//				sortedVerts2.add(Pair.make(pushOut(sortedVerts.get(i).first,buffer), sortedVerts.get(i).second));
	//				p2 = mkSimp(top,sortedVerts2);
	//			}
	//		}
	//		// double-check the initial points
	//		if (!p2.contains(sortedVerts.get(0).first)) {
	//			//f.pln("simplepoly simplify error 0");			
	//			return copy();
	//		}
	//		if (!p2.contains(sortedVerts.get(1).first)) {
	////f.pln("simplepoly simplify error 1");
	//			return copy();
	//		}
	//		if (!p2.contains(sortedVerts.get(2).first)) {
	////f.pln("simplepoly simplify error 2");
	//			return copy();
	//		}
	////f.pln("simplepoly simplify from "+size()+" to "+p2.size());
	//		return p2;
	//	}
	//	
	//
	//	public SimplePoly simplify3(double buffer) {
	//		if (size() <= 3) return copy();
	//		Position c = centroid();
	//		if (!contains(c)) return copy();
	//		int max = maxDistIdx(c);
	//		SimplePoly tmp = new SimplePoly(this.bottom,this.top);
	//		for (int i = 0; i < size(); i++) {
	//			tmp.addVertex(points.get((i+max)%size()));
	//		}
	//		// max at 0 in tmp.
	//		int i = 4;
	//		while (i < size()/2) {
	//			int incr = size()/i;
	//			SimplePoly p2 = new SimplePoly(this.bottom,this.top);
	//			for (int j = 0; j < size(); j += incr) {
	//				p2.addVertex(pushOut(tmp.points.get(j),buffer));
	//			}
	//			boolean ok = true;
	//			for (int j = 0; j < size(); j++) {
	//				if (!p2.contains2D(points.get(j))) {
	//					ok = false;
	//					break;
	//				}
	//			}
	//			if (ok) {
	////				f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
	//				return p2;
	//			}
	//			i++;
	//		}
	//		return copy();
	//	}

	public int maxInRange(Position p, double a1, double a2) {
		double maxD = 0.0;
		int idx = -1;
		for (int i = 0; i < points.size(); i++) {
			double d = points.get(i).distanceH(p);
			double a = p.initialVelocity(points.get(i), 100).compassAngle();
			if (d > maxD && a1 <= a && a < a2) {
				idx = i;
				maxD = d;
			}
		}
		return idx;		
	}


	//	public SimplePoly simplify(double buffer) {
	//		if (size() <= 3) {
	//			f.pln("Simplify size too small"); 
	//			return copy();
	//		}
	//ArrayList<SimplePoly> fails = new ArrayList<SimplePoly>();
	//		Position c = centroid();
	//		int i = 3;
	//		while (i <= 90) {
	//			// divide up into angular segments and find max point in each segment
	//			double incr = 2*Math.PI/i;
	//			SimplePoly p2 = new SimplePoly(this.bottom,this.top);
	//			for (int j = 0; j < i; j++) {
	//				int idx = maxInRange(c, j*incr, (j+1)*incr);
	//				if (idx >= 0) {
	////					p2.addVertex(pushOut(points.get(idx),buffer));
	//					p2.addVertex(pushOut(idx,buffer));
	//				}
	//			}
	//			// remove points that are probably from non-convex points
	//			
	////			// remove non-convex points
	////			int k = 1;
	////			while (k < p2.size()-1) {
	////				double trk1 = p2.points.get(k-1).finalVelocity(p2.points.get(k), 100).trk();
	////				double trk2 = p2.points.get(k).initialVelocity(p2.points.get(k+1), 100).trk();
	////				double angle = Util.to_pi(Util.turnDelta(trk1, trk2, isClockwise()));
	////				if (angle <= Units.from("deg", 5.0)) {
	////					p2.points.remove(k);
	////				} else {
	////					k++;
	////				}
	////			}
	//			// check containment of original
	//			boolean ok = true;
	//			for (int j = 0; j < size(); j++) {
	//				if (!p2.contains2D(points.get(j))) {
	//					ok = false;
	//fails.add(p2);
	//					f.pln("SimplePoly simplify i="+i+" does not contain "+j+" "+p2.getVertex(j));
	//					break;
	//				}
	//			}
	//			if (ok) {
	////				f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
	//				return p2;
	//			}
	//			if (i >= 45) {
	//				i = i*2;
	//			} else if (i >= 20) {
	//				i = i + 5;
	//			} else {
	//				i++;
	//			}
	//		}
	//		f.pln("SimplePoly.Simplify failed.");
	//for (i = 0; i < fails.size(); i++) {
	//DebugSupport.dumpSimplePoly(fails.get(i), "fail"+i);
	//}
	//
	//		return copy();
	//	}

	//	private Position outsideEdge(Position p, double a1, double buffer) {
	//		Velocity v = Velocity.mkTrkGsVs(a1,1.0,0.0);
	//		double maxD = 0.0;
	//		Position best = Position.INVALID;
	//		for (int i = 0; i < points.size(); i++) {
	//			int j1 = i;
	//			int j2 = i+1;
	//			if (i == points.size()-1) {
	//				j2 = 0;
	//			}
	//			double d = -1.0;
	//			if (p.isLatLon()) {
	//				Velocity v2 = points.get(j1).initialVelocity(points.get(j2), 100);
	//				double t = GreatCircle.intersection(p.lla(), v, points.get(j1).lla(), v2, true).second;
	//				if (t >= 0 && t <= 100) {
	//					d = p.distanceH(points.get(j1).linear(v2, t));
	//				}
	//			} else {
	//				d = VectFuns.intersectSegment(p.vect2(), v.vect2(), points.get(j1).vect2(), points.get(j2).vect2());
	//			}
	//			if (d > maxD) {
	//				maxD = d;
	//				best = p.linear(v, d+buffer);
	//			}
	//		}
	//		return best;
	//	}
	//
	//	
	//	public SimplePoly simplify(double buffer) {
	//		if (size() <= 3) {
	//			f.pln("Simplify size too small"); 
	//			return copy();
	//		}
	//
	//ArrayList<SimplePoly> fails = new ArrayList<SimplePoly>();
	//
	//		Position c = centroid();
	//		int i = 3;
	//		while (i <= 90) {
	//			// divide up into angular segments and find max point in each segment
	//			double incr = 2*Math.PI/i;
	//			SimplePoly p2 = new SimplePoly(this.bottom,this.top);
	//			for (int j = 0; j < i; j++) {
	//				Position pp = outsideEdge(c, j*incr, buffer);
	//				if (!pp.isInvalid()) {
	//					p2.addVertex(pp);
	//				}
	//			}
	//			// remove points that are probably from non-convex points
	//			
	////			// remove non-convex points
	////			int k = 1;
	////			while (k < p2.size()-1) {
	////				double trk1 = p2.points.get(k-1).finalVelocity(p2.points.get(k), 100).trk();
	////				double trk2 = p2.points.get(k).initialVelocity(p2.points.get(k+1), 100).trk();
	////				double angle = Util.to_pi(Util.turnDelta(trk1, trk2, isClockwise()));
	////				if (angle <= Units.from("deg", 5.0)) {
	////					p2.points.remove(k);
	////				} else {
	////					k++;
	////				}
	////			}
	//			// check containment of original
	//			boolean ok = true;
	//			for (int j = 0; j < size(); j++) {
	//				if (!p2.contains2D(points.get(j))) {
	//					ok = false;
	//					f.pln("SimplePoly simplify i="+i+" does not contain "+j+" "+getVertex(j));
	//fails.add(p2);
	//					break;
	//				}
	//			}
	//			if (ok) {
	////				f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
	//				return p2;
	//			}
	//			if (i >= 45) {
	//				i = i*2;
	//			} else if (i >= 20) {
	//				i = i + 5;
	//			} else {
	//				i++;
	//			}
	//		}
	//		f.pln("SimplePoly.Simplify failed.");
	//for (i = 0; i < fails.size(); i++) {
	//	DebugSupport.dumpSimplePoly(fails.get(i), "fail"+i);
	//}
	//		return copy();
	//	}


	//	public SimplePoly simplifyToSize(int num) {
	//		if (size() <= num) return copy();
	//		SimplePoly tmp = simplify(0.0);
	//		double incr = maxRadius()*0.1;
	//		double buff = 0.0;
	//		int count = 0;
	//		while (tmp.size() > num && count < 3) {
	//			buff += incr;
	//			tmp = simplify(buff);
	//			count++;
	//		}
	////		f.pln("SimplePoly simplifyToSize count="+count+"  buff="+buff+"  incr="+incr+" from="+size()+" to="+tmp.size());
	//		return tmp;
	//	}


	private static final String nl = System.getProperty("line.separator");

	//	public String toOutput(String name, int precision, boolean tcpColumns) {
	//		StringBuffer sb = new StringBuffer(100);
	//		ArrayList<String> ret = new ArrayList<String>(NavPoint.TCP_OUTPUT_COLUMNS+2);
	//		ret.add(name);  // name is (0)
	//		for (int j = 0; j < size(); j++) {
	//			ret.addAll(getVertex(j).toStringList(precision)); //vertex 1-3
	//			ret.add(f.FmPrecision(0.0));   // time 4
	//			if (tcpColumns) {
	//				ret.add("-"); // type
	//				int start = 5;
	//				//				if (mode == PathMode.USER_VEL) {
	//				//					ret.addAll(initialVelocity(j).toStringList()); // vel 6-8
	//				//					start = 8;
	//				//				}
	//				for (int k = start; k < NavPoint.TCP_OUTPUT_COLUMNS; k++) {
	//					ret.add("-");
	//				}
	//			} else {
	//				ret.add("-"); // label
	//				//				if (mode == PathMode.USER_VEL) {
	//				//					ret.addAll(initialVelocity(j).toStringList()); // vel 6-8
	//				//				}
	//			}
	//			ret.add(f.FmPrecision(Units.to("ft", getTop()),precision));
	//			sb.append(f.list2str(ret,", ")+nl);
	//		}
	//		return sb.toString();
	//	}

	/**
	 * Return the angle that is perpendicular to the middle of the edge from vertex i to i+1, facing outward.
	 * Return NaN if i is out of bounds or vertex i overlaps vertex i+1. 
	 */
	public double perpSide(int i) {
		if (i < 0 || i >= points.size()) return Double.NaN;
		Position p1 = points.get(i);
		Position p2;
		if (i == points.size()-1) {
			p2 = points.get(0);
		} else {
			p2 = points.get(i+1);
		}
		if (p1.almostEquals(p2)) return Double.NaN;
		double trk = p1.avgVelocity(p2, 100).trk() - Math.PI/2.0;
		if (isClockwise()) {
			return Util.to_2pi(trk);
		} else {
			return Util.to_2pi(trk+Math.PI);
		}
	}

	private boolean vertexConvex(Position p0, Position p1, Position p2) {
		Position[] pts = {p0,p1,p2};
		Double[] wgts = {1.0, 100.0, 1.0};
		Position avg = avgPos(Arrays.asList(pts), Arrays.asList(wgts));
		return contains2D(avg);
	}

	/**
	 * Return the internal angle of vertex i.
	 * Return NaN if i is out of bounds or vertex i overlaps with an adjacent vertex. 
	 */
	public double vertexAngle(int i) {
		if (i < 0 || i >= points.size()) return Double.NaN;
		Position p1 = points.get(i);
		Position p2;
		Position p0;
		if (i == points.size()-1) {
			p2 = points.get(0);
		} else {
			p2 = points.get(i+1);
		}
		if (i == 0) {
			p0 = points.get(points.size()-1);
		} else {
			p0 = points.get(i-1);
		}
		if (p1.almostEquals(p2) || p1.almostEquals(p0) || p0.almostEquals(p2)) return Double.NaN;
		double ang;
		if (p1.isLatLon()) {
			ang = GreatCircle.angle_between(p0.lla(), p1.lla(), p2.lla());
		} else {
			ang = Util.to_2pi(VectFuns.angle_between(p0.vect2(), p1.vect2(), p2.vect2()));
		}
		if (vertexConvex(p0,p1,p2)) {		
			return ang;
		} else {
			return 2*Math.PI - ang;
		}
	}

	public String toOutput(String name, int precision, int numberTcpColumns) {
		StringBuffer sb = new StringBuffer(100);
		for (int j = 0; j < size(); j++) {
			ArrayList<String> ret = new ArrayList<String>(numberTcpColumns+2);
			ret.add(name);  // name is (0)
			ret.addAll(getVertex(j).toStringList(precision)); //vertex 1-3
			ret.add(f.FmPrecision(0.0)); // time 4
			if (numberTcpColumns > 0) {
				ret.add("-"); // type
				int start = 5;
				//						if (mode == PathMode.USER_VEL) {
				//							ret.addAll(initialVelocity(j).toStringList()); // vel 6-8
				//							start = 8;
				//						}
				for (int k = start; k < numberTcpColumns; k++) {
					ret.add("-");
				}
			} else {
				ret.add("-"); // label
				//						if (mode == PathMode.USER_VEL) {
				//							ret.addAll(initialVelocity(j).toStringList()); // vel 6-8
				//						}
			}
			ret.add(f.FmPrecision(Units.to("ft", getTop()),precision));

			sb.append(f.list2str(ret,", ")+nl);
		}
		return sb.toString();
	}



	/**
	 * 
	 * String representation of this SimplePoly.
	 */
	public String toString() {
		String s = "";
		for (int i = 0; i < size(); i++) {
			s = s + getVertex(i);
		}
		return s;
	}

	public List<String> toStringList(int vertex, int precision) {
		ArrayList<String> ret = new ArrayList<String>(4);
		Position p = getVertex(vertex); 
		ret.addAll(p.toStringList(precision));
		if (p.isInvalid()) {
			ret.add("-");
		} else {
			ret.add(f.FmPrecision(Units.to("ft",top),precision));
		}
		return ret;
	}

	public boolean validate() {
		return validate(null); 		
	}

	public boolean validate(ErrorLog error) {
		// not a polygon
		if (size() < 3)	return false;
		for (int i = 0; i < points.size(); i++) {
			// invalid points
			if (points.get(i).isInvalid()) return false;
			for (int j = 0; j < i; j++) {
				// duplicated points
				if (points.get(i).distanceH(points.get(j)) < Constants.get_horizontal_accuracy()) {
					if (error != null) error.addError("polygon has duplicated points at "+i);
					return false;
				}
				if (j < i-2) {
					// check for intersections
					if (isLatLon()) {
						LatLonAlt a = points.get(j).lla();
						LatLonAlt b = points.get(j+1).lla();
						LatLonAlt c = points.get(i-1).lla();
						LatLonAlt d = points.get(i).lla();
						double t = GreatCircle.intersection(a, GreatCircle.velocity_initial(a, b, 100), c, GreatCircle.velocity_initial(c,d,100), true).second;
						if (t >= 0 && t <= 100) {
							if (error != null) error.addError("polygon has intersecting edges at "+(i-1)+"-"+i+" and "+j+"-"+(j+1)+" t="+t);
							return false;
						}
					} else {
						Vect3 a = points.get(j).point();
						Vect3 b = points.get(j+1).point();
						Vect3 c = points.get(i-1).point();
						Vect3 d = points.get(i).point();
						double t = VectFuns.timeOfIntersection(a, Velocity.make(b.Sub(a).Scal(0.01)), c, Velocity.make(d.Sub(c).Scal(0.01)));
						if (t >= 0 && t <= 100) {
							if (error != null) error.addError("polygon has intersecting edges at "+(i-1)+"-"+i+" and "+j+"-"+(j+1));
							return false;
						}
					}
				}
			}
			if (i > 1) {
				// redundant (consecutive collinear) points
				if (isLatLon()) {
					if (GreatCircle.collinear(points.get(i-2).lla(), points.get(i-1).lla(), points.get(i).lla())) {
						if (error != null) error.addWarning("polygon has redundant collinear points at "+(i));
						return false;
					}
				} else {
					if (VectFuns.collinear(points.get(i-2).vect2(), points.get(i-1).vect2(), points.get(i).vect2())) {
						if (error != null) error.addWarning("polygon has redundant collinear points at "+(i));
						return false;					
					}
				}
			}
		}
		return true;
	}

	public SimplePoly cleanup() {
		boolean done = false;
		SimplePoly p2 = this.copy();

		while (!done) {
			done = true;
			int i = 0;
			while (i < p2.size() && p2.size() > 3) {
				int h = i-1;
				int j = i+1;
				if (h < 0) h = p2.size()-1;
				if (j == p2.size()) j = 0;
				if (p2.getVertex(i).almostEquals(p2.getVertex(j))) {
					p2.remove(i);
					done = false;
					//				} else if (isLatLon() && GreatCircle.collinear(points.get(h).lla(), points.get(i).lla(), points.get(j).lla())) {
					//					p2.remove(i);
					//					done = false;
					//				} else if (!isLatLon() && VectFuns.collinear(points.get(h).vect2(), points.get(i).vect2(), points.get(j).vect2())) {
					//					p2.remove(i);
					//					done = false;
				} 
				i++;
			}
			//DebugSupport.dumpSimplePoly(p2, "step"+(step++));
		}
		return p2;
	}

	public BoundingRectangle getBoundingRectangle() {
		BoundingRectangle br = new BoundingRectangle();
		for (int i =  0; i < points.size(); i++) {
			br.add(points.get(i));
		}
		return br;
	}

	//	/**
	//	 * attempts to "fix" errant polygons.  returns true if no fix needed.
	//	 */
	//	public boolean fix() {
	//		boolean ret = true;
	//		if (size() < 3) {
	//			ret = false;
	//			f.pln("Less than 3 points");
	//		}
	//		int i = 0;
	//		while (i < size()) {
	//			if (points.get(i).isInvalid()) {
	//				ret = false;
	//				f.pln("Point "+i+" is invalid");
	//				points.remove(i);
	//			} else {
	//				int j = i+1;
	//				while (j < size()) {
	//					if (points.get(i).distanceH(points.get(j)) < Constants.get_horizontal_accuracy()) {
	//						ret = false;
	//						f.pln("Points "+i+" and "+j+" appear to be redundant (within "+points.get(i).distanceH(points.get(j))+" m of each other)");
	//						remove (j);
	//					} else {
	//						++j;
	//					}
	//				}
	//				++i;
	//			}
	//		}
	//		i = 2;
	//		while (i < size()) {
	//			boolean rem = false;
	//			for (int j = 0; j < i-2; j++) {
	//				if (isLatLon()) {
	//					LatLonAlt a = points.get(j).lla();
	//					LatLonAlt b = points.get(j+1).lla();
	//					LatLonAlt c = points.get(i-1).lla();
	//					LatLonAlt d = points.get(i).lla();
	//					double t = GreatCircle.intersection(a, b, 100.0, c, d).second;
	//					if (t >= 0.0 && t <= 100.0) {
	//						ret = false;
	//						rem = true;
	//						f.pln("Section "+(i-1)+":"+i+" intersects "+j+":"+(j+1));
	//					}
	//				} else {
	//					Vect3 a = points.get(j).point();
	//					Vect3 b = points.get(j+1).point();
	//					Vect3 c = points.get(i-1).point();
	//					Vect3 d = points.get(i).point();
	//					double t = VectFuns.intersection(a, b, 100.0, c, d).second;
	//					if (t >= 0.0 && t <= 100.0) {
	//						ret = false;
	//						rem = true;
	//						f.pln("Section "+(i-1)+":"+i+" intersects "+j+":"+(j+1));
	//					}
	//				}
	//			}
	//			if (rem) {
	//				remove (i);
	//			} else {
	//				++i;
	//			}
	//		}
	//		return ret;
	//	}

}
