/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#ifndef MOVINGPOLYGON3D_H_
#define MOVINGPOLYGON3D_H_

#include <vector>
#include "MovingPolygon2D.h"
#include "Poly3D.h"
#include "Velocity.h"

namespace larcfm {
class MovingPolygon3D {

public:

	MovingPolygon2D horizpoly; // = MovingPolygon2D();
	double vspeed;
	double minalt;
	double maxalt;

	MovingPolygon3D();

	MovingPolygon3D(const MovingPolygon2D& horizpoly, double vspeed, double minalt, double maxalt);

	MovingPolygon3D(const Poly3D& p, const Velocity& v, double end);

	MovingPolygon3D(const Poly3D& p, const std::vector<Velocity>& v, double end);

	Poly3D position(double t) const;

	Velocity velocity(int i) const;

	Velocity averageVelocity() const;

	MovingPolygon3D linear(double t) const;

	bool isStable() const;

	int size() const;

	std::string toString() const;

	MovingPolygon3D reverseOrder() const;

	const MovingPolygon2D& getHorizpoly() const {
		return horizpoly;
	}

	void setHorizpoly(const MovingPolygon2D& horizpoly) {
		this->horizpoly = horizpoly;
	}

	double getMaxalt() const {
		return maxalt;
	}

	void setMaxalt(double maxalt) {
		this->maxalt = maxalt;
	}

	double getMinalt() const {
		return minalt;
	}

	void setMinalt(double minalt) {
		this->minalt = minalt;
	}

	double getVspeed() const {
		return vspeed;
	}

	void setVspeed(double vspeed) {
		this->vspeed = vspeed;
	}


};


}

#endif /* MOVINGPOLYGON3D_H_ */
