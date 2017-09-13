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

#include <Icarous_msg.h>
#include "AircraftData.h"

 AircraftData_t::AircraftData_t(ParameterData* pData){
     pthread_mutex_init(&lock, NULL);
     startMission = -1;
     nextMissionWP = 0;
     nextResolutionWP = 0;
     paramData    = pData;
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
     missionSpeed = 1;
     reset = false;
     visBands.numBands = 0;
 }

 void AircraftData_t::AddMissionItem(waypoint_t* msg){

     pthread_mutex_lock(&lock);
     listMissionItem.push_back(*msg);
     pthread_mutex_unlock(&lock);
 }

uint8_t AircraftData_t::GetStartMissionFlag(){
    int var;
    pthread_mutex_lock(&lock);
    var = startMission;
    startMission = -1;
    pthread_mutex_unlock(&lock);
    return var;
}

void AircraftData_t::SetStartMissionFlag(uint8_t flag){
    pthread_mutex_lock(&lock);
    startMission = flag;
    pthread_mutex_unlock(&lock);
}

uint16_t AircraftData_t::GetFlightPlanSize(){
    int size;
    pthread_mutex_lock(&lock);
    size = listMissionItem.size();
    pthread_mutex_unlock(&lock);
    return size;
}

void AircraftData_t::ClearMissionList(){
	listMissionItem.clear();
}

void AircraftData_t::ConstructPlan(){
	// Create a Plan object with the available mission items
	MissionPlan.clear();
	std::list<waypoint_t>::iterator it;
	int ic;
	for(it = listMissionItem.begin(),ic = 0;
		it != listMissionItem.end(); ++it,++ic){
		larcfm::Position WP = Position::makeLatLonAlt(it->latitude,"degree",it->longitude,"degree",it->altitude,"m");
		double wptime = 0;
		if(ic > 0){

				double vel = it->speed;

				if(vel < 0.5){
					vel = 2;
				}

				double distance = MissionPlan.point(ic - 1).position().distanceH(WP);
				wptime   = MissionPlan.time(ic-1) + distance/vel;
			}
			NavPoint navPoint(WP,wptime);
			MissionPlan.addNavPoint(navPoint);
	}
    printf("Constructed Flight plan with %d waypoints\n",MissionPlan.size());
}

double AircraftData_t::getFlightPlanSpeed(Plan* fp,int nextWP){
	double speed = fp->pathDistance(nextWP-1,true)/(fp->time(nextWP) - fp->time(nextWP-1));
	return speed;
}

void AircraftData_t::AddTraffic(int id,double lat,double lon,double alt,double vx,double vy,double vz){
	pthread_mutex_lock(&lock);
	GenericObject_t newTraffic(1,id,lat,lon,alt,vx,vy,vz);
	GenericObject_t::AddObject(trafficList,newTraffic);
	larcfm::Position pos;larcfm::Velocity vel;
	pthread_mutex_unlock(&lock);
}

void AircraftData_t::GetTraffic(int id,larcfm::Position& pos,larcfm::Velocity& vel){
	pthread_mutex_lock(&lock);
	std::list<GenericObject_t>::iterator it;
	for(it = trafficList.begin(); it != trafficList.end(); ++it){
		if(it->id == id){
			pos = it->pos;
			vel = it->vel;
			break;
		}
	}
	pthread_mutex_unlock(&lock);
}
void AircraftData_t::Reset(){
	 pthread_mutex_lock(&lock);
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
	 pthread_mutex_unlock(&lock);
}

bool AircraftData_t::CheckAck(command_name_t command){
	bool status = false;
	pthread_mutex_lock(&lock);
	while(commandAckList.size() > 0){
		CmdAck_t ack = commandAckList.front();
		commandAckList.pop_front();
		if(ack.name == command && ack.result == 0){
			status = true;
			pthread_mutex_unlock(&lock);
			return status;
		}
	}
	pthread_mutex_unlock(&lock);
	return status;
}

void AircraftData_t::InputAck(CmdAck_t *ack) {
	pthread_mutex_lock(&lock);
	commandAckList.push_back(*ack);
	pthread_mutex_unlock(&lock);
}