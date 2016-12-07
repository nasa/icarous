 /**
 * Geofence
 *
 * Class to store geofence, detect geofence conflicts and resolve them.
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 *
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.
 *  All rights reserved.
 *
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED,
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT,
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,s
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

#ifndef _GEOFENCE_H_
#define _GEOFENCE_H_

#include <vector>
#include "Position.h"
#include "Velocity.h"
#include "Vect2.h"
#include "Vect3.h"
#include "AircraftState.h"
#include "EuclideanProjection.h"
#include "ParameterData.h"
#include "SimplePoly.h"
#include "Poly3D.h"
#include "CDPolycarp.h"
#include "PolycarpDetection.h"
#include "PolycarpResolution.h"
#include "math.h"

using namespace larcfm;

enum FENCE_TYPE {KEEP_IN = 0,KEEP_OUT = 1};

class Geofence_t{

private:

	FENCE_TYPE fenceType;
	uint16_t id;
	uint16_t nVertices;
	double floor;
	double ceiling;
	const double BUFF = 0.1;
	ParameterData* params;
	bool violation;
	bool conflict;
	bool projectedViolation;
	EuclideanProjection proj;
	SimplePoly geoPoly0;          // Original polygon in lat,lon
	SimplePoly geoPoly1;          // Expanded/Contracted polygon in lat,lon
	Poly3D geoPoly3D;             // 3D polygon in cartesian coordinates
	CDPolycarp geoPolyCarp;
	PolycarpResolution geoPolyResolution;
	PolycarpDetection geoPolyDetect;
	std::vector<Vect2> fenceVertices0; // Cartesian coordinates of vertices of geoPoly0
	std::vector<Vect2> fenceVertices1; //Cartesian coordinates of vertices of geoPoly1
	Position recoveryPoint;


public:
	Geofence_t(int ID, FENCE_TYPE ftype, uint16_t nVert, double infloor, double inCeiling,ParameterData* pData);
	void AddVertex(int index, double lat, double lon);
	void CheckViolation(AircraftState acState);
	bool CollisionDetection(Position pos, Vect2 v,double startTime, double stopTime);
	bool CheckWPFeasibility(Position current, Position nextWP);
	uint16_t GetID();
};



#endif /* _GEOFENCE_H_ */
