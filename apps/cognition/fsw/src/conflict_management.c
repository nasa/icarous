#define EXTERN extern

#include <math.h>
#include "cognition.h"
#include "UtilFunctions.h"

void SetGuidanceVelCmd(double track,double gs,double vs){
      double vn = gs*cos(track* M_PI/180);
      double ve = gs*sin(track* M_PI/180);
      double vu = vs;

      argsCmd_t cmd;
      CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
      cmd.name = VECTOR;
      cmd.param1 = (float)vn;
      cmd.param2 = (float)ve;
      cmd.param3 = (float)vu;
      double speed = sqrt(pow(vn,2) + pow(ve,2) + pow(vu,2));
      SendSBMsg(cmd);
}

void SetGuidanceFlightPlan(char name[],int nextWP){
  argsCmd_t cmd;
  CFE_SB_InitMsg(&cmd,GUIDANCE_COMMAND_MID,sizeof(argsCmd_t),TRUE);
  strcpy(cmd.buffer,name);

  appdataCog.Plan0 = false;
  appdataCog.Plan1 = false;
  if(strcmp(name,"Plan0") == 0){
     appdataCog.fp = &appdataCog.flightplan1;
     cmd.name =PRIMARY_FLIGHTPLAN;
     appdataCog.nextPrimaryWP = nextWP;
     appdataCog.Plan0 = true;
  }else if(strcmp(name,"Plan1") == 0){
     appdataCog.fp = &appdataCog.flightplan2;
     cmd.name = SECONDARY_FLIGHTPLAN;
     appdataCog.nextSecondaryWP = nextWP;
     appdataCog.Plan1 = true;

     missionItemReached_t itemReached;
     CFE_SB_InitMsg(&itemReached,ICAROUS_WPREACHED_MID,sizeof(itemReached),TRUE);
     itemReached.feedback = false;
     strcpy(itemReached.planID,"Plan0");
     itemReached.reachedwaypoint = appdataCog.nextFeasibleWP1-1;
     SendSBMsg(itemReached);
  }

  cmd.param1 = nextWP;
  cmd.param2 = appdataCog.nextFeasibleWP1;
  appdataCog.nextWP = nextWP;
  SendSBMsg(cmd);
  strcpy(appdataCog.currentPlanID,name);
}

void SetGuidanceP2P(double lat,double lon,double alt,double speed){
   //Send Goto position
   argsCmd_t guidanceCmd; CFE_SB_InitMsg(&guidanceCmd, GUIDANCE_COMMAND_MID, sizeof(argsCmd_t), TRUE);

   guidanceCmd.name = POINT2POINT;
   guidanceCmd.param1 = lat;
   guidanceCmd.param2 = lon;
   guidanceCmd.param3 = alt;
   guidanceCmd.param4 = speed; 

   SendSBMsg(guidanceCmd);

  }

bool GeofenceConflictManagement(){
   bool geofenceConflict = (appdataCog.keepInConflict || appdataCog.keepOutConflict) && !appdataCog.trafficConflict;
   switch(appdataCog.geofenceConflictState){

      case NOOPC:{
         if(geofenceConflict){
            SetStatus(appdataCog.statustxt,"IC: Geofence conflict detected",SEVERITY_ALERT);
            appdataCog.geofenceConflictState = INITIALIZE;
            appdataCog.requestGuidance2NextWP = -1;
            appdataCog.return2NextWPState = NOOPC;
            break;
         }
         return false;
      }

      case INITIALIZE:{
         //TODO: Get recovery position
         SetGuidanceP2P(appdataCog.recoveryPosition[0],appdataCog.recoveryPosition[1],appdataCog.recoveryPosition[2],appdataCog.resolutionSpeed);
         appdataCog.geofenceConflictState = RESOLVE;
         appdataCog.p2pcomplete = false;
         break;
      }

      case RESOLVE:{
         // Wait for confirmation
         bool val = appdataCog.p2pcomplete;

         if(val){
            appdataCog.geofenceConflictState = COMPLETE;
         }
         break;
      }

      case COMPLETE:{
         // TODO: Fly to next waypoint
         appdataCog.requestGuidance2NextWP = 1;
         appdataCog.geofenceConflictState = NOOPC;
         appdataCog.return2NextWPState = NOOPC;
         SetStatus(appdataCog.statustxt,"IC:Geofence resolution complete",SEVERITY_NOTICE);
         break;
      }
   } 

   return true;
}

