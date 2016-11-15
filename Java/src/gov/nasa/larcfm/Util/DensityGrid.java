/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.Util;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.Hashtable;
import java.util.List;

public class DensityGrid {

	protected Position startPoint = null;
	protected double startTime = -1;
	protected Position endPoint;;
	protected Hashtable<Pair<Integer,Integer>,Double> weights = null;
	protected Hashtable<Pair<Integer,Integer>,Pair<Double,Double>> corners = null;
	protected Hashtable<Pair<Integer,Integer>,Double> searchedWeights = null;
	protected HashSet<Pair<Integer,Integer>> marked = null;
	protected boolean latLon;
	protected double squareSize;
	protected double squareDist;
	protected int sz_x = 0;
	protected int sz_y = 0;
	protected BoundingRectangle bounds; // overall bounds
	protected double minSearchedWeightValue = Double.MAX_VALUE;
	protected double maxSearchedWeightValue = -Double.MAX_VALUE;
	//	protected List<Pair<Integer,Integer>> foundPath = null;



	public DensityGrid() {
		this.startPoint = Position.INVALID;
		this.startTime = -1;
		this.endPoint = Position.INVALID;
		this.weights = new Hashtable<Pair<Integer,Integer>,Double>();
		this.corners = new Hashtable<Pair<Integer,Integer>,Pair<Double,Double>>();
		this.searchedWeights = new Hashtable<Pair<Integer,Integer>,Double>();
		this.marked = new HashSet<Pair<Integer,Integer>>();
		this.latLon = true;
		this.squareSize = 0.0;
		this.squareDist = 0.0;
		this.sz_x = 0;
		this.sz_y = 0;
		this.bounds = new BoundingRectangle();
	}

	// grid coordinates refer to the bottom left (SW) corner of the square
	// buffer is number of extra squares in each direction from the base rectangle
	public DensityGrid(BoundingRectangle b, NavPoint start, Position end, int buffer, double sqSz, boolean ll) {
		init(b, start, end, buffer, sqSz, ll);
	}

	// grid coordinates refer to the bottom left (SW) corner of the square
	// buffer is number of extra squares in each direction from the base rectangle
	public DensityGrid(Plan p, int buffer, double squareSize) {
		init(p.getBound(), p.point(0), p.point(p.size()-1).position(), buffer, squareSize, p.isLatLon());
	}

	public DensityGrid(Plan p, double startT, int buffer, double squareSize) {
		init(p.getBound(), new NavPoint(p.position(startT),startT), p.point(p.size()-1).position(), buffer, squareSize, p.isLatLon());
	}


	void init(BoundingRectangle b, NavPoint start, Position end, int buffer, double sqSz, boolean ll) {
		squareDist = sqSz;
		latLon = ll;
		squareSize = sqSz;
		if (latLon) { // convert this to lat/lon
			squareSize = linearEstY(0.0, sqSz);
		}
//		f.pln(" $$ init: squareSize = "+squareSize);
		bounds = new BoundingRectangle();
		startPoint = start.position();
		startTime = start.time();
		endPoint = end;
		//f.pln(" $$ init: endPoint = "+endPoint);
		weights = new Hashtable<Pair<Integer,Integer>,Double>();
		searchedWeights = new Hashtable<Pair<Integer,Integer>,Double>();
		corners = new Hashtable<Pair<Integer,Integer>,Pair<Double,Double>>();
		marked = new HashSet<Pair<Integer,Integer>>();
		// add an additional 0.5 box buffer on each side, so it's less likely to have start/end at edge of squares
		// also include an additional box to the top and right so that we have boundary positions in the table for those corners
		double maxX = b.getMaxX();
		double minX = b.getMinX();
		double maxY = b.getMaxY();
		double minY = b.getMinY();
		//f.pln("dg.init b="+b.toString());		
		sz_x = (int)Math.ceil((maxX-minX)/squareSize)+buffer*2+1;		
		sz_y = (int)Math.ceil((maxY-minY)/squareSize)+buffer*2+1;
		double dx0 = minX - (buffer)*squareSize;
		double dy0 = minY - (buffer)*squareSize;
		if (latLon && Math.signum(minX) != Math.signum(maxX) && maxX-minX > Math.PI) {
			sz_x = (int)Math.ceil((Math.PI-(maxX-minX))/squareSize)+buffer*2+1;	
			minX = maxX;
			dx0 = minX - (buffer)*squareSize;
		}
//f.pln(" $$ init sx = "+sz_x+" sy = "+sz_y);
		for (int x = 0; x <= sz_x; x++) {
			double dx = dx0 + x*squareSize; 
			//			double dx1 = dx+squareSize;
			for (int y = 0; y <= sz_y; y++) {
				double dy = dy0 + y*squareSize; 				
				//BoundingRectangle br = new BoundingRectangle();
				//f.pln(" $$ init: ("+x+","+y+"): dx = "+dx+" dy = "+dy);
				if (latLon) {
					LatLonAlt norm = LatLonAlt.normalize(dy, dx);
					dx = norm.lon();
					dy = norm.lat();
					bounds.add(norm);
				} else {
					bounds.add(dx,dy);
				}
				//f.pln("dg.init x="+x+" y="+y+" dx="+dx+" dy="+dy+" bounds="+bounds);				
				corners.put(Pair.make(x,y), new Pair<Double,Double>(dx,dy));
				//f.pln(" $$ add br = "+br);
				//f.pln(" $$$$ init ADD ("+x+","+y+")");
			}
		}
		//f.pln("dg.init bounds = "+bounds);		
		//		bounds = b;
		//f.pln(" $$ init DONE!");
	}




