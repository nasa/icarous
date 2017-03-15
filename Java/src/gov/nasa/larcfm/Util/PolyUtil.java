/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import gov.nasa.larcfm.Util.PolyPath.PathMode;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.Random;

public class PolyUtil {

	static Random rand = new Random();

	public static SimplePoly randomPoly(Position p, double baseRadius, int sides, double bottom, double top) {
		SimplePoly poly = new SimplePoly();
		ArrayList<Double>courses = new ArrayList<Double>(sides);
		//f.pln(" $$$ randomPoly: create polygon with "+sides+" sides");
		for (int i = 0; i < sides; i++) {
			courses.add(rand.nextDouble()*Math.PI*2);
		}
		Collections.sort(courses);
		for (int i = 0; i < sides; i++) {
			double dist = baseRadius*2*rand.nextDouble();
			Velocity v = Velocity.mkTrkGsVs(courses.get(i), 1.0, 0.0);
			Position vert = p.linear(v, dist);
			poly.addVertex(vert);
		}
		poly.setBottom(bottom);
		poly.setTop(top);
		return poly;
	}

	public static PolyPath randomUserPath(String id, double startTime, Velocity baseV, Position p, double baseRadius, int sides, double bottom, double top) {
		PolyPath path = new PolyPath(id);
		path.setPathMode(PolyPath.PathMode.USER_VEL);
		SimplePoly poly = randomPoly(p,baseRadius,sides,bottom,top);
		double velocityLength = baseV.norm()*0.3;
		Vect3 v3 = new Vect3(rand.nextDouble()*velocityLength*2 - velocityLength, rand.nextDouble()*velocityLength*2 - velocityLength, 0.0);
		Velocity v = Velocity.make(baseV.Add(v3));
		path.addPolygon(poly, v, startTime);
//f.pln(" $$$ randomUserPath: "+id+"  poly = "+poly.size());
		return path;
	}

	public static ArrayList<PolyPath> randomUserPaths(Position center, double range, Velocity baseV, double bottom, double startTime, int num, int basenum) {
		ArrayList<PolyPath> paths = new ArrayList<PolyPath>(num);
		for (int i = 0; i < num; i++) {
			double dist = range*rand.nextDouble();
			Velocity v = Velocity.mkTrkGsVs(rand.nextDouble()*Math.PI*2, 1.0, 0.0);
			Position p = center.linear(v, dist);
			double rr = 2 + Math.abs(rand.nextGaussian()*10);
			double radius = Units.from("nmi", rr);
			//			 radius = Util.min(radius, range/4);
			int sides = rand.nextInt(22)+3;
			double tp = Units.from("ft", bottom) + Util.max(5000, 20000+rand.nextGaussian()*10000);
			double top = Units.from("ft", tp);
			PolyPath pp = randomUserPath("poly"+(basenum+i), startTime, baseV, p, radius, sides, bottom, top);
			paths.add(pp.simplify(Units.from("nmi", 2.0)));
		}
		return paths;
	}

	
	private static class SIComparator implements Comparator<Triple<Position,Integer,Double>> {
		@Override
		public int compare(Triple<Position,Integer,Double> o1, Triple<Position,Integer,Double> o2) {
			return Double.compare(o1.third, o2.third);
		}
	}


	/**
	 * Return a polygon that is the convex hull of the listed points.  This uses the Graham scan algorithm.
	 * For purposes of this algorithm, we only consider x, y values of positions (since all calculations are relative and no points are moved).
	 * This uses the NE-most point as the origin, to allow for our track computations (0 is north) and proceeds in a clockwise fashion
	 * This assumes that the resulting hull will not include the north or south poles (for lat lon positions)
	 * @param plist list of positions
	 * @param bottom lower altitude
	 * @param top top altitude
	 * @return SimplePoly
	 */
	public static SimplePoly convexHull(ArrayList<Position> plist, double bottom, double top) {
		ArrayList<Triple<Position,Integer,Double>> elems = new ArrayList<Triple<Position,Integer,Double>>(); // vertex position, vertex index in original polygon, angle from origin point
		int origin = 0;
		// pick origin point
		for (int i = 1; i < plist.size(); i++) {
			Position po = plist.get(origin);
			Position pi = plist.get(i);
			if (pi.y() > po.y() || (pi.y() == po.y() && pi.isWest(po))) {
				origin = i;
			}
		}
		// order all other points relative to origin
		for (int i = 0; i < plist.size(); i++) {
			if (i != origin) {
				Position po = plist.get(origin);
				Position pi = plist.get(i);
				double val = Util.to_2pi(po.initialVelocity(pi,100).trk());
				elems.add(Triple.make(pi, i, val));
			}
		}
		Collections.sort(elems, new SIComparator());
		elems.add(0,Triple.make(plist.get(origin),origin,Double.NEGATIVE_INFINITY));
		// delete any points that have a left-hand turn
		int i = 1;
		while (i < elems.size()) {
			int j = i+1;
			if (j == elems.size()) j = 0; // last point
			Position a = elems.get(i-1).first;
			Position b = elems.get(i).first;
			Position c = elems.get(j).first;
			double trk1 = a.finalVelocity(b, 100).trk();
			double trk2 = b.initialVelocity(c, 100).trk();
			if (a.almostEquals(b) || b.almostEquals(c) || Util.turnDir(trk1, trk2) <= 0) {
				elems.remove(i);
				if (i > 1) i--; // backtrack 1
			} else {
				i++;
			}
		}
		SimplePoly p2 = new SimplePoly(bottom, top);
		for (i = 0; i < elems.size(); i++) {
			p2.addVertex(elems.get(i).first);
		}
		return p2;
	}

