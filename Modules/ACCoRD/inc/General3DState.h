/*
 * Copyright (c) 2016-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef GENERAL3DSTATE_H_
#define GENERAL3DSTATE_H_

#include "Vect3.h"
#include "Velocity.h"
#include "MovingPolygon3D.h"
#include <string>


namespace larcfm {

/**
 * A "general state" object that holds Euclidean information about position and velocity for an object.
 * Currently this can either be a (point-mass) aircraft or a (possible morphing) polygon.
 * This is intended to be traffic information for a GeneralDetector object.  
 */
class General3DState {
private:
	Vect3 si;
	Velocity vi;
	MovingPolygon3D mp;
	bool containment;
	bool pointDefined;
	bool polyDefined;
	
public:
	General3DState();
	
	General3DState(const Vect3& s, const Velocity& v);

	General3DState(const MovingPolygon3D& p);

	General3DState(const MovingPolygon3D& p, bool containment);
	
	General3DState(const General3DState& g);

	static const General3DState INVALID;
	static const General3DState ZERO;
	

	bool hasPointMass() const;
	
	bool hasPolygon() const;
	
	Vect3 getVect3() const;
	
	Velocity getVelocity() const;
	
	MovingPolygon3D getPolygon() const;
	
	bool isContainment() const;
	
	Vect3 representativePosition() const;
	
	Velocity representativeVelocity() const;
	
	/**
	 * Advance this state by the indicated amount of (relative) time
	 */
	General3DState linear(double dt) const;
	
	std::string toString() const;

};
}// namespace

#endif
