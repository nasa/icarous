/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect3.h"
#include "Velocity.h"
#include <string>
#include "MovingPolygon3D.h"
#include "General3DState.h"

namespace larcfm {


General3DState::General3DState() {
	pointDefined = false;
	polyDefined = false;
	containment = false;
}

const General3DState General3DState::INVALID = General3DState();

const General3DState General3DState::ZERO = General3DState(Vect3::ZERO(),Velocity::ZEROV());


General3DState::General3DState(const Vect3& s, const Velocity& v) {
	si = s;
	vi = v;
	pointDefined = true;
	polyDefined = false;
	containment = false;
}

General3DState::General3DState(const MovingPolygon3D& p) {
	pointDefined = false;
	polyDefined = true;
	mp = p;
	containment = false;
}

General3DState::General3DState(const MovingPolygon3D& p, bool c) {
	pointDefined = false;
	polyDefined = true;
	mp = p;
	containment = c;
}

General3DState::General3DState(const General3DState& g) {
	si = g.si;
	vi = g.vi;
	mp = g.mp;
	containment = g.containment;
	pointDefined = g.pointDefined;
	polyDefined = g.polyDefined;
}

bool General3DState::hasPointMass() const {
	return pointDefined;
}

bool General3DState::hasPolygon() const {
	return polyDefined;
}

Vect3 General3DState::getVect3() const {
	return si;
}

Velocity General3DState::getVelocity() const {
	return vi;
}

MovingPolygon3D General3DState::getPolygon() const {
	return mp;
}

bool General3DState::isContainment() const {
	return containment;
}

Vect3 General3DState::representativePosition() const {
	if (hasPointMass()) {
		return si;
	} else if (hasPolygon()) {
		return mp.position(0).averagePoint();
	} else {
		return Vect3::INVALID();
	}
}

Velocity General3DState::representativeVelocity() const {
	if (hasPointMass()) {
		return vi;
	} else if (hasPolygon()) {
		return mp.averageVelocity();
	} else {
		return Velocity::INVALIDV();
	}
}


/**
 * Advance this state by the indicated amount of (relative) time
 */
General3DState General3DState::linear(double dt) const {
	if (hasPointMass()) {
		return General3DState(si.AddScal(dt, vi), vi);
	} else if (hasPolygon()) {
		return General3DState(mp.linear(dt), containment);
	} else {
		return INVALID;
	}
}

std::string General3DState::toString() const {
	if (polyDefined)
		return "("+mp.toString()+", "+(containment?"contain":"avoid")+")";
	else
		return "("+si.toString()+", "+vi.toString()+")";
}

}
