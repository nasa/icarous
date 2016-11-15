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

public class DensityGridAStarSearch implements DensityGridSearch, DensityGridTimedSearch {

	public double dirWeight = 1.0; 
	public double distWeight = 1.0;

	class FringeEntry {
		int x;
		int y;
		double t = 0;
		double cost;
		ArrayList<Pair<Integer,Integer>> path;

		public FringeEntry(Pair<Integer,Integer> xy, double cc) {
			x = xy.first;
			y = xy.second;
			cost = cc;
			path = new ArrayList<Pair<Integer,Integer>>();
			path.add(xy);
		}

		public FringeEntry(Pair<Integer,Integer> xy, double cc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			cost = f.cost + cc;
			path = new ArrayList<Pair<Integer,Integer>>();
			path.addAll(f.path);
			path.add(xy);			
		}

		public FringeEntry(Pair<Integer,Integer> xy, double t, double cc) {
			x = xy.first;
			y = xy.second;
			this.t = t; 
			cost = cc;
			path = new ArrayList<Pair<Integer,Integer>>();
			path.add(xy);
		}

		public FringeEntry(Pair<Integer,Integer> xy, double t, double cc, FringeEntry f) {
			x = xy.first;
			y = xy.second;
			this.t = t; 
			cost = f.cost + cc;
			path = new ArrayList<Pair<Integer,Integer>>();
			path.addAll(f.path);
			path.add(xy);			
		}

		public Pair<Integer,Integer> getCell() {
			return Pair.make(x, y);
		}

		public Triple<Integer,Integer,Integer> getCell3() {
			return Triple.make(x, y, (int)t);
		}

		public double getCost() {
			return cost;
		}

		public ArrayList<Pair<Integer,Integer>> getPath() {
			return path;
		}

		public String toString() {
			return "("+x+","+y+","+t+") = "+cost;
		}
	}


	class FringeCompare implements Comparator<FringeEntry> {
		@Override
		public int compare(FringeEntry c1, FringeEntry c2) {
			return Double.compare(c1.cost, c2.cost);
		}
	}

	FringeCompare comp = new FringeCompare();

	/** 
	 * Return true if cell2 is in the "same direction" as the previous search (not more than a 90 degree turn in the search needed)
	 */
	boolean sameDirection(FringeEntry c, Pair<Integer,Integer> cell2) {
		if (c.path.size() == 0) return true; // no history, go anywhere
		Pair<Integer,Integer> cell1 = c.path.get(c.path.size()-1);
		int dx1 = c.x - cell1.first;
		int dy1 = c.y - cell1.second;
		int dx2 = cell2.first - c.x;
		int dy2 = cell2.second - c.y;
		return ((dx1 == dx2 && Math.abs(dy1-dy2) == 1) || (dy1 == dy2 && Math.abs(dx1-dx2) == 1));
	}

	// this gives a value of 0 to same direction, 1 to a 45 degree turn, 2 to a 90 degree turn, and 3 to a 135 degree turn, multiplied by the dirWeigh
	double directionCost(FringeEntry c, Pair<Integer,Integer> cell2) {
		if (c.path.size() == 0) return 0.0; // no history, go anywhere
		Pair<Integer,Integer> cell1 = c.path.get(c.path.size()-1);
		int dx1 = c.x - cell1.first;
		int dy1 = c.y - cell1.second;
		int dx2 = cell2.first - c.x;
		int dy2 = cell2.second - c.y;
		return (Math.abs(dx2-dx1)+Math.abs(dy2-dy1))*dirWeight;
	}

	// this computes a distance from the end point, in squares, multiplied by the distWeight
	double distanceCost(Pair<Integer,Integer> cell2, int endx, int endy) {
		//		double sqdist = Math.max(Math.abs(endx-cell2.first), Math.abs(endy-cell2.second));
		double sqdist = (new Vect2(endx,endy)).Sub(new Vect2(cell2.first, cell2.second)).norm();
		return sqdist*distWeight;
	}