	/**
	 * Returns the convex hull of a polygon.
	 * This assumes that the resulting hull will not include the north or south poles.
	 */
	public static SimplePoly convexHull(SimplePoly p) {
		return convexHull(p.points, p.bottom, p.top);
	}

	/**
	 * Returns the convex hull of a set of polygons.
	 * This assumes that the resulting hull will not include the north or south poles.
	 */
	public static SimplePoly convexHull(ArrayList<SimplePoly> p) {
		ArrayList<Position> ps = new ArrayList<Position>();
		double t = -Double.MAX_VALUE; 
		double b = Double.MAX_VALUE;
		for (int i = 0; i < p.size(); i++) {
			ps.addAll(p.get(i).points);
			t = Util.max(t, p.get(i).top);
			b = Util.min(b, p.get(i).bottom);
		}
		return convexHull(ps, b, t);
	}

	
	/**
	 * Returns the convex hull of a polygon that has been expanded by (approximately) the given buffer size.
	 * This is done by adding additional points around each polygon point and taking the convex hull of them all.
	 * This assumes that the resulting hull will not include the north or south poles.
	 */
	public static SimplePoly convexHull(SimplePoly p, double buffer) {
		ArrayList<Position> ps = new ArrayList<Position>();
		for (int i = 0; i < p.size(); i++) {
			for (int j = 0; j < 360; j += 30) {
				Velocity v = Velocity.makeTrkGsVs(j, buffer, 0.0);
				Position pos = p.getVertex(i).linear(v, 1.0);
				ps.add(pos);
			}
		}
		return convexHull(ps,p.bottom, p.top);
	}
	
	/**
	 * Return a position that is buffer distance further away from the poly's centroid
	 */
	public static Position pushOut(SimplePoly poly, Position p, double buffer) {
		Position cent = poly.centroid();
		double dist = cent.distanceH(p);
		Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
		return p.linear(v, buffer);
	}

	/**
	 * Return a position that is buffer distance further away from the poly's centroid at vertex i
	 */
	public static Position pushOut(SimplePoly poly, int i, double buffer) {
		Position p = poly.getVertex(i);
		Position cent = poly.centroid();
		double dist = cent.distanceH(p);
		Velocity v = cent.finalVelocity(p, dist).mkVs(0.0); // should have a 1 m/s gs
		int h = i-1;
		if (h < 0) {
			h = poly.size()-1;
		}
		int j = i+1;
		if (j >= poly.size()) {
			j = 0;
		}
		if (!poly.isClockwise()) {
			int tmp = h;
			h = j;
			j = tmp;
		}
		double trk1 = p.initialVelocity(poly.getVertex(j), 100).trk();
		double trk2 = p.initialVelocity(poly.getVertex(h), 100).trk();
		v.mkTrk((trk1+trk2)/2 + Math.PI);
		return p.linear(v, buffer);
	}




	//		public SimplePoly simplify3(SimplePoly p, double buffer) {
	//			if (p.size() <= 3) return p.copy();
	//			Position c = p.centroid();
	//			if (!p.contains(c)) return p.copy();
	//			int max = p.maxDistIdx(c);
	//			SimplePoly tmp = new SimplePoly(p.bottom,p.top);
	//			for (int i = 0; i < p.size(); i++) {
	//				tmp.addVertex(p.getVertex((i+max)%p.size()));
	//			}
	//			// max at 0 in tmp.
	//			int i = 4;
	//			while (i < p.size()/2) {
	//				int incr = p.size()/i;
	//				SimplePoly p2 = new SimplePoly(p.bottom,p.top);
	//				for (int j = 0; j < p.size(); j += incr) {
	//					p2.addVertex(pushOut(p, tmp.points.get(j),buffer));
	//				}
	//				boolean ok = true;
	//				for (int j = 0; j < p.size(); j++) {
	//					if (!p2.contains2D(p.getVertex(j))) {
	//						ok = false;
	//						break;
	//					}
	//				}
	//				if (ok) {
	////					f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
	//					return p2;
	//				}
	//				i++;
	//			}
	//			return p.copy();
	//		}

