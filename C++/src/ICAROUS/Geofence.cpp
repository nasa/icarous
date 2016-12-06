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

#include "Geofence.h"

Geofence::Geofence(int ID, FENCE_TYPE ftype, uint16_t nVert, double infloor, double inCeiling,ParameterData* pData){
	fenceType = ftype;
	id        = ID;
	nVertices = nVert;
	floor     = infloor;
	ceiling   = inCeiling;
	params    = pData;
}

void Geofence::AddVertex(int index, double lat,double lon){
	Position pos = Position::makeLatLonAlt(lat,"degree",lon,"degree",0,"m");
	geoPoly0.addVertex(pos);

	double ResolBUFF = params->getValue("HTRHESHOLD"); // expansion/contraction amount

	if(geoPoly0.size() == nVertices){
		proj = Projection::createProjection(geoPoly0.getVertex(0));
		geoPoly3D = geoPoly0.poly3D(proj);

		for(int i=0;i<nVertices;++i){
			fenceVertices0.push_back(geoPoly3D.getVertex(0));
		}

		if(fenceType == KEEP_IN){
			fenceVertices1 = geoPolyResolution.contract_polygon_2D(BUFF,ResolBUFF,fenceVertices0);
		}
		else{
			fenceVertices1 = geoPolyResolution.expand_polygon_2D(BUFF,ResolBUFF,fenceVertices0);
		}

		Poly2D p2D(fenceVertices1);
		Poly3D p3D(p2D,floor,ceiling);
		geoPoly1 = SimplePoly::make(p3D,proj);
	}
}

void Geofence::CheckViolation(AircraftState acState){
	double hdist;
	double vdist;
	double alt;

	Position currentPosLLA = acState.positionLast();
	Velocity currentVel    = acState.velocityLast();
	Vect3 currentPosR3     = proj.project(currentPosLLA);

	double lookahead  = params->getValue("LOOKAHEAD");
	double hthreshold = params->getValue("HTHRESHOLD");
	double vthreshold = params->getValue("VTHRESHOLD");
	double hstepback  = params->getValue("HSTEPBACK");

	if(fenceType == KEEP_IN){
		if(geoPolyCarp.nearEdge(currentPosR3,geoPoly3D,hthreshold,vthreshold)){
			conflict = true;
		}
		else{
			conflict = false;
		}

		projectedViolation = CollisionDetection(currentPosLLA,currentVel.vect2(),0,3);

		if(geoPolyCarp.definitelyInside(currentPosR3,geoPoly3D)){
			violation = false;
		}else{
			violation = true;
		}

		Vect2 recPointR2 = geoPolyResolution.inside_recovery_point(BUFF,hstepback,
												fenceVertices0,currentPosR3.vect2());
		LatLonAlt LLA = proj.inverse(recPointR2,currentPosLLA.alt());
		recoveryPoint = Position::makeLatLonAlt(LLA.latitude(),"degree",
										        LLA.longitude(),"degree",
												LLA.altitude(),"ft");
	}
	else{
		Vect2 recPointR2 = geoPolyResolution.outside_recovery_point(BUFF,hstepback,
														fenceVertices0,currentPosR3.vect2());
		LatLonAlt LLA = proj.inverse(recPointR2,currentPosLLA.alt());
		recoveryPoint = Position::makeLatLonAlt(LLA.latitude(),"degree",
												LLA.longitude(),"degree",
												LLA.altitude(),"ft");
	}



}

bool Geofence::CollisionDetection(Position pos,Vect2 v,double startTime,double stopTime){
	Vect2 currentPos = proj.project(pos).vect2();
	Vect2 polygonVel(0,0);
	bool insideBad = false;

	if(fenceType == KEEP_OUT){
		insideBad = true;
	}

	bool val = geoPolyDetect.Static_Collision_Detector(startTime,stopTime,fenceVertices0,
														polygonVel,currentPos,v,BUFF,insideBad);

	return val;
}

bool Geofence::CheckWPFeasibility(Position currentPos,Position nextWP){
	double heading2WP = currentPos.track(nextWP);

	// Velocity components assuming speed is 1 m/s
	double vy = cos(heading2WP);
	double vx = sin(heading2WP);
	Vect2 vel(vx,vy);

	bool val = CollisionDetection(currentPos,vel,0,5);

	return val;
}

uint16_t Geofence::GetID(){
	return id;
}
