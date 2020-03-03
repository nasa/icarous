

#define _GNU_SOURCE
#include <string.h>
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include "Icarous.h"

#ifdef APPDEF_TRAFFIC
#include "traffic_msg.h"
#include "traffic_msgids.h"
#endif

#ifdef APPDEF_TRACKING
#include "tracking_msg.h"
#include "tracking_msgids.h"
#endif 

#ifdef APPDEF_TRAJECTORY
#include "trajectory_msg.h"
#include "trajectory_msgids.h"
#endif 

#ifdef APPDEF_GEOFENCE
#include "geofence_msg.h"
#include "geofence_msgids.h"
#endif

#ifdef APPDEF_ROTORSIM
#include "rotorsim_msg.h"
#include "rotorsim_msgids.h"
#endif

#ifdef APPDEF_MERGER
#include "merger_msg.h"
#include "merger_msgids.h"
#endif

#ifdef APPDEF_GUIDANCE
#include "guidance_msg.h"
#include "guidance_msgids.h"
#endif

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 Icarous_LibInit(void)
{
    OS_printf ("ICAROUS_LIB Initialized.\n");
    return CFE_SUCCESS;
}

/**
 *Function to publish parameters on the software bus
 */
void PublishParams(param_t *params) {

    //Store the locally saved parameters to the messages
    int i = 0;
    //Traffic Parameters

    #ifdef APPDEF_TRAFFIC
    traffic_parameters_t localTrafficParams;
    CFE_SB_InitMsg(&localTrafficParams,TRAFFIC_PARAMETERS_MID,sizeof(traffic_parameters_t),TRUE);
    localTrafficParams.trafficSource = (uint32_t) params[i].value; i++;
    localTrafficParams.resType = (uint32_t) params[i].value;i++;;
    localTrafficParams.logDAAdata = (bool) params[i].value;i++;;
    localTrafficParams.lookahead_time = params[i].value;i++;;
    localTrafficParams.left_trk = params[i].value;i++;;
    localTrafficParams.right_trk = params[i].value;i++;;
    localTrafficParams.min_gs = params[i].value;i++;;
    localTrafficParams.max_gs = params[i].value;i++;;
    localTrafficParams.min_vs = params[i].value;i++;;
    localTrafficParams.max_vs = params[i].value;i++;;
    localTrafficParams.min_alt = params[i].value;i++;;
    localTrafficParams.max_alt = params[i].value;i++;;
    localTrafficParams.trk_step = params[i].value;i++;;
    localTrafficParams.gs_step = params[i].value;i++;;
    localTrafficParams.vs_step = params[i].value;i++;;
    localTrafficParams.alt_step = params[i].value;i++;;
    localTrafficParams.horizontal_accel = params[i].value;i++;;
    localTrafficParams.vertical_accel = params[i].value;i++;;
    localTrafficParams.turn_rate = params[i].value;i++;;
    localTrafficParams.bank_angle = params[i].value;i++;;
    localTrafficParams.vertical_rate = params[i].value;i++;;
    localTrafficParams.recovery_stability_time = params[i].value;i++;;
    localTrafficParams.min_horizontal_recovery = params[i].value;i++;;
    localTrafficParams.min_vertical_recovery = params[i].value;i++;;
    localTrafficParams.recovery_trk = (bool) params[i].value;i++;;
    localTrafficParams.recovery_gs = (bool) params[i].value;i++;;
    localTrafficParams.recovery_vs = (bool) params[i].value;i++;;
    localTrafficParams.recovery_alt = (bool) params[i].value;i++;;
    localTrafficParams.ca_bands = (bool) params[i].value;i++;;
    localTrafficParams.ca_factor = params[i].value;i++;;
    localTrafficParams.horizontal_nmac = params[i].value;i++;;
    localTrafficParams.vertical_nmac = params[i].value;i++;;
    localTrafficParams.conflict_crit = (bool) params[i].value;i++;;
    localTrafficParams.recovery_crit = (bool) params[i].value;i++;;
    localTrafficParams.contour_thr = params[i].value;i++;;
    localTrafficParams.alert_1_alerting_time = params[i].value;i++;;
    strcpy(localTrafficParams.alert_1_detector, "det_1");   //Hard coded, not parameter
    localTrafficParams.alert_1_early_alerting_time = params[i].value;i++;;
    strcpy(localTrafficParams.alert_1_region, "NEAR");      //Hard coded, not parameter
    localTrafficParams.alert_1_spread_alt = params[i].value;i++;;
    localTrafficParams.alert_1_spread_gs = params[i].value;i++;;
    localTrafficParams.alert_1_spread_trk = params[i].value;i++;;
    localTrafficParams.alert_1_spread_vs = params[i].value;i++;;
    localTrafficParams.conflict_level = (uint8_t) params[i].value;i++;;
    strcpy(localTrafficParams.load_core_detection_det_1, "WCV_TAUMOD"); //Hard coded, not parameter
    localTrafficParams.det_1_WCV_DTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TCOA = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_ZTHR = params[i].value;i++;;

    SendSBMsg(localTrafficParams);
    #else
       for(int k=0;k<46;++k) i++;
    #endif

    // Tracking parameters
    #ifdef APPDEF_TRACKING
    tracking_parameters_t localTrackingParams;
    CFE_SB_InitMsg(&localTrackingParams,TRACKING_PARAMETERS_MID,sizeof(tracking_parameters_t),TRUE);
    localTrackingParams.command = (bool) params[i].value;i++;;
    localTrackingParams.trackingObjId = (int) params[i].value;i++;;
    localTrackingParams.pGainX = (double) params[i].value;i++;;
    localTrackingParams.pGainY = (double) params[i].value;i++;;
    localTrackingParams.pGainZ = (double) params[i].value;i++;;
    localTrackingParams.heading = (double) params[i].value;i++;;
    localTrackingParams.distH = (double) params[i].value;i++;;
    localTrackingParams.distV = (double) params[i].value;i++;;
    SendSBMsg(localTrackingParams);
    #else
       for(int k=0;k<8;++k) i++;
    #endif

    // Trajectory parameters
    #ifdef APPDEF_TRAJECTORY
    trajectory_parameters_t localTrajectoryParams;
    CFE_SB_InitMsg(&localTrajectoryParams,TRAJECTORY_PARAMETERS_MID,sizeof(trajectory_parameters_t),TRUE);
    localTrajectoryParams.obsbuffer = (double) params[i].value;i++;;
    localTrajectoryParams.maxCeiling = (double) params[i].value;i++;;
    localTrajectoryParams.astar_enable3D = (bool) params[i].value;i++;;
    localTrajectoryParams.astar_gridSize = (double) params[i].value;i++;;
    localTrajectoryParams.astar_resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.astar_lookahead = (double) params[i].value;i++;;
    strcpy(localTrajectoryParams.astar_daaConfigFile, "../ram/DaidalusQuadConfig.txt");   //Hard coded, not parameter
    //params[i].value;i++;;
    localTrajectoryParams.rrt_resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.rrt_numIterations = (int) params[i].value;i++;;
    localTrajectoryParams.rrt_dt = (double) params[i].value;i++;;
    localTrajectoryParams.rrt_macroSteps = (int) params[i].value;i++;;
    localTrajectoryParams.rrt_capR = (double) params[i].value;i++;;
    strcpy(localTrajectoryParams.rrt_daaConfigFile, "../ram/DaidalusQuadConfig.txt");     //Hard coded, not parameter
    //params[i].value;i++;;
    localTrajectoryParams.xtrkDev = (double) params[i].value;i++;;
    localTrajectoryParams.xtrkGain = (double) params[i].value;i++;;
    localTrajectoryParams.resSpeed = (double) params[i].value;i++;;
    localTrajectoryParams.searchAlgorithm = (uint8_t) params[i].value;i++;;

    SendSBMsg(localTrajectoryParams);
    #else
       for(int k=0;k<15;++k) i++;
    #endif

    // Geofence parameters
    #ifdef APPDEF_GEOFENCE
    geofence_parameters_t localGeofenceParams;
    CFE_SB_InitMsg(&localGeofenceParams,GEOFENCE_PARAMETERS_MID,sizeof(geofence_parameters_t),TRUE);
    localGeofenceParams.lookahead = (double) params[i].value;i++;;
    localGeofenceParams.hthreshold = (double) params[i].value;i++;;
    localGeofenceParams.vthreshold = (double) params[i].value;i++;;
    localGeofenceParams.hstepback = (double) params[i].value;i++;;
    localGeofenceParams.vstepback = (double) params[i].value;i++;;
    SendSBMsg(localGeofenceParams);
    #else
       for(int k=0;k<5;++k) i++;
    #endif

    #ifdef APPDEF_ROTORSIM
    // Rotorsim parameters
    rotorsim_parameters_t localRotorsimParams;
    CFE_SB_InitMsg(&localRotorsimParams,ROTORSIM_PARAMETERS_MID,sizeof(rotorsim_parameters_t),TRUE);
    localRotorsimParams.speed = (double) params[i].value;i++;;
    SendSBMsg(localRotorsimParams);
    #else
       for(int k=0;k<1;++k) i++;
    #endif

    #ifdef APPDEF_MERGER
    merger_parameters_t localMergerParams;
    CFE_SB_InitMsg(&localMergerParams,MERGER_PARAMETERS_MID,sizeof(merger_parameters_t),TRUE);
    localMergerParams.missionSpeed = appdataIntGS.storedparams[i].value;
    localMergerParams.maxVehicleSpeed = (double) params[i].value;i++;;
    localMergerParams.minVehicleSpeed = (double) params[i].value;i++;;
    localMergerParams.corridorWidth = (double) params[i].value;i++;;
    localMergerParams.entryRadius = (double) params[i].value;i++;;
    localMergerParams.coordZone = (double) params[i].value;i++;;
    localMergerParams.scheduleZone = (double) params[i].value;i++;;
    localMergerParams.minSeparationDistance = (double) params[i].value;i++;;
    localMergerParams.minSeparationTime = (double) params[i].value;i++;;
    localMergerParams.maxVehicleTurnRadius = (double) params[i].value;i++;;
    localMergerParams.startIntersection = (int) params[i].value;i++;;
    SendSBMsg(localMergerParams);
    #else
       for(int k=0;k<10;++k) i++;
    #endif

    #ifdef APPDEF_GUIDANCE
    guidance_parameters_t localGuidanceParams;
    CFE_SB_InitMsg(&localGuidanceParams,GUIDANCE_PARAMETERS_MID,sizeof(guidance_parameters_t),TRUE);
    localGuidanceParams.defaultWpSpeed = (double) params[i].value;i++;;
    localGuidanceParams.captureRadiusScaling = (double) params[i].value;i++;;
    localGuidanceParams.climbAngle = (double) params[i].value;i++;;
    localGuidanceParams.climbAngleVRange = (double) params[i].value;i++;;
    localGuidanceParams.climbAngleHRange = (double) params[i].value;i++;;
    localGuidanceParams.climbRateGain = (double) params[i].value;i++;;
    localGuidanceParams.maxClimbRate = (double) params[i].value;i++;;
    localGuidanceParams.minClimbRate = (double) params[i].value;i++;;
    SendSBMsg(localGuidanceParams);
    #endif

    OS_printf("Published parameters\n");
}

