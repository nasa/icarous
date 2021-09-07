#include "Merger.hpp"
#include "StateReader.h"
#include "ParameterData.h"

Merger::Merger(std::string callsign,std::string config,int vID){

    vehicleID = vID;
    // Open log files
    struct timespec  tv;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;

    char filename1[50];
    sprintf(filename1,"log/mergerLog-%s-%f.log",callsign.c_str(),localT);
    logFile1 = fopen(filename1,"w");
    fprintf(logFile1, "Vehicle ID: %d",vID);   

    char filename2[50];
    sprintf(filename2,"log/merger_appdata-%s-%f.log",callsign.c_str(),localT);
    logFile2 = fopen(filename2,"w");
    fprintf(logFile2,"# time (s), intID, dist2int, speed, f/c/l, (r,t,d), zone, numSch, merge speed, merge dev, merging status, lat, lon, alt\n");

    // Initialize total number of fixes
    totalFixes = 0;
    ReadParamFromFile(config);
    // Iniitalize all internal variables
    ResetData();
}

void Merger::ReadParamFromFile(std::string config){
    larcfm::StateReader reader;
    larcfm::ParameterData parameters;
    reader.open(config);
    reader.updateParameterData(parameters);

    separationTime = parameters.getValue("separation_time");
    coordZone = parameters.getValue("coordination_zone");
    schedZone = parameters.getValue("schedule_zone");
    entryZone = parameters.getValue("entry_zone");
    corridorWidth = parameters.getValue("corridor_width"); 
    maxSpeed = parameters.getValue("max_hs"); 
    minSpeed = parameters.getValue("min_hs");
}

void Merger::ResetData(){

    // The vehicle has exited the merge fix
    // Let's reset states that must be reset
    // before entering another intersection
    numSchedulesComputed = 0;
    numNodesInt = 1;
    dist2Int = 0;
    time2ZoneEntry = 0;
    mergingSpeed = -1;
    approaching = false;
    mergeConflict = false;
    entryPointComputed = false;
    arrivalDataUpdated = false;
    defaultEntryPlan = true;
    inComputeZone = false;
    publishMergingStatus = false;
    executePath = false;
    currentFixIndex = -1;
    navState = _MERGER_NOOP_;
    numSchedulesComputed = 0;
    refSpeed = 0;
    XtrkDev = 0;
    raftRole = 0;

    // Update arrival data with NULL zone and invalid intersection
    memset(&ownshipArrivalData,0,sizeof(mergingData_t));
    ownshipArrivalData.aircraftID = vehicleID;
    ownshipArrivalData.intersectionID = -1;
    ownshipArrivalData.numSchedulesComputed = 0;
    ownshipArrivalData.zoneStatus = NULL_ZONE;

    mergingStatus = MERGING_INACTIVE;

    //Erase all existing merging data
    memset(globalArrivalData, 0, sizeof(mergingData_t) * MERGER_MAXAIRCRAFT);

    //fprintf(logFile1, " <<< Resetting Internal Data >>> \n");
}

void Merger::SetVehicleConstraints(double min_gs, double max_gs, double turn_radius){
    maxSpeed = max_gs; 
    minSpeed = min_gs;
    turnRadius = turn_radius;
}

void Merger::SetMergefixParams(double sepTime,double coordzone,double schzone,double entryzone,double corridor_width){
    separationTime = sepTime;
    coordZone = coordzone;
    schedZone = schzone;
    entryZone = entryzone;
    corridorWidth = corridor_width;
}

void Merger::SetVehicleState(double pos[],double trkgsvs[]){
    double vel[3];
    vel[0] = trkgsvs[1] * cos(trkgsvs[0]*M_PI/180);
    vel[1] = trkgsvs[1] * sin(trkgsvs[0]*M_PI/180);
    vel[2] = trkgsvs[2];
    memcpy(position,pos,sizeof(double)*3);
    memcpy(velocity,vel,sizeof(double)*3);
    currentSpeed = sqrt(pow(velocity[0],2) + 
                        pow(velocity[1],2) + 
                        pow(velocity[2],2));
}

void Merger::SetIntersectionData(int i,int id,double pos[]){
    if(i>=totalFixes) totalFixes++;
    if(totalFixes > INTERSECTION_MAX) return;
    mergeFixId[i] = id;
    memcpy(mergeFixes[i],pos,sizeof(double)*3);
}

