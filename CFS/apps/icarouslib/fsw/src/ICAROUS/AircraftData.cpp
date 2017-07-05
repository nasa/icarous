/**
 * Aircraft data
 * 
 * Shared data structure containing all flight relevant data and functions
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
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM, 
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT, 
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.  
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

 #include "AircraftData.h"
 #include <iostream>
 AircraftData_t::AircraftData_t(icarous_table_t* pData){
     startMission = -1;
     nextMissionWP = 0;
     nextResolutionWP = 0;
     crossTrackDeviation = 0;
     crossTrackOffset = 0;
     heading = 0;
     roll = 0;
     pitch = 0;
     yaw = 0;
     maneuverHeading = 0;
     maneuverVe = 0;
     maneuverVn = 0;
     maneuverVu = 0;
     reset = false;
     paramData = pData;
 }

 void AircraftData_t::AddMissionItem(waypoint_t* wp){
	 listMissionItem.push_back(*wp);
 }

uint8_t AircraftData_t::GetStartMissionFlag(){
    int var = startMission;
    startMission = -1;
    return var;
}

void AircraftData_t::SetStartMissionFlag(uint8_t flag){
    startMission = flag;
}

uint16_t AircraftData_t::GetFlightPlanSize(){
    uint16_t size = listMissionItem.size();
    return size;
}

void AircraftData_t::ClearMissionList(){
	MissionPlan.clear();
	listMissionItem.clear();
}

void AircraftData_t::ConstructPlan(){
	// Create a Plan object with the available mission items
	MissionPlan.clear();
	std::list<waypoint_t>::iterator it;
	int ic;
	for(it = listMissionItem.begin(),ic = 0;
		it != listMissionItem.end(); ++it,++ic){
			Position WP = Position::makeLatLonAlt(it->latitude,"degree",it->longitude,"degree",it->altitude,"m");
			double wptime = 0;
			if(ic > 0){
				double vel = it->speed;
				if(vel < 0.5){
					vel = 1;
				}
				double distance = MissionPlan.point(ic - 1).position().distanceH(WP);
				if(distance < 0.01){
					distance = MissionPlan.point(ic - 1).position().distanceV(WP);
					vel = 1;
				}
				wptime   = MissionPlan.time(ic-1) + distance/vel;
			}
			NavPoint navPoint(WP,wptime);
			MissionPlan.addNavPoint(navPoint);
	}
}

void AircraftData_t::AddTraffic(int id,double lat,double lon,double alt,double vx,double vy,double vz){
	GenericObject_t newTraffic(1,id,lat,lon,alt,vx,vy,vz);
	GenericObject_t::AddObject(trafficList,newTraffic);
	Position pos;Velocity vel;

}

void AircraftData_t::GetTraffic(int id,Position& pos,Velocity& vel){
	std::list<GenericObject_t>::iterator it;
	for(it = trafficList.begin(); it != trafficList.end(); ++it){
		if(it->id == id){
			pos = it->pos;
			vel = it->vel;
			break;
		}
	}
}

void AircraftData_t::AddMissionObject(int id,double lat,double lon,double alt,double vx,double vy,double vz){
	GenericObject_t obj(0,id,lat,lon,alt,vx,vy,vz);
	std::list<GenericObject_t>::iterator it;
	GenericObject_t::AddObject(missionObjList,obj);
}

void AircraftData_t::AddTrackingObject(int id,double lat,double lon,double alt,double vx,double vy,double vz){
	GenericObject_t obj(2,id,lat,lon,alt,vx,vy,vz);
	TrackingObject = obj;
}

void AircraftData_t::Reset(){
	 startMission = -1;
	 nextMissionWP = 0;
	 nextResolutionWP = 0;
	 crossTrackDeviation = 0;
	 crossTrackOffset = 0;
	 heading = 0;
	 roll = 0;
	 pitch = 0;
	 yaw = 0;
	 maneuverHeading = 0;
	 maneuverVe = 0;
	 maneuverVn = 0;
	 maneuverVu = 0;
	 reset = true;
}
