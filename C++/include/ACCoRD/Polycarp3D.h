/*
 * Polycarp3D - containment and conflict detection for 3D polygons
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * Polycarp3D.h
 *
 *  Created on: Nov 23, 2015
 *      Author: ghagen
 */

#ifndef POLYCARP3D_H_
#define POLYCARP3D_H_

#include "Vect3.h"
#include "Poly3D.h"
#include "Velocity.h"
#include "MovingPolygon3D.h"
#include "IntervalSet.h"

namespace larcfm {

class Polycarp3D {
public:
static bool nearEdge(const Vect3& so, const Poly3D& p, double h, double v, bool checkNice);
static bool definitely_inside(const Vect3& so, const Poly3D& p, double buff, bool checkNice);
static bool definitely_outside(const Vect3& so, const Poly3D& p, double buff, bool checkNice);

static bool violation(const Vect3& so, const Poly3D& p, double buff, bool checkNice);
static bool entranceDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac, bool checkNice);
static bool exitDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac, bool checkNice);
static IntervalSet conflictTimes(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac, bool startOutside, bool checkNice);
};

}

#endif /* SRC_POLYCARP3D_H_ */
