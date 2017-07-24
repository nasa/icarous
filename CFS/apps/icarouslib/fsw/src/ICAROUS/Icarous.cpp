/**
 * ICAROUS
 *
 * Icarous definitions
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

#include "Icarous.h"
#include "Position.h"
#include "Velocity.h"

#define TO_CPP(a) (reinterpret_cast<Icarous_t*>(a))
#define TO_C(a)   (reinterpret_cast<icarous_t*>(a))

std::string Icarous_t::VERSION = "1.2.1";

std::string Icarous_t::release() {
	return "ICAROUS++ V-"+VERSION+
			"-FormalATM-"+Constants::version+" (July-24-2017)";
}

Icarous_t::Icarous_t(icarous_table_t* pData):FlightData(pData),FMS(FlightData){
	cout << "ICAROUS Release: " << release() << std::endl;
}


void Icarous_t::SetFlightPlanData(waypoint_t* wp){
	FlightData.AddMissionItem(wp);
}

void Icarous_t::SetGeofenceData(geofence_t* gf){

	if(gf->vertexIndex == 0){
		tempVertices.clear();
	}
	tempVertices.push_back(*gf);

	if(gf->vertexIndex+1 == gf->totalvertices){
		Geofence_t fence((int)gf->index,(FENCE_TYPE)gf->type,(int)gf->totalvertices,gf->floor,gf->ceiling,FlightData.paramData);
		for(geofence_t sgf: tempVertices){
			fence.AddVertex(sgf.vertexIndex,sgf.latitude,sgf.longitude);
		}

		if(FlightData.fenceList.size() <= gf->index){
			FlightData.fenceList.push_back(fence);
			cout << "Got fence: "<<gf->index <<endl;
		}
		else{
			std::list<Geofence_t>::iterator it;
			for(it = FlightData.fenceList.begin(); it != FlightData.fenceList.end(); ++it){
				if(it->GetID() == fence.GetID()){
					it = FlightData.fenceList.erase(it);
					FlightData.fenceList.insert(it,fence);
					break;
				}
			}
		}
	}
}

void Icarous_t::StartMission(int param1){
	FlightData.SetStartMissionFlag(param1);
}

int Icarous_t::GetCommand(ArgsCmd_t* cmd){
	if(FlightData.commandList.size() > 0){
		ArgsCmd_t icCmd;
		icCmd = (ArgsCmd_t)FlightData.commandList.front();
		cmd->name = icCmd.name;
		cmd->param1 = icCmd.param1;
		cmd->param2 = icCmd.param2;
		cmd->param3 = icCmd.param3;
		cmd->param4 = icCmd.param4;
		cmd->param5 = icCmd.param5;
		cmd->param6 = icCmd.param6;
		cmd->param7 = icCmd.param7;
		cmd->param8 = icCmd.param8;
		FlightData.commandList.pop_front();
		return FlightData.commandList.size();
	}else{
		return -1;
	}

}

void Icarous_t::Run(){
	FMS.RunFMS();
}

void Icarous_t::InputAck(CmdAck_t* ack){
	FlightData.commandAckList.push_back(*ack);
}

void Icarous_t::InputPosition(position_t* pos){
	Position currentPos = Position::makeLatLonAlt(pos->latitude,"degree",pos->longitude,"degree",pos->altitude_rel,"m");
	Velocity currentVel = Velocity::makeVxyz(pos->vy,pos->vx,"m/s",pos->vz,"m/s");

	FlightData.acState.add(currentPos,currentVel,pos->time_gps);
	FlightData.acTime = pos->time_gps;
}

void Icarous_t::InputAttitude(attitude_t* att){
	double roll, pitch, yaw, heading;


	heading = FlightData.acState.velocityLast().track("degree");
	if(heading < 0){
		heading = 360 + heading;
	}

	FlightData.roll = att->roll;
	FlightData.pitch = att->pitch;
	FlightData.yaw = att->yaw;
	FlightData.heading = heading;
}

void Icarous_t::InputMissionItemReached(missionItemReached_t* msnItem){
	FlightData.nextMissionWP++;
}

void Icarous_t::InputTraffic(object_t* traffic){
	FlightData.AddTraffic(traffic->index,traffic->latitude,traffic->longitude,traffic->altiude,
			traffic->vx,traffic->vy,traffic->vz);
}

int Icarous_t::SendKinematicBands(visbands_t* bands){
	if(FlightData.visBands.numBands > 0){
		bands->numBands = FlightData.visBands.numBands;
		bands->type1 = FlightData.visBands.type1;
		bands->type2 = FlightData.visBands.type2;
		bands->type3 = FlightData.visBands.type3;
		bands->type4 = FlightData.visBands.type4;
		bands->type5 = FlightData.visBands.type5;
		bands->min1  = FlightData.visBands.min1;
		bands->min2  = FlightData.visBands.min2;
		bands->min3  = FlightData.visBands.min3;
		bands->min4  = FlightData.visBands.min4;
		bands->min5  = FlightData.visBands.min5;
		bands->max1  = FlightData.visBands.max1;
		bands->max2  = FlightData.visBands.max2;
		bands->max3  = FlightData.visBands.max3;
		bands->max4  = FlightData.visBands.max4;
		bands->max5  = FlightData.visBands.max5;
		return FlightData.visBands.numBands;
	}
	return 0;
}

/*****************************************************************
 * Wrapper functions to interface C code with CPP
 *
 ******************************************************************/
struct icarous_t *icarous_create_init(icarous_table_t* pData){
	 icarous_t *ic = TO_C(new Icarous_t(pData));
	 return ic;
}

void icarous_destroy(icarous_t* ic){
	delete TO_CPP(ic);
}

void icarous_setFlightData(struct icarous_t* ic,waypoint_t *wp){
	TO_CPP(ic)->SetFlightPlanData(wp);
}

void icarous_setGeofenceData(struct icarous_t* ic,geofence_t *gfdata){
	TO_CPP(ic)->SetGeofenceData(gfdata);
}

void icarous_StartMission(struct icarous_t *ic,float param1){
	TO_CPP(ic)->StartMission((int)param1);
}

int icarous_GetCommand(struct icarous_t *ic,ArgsCmd_t* cmd){
	return TO_CPP(ic)->GetCommand(cmd);
}

void icarous_run(struct icarous_t *ic){
	TO_CPP(ic)->Run();
}

void icarous_inputAck(struct icarous_t *ic,CmdAck_t* ack){
	TO_CPP(ic)->InputAck(ack);
}

void icarous_inputPosition(struct icarous_t *ic,position_t* pos){
	TO_CPP(ic)->InputPosition(pos);
}

void icarous_inputAttitude(struct icarous_t *ic,attitude_t* att){
	TO_CPP(ic)->InputAttitude(att);
}

void icarous_inputMissionItemReached(struct icarous_t *ic,missionItemReached_t* msnItem){
	TO_CPP(ic)->InputMissionItemReached(msnItem);
}

void icarous_inputTraffic(struct icarous_t* ic,object_t* traffic){
	TO_CPP(ic)->InputTraffic(traffic);
}

int icarous_sendKinematicBands(struct icarous_t* ic,visbands_t* bands){
	return TO_CPP(ic)->SendKinematicBands(bands);
}

int32_t ICAROUS_LibInit(void){

	printf("ICAROUS version 1.1\n");

	return 0;
}


