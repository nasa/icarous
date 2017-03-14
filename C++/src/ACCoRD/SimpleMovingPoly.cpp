/*
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include <vector>
#include <string>
#include "Position.h"
#include "EuclideanProjection.h"
#include "BoundingRectangle.h"
#include "MovingPolygon3D.h"
#include "Velocity.h"
#include "Poly3D.h"
#include "SimpleMovingPoly.h"
#include "Velocity.h"
#include "format.h"

namespace larcfm {

using std::vector;
using std::string;
using std::endl;


SimpleMovingPoly::SimpleMovingPoly(const SimplePoly& start, const vector<Velocity>& vs) {
	poly = start;
	vlist = vs;
	morphingPoly = true;
	while ((int) vlist.size() < (int) start.size()) {
		vlist.push_back(Velocity::ZEROV());
	}
}

SimpleMovingPoly::SimpleMovingPoly(const SimplePoly& start, const Velocity& v) {
	poly = start;
	vlist =  vector<Velocity>();
	morphingPoly = true;
	for (int i = 0; i < start.size(); i++) {
		vlist.push_back(v);
	}
	morphingPoly = false;
}

SimpleMovingPoly::SimpleMovingPoly() {
	poly =  SimplePoly();
	vlist =  vector<Velocity>();
	morphingPoly = true;
}

/**
 * Constructor for a SimplePoly with predefined top and bottom altitudes.
 *
 * @param b Bottom altitude
 * @param t Top Altitude
 */
SimpleMovingPoly::SimpleMovingPoly(double b, double t) {
	poly =  SimplePoly(b,t);
	vlist =  vector<Velocity>();
	morphingPoly = true;
}

/**
 * Constructor for a SimplePoly with predefined top and bottom altitudes.
 *
 * @param b Bottom altitude
 * @param t Top Altitude
 */
SimpleMovingPoly::SimpleMovingPoly(double b, double t, const string& units) {
	poly =  SimplePoly(b,t,units);
	vlist =  vector<Velocity>();
	morphingPoly = true;
}


/**
 * Create a deep copy of a SimplePoly
 *
 * @param p Source poly.
 */
SimpleMovingPoly::SimpleMovingPoly(const SimpleMovingPoly& p) {
	poly =  SimplePoly(p.poly);
	vlist =  vector<Velocity>();
	morphingPoly = true;
	for(int i = 0; i < p.size(); i++) {
		vlist.push_back(p.vlist[i]);
	}
}

SimpleMovingPoly SimpleMovingPoly::copy() const {
	SimpleMovingPoly sp;
	sp.poly = poly;
	sp.vlist = vlist;
	sp.morphingPoly = morphingPoly;
	return sp;
}

bool SimpleMovingPoly::addVertex(const Position& p, const Velocity& v) {
	bool ret = poly.addVertex(p);
	if (ret) {
		vlist.push_back(v);
	}
	return ret;
}

void SimpleMovingPoly::removeVertex(int i) {
	if (i >= 0 && i < poly.size()) {
		poly.remove(i);
		vlist.erase(vlist.begin()+i);
	}
}

void SimpleMovingPoly::setTop(double top) {
	poly.setTop(top);
}

void SimpleMovingPoly::setBottom(double bot) {
	poly.setBottom(bot);
}

/**
 * Create a SimplePoly from a Poly3D.  This SimplePoly will use latlon coordinates.
 */
SimpleMovingPoly SimpleMovingPoly::make(const MovingPolygon3D& p3, const EuclideanProjection& proj) {
	Poly3D base = p3.position(0);
	if (!p3.isStable()) {
		vector<Velocity> vs =  vector<Velocity>();
		for(int i = 0; i < p3.size(); i++) {
			vs.push_back(proj.inverseVelocity( Vect3(base.getVertex(i),0), Velocity::make( Vect3(p3.horizpoly.polyvel[i],p3.vspeed)), true));
		}
		return  SimpleMovingPoly(SimplePoly::make(base,proj), vs);
	} else {
		return  SimpleMovingPoly(SimplePoly::make(base,proj), proj.inverseVelocity(base.averagePoint(), Velocity::make( Vect3(p3.horizpoly.polyvel[0],p3.vspeed)), true));
	}
}

/**
 * Create a SimplePoly from a Poly3D.  This SimplePoly will use Euclidean coordinates.
 */
SimpleMovingPoly SimpleMovingPoly::make(const MovingPolygon3D& p3) {
	SimpleMovingPoly sp;
	sp.poly = SimplePoly::make(p3.position(0));
	for(int i = 0; i < p3.size(); i++) {
		Velocity v = Velocity::make( Vect3(p3.horizpoly.polyvel[i],p3.vspeed));
		sp.vlist.push_back(v);
	}
	return sp;
}

bool SimpleMovingPoly::isLatLon() const {
	return poly.isLatLon();
}

/**
 * Return the polygon projected to be at time dt (dt = 0 returns a copy of the base polygon)
 */