	//	static double linearEstX(double lati, double longi, double de) {
	//		return longi + de/(GreatCircle.spherical_earth_radius*Math.cos(lati));
	//	}

	private static double linearEstY(double lati, double dn) {
		return lati + dn/GreatCircle.spherical_earth_radius;
	}

	// distance in meters from a delta latitude
	static double distEstLatLon(double lat1, double lat2) {
		return (lat2-lat1)*GreatCircle.spherical_earth_radius;
	}

	/**
	 * Approximate size of square, in either meters (if Euclidean) or radians (if latlon)
	 * @return
	 */
	public double getNativeSquareDist() {
		return squareSize;
	}

	/**
	 * Approximate size of square, in meters
	 */
	public double getSquareDist() {
		return squareDist;
	}

	/**
	 * Adjust all grid corner coordinates so that the start point it in the center of its grid.
	 */
	public void snapToStart() {
		Position cent = center(gridPosition(startPoint));
		double offx,offy;
		if (latLon) {
			offx = startPoint.lon()-cent.lon();
			offy = startPoint.lat()-cent.lat();
		} else {
			offx = startPoint.x()-cent.x();
			offy = startPoint.y()-cent.y();
		}
		for (Pair<Integer,Integer> key : corners.keySet()) {
			Pair<Double,Double> val = corners.get(key);
			corners.put(key, Pair.make(val.first+offx, val.second+offy));
		}
	}

	//	public void mark(int x, int y) {
	//		marked.add(Pair.make(x,y));
	//	}
	//
	//	public void mark(Pair<Integer,Integer> xy) {
	//		marked.add(xy);
	//	}
	//
	//	public boolean isMarked(int x, int y) {
	//		return marked.contains(Pair.make(x, y));
	//	}
	//	
	//	public boolean isMarked(int x, int y) {
	//		return marked.contains(Pair.make(x, y));
	//	}
	//
	//	public void clearMarks() {
	//		marked.clear();
	//	}
	//	

	//	public int gridXPosition(Position p) {
	//		if (bounds.contains(p)) {
	//			double x = p.x();
	//			if (latLon) {
	//				x = p.lon();
	//			}
	//			return (int)Math.floor((x - bounds.getMinX())/squareSize);
	//		}
	//		return -1;
	//	}
	//
	//	public int gridYPosition(Position p) {
	//		if (bounds.contains(p)) {
	//			double y = p.y();
	//			if (latLon) {
	//				y = p.lat();
	//			}
	//			return (int)Math.floor((y - bounds.getMinY())/squareSize);			
	//		}
	//		return -1;
	//	}

	public Pair<Integer,Integer> gridPosition(Position p) {
		if (bounds.contains(p)) {
			double x = p.x();
			double y = p.y();

			if (latLon) {
				Position np = bounds.denormalize(p);
				x = np.lon();
				y = np.lat();
			}
			int gridx = (int)Math.floor((x - bounds.getMinX())/squareSize);
			int gridy = (int)Math.floor((y - bounds.getMinY())/squareSize);
			return Pair.make(gridx,gridy);
		}
		return Pair.make(-1, -1);
	}