	public static SimplePoly simplify2(SimplePoly p, double buffer) {
		if (p.size() <= 3) {
			f.pln("Simplify size too small"); 
			return p.copy();
		}
		//ArrayList<SimplePoly> fails = new ArrayList<SimplePoly>();
		Position c = p.centroid();
		int i = 3;
		while (i <= 90 && 360.0/i < p.size()) {
			// divide up into angular segments and find max point in each segment
			double incr = 2*Math.PI/i;
			SimplePoly p2 = new SimplePoly(p.bottom,p.top);
			for (int j = 0; j < i; j++) {
				int idx = p.maxInRange(c, j*incr, (j+1)*incr);
				if (idx >= 0) {
					//					p2.addVertex(pushOut(points.get(idx),buffer));
					p2.addVertex(pushOut(p,idx,buffer));
				}
			}
			// remove points that are probably from non-convex points

			//			// remove non-convex points
			//			int k = 1;
			//			while (k < p2.size()-1) {
			//				double trk1 = p2.points.get(k-1).finalVelocity(p2.points.get(k), 100).trk();
			//				double trk2 = p2.points.get(k).initialVelocity(p2.points.get(k+1), 100).trk();
			//				double angle = Util.to_pi(Util.turnDelta(trk1, trk2, isClockwise()));
			//				if (angle <= Units.from("deg", 5.0)) {
			//					p2.points.remove(k);
			//				} else {
			//					k++;
			//				}
			//			}
			// check containment of original
			boolean ok = true;
			for (int j = 0; j < p.size(); j++) {
				if (!p2.contains2D(p.getVertex(j))) {
					ok = false;
					//fails.add(p2);
					f.pln("SimplePoly simplify i="+i+" does not contain "+j+" "+p2.getVertex(j));
					break;
				}
			}
			if (ok) {
				//				f.pln("SimplePoly simplify from="+size()+" to="+p2.size());
				return p2;
			}
			if (i >= 45) {
				i = i*2;
			} else if (i >= 20) {
				i = i + 5;
			} else {
				i++;
			}
		}
		f.pln("SimplePoly.Simplify failed.");
		//for (i = 0; i < fails.size(); i++) {
		//DebugSupport.dumpSimplePoly(fails.get(i), "fail"+i);
		//}

		return p.copy();
	}

	/**
	 * Attempt to (over) approximate the given polygon with one with fewer edges.
	 * @param p original polygon
	 * @param buffer approximate increase in size in any dimension.
	 * @return new larger polygon, or original polygon on failure
	 */
	public static SimplePoly simplify(SimplePoly p, double buffer) {
		if (p.size() <= 3) {
//			f.pln("Simplify size too small"); 
			return p.copy();
		}
		//	ArrayList<SimplePoly> fails = new ArrayList<SimplePoly>();
		Position c = p.centroid();
		int i = 3;
//f.pln("Simplify i="+i+" size="+p.size());			
		while (i <= 90 && i < p.size()) {
//f.pln("Simplify i="+i);			
			// divide up into angular segments and find max point in each segment
			double incr = 2*Math.PI/i;
			SimplePoly p2 = new SimplePoly(p.bottom,p.top);
			for (int j = 0; j < i; j++) {
				int idx = p.maxInRange(c, j*incr, (j+1)*incr);
				if (idx >= 0) {
					//						p2.addVertex(pushOut(points.get(idx),buffer));
					p2.addVertex(pushOut(p,idx,buffer));
				}
			}
			// remove points that are probably from non-convex points

			//				// remove non-convex points
			//				int k = 1;
			//				while (k < p2.size()-1) {
			//					double trk1 = p2.points.get(k-1).finalVelocity(p2.points.get(k), 100).trk();
			//					double trk2 = p2.points.get(k).initialVelocity(p2.points.get(k+1), 100).trk();
			//					double angle = Util.to_pi(Util.turnDelta(trk1, trk2, isClockwise()));
			//					if (angle <= Units.from("deg", 5.0)) {
			//						p2.points.remove(k);
			//					} else {
			//						k++;
			//					}
			//				}
			// check containment of original
			boolean ok = true;
			for (int j = 0; j < p.size(); j++) {
				if (!p2.contains2D(p.getVertex(j))) {
					ok = false;
					//	fails.add(p2);
//					f.pln("SimplePoly simplify i="+i+" does not contain "+j+" "+p2.getVertex(j));
					break;
				}
			}
			if (ok) {
//				f.pln("SimplePoly simplify from="+p.size()+" to="+p2.size());
				return p2;
			}
			if (i >= 45) {
				i = i*2;
			} else if (i >= 20) {
				i = i + 5;
			} else {
				i++;
			}
		}
//		f.pln("SimplePoly.Simplify failed.");
		//	for (i = 0; i < fails.size(); i++) {
		//	DebugSupport.dumpSimplePoly(fails.get(i), "fail"+i);
		//	}

		return p.copy();
	}

