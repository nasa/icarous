/*
 * PlanUtil - Utilities for Plans
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef PlanIO_H
#define PlanIO_H

#include "Units.h"
#include "NavPoint.h"
#include "Position.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "EuclideanProjection.h"
#include "Plan.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>

namespace larcfm {
class PlanIO {

public:


	static void savePlan(const Plan& plan, std::string str) ;



};

}
#endif
