#ifndef MERGER_H
#define MERGER_H

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_NODES 10 
#define INTERSECTION_MAX 5
#define MERGER_MAXAIRCRAFT MAX_NODES

typedef enum{
    _MERGER_NOOP_,
    _MERGER_IDLE_,
    _MERGER_VECTOR_CONTROL_,
    _MERGER_GIVECONTROL_
}localMergerNavStates_e;

typedef enum{
   NULL_ZONE,
   COORD_ZONE,
   SCHED_ZONE,
   ENTRY_ZONE
}zoneType_e;

typedef enum{
   MERGING_INACTIVE,
   MERGING_ACTIVE,
   MERGING_GUIDANCE_INACTIVE,
   MERGING_GUIDANCE_ACTIVE
}mergeStatus_e;


/**
 * @struct mergingData_t
 * @brief data to calculate schedule
 */
typedef struct __attribute__((__packed__)){
   int8_t aircraftID;           /**< Aircraft ID */
   int8_t intersectionID;       /**< Current intersection for which scheduling is required */
   double earlyArrivalTime;     /**< Earliest a/c can reach the intersection (using max speed) */
   double currentArrivalTime;   /**< Current arrival time  (using current speed)*/
   double lateArrivalTime;      /**< Late arrival time (using slowest speed + allowed deviations) */
   int32_t numSchedulesComputed;/**< Number of times scheduler has been run */
   uint8_t zoneStatus;          /**< zone status */
}mergingData_t;

typedef struct __attribute__((__packed__)){
   int32_t intersectionID;                  /**< Intersection ID for the current raft network */
   uint8_t nodeRole;                        /**< Raft node status of current vehicle */
   uint32_t totalNodes;                     /**< Total number of nodes in the network */
   mergingData_t log[MAX_NODES];
}dataLog_t;


void* MergerInit(char callsign[],int vehicleID);
void  MergerDeinit(void *obj);
void  MergerSetAircraftState(void* obj, double pos[],double vel[]);
void  MergerSetVehicleConstraints(void* obj, double minVel, double maxVel, double turnRadius);
void  MergerSetFixParams(void *obj, double separationTime,double coordzone,double schzone,double entryzone,double corridorWidth);
void  MergerSetIntersectionData(void *obj, int i,int id, double position[]);
void  MergerSetNodeLog(void* obj,dataLog_t* mgData);
unsigned char  MergerRun(void *obj,double time);
void  MergerOutputVelocity(void* obj,double* trk,double* gs,double* vs);
int   MergerOutputTrajectory(void* obj,int i, double wp[4]);
bool MergerGetArrivalTimes(void* obj,mergingData_t* mgData);


#ifdef __cplusplus
}
#endif

#endif
