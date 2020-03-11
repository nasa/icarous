#define EXTERN extern
#include "guidance.h"
#include <math.h>

double ComputeClimbRate(double position[3],double nextWaypoint[3],double speed,guidanceTable_t* guidanceParams){
    double deltaH = nextWaypoint[2] - position[2];
    double climbrate;
    if (fabs(deltaH) > guidanceParams->climbAngleVRange &&
        ComputeDistance(position, nextWaypoint) > guidanceParams->climbAngleHRange){
        // Over longer altitude changes and distances, control the ascent/descent angle
        double angle = guidanceParams->climbFpAngle;
        if (deltaH < 0){
            angle = -angle;
        }
        double cfactor = tan(angle * M_PI/180);
        climbrate = cfactor * speed;
    } else {
        // Over shorter altitude changes and distances, use proportional control
        climbrate = deltaH * guidanceParams->climbRateGain;
    }
    if (climbrate > guidanceParams->maxClimbRate) {
        climbrate = guidanceParams->maxClimbRate;
    } else if (climbrate < guidanceParams->minClimbRate) {
        climbrate = guidanceParams->minClimbRate;
    }
    return climbrate;
}

int ComputeFlightplanGuidanceInput(flightplan_t* fp, double position[3], int nextWP, bool* reachedStatusUpdated, guidanceTable_t* guidanceParams,
                                   double velCmd[4], double* refSpeedPtr)
{
    // Default to position control
    if (nextWP >= fp->num_waypoints)
        nextWP = fp->num_waypoints - 1;

    double nextWaypoint[3] = {fp->waypoints[nextWP].latitude,
                              fp->waypoints[nextWP].longitude,
                              fp->waypoints[nextWP].altitude};


    double dist = ComputeDistance(position, nextWaypoint);

    if (fp->waypoints[nextWP - 1].wp_metric == WP_METRIC_SPEED)
    {
        *refSpeedPtr = fp->waypoints[nextWP - 1].value_to_next_wp;
    }

    double refSpeed = *refSpeedPtr;
    
    double newPositionToTrack[3];

    bool maintainspeed = ComputeOffSetPositionOnPlan(fp, position, nextWP - 1, refSpeed, newPositionToTrack);

    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (dist <= refSpeed * guidanceParams->captureRadiusScaling)
    {
        if (!*reachedStatusUpdated)
        {
            *reachedStatusUpdated = true;
            return (++nextWP);
        }else{
            return nextWP;
        }
    }
    else
    {
        *reachedStatusUpdated = false;
        // Compute velocity command to next waypoint
        double heading = ComputeHeading(position, newPositionToTrack);
        double speed = refSpeed;
        if(!maintainspeed){
            speed = speed/3;
        }
        
        double climbrate = ComputeClimbRate(position, nextWaypoint, speed, guidanceParams);

        double vn, ve, vd;
        ConvertTrkGsVsToVned(heading, speed, climbrate, &vn, &ve, &vd);

        // Store velocity command in relevant structure
        // A weighted average of the new command is used.
        velCmd[0] = 0.2 * velCmd[0] + 0.8 * vn;
        velCmd[1] = 0.2 * velCmd[1] + 0.8 * ve;
        velCmd[2] = 0.2 * velCmd[2] + 0.8 * vd;

        return nextWP;
    }
}

bool ComputeOffSetPositionOnPlan(flightplan_t *fp,double position[],int currentLeg,double refSpeed,double outputLLA[]){
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
        wpSpeed = refSpeed;
    }

    if(currentLeg+2 < fp->num_waypoints){
        wpC[0] = fp->waypoints[currentLeg+2].latitude;
        wpC[1] = fp->waypoints[currentLeg+2].longitude;
        wpC[2] = fp->waypoints[currentLeg+2].altitude;
        if(fp->waypoints[currentLeg + 1].wp_metric == WP_METRIC_SPEED){
            wpSpeed = fp->waypoints[currentLeg + 1].value_to_next_wp;
        }else{
            wpSpeed = refSpeed;
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

bool Point2PointControl(double position[3], double target_point[3], double speed, guidanceTable_t* guidanceParams, double velCmd[3]){

    double heading = ComputeHeading(position, target_point);

    double climbrate = ComputeClimbRate(position, target_point, speed, guidanceParams);

    double dist = ComputeDistance(position, target_point);

    bool reached = false;
    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (dist <= speed * guidanceParams->captureRadiusScaling)
    {
        reached = true;
    }

    double vn, ve, vd;
    ConvertTrkGsVsToVned(heading, speed, climbrate, &vn, &ve, &vd);

    // Store velocity command in relevant structure
    if (!reached)
    {
        // Store velocity command in relevant structure
        // A weighted average of the new command is used.
        velCmd[0] = 0.7 * velCmd[0] + 0.3 * vn;
        velCmd[1] = 0.7 * velCmd[1] + 0.3 * ve;
        velCmd[2] = 0.7 * velCmd[2] + 0.3 * vd;
    } else {
        velCmd[0] = 0;
        velCmd[1] = 0;
        velCmd[2] = 0;
    }
    return reached;
}

double distance(double x1,double y1,double x2,double y2){
    return sqrt(pow(x2-x1,2) +pow(y2-y1,2));
}