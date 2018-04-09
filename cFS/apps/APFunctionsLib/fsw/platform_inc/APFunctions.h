//
// Created by Swee Balachandran on 11/28/17.
//

#ifndef APFUNCTIONS_H
#define APFUNCTIONS_H

#include "cfe.h"
#include "msgids/msgids.h"
#include "Icarous_msg.h"

#define InitCmdMsg(cmd)\
ArgsCmd_t cmd; \
CFE_SB_InitMsg(&cmd,AP_COMMANDS_MID,sizeof(ArgsCmd_t),TRUE);

#define InitTrafficServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_TRAFFIC_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

#define InitTrajectoryServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_TRAJECTORY_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

#define InitGeofenceServiceMsg(msg)\
service_t msg; \
CFE_SB_InitMsg(&msg,SERVICE_GEOFENCE_MID,sizeof(service_t),TRUE);\
msg.id = (rand() + rand());

int32 APFunctionsLibInit(void);
void SetMode(icarous_control_mode_t mode);
void ArmThrottles(uint8_t c);
void SetNextMissionItem(int nextWP);
void Takeoff(float alt);
void Land();
void SetYaw(uint8_t relative,double heading);
void SetGPSPosition(double lat,double lon,double alt);
void SetVelocity(double Vn, double Ve, double Vu);
void SetSpeed(float speed);

// helper functions to request services from geofence app
/**
 * Find fence violation given current position and velocity
 * @param position pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @param velocity pointer to double[3] containing track (deg),ground speed (m/s),vertical speed (m/s)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceFence_GetFenceViolation(double* position,double* velocity);

/**
 * Check path feasibility between two waypoints
 * @param positionA pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @param positionB pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */

int ServiceFence_GetWPFeasibility(double* positionA,double* positionB);

/**
 * Get safe recovery position free from geofence conflict closest to given current position
 * @param position pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceFence_GetRecoveryPosition(double* position);

// helper function to request services from traffic app given current position and velocity of ownship.
/**
 * Check for conflicts against any available intruders
 * @param bands true/false indicating if bands must be computed.
 * @param position pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @param velocity pointer to double[3] containing track (deg),ground speed (m/s),vertical speed (m/s)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTraffic_GetTrafficConflict(bool bands,double* position,double* velocity);

/**
 * Check if its safe to turn from headingA to headingB
 * @param position pointer to double[3] containing lat (deg),lon (deg), alt (m)
 * @param velocity pointer to double[3] containing track (deg),ground speed (m/s),vertical speed (m/s)
 * @param headingA heading in degrees
 * @param headingB heading in degrees
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTraffic_GetSafe2Turn(double* position,double* velocity,double headingA,double headingB);

// helper functions to request services from trajectory app

/**
 * Get waypoint of given index in given plan
 * @param planID name of plan
 * @param index index of waypoint
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetWaypoint(char* planID,int index);

/**
 * Get total number of waypoints
 * @param planID name of plan
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetTotalWaypoints(char* planID);

/**
 * Compute a new path from current position to new position
 * @param planID name of plan (max of 5 characters)
 * @param algorithm algorithm to use (GRID,ASTAR,RRT)
 * @param positionA from position, double[3] containing lat (deg),lon (deg), alt (m)
 * @param velocity from velocity, double[3] containing track (deg),ground speed (m/s),vertical speed (m/s)
 * @param positionB to position, double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetNewPath(char* planID,char* algorithm,double* positionA,double* velocity,double* positionB);

/**
 * Get cross track deviation from given plan in given leg at given position.
 * @param planID name of plan
 * @param leg flight plan leg
 * @param position double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetXTrackDeviation(char* planID,int leg,double* position);

/**
 * Get distance from given between given positions.
 * @param positionA double[3] containing lat (deg),lon (deg), alt (m)
 * @param positionB double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetDistance(double* positionA,double* positionB);

/**
 * Get the nearest exit point (if flight plan goes through a geofence)
 * @param planID name of flight plan
 * @param position current position, double[3] containing lat (deg),lon (deg), alt (m)
 * @param nextWP next waypoint index in the flight plan.
 * @return returns an id that should be used to listen for a response.
 */

int ServiceTrajectory_GetExitPoint(char* planID,double* position,int nextWP);

