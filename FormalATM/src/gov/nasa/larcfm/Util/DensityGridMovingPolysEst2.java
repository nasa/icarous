/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.List;

public class DensityGridMovingPolysEst2 extends DensityGrid {
	
	List<PolyPath> paths;
	double gs;

	public DensityGridMovingPolysEst2(Plan p, int buffer, double squareSize, double gs, List<PolyPath> ps) {
		super(p, buffer, squareSize);
		paths = ps;
		startTime = p.getFirstTime();
		this.gs = gs;
	}

	public double getWeight(int x, int y) {
		if (!weights.containsKey(Pair.make(x,y))) return Double.POSITIVE_INFINITY;
		double w = weights.get(Pair.make(x,y));
		double cost = 0;
		Position cent = center(x,y);
		Pair<Integer,Integer> startCell = this.gridPosition(startPoint);
		// estimate time for this square
		double dt = (Math.abs(startCell.first-x)+Math.abs(startCell.second-y))*squareSize/gs*1.25; // should be dist from path
		for (int i = 0; i < paths.size(); i++) {
			if (paths.get(i).contains2D(cent, startTime+dt)) {
				cost = Double.POSITIVE_INFINITY;
				break;
			}
		}
		return w + cost;
	}

	public double getWeight(Pair<Integer,Integer> pii) {
		return getWeight(pii.first,pii.second);
	}


	
}
