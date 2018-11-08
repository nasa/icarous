//
// Created by Swee Balachandran on 7/17/18.
//

#include <UtilFunctions.h>
#include <cfe_time.h>
#include <osapi.h>
#include "IntersectionScheduler.h"
#include "msgdef/merger_msg.h"
#include "msgids/merger_msgids.h"
#include "merger.h"
#include "merger_table.h"
#include <time.h>
#include <unistd.h>
#include <msgdef/ardupilot_msg.h>

extern mergerAppData_t mergerAppData;

uint32_t  within(int n,uint32_t  x,uint32_t  F[][2],bool closed,bool min){

    if(closed){
        for(int i=0;i<n;i++){
            if ( (x >= F[i][0]) && (x <= F[i][1]) ){
                if(min){
                    return F[i][0]>F[i][1]?F[i][1]:F[i][0];
                }else{
                    return F[i][0]>F[i][1]?F[i][0]:F[i][1];
                }
            }
        }
    }else{
        for(int i=0;i<n;i++){
            if ( (x > F[i][0]) && (x < F[i][1]) ){
                if(min){
                    return F[i][0]>F[i][1]?F[i][1]:F[i][0];
                }else{
                    return F[i][0]>F[i][1]?F[i][0]:F[i][1];
                }
            }
        }
    }

    return INT32_MAX;
}

// A utility function to swap two elements
void swapD(uint32_t * a, uint32_t * b)
{
    uint32_t  t = *a;
    *a = *b;
    *b = t;
}

void swapI(uint8_t * a, uint8_t * b)
{
    uint8_t t = *a;
    *a = *b;
    *b = t;
}

/* This function takes last element as pivot, places
   the pivot element at its correct position in sorted
    array, and places all smaller (smaller than pivot)
   to left of pivot and all greater elements to right
   of pivot */
int partition (int low, int high,uint32_t arrA[],uint32_t arrB[],uint8_t arrC[])
{
    double pivot = arrA[high];    // pivot
    int i = (low - 1);         // Index of smaller element

    for (int j = low; j <= high- 1; j++)
    {
        // If current element is smaller than or
        // equal to pivot
        if (arrA[j] <= pivot)
        {
            i++;    // increment index of smaller element
            swapD(&arrA[i], &arrA[j]);
            swapD(&arrB[i],&arrB[j]);
            swapI(&arrC[i],&arrC[j]);
        }
    }
    swapD(&arrA[i + 1], &arrA[high]);
    swapD(&arrB[i + 1], &arrB[high]);
    swapI(&arrC[i + 1], &arrC[high]);
    return (i + 1);
}

/* The main function that implements QuickSort
 arr[] --> Array to be sorted,
  low  --> Starting index,
  high  --> Ending index */
void quickSort( int low, int high,uint32_t  arrA[],uint32_t  arrB[],uint8_t arrC[])
{
    if (low < high)
    {
        /* pi is partitioning index, arr[p] is now
           at right place */
        int pi = partition(low, high,arrA,arrB,arrC);

        // Separately sort elements before
        // partition and after partition
        quickSort(low, pi - 1,arrA,arrB,arrC);
        quickSort(pi + 1, high,arrA,arrB,arrC);
    }
}

uint8_t LeastDeadlineAtT(int n,uint32_t  *R,uint32_t  *D,uint8_t* id,uint32_t  t, uint8_t* ScheduledJobs,int numSchJobs){
    uint32_t newR[MERGER_MAXAIRCRAFT];
    uint32_t newD[MERGER_MAXAIRCRAFT];
    uint8_t newID[MERGER_MAXAIRCRAFT];

    int num = 0;
    for(int i=0;i<n;++i){
        if(R[i] <= t){
            newR[i] = R[i];
            newD[i] = D[i];
            newID[i] = id[i];
            num++;
        }
    }

    quickSort(0,num-1,newD,newR,newID);

    if(numSchJobs == 0)
        return newID[0];

    for(int i=0;i<num;++i){
        bool available = false;
        for(int j=0;j<numSchJobs;++j){
            if (newID[i] == ScheduledJobs[j]){
                available = true;
                break;
            }
        }

        if(!available)
            return newID[i];
    }
}