/**
 * Get heading to intercept the given flight plan.
 * @param planID name of flight plan.
 * @param nextWP next waypoint index in flight plan.
 * @param position current position,double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetInterceptHeading(char* planID,int nextWP,double* position);

/**
 * Get velocity vector to intercept given flight plan
 * @param planID name of flight plan
 * @param nextWP next waypoint index in flight plan
 * @param position current position,double[3] containing lat (deg),lon (deg), alt (m)
 * @return returns an id that should be used to listen for a response.
 */
int ServiceTrajectory_GetInterceptManeuver(char* planID,int nextWP,double* position);

/**
 * Get heading to follow to get from positionA to position B
 * @param positionA current position,double[3] containing lat (deg),lon (deg), alt (m)
 * @param velocity current velocity, double[3] containing track (deg),ground speed (m/s),vertical speed (m/s)
 * @param positionB target position, double[3] containing lat (deg),lon (deg), alt (m)
 * @return
 */
int ServiceTrajectory_GetManeuverHeading(double* positionA,double* positionB);


// helper functions to decode service response
/**
 * Decode a GetFenceViolation service response message
 * @param msg service_t message received from SERVICE_GEOFENCE_RESPONSE_MID subscription.
 * @param output pointer to bool[2] ([0] keepin conflict, [1] keepout conflict).
 */
void ServiceFence_DecodeFenceViolation(service_t* msg,bool* output);

/**
 * Decode a WPFeasibility service response message
 * @param msg service_t message received from SERVICE_GEOFENCE_RESPONSE_MID subscription.
 * @param output pointer to bool.
 */
void ServiceFence_DecodeWPFeasibility(service_t* msg,bool* output);

/**
 * Decode a RecoveryPosition service response message
 * @param msg service_t message received from SERVICE_GEOFENCE_RESPONSE_MID subscription.
 * @param output pointer to double[3].
 */
void ServiceFence_DecodeRecoveryPosition(service_t* msg,double* output);

/**
 * Decode a TrafficConflict service response message
 * @param msg service_t message received from SERVICE_TRAFFIC_RESPONSE_MID subscription.
 * @param conflict pointer to bool.
 * @param output pointer to double[3]. [0] resolution track heading.
 */
void ServiceTraffic_DecodeTrafficConflict(service_t* msg,bool* conflict,double* output);

/**
 * Decode a Safe2Turn service response message
 * @param msg service_t message received from SERVICE_TRAFFIC_RESPONSE_MID subscription.
 * @param output pointer to bool.
 */
void ServiceTraffic_DecodeSafe2Turn(service_t* msg,bool* output);

/**
 * Decode a Waypoint service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double[3],  lat (deg),lon (deg), alt (m)
 */
void ServiceTrajectory_DecodeWaypoint(service_t* msg,double* output);

/**
 * Decode a TotalWaypoints service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to int.
 */
void ServiceTrajectory_DecodeTotalWaypoints(service_t* msg,int* output);

/**
 * Decode a NewPath service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to int. (number of waypoints in computed path)
 */
void ServiceTrajectory_DecodeNewPath(service_t* msg,int* output);

/**
 * Decode XtrackDeviation service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double.
 */
void ServiceTrajectory_DecodeXTrackDeviation(service_t* msg,double* output);

/**
 * Decode Distance service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double.
 */
void ServiceTrajectory_DecodeDistance(service_t* msg,double* output);

/**
 * Decode ExitPoint service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double[3], lat (deg),lon (deg), alt (m)
 */
void ServiceTrajectory_DecodeExitPoint(service_t* msg,double* output);

/**
 * Decode InterceptHeading service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double
 */
void ServiceTrajectory_DecodeInterceptHeading(service_t* msg,double* output);

/**
 * Decode InterceptManeuver service response message
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double[3], containing track (deg),ground speed (m/s) and vertical speed (m/s)
 */
void ServiceTrajectory_DecodeInterceptManeuver(service_t* msg,double* output);

/**
 * Decode heading for maneuver
 * @param msg service_t message received from SERVICE_TRAJECTORY_RESPONSE_MID subscription.
 * @param output pointer to double[3], containing track (deg),ground speed (m/s) and vertical speed (m/s)
 */
void ServiceTrajectory_DecodeManeuverHeading(service_t* msg,double* output);

#endif //APFUNCTIONS_H
