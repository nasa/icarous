/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;
import java.awt.geom.GeneralPath;
import java.io.Serializable;
import java.util.ArrayList;
import java.util.Arrays;

/**
 * Encapsulates a geometric polygon. The polygon is defined in terms of its vertex coordinates. 
 * This implementation assumes a simply connected polygon. The Polygon is otherwise quite general allowing multiply
 * connected regions. The class provides a containment test for points and uses bounding rectangles
 * to speed up computations.
 */
public class Poly2D implements Serializable {
	private final static long serialVersionUID = 1;
	ArrayList<Vect2> vertices;
	boolean boundingRectangleDefined;
	private BoundingRectangle boundingRect;
	private static final double vertexPerturb = 1E-10;
	private static final double perturbAmount = 2 * vertexPerturb;
	
	public static final Poly2D ZERO = new Poly2D();

	public Poly2D() {
		vertices = new ArrayList<Vect2>(0);
		boundingRectangleDefined = false;
	}
	
	public int size() {
	     return vertices.size();	
	}
			
	
	public Poly2D(ArrayList<Vect2> verts) {
		boundingRectangleDefined = false;
		vertices = new ArrayList<Vect2>(verts.size());
		for (int i = 0; i < verts.size(); i++) {  // copy
			//vertices.set(i,new Vect2(verts.get(i).x,verts.get(i).y));
			vertices.add(new Vect2(verts.get(i).x,verts.get(i).y));
		}
		if (vertices.size() > 0) {
	    	boundingRect = new BoundingRectangle(vertices);
	    	boundingRectangleDefined = true;
		}
	}

	
	/**
	 * Constructor for creating a copy of a Polygon
	 * 
	 * @param polygon The polygon to be copied
	 */
	public Poly2D(Poly2D polygon) {
		vertices = new ArrayList<Vect2>(polygon.size());
		for (int i = 0 ; i < polygon.size() ; i++) {
			//vertices.set(i,new Vect2(polygon.vertices.get(i).x,polygon.vertices.get(i).y));
			vertices.add(new Vect2(polygon.vertices.get(i).x,polygon.vertices.get(i).y));
		}
		boundingRect = new BoundingRectangle(polygon.boundingRect);
		boundingRectangleDefined = polygon.boundingRectangleDefined;
	}
	
	public Poly2D linear(Vect2 v, double t) {
		Poly2D rtn = new Poly2D();
		for (Vect2 vt: vertices) {
			rtn.addVertex(vt.linear(v,t));
		}		
		return rtn;	
	}

	// create a new polygon from this one and an arrayList of velocities.  It assumes that the arraylist is at least as 
	//long as the starting polygon.
	public Poly2D linear(ArrayList<Vect2> v, double t) {
		Poly2D rtn = new Poly2D();
		for (int i = 0; i < size(); i++) {
			Vect2 vt = vertices.get(i);
			rtn.addVertex(vt.linear(v.get(i),t));
		}		
		return rtn;	
	}

	
	public void addVertex(double x, double y) {
		vertices.add(new Vect2(x,y));
		boundingRectangleDefined = false;
	}

	
	public void addVertex(Vect2 v) {
//f.pln("Poly2D.addVertex "+v);	
		vertices.add(v);		
	}

