#define _GNU_SOURCE
#include <string.h>
#include "cfe.h"
#include "cfe_error.h"
#include "cfe_evs.h"
#include "cfe_sb.h"
#include "cfe_es.h"
#include "cfe_platform_cfg.h"
#include "Icarous.h"

#ifdef APPDEF_PORT_LIB
#include "port_lib.h"
#endif

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

struct callsign {
    callsign_t value;
    bool isInitialized;
} callsignCache;

int32 Icarous_LibInit(void)
{
    OS_printf ("ICAROUS_LIB Initialized with callsign %s.\n", GetAircraftCallSign());
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
    strcpy(localTrafficParams.load_core_detection_det_1, "WCV_TAUMOD"); //Hard coded, not parameter
    localTrafficParams.det_1_WCV_DTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TCOA = params[i].value;i++;;
    localTrafficParams.det_1_WCV_TTHR = params[i].value;i++;;
    localTrafficParams.det_1_WCV_ZTHR = params[i].value;i++;;

    SendSBMsg(localTrafficParams);
    #else
       for(int k=0;k<45;++k) i++;
    #endif

    // Trajectory parameters
    #ifdef APPDEF_TRAJECTORY
    trajectory_parameters_t localTrajectoryParams;
    CFE_SB_InitMsg(&localTrajectoryParams,TRAJECTORY_PARAMETERS_MID,sizeof(trajectory_parameters_t),TRUE);
    localTrajectoryParams.dbparams.vertexBuffer = (double) params[i].value;i++;;
    localTrajectoryParams.dbparams.maxH = (double) params[i].value;i++;;
    localTrajectoryParams.dbparams.climbgs =  (double) params[i].value;i++;;
    localTrajectoryParams.dbparams.zSections = (double) params[i].value;i++;;
    localTrajectoryParams.dbparams.wellClearDistH = localTrafficParams.det_1_WCV_DTHR * 0.3; 
    localTrajectoryParams.dbparams.wellClearDistV = localTrafficParams.det_1_WCV_ZTHR * 0.3; 
    localTrajectoryParams.dbparams.turnRate = localTrafficParams.turn_rate;
    localTrajectoryParams.dbparams.maxGS = localTrafficParams.max_gs * 0.5;
    localTrajectoryParams.dbparams.minGS = localTrafficParams.min_gs * 0.5;
    localTrajectoryParams.dbparams.maxVS = localTrafficParams.max_vs * 0.00508;
    localTrajectoryParams.dbparams.minVS = localTrafficParams.min_vs * 0.00508;
    localTrajectoryParams.dbparams.hAccel = localTrafficParams.horizontal_accel;
    localTrajectoryParams.dbparams.hDaccel = -localTrafficParams.horizontal_accel*0.8;
    localTrajectoryParams.dbparams.vAccel = localTrafficParams.vertical_accel;
    localTrajectoryParams.dbparams.vDaccel = -localTrafficParams.vertical_accel;
    localTrajectoryParams.crossTrackDeviation = (double)params[i].value;i++;;

    SendSBMsg(localTrajectoryParams);
    #else
       for(int k=0;k<5;++k) i++;
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

    #ifdef APPDEF_MERGER
    merger_parameters_t localMergerParams;
    CFE_SB_InitMsg(&localMergerParams,MERGER_PARAMETERS_MID,sizeof(merger_parameters_t),TRUE);
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
    localGuidanceParams.guidanceRadiusScaling = (double) params[i].value;i++;;
    localGuidanceParams.xtrkDev = localTrajectoryParams.crossTrackDeviation;
    localGuidanceParams.climbAngle = (double) params[i].value;i++;;
    localGuidanceParams.climbAngleVRange = (double) params[i].value;i++;;
    localGuidanceParams.climbAngleHRange = (double) params[i].value;i++;;
    localGuidanceParams.climbRateGain = (double) params[i].value;i++;;
    localGuidanceParams.minClimbRate = (double) params[8].value * 0.00508;
    localGuidanceParams.maxClimbRate = (double) params[9].value * 0.00508;
    localGuidanceParams.maxCap = (double) params[i].value;i++;;
    localGuidanceParams.minCap = (double) params[i].value;i++;;
    localGuidanceParams.minSpeed = (double) params[6].value * 0.5;
    localGuidanceParams.maxSpeed = (double) params[7].value * 0.5;
    localGuidanceParams.yawForward = (bool) params[i].value;i++;;
    SendSBMsg(localGuidanceParams);
    #endif

}

