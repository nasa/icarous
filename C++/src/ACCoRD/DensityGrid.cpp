#include "DensityGrid.h"
#include "Util.h"
#include "GreatCircle.h"
#include "Position.h"
#include "format.h"
#include "PolyPath.h"
//#include "DensityGridSearch.h"
//#include "DensityGridAStarSearch.h"
#include <limits>
#include <float.h>

namespace larcfm {
	DensityGrid::DensityGrid() {
		startPoint_ = Position::INVALID();
		startTime_ = -1;
		endPoint_= Position::INVALID();
		latLon = true;
		squareSize = 0.0;
		squareDist = 0.0;
		sz_x = 0;
		sz_y = 0;
	}

	// grid coordinates refer to the bottom left (SW) corner of the square
	// buffer is number of extra squares in each direction from the base rectangle
	DensityGrid::DensityGrid(const BoundingRectangle& b, const NavPoint& start, const Position& end, int buffer, double sqSz, bool ll) {
		init(b, start, end, buffer, sqSz, ll);
	}

	// grid coordinates refer to the bottom left (SW) corner of the square
	// buffer is number of extra squares in each direction from the base rectangle
	DensityGrid::DensityGrid(const Plan& p, int buffer, double squareSize) {
		init(p.getBound(), p.point(0), p.point(p.size()-1).position(), buffer, squareSize, p.isLatLon());
	}

	DensityGrid::DensityGrid(const Plan& p, double startT, int buffer, double squareSize) {
		init(p.getBound(), NavPoint(p.position(startT),startT), p.point(p.size()-1).position(), buffer, squareSize, p.isLatLon());
	}


	void DensityGrid::init(const BoundingRectangle& b, const NavPoint& start, const Position& end, int buffer, double sqSz, bool ll) {
		squareDist = sqSz;
		latLon = ll;
		squareSize = sqSz;
		if (latLon) { // convert this to lat/lon
			squareSize = linearEstY(0.0, sqSz);
		}
		startPoint_ = start.position();
		startTime_ = start.time();
		endPoint_ = end;
		// add an additional 0.5 box buffer on each side, so it's less likely to have start/end at edge of squares
		// also include an additional box to the top and right so that we have boundary positions in the table for those corners
		double maxX = b.getMaxX();
		double minX = b.getMinX();
		double maxY = b.getMaxY();
		double minY = b.getMinY();
		sz_x = (int)std::ceil((maxX-minX)/squareSize)+buffer*2+1;
		sz_y = (int)std::ceil((maxY-minY)/squareSize)+buffer*2+1;
		double dx0 = minX - (buffer)*squareSize;
		double dy0 = minY - (buffer)*squareSize;
		if (latLon && Util::sign(minX) != Util::sign(maxX) && maxX-minX > Pi) {
			sz_x = (int)std::ceil((Pi-(maxX-minX))/squareSize)+buffer*2+1;
			minX = maxX;
			dx0 = minX - (buffer)*squareSize;
		}
		for (int x = 0; x <= sz_x; x++) {
			double dx = dx0 + x*squareSize;
			for (int y = 0; y <= sz_y; y++) {
				double dy = dy0 + y*squareSize;
				if (latLon) {
					LatLonAlt norm = LatLonAlt::normalize(dy, dx);
					dx = norm.lon();
					dy = norm.lat();
					bounds.add(norm);
				} else {
					bounds.add(dx,dy);
				}
				corners[std::pair<double,double>(x,y)] = std::pair<double,double>(dx,dy);
			}
		}
	}




	double DensityGrid::linearEstY(double lati, double dn) {
		return lati + dn/GreatCircle::spherical_earth_radius;
	}

	// distance in meters from a delta latitude
	double DensityGrid::distEstLatLon(double lat1, double lat2) {
		return (lat2-lat1)*GreatCircle::spherical_earth_radius;
	}

	/**
	 * Approximate size of square, in either meters (if Euclidean) or radians (if latlon)
	 * @return
	 */
	double DensityGrid::getNativeSquareDist() const {
		return squareSize;
	}

	/**
	 * Approximate size of square, in meters
	 */
	double DensityGrid::getSquareDist() const {
		return squareDist;
	}

