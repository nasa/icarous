/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <vector>
#include"MovingPolygon3D.h"
#include "Vect2.h"
#include "format.h"

namespace larcfm {


MovingPolygon3D::MovingPolygon3D() {
	horizpoly = MovingPolygon2D();
   	vspeed = 0;
    minalt = 10000;
    maxalt = 10000;
}

MovingPolygon3D::MovingPolygon3D(const MovingPolygon2D& horizpoly_m, double vspeed_d, double minalt_d, double maxalt_d) {
	//super();
	horizpoly = horizpoly_m;
	vspeed = vspeed_d;
	minalt = minalt_d;
	maxalt = maxalt_d;
}

MovingPolygon3D::MovingPolygon3D(const Poly3D& p, const Velocity& v, double end) {
	vspeed = v.vs();
	minalt = p.getBottom();
	maxalt = p.getTop();
	std::vector<Vect2> ps = std::vector<Vect2>();
	std::vector<Vect2> vs = std::vector<Vect2>();
	for (int i = 0; i < p.size(); i++) {
		ps.push_back(p.getVertex(i));
		vs.push_back(v.vect2());
//fpln("MovingPolygon3D "+Fm0(i)+" "+ps[i].toString()+" "+vs[i].toString());
	}
	horizpoly = MovingPolygon2D(ps,vs,end);
}

MovingPolygon3D::MovingPolygon3D(const Poly3D& p, const std::vector<Velocity>& vlist, double end) {
	vspeed = vlist[0].vs();
	minalt = p.getBottom();
	maxalt = p.getTop();
	std::vector<Vect2> ps = std::vector<Vect2>();
	std::vector<Vect2> vs = std::vector<Vect2>();
	for (int i = 0; i < p.size(); i++) {
		ps.push_back(p.getVertex(i));
		vs.push_back(vlist[i].vect2());
//fpln("MovingPolygon3D "+Fm0(i)+" "+ps[i].toString()+" "+vs[i].toString());
	}
	horizpoly = MovingPolygon2D(ps,vs,end);
}


Poly3D MovingPolygon3D::position(double t) const {
	Poly2D p2d = horizpoly.position(t);
	return Poly3D(p2d, minalt+vspeed*t, maxalt+vspeed*t);
}

Velocity MovingPolygon3D::velocity(int i) const {
   	return Velocity::make(horizpoly.polyvel[i]).mkVs(vspeed);
}

Velocity MovingPolygon3D::averageVelocity() const {
	Vect2 v = Vect2::ZERO();
	for (int i = 0; i < size(); i++) {
		v = v.Add(horizpoly.polyvel[i]);
	}
	return Velocity::make(Vect3(v.Scal(1.0/size()),vspeed));
}

MovingPolygon3D MovingPolygon3D::linear(double t) const {
	std::vector<Velocity> vlist;
	for (int i = 0; i < size(); i++) {
		vlist.push_back(velocity(i));
	}
	return MovingPolygon3D(position(t), vlist, horizpoly.tend+t);
}

int MovingPolygon3D::size() const {
	return horizpoly.size();
}

bool MovingPolygon3D::isStable() const {
	return horizpoly.isStable();
}

std::string MovingPolygon3D::toString() const {
	return horizpoly.toString()+" vspd="+Units::str("fpm",vspeed)+" minalt="+Units::str("ft",minalt)+" maxalt="+Units::str("ft",maxalt);
}

MovingPolygon3D MovingPolygon3D::reverseOrder() const {
	return MovingPolygon3D(horizpoly.reverseOrder(), vspeed, minalt, maxalt);
}

}
