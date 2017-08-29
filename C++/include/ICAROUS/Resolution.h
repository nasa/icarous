/*
 * Resolution.h
 *
 *  Created on: Mar 21, 2017
 *      Author: research133
 */

#ifndef RESOLUTION_H_
#define RESOLUTION_H_

#include "AircraftData.h"
#include "Position.h"
#include "Plan.h"
#include "Daidalus.h"
#include "KinematicMultiBands.h"

class FlightManagementSystem_t;

class Resolution_t{
private:
	FlightManagementSystem_t* FMS;
	AircraftData_t* FlightData;
	double resolutionSpeed;
	double gridsize;
	double lookahead;
	double allowedDev;
	double xtrkDevGain;

	double alertTime0;
	int gotoNextWP;
	double diffAlertTime;
	time_t trafficResolutionTime;
	Daidalus DAA;

public:
	bool returnPathConflict;
	Resolution_t(){};
	Resolution_t(FlightManagementSystem_t *fms);
	void ResolveKeepInConflict();
	void ResolveKeepOutConflict_Astar();
	void ResolveKeepOutConflict_RRT();
	void ResolveFlightPlanDeviation();
	void ResolveTrafficConflictRRT();
	void ResolveTrafficConflictDAA();
	double SaturateVelocity(double V, double Vsat);
	Plan ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed);
	Position GetPointOnPlan(double offset,Plan fp,int next);
};




#endif /* RESOLUTION_H_ */