void Merger::SetNodeLog(dataLog_t* mgData){
    if ((int) currentFixIndex >= 0) 
    {
        if (mgData->intersectionID == mergeFixId[currentFixIndex])
        {
            raftRole = mgData->nodeRole;
            numNodesInt = mgData->totalNodes;
            memset(globalArrivalData, -1, sizeof(mergingData_t) * MERGER_MAXAIRCRAFT);
            memcpy(globalArrivalData, mgData->log, sizeof(mergingData_t) * numNodesInt);

            fprintf(logFile1,"\n\n***********LOG ENTRIES: ***************\n");
            fprintf(logFile1,"time: %.5f\n",currentLocalTime);
            fprintf(logFile1,"Total nodes: %d\n",numNodesInt);

            for(int i=0;i<MAX_NODES;i++)
            {
                fprintf(logFile1, "Aircraft id: %d, intersection: %d\n", globalArrivalData[i].aircraftID, 
                                                                         globalArrivalData[i].intersectionID);
                fprintf(logFile1, "zone: %d\n", globalArrivalData[i].zoneStatus);
                fprintf(logFile1, "R, T, D: %f, %f, %f\n", globalArrivalData[i].earlyArrivalTime,
                        globalArrivalData[i].currentArrivalTime,
                        globalArrivalData[i].lateArrivalTime);
            }
            fprintf(logFile1,"\n");
        }
    }
}

double Merger::GetApproachPrecision(double position[],double velocity[], double intersection[]){
    // Look at the dot product of current velocity
    // with the position of the intersection in relative NED coordinates
    // dot product >= 0 IMPLIES we are approaching the intersection
    double ENDIntersection[3];
    ConvertLLA2END(position, intersection, ENDIntersection);

    double vel_norm   = sqrt( velocity[0] * velocity[0] + velocity[1] * velocity[1]);
    double dist_norm  = sqrt( ENDIntersection[0] * ENDIntersection[0] + ENDIntersection[1] * ENDIntersection[1]);
    double vel_hat[2] =  {velocity[0]/vel_norm, velocity[1]/vel_norm};
    double dist_hat[2] = {ENDIntersection[0]/dist_norm, ENDIntersection[1]/dist_norm};

    double dot_VelocityIntersection = vel_hat[0] * dist_hat[1] +
                                      vel_hat[1] * dist_hat[0];

    return dot_VelocityIntersection;
}

int Merger::GetOutputTrajectory(int i, double wp[4]){
    wp[0] = flightplan[i][0];
    wp[1] = flightplan[i][1];
    wp[2] = flightplan[i][2];
    wp[3] = flightplan[i][3];

    if(XtrkDev > 0){
        return 3;
    }else{
        return 2;
    }
}

void Merger::GetOutputVelocity(double *trk,double *gs,double *vs){
    *trk = cmdOutput[0];
    *gs  = cmdOutput[1];
    *vs  = cmdOutput[2];
}

bool Merger::GetArrivalTimes(mergingData_t* mgData){
    int n = outputArrData.size();
    if(n > 0){
        mergingData_t arrUpdate;
        arrUpdate = outputArrData.front();
        outputArrData.pop();
        memcpy(mgData, &arrUpdate, sizeof(mergingData_t));
        return true;
    }else{
        return false;
    }
}

/* Function to determine the neartest merge fix */
int Merger::FindCurrentIntersection(){

    double mindist = DBL_MAX;
    unsigned int minindex;
    for(int i =0;i<totalFixes;++i){
        // If intersection ID is invalid, skip that entry
        if(mergeFixId[i] < 1){
            continue;
        }

        // Distance to this intersection
        double dist = ComputeDistance(mergeFixes[i], position);

        // Look at the dot product of current velocity 
        // with the position of the intersection in relative NED coordinates
        // dot product >= 0 IMPLIES we are approaching the intersection
        double ENDIntersection[3];
        ConvertLLA2END(position, mergeFixes[i], ENDIntersection);
        double approachPrecision = GetApproachPrecision(position,velocity,mergeFixes[i]);

        // If there is an intersection that we are approaching and closer
        // than any other intersection, that should be the current intersection
        if (dist < mindist && approachPrecision >=0.8){
            mindist = dist;
            minindex = i;
        }

    }

    // Consider a valid intersection only if we are within
    // the coordination zone for that intersection
    if(mindist <= coordZone){
        return minindex;
    }
    else
    {
        return -1;
    }
    
}