	public static SimplePoly simplify(SimplePoly p) {
		boolean done = false;
		SimplePoly p2 = p.copy();
		int step = 0;

		ErrorLog error = new ErrorLog("SimplePoly");

		while (!done) {
			done = true;
			int i = 0;
			while (i < p2.size() && p2.size() > 3) {
				int h = i-1;
				int j = i+1;
				if (h < 0) h = p2.size()-1;
				if (j == p2.size()) j = 0;
				double trk1 = p2.getVertex(h).finalVelocity(p2.getVertex(i), 100).trk();
				double trk2 = p2.getVertex(i).initialVelocity(p2.getVertex(j),100).trk();
				if (Util.clockwise(trk1, trk2) != p.isClockwise()) {
					p2.remove(i);
					done = false;
				} 
				i++;
				if (p.isClockwise() != p2.isClockwise()) f.pln("CLOCKWISE MISMATCH");
			}
			//DebugSupport.dumpSimplePoly(p2, "step"+(step++));
		}
		return p2;
	}


	public static SimplePoly simplifyToSize(SimplePoly p, int num) {
		if (p.size() <= num) return p.copy();
		SimplePoly tmp = simplify(p, 0.0);
		double incr = p.maxRadius()*0.1;
		double buff = 0.0;
		int count = 0;
		while (tmp.size() > num && count < 3) {
			buff += incr;
			tmp = simplify(p, buff);
			count++;
		}
		//			f.pln("SimplePoly simplifyToSize count="+count+"  buff="+buff+"  incr="+incr+" from="+size()+" to="+tmp.size());
		return tmp;
	}


	/**
	 * Stretch a polygon so that it covers (at least) what the original would as it moves over a given time range.
	 * The uses a convex hull, so will be an over-approximation.  There may be inaccuracies for very long periods of time if using geodetic coordinates.<p>
	 * 
	 * Example: Given a polygon W and an aircraft A with a timeAfter of 100 seconds, a conflict detection for 
	 * A against the stretched polygon W' is approximately equivalent to the disjunction of conflict detections 
	 * (any positive is a positive) of the set A' against the original W, where A` is the set A plus all of "echos" timeshifted
	 * up to 100 seconds into the future, modulo distortion from geodetic projections and convex hull expansions.  Effectively 
	 * this is saying that if the conflict detection of A vs. W' is clear, then A will be clear of W and any aircraft that 
	 * precisely follow A will also be clear of W for up to 100 seconds.  (If it were not clear for 100 seconds, then A would 
	 * have impacted the extended W', which "arrives" 100 seconds earlier than W would.)<p>
	 * 
	 * Similarly for timeBefore.<p>
	 * 
	 * If A is clear of W', then it is also clear of the original W, which is a subset of W'.
	 * 
	 * @param sp base polygon
	 * @param v average velocity of polygon
	 * @param timeBefore time before stated position to cover (in sec)
	 * @param timeAfter time after stated position to cover (in sec)
	 * @return  enlarged polygon
	 * 
	 */
	public static SimplePoly stretchOverTime(SimplePoly sp, Velocity v, double timeBefore, double timeAfter) {
		ArrayList<SimplePoly> alist = new ArrayList<SimplePoly>();
		alist.add(sp);
		alist.add(sp.linear(v, timeAfter));
		alist.add(sp.linear(v.Neg(), timeBefore));
		return convexHull(alist);
	}

	/**
	 * NOTE: THIS REQUIRES MORE THOUGHT
	 * Given a polypath, expand the polygons on it so they at least cover the areas that they would when over a longer time.
	 * Note: this does not work for MORPHING paths, and will convert them instead to USER_VEL_FINITE paths.
	 * Polygons are expanded to a new convex hull, meaning this will be an over-approximation.
	 * This may also not be accurate for paths with very long legs in geodetic coordinates.
	 * This may also overestimate the polygons at the start and end of each leg in the path.
	 * @param pp starting path
	 * @param timeBefore time before the base path time to cover (relative, in seconds)
	 * @param timeAfter time after the base path time to cover (relative, in seconds)
	 * @return new path with expanded polygons.
	 */
	public static PolyPath stretchOverTime(PolyPath pbase, double timeBefore, double timeAfter) {
		PolyPath pp = new PolyPath(pbase);
		if (pp.getPathMode() == PathMode.MORPHING) {
			pp.setPathMode(PathMode.USER_VEL_FINITE);
		}
		for (int i = 0; i < pp.size(); i++) {
			SimplePoly sp = pp.getPolyRef(i);
			SimplePoly np = stretchOverTime(sp,pp.initialVelocity(i), timeBefore, timeAfter);
			pp.setPolygon(i, np);			
		}
		return pp;
	}
	