bool Scheduler(int n,uint32_t *R,uint32_t *D,uint32_t * ScheduledT,uint8_t* ScheduledJobs){
    uint32_t F[MERGER_MAXAIRCRAFT][2]={{0}};
    uint32_t c[MERGER_MAXAIRCRAFT];
    uint8_t id[MERGER_MAXAIRCRAFT];

    for(int i=0;i<n;++i){
        id[i] = (uint8_t)i;
        c[i] = INT32_MAX;
    }

    quickSort(0,n-1,R,D,id);

    for(int i=n-1;i>=0;i--){
        for (int j=0;j<n;j++){
            if(D[j] >= D[i]){
                if(c[j] == INT32_MAX){
                    c[j] = D[j] - 1;
                }else{
                    c[j] = c[j] - 1;
                }
            }

            uint32_t _c = 0;
            while (_c != INT32_MAX) {
                _c = within(n,c[j],F,false,true);
                if(_c != INT32_MAX)
                    c[j] = _c;
            }
        }

        if (i==0){
            uint32_t C = INT32_MAX;
            for(int j=0;j<n;++j){
                if (c[j] < C){
                    C = c[j];
                }
            }

            if(C < R[i]){
                return false;
            }

            uint32_t _c = 0;
            uint32_t _F[1][2] = {R[i],R[i] + 1};
            _c = within(1,C,_F,true,true);
            if(_c != INT32_MAX){
                F[i][0] = C-1;
                F[i][1] = R[i];
            }
        }else if(R[i-1] < R[i]){
            uint32_t C = INT32_MAX;
            for(int j=0;j<n;++j){
                if (c[j] < C){
                    C = c[j];
                }
            }

            if (C < R[i]){
                return false;
            }

            uint32_t _c = 0;
            uint32_t _F[1][2] = {R[i],R[i] + 1};
            _c = within(1,C,_F,true,true);
            if(_c != INT32_MAX){
                F[i][0] = C-1;
                F[i][1] = R[i];
            }
        }
    }

    uint32_t t = 0;

    for(int i=0;i<n;++i){
        ScheduledJobs[i] = INT32_MAX;
        ScheduledT[i] = 0;
    }

    for(int i=0;i<n;++i){
        uint32_t rmin = INT32_MAX;
        uint32_t RMIN[MERGER_MAXAIRCRAFT];
        int count = 0;

        for(int j=0;j<n;++j){
            bool in = false;
            for(int k=0;k<n;k++){
                if(id[j] == ScheduledJobs[k]){
                    in = true;
                }
            }
            if(!in){
                RMIN[count] = R[j];
                count++;
            }
        }

        for(int j=0;j<count;j++){
            if(RMIN[j] < rmin)
                rmin = RMIN[j];
        }

        t = t>rmin?t:rmin;
        uint32_t _c = 0;
        while(_c != INT32_MAX){
            _c = within(n,t,F,false,false);
            if(_c != INT32_MAX)
                t = _c;
        }

        uint8_t minDJobId = LeastDeadlineAtT(n,R,D,id,t,ScheduledJobs,i);
        ScheduledT[minDJobId] = t;
        t = t+1;
        ScheduledJobs[i] = minDJobId;
    }
}

void ComputeEntryPoint(){

    double intersectionLoc[3] = {mergerAppData.mergerTable.IntersectionLocation[0][0],
                                 mergerAppData.mergerTable.IntersectionLocation[0][1],
                                 mergerAppData.mergerTable.IntersectionLocation[0][2]};

    double headingToMyPos = ComputeHeading(intersectionLoc, mergerAppData.position);

    double entryPoint[3];
    ComputeOffsetPosition(intersectionLoc, headingToMyPos, mergerAppData.mergerTable.entryRadius,
                          entryPoint);
    memcpy(mergerAppData.entryPoint,entryPoint,sizeof(double)*3);


}