	/**
	 * Adjust all grid corner coordinates so that the start point it in the center of its grid.
	 */
	void DensityGrid::snapToStart() {
		Position cent = center(gridPosition(startPoint_));
		double offx,offy;
		if (latLon) {
			offx = startPoint_.lon()-cent.lon();
			offy = startPoint_.lat()-cent.lat();
		} else {
			offx = startPoint_.x()-cent.x();
			offy = startPoint_.y()-cent.y();
		}
		for (cornerspos = corners.begin(); cornerspos != corners.end(); ++cornerspos) {
			std::pair<double,double> val = cornerspos->second;
			corners[cornerspos->first] = std::pair<double,double>(val.first+offx, val.second+offy);
		}
	}



	std::pair<int,int> DensityGrid::gridPosition(Position p) const {
		if (bounds.contains(p)) {
			double x = p.x();
			double y = p.y();

			if (latLon) {
				Position np = bounds.denormalize(p);
				x = np.lon();
				y = np.lat();
			}
			int gridx = (int)std::floor((x - bounds.getMinX())/squareSize);
			int gridy = (int)std::floor((y - bounds.getMinY())/squareSize);
			return std::pair<int,int>(gridx,gridy);
		}
		return std::pair<int,int>(-1, -1);
	}



	bool DensityGrid::containsCell(const std::pair<int,int>& xy) const {
		return corners.find(xy) != corners.end();
	}


	int DensityGrid::sizeX() const {
		return sz_x;
	}

	/**
	 * Note: the grid size should be 1 larger than expected (to allow for the first point to be in the middle of the square)
	 * @return
	 */
	int DensityGrid::sizeY() const {
		return sz_y;
	}


	Position DensityGrid::getPosition(int x, int y) const {
		std::pair<int,int> xy = std::pair<int,int>(x,y);
		if (corners.find(xy) == corners.end()) return Position::INVALID();
		std::pair<double,double> b = corners.find(xy)->second;
		if (latLon) {
			return Position::mkLatLonAlt(b.second, b.first, 0.0);
		} else {
			return Position(Vect3::mkXYZ(b.first, b.second, 0.0));
		}
	}

	Position DensityGrid::getPosition(const std::pair<int,int>& pii) const {
		return getPosition(pii.first,pii.second);
	}

	Position DensityGrid::center(int x, int y) const {
		if (corners.find(std::pair<int,int>(x,y)) == corners.end() || corners.find(std::pair<int,int>(x+1,y+1)) == corners.end()) {
			return Position::INVALID();
		}
		Position p1 = getPosition(x,y);
		Position p2 = getPosition(x+1,y+1);
		return p1.interpolate(p2, 0.5);
	}

	Position DensityGrid::center(const std::pair<int,int>& pii) const {
		return center(pii.first,pii.second);
	}

	double DensityGrid::getWeight(int x, int y) const {
		std::pair<int,int> xy = std::pair<int,int>(x,y);
		if (corners.find(xy) == corners.end()) return std::numeric_limits<double>::infinity();
		return weights.find(xy)->second; // the bracket accessor is not const!
	}


	double DensityGrid::getWeight(const std::pair<int,int>& pii) const {
		return getWeight(pii.first,pii.second);
	}

	void DensityGrid::setWeight(int x, int y, double d) {
		std::pair<int,int> xy = std::pair<int,int>(x,y);
		if (corners.find(xy) != corners.end() && d >= 0) {
			weights[xy] = d;
		}
	}

