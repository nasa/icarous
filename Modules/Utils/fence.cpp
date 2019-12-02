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

#include "fence.h"

fence::fence(int ID, FENCE_TYPE ftype, uint16_t nVert, double infloor, double inCeiling){
	fenceType = ftype;
	id        = ID;
	nVertices = nVert;
	floor     = infloor;
	ceiling   = inCeiling;
	conflict = false;
	violation = false;
	projectedViolation = false;
    geoCDIIPolygon = CDIIPolygon(&geoPolyCarp);
    entryTime = 0;
    exitTime = 0;
}

void fence::AddVertex(int index, double lat,double lon,double ResolBUFF){
	Position pos = Position::makeLatLonAlt(lat,"degree",lon,"degree",0,"m");
	geoPoly0.add(pos);

	if(geoPoly0.size() == nVertices){
		geoPoly0.setBottom(floor);
		geoPoly0.setTop(ceiling);
		proj = Projection::createProjection(geoPoly0.getVertex(0));
		geoPoly3D = geoPoly0.poly3D(proj);

		for(int i=0;i<nVertices;++i){
			fenceVertices0.push_back(geoPoly3D.get2D(i));
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

		Velocity vel = Velocity::makeTrkGsVs(0,0,0);
		geoPolyPath.addPolygon(geoPoly0,vel,0);
	}
}

int16_t fence::GetID(){
	return id;
}

uint16_t fence::GetSize(){
	return nVertices;
}


Position fence::GetRecoveryPoint(){
	return recoveryPoint;
}

bool fence::GetConflictStatus(){
	return conflict;
}

bool fence::GetProjectedStatus(){
	return projectedViolation;
}

bool fence::GetViolationStatus(){
	return violation;
}

FENCE_TYPE fence::GetType(){
	return fenceType;
}

SimplePoly* fence::GetPoly(){
	return &geoPoly0;
}

SimplePoly* fence::GetPolyMod(){
    return &geoPoly1;
}

Poly3D* fence::GetPoly3D(){
	return &geoPoly3D;
}

PolyPath* fence::GetPolyPath() {
    return &geoPolyPath;
}

EuclideanProjection* fence::GetProjection(){
	return &proj;
}

void fence::GetEntryExitTime(double& in, double &out){
	in = entryTime;
	out = exitTime;
}

double fence::GetCeiling(){
	return ceiling;
}

void fence::Print(){
	std::cout<<"id:"<<id<<std::endl;
	std::cout<<"type:"<<fenceType<<std::endl;
	std::cout<<"floor:"<<floor<<std::endl;
	std::cout<<"roof:"<<ceiling<<std::endl;
	for(int i=0;i<nVertices;++i){
		std::cout<<geoPoly3D.get2D(i).toString(6)<<std::endl;
	}
}

std::vector<Vect2>* fence::getCartesianVertices(){
    return &fenceVertices0;
}

std::vector<Vect2>* fence::getModCartesianVertices(){
    return &fenceVertices1;
}