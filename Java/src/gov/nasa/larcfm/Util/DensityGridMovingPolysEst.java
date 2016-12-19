/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.List;

public class DensityGridMovingPolysEst extends DensityGridTimed {

	ArrayList<Triple<Plan,Double,Double>> plans; // plan, width, height
	ArrayList<Triple<Plan,Double,Double>> contains;

	public DensityGridMovingPolysEst(Plan p, int buffer, double squareSize, double gs, List<PolyPath> ps, List<PolyPath> containment) {
		super(p, buffer, squareSize);
		plans = new ArrayList<Triple<Plan,Double,Double>>();
		for (int i = 0; i < ps.size(); i++) {
//f.pln("DensityGridMovingPolysEst() i="+i+" ps i="+ps.get(i));
			Triple<Plan,Double,Double> pp = ps.get(i).buildPlan();
			plans.add(pp);
		}
		contains = new ArrayList<Triple<Plan,Double,Double>>();
		if (containment != null) {
			for (int i = 0; i < containment.size(); i++) {
				// static plans have already been handled in StratwayCore with a call to setWeightsWithin()
				if (!containment.get(i).isStatic()) {
					Triple<Plan,Double,Double> cc = containment.get(i).buildPlan();
					contains.add(cc);
				}
			}
		}
		startTime = p.getFirstTime();
//		double endT = -1.0; 
		this.gs = gs;
	}

	//	public DensityGridMovingPolysEst(Plan p, int buffer, double squareSize, double gs, List<PolyPath> ps, List<Plan> traffic, double D, double H) {
	//		super(p, buffer, squareSize);
	//		plans = new ArrayList<Triple<Plan,Double,Double>>();
	//		for (int i = 0; i < ps.size(); i++) {
	//			Triple<Plan,Double,Double> pp = ps.get(i).buildPlan();
	//			plans.add(pp);
	//		}
	//		for (int i = 0; i < traffic.size(); i++) {
	//			Triple<Plan,Double,Double> pp = Triple.make(traffic.get(i), D, H);
	//			plans.add(pp);
	//		}
	//		startTime = p.getFirstTime();
	//		this.gs = gs;
	//		
	//	}	

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
	//		for (int i = 0; i < plans.size(); i++) {
	//			if (t >= plans.get(i).first.getFirstTime() && t <= plans.get(i).first.getLastTime()) {
	//				Position p = plans.get(i).first.position(t);
	//				double D = plans.get(i).second;
	//				
	//				double d0 = cent.distanceH(p);
	//				if (d0 <= D) {
	//					c0 = 1;
	//				}
	//				double d1 = corner1.distanceH(p);
	//				if (d1 <= D) {
	//					c1 = 1;
	//				}
	//				double d2 = corner2.distanceH(p);
	//				if (d2 <= D) {
	//					c2 = 1;
	//				}
	//				double d3 = corner3.distanceH(p);
	//				if (d3 <= D) {
	//					c3 = 1;
	//				}
	//				double d4 = corner4.distanceH(p);
	//				if (d4 <= D) {
	//					c4 = 1;
	//				}
	//				
	//				if (c0 > 0 && c1 > 0 && c2 > 0 && c3 >0 && c4 > 0) {
	//					cost = Double.POSITIVE_INFINITY;
	//					break;
	//				}
	//			}
	//			cost = (c0+c1+c2+c3+c4)*2;
	//		}
	//		return w + cost;
	//	}