	/**
	 * Determines if the supplied point lies within this polygon. Uses the "crossing number" algorithm
	 * This uses a standard raycasting check for point inclusion.  It does not explicitly use ACCoRD detection algorithms.
	 * 
	 * @param a - x coordinate of the point
	 * @param b - y coordinate of the point
	 * @return true if the Polygon contains the point (a, b), otherwise false
	 */
	public boolean contains(double a, double b) {
		if (!boundingRectangleDefined) {
	    	boundingRect = new BoundingRectangle(vertices);
	    	boundingRectangleDefined = true;
		}
		if (!getBoundingRectangle().contains(a, b)) {
			return false;
		}		
		// Adjust the x coordinate if it is very close to that of the point in question to avoid ambiguity caused by loss of 
		// significance in floating point calculations. This assures that all polygon x coordinates will be at least vertexPerturb 
		// to the left or right of the x coordinate of the point in question.
		Poly2D poly2 = new Poly2D(this);
//f.pln(" ^^$$>> Poly2D.contains:  poly2 = "+poly2+" a="+a+" b="+b);
		for (int i = 0 ; i < vertices.size() ; i++) {
			if (Math.abs(a - vertices.get(i).x) <= vertexPerturb) {
				double yi = poly2.vertices.get(i).y;
				poly2.vertices.set(i,new Vect2(vertices.get(i).x + perturbAmount, yi));
			}
		}		
		// Determine, for each edge of the polygon, whether the ray parallel to the y-axis
		// which proceeds in the positive direction from the point in question intersects the edge.
		// Count the number of edges intersected. If the count is odd, the point lies within the polygon
		int crossingCount = 0;
		int n = poly2.vertices.size();
		for (int i = 0 ; i < n ; i++) {
			// Note: the purpose of the modulo n on the index is to make the last
			// line segment
			// the one that connects the last point back to the first.
			if ((poly2.vertices.get(i).x < a && poly2.vertices.get((i + 1) % n).x < a)|| (poly2.vertices.get(i).x > a && poly2.vertices.get((i + 1) % n).x > a)) {
				// Line is entirely to the left or right of the y-axis ray
				continue;
			}
			if (poly2.vertices.get(i).y < b && poly2.vertices.get((i + 1) % n).y < b) {
				// Line is entirely below the y-axis ray
				continue;
			}
			if (poly2.vertices.get(i).y > b && poly2.vertices.get((i + 1) % n).y > b) {
				// both y coordinates are above the point, and the x coordinates straddle it; The ray must cross the line;
				crossingCount++;
				continue;
			}
			// because of the perturbation, we cannot have any candidate vertical edges (those would be either all-left or all-right)
			// Calculate the inverse (x/y) slope of the line
			double slope = (poly2.vertices.get((i + 1) % n).x - poly2.vertices.get(i).x) / (poly2.vertices.get((i + 1) % n).y - poly2.vertices.get(i).y);
			// Now the x intercept at the y value of the ray
			double yRayIntercept = (a - poly2.vertices.get(i).x)/slope + poly2.vertices.get(i).y;
			if (yRayIntercept >= b) {
				crossingCount++;
				continue;
			}
		}
//f.pln(" $$>> Poly2D.contains crossingCount = "+crossingCount);
		if (crossingCount % 2 > 0){
			return true;
		}
		return false;
	}
	
	public boolean contains(Vect2 v) {
	    return contains(v.x,v.y);	
	}


	public void setVertex(int i, Vect2 v) {
		if (i < 0 || i >= vertices.size()) {
			f.pln("Poly2D.setVertex index out of bounds: "+i);
		} else {
			vertices.set(i,v);
		}
    	boundingRectangleDefined = false;
	}

	/**
	 * Return vertex, or INVALID if out of bounds.
	 * @param i
	 * @return
	 */
	public Vect2 getVertex(int i) {
		if (i < 0 || i >= vertices.size()) {
//			f.pln("Poly2D.getVertex index out of bounds: "+i);
			return Vect2.INVALID;
		} else {
			return vertices.get(i);
		}
	}

	public ArrayList<Vect2> getVertices() {
		return vertices;
	}

	public BoundingRectangle getBoundingRectangle() {
		return boundingRect;
	}

	public void setBoundingRectangle(BoundingRectangle boundingRect) {
		this.boundingRect = boundingRect;
	}


	/**
	 * Planar geometric area of a <b>simple</b> polygon. It is positive if the
	 * vertices are ordered counterclockwise and negative if clockwise.
	 * 
	 * @return the signed area of the polygon
	 */
	public double signedArea() {
		double temp = 0;
		int size = vertices.size();
		if (vertices.get(0).x == vertices.get(size - 1).x && vertices.get(0).y == vertices.get(size - 1).y) {
			size--;
		}
		for (int i = 1 ; i < size - 1 ; ++i) {
			temp += vertices.get(i).x * (vertices.get(i + 1).y - vertices.get(i - 1).y);
		}
		double area2 = vertices.get(0).x * (vertices.get(1).y - vertices.get(size - 1).y) 
		                 	+ temp + vertices.get(size - 1).x * (vertices.get(0).y - vertices.get(size - 2).y);
		return area2 / 2;
	}
	
	// area and centroid courtesy of Paul Bourke (1988) http://paulbourke.net/geometry/polyarea/
	// these are for non self-intersecting polygons
	// this calculation assumes that point 0 = point n (or the start point is counted twice)
	/**
	 * Return the horizontal area (in m^2) of this Poly3D.
	 */
	public double area() {
		double a = 0;
		vertices.add(vertices.get(0));
		for (int i = 0; i < vertices.size()-1; i++) {
			a = a + (vertices.get(i).x()*vertices.get(i+1).y() - vertices.get(i+1).x()*vertices.get(i).y());
		}
		vertices.remove(vertices.size()-1);
		return 0.5*a;
	}

