/*
 * icarous_table.h
 *
 */

#ifndef ICAROUS_TABLE_H_
#define ICAROUS_TABLE_H_


#include <stdint.h>

typedef struct{
	uint8_t heartbeatRate;
	double takeoffAlt;
	uint8_t allowYaw;
	double maxCeiling;

	// Geofence parameters
	double hThreshold;
	double vThreshold;
	double hStepback;
	double vStepback;

	//A star search options
	uint8_t cheapSearch;
	double gridSize;
	double buffer;
	double lookahead;
	double proxFactor;
	double resolutionSpeed;

	// Flight plan parameters
	double xtrkGain;
	double xtrkDev;
	double Capture_H;
	double Capture_V;
	uint8_t WaitAtWaypoint;

	// Tracking options
	double trkHeading;
	double trkDistH;
	double trkDistV;

	// DAA parameters
	char daaConfig[50];
	uint8_t cheapDAA;
	uint8_t gotoNextWP;
	uint8_t conflictHold;
	double cylRadius;
	double cylHeight;
	double alertTime;
	double earlyAlertTime;
	double daaLookAhead;

}icarous_table_t;

#endif /* ICAROUS_TABLE_H_ */