void ComputeArrivalData(){
    int id = mergerAppData.nextIntersectionId;
    double intersection[3] = {mergerAppData.mergerTable.IntersectionLocation[id][0],
                              mergerAppData.mergerTable.IntersectionLocation[id][1],
                              mergerAppData.mergerTable.IntersectionLocation[id][2]};
    double dist2int = ComputeDistance(intersection,mergerAppData.position);
    OS_printf("Distance to intersection: %f\n",dist2int);
    double diff = mergerAppData.mergerTable.coordZone - mergerAppData.mergerTable.entryRadius;

    if(dist2int > mergerAppData.mergerTable.coordZone )
        return;
    else if(dist2int > mergerAppData.mergerTable.entryRadius+0.4*diff && dist2int <= mergerAppData.mergerTable.coordZone) {
        double dist = 0;
        if (mergerAppData.defaultEntryPlan) {
            dist = mergerAppData.mergerTable.entryRadius;
        } else {
            dist = mergerAppData.detourDist; // Compute distance of entry flight plan
        }

        if (!mergerAppData.entryPointComputed){
            mergerAppData.entryPointComputed = true;
            ComputeEntryPoint();
            OS_printf("Entry point computed\n");
        }


        mergerAppData.dist2ZoneEntry = ComputeDistance(mergerAppData.position,mergerAppData.entryPoint);

        double currentSpeed = mergerAppData.currentSpeed;
        OS_time_t latchTime;
        CFE_PSP_GetTime(&latchTime);
        time_t nowtime = time(NULL);

        arrivalData_t arrivalData;
        CFE_SB_InitMsg(&arrivalData,ICAROUS_ARRIVALTIMES_MID, sizeof(arrivalData),TRUE);
        arrivalData.aircraftID = CFE_PSP_GetSpacecraftId();
        arrivalData.intersectionID = 0;


        double d = mergerAppData.mergerTable.entryRadius;//mergerAppData.mergerTable.corridorWidth;
        double  xc1 = mergerAppData.mergerTable.maxVehicleTurnRadius;
        double xc3 = mergerAppData.mergerTable.corridorWidth;

        double nfac1 = sqrt( pow(xc1,2) + pow(xc3,2));
        double nfac2 = sqrt( pow(d - xc1,2) + pow(xc3,2) );

        double earlyTime = dist / mergerAppData.mergerTable.maxVehicleSpeed;
        double lateTime = (nfac1+nfac2) / mergerAppData.mergerTable.minVehicleSpeed;
        double time2PassInt =  (dist / mergerAppData.currentSpeed);
        double time2ZoneEntry = nowtime +  (mergerAppData.dist2ZoneEntry / currentSpeed);

        //OS_printf("now time   : %lu\n",nowtime);
        //OS_printf("Timing data: %f,%f,%f,%f\n",earlyTime,lateTime,time2PassInt,time2ZoneEntry);
        //OS_printf("current speed: %f\n",currentSpeed);
        //OS_printf("dist 2 zone entry: %f\n",mergerAppData.dist2ZoneEntry);

        mergerAppData.time2ZoneEntry = (uint32_t) time2ZoneEntry;
        arrivalData.earlyArrivalTime =  (uint32_t)(time2ZoneEntry + earlyTime);
        arrivalData.lateArrivalTime = (uint32_t)(time2ZoneEntry + lateTime);
        arrivalData.currentArrivalTime = (uint32_t)(time2ZoneEntry + time2PassInt);


        if (abs(arrivalData.currentArrivalTime - mergerAppData.intersectionEntryData[CFE_PSP_GetSpacecraftId()].currentArrivalTime) > 5) {
            OS_printf("Timing data in uint32: %lu,%lu,%lu\n",arrivalData.earlyArrivalTime,arrivalData.lateArrivalTime,arrivalData.currentArrivalTime);
            memcpy(&mergerAppData.arrivalData, &arrivalData, sizeof(arrivalData));
            SendSBMsg(arrivalData);
        }

    }else{
        mergerAppData.inComputeZone = true;
        if(mergerAppData.arrivalDataUpdated){
            arrivalData_t arrivalData;
            CFE_SB_InitMsg(&arrivalData,ICAROUS_ARRIVALTIMES_MID, sizeof(arrivalData),TRUE);
            arrivalData.aircraftID = CFE_PSP_GetSpacecraftId();
            arrivalData.intersectionID = 0;
            arrivalData.earlyArrivalTime = mergerAppData.arrivalData.earlyArrivalTime;
            arrivalData.currentArrivalTime = mergerAppData.arrivalData.currentArrivalTime;
            arrivalData.lateArrivalTime = mergerAppData.arrivalData.lateArrivalTime;
            mergerAppData.arrivalDataUpdated = false;

            OS_printf("Publishing arrival times to leader after scheduling\n");
            OS_printf("Timing data in uint32: %lu,%lu,%lu\n",arrivalData.earlyArrivalTime,arrivalData.lateArrivalTime,arrivalData.currentArrivalTime);
        }
        return;
    }
}

