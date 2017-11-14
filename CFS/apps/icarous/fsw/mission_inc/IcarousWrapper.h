#ifndef _ICAROUS_WRAPPER_
#define _ICAROUS_WRAPPER_

#include "Icarous_msg.h"
#include "icarous_table.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct icarous_t; // a nice opaque type
    struct icarous_t *icarous_create_init(icarous_table_t* pData);
    void icarous_destroy(struct icarous_t *ic);

    void icarous_setFlightData(struct icarous_t* ic,waypoint_t* wp);
    void icarous_setGeofenceData(struct icarous_t* ic,geofence_t* gfdata);
    void icarous_StartMission(struct icarous_t *ic,float param);
    int icarous_GetCommand(struct icarous_t *ic, ArgsCmd_t* cmd);
    void icarous_run(struct icarous_t* ic);
    void icarous_inputAck(struct icarous_t* ic,CmdAck_t* ack);
    void icarous_inputPosition(struct icarous_t* ic,position_t* pos);
    void icarous_inputAttitude(struct icarous_t* ic,attitude_t* att);
    void icarous_inputMissionItemReached(struct icarous_t* ic,missionItemReached_t* msnItem);
    void icarous_inputTraffic(struct icarous_t* ic,object_t* traffic);
    int icarous_sendKinematicBands(struct icarous_t* ic,visbands_t* bands);
    int32_t ICAROUS_LibInit(void);

#ifdef __cplusplus
}
#endif

#endif