/* The main function that makes the main decisions for merging */
unsigned char Merger::RunMergingOperation(double time)
{

    currentLocalTime = time;
    // Check if we've exited an merge fix
    // This should be done first to so that the interal states are reset
    CheckIntersectionExit();

    // Find the nearest merge fix that we are currently approaching
    currentFixIndex = FindCurrentIntersection();
    if((int)currentFixIndex >= 0){

        // Compute distance to merge fix
        memcpy(currentIntersection,mergeFixes[currentFixIndex],sizeof(double)*3);
        dist2Int = ComputeDistance(currentIntersection, position);
        fprintf(logFile1, "distance to intersection %d : %f\n",mergeFixId[currentFixIndex],dist2Int);   

        // Identify the zone the vehicle is currently in
        zoneType_e zone = IdentifyCurrentZone();
        switch (zone)
        {
            case NULL_ZONE:
                break;

            case COORD_ZONE:
            {
                mergingStatus = MERGING_ACTIVE;
                // Compute the arrival times
                ComputeArrivalData();

                // Exchange arrival time information
                ExchangeArrivalTimes();
                break;
            }

            case SCHED_ZONE:
            {
                // NOTE: Arrival time information is not updated after you enter 
                // schedule zone (a.k.a computation zone). This is so that vehicles
                // can agree on each other's arrival time information.

                // Check for conflicting arrival at the intersection
                // based on available arrival time information
                CheckIntersectionConflict();

                // Reschedule if there is a conflict
                if(mergeConflict){
                    ComputeSchedule();
                }

                // Exchange arrival times
                ExchangeArrivalTimes();

                break;
            }

            case ENTRY_ZONE:
            {

                // Execute the required controls to resolve any conflict
                if(mergingSpeed > 0){
                    ExecuteNewPath();
                }


                // Exchange arrival times
                ExchangeArrivalTimes();

                break;
            }
        }
        // Append entry to log


    }

    AddLogEntry();
    return mergingStatus;
}

/* Function to Identify the current zone */
// Zones are determined based on distance to the merge fix
zoneType_e Merger::IdentifyCurrentZone()
{
    if (dist2Int > coordZone)
    {
        // If the vehicle is outside the coordization zone, no need for any computation
        currentZone = NULL_ZONE;
    }
    else if (dist2Int > schedZone && dist2Int <= coordZone)
    {
        currentZone = COORD_ZONE;
    }
    else if (dist2Int > entryZone && dist2Int <= schedZone)
    {
        currentZone = SCHED_ZONE;
    }
    else
    {
        currentZone = ENTRY_ZONE;
    }

    return currentZone;
}


/* Check if vehicle has exited a merge fix */
void Merger::CheckIntersectionExit()
{
    double approachPrecision = GetApproachPrecision(position,velocity,currentIntersection);
    // Check when intersection is left behind
    if (approachPrecision >= 0.8)
    {
        approaching = true;
    }
    else
    {
        if(currentFixIndex < 0){
             return;
        }

        // Only reset if we passed half way through the entry zone.
        // Reset all necessary state variables
        if(dist2Int < 0.5*entryZone){
            ResetData(); 
        }
    }
}

void Merger::ComputeArrivalData()
{
    double dist = 0;
    if (defaultEntryPlan)
    {
        dist = entryZone;
    }else
    {
        dist = detourDist; // Compute distance of entry flight plan
    }

    if (!entryPointComputed)
    {
        // Compute Entry point
        double headingToMyPos = ComputeHeading(currentIntersection, position);
        ComputeOffsetPosition(currentIntersection, headingToMyPos, entryZone, entryPoint);

        entryPointComputed = true;
        fprintf(logFile1, "Entry point computed %f,%f\n",entryPoint[0],entryPoint[1]);
    }

    dist2ZoneEntry = ComputeDistance(position, entryPoint);

    double d = entryZone;
    double xc1 = turnRadius;
    double xc3 = corridorWidth;

    double nfac1 = sqrt(pow(xc1, 2) + pow(xc3, 2));
    double nfac2 = sqrt(pow(d - xc1, 2) + pow(xc3, 2));

    double earlyTime = dist / maxSpeed;
    double lateTime = (nfac1 + nfac2) / minSpeed;
    double time2PassInt = (dist /currentSpeed);
    time2ZoneEntry = currentLocalTime + (dist2ZoneEntry / currentSpeed);

    ownshipArrivalData.aircraftID = vehicleID;
    ownshipArrivalData.intersectionID = mergeFixId[currentFixIndex];
    ownshipArrivalData.earlyArrivalTime = (time2ZoneEntry + earlyTime);
    ownshipArrivalData.lateArrivalTime = (time2ZoneEntry + lateTime);
    ownshipArrivalData.currentArrivalTime = (time2ZoneEntry + time2PassInt);
    ownshipArrivalData.zoneStatus = currentZone;

    // If there are vehicles that have already passed into the entry zone,
    // earlyArrivalTime must be the latest early arrivaltime of all the vehicles within 
    // entry zone
    double revisedTime = ReviseEarlyArrTimeInEntryZone(); 

    if(revisedTime > 0){
        ownshipArrivalData.earlyArrivalTime = revisedTime;
    }
}