	/**
	 * Return a convex hull that has been expanded by hbuff and vbuff.
	 * This extends the polygons around points, both perpendicular to the segments and directly away from vertices, approximating a round join.
	 * There may be points outside the resulting hull (near vertices) that are slightly closer to the original than the requested buffer.
	 * @param p base polygon
	 * @param hbuff size of horizontal buffer (approx)
	 * @param vbuff size of vertical buffer
	 * @return
	 */
	public static SimplePoly bufferedConvexHull(SimplePoly p, double hbuff, double vbuff) {
		ArrayList<Position> points = new ArrayList<Position>();
		for (int i = 0; i < p.points.size(); i++) {
			Position pos = p.points.get(i);
			double ang = p.vertexAngle(i);
			double ang2 = ang/2.0;
			double trk1 = p.perpSide(i);
			Velocity v1 = Velocity.mkTrkGsVs(trk1, 1.0, 0.0);
			double trk3 = p.isClockwise() ?  (trk1 - Math.PI/2 + ang2) : (trk1 + Math.PI/2 - ang2);
			Velocity v3 = Velocity.mkTrkGsVs(trk3, 1.0, 0.0);
			points.add(pos.linear(v3, hbuff));

//f.pln("PolyUtil.bufferedConvexHull ang="+Units.to("deg", ang));
			
//			//add extra points if at a sharp corner
//			if (ang < Math.PI/6.0) {
				double trk2;
				if (i == 0) {
					trk2 = p.perpSide(p.points.size()-1);
				} else {
					trk2 = p.perpSide(i-1);
				}
				Velocity v2 = Velocity.mkTrkGsVs(trk2, 1.0, 0.0);
				points.add(pos.linear(v1, hbuff));
				points.add(pos.linear(v2, hbuff));
//			}
		}
		return convexHull(points, p.bottom-vbuff, p.top+vbuff);		
	}

	/**
	 * Return a path where polygons are replaced by convex hulls that have been expanded by hbuff and vbuff. 
	 * @param p base polygon path
	 * @param hbuff size of horizontal buffer (approx)
	 * @param vbuff size of vertical buffer
	 * @return
	 */
	public static PolyPath bufferedConvexHull(PolyPath pbase, double hbuff, double vbuff) {
		PolyPath pp = new PolyPath(pbase);
		if (pp.getPathMode() == PathMode.MORPHING) {
			pp.setPathMode(PathMode.USER_VEL_FINITE);
		}
		for (int i = 0; i < pp.size(); i++) {
			SimplePoly sp = pp.getPolyRef(i);
			SimplePoly np = bufferedConvexHull(sp,hbuff, vbuff);
			pp.setPolygon(i, np);
		}
		return pp;
	}

	/**
	 * This is a SLOW, APPROXIMATE test for 2D intersection between a moving polygon and a moving point.  For more accurate 
	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement 
	 * the DetectionPolygon interface and CDSSPolygon.
	 * @param so point-mass starting position
	 * @param vo point-mass velocity
	 * @param sp polygon starting position
	 * @param vp polygon average velocity
	 * @param T end time for test (relative)
	 * @param incr time increment for search (&gt; 0)
	 * @return true if the point mass will intersect with the polygon at or before time T
	 */
	public static boolean intersectsPolygon2D(Position so, Velocity vo, SimplePoly sp, Velocity vp, double T, double incr) {
		if (incr <= 0) return false;
		BoundingRectangle br1 = new BoundingRectangle();
		br1.add(so);
		br1.add(so.linear(vo, T));
		BoundingRectangle br2 = sp.getBoundingRectangle();
		br2.add(sp.linear(vp, T).getBoundingRectangle());
		if (!br1.intersects(br2)) return false;
		for (double t = 0; t <= T; t += incr) {
			if (sp.linear(vp, t).contains2D(so.linear(vo, t))) return true;
		}
		return false;
	}

