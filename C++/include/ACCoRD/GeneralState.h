/*
 * Copyright (c) 2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef GENERALSTATE_H_
#define GENERALSTATE_H_

#include "Position.h"
#include "Velocity.h"
#include "SimpleMovingPoly.h"
#include "SimplePoly.h"
#include "General3DState.h"
#include "EuclideanProjection.h"
#include <string>


namespace larcfm {

/**
 * A "general state" object that holds Euclidean or Lat/Lon information about position and velocity for an object.
 * Currently this can either be a (point-mass) aircraft or a (possible morphing) polygon.
 * This is intended to be traffic information that will be converted to a General3DState 
 * object to be sent to a GeneralDetector object.  
 */
class GeneralState {
private:
	std::string id;
	Position si;
	Velocity vi;
	SimpleMovingPoly mp;
	double t;
	bool containment;
	bool pointDefined;
	bool polyDefined;
	mutable General3DState state;
	
public:
	GeneralState();
	
	GeneralState(const std::string& name, const Position& s, const Velocity& v, double time);

	GeneralState(const std::string& name, const SimpleMovingPoly& p, double time);

	GeneralState(const std::string& name, const SimpleMovingPoly& p, double time, bool cont);

	GeneralState(const std::string& name, const SimplePoly& p, const Velocity& v, double time, bool cont);

	GeneralState(const MovingPolygon3D& p, bool containment);
	
	GeneralState(const GeneralState& g);

	/**
	 * Creates a Euclidean GeneralState 
	 * @param g
	 */
	static GeneralState make(const General3DState& g, const std::string& name, double time);

	/**
	 * Creates a LatLon GeneralState
	 * @param g
	 * @param proj
	 */
	static GeneralState make(const General3DState& g, const EuclideanProjection& proj, const std::string& name, double time);

	static const GeneralState INVALID;
	
//	double distanceH(const Position& p) const;
//
//	double distanceV(const Position& p) const;

	/**
	 * Return a representative position for this state (averagePoint in the case of polygons)
	 */
	Position representativePosition() const;

	Velocity representativeVelocity() const;

	std::string getName() const;

	GeneralState copy() const;

	bool hasPointMass() const;
	
	bool hasPolygon() const;
	
	Position getPosition() const;
	
	Velocity getVelocity() const;
	
	SimpleMovingPoly getPolygon() const;
	
	double getTime() const;

	bool isContainment() const;
	
	General3DState get3DState(const EuclideanProjection& proj) const;
	
	/**
	 * Return the last computed 3D state (via get3DState)
	 */
	General3DState getLast3DState() const;

	/**
	 * Return most recent 3d projection advanced to new time t (or invalid if get3DState has not been called)
	 */
	General3DState pred(double t) const;

	/**
	 * Return a new general state advanced to new (absolute) time t
	 */
	GeneralState linearPred(double t) const;
	
	/**
	 * Return a new general state advanced by (relative) time dt
	 */
	GeneralState linear(double dt) const;

	bool checkLatLon(bool ll) const;

	bool isLatLon() const;

	bool isInvalid() const;

	std::string toString() const;

//	std::string toOutput(int precision) const;

};


}// namespace

#endif