/**
 * Function to read a flightplan from a mavlink flightplan file
 */
void ReadFlightplanFromFile(char* filename,flightplan_t *fplan){
   
   FILE *fp = fopen(filename,"r");
  
   if (fp == NULL){
       printf("error reading flight plan file file\n");
       return;
   }

   char *buffer;
   size_t size = 250; 
   int n = 1;

   buffer = ((char*) malloc(size));

   int index = -1;
   int count = 0;

   flightplan_t* plan = fplan;
   while(n > 0){
       n = getline(&buffer,&size,fp);

       if(n < 20){
           continue;
       }

       count = 0;
       index++;

       if (buffer[0] == '#'){
           continue;
       }else{
           char* tok;
           tok = strtok(buffer," ,\t");
           int command = 16;
           while(tok != NULL){
               switch(count){
                    case 0:{
                            // index
                            //printf("index: %d\n",atoi(tok));
                            plan->num_waypoints++;
                            break;
                          }

                    case 1:{
                             // current wp
                             break;
                          }

                    case 2:{

                            // coord frame
                            break;
                           }

                    case 3:{
                            // command
                            command = atoi(tok);
                            //printf("command: %d\n",command);
                            break;
                           }

                    case 4:{
                            // param1
                            break;
                           }

                    case 5:{
                            // param2
                            break;
                           }

                    case 6:{
                            // param3
                            break;
                           }

                    case 7:{
                            // param4
                            int param4 = atoi(tok);
                            //printf("Param 4: %d\n",param4);
                            char wp_name[MAX_FIX_NAME_SIZE];
                            sprintf(wp_name,"%d",param4);
                            memcpy(plan->waypoints[index].name,wp_name,MAX_FIX_NAME_SIZE);
                            break;
                           }
                    
                    case 8:{
                            // x
                            double x = atof(tok);
                            //printf("x: %f\n",x);
                            plan->waypoints[index].latitude = x;
                            break;
                           }

                    case 9:{
                            // y
                            double y = atof(tok);
                            //printf("y: %f\n",y); 
                            plan->waypoints[index].longitude = y;
                            break;
                           }

                    case 10:{
                            // z
                            double z = atof(tok);
                            //printf("z: %f\n",z);
                            plan->waypoints[index].altitude = z;
                            break;
                            }

                    case 11:{
                            // auto continue
                            break;
                            }

                    default:{
                                break;
                            }

               }
               count++;
               tok = strtok(NULL," \t");
               if(command != 16){
                   index--;
                   break;
               }
           }
       }
    }
}