	void DensityGrid::clearWeight(int x, int y) {
		std::pair<int,int> xy = std::pair<int,int>(x,y);
		if (weights.find(xy) != weights.end()) {
			weights.erase(xy);
		}
	}

//	/**
//	 * Return weight determined by search, or -Infinity if not set by search
//	 * @param x
//	 * @param y
//	 * @return
//	 */
//	double DensityGrid::getSearchedWeight(int x, int y) {
//		if (!searchedWeights.containsKey(Pair.make(x,y))) return Double.NEGATIVE_INFINITY;
//		return searchedWeights.get(Pair.make(x,y));
//	}
//
//	void DensityGrid::setSearchedWeight(int x, int y, double d) {
//		if (corners.containsKey(Pair.make(x,y)) && d >= 0) {
//			searchedWeights.put(Pair.make(x, y), d);
//			if (Double.isFinite(d)) {
//				minSearchedWeightValue = Math.min(d, minSearchedWeightValue);
//				maxSearchedWeightValue = Math.max(d, maxSearchedWeightValue);
//			}
//		}
//	}
//
//	void DensityGrid::clearSearchedWeights() {
//		searchedWeights.clear();
//		minSearchedWeightValue = Double.MAX_VALUE;
//		maxSearchedWeightValue = -Double.MAX_VALUE;
//	}
//
//	double DensityGrid::getMinSearchedWeightValue() {
//		return minSearchedWeightValue;
//	}
//
//	double DensityGrid::getMaxSearchedWeightValue() {
//		return maxSearchedWeightValue;
//	}
//
//	bool DensityGrid::getMark(std::pair<int,int> pii) {
//		return marked.contains(pii);
//	}
//
//	bool DensityGrid::getMark(int x, int y) {
//		return getMark(Pair.make(x, y));
//	}
//
//	void DensityGrid::setMark(std::pair<int,int> pii, bool b) {
//		if (b) {
//			marked.add(pii);
//		} else {
//			marked.remove(pii);
//		}
//	}
//
//	void DensityGrid::setMark(int x, int y, bool b) {
//		setMark(Pair.make(x,y), b);
//	}
//
//	void DensityGrid::clearMarks() {
//		marked.clear();
//	}

	/**
	 * Set all weights to d in grid cells minX to maxX and minY to maxY, inclusive
	 */
	void DensityGrid::setWeights(int minX, int minY, int maxX, int maxY, double d) {
		for (int x = minX; x <= maxX; x++) {
			for (int y = minY; y <= maxY; y++) {
				setWeight(x,y,d);
			}
		}
	}

