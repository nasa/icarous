#define EXTERN extern
#include "guidance.h"
#include <math.h>

void GUIDANCE_Run(){

    switch(guidanceAppData.guidanceMode){
        case NOOP:{
            break;
        }

        case PRIMARY_FLIGHTPLAN:{
            int nextWP  = ComputeFlightplanGuidanceInput(&guidanceAppData.primaryFlightPlan,
                                                        guidanceAppData.nextPrimaryWP);
            guidanceAppData.nextPrimaryWP = nextWP;
            break;
        }

        case SECONDARY_FLIGHTPLAN:{
            int nextWP  = ComputeFlightplanGuidanceInput(&guidanceAppData.secondaryFlightPlan,
                                                        guidanceAppData.nextSecondaryWP);
            guidanceAppData.nextSecondaryWP = nextWP;

            break;
        }

        case POINT2POINT:{
            flightplan_t fp;
            fp.num_waypoints = 2;
            memset(fp.id,0,sizeof(char)*20);
            strcpy(fp.id,"P2P");
            // Starting position is the first waypoint
            fp.waypoints[0].latitude = guidanceAppData.position.latitude;
            fp.waypoints[0].longitude = guidanceAppData.position.longitude;
            fp.waypoints[0].altitude = guidanceAppData.position.altitude_abs;

            // Target position is second waypoint
            fp.waypoints[1].latitude = guidanceAppData.point[0];
            fp.waypoints[1].longitude = guidanceAppData.point[1];
            fp.waypoints[1].altitude = guidanceAppData.point[2];

            fp.waypoints[0].wp_metric = WP_METRIC_SPEED;
            fp.waypoints[0].value_to_next_wp = guidanceAppData.pointSpeed;
            ComputeFlightplanGuidanceInput(&fp,1);

            //bool status = Point2PointControl();
            //if(status)
                //guidanceAppData.guidanceMode = NOOP;
            break;
        }

        case VECTOR:{
            SendSBMsg(guidanceAppData.velCmd);
            break;
        }

        case ORBIT:{
            //TODO: 
            break;
        }

        case HELIX:{
            //TODO:
            break;
        }

        case TAKEOFF:{
            ComputeTakeoffGuidanceInput();
            break;
        }

        case LAND:{
            //TODO:
            break;
        }

        case SPEED_CHANGE: break;

    }

    PublishGuidanceStatus();
}

void PublishGuidanceStatus(){

    guidance_status_t status;
    CFE_SB_InitMsg(&status,GUIDANCE_STATUS_MID,sizeof(guidance_status_t),TRUE);
    status.mode = guidanceAppData.guidanceMode;
    if(status.mode == PRIMARY_FLIGHTPLAN){
        status.nextWP = guidanceAppData.nextPrimaryWP;
        status.totalWP = guidanceAppData.primaryFlightPlan.num_waypoints;
    }
    else{
        status.nextWP = guidanceAppData.nextSecondaryWP;
        status.totalWP = guidanceAppData.secondaryFlightPlan.num_waypoints;
    }

    status.velCmd[0] = guidanceAppData.velCmd.param1;
    status.velCmd[1] = guidanceAppData.velCmd.param2;
    status.velCmd[2] = guidanceAppData.velCmd.param3;
    SendSBMsg(status);

}

void ComputeTakeoffGuidanceInput(){
   if(!guidanceAppData.takeoffComplete){
       // Send the takeoff command and let the autopilot interfaces takeoff 
       // care of the various operations involved in takeoff
       argsCmd_t cmd;
       CFE_SB_InitMsg(&cmd, ICAROUS_COMMANDS_MID, sizeof(cmd), TRUE);
       cmd.name = _TAKEOFF_;
       cmd.param1 = 10;
       SendSBMsg(cmd);
       guidanceAppData.takeoffComplete = true;
   }
   else
   {
   }
}

