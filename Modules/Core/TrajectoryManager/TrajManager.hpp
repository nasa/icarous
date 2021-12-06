/**
 * @file TrajManager.hpp
 * @brief Trajectory Management capabilities
 */
#ifndef TRAJMANAGER_H
#define TRAJMANAGER_H

#include "ParameterData.h"
#include "CDII.h"
#include "CDSI.h"
#include "CDIIPolygon.h"
#include "CDPolycarp.h"
#include "PolycarpDetection.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "DubinsPlanner.hpp"
#include "Interfaces.h"
#include <list>
#include <map>

/**
 * @struct pObject
 * @brief object data structure
 */
typedef struct{
    std::string callsign;      ///< callsign
    int id;                    ///< id
    double time;               ///< time stamp
    larcfm::Position position; ///< position
    larcfm::Velocity velocity; ///< velocity
}pObject;

/**
 * @struct fenceObject
 * @brief datastructure to hold fence data
 */
typedef struct{
    enum FENCE_TYPE {KEEP_IN, KEEP_OUT}; ///< enum for fence type
    int fenceType;                       ///< fence type
    int id;                              ///< fence index
    larcfm::SimplePoly polygon;          ///< polygon
}fenceObject;

/**
 * @brief Trajectory Management object
 */
class TrajManager{

private:

    DubinsPlanner dbPlanner;                     ///< Dubins planner object
    double wellClearDistH;                       ///< well clear radius for planning
    double wellClearDistV;                       ///< well clear height for planning
    int numPlans;                                ///< number of plans
    std::ofstream log;                           ///< log holder
    std::list<larcfm::Plan> flightPlans;         ///< list of plans
    std::list<larcfm::Plan> trafficPlans;        ///< list of plans for traffic vehicles (intent information)
    std::list<fenceObject> fenceList;            ///< list of fences
    std::map<std::string,pObject> trafficList;   ///< list of traffic

    /**
     * @brief Compute new path
     * 
     * @param planID id to assign to plan
     * @return int64_t total number of waypoints computed
     */
    int64_t FindDubinsPath(std::string planID);

    /**
     * @brief Function to compute projected time to violation to all fence edges
     * 
     * @param polygon fence object
     * @param so current position
     * @param vel current velocity
     * @return double time to violation
     */
    double FindTimeToFenceViolation(larcfm::Poly3D polygon,larcfm::Vect3 so,larcfm::Velocity vel);

    /**
     * @brief Compute cross track (perpendicular deviation), along track (longitudinal deviations) and time delays 
     * 
     * @param planID id of plan to compute metrics
     * @param nextWP next waypoint index
     * @param currentPos current position
     * @param currentVel current velocity
     * @param timeAtPos current time
     * @return std::vector<double> cross track offset, longitudinal offset, time offset
     */
    std::vector<double> ComputePlanOffsets(const std::string planID,int nextWP,larcfm::Position currentPos,larcfm::Velocity currentVel,double timeAtPos);

    /**
     * @brief Check line of sight conflict
     * 
     * @param start start position
     * @param end end position
     */
    bool CheckLineOfSightconflict(larcfm::Position start, larcfm::Position end);

    /**
     * @brief Log inputs to planning
     */
    void LogInput();
public:
    larcfm::Position startPos; ///< start position for planning
    larcfm::Position endPos;   ///< goal position for planning
    larcfm::Velocity startVel; ///< start velocity for planning
    larcfm::Velocity endVel;   ///< goal velocity for planning

    /**
     * @brief Construct a new Traj Manager object
     * 
     * @param callsign callsign string
     * @param config configuration file
     */
    TrajManager(std::string callsign,std::string config);

    /**
     * @brief Read parameters from file
     * 
     * @param config configuration file name
     */
    void ReadParamFromFile(std::string config);

    /**
     * @brief Update dubins planner parameters
     * 
     * @param params 
     */
    void UpdateDubinsPlannerParameters(DubinsParams_t &params);