/**
 * Function to read in key value pairs from a file
 * Returns the number of pairs read from the file
 * KEY VALUE
 */
int GetParams(char *filename, char (*params)[16],char (*val)[16]){
   FILE* fp = fopen(filename,"r");
   if(fp == NULL){
       OS_printf("Error opening file: %s\n",filename);
       return -1;
   }

   int i = 0;
   while(1){
       int x;
       char line[200];
       if(fgets(line,199,fp) == NULL){
          break;
       }
       memset(params[i],0,16);
       memset(val[i],0,16);
       x =  sscanf(line,"%s %16s",params[i],val[i]);
       if(x > 0){
            if (params[i][0] !='#'){
                i++;
            }
       }
   }
   fclose(fp);
   return i;
}

bool InitializeParams(char *filename,param_t* params,uint16_t paramCount){

    char locparams[250][16];
    char locvals[250][16];
    int n = GetParams(filename,locparams,locvals);
    if (n != paramCount){
        OS_printf("Parameter count incorrect in %s",filename);
        return false;
    }else{
        for(int i=0;i<PARAM_COUNT;++i){
            memcpy(params[i].param_id,locparams+i,16);
            params[i].value = atof(locvals[i]);
            params[i].type = 10;
        }
        return true;
    }

}

bool InitializeAircraftCallSign(char* callsign){
    char configfile[25];
    uint32_t aircraft_id = CFE_PSP_GetSpacecraftId();
    sprintf(configfile,"../ram/aircraft%d.cfg",aircraft_id);
    char configParams[5][MAX_CALLSIGN_LEN];
    char configValues[5][MAX_CALLSIGN_LEN];

    GetParams(configfile,configParams,configValues);
    
    if(strcmp(configParams[0],"CALLSIGN") == 0){
        strcpy(callsign,configValues[0]);
    }else{
        return false;
    }

    return true;
}

bool InitializePortConfig(char* filename,port_t* prt){

    char configfile[25];
    uint32_t aircraft_id = CFE_PSP_GetSpacecraftId();
    sprintf(configfile,"../ram/%s%d.cfg",filename,aircraft_id);

    char configParams[20][16];
    char configValues[20][16];
    int n = GetParams(configfile,configParams,configValues);

    if(n == 5){
        if(strcmp(configValues[0],"SERIAL") == 0){
            prt->portType = SERIAL;
        }else{
            prt->portType = SOCKET;
        }

        strcpy(prt->target,configValues[1]);
        prt->baudrate = atoi(configValues[2]);
        prt->portin = atoi(configValues[3]);
        prt->portout = atoi(configValues[4]);
    }else{
        OS_printf("Insufficient port parameters\n");
        return false;
    }

    return true;
}