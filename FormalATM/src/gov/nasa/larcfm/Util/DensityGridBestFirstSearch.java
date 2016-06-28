/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.List;

/**
 * This assumes that a grid weight of 100+ is impassible
 */
public class DensityGridBestFirstSearch implements DensityGridSearch {

	class fringeCompare implements Comparator<Triple<Integer,Integer,Double>> {
		@Override
		public int compare(Triple<Integer,Integer,Double> c1, Triple<Integer,Integer,Double> c2) {
			return Double.compare(c1.third, c2.third);
		}
	}

	ArrayList<Pair<Integer,Integer>> bfs(DensityGrid dg, int endx, int endy, ArrayList<Triple<Integer,Integer,Double>> fringe, ArrayList<Pair<Integer,Integer>> searched) {
		while (fringe.size() > 0) {
			Triple<Integer,Integer,Double> c = fringe.remove(0);
			Pair<Integer,Integer> cell = Pair.make(c.first, c.second);
			if (c.first == endx && c.second == endy) {
				ArrayList<Pair<Integer,Integer>> path = new ArrayList<Pair<Integer,Integer>>();
				path.add(cell);
				return path;
			} else {
				ArrayList<Triple<Integer,Integer,Double>> fringe2 = new ArrayList<Triple<Integer,Integer,Double>>();
				for (int x = -1; x <= 1; x++) {
					for (int y = -1; y <= 1 ; y++) {
						double localWeight = dg.getWeight(c.first,c.second);
						double cost = c.third + localWeight + Math.max(Math.abs(endx-c.first), Math.abs(endy-c.second)); // weight plus distance from end
						Triple<Integer,Integer,Double> c2 = Triple.make(c.first+x, c.second+y, cost);
						if (localWeight < 100.0 && dg.containsCell(Pair.make(c2.first, c2.second)) && !searched.contains(Pair.make(c2.first,c2.second))) {
							fringe2.add(0,c2);
						}
					}
				}
				if (fringe2.size() > 0) {
					Collections.sort(fringe2, new fringeCompare());
					
					ArrayList<Pair<Integer,Integer>> searched2 = new ArrayList<Pair<Integer,Integer>>();
					searched2.addAll(searched);
					searched2.add(cell);
					ArrayList<Pair<Integer,Integer>> ret = bfs(dg, endx, endy, fringe2, searched2);
					if (ret != null) {
						ret.add(0,cell);
						return ret;
					}
				}
			}
		}
		return null;
	}

	public List<Pair<Integer, Integer>> search(DensityGrid dg, Position startPos, Position endPos) {
		Pair<Integer,Integer> start = dg.gridPosition(startPos);
		Pair<Integer,Integer> end = dg.gridPosition(endPos);
		ArrayList<Triple<Integer,Integer,Double>> fringe = new ArrayList<Triple<Integer,Integer,Double>>();
		fringe.add(Triple.make(start.first, start.second, dg.getWeight(start)));
		return bfs(dg, end.first, end.second, fringe, new ArrayList<Pair<Integer,Integer>>());
	}


}