	ArrayList<Pair<Integer,Integer>> astar(DensityGrid dg, int endx, int endy, ArrayList<FringeEntry> fringe, ArrayList<Pair<Integer,Integer>> searched) {
		while (fringe.size() > 0) {
			Collections.sort(fringe, comp);
			FringeEntry c = fringe.remove(0);
			if (Double.isFinite(c.cost)) { // ignore infinite cost entries
				if (c.x == endx && c.y == endy) {
					return c.getPath();
				} else {
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1 ; y++) {
							Pair<Integer,Integer> cell2 = Pair.make(c.x+x, c.y+y);
							if (dg.containsCell(cell2) && !searched.contains(cell2)) {
								double cost2 = dg.getWeight(cell2) + distanceCost(cell2,endx,endy) + directionCost(c,cell2);
								//f.pln("x="+x+" y="+y+" cost="+cost2);								
								FringeEntry c2 = new FringeEntry(cell2, cost2, c);
								if (!dg.searchedWeights.containsKey(cell2)) {
									dg.setSearchedWeight(c2.x, c2.y, c2.cost);
								}
								fringe.add(c2);
								searched.add(cell2);
							}
						}
					}
				}
			}
		}
		return null;
	}


	// in this one, searched includes x, y, and source square (as  
	ArrayList<Pair<Integer,Integer>> astarT(DensityGridTimed dg, int endx, int endy, double gs, ArrayList<FringeEntry> fringe, ArrayList<Triple<Integer,Integer,Integer>> searched) {
		double basedt =  dg.center(0,0).distanceH(dg.center(0,1))/gs; // base time to move one square n/s
		int cnt = 0;
		while (fringe.size() > 0) {
			Collections.sort(fringe, comp);
			FringeEntry c = fringe.remove(0);
			cnt++;
			if (Double.isFinite(c.cost)) { // ignore infinite cost entries
				if (c.x == endx && c.y == endy) {
					return c.getPath();
				} else {
					for (int x = -1; x <= 1; x++) {
						for (int y = -1; y <= 1 ; y++) {
							Pair<Integer,Integer> cell2 = Pair.make(c.x+x, c.y+y);
							Position pos1 = dg.center(c.x,c.y);
							Position pos2 = dg.center(cell2);
							double dist = pos1.distanceH(pos2);
							if (!Double.isNaN(dist)) { // dist == NaN if either position is invalid (i.e. center cannot be calculated)
								double dt = dist/gs;
								//							double normdt = dt/basedt;
								double t = c.t + dt;
								if (Double.isNaN(t)) {
								    //f.pln("t="+t+" c.t="+c.t+" dt="+dt+" dist="+dist+" gs="+gs+" ");
								}

								//							double normT = t/basedt;
								Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, 0); // do not allow revisiting cells, eventually change this to a pair?			
								//							Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, y*3+x);							
								//							Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, (int)t);
								if (dg.containsCell(cell2) && !searched.contains(searchedcell3)) {
									double cost2 = dg.getWeight(c.x+x, c.y+y, t) + distanceCost(cell2,endx,endy) + directionCost(c,cell2); 
									FringeEntry c2 = new FringeEntry(cell2, t, cost2, c);
									if (!dg.searchedWeights.containsKey(cell2)) {
										dg.setSearchedWeight(c2.x, c2.y, cost2);
									}
									fringe.add(c2);
									searched.add(searchedcell3);
								}
							}
						}
					}
				}
			}
		}
		return null;
	}

	//	/**
	//	 * Search against a range of times.  This performs a normal astar search if timeBefore or timeAfter < 0, or if interval is <= 0
	//	 * @param dg timed grid
	//	 * @param endx x grid position of end point
	//	 * @param endy y grid position of end point
	//	 * @param gs nominal groundspeed
	//	 * @param fringe list of to-be-searched-next nodes
	//	 * @param searched list of already searched nodes
	//	 * @param timeBefore (positive, relative) time before the given aircraft's path to check 
	//	 * @param timeAfter (positive, relative) time after the given aircraft's path to check
	//	 * @param interval interval for number of checks between timeBefore and timeAfter
	//	 * @return list of grid x-y coordinates for successful path, or null if no path.
	//	 */
	//	ArrayList<Pair<Integer,Integer>> astarT(DensityGridTimed dg, int endx, int endy, double gs, ArrayList<FringeEntry> fringe, ArrayList<Triple<Integer,Integer,Integer>> searched, double timeBefore, double timeAfter, double interval) {
	//		double basedt =  dg.center(0,0).distanceH(dg.center(0,1))/gs; // base time to move one square n/s
	//		int cnt = 0;
	//		while (fringe.size() > 0) {
	//			Collections.sort(fringe, comp);
	//			FringeEntry c = fringe.remove(0);
	//			cnt++;
	//			if (Double.isFinite(c.cost)) { // ignore infinite cost entries
	//				if (c.x == endx && c.y == endy) {
	//					return c.getPath();
	//				} else {
	//					for (int x = -1; x <= 1; x++) {
	//						for (int y = -1; y <= 1 ; y++) {
	//							Pair<Integer,Integer> cell2 = Pair.make(c.x+x, c.y+y);
	//							double dist = dg.center(c.x,c.y).distanceH(dg.center(cell2));
	//							double dt = dist/gs;
	////							double normdt = dt/basedt;
	//							double t = c.t + dt;
	////							double normT = t/basedt;
	//							Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, 0); // do not allow revisiting cells, eventually change this to a pair?			
	////							Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, y*3+x);							
	////							Triple<Integer,Integer,Integer> searchedcell3 = Triple.make(c.x+x, c.y+y, (int)t);
	//							if (dg.containsCell(cell2) && !searched.contains(searchedcell3)) {
	//								double baseWgt = dg.getWeight(c.x+x, c.y+y, t);
	//								if (interval > 0.0 && timeBefore >= 0.0 || timeAfter >= 0.0) {
	//									// get largest weight between t-timeBefore and t+timeAfter, short cutting if we hit infinity.  do not start before time 0.
	//									for (double tt = Math.max(0, t-timeBefore); tt <= t+timeAfter && Double.isFinite(baseWgt); tt += interval) {
	//										baseWgt = Math.max(baseWgt, dg.getWeight(c.x+x, c.y+y, tt));
	//									}
	//								}
	//								double cost2 = baseWgt + distanceCost(cell2,endx,endy) + directionCost(c,cell2); 
	//								FringeEntry c2 = new FringeEntry(cell2, t, cost2, c);
	//								if (!dg.searchedWeights.containsKey(cell2)) {
	//									dg.setSearchedWeight(c2.x, c2.y, cost2);
	//								}
	//								fringe.add(c2);
	//								searched.add(searchedcell3);
	//							}
	//						}
	//					}
	//				}
	//			}
	//		}
	//		return null;
	//	}

	/**
	 * Perform an astar search through the given DensitGrid, starting at startPos and ending at endPos.
	 * This assumes the grid has a set of static polygons
	 * Returns a list of grid coordinates defining a path, or null if no path found.
	 * This has the side effect of setting the searchedWeights values in the DensityGrid for display or analysis purposes.
	 */
	public List<Pair<Integer, Integer>> search(DensityGrid dg, Position startPos, Position endPos) {
		Pair<Integer,Integer> start = dg.gridPosition(startPos);		
		Pair<Integer,Integer> end = dg.gridPosition(endPos);
		ArrayList<FringeEntry> fringe = new ArrayList<FringeEntry>();
		ArrayList<Pair<Integer,Integer>> searched = new ArrayList<Pair<Integer,Integer>>();
		searched.add(start);
		double firstWeight = dg.getWeight(start);
		dg.setSearchedWeight(start.first, start.second, firstWeight);
		if (Double.isInfinite(firstWeight)) return null; // first cell is invalid, abort search
		fringe.add(new FringeEntry(start, 1, dg.getWeight(start)));
		return astar(dg, end.first, end.second, fringe, searched);
	}

	/**
	 * Perform a search on the given densitygrid, assuming a constant groundspeed.  Polygons may be static or moving.
	 * @param dg Timed density grid with polygon and initial weight information.  Polygons may be mosing or static.
	 * @param startPos Start position for search.
	 * @param endPos Ending Position for search.
	 * @param startTime Absolute time to start the search at startPos
	 * @param gs ground speed of aircraft
	 * @return List of grid coordinates for a successful path, or null if no path found.
	 */
	public List<Pair<Integer, Integer>> search(DensityGridTimed dg, Position startPos, Position endPos, double startTime, double gs) {
		Pair<Integer,Integer> start = dg.gridPosition(startPos);
		Pair<Integer,Integer> end = dg.gridPosition(endPos);
		ArrayList<FringeEntry> fringe = new ArrayList<FringeEntry>();
		ArrayList<Triple<Integer,Integer,Integer>> searched = new ArrayList<Triple<Integer,Integer,Integer>>();
		//		searched.add(Triple.make(start.first, start.second, dg.cellNum(start)));
		searched.add(Triple.make(start.first, start.second, 0));
		double firstWeight = dg.getWeight(start);
		dg.setSearchedWeight(start.first, start.second, firstWeight);
		if (Double.isInfinite(firstWeight)) return null; // first cell is invalid, abort search
		fringe.add(new FringeEntry(start, startTime, dg.getWeight(start)));
		return astarT(dg, end.first, end.second, gs, fringe, searched);
	}

	//	/**
	//	 * Perform a search on the given timed density grid assuming a constant groundspeed.  Polygons may be static or moving.  
	//	 * This search includes a user-specified time buffer around the given path, meaning the path will be will be clear not only 
	//	 * for the given ownship trajectory, but also for a user-specified time before and after (along the same trajectory).
	//	 * @param dg Timed density grid with polygon and initial weight information.  Polygons may be moving or static.
	//	 * @param startPos Start position for search.
	//	 * @param endPos Ending Position for search.
	//	 * @param startTime Absolute time to start the search at startPos
	//	 * @param gs ground speed of aircraft
	//	 * @param timeBefore positive, relative time before the ownship's path that must remain clear (absolute times before zero are ignored)
	//	 * @param timeAfter positive, relative time after the ownship's path that must remain clear
	//	 * @param interval polling interval for checks with the time range (suggest 1-5 mins for normal weather sizes and speeds)
	//	 * @return List of grid coordinates for a successful path, or null if no path found.
	//	 */
	//	public List<Pair<Integer, Integer>> search(DensityGridTimed dg, Position startPos, Position endPos, double startTime, double gs, double timeBefore, double timeAfter, double interval) {
	//		Pair<Integer,Integer> start = dg.gridPosition(startPos);
	//		Pair<Integer,Integer> end = dg.gridPosition(endPos);
	//		ArrayList<FringeEntry> fringe = new ArrayList<FringeEntry>();
	//		ArrayList<Triple<Integer,Integer,Integer>> searched = new ArrayList<Triple<Integer,Integer,Integer>>();
	////		searched.add(Triple.make(start.first, start.second, dg.cellNum(start)));
	//		searched.add(Triple.make(start.first, start.second, 0));
	//		dg.setSearchedWeight(start.first, start.second, dg.getWeight(start));
	//		fringe.add(new FringeEntry(start, startTime, dg.getWeight(start)));
	//		return astarT(dg, end.first, end.second, gs, fringe, searched, timeBefore, timeAfter, interval);
	//	}

}
