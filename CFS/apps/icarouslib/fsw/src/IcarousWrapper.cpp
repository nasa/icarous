#include "IcarousWrapper.h"
#include "Icarous.h"
#include "ParameterData.h"

#define TO_CPP(a) (reinterpret_cast<Icarous_t*>(a))
#define TO_C(a)   (reinterpret_cast<icarous_t*>(a))

struct icarous_t *icarous_create_init(icarous_table_t* pData){

     ParameterData params;
	 params.setInternal("TAKEOFF_ALT",pData->takeoffAlt,"unspecified");
	 params.setInternal("ALLOW_YAW",pData->allowYaw,"unspecified");
	 params.setInternal("MAX_CEILING",pData->maxCeiling,"unspecified");
	 params.setInternal("HTHRESHOLD",pData->hThreshold,"unspecified");
	 params.setInternal("VTHRESHOLD",pData->vThreshold,"unspecified");
	 params.setInternal("HSTEPBACK",pData->hStepback,"unspecified");
	 params.setInternal("VSTEPBACK",pData->vStepback,"unspecified");
	 params.setInternal("CHEAP_SEARCH",pData->cheapSearch,"unspecified");
	 params.setInternal("GRID_SIZE",pData->gridSize,"unspecified");
	 params.setInternal("BUFFER",pData->buffer,"unspecified");
	 params.setInternal("LOOKAHEAD",pData->lookahead,"unspecified");
	 params.setInternal("PROXFACTOR",pData->proxFactor,"unspecified");
	 params.setInternal("RES_SPEED",pData->resolutionSpeed,"unspecified");
	 params.setInternal("XTRK_GAIN",pData->xtrkGain,"unspecified");
	 params.setInternal("XTRK_DEV",pData->xtrkDev,"unspecified");
	 params.setInternal("TRK_HEADING",pData->trkHeading,"unspecified");
	 params.setInternal("TRK_DISTH",pData->trkDistH,"unspecified");
	 params.setInternal("TRK_DISTV",pData->trkDistV,"unspecified");
	 params.set("DAA_CONFIG",pData->daaConfig);
	 params.setInternal("CHEAP_DAA",pData->cheapDAA,"unspecified");
	 params.setInternal("GOTO_NEXTWP",pData->gotoNextWP,"unspecified");
	 params.setInternal("CONFLICT_HOLD",pData->conflictHold,"unspecified");
	 params.setInternal("CAPTURE_H",pData->Capture_H,"unspecified");
	 params.setInternal("CAPTURE_V",pData->Capture_V,"unspecified");
	 params.setInternal("WAIT",pData->WaitAtWaypoint,"unspecified");

	 
	 Icarous_t *IC = new Icarous_t();
	 IC->InputParamTable(&params);
	 icarous_t *ic = TO_C(IC);
	 return ic;
}

void icarous_destroy(icarous_t* ic){
	delete TO_CPP(ic);
}

void icarous_setFlightData(struct icarous_t* ic,waypoint_t *wp){
	TO_CPP(ic)->InputFlightPlanData(wp);
}

void icarous_setGeofenceData(struct icarous_t* ic,geofence_t *gfdata){
	TO_CPP(ic)->InputGeofenceData(gfdata);
}

void icarous_StartMission(struct icarous_t *ic,float param1){
	TO_CPP(ic)->InputStartMission((int)param1);
}

int icarous_GetCommand(struct icarous_t *ic,ArgsCmd_t* cmd){
	return TO_CPP(ic)->OutputCommand(cmd);
}

void icarous_run(struct icarous_t *ic){
	TO_CPP(ic)->RunFMS();
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
	return TO_CPP(ic)->OutputKinematicBands(bands);
}

int32_t ICAROUS_LibInit(void){
	printf("ICAROUS version 1.1\n");
	return 0;
}