	/**
	 * Set all weights to d
	 */
	void DensityGrid::setWeights(double d) {
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
	void DensityGrid::clearWeightsOutside(SimplePoly poly) {
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
	void DensityGrid::setWeightsInside(SimplePoly poly, double d) {
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


	void DensityGrid::clearWeights() {
		weights.clear();
	}

	Position DensityGrid::startPoint() const {
		return startPoint_;
	}

	Position DensityGrid::endPoint() const {
		return endPoint_;
	}

	double DensityGrid::startTime() const {
		return startTime_;
	}

	bool DensityGrid::validPair(const std::pair<int,int>&  p) {
		if (p.first == -1) return false;
		if (p.second == -1) return false;
		return true;
	}

	std::vector<std::pair<int,int> > DensityGrid::gridPath(const Plan& p) const {
		std::vector<std::pair<int,int> > rtn;
		if (p.size() < 1) return rtn;
		NavPoint p0 = p.point(0);
//		std::pair<int,int> gpPair0 = gridPosition(p0.position());
		if (p.size() < 2) return rtn;
		NavPoint pN = p.getLastPoint();
		double deltaTime = pN.time() - p0.time();
		int numSteps = 2*(sizeX() + sizeY());
		double stepSizeTm = deltaTime/numSteps;
		std::pair<int,int> lastPair = std::pair<int,int>(-1,-1);
		for (double t = p0.time(); t < pN.time() + 1.0; t = t + stepSizeTm) {
			Position pt = p.position(t);
			std::pair<int,int> gpPairt = gridPosition(pt);
			if (validPair(gpPairt) && gpPairt != lastPair) {
				rtn.push_back(gpPairt);
				lastPair = gpPairt;
			}
		}
		return rtn;
	}

	void DensityGrid::printGridPath(const std::vector<std::pair<int,int> >& gPath) {
		for (int i = 0; i < gPath.size(); i++) {
			std::pair<int,int> ijPair = gPath[i];
			fpln("printGridPath i = "+Fm0(i)+" ijPair = "+Fm0(ijPair.first)+","+Fm0(ijPair.second));
		}
	}

//	std::vector<std::pair<int,int> > DensityGrid::optimalPath() {
//		DensityGridAStarSearch dgs = DensityGridAStarSearch();
//		return dgs.search(*this, startPoint_, endPoint_);
//	}
//
//	std::vector<std::pair<int,int> > optimalPath(const DensityGridSearch& dgs) {
//		return dgs.search(*this, startPoint_, endPoint_);
//	}


	DensityGrid::Direction DensityGrid::direction(const std::pair<int,int>& p1, const std::pair<int,int>& p2) {
		if (p1.first == p2.first) {
			if (p1.second == p2.second) {
				return undef;
			} else if (p1.second < p2.second) {
				return N;
			} else {
				return S;
			}
		} else if (p1.first < p2.first) {
			if (p1.second == p2.second) {
				return W;
			} else if (p1.second < p2.second) {
				return NW;
			} else {
				return SW;
			}
		} else {
			if (p1.second == p2.second) {
				return E;
			} else if (p1.second < p2.second) {
				return NE;
			} else {
				return SE;
			}
		}
	}


	std::vector<std::pair<int,int> > DensityGrid::thin(const std::vector<std::pair<int,int> >& gPath) {
		std::vector<std::pair<int,int> > rtn = std::vector<std::pair<int,int> >(10);
		std::pair<int,int> lastPair = gPath[0];
		Direction lastDirection = direction(std::pair<int,int>(0, 0),lastPair);
		rtn.push_back(lastPair);
		for (int i = 1; i < gPath.size(); i++) {
			std::pair<int,int> pp = gPath[i];
			Direction dir = direction(lastPair,pp);
			if (dir == lastDirection) {
				rtn.erase(rtn.end()-1);
			}
			rtn.push_back(pp);
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
	std::vector<std::pair<int,int> > DensityGrid::reduceGridPath(const std::vector<std::pair<int,int> >& gp) {
		return thin(gp);
	}


	Plan DensityGrid::gridPathToPlan(const std::vector<std::pair<int,int> >& gPath, double gs, double vs, bool reduce) {
		std::vector<std::pair<int,int> > gPath2;
		if (reduce) {
			gPath2 = reduceGridPath(gPath);
		} else {
			gPath2 = thin(gPath);
		}
		Plan p =  Plan();
		NavPoint np0 = NavPoint(startPoint_,startTime_);
		p.add(np0);
		Position lastCenter = startPoint_;
		double lastTime = startTime_;
		double lastAlt = np0.alt();
		for (int i = 1; i < gPath2.size()-1; i++) {   // don't add first or last pair
			Position cntr = center(gPath2[i]);
			double dist = cntr.distanceH(lastCenter);
			double dt = dist/gs;
			double time = lastTime + dt;
			double newAlt = lastAlt + vs*dt;
			cntr = cntr.mkAlt(newAlt);
			p.add(NavPoint(cntr, time));
			lastTime = time;
			lastCenter = cntr;
			lastAlt = newAlt;
		}
		// we need to check to see if adding back the last point makes a weird kink in the end plan.
		// if so, we need to get rid of the last grid-point.
		double trk1 = p.finalVelocity(p.size()-2).trk();
		double trk2 = p.point(p.size()-1).position().track(endPoint_);
		if (Util::turnDelta(trk1, trk2) > Pi/4 && gPath.size() > 3) {
			Position pivot = center(gPath[gPath.size()-3]); // check the original two-from-last point (before thinning)
																// (we're already dropping the last point)
			NavPoint nearest = p.closestPoint(pivot); // this will be the breakpoint, if it's not already in the plan
			p.remove(p.size()-1);
			if (p.point(p.size()-1).distanceH(nearest) > squareDist) {
				p.add(nearest);
			}
			lastCenter = p.point(p.size()-1).position();
		}

		double dist = lastCenter.distanceH(endPoint_);
		double dt = dist/gs;
		NavPoint last = NavPoint(endPoint_,lastTime+dt);
		p.add(last);
		return p;
	}



	bool DensityGrid::contains(const std::vector<std::pair<int,int> >& gPath, const std::pair<int,int>& pii) const {
		for (int i = 0; i < gPath.size()-1; i++) {   // don't do last pair
			if (gPath[i] == pii) {
				return true;
			}
		}
		return false;
	}




	void DensityGrid::setProximityWeights(const std::vector<std::pair<int,int> >& gPath, double factor, bool applyToUndefined) {
		double myWeights[sz_x][sz_y];
		for (int i = 0; i < sz_x; i++) {
			for (int j = 0; j < sz_y; j++) {
				std::pair<int,int> pij = std::pair<int,int>(i,j);
				if (contains(gPath, pij))  {
					myWeights[i][j] = -1.0;
				} else {
					myWeights[i][j] = DBL_MAX;
				}
			}
		}
		for (int i = 0; i < gPath.size(); i++) {
			std::pair<int,int> xy = gPath[i];
			int x1 = xy.first;
			int y1 = xy.second;
			for (int x = 0; x < sz_x; x++) {
				for (int y = 0; y < sz_y; y++) {
					double dist = std::sqrt((x-x1)*(x-x1)+(y-y1)*(y-y1));
					myWeights[x][y] = std::min(myWeights[x][y], dist*factor);
				}
			}
		}
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				if (applyToUndefined || (weights.find(std::pair<int,int>(x,y)) != weights.end())) {
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
	void DensityGrid::setProximityWeights(const Plan& p, double factor, bool applyToUndefined) {
		double myWeights[sz_x][sz_y];
		for (int i = 0; i < sz_x; i++) {
			for (int j = 0; j < sz_y; j++) {
				myWeights[i][j] = DBL_MAX;
			}
		}
		for (int i = 1; i < p.size(); i++) {
			std::pair<int,int> pr = gridPosition(p.point(i).position());
			int x0 = pr.first;
			int y0 = pr.second;
			for (int x = 0; x < sz_x; x++) {
				for (int y = 0; y < sz_y; y++) {
					double thisweight = ((Vect2(x0,y0)).Sub(Vect2(x, y)).norm()+p.size()-1-i)*factor;
					if (applyToUndefined || weights.find(std::pair<int,int>(x,y)) != weights.end()) {
						myWeights[x][y] = std::min(myWeights[x][y], thisweight);
					}
				}
			}
		}
		for (int x = 0; x < sz_x; x++) {
			for (int y = 0; y < sz_y; y++) {
				if (applyToUndefined || weights.find(std::pair<int,int>(x,y)) != weights.end()) {
					if (myWeights[x][y] >= 0) setWeight(x,y,myWeights[x][y]);
					else setWeight(x,y,0.0);
				}
			}
		}
	}

	void DensityGrid::setPolyWeights(double time, const std::vector<PolyPath>& path) {
		for (int i = 0; i < path.size(); i++) {
			PolyPath pp = path[i];
			SimplePoly poly = pp.position(time);
			//f.pln(" $$$ poly = "+poly);
			double alt = (poly.getTop() + poly.getBottom())/2.0;
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

	std::string DensityGrid::toString() const {
		return "DensityGrid [startPoint=" + startPoint_.toString() + ", endPoint_="
				+ endPoint_.toString()
//				+ "\n, weights="+ weights + ", corners=" + corners
//				+ "\n, latLon=" + latLon + ", squareSize=" + squareSize
				+ "\n, sz_x=" + Fm0(sz_x) + ", sz_y=" + Fm0(sz_y)
//				+ ", bounds=" + bounds
				+ "]";
	}


	void DensityGrid::printWeights() const {
		for (int y = sz_y-1; y >= 0; y--) {
			for (int x=0; x < sz_x; x++) {
				//f.pln(" $$$ getWeight("+x+","+y+") = "+getWeight(x,y));
				//if (Double.isFinite(getWeight(x,y))) {
				if (getWeight(x,y) < DBL_MAX - 10.0) {
					fp(" "+FmLead((int)getWeight(x,y),3));
				} else {
					fp(" ---");;
				}
			}
			fpln("");
		}
	}

//	void DensityGrid::printSearchedWeights() const {
//		for (int y = sz_y-1; y >= 0; y--) {
//			for (int x=0; x < sz_x; x++) {
//				//f.pln(" $$$ getWeight("+x+","+y+") = "+getWeight(x,y));
//				//if (Double.isFinite(getWeight(x,y))) {
//				if (getWeight(x,y) < Double.MAX_VALUE - 10.0) {
//					fp(" "+FmLead((int)getSearchedWeight(x,y),3));
//				} else {
//					fp(" ---");;
//				}
//			}
//			fpln("");
//		}
//	}

	void DensityGrid::printCorners() const {
		fpln("DensityGrid.printCorners:");
		for (int y = sz_y-1; y >= 0; y--) {
			for (int x=0; x < sz_x; x++) {
				fp("\t"+getPosition(x,y).toString(2));
			}
			fpln("");
		}
	}

	//	}

}