uint32_t Merger::ReviseEarlyArrTimeInEntryZone(){

    // Check the log to see if there are vehicles in the entry zone. 
    // If so, set early arrival time to there current arrival time + separation time.
    double revisedEarlyArrivalTime = 0;
    for(int i=0;i<numNodesInt;++i){
        if(globalArrivalData[i].zoneStatus == ENTRY_ZONE){
            if(revisedEarlyArrivalTime < globalArrivalData[i].currentArrivalTime){
                revisedEarlyArrivalTime = globalArrivalData[i].currentArrivalTime;
            }
        }
    }

    // Add the minSeparation time to the latest current arrival time
    if(revisedEarlyArrivalTime > 0)
        revisedEarlyArrivalTime += separationTime;

    return revisedEarlyArrivalTime;
}

void Merger::ExchangeArrivalTimes(){
    mergingData_t arrivalData;
    arrivalData.aircraftID = vehicleID;
    arrivalData.intersectionID = mergeFixId[currentFixIndex];
    arrivalData.numSchedulesComputed = numSchedulesComputed;
    arrivalData.earlyArrivalTime = ownshipArrivalData.earlyArrivalTime; 
    arrivalData.lateArrivalTime = ownshipArrivalData.lateArrivalTime; 
    arrivalData.currentArrivalTime = ownshipArrivalData.currentArrivalTime; 
    arrivalData.zoneStatus = currentZone;
    
    
    // Publish data if there is no one in the intersection
    bool send = false;
    if(numNodesInt == 0){
        send = true;
    }

    //Publish data if there is significant difference between new value and old value
    //NOTE: significant difference here is defined by variable sigdef
    uint8_t sigdef = 2;
    if (abs(arrivalData.currentArrivalTime - globalArrivalData[vehicleID].currentArrivalTime) >= sigdef) 
    {
        send = true;
    }

    // Publish data if there is a change in the zone status 
    for (int i = 0; i < numNodesInt; ++i)
    {
        if(globalArrivalData[i].aircraftID == vehicleID){
            if (arrivalData.zoneStatus != globalArrivalData[i].zoneStatus)
            {
               send = true;
            }
            break;
        }
    }

    if(send){
        fprintf(logFile1,"<<<<<<<<<< Publishing data >>>>>>>>>>\n");
        fprintf(logFile1,"Time: %.5f\n",currentLocalTime);
        fprintf(logFile1,"Num Schedules: %d\n",numSchedulesComputed);
        fprintf(logFile1,"Zone status: %d\n",currentZone);
        fprintf(logFile1,"Intersection: %d data\n", mergeFixId[currentFixIndex]);
        fprintf(logFile1,"Timing data : %f,%f,%f\n", arrivalData.earlyArrivalTime,
                arrivalData.currentArrivalTime,
                arrivalData.lateArrivalTime);
        fprintf(logFile1,"=====================================\n");
        outputArrData.push(arrivalData);
    }
}

