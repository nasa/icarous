/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef MOVINGPOLYGON2D_H_
#define MOVINGPOLYGON2D_H_

#include <vector>

#include "Poly2D.h"

namespace larcfm {
/**
 * Polygon detection.
 * Based on UNVERIFIED PVS code.
 */
class MovingPolygon2D {


public:
	std::vector<Vect2> polystart;
	std::vector<Vect2> polyvel;
	double tend;
	mutable bool knownStable;
	mutable bool knownUnstable;


	MovingPolygon2D();

	MovingPolygon2D(std::vector<Vect2> polystart, std::vector<Vect2> polyvel, double tend);

	MovingPolygon2D(const Poly2D& polygon, const Vect2& polyvel, double tend);

	MovingPolygon2D(const Poly2D& polygon, const std::vector<Vect2>& vels, double tend_d);

	Poly2D position(double t) const;

  /** return true if this polygon is "stable", i.e., all vector velocities are the same */
	bool isStable() const;

	int size() const ;

	/**
	 * Reverse order of vertices
	 * @return
	 */
	MovingPolygon2D reverseOrder() const;


	//	  std::string toStringOLD() const ;

	std::string toString() const ;

	std::vector<Vect2> getPolyvel() const ;

	std::vector<Vect2> getPolystart() const ;

	double getTend() const;

	void setPolystart(const std::vector<Vect2>& polystart) {
		this->polystart = polystart;
	}

	void setPolyvel(const std::vector<Vect2>& polyvel) {
		this->polyvel = polyvel;
	}

	void setTend(double tend) {
		this->tend = tend;
	}

};

}

#endif /* MOVINGPOLYGON2D_H_ */
