/**
 * @file DubinsPlanner.hpp
 * @brief Dubins planner implementation
 * 
 */
#ifndef RRTPLANNER_H
#define RRTPLANNER_H

#include "Vect3.h"
#include "Poly3D.h"
#include "Position.h"
#include "Velocity.h"
#include "CDPolycarp.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "Plan.h"
#include "Daidalus.h"
#include "PolycarpResolution.h"
#include "Units.h"
#include "DubinsParams.h"
#include <cmath>
#include <string.h>
#include <list>
#include <vector>

typedef std::vector<std::pair<larcfm::NavPoint,larcfm::TcpData>> tcpData_t;

/**
 * @struct node
 * @brief node data structure used to build search tree
 * 
 */
struct node{
    int id;                       ///< id of node
    double time;                  ///< time at node
    larcfm::Vect3 pos;            ///< position 
    larcfm::Velocity vel;         ///< velocity
 
    bool goal;                    ///< true if node is the goal position
    double g,h;                   ///< cost incurred and heuristic
    double dist2goal;             ///< distance to goal
    tcpData_t TCPdata;            ///< trajectory change point data
    node* source;                 ///< pointer to parent node during search expansion
    std::list<node*> parents;     ///< list of possible parents in the tree
    std::list<node*> children;    ///< list of children nodes
};

/**
 * @brief DubinsPlanner object
 * 
 */
class DubinsPlanner {

private:

    larcfm::Poly3D boundingBox; ///< boundingBox defines the extent of the search space 

    std::vector<larcfm::Vect2> shrunkbbox; ///<  contracted vertices of bounding box 

    std::list<larcfm::Poly3D> obstacleList; ///<  list of obstacles

    std::vector<node> potentialFixes; ///< feasible nodes

    int nodeCount; ///< Total node explored

    larcfm::Vect3 rootFix;    ///< root position
    larcfm::Velocity rootVel; ///< velocity at root
    
    larcfm::Vect3 goalFix;    ///< goal position
    larcfm::Velocity goalVel; ///< velocity at goal

    std::vector<larcfm::Vect3> trafficPosition;    ///< positions of traffic
    std::vector<larcfm::Velocity> trafficVelocity; ///< velocities of traffic

    std::list<larcfm::Plan> trafficPlans; ///< intent information of traffic

    DubinsParams_t params;  ///< parameters for dubins planner
   
public:

    std::vector<node> path;  ///< final solution

    /**
     * @brief Construct a new Dubins Planner object
     * 
     */
    DubinsPlanner(){};

    /**
     * @brief Reset object
     * 
     */
    void Reset();

    /**
     * @brief Shrink traffic volume by given factor
     * 
     * @param factor value < 1
     */
    void ShrinkTrafficVolume(double factor);

    /**
     * @brief Set the Parameters object
     * 
     * @param prms 
     */
    void SetParameters(DubinsParams_t& prms);

    /**
     * @brief Set the Vehicle Initial Conditions 
     * 
     * @param pos vehicle initial position
     * @param vel vehicle initial velocity
     */
    void SetVehicleInitialConditions(larcfm::Vect3& pos, larcfm::Velocity& vel);

    /**
     * @brief Set intruders
     * 
     * @param tfpos list of traffic positions
     * @param tfvel list of corresponding traffic velocities
     */
    void SetTraffic(std::vector<larcfm::Vect3>& tfpos,std::vector<larcfm::Velocity>& tfvel);

    /**
     * @brief Set the Traffic plan if intent information is already known
     * 
     * @param tf 
     */
    void SetTraffic(larcfm::Plan &tf);

    /**
     * @brief Set bounding box constraint 
     * 
     * @param boundingBox 
     */
    void SetBoundary(larcfm::Poly3D& boundingBox);

    /**
     * @brief Set vertical limits
     * 
     * @param zmin 
     * @param zmax 
     */
    void SetZBoundary(double zmin,double zmax);

    /**
     * @brief Input obstacles
     * 
     * @param obstacleList 
     */
    void SetObstacles(std::list<larcfm::Poly3D> &obstacleList);

