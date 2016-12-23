/**
 * RRT
 *
 * RRT search algorithm
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 *
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * Notices:
 *  Copyright 2016 United States Government as represented by the Administrator of the National Aeronautics and Space Administration.
 *  All rights reserved.
 *
 * Disclaimers:
 *  No Warranty: THE SUBJECT SOFTWARE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER EXPRESSED,
 *  IMPLIED, OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL CONFORM TO SPECIFICATIONS, ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, OR FREEDOM FROM INFRINGEMENT,
 *  ANY WARRANTY THAT THE SUBJECT SOFTWARE WILL BE ERROR FREE, OR ANY WARRANTY THAT DOCUMENTATION, IF PROVIDED,
 *  WILL CONFORM TO THE SUBJECT SOFTWARE. THIS AGREEMENT DOES NOT, IN ANY MANNER, CONSTITUTE AN ENDORSEMENT BY GOVERNMENT
 *  AGENCY OR ANY PRIOR RECIPIENT OF ANY RESULTS, RESULTING DESIGNS, HARDWARE, SOFTWARE PRODUCTS OR ANY OTHER APPLICATIONS
 *  RESULTING FROM USE OF THE SUBJECT SOFTWARE.  FURTHER, GOVERNMENT AGENCY DISCLAIMS ALL WARRANTIES AND
 *  LIABILITIES REGARDING THIRD-PARTY SOFTWARE, IF PRESENT IN THE ORIGINAL SOFTWARE, AND DISTRIBUTES IT "AS IS."
 *
 * Waiver and Indemnity:
 *   RECIPIENT AGREES TO WAIVE ANY AND ALL CLAIMS AGAINST THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT.  IF RECIPIENT'S USE OF THE SUBJECT SOFTWARE
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,s
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

#ifndef RRT_H_
#define RRT_H_

#include <list>
#include <Geofence.h>
#include "EuclideanProjection.h"
#include "math.h"
#include "Poly3D.h"
#include "CDPolycarp.h"
#include "Position.h"
#include "Velocity.h"
#include "Vect3.h"
#include "Plan.h"
#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include <vector>
#include <string.h>

struct node_t{
	int id;
	Vect3 pos;
	Vect3 vel;
	std::vector<Vect3> trafficPos;
	std::vector<Vect3> trafficVel;

	bool goal;
	double g,h;
	std::list<node_t> children;
	std::list<node_t> parent;
};


class RRT_t{

public:
	int16_t xmax, xmin;
	int16_t ymax, ymin;
	int16_t zmax, zmin;
	int Tstep;
	double dT;
	std::list<Geofence_t> fenceList;
	Poly3D boundingBox;
	std::list<Poly3D> obstacleList;
	std::list<node_t> nodeList;
	std::list<node_t>::iterator ndit;
	EuclideanProjection proj;
	CDPolycarp geoPolycarp;
	int nodeCount;
	node_t root;
	int trafficSize;
	double closestDist;
	double daaLookAhead;
	node_t closestNode;
	node_t goalNode;
	Daidalus DAA;
	KinematicMultiBands KMB;
	bool goalreached;
	time_t startTime;

	RRT_t();
	RRT_t(std::list<Geofence_t> &fenceList,Position initialPos,Velocity initialVel,
			std::vector<Position> trafficPos,std::vector<Velocity> trafficVel,int stepT,double dt);

	void Initialize(Vect3 Pos,Vect3 Vel,
			std::vector<Vect3> TrafficPos,std::vector<Vect3> trafficVel);

	node_t MotionModel(Vect3 X, Vect3 V,
			std::vector<Vect3> trafficPos,std::vector<Vect3> trafficList, double U[]);

	void F(double X[], double U[],double Y[]);
	bool CheckFenceCollision(Vect3 qPos);
	bool CheckTrafficCollision(bool CheckTurn,Vect3 qPos,Vect3 qVel,
			    std::vector<Vect3> TrafficPos,std::vector<Vect3> trafficVel,Vect3 oldVel);
	bool CheckTurnConflict(double low,double high,double newHeading,double oldHeading);
	void GetInput(node_t nn, node_t qn,double U[]);
	node_t FindNearest(node_t query);
	double NodeDistance(node_t A,node_t B);
	void RRTStep(int i);
	bool CheckGoal();
	Plan GetPlan();
	void SetGoal(Position goal);
	void SetGoal(node_t goal);
	bool CheckDirectPath2Goal(node_t nearest);
	bool LinePlanIntersection(Vect2 A,Vect2 B,double floor,double ceiling,Vect3 currentPos,Vect3 nextPos);
	bool CheckProjectedFenceConflict(node_t qnode,node_t goal);
};




#endif /* RRT_H_ */
