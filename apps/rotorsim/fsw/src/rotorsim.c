//
// Created by Swee Balachandran on 12/22/17.
//

#include <math.h>
#include "rotorsim.h"
#include "UtilFunctions.h"
#include "rotorsim_table.h"
#include "PC_Quadcopter_Simulation.h"
#include "rtwtypes.h"
#include "rotorsim_tbl.c"

/// Event ID filter definition
CFE_EVS_BinFilter_t  RotorSim_EventFilters[] =
        {  /* Event ID    mask */
                {ROTORSIM_STARTUP_INF_EID,       0x0000},
                {ROTORSIM_COMMAND_ERR_EID,       0x0000},
        };

/* Application entry points */
void RotorSim_AppMain(void){

    int32 status;
    uint32 RunStatus = CFE_ES_APP_RUN;

    Rotorsim_AppInit();

    while(CFE_ES_RunLoop(&RunStatus) == TRUE){
        status = CFE_SB_RcvMsg(&rotorsimAppData.Rotorsim_MsgPtr, rotorsimAppData.Rotorsim_Pipe, 10);

        if (status == CFE_SUCCESS)
        {
            Rotorsim_ProcessPacket();
        }
    }

    Rotorsim_AppCleanUp();

    CFE_ES_ExitApp(RunStatus);
}

void Rotorsim_AppInit(void) {

    memset(&rotorsimAppData, 0, sizeof(rotorsimAppData_t));

    int32 status;

    // Register the app with executive services
    CFE_ES_RegisterApp();

    // Register the events
    CFE_EVS_Register(RotorSim_EventFilters,
                     sizeof(RotorSim_EventFilters) / sizeof(CFE_EVS_BinFilter_t),
                     CFE_EVS_BINARY_FILTER);

    // Create pipe to receive SB messages
    status = CFE_SB_CreatePipe(&rotorsimAppData.Rotorsim_Pipe, /* Variable to hold Pipe ID */
                               ROTORSIM_PIPE_DEPTH,       /* Depth of Pipe */
                               ROTORSIM_PIPE_NAME);       /* Name of pipe */


    CFE_SB_SubscribeLocal(ICAROUS_COMMANDS_MID, rotorsimAppData.Rotorsim_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(ICAROUS_FLIGHTPLAN_MID,rotorsimAppData.Rotorsim_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);
    CFE_SB_SubscribeLocal(FREQ_20_WAKEUP_MID,rotorsimAppData.Rotorsim_Pipe,CFE_SB_DEFAULT_MSG_LIMIT);

    // Register table with table services
    CFE_TBL_Handle_t tblHandle;
    status = CFE_TBL_Register(&tblHandle,
                              "RotorsimTable",
                              sizeof(RotorsimTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &RotorsimTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(tblHandle, CFE_TBL_SRC_ADDRESS,&RotorsimTblStruct);


    RotorsimTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, tblHandle);
    memcpy(&rotorsimAppData.Rotorsim_Tbl,TblPtr,sizeof(RotorsimTable_t));

    // Send event indicating app initialization
    if(status == CFE_SUCCESS){
        CFE_EVS_SendEvent(ROTORSIM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                        "Rotorsim App Initialized. Version %d.%d",
                        ROTORSIM_MAJOR_VERSION,
                        ROTORSIM_MINOR_VERSION);
    }


    Rotorsim_AppInitData(TblPtr);
}

void Rotorsim_AppInitData(RotorsimTable_t* TblPtr){
    rotorsimAppData.nextWP = 0;
    rotorsimAppData.time = 0;
    rotorsimAppData.externalCmdV[0] = 0;
    rotorsimAppData.externalCmdV[1] = 0;
    rotorsimAppData.externalCmdV[2] = 0;
    rotorsimAppData.velcount = 0;

    rotorsimAppData.position[0] = rotorsimAppData.Rotorsim_Tbl.originLL[0];
    rotorsimAppData.position[1] = rotorsimAppData.Rotorsim_Tbl.originLL[1];
    rotorsimAppData.position[2] = rotorsimAppData.Rotorsim_Tbl.initialAlt;

    PC_Quadcopter_Simulation_initialize();
    memset(PC_Quadcopter_Simulation_Y.yout,0,sizeof(real_T)*30);
}

void Rotorsim_AppCleanUp(){
    // Do clean up here
}

void Rotorsim_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(rotorsimAppData.Rotorsim_MsgPtr);

    switch(MsgId){

        case FREQ_20_WAKEUP_MID:{
            timer_callback(0);
            break;
        }


        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fp;
            fp = (flightplan_t*) rotorsimAppData.Rotorsim_MsgPtr;
            memcpy(&rotorsimAppData.flightPlan,fp,sizeof(flightplan_t));
            break;
        }

        case ICAROUS_COMMANDS_MID:{
           argsCmd_t *msg = (argsCmd_t*)rotorsimAppData.Rotorsim_MsgPtr;

           switch(msg->name){
               case _SETVEL_:{
                   // set velocity control to true
                   rotorsimAppData.positionControl = false;

                   // store commanded velocity
                   rotorsimAppData.externalCmdV[0] = msg->param1;
                   rotorsimAppData.externalCmdV[1] = msg->param2;
                   rotorsimAppData.externalCmdV[2] = -msg->param3;
                   rotorsimAppData.velcount = 0;
                   break;
               }

               case _TAKEOFF_:{
                   // Send takeoff commands
                   // by the interface app. Each rotorcraft may have its own sequence.
                   missionItemReached_t takeoffStatus;
                   CFE_SB_InitMsg(&takeoffStatus, ICAROUS_WPREACHED_MID, sizeof(missionItemReached_t), TRUE);

                   takeoffStatus.feedback = true;
                   takeoffStatus.reachedwaypoint = 1;
                   strcpy(takeoffStatus.planID, "Takeoff\0");
                   SendSBMsg(takeoffStatus);
               }
           }
           break;
        }
    }
}