    /**
     * @brief Set goal position and velocity
     * 
     * @param goal 
     * @param finalVel 
     */
    void SetGoal(larcfm::Vect3& goal,larcfm::Velocity finalVel);

    /**
     * @brief Compute path
     * 
     * @param startTime 
     * @return true 
     * @return false 
     */
    bool ComputePath(double startTime);

    /**
     * @brief Get plan 
     * 
     * @param proj projection to compute gps coordinates from
     * @param plan output
     */
    void GetPlan(larcfm::EuclideanProjection& proj,larcfm::Plan& plan);

    /**
     * @brief Compute intersection between line and plane
     * 
     * @param A point A on plane
     * @param B point B on plane
     * @param floor plane floor
     * @param ceiling plane ceiling
     * @param currentPos start position of line
     * @param nextPos  end position of line
     * @return true 
     * @return false 
     */
    static bool LinePlanIntersection(larcfm::Vect2& A,larcfm::Vect2& B,double floor,double ceiling,larcfm::Vect3& currentPos,larcfm::Vect3& nextPos);

private:

    /**
     * @brief Find nearest node to a given query node
     * 
     * @param query 
     * @return node* 
     */
    node* FindNearest(node& query);

    /**
     * @brief Compute potential fixes used for planning
     * 
     */
    void GetPotentialFixes();

    /**
     * @brief Check if two circle arcs intersect. Each arc is occupied for a certain duration
     * 
     * @param center1 center of arc1
     * @param start1 start point on arc1
     * @param stop1 stop point on arc1
     * @param startVel1 start velocity on arc1
     * @param timeInterval1 time interval on arc1
     * @param radius1 radius of arc1
     * @param center2 center of arc2
     * @param start2 start point on arc2
     * @param stop2 stop point on arc2
     * @param startVel2 start velocity on arc2
     * @param timeInterval2 time interval on arc2
     * @param radius2 radius of arc2
     * @return true if there is a conflict
     */
    bool CheckConflictCircleCircle(larcfm::Vect3 center1, larcfm::Vect3 start1, larcfm::Vect3 stop1,larcfm::Velocity startVel1,larcfm::Vect2 timeInterval1, double radius1,
                                   larcfm::Vect3 center2, larcfm::Vect3 start2, larcfm::Vect3 stop2,larcfm::Velocity startVel2,larcfm::Vect2 timeInterval2, double radius2 );

    /**
     * @brief Check intersection of line segment and arc segment
     * 
     * @param center1 center of arc1
     * @param start1 start point of arc1
     * @param stop1 stop point of arc1
     * @param turnRate turn rate on arc
     * @param timeInterval1 time interval on arc1
     * @param radius1 radius of arc1
     * @param start2 start point on line
     * @param startVel2 start velocity on line
     * @param t2 start time on line
     * @param timeInterval2 time interval on line
     * @return true 
     * @return false 
     */
    bool CheckConflictLineCircle(larcfm::Vect3 center1, larcfm::Vect3 start1, larcfm::Vect3 stop1,double turnRate,larcfm::Vect2 timeInterval1, double radius1,
                                 larcfm::Vect3 start2, larcfm::Vect3 startVel2,double t2,larcfm::Vect2 timeInterval2);

    /**
     * @brief Compute line circle intersection. circle centered at origin
     * 
     * @param start start point
     * @param direction velocity
     * @param t0 start time
     * @param R radius of circle
     * @return std::pair<double,double> 
     */
    std::pair<double,double> GetLineCircleIntersection(larcfm::Vect2 start,larcfm::Vect2 direction,double t0,double R);

    /**
     * @brief Compute Line Line Intersection 
     * 
     * @param start1 start point on line1
     * @param direction1 direction of line1
     * @param t1 start time on line1
     * @param start2 start point on line2
     * @param direction2 direction on line2
     * @param t2 start time on line3
     * @param R collision radius. Consider intersection if points are R distance from each other.
     * @return std::pair<double,double> 
     */
    std::pair<double,double> GetLineLineIntersection(larcfm::Vect3 start1,larcfm::Vect3 direction1,double t1,larcfm::Vect3 start2,larcfm::Vect3 direction2,double t2,double R);