SimplePoly SimpleMovingPoly::position(double dt) const {
	if (dt == 0.0) {
		return poly.copy();
	}
	SimplePoly Poly =  SimplePoly();
	int sz = poly.size();
	if (morphingPoly) {
		for (int j = 0; j < sz; j++) {
			Position n1 = poly.getVertex(j);
			Position n1t = poly.getTopPoint(j);
			Position p = n1.linear(vlist[j],dt);
			Position pt = n1t.linear(vlist[j],dt);
			Poly.addVertex(p);
			Poly.setBottom(p.z());
			Poly.setTop(pt.z());
		}
	} else {
		Velocity v = vlist[0];
		for (int j = 0; j < sz; j++) {
			Position n1 = poly.getVertex(j);
			Position n1t = poly.getTopPoint(j);
			Position p = n1.linear(v,dt);
			Position pt = n1t.linear(v,dt);
			Poly.addVertex(p);
			Poly.setBottom(p.z());
			Poly.setTop(pt.z());
		}
	}
	return Poly;
}

/**
 * Return the average Velocity (at time 0).
 */
Velocity SimpleMovingPoly::averageVelocity() const {
	if (morphingPoly) {
		Velocity v = Velocity::ZEROV();
		for (int i = 0; i < (int) vlist.size(); i++) {
			v = v.Add(vlist[i]);
		}
		return Velocity::make(v.Scal(1.0/vlist.size()));
	} else {
		return vlist[0];
	}
}

SimpleMovingPoly SimpleMovingPoly::linear(double dt) const {
	SimplePoly Poly =  SimplePoly();
	int sz = poly.size();
	if (morphingPoly) {
		for (int j = 0; j < sz; j++) {
			Position n1 = poly.getVertex(j);
			Position n1t = poly.getTopPoint(j);
			Position p = n1.linear(vlist[j],dt);
			Position pt = n1t.linear(vlist[j],dt);
			Poly.addVertex(p);
			Poly.setBottom(p.z());
			Poly.setTop(pt.z());
		}
	} else {
		Velocity v = vlist[0];
		for (int j = 0; j < sz; j++) {
			Position n1 = poly.getVertex(j);
			Position n1t = poly.getTopPoint(j);
			Position p = n1.linear(v,dt);
			Position pt = n1t.linear(v,dt);
			Poly.addVertex(p);
			Poly.setBottom(p.z());
			Poly.setTop(pt.z());
		}
	}
	return  SimpleMovingPoly(Poly,vlist);
}


/**
 * This will return a moving polygon that starts at point i and ends at point i+1
 * @param i
 * @param proj
 * @return
 */
MovingPolygon3D SimpleMovingPoly::getMovingPolygon(double time, const EuclideanProjection& proj) const {
	Poly3D p3d = position(time).poly3D(proj);
	if (morphingPoly) {
		vector<Velocity> vs =  vector<Velocity>();
		for (int i = 0; i < p3d.size(); i++) {
			if (isLatLon()) {
				vs.push_back(proj.projectVelocity(poly.getVertex(i), vlist[i]));
			} else {
				vs.push_back(vlist[i]);
			}
		}
		return  MovingPolygon3D(p3d,vs,1000000.0);
	} else {
		if (isLatLon()) {
			return  MovingPolygon3D(p3d,proj.projectVelocity(poly.averagePoint(), vlist[0]),1000000.0);
		} else {
			return  MovingPolygon3D(p3d,vlist[0],1000000.0);
		}
	}
}

MovingPolygon3D SimpleMovingPoly::getMovingPolygon(const EuclideanProjection& proj) const {
	return getMovingPolygon(0.0,proj);
}


int SimpleMovingPoly::size() const {
	return poly.size();
}

/**
 * Return true if point p is within the polygon at time dt from now.
 */
bool SimpleMovingPoly::contains(const Position& p, double dt) const {
	return dt >= 0 && position(dt).contains(p);
}

///**
// * Create a vector of strings describing one vertex: lat, lon, alt, alt2, trk, gs, vs (or sx, sy, sz)
// * @param vertex vertex number
// * @param trkgsvs true use track/Gs/Vs, false use Euclidean velocity vector (sx, sy, sz)
// * @param precision number of fractional digits in fields
// * @return
// */
//vector<string> SimpleMovingPoly::toStringvector(int vertex, bool trkgsvs, int precision) const {
//	vector<string> ret;
//	Velocity v = vlist[vertex];
//	vector<string> ps = poly.toStringvector(vertex,  precision);
//	ret.insert(ret.end(),ps.begin(),ps.end());
//	if (trkgsvs) {
//		ret.addAll(v.toStringvector(precision));
//	} else {
//		ret.addAll(v.toStringXYZvector(precision));
//	}
//	return ret;
//}

std::string SimpleMovingPoly::toString() const {
	return poly.toString();
}

std::vector<std::string> SimpleMovingPoly::toStringList(int vertex, bool trkgsvs, int precision) const {
	std::vector<std::string> ret;
    Velocity v = vlist[vertex];
    std::vector<std::string> polystr = poly.toStringList(vertex,  precision);
    ret.insert(ret.end(), polystr.begin(), polystr.end());
    if (trkgsvs) {
    	std::vector<std::string> vstr = v.toStringList(precision);
    	ret.insert(ret.end(), vstr.begin(), vstr.end());
    } else {
    	std::vector<std::string> vstr = v.toStringXYZList(precision);
    	ret.insert(ret.end(), vstr.begin(), vstr.end());
    }
    return ret;
}


} // namespace




