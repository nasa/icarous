//
// Created by Swee Balachandran on 12/22/17.
//
#include <msgdef/ardupilot_msg.h>
#include <msgids/ardupilot_msgids.h>
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


    CFE_SB_Subscribe(ICAROUS_COMMANDS_MID, rotorsimAppData.Rotorsim_Pipe);
    CFE_SB_Subscribe(ICAROUS_STARTMISSION_MID,rotorsimAppData.Rotorsim_Pipe);
    CFE_SB_Subscribe(ICAROUS_FLIGHTPLAN_MID,rotorsimAppData.Rotorsim_Pipe);

    // Register table with table services
    status = CFE_TBL_Register(&rotorsimAppData.Rotorsim_tblHandle,
                              "RotorsimTable",
                              sizeof(RotorsimTable_t),
                              CFE_TBL_OPT_DEFAULT,
                              &RotorsimTableValidationFunc);

    // Load app table data
    status = CFE_TBL_Load(rotorsimAppData.Rotorsim_tblHandle, CFE_TBL_SRC_ADDRESS,&RotorsimTblStruct);


    RotorsimTable_t *TblPtr;
    status = CFE_TBL_GetAddress((void**)&TblPtr, rotorsimAppData.Rotorsim_tblHandle);
    memcpy(&rotorsimAppData.Rotorsim_Tbl,TblPtr,sizeof(RotorsimTable_t));

    // Send event indicating app initialization
    CFE_EVS_SendEvent(ROTORSIM_STARTUP_INF_EID, CFE_EVS_INFORMATION,
                      "Rotorsim App Initialized. Version %d.%d",
                      ROTORSIM_MAJOR_VERSION,
                      ROTORSIM_MINOR_VERSION);

    rotorsimAppData.offsetdist = 3;
    rotorsimAppData.passive = true;
    rotorsimAppData.startMission = false;
    rotorsimAppData.nextWP = 0;
    rotorsimAppData.time = 0;
    rotorsimAppData.flightplanSent = false;
    rotorsimAppData.externalCmdV[0] = 0;
    rotorsimAppData.externalCmdV[1] = 0;
    rotorsimAppData.externalCmdV[2] = 0;

    rotorsimAppData.flightPlan.num_waypoints = rotorsimAppData.Rotorsim_Tbl.numWP;
    for(int i=0;i<rotorsimAppData.flightPlan.num_waypoints;++i){
       rotorsimAppData.flightPlan.waypoints[i].latitude = rotorsimAppData.Rotorsim_Tbl.flightPlan[i][0];
       rotorsimAppData.flightPlan.waypoints[i].longitude = rotorsimAppData.Rotorsim_Tbl.flightPlan[i][1];
       rotorsimAppData.flightPlan.waypoints[i].altitude = rotorsimAppData.Rotorsim_Tbl.flightPlan[i][2];

    }

    rotorsimAppData.position[0] = rotorsimAppData.Rotorsim_Tbl.originLL[0];
    rotorsimAppData.position[1] = rotorsimAppData.Rotorsim_Tbl.originLL[1];
    rotorsimAppData.position[2] = rotorsimAppData.Rotorsim_Tbl.initialAlt;

    PC_Quadcopter_Simulation_initialize();
    memset(PC_Quadcopter_Simulation_Y.yout,0,sizeof(real_T)*30);

    uint32_t clockAccuracy = 1000;

    status = OS_TimerCreate(&rotorsimAppData.timerId,"SIMSTEP",&clockAccuracy,timer_callback);
    if(status != CFE_SUCCESS){
       OS_printf("Could not create SIMSTEP timer\n");
    }

    status = OS_TimerSet(rotorsimAppData.timerId,10000000,50000);
    if(status != CFE_SUCCESS){
        OS_printf("Could not set SIMSTEP timer\n");
    }

    // publish flight plan on cFS bus
    flightplan_t fplan;
    CFE_SB_InitMsg(&fplan,ICAROUS_FLIGHTPLAN_MID, sizeof(flightplan_t),TRUE);
    fplan.num_waypoints = rotorsimAppData.flightPlan.num_waypoints;
    memcpy(fplan.waypoints,rotorsimAppData.flightPlan.waypoints,fplan.num_waypoints*sizeof(waypoint_t));
}

