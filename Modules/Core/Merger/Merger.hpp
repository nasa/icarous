#ifndef MERGER_HPP
#define MERGER_HPP

#include <string>
#include <queue>

#include <float.h>
#include <string.h>
#include <math.h>

#include "UtilFunctions.h"
#include "scheduler.h"
#include "Merger.h"

class Merger{
   public:
        Merger(std::string callsign,std::string config,int vehicleID);

        void ReadParamFromFile(std::string config);

        void SetVehicleConstraints(double minVel, double maxVel, double turnRadius);

        void SetMergefixParams(double separationTime,double coordzone,double schzone,double entryzone,double corridorWidth);

        void SetVehicleState(double position[],double velocity[]);

        void SetIntersectionData(int i,int id, double position[]);

        unsigned char RunMergingOperation(double time);

        void SetNodeLog(dataLog_t* mgData);

        int GetOutputTrajectory(int i, double wp[4]);

        void GetOutputVelocity(double* trk,double* gs,double* vs);

        bool GetArrivalTimes(mergingData_t* arrData);

   private:
        double GetApproachPrecision(double position[],double velocity[], double intersection[]);

        void ComputeArrivalData(void);

        double ComputePathLength(void);

        bool CheckIntersectionConflict(void);

        void CheckIntersectionExit(void);

        void ComputeSchedule();

        bool ComputeMergingSpeed(uint32_t arrivalTime);

        void ComputeTrajectory(double xc1,double xc2);

        void ExecuteNewPath(void);

        void ExchangeArrivalTimes(void);

        int32_t FindCurrentIntersection(void);

        uint32_t ReviseEarlyArrTimeInEntryZone(void);

        zoneType_e IdentifyCurrentZone();

        void AddLogEntry();

        void ResetData();


   private:
        FILE* logFile1;                                       ///< Log file descriptor
        FILE* logFile2;                                       ///< Log file descriptor

        std::string callSign;                                 ///< Vehicle call sign string
        unsigned int vehicleID;                               ///< Numerial identifier
        double mergeFixes[INTERSECTION_MAX][3];               ///< merge fixes
        unsigned int mergeFixId[INTERSECTION_MAX];            ///< Identifiers of merge fixes
        unsigned int currentFixIndex;                         ///< Current fix index
        unsigned int totalFixes;                              ///< Total number of merge fixes
        double position[3];                                   ///< Current position
        double velocity[3];                                   ///< Current velocity
        double currentSpeed;                                  ///< Current speed
        double currentIntersection[3];                        ///< Current intersection
        double dist2Int;                                      ///< Distance to intersection
        unsigned int raftRole;                                ///< Raft node role 
        double currentLocalTime;

        double coordZone;                                     ///< Coordination zone size
        double schedZone;                                     ///< Schedule zone size
        double entryZone;                                     ///< Entry zone size
        double corridorWidth;                                 ///< Corridor width at the intersection
        double separationTime;                                ///< Minimum separation time
        zoneType_e currentZone;                               ///< Current zone

        double turnRadius;
        double maxSpeed;
        double minSpeed;


        bool publishMergingStatus;                            ///< Merging status publish notification
        bool arrivalDataUpdated;                              ///< true if new arrival data exists
        bool defaultEntryPlan;
        bool entryPointComputed;
        bool executePath;
        bool inComputeZone;
        bool approaching;
        bool mergeConflict;

        int numSchedulesComputed;                              ///< Number of times schedule has been run
        unsigned char waypointIndex;
        unsigned char mergingStatus;
        unsigned char nodeRole;
        bool passive;

        double dist2ZoneEntry;                                 ///< Distance to entry point
        double time2ZoneEntry;                                 ///< Time to entry
        double entryPoint[3];                                  ///< Entry point
        mergingData_t ownshipArrivalData;                      ///< arrival data
        mergingData_t globalArrivalData[MERGER_MAXAIRCRAFT];   ///< Intersection entry data
        std::queue<mergingData_t> outputArrData;

        double Release[MERGER_MAXAIRCRAFT];
        double Deadline[MERGER_MAXAIRCRAFT];
        double arrivalTime[MERGER_MAXAIRCRAFT];

        int numNodesInt;
        double newWaypoint[3];                                  ///< New detour waypoint
        double mergingSpeed;
        double detourDist;
        double XtrkDev;
        double nextNavWaypoint[3];
        double dist2NextWaypoint;
        double speed2NextWaypoint;
        double refSpeed;
        double flightplan[3][4];
        double cmdOutput[3];
        localMergerNavStates_e navState;
};



#endif