	/**
	 * This is a SLOW, APPROXIMATE test for 3D intersection between a moving polygon and a moving point.  For more accurate 
	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement 
	 * the DetectionPolygon interface and CDSSPolygon.
	 * @param so point-mass starting position
	 * @param vo point-mass velocity
	 * @param sp polygon starting position
	 * @param vp polygon average velocity
	 * @param T end time for test (relative)
	 * @param incr time increment for search (&gt; 0)
	 * @return true if the point mass will intersect with the polygon at or before time T
	 */
	public static boolean intersectsPolygon(Position so, Velocity vo, SimplePoly sp, Velocity vp, double T, double incr) {
		if (incr <= 0) return false;
		BoundingRectangle br1 = new BoundingRectangle();
		br1.add(so);
		br1.add(so.linear(vo, T));
		BoundingRectangle br2 = sp.getBoundingRectangle();
		br2.add(sp.linear(vp, T).getBoundingRectangle());
		if (!br1.intersects(br2)) return false;
		for (double t = 0; t <= T; t += incr) {
			if (sp.linear(vp, t).contains(so.linear(vo, t))) return true;
		}
		return false;
	}


	/**
	 * This is a SLOW, APPROXIMATE test for 2D intersection between a moving polygon and a moving point.  For more accurate 
	 * (and verified) tests for this property, see polygon functions in the ACCoRD framework, specifically classes that implement 
	 * the DetectionPolygon interface and CDIIPolygon.
	 * @param p plan describing point-mass trajectory
	 * @param pp path describing polygon movement
	 * @param B start time to check (absolute)
	 * @param T end time to check (absolute)
	 * @param incr time increment for search (&gt; 0)
	 * @return time of loss of separation if aircraft will intersect with the polygon between times B and T
	 *         and polygon name that plan is in conflict with
	 * 
	 * Note: the return time will be at an interior point, and the name of the polygon 
	 */
	public static Pair<Double,String> intersectsPolygon2D(Plan p, PolyPath pp, double B, double T, double incr) {
		Pair<Double,String> rtn = new Pair<Double,String>(-1.0,"");
		if (incr <= 0) return rtn;
		BoundingRectangle br1 = p.getBound();
		BoundingRectangle br2 = pp.getBoundingRectangle();
		if (!br1.intersects(br2)) return rtn;
		double start = Util.max(B, Util.max(p.getFirstTime(), pp.getFirstTime()));
		double end = Util.min(T, Util.min(p.getLastTime(), pp.getLastTime()));
		//f.pln(" $$ PolyUtil.intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" start="+start+" end="+end+" CLEAR");
		for (double t = start; t <= end; t += incr) {
			SimplePoly sp = pp.position(t);
			Position ac = p.position(t);
			if (sp.contains2D(ac)) {
				//f.pln(" $$ PolyUtil.intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" t="+t+" LOSS");
				rtn = new Pair<Double,String>(t,pp.getName());
				return rtn;
			}
		}
		//f.pln(" $$ PolyUtil.intersectsPolygon2D p="+p.getName()+" path="+pp.getName()+" B="+B+" T="+T+" CLEAR");
		return rtn;
	}

	/**
	 * Attempt to minimize a given plan (by removing points) such that new segments do not intersect with any polygons.
	 * This uses the intersectsPolygon2D() check, and so has the limitations associated with it.  Use ACCoRD 
	 * calls instead for better performance and/or more accuracy.  Returns a new plan that is hopefully smaller than 
	 * the original plan. 
	 */
	public static Plan reducePlanAgainstPolys(Plan p, ArrayList<PolyPath> paths, double incr) {
		double gs = p.initialVelocity(0).gs();
		return reducePlanAgainstPolys(p,gs,paths,incr);
	}
	
	public static Plan reducePlanAgainstPolys(Plan plan, double gs, ArrayList<PolyPath> paths, double incr) {
		if (incr <= 0) return null;
		Plan curr = plan.copy();
		Plan tmp;
		int lastsz = plan.size()+1;
		while (lastsz > curr.size()) {
//f.pln("lastsz="+lastsz+" currsz="+curr.size());			
			lastsz = curr.size();
			int i = 1;
			while (i < curr.size()-1) {
				tmp = curr.copy();
				double start = tmp.time(i-1);
				double end = tmp.time(i+1);
				tmp.remove(i);
				tmp = PlanUtil.linearMakeGSConstant(tmp,gs);
				if (isPlanInConflictWx(tmp, paths, start, end, incr).first < 0) {
					curr = tmp; // shrink current
				} else {
					i++; // advance index
				}
			}
		};		
		return curr;
	}

	/**
	 * Returns time of intersection if the plan is NOT free of polygons from time start to time end.  This may miss intrusions of up to incr sec
	 * so it may not detect 
	 * @param plan   plan to test
	 * @param paths  set of polygons
	 * @param start  start time of search
	 * @param end    end time of search
	 * @param incr   search increment
	 * @return  time of intersection with a polygon if it occurs when search time interval, otherwise -1
	 *          and polygon name that plan is in conflict with
	 */
	public static Pair<Double,String> isPlanInConflictWx(Plan plan, ArrayList<PolyPath> paths, double start, double end, double incr) {
		for (int i = 0; i < paths.size(); i++) {
			Pair<Double,String> ip2D = intersectsPolygon2D(plan, paths.get(i), start, end, incr);
			double tmOfIntersection = ip2D.first;
			if (tmOfIntersection >= 0) {
				return ip2D;
			}
		}
		return new Pair<Double,String>(-1.0,"");
	}

