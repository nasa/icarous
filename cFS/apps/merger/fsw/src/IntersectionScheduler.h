//
// Created by Swee Balachandran on 7/17/18.
//

#ifndef ICAROUS_CFS_INTERSECTIONSCHEDULER_H
#define ICAROUS_CFS_INTERSECTIONSCHEDULER_H

#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <merger.h>
#include <msgdef/merger_msg.h>

#define MERGER_MAXAIRCRAFT 10
#define MAX_RADIUS 100
#define SEPARATION_TIME 5
#define MAX_TRAJDEV 15
#define DEVSTEP 10


/**
 * Scheduling algorithm and relevant helper functions
 */
bool Scheduler(int n,uint32_t *R,uint32_t *D,uint32_t * ScheduledT,uint8_t* ScheduledJobs);

uint32_t  within(int n,uint32_t x,uint32_t F[][2],bool closed,bool min);

void swapD(uint32_t * a, uint32_t * b);

void swapI(uint8_t * a, uint8_t * b);

int partition (int low, int high,uint32_t  arrA[],uint32_t  arrB[],uint8_t arrC[]);

void quickSort( int low, int high,uint32_t  arrA[],uint32_t  arrB[],uint8_t arrC[]);

uint8_t LeastDeadlineAtT(int n,uint32_t *R,uint32_t *D,uint8_t* id,uint32_t  t, uint8_t* ScheduledJobs,int numSchJobs);

void ComputeArrivalData(void);

double ComputePathLength(void);

bool CheckIntersectionConflict(void);

void ComputeEntryPoint(void);

bool ComputeNewTrajectory(uint32_t arrivalTime);

void ExecuteNewPath(void);

#endif //ICAROUS_CFS_INTERSECTIONSCHEDULER_H
