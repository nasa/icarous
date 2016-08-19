/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.List;

public class DensityGridMovingPolys extends DensityGridTimed {

	protected List<PolyPath> paths;
	protected List<PolyPath> contains;

	public DensityGridMovingPolys(Plan p, int buffer, double squareSize, double gs, List<PolyPath> ps, List<PolyPath> cs) {
		super(p, buffer, squareSize);
		paths = ps;
		contains = cs;
		startTime = p.getFirstTime();
		double endT = -1.0; 
		this.gs = gs;
	}

	//	double getWeight(int x, int y, double t) {
	//		if (!weights.containsKey(Pair.make(x,y))) return Double.POSITIVE_INFINITY;
	//		double w = weights.get(Pair.make(x,y));
	//		double cost = 0;
	//		Position cent = center(x,y);
	//		Position corner1 = getPosition(x,y).interpolate(center(x,y), 0.5);
	//		Position corner2 = getPosition(x+1,y).interpolate(center(x,y), 0.5);
	//		Position corner3 = getPosition(x,y+1).interpolate(center(x,y), 0.5);
	//		Position corner4 = getPosition(x+1,y+1).interpolate(center(x,y), 0.5);
	//		double c0 = 0;
	//		double c1 = 0;
	//		double c2 = 0;
	//		double c3 = 0;
	//		double c4 = 0;
	//		for (int i = 0; i < paths.size(); i++) {
	//			if (t >= paths.get(i).getFirstTime() && t <= paths.get(i).getLastTime()) {
	//				SimplePoly poly = paths.get(i).position(t);
	//				double r = poly.boundingCircleRadius();
	//				Position c = poly.centroid();
	//				if (poly.contains2D(cent)) {
	//					double dist = c.distanceH(cent);
	//					c0 = c0 + (r - dist);
	//				}
	//				if (poly.contains2D(corner1)) {
	//					double dist = c.distanceH(corner1);
	//					c1 = c1 + (r - dist);
	//				}
	//				if (poly.contains2D(corner2)) {
	//					double dist = c.distanceH(corner2);
	//					c2 = c2 + (r - dist);
	//				}
	//				if (poly.contains2D(corner3)) {
	//					double dist = c.distanceH(corner3);
	//					c3 = c3 + (r - dist);
	//				}
	//				if (poly.contains2D(corner4)) {
	//					double dist = c.distanceH(corner4);
	//					c4 = c4 + (r - dist);
	//				}
	//				if (c0 > 0 && c1 > 0 && c2 > 0 && c3 >0 && c4 > 0) {
	//					cost = Double.POSITIVE_INFINITY;
	//					break;
	//				}
	//			}
	//			cost = c0+c1+c2+c3+c4;
	//		}
	//		return w + cost;
	//	}


	public double getWeight(int x, int y, double t) {
//f.pln("DensityGridMovingPoly getWeight x="+x+" y="+y+" t="+t);		
		if (lookaheadEndTime > 0 && t > lookaheadEndTime) {
//			f.pln("--DensityGridMovingPoly getWeight out of time = 0");
			return 0.0;
		}
		if (!weights.containsKey(Pair.make(x,y))) {
//			f.pln("--DensityGridMovingPoly getWeight not in graph = INF");
			return Double.POSITIVE_INFINITY;
		}
		double w = weights.get(Pair.make(x,y));
		double cost = 0;
		Position cent = center(x,y);
		for (int i = 0; i < paths.size(); i++) {
			if (paths.get(i).contains2D(cent, t)) {
				cost = Double.POSITIVE_INFINITY;
//				f.pln("--DensityGridMovingPoly getWeight hit polygon "+i+" = INF");
				break;
			}
		}
		if (contains != null) {
			boolean within = (contains.size() == 0); // no containment is vacuously fulfilled
			for (int i = 0; i < contains.size(); i++) {
				if (contains.get(i).contains2D(cent, t)) {
					within = true;
					break;
				}
			}
			if (!within) {
				cost = Double.POSITIVE_INFINITY;
//				f.pln("--DensityGridMovingPoly getWeight failed contaiment = INF");
			}
		}
		return w + cost;
	}

	public double getWeight(Triple<Integer,Integer,Integer> pii) {
		return getWeight(pii.first,pii.second, pii.third);
	}



}
