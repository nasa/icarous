/*
u * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#ifndef TrajTemplates_H
#define TrajTemplates_H

#include "Plan.h"
//#include "UnitSymbols.h"
#include "Vect3.h"
#include "NavPoint.h"
#include "GreatCircle.h"
#include "ProjectedKinematics.h"
#include "Velocity.h"
#include "VectFuns.h"
#include "format.h"
#include "Util.h"
#include "Constants.h"
#include "string_util.h"
#include "Triple.h"
#include <iostream>
#include <sstream>
#include <cctype>
#include <string>
#include <cmath>
#include <stdexcept>
#include <vector>



namespace larcfm {

/**
 * Trajectory generation functionality.  This class translates between Linear and Kinematic plans.  Note that the translations are 
 * not currently robust.
 * 
 */
class TrajTemplates {

public:


	static Plan makeLPC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt);

	static Plan makeKPC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt) ;

	static Plan makeLPC_Turn(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt);

	static Plan makeKPC_Turn(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs, double cruiseAlt);

	static Plan makeLPC_FLC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs) ;

	static Plan makeKPC_FLC(const LatLonAlt& start, const LatLonAlt& end, double gs, double vs) ;

	/* Take a linear PlanCore and assume first and last waypoint are airports.
	 * First make the altitude of those waypoints 0.
	 * Then add a top of climb point and beginning of descent point.
	 * Also add level sections at beginning and end
	 */
	static Plan addClimbDescent(Plan lpc, double gs, double vs, double cruiseAlt);

};

}
#endif 