bool CheckIntersectionConflict(void){


    int id = mergerAppData.nextIntersectionId;
    double intersection[3] = {mergerAppData.mergerTable.IntersectionLocation[id][0],
                              mergerAppData.mergerTable.IntersectionLocation[id][1],
                              mergerAppData.mergerTable.IntersectionLocation[id][2]};
    double dist2int = ComputeDistance(intersection,mergerAppData.position);
    double barrier1 = mergerAppData.mergerTable.coordZone;
    double barrier2 = mergerAppData.mergerTable.entryRadius;
    double diff = barrier1 - barrier2;
    bool conflict = false;
    if(dist2int <= barrier2 + 0.4*diff && dist2int >= barrier2 + 0.1*diff) {
        uint32_t Release[MERGER_MAXAIRCRAFT];
        uint32_t Deadline[MERGER_MAXAIRCRAFT];
        uint32_t arrivalTime[MERGER_MAXAIRCRAFT];
        uint8_t ownshipIndex = 255;
        int numAircraft = mergerAppData.numNodesInt;

        OS_printf("In computation zone with %d aircrafts\n",numAircraft);
        for (uint8_t i = 0; i < 10; ++i) {
            if(mergerAppData.aircraftIdInt[i] == -1){
                continue;
            }
            if (mergerAppData.intersectionEntryData[i].aircraftID == CFE_PSP_GetSpacecraftId())
                ownshipIndex = i;
            Release[i] = (uint32_t) (ceil((double)mergerAppData.intersectionEntryData[i].earlyArrivalTime /
                                          mergerAppData.mergerTable.minSeparationTime));
            Deadline[i] = (uint32_t) (
                    mergerAppData.intersectionEntryData[i].lateArrivalTime / mergerAppData.mergerTable.minSeparationTime + 1);
            arrivalTime[i] = mergerAppData.intersectionEntryData[i].currentArrivalTime;
        }

        for (int i = 0; i < numAircraft - 1; ++i) {
            for (int j = i + 1; j < numAircraft; ++j) {
                uint32_t t1 = arrivalTime[i];
                uint32_t t2 = arrivalTime[j];
                OS_printf("Diff time: %lu, min separation time: %f\n",abs(t1-t2),mergerAppData.mergerTable.minSeparationTime);
                if (abs(t1 - t2) < mergerAppData.mergerTable.minSeparationTime) {
                    conflict = true;
                    break;
                }
            }
        }

        uint32_t ScheduledTime[INTERSECTION_MAX];
        uint8_t ScheduledIndex[INTERSECTION_MAX];

        if (conflict) {
            OS_printf("Conflict detected\n");
            //OS_printf("Release times: %lu, %lu\n",Release[0],Release[1]);
            //OS_printf("Deadlines: %lu, %lu\n",Deadline[0],Deadline[1]);
            //OS_printf("Current entry: %lu,%lu\n",arrivalTime[0],arrivalTime[1]);
            bool schStatus = Scheduler(numAircraft, Release, Deadline, ScheduledTime, ScheduledIndex);
            //OS_printf("Scheduled time: %lu, %lu \n",ScheduledTime[0]*5,ScheduledTime[1]*5);
            uint32_t newArrivalTime = 0;
            uint32_t myArrivalTime = 0;
            if(schStatus){
                for (int i = 0; i < numAircraft; ++i) {
                    newArrivalTime = (uint32_t) (ScheduledTime[i] * (uint32_t)mergerAppData.mergerTable.minSeparationTime);
                    mergerAppData.intersectionEntryData[i].currentArrivalTime = newArrivalTime;
                    mergerAppData.arrivalData.currentArrivalTime = newArrivalTime;
                    mergerAppData.arrivalDataUpdated = true;
                    //OS_printf("Setting new arrival time %lu for aircraft: %d\n",mergerAppData.arrivalData.currentArrivalTime,i);
                    if(i == CFE_PSP_GetSpacecraftId()){
                        myArrivalTime = newArrivalTime;
                    }
                }
                double waypoint[3] = {0, 0, 0};
                memcpy(mergerAppData.newWaypoint, waypoint, sizeof(double) * 3);
                bool status = ComputeNewTrajectory(myArrivalTime);
            }
        }else{

        }

    }

    return conflict;
}

