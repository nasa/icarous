//
// Created by Swee Balachandran on 12/7/17.
//

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

struct node{
    int id;
    double time;
    larcfm::Vect3 pos;
    larcfm::Velocity vel;

    bool goal;
    double g,h;
    double dist2goal;
    tcpData_t TCPdata;
    node* source;
    std::list<node*> parents;
    std::list<node*> children;
};
typedef node node_t;

class DubinsPlanner {

private:

    /* min,max values for the 3D coordinates to restrict search */
    double xmax,xmin;
    double ymax,ymin;
    double zmax,zmin;

    /* boundingBox defines the extent of the search space */
    larcfm::Poly3D boundingBox;

    /* Contracted vertices of bounding box */
    std::vector<larcfm::Vect2> shrunkbbox;

    /* List of obstacles */
    std::list<larcfm::Poly3D> obstacleList;


    /* Feasible nodes */
    std::vector<node_t> potentialFixes;

    

    /* Total node explored*/
    int nodeCount;

    /* Root position */
    larcfm::Vect3 rootFix;
    larcfm::Velocity rootVel;
    

    /* Goal position */
    larcfm::Vect3 goalFix;
    larcfm::Velocity goalVel;

    std::vector<larcfm::Vect3> trafficPosition;
    std::vector<larcfm::Velocity> trafficVelocity;

    std::list<larcfm::Plan> trafficPlans;

    DubinsParams_t params;
   
public:

    std::vector<node_t> path;
    DubinsPlanner(){};
    void Reset();
    void ShrinkTrafficVolume(double factor);
    void SetParameters(DubinsParams_t& prms);
    void SetVehicleInitialConditions(larcfm::Vect3& pos, larcfm::Velocity& vel);
    void SetTraffic(std::vector<larcfm::Vect3>& tfpos,std::vector<larcfm::Velocity>& tfvel);
    void SetTraffic(larcfm::Plan &tf);
    void SetBoundary(larcfm::Poly3D& boundingBox);
    void SetZBoundary(double zmin,double zmax);
    void SetObstacles(std::list<larcfm::Poly3D> &obstacleList);
    void SetGoal(larcfm::Vect3& goal,larcfm::Velocity finalVel);
    bool ComputePath(double startTime);
    void GetPlan(larcfm::EuclideanProjection& proj,larcfm::Plan& plan);

private:
    node_t* FindNearest(node_t& query);
    void GetPotentialFixes();
    bool CheckConflictCircleCircle(larcfm::Vect3 center1, larcfm::Vect3 start1, larcfm::Vect3 stop1,larcfm::Velocity startVel1,larcfm::Vect2 timeInterval1, double radius1,
                                   larcfm::Vect3 center2, larcfm::Vect3 start2, larcfm::Vect3 stop2,larcfm::Velocity startVel2,larcfm::Vect2 timeInterval2, double radius2 );
    bool CheckConflictLineCircle(larcfm::Vect3 center1, larcfm::Vect3 start1, larcfm::Vect3 stop1,double turnRate,larcfm::Vect2 timeInterval1, double radius1,
                                 larcfm::Vect3 start2, larcfm::Vect3 startVel2,double t2,larcfm::Vect2 timeInterval2);
    std::pair<double,double> GetLineCircleIntersection(larcfm::Vect2 start,larcfm::Vect2 direction,double t0,double R);
    std::pair<double,double> GetLineLineIntersection(larcfm::Vect3 start1,larcfm::Vect3 direction1,double t1,larcfm::Vect3 start2,larcfm::Vect3 direction2,double t2,double R);
    bool CheckConflictLineLine(larcfm::Vect3 start1, larcfm::Velocity startVel1,double t1,larcfm::Vect2 timeInterval1,
                               larcfm::Vect3 start2, larcfm::Velocity startVel2,double t2,larcfm::Vect2 timeInterval2);
    void BuildTree(node_t* nd);
    bool DijkstraSearch(node_t* root,node_t* goal);
    bool AstarSearch(node_t* root,node_t* goal);
    bool GetNextTrkVs(node_t& node,double &trk, double &vs);
    bool GreedyDepthFirst(node_t q,node_t* g);
    double NodeDistance(node_t& A,node_t& B);
    bool CheckGoal();
    bool LinePlanIntersection(larcfm::Vect2& A,larcfm::Vect2& B,double floor,double ceiling,larcfm::Vect3& currentPos,larcfm::Vect3& nextPos);
    bool CheckProjectedFenceConflict(node_t* qnode,node_t* goal);
    bool GetDubinsParams(node_t* start,node_t* end);
    bool CheckAltFeasibility(double startz,double endz,double dist,double gs1,double gs2);
    tcpData_t ComputeAltTcp(tcpData_t &TCPdata,double startgs,double stopgs);
    tcpData_t ComputeSpeedTcp(tcpData_t &TCPdata,double startgs,double stopgs);
    bool CheckFenceConflict(tcpData_t trajectory);
    bool CheckTrafficConflict(tcpData_t trajectory);
    bool CheckTrafficConflict(double startTime,larcfm::Vect3 center1, larcfm::Vect3 startPos, larcfm::Vect3 tcp1,
                                               larcfm::Vect3 center2, larcfm::Vect3 tcp2, larcfm::Vect3 endPos,double r,double t1,double t2,double t3,double gs,double vs);
    std::list<node_t>* GetNodeList();

};


#endif //RRT_RRTPLANNER_H