bool Merger::CheckIntersectionConflict(void)
{

    bool conflict = false;
    int numAircraft = numNodesInt;
    double revisedEarlyArrivalTime = ReviseEarlyArrTimeInEntryZone();

    fprintf(logFile1, "In computation zone with %d aircrafts\n", numAircraft);
    for (uint8_t i = 0; i < MERGER_MAXAIRCRAFT; ++i)
    {
        if (globalArrivalData[i].aircraftID == -1 || globalArrivalData[i].zoneStatus == COORD_ZONE)
        {
            Release[i] = 0;
            Deadline[i] = 0;
            arrivalTime[i] = 0;
            continue;
        }
        Release[i] = globalArrivalData[i].earlyArrivalTime;
        Deadline[i] = globalArrivalData[i].lateArrivalTime;
        arrivalTime[i] = globalArrivalData[i].currentArrivalTime;

        if(globalArrivalData[i].zoneStatus != ENTRY_ZONE && revisedEarlyArrivalTime > 0){
            Release[i] = revisedEarlyArrivalTime;
        }
    }


    for (int i = 0; i < MERGER_MAXAIRCRAFT - 1; ++i)
    {
        for (int j = i + 1; j < MERGER_MAXAIRCRAFT; ++j)
        {
            double t1 = arrivalTime[i];
            double t2 = arrivalTime[j];

            if(t1 < 1e-3 || t2 < 1e-3){
                continue;
            }
            fprintf(logFile1,"Arrival times: %f, %f\n",t1,t2);
            fprintf(logFile1, "Diff time: %f, min separation time: %f\n", fabs(t1 - t2), separationTime);
            if (fabs(t1 - t2) < separationTime)
            {
                conflict = true;
                break;
            }
        }
    }
    mergeConflict = conflict;
    return conflict;
}

void Merger::ComputeSchedule(){
    int numAircraft = numNodesInt;
    double *gRelease = Release;
    double *gDeadline = Deadline;
    double ScheduledTime[MERGER_MAXAIRCRAFT];
    uint8_t ScheduledIndex[MERGER_MAXAIRCRAFT];
    double sepTime = separationTime;
    fprintf(logFile1, "Conflict detected\n");

    double Release[MERGER_MAXAIRCRAFT];
    double Deadline[MERGER_MAXAIRCRAFT];

    int index = 0;
    uint32_t aircraftIndex[MERGER_MAXAIRCRAFT];
    for(int i=0;i<MERGER_MAXAIRCRAFT;++i){
        if(gRelease[i] > 0){
            aircraftIndex[index] = globalArrivalData[i].aircraftID;
            Release[index] = gRelease[i]/sepTime;
            // NOTE: We add 1 to make sure that it's okay to schedule a job at the deadline.
            // This should be 1 instead of sepTime because R,D values are normalized by the sepTime
            Deadline[index] = gDeadline[i]/sepTime + 1; 
            index++;
        }
    }

    bool schStatus = Scheduler(index, Release, Deadline, ScheduledTime, ScheduledIndex);
    double newArrivalTime = 0;
    double myArrivalTime = 0;
    if (schStatus)
    {
        numSchedulesComputed++;
        for (int i = 0; i < numAircraft; ++i)
        {

            uint32_t _si = ScheduledIndex[i];
            uint32_t _i = aircraftIndex[_si]; 
            newArrivalTime = (ScheduledTime[_si] * separationTime);
            arrivalDataUpdated = true;
            fprintf(logFile1,"scheduled job %d : %d\n",i,ScheduledIndex[i]);
            fprintf(logFile1,"scheduled time %f\n",ScheduledTime[_si]);
            if (aircraftIndex[ScheduledIndex[i]] == vehicleID)
            {
                myArrivalTime = newArrivalTime;
                ownshipArrivalData.currentArrivalTime = myArrivalTime;
                fprintf(logFile1,"scheduled arrival time: %f\n",myArrivalTime);
            }
        }
        double waypoint[3] = {0, 0, 0};
        memcpy(newWaypoint, waypoint, sizeof(double) * 3);
        ComputeMergingSpeed(myArrivalTime);
    }
    else
    {
        mergingSpeed = 0.0;
        fprintf(logFile1, "##$$** No schedule found ##$$**\n");
    }
}