bool ComputeNewTrajectory(uint32_t arrivalTime){

    double resSpeed = -1, dev = 0;
    double intersectionLoc[3] = {mergerAppData.mergerTable.IntersectionLocation[0][0],
                                 mergerAppData.mergerTable.IntersectionLocation[0][1],
                                 mergerAppData.mergerTable.IntersectionLocation[0][2]};

    uint32_t eta = arrivalTime - mergerAppData.time2ZoneEntry;

    double step = mergerAppData.mergerTable.maxVehicleTurnRadius;
    int N = (int) (mergerAppData.mergerTable.corridorWidth/step);
    double d = mergerAppData.mergerTable.entryRadius;//mergerAppData.mergerTable.corridorWidth;
    double  xc1 = mergerAppData.mergerTable.maxVehicleTurnRadius;
    double xc3 = 0;
    resSpeed = mergerAppData.currentSpeed;
    OS_printf("eta: %d\n",eta);
    for(int i=0;i<N;++i){
        xc3 = i*step;

        double nfac1 = sqrt( pow(xc1,2) + pow(xc3,2));
        double nfac2 = sqrt( pow(d - xc1,2) + pow(xc3,2) );
        double vres = (nfac1 + nfac2)/eta;
        OS_printf("%d, %f, %f\n",i,nfac1,nfac2);
        if( (mergerAppData.mergerTable.minVehicleSpeed <= vres) &&
            (mergerAppData.mergerTable.maxVehicleSpeed >= vres) ){
            resSpeed = vres;
            dev = xc3;
            break;
        }else{
            dev = 0;
        }
    }

    mergerAppData.mergingSpeed = resSpeed;
    if(dev > 0) {
        double headingToIntersection = ComputeHeading(mergerAppData.position, intersectionLoc);
        double headingPerp = ((int) headingToIntersection + 270) % 360;
        double tempPosition[3];
        double devWaypoint[3];
        ComputeOffsetPosition(mergerAppData.entryPoint, headingPerp, xc3, tempPosition);
        ComputeOffsetPosition(tempPosition,headingToIntersection,xc1,devWaypoint);
        memcpy(mergerAppData.newWaypoint,devWaypoint,sizeof(double)*3);
        mergerAppData.detourDist = ComputeDistance(mergerAppData.entryPoint,mergerAppData.newWaypoint) +
                                   ComputeDistance(mergerAppData.newWaypoint,intersectionLoc);
        mergerAppData.defaultEntryPlan = false;
        OS_printf("** New waypoint computed %f,%f **\n",mergerAppData.newWaypoint[0],mergerAppData.newWaypoint[1]);
        OS_printf("** Deviation  = %f\n",dev);
        return true;
    }else{
        mergerAppData.defaultEntryPlan = true;
    }

    return false;
}