int ComputeFlightplanGuidanceInput(flightplan_t* fp, int nextWP)
{
    // Default to position control
    if (nextWP >= fp->num_waypoints)
        nextWP = fp->num_waypoints - 1;

    double nextWaypoint[3] = {fp->waypoints[nextWP].latitude,
                              fp->waypoints[nextWP].longitude,
                              fp->waypoints[nextWP].altitude};

    double position[3] = {guidanceAppData.position.latitude,
                          guidanceAppData.position.longitude,
                          guidanceAppData.position.altitude_rel};

    double dist = ComputeDistance(position, nextWaypoint);

    if (fp->waypoints[nextWP - 1].wp_metric == WP_METRIC_SPEED)
    {
        guidanceAppData.refSpeed = fp->waypoints[nextWP - 1].value_to_next_wp;
    }

    double refSpeed = guidanceAppData.refSpeed;
    
    double newPositionToTrack[3];

    bool maintainspeed = ComputeOffSetPositionOnPlan(fp, position, nextWP - 1, newPositionToTrack);

    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (dist <= refSpeed * guidanceAppData.capRScaling)
    {
        if (!guidanceAppData.reachedStatusUpdated)
        {
            missionItemReached_t wpReached;
            CFE_SB_InitMsg(&wpReached, ICAROUS_WPREACHED_MID, sizeof(wpReached), TRUE);
            strcpy(wpReached.planID, fp->id);
            wpReached.reachedwaypoint = nextWP;
            wpReached.feedback = true;
            SendSBMsg(wpReached);
            guidanceAppData.reachedStatusUpdated = true;
            return (++nextWP);
        }else{
            return nextWP;
        }
    }
    else
    {
        guidanceAppData.reachedStatusUpdated = false;
        // Compute velocity command to next waypoint
        double heading = ComputeHeading(position, newPositionToTrack);
        double speed = refSpeed;
        if(!maintainspeed){
            speed = speed/3;
        }
        

        //TODO: climbrate gain = -0.5
        double climbrate = (nextWaypoint[2] - position[2]) * guidanceAppData.guidance_tbl.climbRageGainP;

        if (climbrate > guidanceAppData.guidance_tbl.maxClimbRate)
        {
            climbrate = guidanceAppData.guidance_tbl.maxClimbRate;
        }
        else if (climbrate < guidanceAppData.guidance_tbl.minClimbRate)
        {
            climbrate = guidanceAppData.guidance_tbl.minClimbRate;
        }

        double vn, ve, vd;
        ConvertTrkGsVsToVned(heading, speed, climbrate, &vn, &ve, &vd);

        // Store velocity command in relevant structure
        // A weighted average of the new command is used.
        double velCmd[3] = {guidanceAppData.velCmd.param1,
                            guidanceAppData.velCmd.param2,
                            guidanceAppData.velCmd.param3};
        velCmd[0] = 0.2 * velCmd[0] + 0.8 * vn;
        velCmd[1] = 0.2 * velCmd[1] + 0.8 * ve;
        velCmd[2] = vd;

        guidanceAppData.velCmd.param1 = velCmd[0];
        guidanceAppData.velCmd.param2 = velCmd[1];
        guidanceAppData.velCmd.param3 = velCmd[2];

        SendSBMsg(guidanceAppData.velCmd);
        return nextWP;
    }
}