bool Merger::ComputeMergingSpeed(uint32_t arrivalTime)
{
    bool status = true;
    double resSpeed = -1, dev = 0;
    double intersectionLoc[3]; 
    memcpy(intersectionLoc,currentIntersection,sizeof(double)*3);

    double eta = arrivalTime - time2ZoneEntry;

    double step = 0;
    int N;
    if (corridorWidth < 1e-3) {
        N = 1;
    }else{
        N = (int)(corridorWidth / step);
    }
    double d = entryZone; 
    double xc1 = 0; 
    double xc3 = corridorWidth;
    resSpeed = currentSpeed;
    fprintf(logFile1, "eta: %f\n", eta);

    for (int i = 0; i < N; ++i)
    {
        xc3 = i * step;

        double nfac1 = sqrt(pow(xc1, 2) + pow(xc3, 2));
        double nfac2 = sqrt(pow(d - xc1, 2) + pow(xc3, 2));
        double vres = (nfac1 + nfac2) / eta;
        fprintf(logFile1, "iteration: %d,nfac1: %f,nfac2: %f\n", i, nfac1, nfac2);
        if ((minSpeed <= vres) &&
            (maxSpeed >= vres))
        {
            resSpeed = vres;
            dev = xc3;
            break;
        }
        else if (vres < minSpeed)
        {
            resSpeed = minSpeed;
            status = false;
            fprintf(logFile1, "min vehicle speed reached\n");
        }
        else if (vres > maxSpeed)
        {
            dev = 0;
            status = false;
            resSpeed = maxSpeed;
            fprintf(logFile1, "max vehicle speed exceeded by %f\n. Check R,D computation",
                    vres - maxSpeed);
        }
    }

    mergingSpeed = resSpeed;
    XtrkDev = dev;
    return status;
}

void Merger::ComputeTrajectory(double xc1,double xc3){
    double intersectionLoc[3]; 
    memcpy(intersectionLoc,currentIntersection,sizeof(double)*3);
    if (XtrkDev > 0)
    {
        
        double headingToIntersection = ComputeHeading(position, intersectionLoc);
        double headingPerp = ((int)headingToIntersection + 270) % 360;
        double tempPosition[3];
        double devWaypoint[3];
        ComputeOffsetPosition(entryPoint, headingPerp, xc3, tempPosition);
        ComputeOffsetPosition(tempPosition, headingToIntersection, xc1, devWaypoint);
        memcpy(newWaypoint, devWaypoint, sizeof(double) * 3);
        double detourDist1 = ComputeDistance(entryPoint, newWaypoint);
        double detourDist2 = ComputeDistance(newWaypoint, intersectionLoc);
        detourDist = detourDist1 + detourDist2; 
        defaultEntryPlan = false;
        fprintf(logFile1, "** New waypoint computed %f,%f **\n", newWaypoint[0], newWaypoint[1]);
        fprintf(logFile1, "** Deviation  = %f\n", XtrkDev);

        defaultEntryPlan = true;
        double diffTime1,diffTime2;
        diffTime1 = detourDist1/mergingSpeed;
        diffTime2 = detourDist2/mergingSpeed;
        flightplan[0][0]= entryPoint[0];
        flightplan[0][1]= entryPoint[1];
        flightplan[0][2]= entryPoint[2];
        flightplan[0][3]= ownshipArrivalData.currentArrivalTime - diffTime1 - diffTime2;

        flightplan[1][0]= newWaypoint[0];
        flightplan[1][1]= newWaypoint[1];
        flightplan[1][2]= newWaypoint[2];
        flightplan[1][3]= ownshipArrivalData.currentArrivalTime - diffTime2;

        flightplan[2][0]= newWaypoint[0];
        flightplan[2][1]= newWaypoint[1];
        flightplan[2][2]= newWaypoint[2];
        flightplan[2][3]= ownshipArrivalData.currentArrivalTime - diffTime2;
    }
    else
    {

        double diffTime = entryZone/mergingSpeed;
        defaultEntryPlan = true;

        flightplan[0][0]= entryPoint[0];
        flightplan[0][1]= entryPoint[1];
        flightplan[0][2]= entryPoint[2];
        flightplan[0][3]= ownshipArrivalData.currentArrivalTime - diffTime;

        flightplan[1][0]= intersectionLoc[0];
        flightplan[1][1]= intersectionLoc[1];
        flightplan[1][2]= intersectionLoc[2];
        flightplan[1][3]= ownshipArrivalData.currentArrivalTime;
    }
}