	public double getWeightT(int x, int y, double t) {
//if (Double.isNaN(t)) {
//Thread.dumpStack();
//f.pln("DensityGridMovingPolysEst getWeight x="+x+" y="+y+" t="+t);	
//}
		if (lookaheadEndTime > 0 && t > lookaheadEndTime) {
//f.pln("--DensityGridMovingPolysEst getWeight out of time = 0");
			return 0.0;
		}
		if (!weights.containsKey(Pair.make(x,y))) {
//f.pln("--DensityGridMovingPolysEst getWeight not in graph = INF");
			return Double.POSITIVE_INFINITY;
		}
		double w = weights.get(Pair.make(x,y));
		double cost = 0;
		Position cent = center(x,y);
		// disallow anything within one of the weather cells
		for (int i = 0; i < plans.size(); i++) {
			if (t >= plans.get(i).first.getFirstTime() && t <= plans.get(i).first.getLastTime()) {
				Position p = plans.get(i).first.position(t);
				double D = plans.get(i).second;
				double centdist = cent.distanceH(p); 
				if (centdist <= D) {
					cost = Double.POSITIVE_INFINITY;
//f.pln("--DensityGridMovingPolysEst getWeight hit polygon "+i+" = INF D="+Units.str("nmi", D)+" centdist="+Units.str("nmi", centdist)+" cent="+cent);
					break;
				}
			}
		}
		
		// disallow anything outside of one of the contains
		boolean within = (contains.size() == 0);
		for (int i = 0; i < contains.size(); i++) {
			if (t >= contains.get(i).first.getFirstTime() && t <= contains.get(i).first.getLastTime()) {
				Position p = contains.get(i).first.position(t);
				double D = contains.get(i).second;
				double centdist = cent.distanceH(p); 
				if (centdist <= D) {
					within = true;
					break;
				}
			}
		}
		if (!within) {
//f.pln("--DensityGridMovingPolysEst getWeight failed contaiment = INF");
			cost = Double.POSITIVE_INFINITY;
		}
		
		return w + cost;
	}

	
	//	double costBetween(int x1, int y1, int x2, int y2, double t1) {
	//		double dt = Math.sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1))*5;
	//		Position c1 = center(x1,y1);
	//		Position c2 = center(x2,y2);
	//		Velocity v = c1.initialVelocity(c2, dt);
	//		double w = 0.0;
	//		for (double t = 1; t < dt; t++) {
	//			Pair<Integer,Integer> xy= gridPosition(c1.linear(v, t));
	//			w += getWeight(xy,t1+t);
	//		}
	//		return w;
	//	}
	//
	//	double costBetween(Pair<Integer,Integer> xy1, Pair<Integer,Integer> xy2, double t1) {
	//		return costBetween(xy1.first, xy1.second, xy2.first, xy2.second, t1);
	//	}

	boolean onPath(Pair<Integer,Integer> xy1, Pair<Integer,Integer> xy2, List<Pair<Integer,Integer>> gp) {
		double dt = Math.sqrt((xy2.first-xy1.first)*(xy2.first-xy1.first)+(xy2.second-xy1.second)*(xy2.second-xy1.second))*5;
		Position c1 = center(xy1);
		Position c2 = center(xy2);
		Velocity v = c1.initialVelocity(c2, dt);
		for (int i = 0; i < dt; i++) {
			if (!gp.contains(gridPosition(c1.linear(v,i)))) return false;
		}
		return true;
	}

	/**
	 * This attempts to remove as many points as possible while still retaining the given gridpath.
	 */
	public List<Pair<Integer,Integer>> reduceGridPath(List<Pair<Integer,Integer>> gp) {
		List<Pair<Integer,Integer>> gp1 = thin(gp);
//		List<Pair<Integer,Integer>> gp1 = gp;
		List<Pair<Integer,Integer>> gp2 = new ArrayList<Pair<Integer,Integer>>();
		int j = 0;
		gp2.add(gp1.get(j));
		for (int i = 1; i < gp1.size(); i++) {
			if (!onPath(gp1.get(j),gp1.get(i),gp) || i == gp1.size()-1) {
				gp2.add(gp1.get(i));
				j = i;
			} 
		}
		return gp2;
	}


	public double getWeightT(Triple<Integer,Integer,Integer> pii) {
		return getWeightT(pii.first,pii.second, pii.third);
	}

	@Override
	public String toString() {
		return "DensityGridMovingPolysEst [startTime=" + startTime + ", gs="
				+ gs + ", startPoint=" + startPoint + ", endPoint=" + endPoint
				//				+ ", weights=" + weights + ", corners=" + corners
				//				+ ", searchedWeights=" + searchedWeights + ", marked=" + marked
				+ ", latLon=" + latLon + ", squareSize=" + squareSize
				+ ", squareDist=" + squareDist + ", sz_x=" + sz_x + ", sz_y="
				+ sz_y + ", bounds=" + bounds + "]";
	}



}