    /**
     * @brief Compute path
     * 
     * @param planID plan id to assign to this plan
     * @param fromPosition start position
     * @param toPosition end position
     * @param fromVelocity start velocity 
     * @param toVelocity end velocity
     * @return int total number of waypoints
     */
    int FindPath(std::string planID, larcfm::Position fromPosition,larcfm::Position toPosition,larcfm::Velocity fromVelocity,larcfm::Velocity toVelocity);

    /**
     * @brief Get the Total Waypoints 
     * 
     * @param planID 
     * @return int 
     */
    int GetTotalWaypoints(std::string planID);

    /**
     * @brief Get the Waypoint object
     * 
     * @param planID flight plan id 
     * @param wpid  waypoint index
     * @param wp [out] waypoint
     * @return int 
     */
    int GetWaypoint(std::string planID,int wpid,waypoint_t& wp);

    /**
     * @brief Get pointer to Plan object
     * 
     * @param planID 
     * @return larcfm::Plan* 
     */
    larcfm::Plan* GetPlan(std::string planID);

    /**
     * @brief Clear all plans
     * 
     */
    void ClearAllPlans();

    /**
     * @brief Clear fences
     * 
     */
    void ClearFences();

    /**
     * @brief Input geofence
     * 
     * @param type fence type (keep in or keep out)
     * @param index index of fence
     * @param totalVertices total vertices in fences
     * @param floor floor altitude
     * @param ceiling ceiling altitude
     * @param pos array of positions
     */
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]);

    /**
     * @brief Input geofence
     * 
     * @param type fence type (keep in or keep out)
     * @param index index of fence
     * @param totalVertices total vertices in fences
     * @param floor floor altitude
     * @param ceiling ceiling altitude
     * @param vertices list of all vertices
     */
    void InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, std::vector<larcfm::Position> &vertices);

    /**
     * @brief Input a flightplan
     * 
     * @param plan_id id of plan
     * @param waypoints list of waypoints in plan
     * @param initHeading initial heading
     * @param repair convert linear plan to kinematic plan
     * @param repairTurnRate turn rate used for repairing
     */
    void InputFlightPlan(const std::string &plan_id, const std::list<waypoint_t> &waypoints, 
                         const double initHeading,bool repair,double repairTurnRate);
    
    /**
     * @brief Input traffic data
     * 
     * @param callsign 
     * @param position 
     * @param velocity 
     * @param time 
     * @return int 
     */
    int InputTraffic(std::string callsign, larcfm::Position &position, larcfm::Velocity &velocity,double time);

    /**
     * @brief Convert plan to string
     * 
     * @param planID 
     * @return std::string 
     */
    std::string PlanToString(std::string planID);

    /**
     * @brief Convert string to plan
     * 
     * @param planID 
     * @param inputString 
     */
    void StringToPlan(std::string planID,std::string inputString);

    /**
     * @brief Combine planA with planB from given index
     * 
     * @param planA 
     * @param planB 
     * @param index 
     */
    void CombinePlan(std::string planA,std::string planB,int index);

    /**
     * @brief Set the time Offset for given plan
     * 
     * @param planID id of plan
     * @param n  index of waypoint from which timeshift should be applied
     * @param offset time offset
     */
    void SetPlanOffset(std::string planID,int n,double offset);

    /**
     * @brief Get the position on plan at given t
     * 
     * @param planID id of plan
     * @param t query time
     * @return larcfm::Position 
     */
    larcfm::Position GetPlanPosition(std::string planID,double t);

    /**
     * @brief Monitor trajectory
     * 
     * @param time current time
     * @param planID id of plan
     * @param pos current position
     * @param vel current velocity
     * @param nextWP1 next wp on primary plan (Plan0)
     * @param nextWP2 next wp on second plan 
     * @return trajectoryMonitorData_t 
     */
    trajectoryMonitorData_t MonitorTrajectory(double time, std::string planID, larcfm::Position pos,larcfm::Velocity vel, int nextWP1,int nextWP2);
};


#endif //PATHPLANNER_H