	/**
	 * Distance from centroid to closest vertex
	 * @return
	 */
	public double innerRadius() {
		Vect2 cpos = centroid();
		double minDist = Double.MAX_VALUE;
		for (Vect2 v: vertices) {
			double dist = cpos.distance(v);
			if (dist < minDist) minDist = dist;
		}
		return minDist;
	}

	
	/**
	 * Distance from centroid to farthest vertex
	 * @return
	 */
	public double outerRadius() {
		Vect2 cpos = centroid();
//		f.pln(" $$$$$ outerRadius: cpos = "+cpos);
		double maxDist = 0.0;
		for (Vect2 v: vertices) {
			double dist = cpos.distance(v);
			if (dist > maxDist) maxDist = dist;
		}
		return maxDist;
	}
	
	/**
	 * Distance from averagePoint to farthest vertex
	 * @return
	 */
	public double apBoundingRadius() {
		Vect2 cpos = averagePoint();
		double maxDist = 0.0;
		for (Vect2 v: vertices) {
			double dist = cpos.distance(v);
			if (dist > maxDist) maxDist = dist;
		}
		return maxDist;
	}


	/**
	 * @return the geometric centroid.
	 */
	public Vect2 centroid()	{
		double temp = 0;
		double tempX = 0;
		double tempY = 0;
		int size = vertices.size();
		if (vertices.get(0).x == vertices.get(size - 1).x && vertices.get(0).y == vertices.get(size - 1).y) {
			size--;
		}
		for (int i = 0 ; i < size - 1 ; ++i) {
			temp = vertices.get(i).x * vertices.get(i + 1).y - vertices.get(i + 1).x * vertices.get(i).y;
			tempX += (vertices.get(i).x + vertices.get(i + 1).x) * temp;
			tempY += (vertices.get(i).y + vertices.get(i + 1).y) * temp;
		}
		temp = vertices.get(size - 1).x * vertices.get(0).y - vertices.get(0).x * vertices.get(size - 1).y;
		tempX += (vertices.get(size - 1).x + vertices.get(0).x) * temp;
		tempY += (vertices.get(size - 1).y + vertices.get(0).y) * temp;
		double area6 = 6 * signedArea();
		tempX /= area6;
		tempY /= area6;
		return new Vect2(tempX, tempY);
	}

	/**
	 * Return the average of all vertices.  Note this is not the same as the centroid, and will be weighted 
	 * towards concentrations of vertices instead of concentrations of area/mass.  This will, however, have the nice property of having
	 * a constant linear velocity between two polygons, even if they morph shape. 
	 */
	public Vect2 averagePoint() {
		double x = 0;
		double y = 0;
		for (int i = 0; i < vertices.size(); i++) {
			x += vertices.get(i).x();
			y += vertices.get(i).y();
		}
		return new Vect2(x/vertices.size(), y/vertices.size());
	}
	
	
	/**
	 * Return true if p is a subset of this polygon.
	 * This determines if the starting point of p is within this polygon, and if any edges intersect with this polygon.
	 * This does not explicitly use ACCoRD detection algorithms.
	 * @param p
	 * @return
	 */
	public boolean contains(Poly2D p) {
		if (size() == 0 || p.size() == 0) return false;
		if (!contains(p.getVertex(0))) return false;
		for (int i = 0; i < size(); i++) {
			Vect2 a = getVertex(i);
			Vect2 b = getVertex(i+1);
			if (i == size()-1) {
				a = getVertex(size()-1);
				b = getVertex(0);
			}
			for (int j = 0; j < p.size(); j++) {
				Vect2 c = p.getVertex(i);
				Vect2 d = p.getVertex(i+1);
				if (j == p.size()-1) {
					c = p.getVertex(size()-1);
					d = p.getVertex(0);
				}
				double t = VectFuns.intersection(new Vect3(a,0), new Vect3(b,0), 100.0, new Vect3(c,0), new Vect3(d,0)).second;
				if (t >= 0 && t <= 100.0) return false;
			}
		}
		return true;
	}
	
	
	public Poly2D relative(Vect2 so, Vect2 vo) {
		  ArrayList<Vect2> p = new ArrayList<Vect2>();
		  Vect2 vx = vo.PerpR().Hat();
		  Vect2 vy = vo.Hat();
		  for (int i = 0; i < vertices.size(); i++) {
			  Vect2 pi = new Vect2(vx.dot(vertices.get(i).Sub(so)), vy.dot(vertices.get(i).Sub(so)));
			  p.add(pi);
		  }
		  return new Poly2D(p);

	}
	