bool ComputeOffSetPositionOnPlan(flightplan_t *fp,double position[],int currentLeg,double outputLLA[]){
    //Starting waypoint
    double wpA[3] = {fp->waypoints[currentLeg].latitude,
                     fp->waypoints[currentLeg].longitude,
                     fp->waypoints[currentLeg].altitude};

    //Ending waypoint
    double wpB[3] = {fp->waypoints[currentLeg+1].latitude,
                     fp->waypoints[currentLeg+1].longitude,
                     fp->waypoints[currentLeg+1].altitude};

    double wpC[3];
    bool finalleg = false;
    double wpSpeed;

    if(fp->waypoints[currentLeg].wp_metric == WP_METRIC_SPEED){
        wpSpeed = fp->waypoints[currentLeg].value_to_next_wp;
    }else{
        wpSpeed = guidanceAppData.refSpeed;
    }

    if(currentLeg+2 < fp->num_waypoints){
        wpC[0] = fp->waypoints[currentLeg+2].latitude;
        wpC[1] = fp->waypoints[currentLeg+2].longitude;
        wpC[2] = fp->waypoints[currentLeg+2].altitude;
        if(fp->waypoints[currentLeg + 1].wp_metric == WP_METRIC_SPEED){
            wpSpeed = fp->waypoints[currentLeg + 1].value_to_next_wp;
        }else{
            wpSpeed = guidanceAppData.refSpeed;
        }
    }else{
        finalleg = true;
    }

    double distAB = ComputeDistance(wpA,wpB);
    double distAP = ComputeDistance(wpA,position);

    double heading1 = ComputeHeading(wpA,wpB);
    double heading2 = ComputeHeading(wpB,wpC);

    //Convert to local frame
    double _wpA[3],_wpB[3],_wpC[3];
    ConvertLLA2END(position,wpA,_wpA);
    ConvertLLA2END(position,wpB,_wpB);
    ConvertLLA2END(position,wpC,_wpC);
    
    double r = wpSpeed*2;
    double outputEND[3] = {0.0,0.0,0.0};

    // If there is enough room on the current leg, get the point to track on this leg
    // else, use the other leg.
    if(distAB - distAP > r){
        GetCorrectIntersectionPoint(_wpA,_wpB,heading1,r,outputEND); 
        ConvertEND2LLA(position,outputEND,outputLLA);
        return true;
    }else{
        if(finalleg){
            memcpy(outputLLA,wpB,sizeof(double)*3);
            return false;
        }else{
            GetCorrectIntersectionPoint(_wpB,_wpC,heading2,r,outputEND); 
            ConvertEND2LLA(position,outputEND,outputLLA);
            if(fabs(heading1 - heading2) > 30){
                return false;
            }
        }
    }
    return false;
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
           // Complex roots
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
        if(r > fabs(x1)){
            y1 = sqrt(pow(r,2) - pow(x1,2)); 
            y2 = -sqrt(pow(r,2) - pow(x1,2));  
        }else{
            y1 = _wpA[1];
            y2 = _wpB[1];
        }
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

bool Point2PointControl(){

    double position[3] = {guidanceAppData.position.latitude,
                          guidanceAppData.position.longitude,
                          guidanceAppData.position.altitude_abs};

    double heading = ComputeHeading(position, guidanceAppData.point);
    double speed = guidanceAppData.pointSpeed;
    double climbrate = (guidanceAppData.point[2] - position[2]) * guidanceAppData.guidance_tbl.climbRageGainP;

    double dist = ComputeDistance(position, guidanceAppData.point);

    bool reached = false;
    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (dist <= speed * guidanceAppData.capRScaling)
    {
        reached = true;
    }

    if (climbrate > guidanceAppData.guidance_tbl.maxClimbRate)
    {
        climbrate = guidanceAppData.guidance_tbl.maxClimbRate;
    }
    else if (climbrate < guidanceAppData.guidance_tbl.minClimbRate)
    {
        climbrate = guidanceAppData.guidance_tbl.minClimbRate;
    }

    double vn, ve, vd;
    ConvertTrkGsVsToVned(heading, speed, climbrate, &vn, &ve, &vd);

    // Store velocity command in relevant structure

    double velCmd[3] = {guidanceAppData.velCmd.param1,
                        guidanceAppData.velCmd.param2,
                        guidanceAppData.velCmd.param3};

    if (!reached)
    {
        // Store velocity command in relevant structure
        // A weighted average of the new command is used.
        velCmd[0] = 0.7 * velCmd[0] + 0.3 * vn;
        velCmd[1] = 0.7 * velCmd[1] + 0.3 * ve;
        velCmd[2] = vd;
    }
    else
    {
        velCmd[0] = 0;
        velCmd[1] = 0;
        velCmd[2] = 0;

        missionItemReached_t wpReached;
        CFE_SB_InitMsg(&wpReached, ICAROUS_WPREACHED_MID, sizeof(wpReached), TRUE);
        strcpy(wpReached.planID, "P2P\0");
        wpReached.reachedwaypoint = 1;
        wpReached.feedback = true;
        SendSBMsg(wpReached);
        SetStatus(guidanceAppData.statustxt,"Reached position",SEVERITY_INFO);

    }
    guidanceAppData.velCmd.param1 = velCmd[0];
    guidanceAppData.velCmd.param2 = velCmd[1];
    guidanceAppData.velCmd.param3 = velCmd[2];

    SendSBMsg(guidanceAppData.velCmd);
    return reached;
}

double distance(double x1,double y1,double x2,double y2){
    return sqrt(pow(x2-x1,2) +pow(y2-y1,2));
}