bool XtrackManagement(){

   bool XtrackConflict,XtrackConflict1, XtrackConflict2;

   XtrackConflict1 = (fabs(appdataCog.fp1monitor.crossTrackDeviation) > appdataCog.fp1monitor.allowedXtrackError);
   XtrackConflict2 = (fabs(appdataCog.fp2monitor.crossTrackDeviation) > appdataCog.fp2monitor.allowedXtrackError);

   double interceptManeuver[3];
   if(appdataCog.Plan0){
      XtrackConflict = XtrackConflict1;
      memcpy(interceptManeuver,appdataCog.fp1monitor.interceptManeuver,sizeof(double)*3);
   }else if(appdataCog.Plan1){
      XtrackConflict = XtrackConflict2;
      memcpy(interceptManeuver,appdataCog.fp2monitor.interceptManeuver,sizeof(double)*3);
   }

   bool conflict = XtrackConflict && (appdataCog.trafficConflictState == NOOPC) && (appdataCog.requestGuidance2NextWP == 0);

   switch (appdataCog.XtrackConflictState) {
      case NOOPC:{
         if(conflict){
            SetStatus(appdataCog.statustxt,"IC:xtrack conflict",SEVERITY_ALERT);
            appdataCog.XtrackConflictState = INITIALIZE;
            break;
         }
         return false;
      }

      case INITIALIZE:{
         // Send appropriate mode change command to guidance module
         appdataCog.XtrackConflictState = RESOLVE;
         break;
      }

      case RESOLVE:{
         // Send velocity command to guidance
         SetGuidanceVelCmd(interceptManeuver[0],interceptManeuver[1],interceptManeuver[2]);
         if(!XtrackConflict){
            appdataCog.XtrackConflictState = COMPLETE;
         }
         break;
      }

      case COMPLETE:{
         //Continue on with current mission
         appdataCog.XtrackConflictState = NOOPC;
         appdataCog.return2NextWPState = NOOPC;
         if(appdataCog.Plan0){
            SetGuidanceFlightPlan("Plan0",appdataCog.nextPrimaryWP);
         }else if(appdataCog.Plan1){
            SetGuidanceFlightPlan("Plan1",appdataCog.nextSecondaryWP);
         }
         SetStatus(appdataCog.statustxt,"IC:xtrack conflict resolved",SEVERITY_NOTICE);
         
         break;
      }
   }

   return true;
}

bool TrafficConflictManagement(){

   // Check for traffic conflict 
   appdataCog.trafficConflict = appdataCog.trafficSpeedConflict |
                                appdataCog.trafficAltConflict |
                                appdataCog.trafficTrackConflict;

   switch(appdataCog.trafficConflictState){
      case NOOPC:{
         if(appdataCog.trafficConflict){
            SetStatus(appdataCog.statustxt,"IC:traffic conflict",SEVERITY_ALERT);
            appdataCog.trafficConflictState = INITIALIZE;             
            appdataCog.requestGuidance2NextWP = -1;
            break;
         }
         return false;
      } 

      case INITIALIZE:{
         appdataCog.trafficConflictState = RESOLVE;        
         GetResolutionType();

         // Use this only for search based resolution
         if(appdataCog.resolutionTypeCmd == SEARCH_RESOLUTION){
            appdataCog.return2NextWPState = INITIALIZE;
         }
         break;
      }

      case RESOLVE:{
         if(appdataCog.resolutionTypeCmd == SEARCH_RESOLUTION){
            // Only for search based resolution
            if(appdataCog.return2NextWPState != NOOPC){
               ReturnToNextWP();
            }else{
               appdataCog.trafficConflictState = NOOPC;
            }
         }else{
            if(!RunTrafficResolution()){
               appdataCog.trafficConflictState = COMPLETE;
            }
         }

         break;
      }

      case COMPLETE:{
         appdataCog.requestGuidance2NextWP = 1;
         if(appdataCog.requestGuidance2NextWP == 0 && appdataCog.Plan0 && appdataCog.trkBands.fp1ClosestPointFeasible){
            appdataCog.trafficConflictState = NOOPC;
         }else if(appdataCog.requestGuidance2NextWP == 0 && appdataCog.Plan1 && appdataCog.trkBands.fp2ClosestPointFeasible){
            appdataCog.trafficConflictState = NOOPC;
         }else if(appdataCog.requestGuidance2NextWP == 1){
            appdataCog.trafficConflictState = NOOPC;
         }else{
            appdataCog.requestGuidance2NextWP = -1;
         }
         SetStatus(appdataCog.statustxt,"IC:traffic conflict resolved",SEVERITY_NOTICE);
         appdataCog.return2NextWPState = NOOPC;
         break;
      }
   }

   return true;
}