	/**
	 * Is the angle from i-j and j-k less than 180 degrees?
	 */
	private boolean convexAngle(int i, int j, int k) {
//		if (k == vertices.size()) k = 0;
//		if (i == -1) i = vertices.size()-1;
		double a1 = vertices.get(j).Sub(vertices.get(i)).trk();
		double a2 = vertices.get(k).Sub(vertices.get(j)).trk();
		return Util.turnDelta(a1, a2, true) <= Math.PI;			
	}
	
	public boolean vertexConvex(int i) {
		int sz = vertices.size()-1;
		if (i == 0) {
			return convexAngle(sz,0,1);
		} else if (i == sz) {
			return convexAngle(sz-1,sz,0);
		} else {
			return convexAngle(i-1,i,i+1);
		}
	}
	
	public ArrayList<Integer> nonConvexVertices() {
		ArrayList<Integer> ret = new ArrayList<Integer>();
		for (int i = 0; i < vertices.size(); i++) {
			if (vertexConvex(i)) ret.add(i);
		}
		return ret;
	}
	
	public boolean isConvex() {
		return nonConvexVertices().size() == 0;
	}
	
	
//	public ArrayList<Poly2D> convexComponents() {
//		return convexComponents(new Poly2D(this));
//	}
	
//	private ArrayList<Poly2D> convexComponents(Poly2D p2d) {
//		ArrayList<Poly2D> ret = new ArrayList<Poly2D>();
//		ArrayList<Integer> ncvs = p2d.nonConvexVertices();
//		if (ncvs.size() == 0) {
//			ret.add(this);
//			return ret;
//		}
//		int checked = 0;
//		CircularArrayList<Vect2> verts = new CircularArrayList<Vect2>(vertices);
//		int sz = size();
//		int i = (ncvs.get(0) + sz/2);
//		int j =  (i+1);
//		int curr = ncvs.get(0);
//		i = ncvs.get(ncvs.size()-1);
//		j = ncvs.get(1%ncvs.size());
//		while (i != curr || j != curr) {
//			if (convexAngle(i-1,i,i+1))
//		}
//		return ret;
//	}
	
//	private Vect2 calcCentroid() {
//		double a = area();
//		Vect2 cpos;
//		// if a point or line, use old method
//		if (a == 0) {
//			Vect2 v2 = new Vect2(0,0);
//			for (int i = 0; i < vertices.size(); i++) {
//				v2 = v2.Add(vertices.get(i));
//			}
//			v2 = v2.Scal(1.0/vertices.size());
//			double x = v2.x;
//			double y = v2.y;
//			cpos = new Vect2(x, y);
//		} else {
//			// Paul's calculation
//			double x = 0;
//			double y = 0;
//			vertices.add(vertices.get(0));
//			for (int i = 0; i < vertices.size()-1; i++) {
//				double x0 = vertices.get(i).x();
//				double x1 = vertices.get(i+1).x();
//				double y0 = vertices.get(i).y();
//				double y1 = vertices.get(i+1).y();
//				x = x + (x0+x1)*(x0*y1-x1*y0);
//				y = y + (y0+y1)*(x0*y1-x1*y0);
//			}
//			vertices.remove(vertices.size()-1);
//			x = x/(6*a);
//			y = y/(6*a);
//			cpos = new Vect2(x, y);
//		}
//		//centroidDefined = true;
//		return cpos;
//	}

	
	/**
	 * Returns a GeneralPath representation for testing purposes.
	 * 
	 * @return this polygon as a GeneralPath
	 */
	public GeneralPath getGeneralPath() {
		final double epsilon = 1e-6;
		GeneralPath gp = null;
		//if (gp == null) {
		gp = new GeneralPath();
		gp.moveTo((float) vertices.get(0).x, (float) vertices.get(0).y);
		int length = vertices.size();
		if (Math.abs(vertices.get(length-1).x - vertices.get(0).x) < epsilon && Math.abs(vertices.get(length-1).y - vertices.get(0).y) < epsilon) --length;
		for (int i = 1 ; i <= length - 1 ; i++) {
			gp.lineTo((float) vertices.get(i).x, (float) vertices.get(i).y);
		}
		gp.closePath();
		//}
		return gp;
	}
	
	
	/**
	 * Reverse order of vertices
	 * @return
	 */
	public Poly2D reverseOrder() {
		Poly2D np = new Poly2D();
		for (int i = size()-1; i >= 0; i--) {
			np.addVertex(vertices.get(i));
		}
		return np;
	}

	@Override
	public String toString() {
		return "Poly2D [vertices=" + vertices+ ", boundingRect=" + boundingRect + "]";
	}
}