int32_t RotorsimTableValidationFunc(void *TblPtr){
    return 0;
}

void timer_callback(uint32_t timerId){
    static bool OverrunFlag = false;
    rotorsimAppData.time += 0.05;
    rotorsimAppData.velcount++;

    /* Check for overrun */
    if (OverrunFlag) {
        rtmSetErrorStatus(PC_Quadcopter_Simulation_M, "Overrun");
        return;
    }

    OverrunFlag = true;

    /* Save FPU context here (if necessary) */
    /* Re-enable timer or interrupt here */
    /* Set model inputs here */

    // Set inputs
    if(rotorsimAppData.velcount < 20){
        PC_Quadcopter_Simulation_U.Xd_I = rotorsimAppData.externalCmdV[1];
        PC_Quadcopter_Simulation_U.Yd_I = rotorsimAppData.externalCmdV[0];
        PC_Quadcopter_Simulation_U.Zd_I = rotorsimAppData.externalCmdV[2];
    }else{
        PC_Quadcopter_Simulation_U.Xd_I = 0;
        PC_Quadcopter_Simulation_U.Yd_I = 0;
        PC_Quadcopter_Simulation_U.Zd_I = 0;
    }

    /* Step the model */
    PC_Quadcopter_Simulation_step();

    /* Get model outputs here */
    Rotorsim_GetOutputs();

    /* Indicate task complete */
    OverrunFlag = false;

    /* Disable interrupts here */
    /* Restore FPU context here (if necessary) */
    /* Enable interrupts here */
}

void Rotorsim_GetOutputs(){

    // Get outputs from relevant structure
    // Current position in local frame
    double currentPosition[3] = {PC_Quadcopter_Simulation_Y.yout[9],
                                 PC_Quadcopter_Simulation_Y.yout[10],
                                 -PC_Quadcopter_Simulation_Y.yout[11]};

    // Current velocity
    double currentVelocity[3] = {PC_Quadcopter_Simulation_Y.yout[13],
                                 PC_Quadcopter_Simulation_Y.yout[12],
                                 -PC_Quadcopter_Simulation_Y.yout[14]};

    double currentAttitude[3] = {PC_Quadcopter_Simulation_Y.yout[24],
                                 PC_Quadcopter_Simulation_Y.yout[25],0};
    rotorsimAppData.velocity[0] = currentVelocity[0];
    rotorsimAppData.velocity[1] = currentVelocity[1];
    rotorsimAppData.velocity[2] = currentVelocity[2];


    rotorsimAppData.heading = fmod(2*M_PI + atan2(rotorsimAppData.velocity[1],rotorsimAppData.velocity[0]),2*M_PI)*180/M_PI;
    // Do coordinate transformation from NED to LLA
    //double origin[3] = {rotorsimAppData.Rotorsim_Tbl.originLL[0],
    //                    rotorsimAppData.Rotorsim_Tbl.originLL[1],0};
    double origin[3] = {rotorsimAppData.flightPlan.waypoints[0].latitude,
                        rotorsimAppData.flightPlan.waypoints[0].longitude,0};
    ConvertEND2LLA(origin,currentPosition,rotorsimAppData.position);

    // Publish position messages
    position_t positionGPS;

    CFE_SB_InitMsg(&positionGPS,ICAROUS_POSITION_MID, sizeof(position_t),TRUE);

    positionGPS.aircraft_id = CFE_PSP_GetSpacecraftId();
    positionGPS.latitude = rotorsimAppData.position[0];
    positionGPS.longitude = rotorsimAppData.position[1];
    positionGPS.altitude_rel = rotorsimAppData.position[2];
    positionGPS.altitude_abs = rotorsimAppData.position[2];
    positionGPS.vn = rotorsimAppData.velocity[0];
    positionGPS.ve = rotorsimAppData.velocity[1];
    positionGPS.vd = rotorsimAppData.velocity[2];
    positionGPS.hdg = rotorsimAppData.heading; 

    CFE_TIME_SysTime_t timeNow = CFE_TIME_GetTAI();
    positionGPS.time_gps = timeNow.Seconds;


    double heading = fmod(2*M_PI + atan2(positionGPS.ve,positionGPS.vn),2*M_PI)*180/M_PI;
    double roll    = fmod(2*M_PI + currentAttitude[0],2*M_PI)*180/M_PI;
    double pitch    = fmod(2*M_PI + currentAttitude[1],2*M_PI)*180/M_PI;
    //OS_printf("position lat : %f\n",positionForBus.latitude);
    //OS_printf("position lon : %f\n",positionForBus.longitude);
    //OS_printf("position alt : %f\n",positionForBus.altitude_rel);

    SendSBMsg(positionGPS);

    attitude_t attitude;
    CFE_SB_InitMsg(&attitude,ICAROUS_ATTITUDE_MID, sizeof(attitude),TRUE);
    attitude.roll = roll;
    attitude.pitch = pitch;
    attitude.yaw = heading;
}