	//	// iterating through the grid, return null if out of bounds
	//	public Pair<Integer,Integer>cell(int i) {
	//		if (i < 0 || i >= sz_x*sz_y) return null;
	//		return Pair.make(i/sz_x, i%sz_x);
	//	}

	//	public int cellNum(int x, int y) {
	//		if (x < 0 || y < 0 || x >= sz_x || y >= sz_y) return -1;
	//		return x * sz_x | y;
	//	}
	//
	//	public int cellNum(Pair<Integer,Integer> xy) {
	//		return cellNum(xy.first, xy.second);
	//	}

	public boolean containsCell(Pair<Integer,Integer> xy) {
		return corners.containsKey(xy);
	}

	//	public boolean containsCell(int x, int y) {
	//		return containsCell(Pair.make(x,y));
	//	}


	/**
	 * Note: the grid size should be 1 larger than expected (to allow for the first point to be in the middle of the square)
	 * The SW corner of the plan bounding box should be in square (buffer,buffer)
	 * @return
	 */
	public int sizeX() {
		return sz_x;
	}

	/**
	 * Note: the grid size should be 1 larger than expected (to allow for the first point to be in the middle of the square)
	 * @return
	 */
	public int sizeY() {
		return sz_y;
	}

	//	public int size() {
	//		return sz_x*sz_y;
	//	}

	public Position getPosition(int x, int y) {
		if (!corners.containsKey(Pair.make(x,y))) return Position.INVALID;
		Pair<Double,Double> b = corners.get(Pair.make(x,y));
		//f.pln(" $$$$$$ getPosition  x = "+x+" y = "+y+" b = "+b);
		if (latLon) {
			return Position.mkLatLonAlt(b.second, b.first, 0.0);
		} else {
			return new Position(Vect3.mkXYZ(b.first, b.second, 0.0));
		}
	}

	public Position getPosition(Pair<Integer,Integer> pii) {
		return getPosition(pii.first,pii.second);
	}

	public Position center(int x, int y) {
		if (!corners.containsKey(Pair.make(x,y)) || !corners.containsKey(Pair.make(x+1,y+1))) {
			return Position.INVALID;
		}
		Position p1 = getPosition(x,y);
		Position p2 = getPosition(x+1,y+1);
		return p1.interpolate(p2, 0.5);
	}

	public Position center(Pair<Integer,Integer> pii) {
		return center(pii.first,pii.second);
	}

	public double getWeight(int x, int y) {
		if (!weights.containsKey(Pair.make(x,y))) return Double.POSITIVE_INFINITY;
		return weights.get(Pair.make(x,y));
	}


	public double getWeight(Pair<Integer,Integer> pii) {
		return getWeight(pii.first,pii.second);
	}

	public void setWeight(int x, int y, double d) {
		if (corners.containsKey(Pair.make(x,y)) && d >= 0) {
			weights.put(Pair.make(x, y), d);
		}
	}

	public void clearWeight(int x, int y) {
		weights.remove(Pair.make(x, y));
	}

	/**
	 * Return weight determined by search, or -Infinity if not set by search 
	 * @param x
	 * @param y
	 * @return
	 */
	public double getSearchedWeight(int x, int y) {
		if (!searchedWeights.containsKey(Pair.make(x,y))) return Double.NEGATIVE_INFINITY;
		return searchedWeights.get(Pair.make(x,y));
	}

	public void setSearchedWeight(int x, int y, double d) {
		if (corners.containsKey(Pair.make(x,y)) && d >= 0) {
			searchedWeights.put(Pair.make(x, y), d);
			if (Double.isFinite(d)) {
				minSearchedWeightValue = Math.min(d, minSearchedWeightValue);
				maxSearchedWeightValue = Math.max(d, maxSearchedWeightValue);
			}
		}
	}

	public void clearSearchedWeights() {
		searchedWeights.clear();
		minSearchedWeightValue = Double.MAX_VALUE;
		maxSearchedWeightValue = -Double.MAX_VALUE;
	}

	public double getMinSearchedWeightValue() {
		return minSearchedWeightValue;
	}

	public double getMaxSearchedWeightValue() {
		return maxSearchedWeightValue;
	}

	public boolean getMark(Pair<Integer,Integer> pii) {
		return marked.contains(pii);
	}

	public boolean getMark(int x, int y) {
		return getMark(Pair.make(x, y));
	}

