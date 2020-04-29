#define EXTERN extern
#include "guidanceFunctions.h"

double ComputeClimbRate(double position[4],double nextWaypoint[4],double speed,guidanceParams_t* guidanceParams){
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

double ComputeSpeed(double currPosition[5],double nextWP[5],double currSpeed,guidanceParams_t* guidanceParams){

   // If speed is provided for nextWP, use given speed
   if (nextWP[3] < 1){
       double speed = nextWP[4];

       if( (speed - 0.25) < 1e-3){
           speed = guidanceParams->defaultWpSpeed;
       }

       if(speed <= guidanceParams->minSpeed){
           return guidanceParams->minSpeed;
       }else if(speed >= guidanceParams->maxSpeed){
           return guidanceParams->maxSpeed;
       }else{
           return speed;
       }
   }

   double distH = ComputeDistance(currPosition,nextWP);
   double distV = fabs(currPosition[2] - nextWP[2]);
   double dist = sqrt(distH*distH + distV*distV);
   struct timespec ts;
   clock_gettime(CLOCK_REALTIME,&ts);

   double currTime = ts.tv_sec + (double)(ts.tv_nsec)/1E9;
   double nextWP_STA = nextWP[4];
   double arrTolerance = 3; //TODO: Make this a user defined parameter
   double maxSpeed = guidanceParams->maxSpeed;
   double minSpeed = guidanceParams->minSpeed;
   double newSpeed;
   newSpeed = dist/(nextWP_STA - currTime);
   if (newSpeed > maxSpeed){
       newSpeed = maxSpeed;
   }else{
       if(newSpeed < minSpeed){
       newSpeed = minSpeed;
       }
   }
   return newSpeed;
}

int ComputeFlightplanGuidanceInput(guidanceInput_t* guidanceInput, guidanceOutput_t* guidanceOutput, guidanceParams_t* guidanceParams) {
    int nextWP = guidanceInput->nextWP;
    bool finalleg = false;
    if (nextWP >= guidanceInput->num_waypoints - 1){
        nextWP = guidanceInput->num_waypoints - 1;
        finalleg = true;
    }

    double distH = ComputeDistance(guidanceInput->position, guidanceInput->curr_waypoint);
    double distV = fabs(guidanceInput->position[2] - guidanceInput->curr_waypoint[2]);

    double currSpeed = sqrt( guidanceInput->velocity[0] * guidanceInput->velocity[0] + 
                             guidanceInput->velocity[1] * guidanceInput->velocity[1] + 
                             guidanceInput->velocity[2] * guidanceInput->velocity[2]);
    double speedRef = ComputeSpeed(guidanceInput->position, guidanceInput->curr_waypoint,currSpeed,guidanceParams);
    double capture_radius = speedRef * guidanceParams->captureRadiusScaling;
    if (guidanceParams->minCap >= capture_radius){
        capture_radius = guidanceParams->minCap;
    }else if(capture_radius > guidanceParams->maxCap){
        capture_radius = guidanceParams->maxCap;
    }
    double newPositionToTrack[3];
    ComputeOffSetPositionOnPlan(speedRef, guidanceInput, guidanceParams, newPositionToTrack);
    // Reduce speed if approaching final waypoint or if turning sharply
    double ownship_heading = fmod(2*M_PI + atan2(guidanceInput->velocity[1],guidanceInput->velocity[0]),2*M_PI) *180/M_PI;
    double target_heading = ComputeHeading(guidanceInput->position, newPositionToTrack);
    double turn_angle = fabs(fmod(180 + ownship_heading - target_heading, 360) - 180);
    if((finalleg && distH < capture_radius) || turn_angle > 60){
        double range = guidanceParams->maxSpeed - guidanceParams->minSpeed;
        if( speedRef > (guidanceParams->minSpeed + range * 0.6) ){
            speedRef = speedRef/2;
        }
    }


    // If distance to next waypoint is < captureRadius, switch to next waypoint
    if (distH <= capture_radius && distV <= guidanceParams->climbAngleVRange)
    {
        if (!guidanceInput->reachedStatusUpdated)
        {
            guidanceOutput->reachedStatusUpdated = true;
            guidanceOutput->newNextWP = nextWP + 1;
        }else{
            guidanceOutput->newNextWP = nextWP;
        }
    }
    else
    {
        guidanceOutput->reachedStatusUpdated = false;
        // Compute velocity command to next waypoint
        double heading = ComputeHeading(guidanceInput->position, newPositionToTrack);
        double climbrate = ComputeClimbRate(guidanceInput->position, guidanceInput->curr_waypoint, speedRef, guidanceParams);

        double vn, ve, vd;
        ConvertTrkGsVsToVned(heading, speedRef, climbrate, &vn, &ve, &vd);

        // Store velocity command in relevant structure
        // A weighted average of the new command is used.
        double n = 1;
        guidanceOutput->velCmd[0] = (n-1) * guidanceInput->velCmd[0] + n * vn;
        guidanceOutput->velCmd[1] = (n-1) * guidanceInput->velCmd[1] + n * ve;
        guidanceOutput->velCmd[2] = (n-1) * guidanceInput->velCmd[2] + n * vd;

        guidanceOutput->newNextWP = nextWP;
    }
    return guidanceOutput->newNextWP;
}

void ComputeOffSetPositionOnPlan(double speedRef,guidanceInput_t* guidanceInput, guidanceParams_t* guidanceParams, double outputLLA[]){
    double guidance_radius = speedRef*guidanceParams->guidanceRadiusScaling;
    double xtrkDev = guidanceParams->xtrkDev;
    if(xtrkDev > guidance_radius)
        xtrkDev = guidance_radius;

    // Create vectors for wpA->wpB and wpA->ownship_position
    double AB[3], AP[3];
    ConvertLLA2END(guidanceInput->prev_waypoint,guidanceInput->curr_waypoint, AB);
    ConvertLLA2END(guidanceInput->prev_waypoint,guidanceInput->position, AP);

    double distAB = ComputeDistance(guidanceInput->prev_waypoint,guidanceInput->curr_waypoint);
    double distAP = ComputeDistance(guidanceInput->position,guidanceInput->prev_waypoint);
    double distPB = ComputeDistance(guidanceInput->position,guidanceInput->curr_waypoint);

    double projection = (AB[0]*AP[0]+AB[1]*AP[1])/pow(distAB,2);
    double closest_point[3];
    closest_point[0] = AB[0]*projection;
    closest_point[1] = AB[1]*projection;
    closest_point[2] = AB[2]*projection;
    double deviation = distance(AP[0], AP[1], closest_point[0], closest_point[1]);

    //Convert waypoints to local frame (with ownship position as origin)
    double _wpA[3],_wpB[3];
    ConvertLLA2END(guidanceInput->position,guidanceInput->prev_waypoint,_wpA);
    ConvertLLA2END(guidanceInput->position,guidanceInput->curr_waypoint,_wpB);

    double outputEND[3] = {0.0,0.0,0.0};

    if(distPB <= guidance_radius){
        //If within guidance radius of wpB, track to wpB
        memcpy(outputLLA,guidanceInput->curr_waypoint,sizeof(double)*3);
    } else if(distAP <= guidance_radius){
        // If within guidance radius of wpA, use guidance circle method
        GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius,outputEND);
        ConvertEND2LLA(guidanceInput->position,outputEND,outputLLA);
    } else {
        // Otherwise, check projection of position onto flight plan
        if(projection <= 0){
            // If behind wpA, track to wpA
            memcpy(outputLLA,guidanceInput->prev_waypoint,sizeof(double)*3);
        } else if(projection >= 1){
            // If past wpB, track to wpB
            memcpy(outputLLA,guidanceInput->curr_waypoint,sizeof(double)*3);
        } else {
            // If between wpA and wpB
            if(deviation < xtrkDev){
                // If close enough to flight plan, use guidance circle method
                GetCorrectIntersectionPoint(_wpA,_wpB,guidance_radius,outputEND);
                ConvertEND2LLA(guidanceInput->position,outputEND,outputLLA);
            } else {
                // If far from flight plan, track to closest point on flight plan
                ConvertEND2LLA(guidanceInput->prev_waypoint,closest_point,outputLLA);
            }
        }
    }
}

void GetCorrectIntersectionPoint(double _wpA[],double _wpB[],double r,double output[]){
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

    // Check which point is closest to waypoint B
    double p1[] = {x1, y1};
    double p2[] = {x2, y2};
    double dist1 = distance(p1[0],p1[1],_wpB[0],_wpB[1]);
    double dist2 = distance(p2[0],p2[1],_wpB[0],_wpB[1]);
    if(dist1 < dist2){
        // Use (x1,y1)
        output[0] = x1;
        output[1] = y1;
    }else{
        // Use (x2,y2)
        output[0] = x2;
        output[1] = y2;
    }
}

bool Point2PointControl(double position[3], double target_point[3], double speed, guidanceParams_t* guidanceParams, double velCmd[3]){

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