bool RunTrafficResolution(){
   switch(appdataCog.resolutionTypeCmd){

      case SPEED_RESOLUTION:{
         double speedPref = appdataCog.preferredSpeed;

         //OS_printf("resolution speed = %f\n",speedPref);
         if(speedPref >= 0){
            SetGuidanceVelCmd(appdataCog.position.hdg,speedPref,0);
            appdataCog.prevResSpeed = speedPref;
         }else{
            SetGuidanceVelCmd(appdataCog.position.hdg,appdataCog.prevResSpeed,0);
         }

         bool val;
         if(appdataCog.Plan0){
            val = appdataCog.gsBands.wpFeasibility1[appdataCog.nextPrimaryWP];
         }else if(appdataCog.Plan1){
            val = appdataCog.gsBands.wpFeasibility2[appdataCog.nextSecondaryWP];
         }
         appdataCog.returnSafe = val;
         break;
      }

      case ALTITUDE_RESOLUTION:{
         double climbRate = 0.0;
         double diff;

         double currentAlt = appdataCog.position.altitude_rel;
         double altPref = appdataCog.preferredAlt;

         diff = altPref - currentAlt;
         //OS_printf("preferred altitude: %f\n",altPref);
         if(diff > -1e2 && diff < 1e2){

            if(diff > 50){
               diff = 50;  
            }else if(diff < -50){
               diff = -50;
            }

            climbRate = -0.1*diff;
            double speed = appdataCog.resolutionSpeed;
            SetGuidanceVelCmd(appdataCog.position.hdg,speed,climbRate);
            appdataCog.prevResVspeed = climbRate;
            //OS_printf("climb rate = %f,diff = %f\n",climbRate,diff);
         }else if(altPref > -1000 && altPref < 0){
            double speed = appdataCog.resolutionSpeed; 
            SetGuidanceVelCmd(appdataCog.position.hdg,speed,0);
         }else{

         }


         bool val;
         if(appdataCog.Plan0){
            val = appdataCog.altBands.wpFeasibility1[appdataCog.nextPrimaryWP];
         }else if(appdataCog.Plan1){
            val = appdataCog.altBands.wpFeasibility2[appdataCog.nextSecondaryWP];
         }
         appdataCog.returnSafe = val;
         break;
      }

      case TRACK_RESOLUTION:{
         //OS_printf("executing traffic resolution\n");
         double prefTrack = appdataCog.preferredTrack;
         double speed = appdataCog.resolutionSpeed;
         double climbrate = 0;
         if(prefTrack > 0){
            SetGuidanceVelCmd(prefTrack,speed,0);
            appdataCog.prevResTrack = prefTrack;
         }else if(prefTrack > -10000){
            SetGuidanceVelCmd(appdataCog.prevResTrack,speed,0);
         }else{

         }

         if(appdataCog.Plan0){
            appdataCog.returnSafe = CheckSafeToTurn(appdataCog.position.hdg,appdataCog.fp1monitor.interceptHeadingToPlan);
         }else if(appdataCog.Plan1){
            appdataCog.returnSafe = CheckSafeToTurn(appdataCog.position.hdg,appdataCog.fp2monitor.interceptHeadingToPlan);
         }

         if(!appdataCog.returnSafe){
            SetGuidanceVelCmd(appdataCog.prevResTrack,speed,0);
         }else{
            //OS_printf("return safe\n");
         }

         break;
      }

      case VERTICALSPEED_RESOLUTION:{

         break;
      }

      default:{

         break;
      }
   }    
   appdataCog.trafficConflict |= (!appdataCog.returnSafe);
   return appdataCog.trafficConflict;
}