void ExecuteNewPath(){

    int id = mergerAppData.nextIntersectionId;
    double intersection[3] = {mergerAppData.mergerTable.IntersectionLocation[id][0],
                              mergerAppData.mergerTable.IntersectionLocation[id][1],
                              mergerAppData.mergerTable.IntersectionLocation[id][2]};
    double dist2int = ComputeDistance(intersection,mergerAppData.position);

    double barrier1 = mergerAppData.mergerTable.coordZone;
    double barrier2 = mergerAppData.mergerTable.entryRadius;
    double diff = barrier1 - barrier2;
    if(dist2int >= mergerAppData.mergerTable.entryRadius + 0.1*diff){
        return;
    }
    OS_printf("Executing new path\n");
    OS_printf("** New waypoint computed %f,%f **\n",mergerAppData.newWaypoint[0],mergerAppData.newWaypoint[1]);
    OS_printf("Merging speed: %f\n",mergerAppData.mergingSpeed);


    switch(mergerAppData.navState){
        case _MERGER_NOOP_:{
            mergerAppData.navState = _MERGER_TAKECONTROL_;
        }

        case _MERGER_TAKECONTROL_:{
            argsCmd_t command;
            CFE_SB_InitMsg(&command,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
            command.name = _SETMODE_;
            command.param1 = _ACTIVE_;
            SendSBMsg(command);
            mergerAppData.navState = _MERGER_SETNEWPOS_;
            mergerAppData.waypointIndex = 0;
            OS_printf("Sending mode change  command\n");
            usleep(500);
            break;
        }

        case _MERGER_SETNEWPOS_:{

            if(mergerAppData.waypointIndex == 0) {
                memcpy(mergerAppData.nextNavWaypoint,mergerAppData.entryPoint,sizeof(double)*3);
                mergerAppData.speed2NextWaypoint = mergerAppData.currentSpeed;
                OS_printf("Setting entry point as waypoint\n");
            }else if(mergerAppData.waypointIndex == 1 && !mergerAppData.defaultEntryPlan){
                memcpy(mergerAppData.nextNavWaypoint,mergerAppData.newWaypoint,sizeof(double)*3);
                mergerAppData.speed2NextWaypoint = mergerAppData.mergingSpeed;
                OS_printf("Setting dev point as waypoint\n");

            }else{
                memcpy(mergerAppData.nextNavWaypoint,intersection,sizeof(double)*3);
                mergerAppData.speed2NextWaypoint = mergerAppData.mergingSpeed;
                OS_printf("Setting intersection as waypoint %f,%f\n",intersection[0],intersection[1]);
            }

            // Set position
            argsCmd_t command1;
            CFE_SB_InitMsg(&command1,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
            command1.name = _SETPOS_;
            command1.param1 = mergerAppData.nextNavWaypoint[0];
            command1.param2 = mergerAppData.nextNavWaypoint[1];
            command1.param3 = mergerAppData.nextNavWaypoint[2];
            SendSBMsg(command1);

            OS_printf("New waypoint: %f,%f,%f\n",command1.param1,command1.param2,command1.param3);

            argsCmd_t command2;
            CFE_SB_InitMsg(&command2,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
            // Set speed;
            command2.name = _SETSPEED_;
            command2.param1 = (float)(mergerAppData.speed2NextWaypoint);
            SendSBMsg(command2);
            OS_printf("Sending and speed %f\n",command2.param1);
            mergerAppData.navState = _MERGER_TRANSITION_;

            break;
        }

        case _MERGER_TRANSITION_:{

            double dist2NextNavWP = ComputeDistance(mergerAppData.position,mergerAppData.nextNavWaypoint);

            double captureH = mergerAppData.currentSpeed*3;
            argsCmd_t command1;
            CFE_SB_InitMsg(&command1,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
            command1.name = _SETPOS_;
            command1.param1 = mergerAppData.nextNavWaypoint[0];
            command1.param2 = mergerAppData.nextNavWaypoint[1];
            command1.param3 = mergerAppData.nextNavWaypoint[2];
            SendSBMsg(command1);


            argsCmd_t command2;
            CFE_SB_InitMsg(&command2,ICAROUS_COMMANDS_MID, sizeof(argsCmd_t),TRUE);
            // Set speed;
            command2.name = _SETSPEED_;
            command2.param1 = (float)((int)(mergerAppData.speed2NextWaypoint));
            SendSBMsg(command2);
            mergerAppData.navState = _MERGER_TRANSITION_;

            if(dist2NextNavWP < captureH){
                mergerAppData.waypointIndex++;
                if(mergerAppData.defaultEntryPlan && mergerAppData.waypointIndex <= 2){
                    mergerAppData.navState = _MERGER_SETNEWPOS_;
                    OS_printf("Goto next waypoint\n");
                }
                else if(!mergerAppData.defaultEntryPlan && mergerAppData.waypointIndex <= 3) {
                    mergerAppData.navState = _MERGER_SETNEWPOS_;
                    OS_printf("Goto next waypoint\n");
                }
                else {
                    mergerAppData.navState = _MERGER_GIVECONTROL_;
                    OS_printf("Give control back\n");
                }
            }
            OS_printf("In transition %f\n",dist2NextNavWP);
            break;
        }

        case _MERGER_GIVECONTROL_:{
            argsCmd_t command;
            CFE_SB_InitMsg(&command,ICAROUS_COMMANDS_MID,sizeof(argsCmd_t),TRUE);
            command.name = _SETMODE_;
            command.param1 = _PASSIVE_;
            SendSBMsg(command);
            //mergerAppData.navState = _MERGER_NOOP_;
            mergerAppData.executePath = false;
            OS_printf("Switch to auto\n");
            break;
        }

        default:
            break;
    }
}