    /**
     * @brief Check conflict between line-line segment. Line segments have durations associated with them
     * 
     * @param start1 
     * @param startVel1 
     * @param t1 
     * @param timeInterval1 
     * @param start2 
     * @param startVel2 
     * @param t2 
     * @param timeInterval2 
     * @return true 
     * @return false 
     */
    bool CheckConflictLineLine(larcfm::Vect3 start1, larcfm::Velocity startVel1,double t1,larcfm::Vect2 timeInterval1,
                               larcfm::Vect3 start2, larcfm::Velocity startVel2,double t2,larcfm::Vect2 timeInterval2);

    /**
     * @brief Build tree by recursively expanding nodes
     * 
     * @param nd 
     */
    void BuildTree(node* nd);

    /**
     * @brief Perform Astar search from root to goal
     * 
     * @param root 
     * @param goal 
     * @return true 
     * @return false 
     */
    bool AstarSearch(node* root,node* goal);

    /**
     * @brief Get next trk and gs to use based on the smallest cost to go child
     * 
     * @param node 
     * @param trk 
     * @param vs 
     * @return true 
     * @return false 
     */
    bool GetNextTrkVs(node& node,double &trk, double &vs);

    /**
     * @brief Distance between two given nodes
     * 
     * @param A 
     * @param B 
     * @return double 
     */
    double NodeDistance(node& A,node& B);

    /**
     * @brief Check if goal was reached
     * 
     * @return true 
     * @return false 
     */
    bool CheckGoal();

    /**
     * @brief Check if line connecting two nodes is conflicting with available fences
     * 
     * @param qnode 
     * @param goal 
     * @return true 
     * @return false 
     */
    bool CheckProjectedFenceConflict(node* qnode,node* goal);

    /**
     * @brief Get parameters describing dubins curves between two nodes
     * 
     * @param start 
     * @param end 
     * @return true 
     * @return false 
     */
    bool GetDubinsParams(node* start,node* end);

    /**
     * @brief Check if altitude can be reached within the given distance and speed constraints
     * 
     * @param startz 
     * @param endz 
     * @param dist 
     * @param gs1 
     * @param gs2 
     * @return true 
     * @return false 
     */
    bool CheckAltFeasibility(double startz,double endz,double dist,double gs1,double gs2);

    /**
     * @brief Compute alittude trajectory change points
     * 
     * @param TCPdata 
     * @param startgs 
     * @param stopgs 
     * @return tcpData_t 
     */
    tcpData_t ComputeAltTcp(tcpData_t &TCPdata,double startgs,double stopgs);

    /**
     * @brief Compute speed trajectory change points
     * 
     * @param TCPdata 
     * @param startgs 
     * @param stopgs 
     * @return tcpData_t 
     */
    tcpData_t ComputeSpeedTcp(tcpData_t &TCPdata,double startgs,double stopgs);

    /**
     * @brief Check fence conflict for given dubins curve
     * 
     * @param trajectory 
     * @return true 
     * @return false 
     */
    bool CheckFenceConflict(tcpData_t trajectory);

    /**
     * @brief Check traffic conflict for given dubins curve
     * 
     * @param trajectory 
     * @return true 
     * @return false 
     */
    bool CheckTrafficConflict(tcpData_t trajectory);

    /**
     * @brief Check traffic conflict given a curve segment
     * 
     * @param startTime 
     * @param center1 
     * @param startPos 
     * @param tcp1 
     * @param center2 
     * @param tcp2 
     * @param endPos 
     * @param r 
     * @param t1 
     * @param t2 
     * @param t3 
     * @param gs 
     * @param vs 
     * @return true 
     * @return false 
     */
    bool CheckTrafficConflict(double startTime,larcfm::Vect3 center1, larcfm::Vect3 startPos, larcfm::Vect3 tcp1,
                                               larcfm::Vect3 center2, larcfm::Vect3 tcp2, larcfm::Vect3 endPos,double r,double t1,double t2,double t3,double gs,double vs);
    std::list<node>* GetNodeList();

};


#endif //RRT_RRTPLANNER_H
