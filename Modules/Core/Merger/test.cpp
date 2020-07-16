#include "scheduler.h"
#include <stdio.h>
int main(int argc, char** argv){
    //1587392251.128104, 1587392258.796560, 1587392378.149326
    //1587392252.442253, 1587392261.581135, 1587392379.463475 
    double sepTime = 20;
    double Release[2]  = {1587394148.127613/sepTime,1587394148.167289/sepTime}; 
    double Deadline[2] = {1587394275.148835/sepTime + 1, 1587394275.188510/sepTime + 1};
    double scheduledTime[2] = {0,0};
    uint8_t scheduledIndex[2] = {0,0};

    Scheduler(2,Release,Deadline,scheduledTime,scheduledIndex);
    int i, j;
    i = scheduledIndex[0];
    j = scheduledIndex[1];
    printf("job %d, time: %f\n",i,scheduledTime[i] * sepTime);
    printf("job %d, time: %f\n",j,scheduledTime[j] * sepTime);

    return 1;
}