/**
 * Function to read a flightplan from a mavlink flightplan file
 */
int ReadFlightplanFromFile(char* filename,flightplan_t *fplan){
   
   FILE *fp = fopen(filename,"r");
  
   if (fp == NULL){
       printf("Could not open file %s\n",filename);
       return -1;
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
    return 0;
}

/**
 * Function to read in key value pairs from a file
 * Returns the number of pairs read from the file
 * KEY VALUE
 */
int GetParams(char *filename, size_t numOfPairs, char (*params)[MAX_PARAMETER_NAME_SIZE],char (*val)[MAX_PARAMETER_NAME_SIZE]){
   FILE* fp = fopen(filename,"r");
   if(fp == NULL){
       OS_printf("Error opening file: %s\n",filename);
       return -1;
   }

   int i = 0;
   int lineNo = 0;
   while(1){
       int x;
       char line[200];
       if(fgets(line,sizeof(line),fp) == NULL){
          break;
       }
       memset(params[i],0,MAX_PARAMETER_NAME_SIZE);
       memset(val[i],0,MAX_PARAMETER_NAME_SIZE);
       x =  sscanf(line,"%16s %16s\n",params[i],val[i]);
       lineNo++;
       if(x == 2){
            if (params[i][0] !='#'){
                i++;
            }
       } else if (x == 1 && line[0] != '#') {
          OS_printf("[Icarouslib] Malformed configuration file %s at line %d\n", filename, lineNo);
       }
   }
   fclose(fp);
   return i;
}

bool InitializeParams(char *filename,param_t* params,uint16_t paramCount){
    const size_t MAX_PARAMS = 250;
    char locparams[MAX_PARAMS][MAX_PARAMETER_NAME_SIZE];
    char locvals[MAX_PARAMS][MAX_PARAMETER_NAME_SIZE];
    int n = GetParams(filename,MAX_PARAMS,locparams,locvals);
    if (n != paramCount){
        OS_printf("Parameter count incorrect in %s\n",filename);
        return false;
    }else{
        for(int i=0;i<PARAM_COUNT;++i){
            memcpy(params[i].param_id,locparams+i,MAX_PARAMETER_NAME_SIZE);
            params[i].value = atof(locvals[i]);
            params[i].type = 10;
        }
        return true;
    }

}

char const * GetAircraftCallSign(void) {
    if (!callsignCache.isInitialized) {
        InitializeAircraftCallSign(&callsignCache.value);
        callsignCache.isInitialized = true;
    }
    return callsignCache.value.value;
}

void adsb_callsign_from_callsign_t(adsb_callsign * dest, callsign_t * orig) {
    const size_t adsbCallsignMaxSize = 9;
    size_t length = strlen(orig->value);
    if (length < adsbCallsignMaxSize) {
        size_t paddingSize = adsbCallsignMaxSize - (length + 1);
        memcpy(dest->value, orig->value, length);
        memset(dest->value + length, ' ', paddingSize);
        dest->value[adsbCallsignMaxSize - 1] = '\0';
    } else {
        memcpy(dest->value, orig->value, adsbCallsignMaxSize);
        dest->value[adsbCallsignMaxSize - 1] = '\0';
        OS_printf("[Icarouslib] adsb_callsign_from_callsign_t trimmed the callsign %s -> %s\n", orig->value, dest->value);
    }
}

char const * callsign_t_get(callsign_t * dest) {
    return dest->value;
}

void callsign_t_zero(callsign_t * dest)
{
    memset(dest->value,0,sizeof(dest->value));
}

void callsign_t_set(callsign_t * dest, char const * const source)
{
    size_t sourceLength = strlen(source);
    if (sourceLength < MAX_CALLSIGN_LEN) {
        memset(dest->value,0,MAX_CALLSIGN_LEN);
        strncpy(dest->value,source,MAX_CALLSIGN_LEN);
    } else {
        strncpy(dest->value,source,MAX_CALLSIGN_LEN);
        dest->value[MAX_CALLSIGN_LEN - 1] = '\0';
    }
}

bool InitializeAircraftCallSign(callsign_t * callsign) {
    const size_t MAX_CALLSIGNS = 1;
    char configfile[25];
    uint32_t aircraft_id = CFE_PSP_GetSpacecraftId();
    sprintf(configfile,"../ram/aircraft%d.cfg",aircraft_id);
    char configParams[MAX_CALLSIGNS][MAX_CALLSIGN_LEN];
    char configValues[MAX_CALLSIGNS][MAX_CALLSIGN_LEN];

    GetParams(configfile,MAX_CALLSIGNS,configParams,configValues);
    
    if(strcmp(configParams[0],"CALLSIGN") == 0){
        strcpy(callsign->value,configValues[0]);
    }else{
        return false;
    }

    return true;
}

bool InitializePortConfig(char* filename,void* vprt){
    port_t * prt = (port_t*) vprt;
    char configfile[25];
    uint32_t aircraft_id = CFE_PSP_GetSpacecraftId();
    sprintf(configfile,"../ram/%s%d.cfg",filename,aircraft_id);

    const size_t MAX_PARAMS = 20;
    char configParams[MAX_PARAMS][MAX_PARAMETER_NAME_SIZE];
    char configValues[MAX_PARAMS][MAX_PARAMETER_NAME_SIZE];
    int n = GetParams(configfile,MAX_PARAMS,configParams,configValues);

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

#ifdef APPDEF_TRAFFIC
void ConstructDAAParamString(void* vdparam,char* params){
    traffic_parameters_t* tfParam = (traffic_parameters_t*)vdparam;
    int n = 0;
    n += sprintf(params,"lookahead_time=%f [s];",tfParam->lookahead_time);
    n += sprintf(params + n,"left_hdir=%f [deg];",tfParam->left_trk);
    n += sprintf(params + n,"right_hdir=%f [deg];",tfParam->right_trk);
    n += sprintf(params + n,"min_hs=%f [knot];",tfParam->min_gs);
    n += sprintf(params + n,"max_hs=%f [knot];",tfParam->max_gs);
    n += sprintf(params + n,"min_vs=%f [fpm];",tfParam->min_vs);
    n += sprintf(params + n,"max_vs=%f [fpm];",tfParam->max_vs);
    n += sprintf(params + n,"min_alt=%f [ft];",tfParam->min_alt);
    n += sprintf(params + n,"max_alt=%f [ft];",tfParam->max_alt);
    n += sprintf(params + n,"step_hdir=%f [deg];",tfParam->trk_step);
    n += sprintf(params + n,"step_hs=%f [knot];",tfParam->gs_step);
    n += sprintf(params + n,"step_vs=%f [fpm];",tfParam->vs_step);
    n += sprintf(params + n,"step_alt=%f [ft];",tfParam->alt_step);
    n += sprintf(params + n,"horizontal_accel=%f [m/s^2];",tfParam->horizontal_accel);
    n += sprintf(params + n,"vertical_accel=%f [m/s^2];",tfParam->vertical_accel);
    n += sprintf(params + n,"turn_rate=%f [deg/s];",tfParam->turn_rate);
    n += sprintf(params + n,"bank_angle=%f [deg];",tfParam->bank_angle);
    n += sprintf(params + n,"vertical_rate=%f [fpm];",tfParam->vertical_rate);
    n += sprintf(params + n,"recovery_stability_time=%f [s];",tfParam->recovery_stability_time);
    n += sprintf(params + n,"min_horizontal_recovery=%f [ft];",tfParam->min_horizontal_recovery);
    n += sprintf(params + n,"min_vertical_recovery=%f [ft];",tfParam->min_vertical_recovery);
    n += tfParam->recovery_trk? sprintf(params + n,"recovery_hdir=true;"):sprintf(params + n,"recovery_dir=false;");
    n += tfParam->recovery_gs? sprintf(params + n,"recovery_hs=true;"):sprintf(params + n,"recovery_hs=false;");
    n += tfParam->recovery_vs? sprintf(params + n,"recovery_vs=true;"):sprintf(params + n,"recovery_vs=false;");
    n += tfParam->recovery_alt? sprintf(params + n,"recovery_alt=true;"):sprintf(params + n,"recovery_alt=false;");
    n += tfParam->conflict_crit? sprintf(params + n,"conflict_crit=true;"):sprintf(params + n,"conflict_crit=false;");
    n += tfParam->recovery_crit? sprintf(params + n,"recovery_crit=true;"):sprintf(params + n,"recovery_crit=false;");
    n += tfParam->ca_bands? sprintf(params + n,"ca_bands=true;"):sprintf(params + n,"ca_bands=false;");
    n += sprintf(params + n,"ca_factor=%f;",tfParam->ca_factor);
    n += sprintf(params + n,"horizontal_nmac=%f [ft];",tfParam->horizontal_nmac);
    n += sprintf(params + n,"vertical_nmac=%f [ft];",tfParam->vertical_nmac);
    n += sprintf(params + n,"contour_thr=%f [deg];",tfParam->contour_thr);
    n += sprintf(params + n,"alerters=default;");
    n += sprintf(params + n,"default_alert_1_detector=%s;",tfParam->alert_1_detector);
    n += sprintf(params + n,"default_alert_1_region=%s ;",tfParam->alert_1_region); 
    n += sprintf(params + n,"default_alert_1_alerting_time=%f [s];",tfParam->alert_1_alerting_time);
    n += sprintf(params + n,"default_alert_1_early_alerting_time=%f [s];",tfParam->alert_1_early_alerting_time);
    n += sprintf(params + n,"default_alert_1_spread_hdir=%f [deg];",tfParam->alert_1_spread_trk);
    n += sprintf(params + n,"default_alert_1_spread_hs=%f [knot];",tfParam->alert_1_spread_gs);
    n += sprintf(params + n,"default_alert_1_spread_vs=%f [fpm];",tfParam->alert_1_spread_vs);
    n += sprintf(params + n,"default_alert_1_spread_alt=%f [ft];",tfParam->alert_1_spread_alt);
    n += sprintf(params + n,"default_det_1_WCV_DTHR = %f [ft];",tfParam->det_1_WCV_DTHR);
    n += sprintf(params + n,"default_det_1_WCV_TCOA = %f [s];",tfParam->det_1_WCV_TCOA);
    n += sprintf(params + n,"default_det_1_WCV_TTHR = %f [s];",tfParam->det_1_WCV_TTHR);
    n += sprintf(params + n,"default_det_1_WCV_ZTHR = %f [ft];",tfParam->det_1_WCV_ZTHR);
    n += sprintf(params + n,"default_load_core_detection_det_1 = gov.nasa.larcfm.ACCoRD.%s;",tfParam->load_core_detection_det_1);
    n += sprintf(params + n,"default_alert_2_detector=det_2;");
    n += sprintf(params + n,"default_alert_2_region= NEAR ;"); 
    n += sprintf(params + n,"default_alert_2_alerting_time= 0.0 [s];");
    n += sprintf(params + n,"default_alert_2_early_alerting_time= 0.0 [s];");
    n += sprintf(params + n,"default_alert_2_spread_hdir=%f [deg];",tfParam->alert_1_spread_trk);
    n += sprintf(params + n,"default_alert_2_spread_hs=%f [knot];",tfParam->alert_1_spread_gs);
    n += sprintf(params + n,"default_alert_2_spread_vs=%f [fpm];",tfParam->alert_1_spread_vs);
    n += sprintf(params + n,"default_alert_2_spread_alt=%f [ft];",tfParam->alert_1_spread_alt);
    n += sprintf(params + n,"default_det_2_D = %f [ft];",tfParam->det_1_WCV_DTHR);
    n += sprintf(params + n,"default_det_2_H = %f [ft];",tfParam->det_1_WCV_ZTHR);
    n += sprintf(params + n,"default_load_core_detection_det_2 = gov.nasa.larcfm.ACCoRD.CDCylinder;");
}
#endif