	public void setMark(Pair<Integer,Integer> pii, boolean b) {
		if (b) {
			marked.add(pii);
		} else {
			marked.remove(pii);
		}
	}

	public void setMark(int x, int y, boolean b) {
		setMark(Pair.make(x,y), b);
	}

	public void clearMarks() {
		marked.clear();
	}

	/**
	 * Set all weights to d in grid cells minX to maxX and minY to maxY, inclusive
	 */
	public void setWeights(int minX, int minY, int maxX, int maxY, double d) {
		for (int x = minX; x <= maxX; x++) {
			for (int y = minY; y <= maxY; y++) {
				setWeight(x,y,d);
			}
		}
	}

	/**
	 * Set all weights to d
	 */
	public void setWeights(double d) {
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				setWeight(x,y,d);
			}
		}
	}

	/**
	 * Clear all weights of grid squares whose center is outside the given (static) polygon.
	 * @param poly
	 */
	public void clearWeightsOutside(SimplePoly poly) {
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				Position p = center(x,y);
				if (!poly.contains(p)) {
					clearWeight(x,y);
				}
			}
		}		
	}

	/**
	 * set all weights of grid squares whose center is inside the given (static) polygon.
	 * @param poly
	 */
	public void setWeightsInside(SimplePoly poly, double d) {
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				Position p = center(x,y);
				if (poly.contains(p)) {
					//f.pln("setting weight for x="+x+" y="+y);
					setWeight(x,y,d);
				}
			}
		}		
	}


	public void clearWeights() {
		weights.clear();
	}

	public Position startPoint() {
		return startPoint;
	}

	public Position endPoint() {
		return endPoint;
	}

	private boolean validPair(Pair<Integer,Integer>  p) {
		if (p.first == -1) return false;
		if (p.second == -1) return false;
		return true;
	}

	public List<Pair<Integer,Integer>> gridPath(Plan p) {
		ArrayList<Pair<Integer,Integer>> rtn = new ArrayList<Pair<Integer,Integer>>(10);
		if (p.size() < 1) return rtn;
		NavPoint p0 = p.point(0);
		Pair<Integer,Integer> gpPair0 = gridPosition(p0.position());
		//rtn.add(gpPair0);
		if (p.size() < 2) return rtn;
		NavPoint pN = p.getLastPoint();
		double deltaTime = pN.time() - p0.time();
		int numSteps = 2*(sizeX() + sizeY());
		double stepSizeTm = deltaTime/numSteps;	
		//f.pln(" $$$$ gridPath: stepSizeTm = "+stepSizeTm);
		//Pair<Integer,Integer> gpPairN = gridPosition(pN.position());
		Pair<Integer,Integer> lastPair = new Pair<Integer,Integer>(-1,-1);
		for (double t = p0.time(); t < pN.time() + 1.0; t = t + stepSizeTm) {
			Position pt = p.position(t);
			//f.p(" $$$$ gridPath: pt = ("+pt.x()+","+pt.y()+")");
			Pair<Integer,Integer> gpPairt = gridPosition(pt);
			if (validPair(gpPairt) && !gpPairt.equals(lastPair)) {
				rtn.add(gpPairt);
				lastPair = gpPairt; 
				//f.p(" $$$$ gridPath: ADD gpPairt = "+gpPairt);                
			}
			//f.pln("");
		}
		return rtn;
	}

	public void printGridPath(List<Pair<Integer,Integer>> gPath) {
		if (gPath == null) f.pln("printGridPath NULL PATH");
		for (int i = 0; i < gPath.size(); i++) {
			Pair<Integer,Integer> ijPair = gPath.get(i);
			f.pln("printGridPath i = "+i+" ijPair = "+ijPair);
		}	
	}

	public List<Pair<Integer,Integer>> optimalPath() {
		DensityGridSearch dgs = new DensityGridAStarSearch();
		return dgs.search(this, this.startPoint, this.endPoint);
	}

	public List<Pair<Integer,Integer>> optimalPath(DensityGridSearch dgs) {
		return dgs.search(this, this.startPoint, this.endPoint);
	}

	//	public double cost(List<Pair<Integer,Integer>> gPath) {
	//		for (int i = 0; i < gPath.size(); i++) {
	//			double weight = gPath.get(i).ge
	//		}
	//		
	//		return 0;
	//	}

	private static enum Direction {undef, N, NE, E, SE, S, SW, W, NW};

	private Direction direction(Pair<Integer,Integer> p1, Pair<Integer,Integer> p2) {
		if (p1.first == p2.first) {
			if (p1.second == p2.second) {
				return Direction.undef;         
			} else if (p1.second < p2.second) {
				return Direction.N;
			} else {
				return Direction.S;
			}
		} else if (p1.first < p2.first) {
			if (p1.second == p2.second) {
				return Direction.W;         
			} else if (p1.second < p2.second) {
				return Direction.NW;
			} else {
				return Direction.SW;
			}
		} else {
			if (p1.second == p2.second) {
				return Direction.E;         
			} else if (p1.second < p2.second) {
				return Direction.NE;
			} else {
				return Direction.SE;
			}
		}
	}


	List<Pair<Integer,Integer>> thin(List<Pair<Integer,Integer>> gPath) {
		ArrayList<Pair<Integer,Integer>> rtn = new ArrayList<Pair<Integer,Integer>>(10);
		Pair<Integer,Integer> lastPair = gPath.get(0);
		Direction lastDirection = direction(Pair.make(0, 0),lastPair);
		rtn.add(lastPair);
		for (int i = 1; i < gPath.size(); i++) {
			Pair<Integer,Integer> pp = gPath.get(i);
			Direction dir = direction(lastPair,pp);
			//			f.pln("i="+i+" lastpair = "+lastPair+" lastDirection = "+lastDirection+" dir = "+dir);			
			if (dir == lastDirection) {
				rtn.remove(rtn.size()-1);
			}
			rtn.add(pp);
			lastPair = pp;
			lastDirection = dir;
		}
		return rtn;
	}

	/**
	 * Note: This method is overridden in some subclasses.  It will produce a (possibly smaller) set of points whose path should remain within the given gridpath.
	 * @param gp
	 * @return
	 */
	public List<Pair<Integer,Integer>> reduceGridPath(List<Pair<Integer,Integer>> gp) {
		return thin(gp);
	}

	
	public Plan gridPathToPlan(List<Pair<Integer,Integer>> gPath, double gs, double vs, boolean reduce) {
		List<Pair<Integer,Integer>> gPath2;
		if (reduce) {
			gPath2 = reduceGridPath(gPath);
		} else {
			gPath2 = thin(gPath);
		}
//f.pln("gridPathToPlan after thinning");
//printGridPath(gPath);
		Plan p =  new Plan();
		NavPoint np0 = new NavPoint(startPoint,startTime);
		p.add(np0);
		Position lastCenter = startPoint;
		double lastTime = startTime;
		double lastAlt = np0.alt();
		for (int i = 1; i < gPath2.size()-1; i++) {   // don't add first or last pair
			Position cntr = center(gPath2.get(i));
//f.pln("gridPathToPlan "+i+" coord="+gPath.get(i)+" pos="+cntr);			
			double dist = cntr.distanceH(lastCenter);
			double dt = dist/gs;
			double time = lastTime + dt;
			double newAlt = lastAlt + vs*dt;
			//f.pln(" $$$$ newAlt = "+Units.str("ft", newAlt)+" dt = "+dt+ " gs="+gs);
			cntr = cntr.mkAlt(newAlt);
			p.add(new NavPoint(cntr, time));
			//f.pln("plan adding "+cntr+", "+time);			
			lastTime = time;
			lastCenter = cntr;
			lastAlt = newAlt;
		}
		// we need to check to see if adding back the last point makes a weird kink in the end plan.
		// if so, we need to get rid of the last grid-point.
		double trk1 = p.finalVelocity(p.size()-2).trk();
		double trk2 = p.point(p.size()-1).position().track(endPoint);
		if (Util.turnDelta(trk1, trk2) > Math.PI/4 && gPath.size() > 3) {
			Position pivot = center(gPath.get(gPath.size()-3)); // check the original two-from-last point (before thinning)
																// (we're already dropping the last point)
			NavPoint nearest = p.closestPoint(pivot); // this will be the new breakpoint, if it's not already in the plan
			p.remove(p.size()-1);
			if (p.point(p.size()-1).distanceH(nearest) > squareDist) {
				p.add(nearest);
			}
			lastCenter = p.point(p.size()-1).position();
		}
		
		double dist = lastCenter.distanceH(endPoint);
		double dt = dist/gs;
		NavPoint last = new NavPoint(endPoint,lastTime+dt);
		p.add(last);
		return p;
	}



	public boolean contains(List<Pair<Integer,Integer>> gPath, Pair<Integer,Integer> pii) {
		for (int i = 0; i < gPath.size()-1; i++) {   // don't do last pair
			if (gPath.get(i).equals(pii)) {
				return true;
			}
		}
		return false;
	}


	public void setProximityWeightsOld(List<Pair<Integer,Integer>> gPath, Double factor, boolean applyToUndefined) {
		double[][] myWeights = new double[sz_x][sz_y];	
		for (int i = 0; i < sz_x; i++) {
			for (int j = 0; j < sz_y; j++) {
				Pair<Integer,Integer> pij = Pair.make(i,j);
				if (gPath.contains(pij))  myWeights[i][j] = -1.0;
				else myWeights[i][j] = 0.0;
			}
		}
		for (int i = 0; i < sz_x; i++) {
			double currentVal = sz_y*factor;
			for (int j = 0; j < sz_y; j++) {
				if (myWeights[i][j] < 0.0) {
					currentVal = factor;
				} else {
					myWeights[i][j] = currentVal;
					//f.pln(" $$1  SET weights["+i+"]["+j+"] = "+ weights[i][j]);
					currentVal = currentVal + factor;
				}
			}
			currentVal = sz_y*factor;
			for (int j = sz_y-1; j >= 0 ; j--) {
				if (myWeights[i][j] < 0.0) {
					currentVal = factor;
				}
				else {
					myWeights[i][j] = Math.min(currentVal,myWeights[i][j]);
					//f.pln(" $$2  SET weights["+i+"]["+j+"] = "+ weights[i][j]);
					currentVal = currentVal + factor;
				}
			}
		}
		for (int j = 0; j < sz_y; j++) {
			double currentVal = sz_x*factor;
			for (int i = 0; i < sz_x; i++) {
				if (myWeights[i][j] < 0.0) {
					currentVal = factor;
				} else {
					myWeights[i][j] = Math.min(currentVal,myWeights[i][j]);
					//f.pln(" $$3  SET weights["+i+"]["+j+"] = "+ weights[i][j]);
					currentVal= currentVal + factor;
				}
			}
			currentVal = sz_x*factor;
			for (int i = sz_x-1; i >= 0 ; i--) {
				if (myWeights[i][j] < 0.0) {
					currentVal = factor;
				}
				else {
					myWeights[i][j] = Math.min(currentVal,myWeights[i][j]);
					//f.pln(" $$4  SET weights["+i+"]["+j+"] = "+ weights[i][j]);
					currentVal= currentVal + factor;
				}
			}
		}		
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) { 
				if (applyToUndefined || this.weights.containsKey(Pair.make(x, y))) {
					if (myWeights[x][y] >= 0) setWeight(x,y,myWeights[x][y]);
					else setWeight(x,y,0.0);
				}
			}
		}         
	}

	public void setProximityWeights(List<Pair<Integer,Integer>> gPath, Double factor, boolean applyToUndefined) {
		double[][] myWeights = new double[sz_x][sz_y];	
		for (int i = 0; i < sz_x; i++) {
			for (int j = 0; j < sz_y; j++) {
				Pair<Integer,Integer> pij = Pair.make(i,j);
				if (gPath.contains(pij))  myWeights[i][j] = -1.0;
				else myWeights[i][j] = Double.MAX_VALUE;
			}
		}
		for (Pair<Integer,Integer> xy : gPath) {
			int x1 = xy.first;
			int y1 = xy.second;
			for (int x = 0; x < sz_x; x++) {
				for (int y = 0; y < sz_y; y++) {
					double dist = Math.sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));
					myWeights[x][y] = Math.min(myWeights[x][y], dist*factor);
				}
			}         			
		}
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				if (applyToUndefined || this.weights.containsKey(Pair.make(x, y))) {
					if (myWeights[x][y] >= 0) setWeight(x,y,myWeights[x][y]);
					else setWeight(x,y,0.0);
				}
			}
		}         
	}


	/**
	 * Weight against plan points.
	 * @param p
	 * @param factor
	 */
	public void setProximityWeights(Plan p, Double factor, boolean applyToUndefined) {
		double[][] myWeights = new double[sz_x][sz_y];	
		for (int i = 0; i < sz_x; i++) {
			for (int j = 0; j < sz_y; j++) {
				myWeights[i][j] = Double.MAX_VALUE;
			}
		}
		for (int i = 1; i < p.size(); i++) {
			Pair<Integer,Integer> pr = gridPosition(p.point(i).position());
			int x0 = pr.first;
			int y0 = pr.second;
			for (int x = 0; x < sz_x; x++) {
				for (int y = 0; y < sz_y; y++) {
					double thisweight = ((new Vect2(x0,y0)).Sub(new Vect2(x, y)).norm()+p.size()-1-i)*factor;
					if (applyToUndefined || this.weights.containsKey(Pair.make(x, y))) {
						myWeights[x][y] = Math.min(myWeights[x][y], thisweight);
					}
				}
			}
		}
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				if (applyToUndefined || this.weights.containsKey(Pair.make(x, y))) {
					if (myWeights[x][y] >= 0) setWeight(x,y,myWeights[x][y]);
					else setWeight(x,y,0.0);
				}
			}
		}         
	}

	public void setPolyWeights(double time, List<PolyPath> path) {
		for (PolyPath pp: path) {
			SimplePoly poly = pp.position(time);
			//f.pln(" $$$ poly = "+poly);
			double alt = (poly.top + poly.bottom)/2.0;
			for (int i = 0; i < sz_x; i++) {
				for (int j = 0; j < sz_y; j++) {
					Position pc_ij = center(i,j).mkAlt(alt);
					//f.pln(" $$$ pc_ij = "+pc_ij);
					if (poly.contains(pc_ij)) {
						setWeight(i,j,100.0);
					} else {
						setWeight(i,j,0.0);
					}
				}
			}
		}
	}

	public String toString() {
		return "DensityGrid [startPoint=" + startPoint + ", endPoint="
				+ endPoint + "\n, weights="+ weights + ", corners=" + corners
				+ "\n, latLon=" + latLon + ", squareSize=" + squareSize
				+ "\n, sz_x=" + sz_x + ", sz_y=" + sz_y
				+ ", bounds=" + bounds + "]";
	}


	public void printWeights() {
		for (int y = sz_y-1; y >= 0; y--) {
			for (int x=0; x < sz_x; x++) {
				//f.pln(" $$$ getWeight("+x+","+y+") = "+getWeight(x,y));
				//if (Double.isFinite(getWeight(x,y))) {
				if (getWeight(x,y) < Double.MAX_VALUE - 10.0) {
					f.p(" "+f.FmLead((int)getWeight(x,y),3));
				} else {
					f.p(" ---");;
				}
			}
			f.pln();
		}
	}

	public void printSearchedWeights() {
		for (int y = sz_y-1; y >= 0; y--) {
			for (int x=0; x < sz_x; x++) {
				//f.pln(" $$$ getWeight("+x+","+y+") = "+getWeight(x,y));
				//if (Double.isFinite(getWeight(x,y))) {
				if (getWeight(x,y) < Double.MAX_VALUE - 10.0) {
					f.p(" "+f.FmLead((int)getSearchedWeight(x,y),3));
				} else {
					f.p(" ---");;
				}
			}
			f.pln();
		}
	}

	public void printCorners() {
		f.pln("DensityGrid.printCorners:");		
		for (int y = sz_y-1; y >= 0; y--) {
			for (int x=0; x < sz_x; x++) {
				f.p("\t"+getPosition(x,y).toString(2));
			}
			f.pln();
		}
	}

	//	/**
	//	 * Return a plan representation of the entire grid.  Labels are set to be the searched weight values (if any), with a * preceding it for marked points
	//	 * @return
	//	 */
	//	public Plan gridToPlan() {
	//		Plan p = new Plan("DENSITYGRID");
	//		double time = 0;
	////f.pln(" gridToPlan corners size="+corners.size());		
	//		for (int y = sz_y-1; y >= 0; y--) {
	//			for (int x=0; x < sz_x; x++) {
	////f.pln("gridToPlan x="+x+" y="+y+" szx="+sz_x+" szy="+sz_y+" center="+center(x,y));				
	//				Position pxy2 = center(x, y);
	//				if (!pxy2.isInvalid()) {
	//					NavPoint np2 = new NavPoint(pxy2,time+0.5);
	//					np2 = np2.makeLabel(""+getSearchedWeight(x, y));
	//					p.add(np2);
	//				}
	//				time++;
	//			}
	//		}		
	//		return p;
	//	}

	//	List<Pair<Integer,Integer>> getFoundPath() {
	//		return foundPath;
	//	}

}
