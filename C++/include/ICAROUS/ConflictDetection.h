/*
 * ConflictDetection.h
 *
 *  Created on: Mar 21, 2017
 *      Author: research133
 */

#ifndef CONFLICTDETECTION_H_
#define CONFLICTDETECTION_H_

#include "AircraftData.h"
#include "Daidalus.h"
#include "KinematicMultiBands.h"
#include "Geofence.h"
#include <list>

using namespace std;
using namespace larcfm;

class FlightManagementSystem_t;

class ConflictDetection_t{

private:
	FlightManagementSystem_t* FMS;
	AircraftData_t* FlightData;
	double daalogtime;

public:

	Geofence_t KeepInFence;
	Geofence_t KeepOutFence;
	bool keepInConflict;
	bool keepOutConflict;
	bool flightPlanDeviationConflict;
	bool trafficConflict;

	ConflictDetection_t(){};
	ConflictDetection_t(FlightManagementSystem_t* fms,AircraftData_t* fdata);
	void AddFenceConflict(Geofence_t gf);
	uint8_t size();
	void clear();
	int numConflicts;
	Daidalus DAA;
	KinematicMultiBands KMB;
	double daaTimeElapsed;
	time_t daaTimeStart;
	time_t timeStart;
	double daaLookAhead;

	void CheckGeofence();
	void CheckFlightPlanDeviation(bool devAllowed);
	void ComputeCrossTrackDev(Position pos,Plan fp,int nextWP,double stats[]);
	Position GetPointOnPlan(double offset,Plan fp,int next);
	bool CheckTurnConflict(double low,double high,double newHeading,double oldHeading);
	void CheckTraffic();
};


#endif /* CONFLICTDETECTION_H_ */