bool ReturnToNextWP(){

   //Set this as the current mission wp
   switch(appdataCog.return2NextWPState){

      case NOOPC:{

         if (appdataCog.keepInConflict || appdataCog.keepOutConflict || appdataCog.trafficConflict){
            return false;
         }

         if(appdataCog.requestGuidance2NextWP == 1){
            appdataCog.return2NextWPState = INITIALIZE;
            break;
         }
         return false;
      }

      case INITIALIZE:{

         int nextWP;
         bool directPath;
         appdataCog.fp2complete = false;
         appdataCog.p2pcomplete = false;
         if(appdataCog.Plan0){
            nextWP = appdataCog.nextFeasibleWP1;
            directPath = appdataCog.directPathToFeasibleWP1;
         }else if(appdataCog.Plan1){
            nextWP = appdataCog.nextFeasibleWP2;
            directPath = appdataCog.directPathToFeasibleWP2;
         }

         if (directPath)
         {
            missionItemReached_t itemReached;
            CFE_SB_InitMsg(&itemReached, ICAROUS_WPREACHED_MID, sizeof(itemReached), TRUE);
            itemReached.feedback = false;

            if(appdataCog.Plan0){
               strcpy(itemReached.planID, "Plan0");
               itemReached.reachedwaypoint = appdataCog.nextFeasibleWP1 - 1;
            }
            else if(appdataCog.Plan1){
               strcpy(itemReached.planID, "Plan1");
               itemReached.reachedwaypoint = appdataCog.nextFeasibleWP2 - 1;
            }
            SendSBMsg(itemReached);

            SetGuidanceP2P(appdataCog.fp->waypoints[nextWP].latitude,
                           appdataCog.fp->waypoints[nextWP].longitude,
                           appdataCog.fp->waypoints[nextWP].altitude,appdataCog.resolutionSpeed);
            appdataCog.return2NextWPState = RESOLVE;
         }
         else
         {
            appdataCog.return2NextWPState = COMPUTE;
            appdataCog.request = 0;
         }
         break;
      }

      case COMPUTE:
      {
         if (appdataCog.request == 0)
         {
            // Compute a secondary path
            int nextWP;
            if(appdataCog.Plan0){
               nextWP = appdataCog.nextFeasibleWP1;
            }else if(appdataCog.Plan1){
               nextWP = appdataCog.nextFeasibleWP2;
            }

            double positionA[3] = {appdataCog.position.latitude,
                                   appdataCog.position.longitude,
                                   appdataCog.position.altitude_rel};

            double Trk, Gs, Vs;
            ConvertVnedToTrkGsVs(appdataCog.position.vn, 
                                 appdataCog.position.ve,
                                 appdataCog.position.vd, 
                                 &Trk, &Gs, &Vs);

            double velocityA[3] = {Trk,Gs,Vs};

            double positionB[3] = {appdataCog.fp->waypoints[nextWP].latitude,
                                   appdataCog.fp->waypoints[nextWP].longitude,
                                   appdataCog.fp->waypoints[nextWP].altitude};

            FindNewPath(appdataCog.searchAlgType, positionA, velocityA, positionB);
            appdataCog.request = -1;
            SetStatus(appdataCog.statustxt,"IC:Computing secondary path",SEVERITY_NOTICE);
         }
         else if (appdataCog.request == 1)
         {
            appdataCog.request = 0;
            appdataCog.return2NextWPState = RESOLVE;
            appdataCog.fp2complete = false;
            SetGuidanceFlightPlan("Plan1",1);  
         }else{

         }
         break;
      }

      case RESOLVE:{
         // Execute the secondary path

         bool val = appdataCog.fp2complete || appdataCog.p2pcomplete;
         if(val){
            appdataCog.return2NextWPState = COMPLETE;
         }
         break;
      }

      case COMPLETE:{

         appdataCog.return2NextWPState = NOOPC;
         appdataCog.requestGuidance2NextWP = -1;
         if(appdataCog.nextFeasibleWP1 + 1 < appdataCog.flightplan1.num_waypoints){
            SetGuidanceFlightPlan("Plan0",appdataCog.nextFeasibleWP1+1);
         }else{
            appdataCog.nextPrimaryWP = appdataCog.nextFeasibleWP1 + 1;  
         }
         SetStatus(appdataCog.statustxt,"IC:Resuming mission",SEVERITY_NOTICE);
         break;
      }
   }
}

static bool CheckSafeToTurn(double fromHeading, double toHeading) {
   bool conflict = false;
   for (int i = 0; i < appdataCog.trkBands.numBands; i++)
   {
      bool val;
      val = CheckTurnConflict(appdataCog.trkBands.min[i],
                              appdataCog.trkBands.max[i],
                              fromHeading, toHeading);
      conflict |= val;
   }

   if(appdataCog.Plan0)
      conflict |= !appdataCog.trkBands.wpFeasibility1[appdataCog.nextPrimaryWP];
   else if(appdataCog.Plan1)
      conflict |= !appdataCog.trkBands.wpFeasibility2[appdataCog.nextSecondaryWP];

   return !conflict;
}

bool CheckDirectPathFeasibility(position_t posA,waypoint_t posB){
    double positionA[3] = {posA.latitude,posA.longitude,posA.altitude_rel};
    double positionB[3] = {posB.latitude,posB.longitude,posB.altitude};
    double distance;

    pathFeasibilityCheck_t pfcheck;
    CFE_SB_InitMsg(&pfcheck, GEOFENCE_PATH_CHECK_MID, sizeof(pathFeasibilityCheck_t), TRUE);
    memcpy(pfcheck.fromPosition, positionA, sizeof(double) * 3);
    memcpy(pfcheck.toPosition, positionB, sizeof(double) * 3);
    SendSBMsg(pfcheck);
}

void FindNewPath(algorithm_e searchType, double positionA[],double velocityA[],double positionB[]){

   char algName[10];

   trajectory_request_t pathRequest;
   CFE_SB_InitMsg(&pathRequest, ICAROUS_TRAJECTORY_REQUEST_MID, sizeof(trajectory_request_t), TRUE);

   pathRequest.algorithm = searchType;

   memcpy(pathRequest.initialPosition, positionA, sizeof(double) * 3);
   memcpy(pathRequest.initialVelocity, velocityA, sizeof(double) * 3);
   memcpy(pathRequest.finalPosition, positionB, sizeof(double) * 3);

   SendSBMsg(pathRequest);
}

void GetResolutionType(){
   //
}