void Merger::ExecuteNewPath()
{
    switch (navState)
    {
        case _MERGER_NOOP_:
        {
            navState = _MERGER_VECTOR_CONTROL_;
            break;
        }

        case _MERGER_VECTOR_CONTROL_:
        {
            mergingStatus = MERGING_GUIDANCE_ACTIVE;
            if (mergingSpeed < 0){
               mergingSpeed = currentSpeed; 
            }
            double captureH = mergingSpeed * 3;
            double refHeading = ComputeHeading(position,mergeFixes[currentFixIndex]);

            double dist2NextNavWP = ComputeDistance(position, mergeFixes[currentFixIndex]);

            //OS_printf("distance to capture: %f\n",dist2NextNavWP);
            if (dist2NextNavWP < captureH)
            {
        
                navState = _MERGER_GIVECONTROL_;
                //OS_printf("Give control back\n");
                fprintf(logFile1,"Give control back\n");
                //OS_printf("give back control\n");
            }else{

                // difference between planned and current time of arrival
                int32_t timeDelta = ownshipArrivalData.currentArrivalTime - 
                                    (currentLocalTime + dist2Int/currentSpeed);

                double speedDelta = -0.25*timeDelta;

                refSpeed = (currentSpeed + speedDelta);

                if(refSpeed > maxSpeed){
                    refSpeed = maxSpeed;
                }else if(refSpeed < minSpeed){
                    refSpeed = minSpeed;
                }

                double vSpeed = 0;
                cmdOutput[0] = refHeading;
                cmdOutput[1] = refSpeed;
                cmdOutput[2] = vSpeed;
                //OS_printf("merging speed, refspeed: %f,%f\n",mergerAppData.mergingSpeed,refSpeed);
            }
            //OS_printf("In transition %f\n",dist2NextNavWP);

            break;
        }

        case _MERGER_GIVECONTROL_:{
            mergingStatus = MERGING_GUIDANCE_INACTIVE;
            navState = _MERGER_IDLE_;
            break;
        }

        case _MERGER_IDLE_:break;
    }
}

void Merger::AddLogEntry(){
    // LOG DATA for Analysis
    // Populate log entries

    int fixid = -1;
    if ((int) currentFixIndex >= 0){
        fixid = (int) mergeFixId[currentFixIndex]; 
    }

    fprintf(logFile2,"%f, %d, %f, %f, %u, (%f, %f, %f), %u, %u, %f, %f, %f, %u, %f, %f, %f\n",
                      currentLocalTime,
                      fixid,
                      dist2Int,
                      currentSpeed,
                      raftRole,
                      ownshipArrivalData.earlyArrivalTime,
                      ownshipArrivalData.currentArrivalTime,
                      ownshipArrivalData.lateArrivalTime,
                      currentZone,
                      numSchedulesComputed,
                      mergingSpeed,
                      refSpeed,
                      XtrkDev,
                      mergingStatus,
                      position[0],
                      position[1],
                      position[2]);
    return;
}

//functions to interface with C

void* MergerInit(char callsign[],char config[],int vehicleID){
    Merger* mg = new Merger(std::string(callsign),std::string(config),vehicleID);
    return (void*) mg;
}

void MergerReadParamFromFile(void* obj,char config[]){
    Merger* mg = (Merger*)obj;
    mg->ReadParamFromFile(std::string(config));
}

void  MergerDeinit(void *obj){
    delete((Merger*)obj);
}

void  MergerSetAircraftState(void* obj, double pos[],double vel[]){
    Merger* mg = (Merger*)obj;
    mg->SetVehicleState(pos,vel);
}

void  MergerSetVehicleConstraints(void* obj, double minVel, double maxVel, double turnRadius){
    Merger* mg = (Merger*)obj;
    mg->SetVehicleConstraints(minVel,maxVel,turnRadius);
}

void  MergerSetFixParams(void *obj, double separationTime,double coordzone,double schzone,double entryzone,double corridorWidth){
    Merger* mg = (Merger*)obj;
    mg->SetMergefixParams(separationTime,coordzone,schzone,entryzone,corridorWidth);
}

void  MergerSetIntersectionData(void *obj, int i,int id, double position[]){
    Merger* mg = (Merger*)obj;
    mg->SetIntersectionData(i,id,position);
}

void MergerSetNodeLog(void* obj,dataLog_t* mgData){
    Merger* mg = (Merger*)obj;
    mg->SetNodeLog(mgData);
}

unsigned char  MergerRun(void *obj,double time){
    Merger* mg = (Merger*)obj;
    return mg->RunMergingOperation(time);
}

void  MergerOutputVelocity(void *obj, double* trk,double* gs,double* vs){
    Merger* mg = (Merger*)obj;
    mg->GetOutputVelocity(trk,gs,vs);
}

int MergerOutputTrajectory(void* obj, int i, double wp[4]){
    Merger* mg = (Merger*)obj;
    return mg->GetOutputTrajectory(i,wp);
}

bool MergerGetArrivalTimes(void* obj,mergingData_t* mgData){
    Merger *mg = (Merger*) obj;
    return mg->GetArrivalTimes(mgData);
}