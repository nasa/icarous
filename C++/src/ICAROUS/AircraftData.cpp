/**
 * Aircraft data
 * 
 * Shared data structure containing all flight relevent data and functions
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


 AircraftData_t::AircraftData_t(MAVLinkMessages_t* Msgs,ParameterData* pData){
     pthread_mutex_init(&lock, NULL);
     RcvdMessages = Msgs;
     startMission = -1;
     nextMissionWP = 0;
     nextResolutionWP = 0;
     paramData    = pData;
 }

 void AircraftData_t::AddMissionItem(mavlink_mission_item_t msg){

     pthread_mutex_lock(&lock);
     listMissionItem.push_back(msg);
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

void AircraftData_t::ConstructPlan(){
	// Create a Plan object with the available mission items
	MissionPlan.clear();
	std::list<mavlink_mission_item_t>::iterator it;
	int ic;
	for(it = listMissionItem.begin(),ic = 0;
		it != listMissionItem.end(); ++it,++ic){
		if(it->command == MAV_CMD_NAV_WAYPOINT ||
		   it->command == MAV_CMD_NAV_SPLINE_WAYPOINT ){
			Position WP = Position::makeLatLonAlt(it->x,"degree",it->y,"degree",it->z,"m");
			double wptime = 0;
			if(ic > 0){

				double vel = it->param4;

				if(vel < 0.5){
					vel = 1;
				}

				double distance = MissionPlan.point(ic - 1).position().distanceH(WP);
				wptime   = MissionPlan.getTime(ic-1) + distance/vel;
			}

			NavPoint navPoint(WP,wptime);
			MissionPlan.add(navPoint);
		}
	}
}

void AircraftData_t::GetGeofence(Interface_t *gsIntf,mavlink_command_long_t msgIn){
	bool readComplete = false;
	uint16_t nVertices     = (int) msgIn.param4;

	Geofence_t fence((int)msgIn.param2,(FENCE_TYPE)msgIn.param3,(int)msgIn.param4,
					msgIn.param5,msgIn.param6,paramData);

	time_t starttime;
	time_t polltime;
	double seconds;
	uint8_t state = 0;
	uint16_t count = 0;
	mavlink_message_t msg;

	time(&starttime);

	while(!readComplete){


		time(&polltime);
		seconds   = difftime(starttime,polltime);

		if(seconds > 10){
			break;
		}

		switch(state){

		case 0:

			mavlink_msg_fence_fetch_point_pack(1,1,&msg,255,0,count);
			gsIntf->SendMAVLinkMsg(msg);
			state = 1;
			break;

		case 1:
			gsIntf->GetMAVLinkMsg();
			mavlink_fence_point_t msgFencePoint;
			bool have_msg;
			have_msg = RcvdMessages->GetFencePoint(msgFencePoint);
			if(have_msg){
				if(msgFencePoint.idx == count){
					fence.AddVertex(msgFencePoint.idx,msgFencePoint.lat,msgFencePoint.lng);
					count++;
					time(&starttime);
					state = 0;
				}
			}

			if(count == nVertices){
				state = 2;
			}
			break;

		case 2:
			readComplete = true;

			if(fenceList.size() >= fence.GetID()){
				fenceList.push_back(fence);
			}
			else{
				std::list<Geofence_t>::iterator it;
				for(it = fenceList.begin(); it != fenceList.end(); ++it){
					if(it->GetID() == fence.GetID()){
						it = fenceList.erase(it);
						fenceList.insert(it,fence);
						break;
					}
				}
			}
			mavlink_msg_command_ack_pack(1,0,&msg,MAV_CMD_DO_FENCE_ENABLE,1);
			gsIntf->SendMAVLinkMsg(msg);
			break;
		}

	}
}

void AircraftData_t::AddTraffic(int id,double x,double y,double z,double vx,double vy,double vz){

	pthread_mutex_lock(&lock);
	std::list<Object_t>::iterator it;
	bool present = false;
	if(!trafficList.empty()){
		for(it = trafficList.begin(); it != trafficList.end(); ++it){
			if(it->id == id){
				it->x = x;
				it->y = y;
				it->z = z;
				it->vx = vx;
				it->vy = vy;
				it->vz = vz;
				present = true;
				break;
			}


		}
	}

	if(!present){
		Object_t newTraffic = {id,x,y,z,vx,vy,vz,0};
		trafficList.push_back(newTraffic);
	}
	pthread_mutex_unlock(&lock);
}