void Rotorsim_AppCleanUp(){
    // Do clean up here
}

void Rotorsim_ProcessPacket(){

    CFE_SB_MsgId_t  MsgId;
    MsgId = CFE_SB_GetMsgId(rotorsimAppData.Rotorsim_MsgPtr);

    switch(MsgId){
        case ICAROUS_COMMANDS_MID:{
           argsCmd_t *msg = (argsCmd_t*)rotorsimAppData.Rotorsim_MsgPtr;

           switch(msg->name){

               case _SETPOS_:{
                   // set position control to true
                   rotorsimAppData.positionControl = true;
                   // store commanded position
                   rotorsimAppData.externalCmdP[0] = msg->param1;
                   rotorsimAppData.externalCmdP[1] = msg->param2;
                   rotorsimAppData.externalCmdP[2] = msg->param3;
                   break;
               }

               case _SETVEL_:{
                   // set velocity control to true
                   rotorsimAppData.positionControl = false;

                   // store commanded velocity
                   rotorsimAppData.externalCmdV[0] = msg->param1;
                   rotorsimAppData.externalCmdV[1] = msg->param2;
                   rotorsimAppData.externalCmdV[2] = msg->param3;
                   break;
               }

               case _SETMODE_:{
                   if((int) msg->param1  == _ACTIVE_){
                      rotorsimAppData.passive = false;
                   }else if( (int) msg->param1 == _PASSIVE_){
                      rotorsimAppData.passive = true;
                   }
                   break;
               }

               case _SETSPEED_:{
                  rotorsimAppData.Rotorsim_Tbl.missionSpeed = msg->param1;
                  break;
               }

               case _GOTOWP_:{
                  rotorsimAppData.nextWP = (int) msg->param1;
                  missionItemReached_t wpReached;
                  CFE_SB_InitMsg(&wpReached,ICAROUS_WPREACHED_MID, sizeof(wpReached),TRUE);
                  wpReached.reachedwaypoint = (int)msg->param1 - 1;
                  wpReached.feedback = false;
                  SendSBMsg(wpReached);
               }
           }
           break;
        }

        case ICAROUS_STARTMISSION_MID:{
            rotorsimAppData.startMission = true;

            cmdAck_t ack1,ack2;
            CFE_SB_InitMsg(&ack1,ICAROUS_COMACK_MID,sizeof(ack1),TRUE);
            CFE_SB_InitMsg(&ack2,ICAROUS_COMACK_MID,sizeof(ack2),TRUE);

            ack1.name = _ARM_;
            ack1.result = 0;
            ack2.name = _TAKEOFF_;
            ack2.result = 0;

            SendSBMsg(ack1);
            SendSBMsg(ack2);

            break;
        }

        case ICAROUS_FLIGHTPLAN_MID:{
            flightplan_t* fp;
            fp = (flightplan_t*) rotorsimAppData.Rotorsim_MsgPtr;
            memcpy(&rotorsimAppData.flightPlan,fp,sizeof(flightplan_t));
            rotorsimAppData.nextWP=1;
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

    // publish flight plan on cFS bus
  /*  if(!rotorsimAppData.flightplanSent) {
        rotorsimAppData.flightplanSent = true;
        flightplan_t fplan;
        CFE_SB_InitMsg(&fplan, ICAROUS_FLIGHTPLAN_MID, sizeof(flightplan_t), TRUE);
        fplan.num_waypoints = rotorsimAppData.flightPlan.num_waypoints;
        memcpy(fplan.waypoints, rotorsimAppData.flightPlan.waypoints, fplan.num_waypoints * sizeof(waypoint_t));
        SendSBMsg(fplan);
    } */

    /* Check for overrun */
    if (OverrunFlag) {
        rtmSetErrorStatus(PC_Quadcopter_Simulation_M, "Overrun");
        return;
    }

    OverrunFlag = true;

    /* Save FPU context here (if necessary) */
    /* Re-enable timer or interrupt here */
    /* Set model inputs here */

    Rotorsim_GetInputs();

    // Set inputs
    PC_Quadcopter_Simulation_U.Xd_I = rotorsimAppData.externalCmdV[1];
    PC_Quadcopter_Simulation_U.Yd_I = rotorsimAppData.externalCmdV[0];
    PC_Quadcopter_Simulation_U.Zd_I = rotorsimAppData.externalCmdV[2];

    /* Step the model */
    if(rotorsimAppData.nextWP < rotorsimAppData.flightPlan.num_waypoints) {
        PC_Quadcopter_Simulation_step();
    }

    /* Get model outputs here */
    Rotorsim_GetOutputs();

    /* Indicate task complete */
    OverrunFlag = false;

    /* Disable interrupts here */
    /* Restore FPU context here (if necessary) */
    /* Enable interrupts here */
}

void Rotorsim_GetInputs(){
   // if mission has begun
   if(rotorsimAppData.startMission) {
       // if passive
       if(rotorsimAppData.passive) {
           // Default to position control
           if(rotorsimAppData.nextWP < rotorsimAppData.flightPlan.num_waypoints) {
               int nextWP = rotorsimAppData.nextWP;
               double nextWaypoint[3] = {rotorsimAppData.flightPlan.waypoints[nextWP].latitude,
                                         rotorsimAppData.flightPlan.waypoints[nextWP].longitude,
                                         rotorsimAppData.flightPlan.waypoints[nextWP].altitude};
               double dist = ComputeDistance(rotorsimAppData.position, nextWaypoint);
               double speed = sqrt(pow(rotorsimAppData.velocity[0], 2) +
                                   pow(rotorsimAppData.velocity[1], 2) +
                                   pow(rotorsimAppData.velocity[2], 2));

               double wpA[3] = {rotorsimAppData.flightPlan.waypoints[nextWP - 1].latitude,
                                rotorsimAppData.flightPlan.waypoints[nextWP - 1].longitude,
                                rotorsimAppData.flightPlan.waypoints[nextWP - 1].altitude};

               double wpB[3] = {rotorsimAppData.flightPlan.waypoints[nextWP].latitude,
                                rotorsimAppData.flightPlan.waypoints[nextWP].longitude,
                                rotorsimAppData.flightPlan.waypoints[nextWP].altitude};

               double distbtWP   = ComputeDistance(wpA,wpB);

               if(rotorsimAppData.flightPlan.waypoints[nextWP-1].wp_metric == WP_METRIC_ETA) {
                   if(rotorsimAppData.flightPlan.waypoints[nextWP-1].value_to_next_wp > 0) {
                       double setSpeed = distbtWP / rotorsimAppData.flightPlan.waypoints[nextWP - 1].value_to_next_wp;
                       rotorsimAppData.Rotorsim_Tbl.missionSpeed = setSpeed;
                       //OS_printf("Speed = %f\n", setSpeed);
                   }
               }
               double newPositionToTrack[3];
               double heading = rotorsimAppData.heading;
               ComputeOffSetPositionOnPlan(rotorsimAppData.position,nextWP-1,newPositionToTrack);
               // If distance to next waypoint is < captureRadius, switch to next waypoint
               if (dist <= rotorsimAppData.Rotorsim_Tbl.missionSpeed*2) {

                   missionItemReached_t wpReached;
                   CFE_SB_InitMsg(&wpReached,ICAROUS_WPREACHED_MID, sizeof(wpReached),TRUE);
                   wpReached.reachedwaypoint = rotorsimAppData.nextWP;
                   wpReached.feedback = true;
                   SendSBMsg(wpReached);
                   rotorsimAppData.nextWP++;

               } else {
                   // Compute velocity command to next waypoint
                   double heading = ComputeHeading(rotorsimAppData.position, newPositionToTrack);
                   double speed = rotorsimAppData.Rotorsim_Tbl.missionSpeed;
                   double climbrate = (rotorsimAppData.position[2] - nextWaypoint[2]) * -0.5;

                   if (climbrate > 10) {
                       climbrate = 10;
                   } else if (climbrate < -10) {
                       climbrate = -10;
                   }
                   double vn, ve, vd;
                   ConvertTrkGsVsToVned(heading, speed, climbrate, &vn, &ve, &vd);


                   // Store velocity command in relevant structure
                   rotorsimAppData.externalCmdV[0] = 0.7 * rotorsimAppData.externalCmdV[0] + 0.3 * vn;
                   rotorsimAppData.externalCmdV[1] = 0.7 * rotorsimAppData.externalCmdV[1] + 0.3 * ve;
                   rotorsimAppData.externalCmdV[2] = vd;
               }
           }
       }else {
           // else if active
           // if position control
           if(rotorsimAppData.positionControl) {
               double heading = ComputeHeading(rotorsimAppData.position,rotorsimAppData.externalCmdP);
               double speed = rotorsimAppData.Rotorsim_Tbl.missionSpeed;
               double climbrate = (rotorsimAppData.position[2] - rotorsimAppData.externalCmdP[2]) * -0.5;


               double dist = ComputeDistance(rotorsimAppData.position, rotorsimAppData.externalCmdP);
               double currentspeed = sqrt(pow(rotorsimAppData.velocity[0], 2) +
                                     pow(rotorsimAppData.velocity[1], 2) +
                                     pow(rotorsimAppData.velocity[2], 2));

               bool reached = false;
               // If distance to next waypoint is < captureRadius, switch to next waypoint
               if (dist <= currentspeed* 2.5) {
                   reached = true;
               }


               if(climbrate > 10){
                  climbrate = 10;
               }else if(climbrate < -10){
                  climbrate = -10;
               }
               double vn,ve,vd;
               ConvertTrkGsVsToVned(heading,speed,climbrate,&vn,&ve,&vd);

               // Store velocity command in relevant structure

               if(!reached) {
                   rotorsimAppData.externalCmdV[0] = 0.7 * rotorsimAppData.externalCmdV[0] + 0.3 * vn;
                   rotorsimAppData.externalCmdV[1] = 0.7 * rotorsimAppData.externalCmdV[1] + 0.3 * ve;
                   rotorsimAppData.externalCmdV[2] = vd;
               }else{
                   rotorsimAppData.externalCmdV[0] = 0;
                   rotorsimAppData.externalCmdV[1] = 0;
                   rotorsimAppData.externalCmdV[2] = 0;
               }
           }
       }
   }

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
                                 PC_Quadcopter_Simulation_Y.yout[14]};

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
    positionGPS.vn = rotorsimAppData.velocity[0];
    positionGPS.ve = rotorsimAppData.velocity[1];
    positionGPS.vd = rotorsimAppData.velocity[2];
    positionGPS.hdg = rotorsimAppData.heading; 
    positionGPS.time_gps = rotorsimAppData.time;

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

bool ComputeOffSetPositionOnPlan(double position[],int currentLeg,double outputLLA[]){
    //Starting waypoint
    double wpA[3] = {rotorsimAppData.flightPlan.waypoints[currentLeg].latitude,
                     rotorsimAppData.flightPlan.waypoints[currentLeg].longitude,
                     rotorsimAppData.flightPlan.waypoints[currentLeg].altitude};

    //Ending waypoint
    double wpB[3] = {rotorsimAppData.flightPlan.waypoints[currentLeg+1].latitude,
                     rotorsimAppData.flightPlan.waypoints[currentLeg+1].longitude,
                     rotorsimAppData.flightPlan.waypoints[currentLeg+1].altitude};

    double wpC[3];
    bool finalleg = false;
    if(currentLeg+2 < rotorsimAppData.flightPlan.num_waypoints){
        wpC[0] = rotorsimAppData.flightPlan.waypoints[currentLeg+2].latitude;
        wpC[1] = rotorsimAppData.flightPlan.waypoints[currentLeg+2].longitude;
        wpC[2] = rotorsimAppData.flightPlan.waypoints[currentLeg+2].altitude;
    }else{
        finalleg = true;
    }

    double distAB = ComputeDistance(wpA,wpB);
    double heading = ComputeHeading(wpA,wpB);
    double distAP = ComputeDistance(wpA,position);

    //Convert to local frame
    double _wpA[3],_wpB[3],_wpC[3];
    ConvertLLA2END(position,wpA,_wpA);
    ConvertLLA2END(position,wpB,_wpB);
    ConvertLLA2END(position,wpC,_wpC);
    
    double r = rotorsimAppData.Rotorsim_Tbl.missionSpeed*2;
    double outputEND[3] = {0.0,0.0,0.0};
    // If there is enough room on the current leg, get the point to track on this leg
    // else, use the other leg.
    if(distAB - distAP > r){
       GetCorrectIntersectionPoint(_wpA,_wpB,heading,r,outputEND); 
    }else{
       if(finalleg){
           memcpy(outputLLA,wpB,sizeof(double)*3);
           return false;
       } 
       GetCorrectIntersectionPoint(_wpB,_wpC,heading,r,outputEND); 
    }
    ConvertEND2LLA(position,outputEND,outputLLA);
    return true;
}


void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double heading,double r,double output[]){
    double x1,x2,y1,y2;    
    if( fabs(_wpB[0] - _wpA[0]) > 1e-2 ){
        double m = (_wpB[1] - _wpA[1])/(_wpB[0] - _wpA[0]);
        double c = _wpA[1] - _wpA[0]*m;
        double aq = 1 + pow(m,2);
        double bq = 2*m*c;
        double cq = pow(c,2) - pow(r,2);

        double discr = pow(bq,2) - 4*aq*cq;
        double discr_sqrt = sqrt(discr);


        if(discr < 0){
           //OS_printf("complex roots\n"); 
           output[0] = _wpB[0]; 
           output[1] = _wpB[1];
           return;
        }

        x1 = (-bq - discr_sqrt)/(2*aq);
        x2 = (-bq + discr_sqrt)/(2*aq);
        y1 = m*x1 + c; 
        y2 = m*x2 + c;
    }else{
        x1 = _wpA[0];
        x2 = x1;
        y1 = sqrt(pow(r,2) - pow(x1,2)); 
        y2 = -sqrt(pow(r,2) - pow(x1,2));  
    }

    // Check which point has the smallest bearing

    double heading1 = fmod(2*M_PI + atan2(y1,x1) ,2*M_PI) *180/M_PI;
    double heading2 = fmod(2*M_PI + atan2(y2,x2) ,2*M_PI) *180/M_PI;

    heading1 = fmod(360 + 90 - heading1,360);
    heading2 = fmod(360 + 90 - heading2,360);

    double bearing1 = fabs(heading - heading1);
    double bearing2 = fabs(heading - heading2);

    if(bearing1 > 180){
        bearing1 = 180 - fabs(180 - bearing1);
    }

    if(bearing2 > 180){
        bearing2 = 180 - fabs(180 - bearing2);
    }

    if(bearing1 < bearing2){
        // Use (x1,y1)
        output[0] = x1;
        output[1] = y1;
    }else{
        // Use (x2,y2)
        output[0] = x2;
        output[1] = y2;
    }
}

double distance(double x1,double y1,double x2,double y2){
    return sqrt(pow(x2-x1,2) +pow(y2-y1,2));
}