	/** 
	 * 
	 * @param plan     Plan
	 * @param paths    polygon paths
	 * @param incr     the step size for the search 
	 * @return         time of conflict with weather or -1, string containing name of polygon 
	 */
	public static Pair<Double,String>  isPlanInConflictWx(Plan plan, ArrayList<PolyPath> paths, double incr) {
		 return isPlanInConflictWx(plan,paths,incr,plan.getFirstTime());
	}

	public static Pair<Double,String> isPlanInConflictWx(Plan plan, ArrayList<PolyPath> paths, double incr, double fromTime) {
		if (paths != null) {
			for (int i = 0; i < paths.size(); i++) {
				Pair<Double,String> ip2D = intersectsPolygon2D(plan, paths.get(i), fromTime, plan.getLastTime(), incr);
				double tmOfIntersection = ip2D.first;
				if (tmOfIntersection >= 0) {
					return ip2D;
				}
			}
		}
		return new Pair<Double,String>(-1.0,"");
	}
	
	/** Given that plan is in loss with paths at time entryTime, then this function returns the exit time
	 * 
	 * @param plan
	 * @param paths
	 * @param incr
	 * @param entryTime
	 * @return
	 */
	
	public static double calculateWxExitTime(Plan plan, ArrayList<PolyPath> paths, double incr, double entryTime) {
		double lastTm = plan.getLastTime();
		for (double exTm = entryTime; exTm <= lastTm; exTm = exTm + incr) {
			double tm = isPlanInConflictWx(plan, paths, incr, exTm).first;
			if (tm < 0) return exTm;
		}
		return lastTm;
	}


//	private ArrayList<Integer> findInA(int a, ArrayList<Quad<Integer,Integer,Position,Double>> intersectionPoints) {
//		ArrayList<Integer> ret = new ArrayList<Integer>();
//		for (int i = 0; i < intersectionPoints.size(); i++) {
//			if (intersectionPoints.get(i).first == a) {
//				ret.add(i);
//			}
//		}
//		return ret;
//	}
//
////	private ArrayList<Integer> findInB(int b, ArrayList<Quad<Integer,Integer,Position,Double>> intersectionPoints) {
////		ArrayList<Integer> ret = new ArrayList<Integer>();
////		for (int i = 0; i < intersectionPoints.size(); i++) {
////			if (intersectionPoints.get(i).second == b) {
////				ret.add(i);
////			}
////		}
////		return ret;
////	}
//	
//	public static SimplePoly union(SimplePoly a, SimplePoly b) {
//		if (a.isClockwise() == b.isClockwise()) {
//			// non-collinear intersections: a index, b index, position, relative distance on a segment
//			ArrayList<Quad<Integer,Integer,Position,Double>> intersectionPoints = new ArrayList<Quad<Integer,Integer,Position,Double>>();
//			ArrayList<Integer> rma = new ArrayList<Integer>(); // points to remove from a
//			ArrayList<Integer> rmb = new ArrayList<Integer>(); // points to remove from b
//			for (int i = 0; i < a.size(); i++) {
//				Position a1, a2;
//				a1 = a.getVertex(i);
//				if (i == a.size()-1) {
//					a2 = a.getVertex(0);
//				} else {
//					a2 = a.getVertex(i+1);
//				}
//				if (b.contains2D(a1)) {
//					rma.add(i);
//				}
//				for (int j = 0; j < b.size(); j++) {
//					Position b1, b2;
//					b1 = b.getVertex(i);
//					if (j == b.size()-1) {
//						b2 = b.getVertex(0);
//					} else {
//						b2 = b.getVertex(j+1);
//					}
//					if (a.contains2D(b1)) {
//						rma.add(j);
//					}
//					Pair<Position,Double> isp = PositionUtil.intersection(a1, a2, 100, b1, b2);
//					if (isp.second >= 0 && isp.second <= 100) {
//						intersectionPoints.add(Quad.make(i, j, isp.first, isp.second));
//					}
//				}
//			}
//			SimplePoly res = new SimplePoly(Util.min(a.getBottom(),b.getBottom()), Util.max(a.getTop(), b.getTop()));
//			for (int i = 0; i < a.size(); i++) {
//				if (!rma.contains(i)) {
//					res.addVertex(a.getVertex(i));
//				}
//				ArrayList<Integer> as = findInA(i, )
//			}
//			
//		}
//	}
	
//	private static Pair<Integer,Integer> find(Pair<Integer,Integer> key, Hashtable<Pair<Integer,Integer>,TreeSet<Pair<Integer,Integer>>> sets) {
//		for (Pair<Integer,Integer> setrep : sets.keySet()) {
//			TreeSet<Pair<Integer,Integer>> set = sets.get(setrep);
//			if (set.contains(key)) {
//				return setrep;
//			}
//		}
//		return key;
//	}
//	
//	// return true if table changed
//	private static boolean union(Pair<Integer,Integer> key1, Pair<Integer,Integer> key2, Hashtable<Pair<Integer,Integer>,TreeSet<Pair<Integer,Integer>>> sets) {
//		if (!key1.equals(key2)) {
//			TreeSet<Pair<Integer,Integer>> set1 = sets.remove(key1);
//			TreeSet<Pair<Integer,Integer>> set2 = sets.remove(key2);
//			set1.addAll(set2);
//			sets.put(set1.first(),set1);
//			return true;
//		}
//		return false;
//	}
//	
//	private static boolean adjacent(Pair<Integer,Integer> key1, Pair<Integer,Integer> key2, Hashtable<Pair<Integer,Integer>,TreeSet<Pair<Integer,Integer>>> sets) {
//		TreeSet<Pair<Integer,Integer>> set1 = sets.get(key1);
//		TreeSet<Pair<Integer,Integer>> set2 = sets.get(key2);
//		if (set1.contains(key2) || set2.contains(key1)) return true;
//		for (Pair<Integer,Integer> item1 : set1) {
//			for (Pair<Integer,Integer> item2 : set2) {
//				if (Math.abs(item1.first-item2.first) <= 1 && Math.abs(item1.second-item2.second) <= 1) {
//					return true;
//				}
//			}
//		}
//		return false;
//	}
//	
//	//abc
//	//h d
//	//gfe
//	private ArrayList<Pair<Integer,Integer>> reducePoints(TreeSet<Pair<Integer,Integer>> set) {
//		ArrayList<Pair<Integer,Integer>> ret = new ArrayList<Pair<Integer,Integer>>();
//		
//		Pair<Integer,Integer> pt = set.first();
//		while (set.contains(Pair.make(pt.first, pt.first+1))) {
//			pt = Pair.make(pt.first, pt.first+1);
//		}
//		
//		for (Pair<Integer,Integer> pt : set) {
//			boolean grid[][] = new boolean[3][3];
//			for ()
//		}
//		
//	}
//	
//	/**
//	 * Reduce boolean grid point to sets of polygon vertex-equivalents
//	 * @param grid boolean grid (e.g. a screen of pixels) where true is inside a region of interest
//	 * @param xsz x dimension size
//	 * @param ysz y dimension size
//	 * @return List of lists of grid points indicating corresponding polygon vertices.
//	 * It is assumed that there is some outside interpretation of the grid, if necessary
//	 */
//	public static ArrayList<ArrayList<Pair<Integer,Integer>>> reduceGridPoints (boolean[][] grid, int xsz, int ysz) {
//		Hashtable<Pair<Integer,Integer>,TreeSet<Pair<Integer,Integer>>> sets = new Hashtable<Pair<Integer,Integer>,TreeSet<Pair<Integer,Integer>>>();
//		ArrayList<Pair<Integer,Integer>> keys = new ArrayList<Pair<Integer,Integer>>();
//		for (int x = 0; x < xsz; x++) {
//			for (int y = 0; y < ysz; y++) {
//				if (grid[x][y]) {
//					TreeSet<Pair<Integer,Integer>> set = new TreeSet<Pair<Integer,Integer>>();
//					sets.put(Pair.make(x, y), set);
//					keys.add(Pair.make(x, y));
//				}
//			}
//		}
//		int i = 0;
//		while (i < keys.size()) {
//			int j = i+1;
//			while (j < keys.size()) {
//				if (!sets.containsKey(keys.get(i))) break; // next i
//				if (!sets.containsKey(keys.get(j))) continue; // next j
//				Pair<Integer,Integer> key1 = find(keys.get(i), sets);
//				Pair<Integer,Integer> key2 = find(keys.get(j), sets);
//				if (adjacent(key1,key2,sets)) {
//					union(key1,key2,sets);
//				}
//				j++;
//			}
//			i++;
//		}
//		// sets unioned
//		ArrayList<ArrayList<Pair<Integer,Integer>>> polylist = new ArrayList<ArrayList<Pair<Integer,Integer>>>();
//		for (Pair<Integer,Integer> key : sets.keySet()) {
//			
//		}
//	}

}
