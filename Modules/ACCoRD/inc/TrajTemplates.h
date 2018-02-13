/*
u * Copyright (c) 2011-2017 United States Government as represented by
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
 * Note: there are several global variables that are settable by the user and modify low-level aspects of the transformation:
 * trajKinematicsTrack: allow track transformations (default true, if false, no turn TCPs will be generated)
 * trajKinematicsGS: allow gs transformations (default true, if false, no ground speed TCPs will be generated)
 * trajKinematicsVS: allow vs transformations (default true, if false, no vertical speed TCPs will be generated)
 * trajPreserveGS: prioritize preserving grounds speeds (default FALSE. If true, times may change, if false point times should be preserved)
 * trajAccelerationReductionAllowed: if true, if there are vs end points that nearly overlap with existing points, allow the acceleration to be adjusted so that they actually do overlap or are sufficiently distinct that there will not be problems inferring the velocities between them. (default true)
 * 
 * These values may be set through setter methods.
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

