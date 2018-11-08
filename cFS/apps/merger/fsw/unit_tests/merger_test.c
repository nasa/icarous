//
// Created by Swee Balachandran on 7/22/18.
//


#include "merger_test.h"
#include <merger_table.h>
#include <osapi.h>
#include "UtilFunctions.h"

void Test_Setup(){

    Ut_CFE_EVS_Reset();
    Ut_CFE_FS_Reset();
    Ut_CFE_TIME_Reset();
    Ut_CFE_TBL_Reset();
    Ut_CFE_SB_Reset();
    Ut_CFE_ES_Reset();
    Ut_OSAPI_Reset();
    Ut_OSFILEAPI_Reset();
}

void InitializeData(){

    mergerAppData.mergerTable.IntersectionLocation[0][0] =  37.074752;
    mergerAppData.mergerTable.IntersectionLocation[0][1] = -76.466682 ;
    mergerAppData.mergerTable.IntersectionLocation[0][2] = 0;
    CFE_PSP_MemCpy(mergerAppData.map,mergerAppData.mergerTable.IntersectionLocation,sizeof(double)*15);
    mergerAppData.mergerTable.minSeparationTime = 5;
    mergerAppData.mergerTable.maxVehicleSpeed = 12;
    mergerAppData.mergerTable.minVehicleSpeed = 8;
    mergerAppData.mergerTable.corridorWidth = 100;
    mergerAppData.mergerTable.intersectionEntryRadius = 200;
    mergerAppData.mergerTable.maxVehicleTurnRadius = 10;
    mergerAppData.position[0] =  37.066593;
    mergerAppData.position[1] = -76.474022;
    mergerAppData.position[2] = 0;
    mergerAppData.velocity[0] = 8.109;
    mergerAppData.velocity[1] = 5.85;
    mergerAppData.velocity[2] = 0;
    mergerAppData.defaultEntryPlan = true;

    logEntry.index = 0;
    logEntry.entries = 3;

    time_t localTime = time(NULL);

    logEntry.log_ud_acc[0].intersectionID = 0;
    logEntry.log_ud_acc[0].aircraftID = 42;


    logEntry.log_ud_acc[0].currentArrivalTime = localTime+ 100;
    logEntry.log_ud_acc[0].earlyArrivalTime = localTime - 50;
    logEntry.log_ud_acc[0].lateArrivalTime = localTime+ 200;

    printf("current arrival time: %u\n",(localTime + 100));
    printf("current time: %u\n",(localTime));

    logEntry.log_ud_acc[1].intersectionID = 0;
    logEntry.log_ud_acc[1].aircraftID = 10;
    logEntry.log_ud_acc[1].currentArrivalTime = localTime + 102;
    logEntry.log_ud_acc[1].earlyArrivalTime = localTime - 50;
    logEntry.log_ud_acc[1].lateArrivalTime = localTime + 200;

    logEntry.log_ud_acc[2].intersectionID = 0;
    logEntry.log_ud_acc[2].aircraftID = 20;
    logEntry.log_ud_acc[2].currentArrivalTime = localTime + 103;
    logEntry.log_ud_acc[2].earlyArrivalTime = localTime - 50;
    logEntry.log_ud_acc[2].lateArrivalTime = localTime + 200;
}


void Test_ComputeEntryPoint(){

    InitializeData();

    double intersectionLoc[3] = {mergerAppData.mergerTable.IntersectionLocation[0][0],
                                 mergerAppData.mergerTable.IntersectionLocation[0][1],
                                 mergerAppData.mergerTable.IntersectionLocation[0][2]};
    ComputeEntryPoint();
    double dist = ComputeDistance(mergerAppData.entryPoint,intersectionLoc);
    printf("%f\n",dist);
    UtAssert_True (fabs(dist - mergerAppData.mergerTable.intersectionEntryRadius) < 1e-3 , "Entry point is 200 m away");
}

void Test_ComputeArrivalData(){

    InitializeData();
    ComputeEntryPoint();
    ComputeArrivalData();

    printf("early entry time: %u\n",mergerAppData.arrivalData.earlyArrivalTime);
    printf("current arrival time: %u\n",mergerAppData.arrivalData.currentArrivalTime);
    printf("late arrival time: %u\n",mergerAppData.arrivalData.lateArrivalTime);

    double timeA = mergerAppData.arrivalData.earlyArrivalTime;
    double timeB = mergerAppData.arrivalData.currentArrivalTime;
    double timeC = mergerAppData.arrivalData.lateArrivalTime;

    UtAssert_True( (timeA < timeB) && (timeB < timeC),"early time > current time > late time");
}

void Test_CheckIntersectionConflict(){
    InitializeData();
    ComputeEntryPoint();
    ComputeArrivalData();

    uint32_t ScheduledTime[MERGER_MAXAIRCRAFT];
    uint8_t ScheduledIndex[MERGER_MAXAIRCRAFT];
    double dev;
    double speed;

    logEntry.log_ud_acc[0].earlyArrivalTime = mergerAppData.arrivalData.earlyArrivalTime;
    logEntry.log_ud_acc[0].currentArrivalTime = mergerAppData.arrivalData.currentArrivalTime;
    logEntry.log_ud_acc[0].lateArrivalTime = mergerAppData.arrivalData.lateArrivalTime;

    logEntry.log_ud_acc[1].earlyArrivalTime = mergerAppData.arrivalData.earlyArrivalTime + 3;
    logEntry.log_ud_acc[1].currentArrivalTime = mergerAppData.arrivalData.currentArrivalTime + 3+1;
    logEntry.log_ud_acc[1].lateArrivalTime = mergerAppData.arrivalData.lateArrivalTime + 3;

    logEntry.log_ud_acc[2].earlyArrivalTime = mergerAppData.arrivalData.earlyArrivalTime;
    logEntry.log_ud_acc[2].currentArrivalTime = mergerAppData.arrivalData.currentArrivalTime;
    logEntry.log_ud_acc[2].lateArrivalTime = mergerAppData.arrivalData.lateArrivalTime;

    bool conflict =  CheckIntersectionConflict(&logEntry,ScheduledTime,ScheduledIndex,&speed,&dev);

    printf("Normalized schedule time: %u , %u, %u\n",ScheduledTime[0],ScheduledTime[1],ScheduledTime[2]);
    printf("Job index: %d , %d, %d\n",ScheduledIndex[0],ScheduledIndex[1],ScheduledIndex[2]);
    printf("resolution speed: %f\n",speed);
    printf("resolution dev: %f\n",dev);
    printf("%f %f\n",mergerAppData.newWaypoint[0],mergerAppData.newWaypoint[1]);

    UtAssert_True(conflict == true,"Conflict present");


}

void Test_TearDown(){


}

extern uint32 CFE_PSP_GetSpacecraftId(void){
    return 42;
}


