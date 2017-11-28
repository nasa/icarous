#ifndef FLIGHTDATAMANAGER_H
#define FLIGHTDATAMANAGER_H

#include <list>
#include <cstdint>
#include <pthread.h>
#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "Geofence.h"
#include "GenericObject.h"
#include "Icarous_msg.h"
#include "Plan.h"

#define __CWRAP__

class FlightData{

private:
    pthread_mutex_t lock;
    int8_t startMission;
    double acTime;
    double roll;
    double pitch;
    double yaw;
    double heading;
    double currentSpeed;
    double referenceSpeed;
    double takeoffAlt;
    double cruisingAlt;

    uint16_t nextMissionWP;
    uint16_t nextResolutionWP;

    larcfm::AircraftState acState;
    larcfm::Plan missionPlan;
    larcfm::Plan resolutionPlan;
    std::list<Geofence_t> fenceList;
    std::list<Geofence_t>::iterator fenceListIt;
    std::list<GenericObject_t> trafficList;

    std::list<ArgsCmd_t> outputList;
    std::list<CmdAck_t> commandAckList;
    std::list<waypoint_t> listMissionItem;
    std::list<waypoint_t> listResolutionItem;
    visbands_t visBands;

    void ConstructPlan(Plan* pl, std::list<waypoint_t> *listWaypoints);

public:

    // Wrap all functions with the __CWRAP__ defines for calls from C code
    __CWRAP__

    // Constructor
    FlightData();
    void AddMissionItem(waypoint_t* msg);
    void AddResolutionItem(waypoint_t* msg);
    void SetStartMissionFlag(uint8_t flag);
    void ConstructMissionPlan();
    void ConstructResolutionPlan();

    /**
     *
     * @param time
     * @param lat
     * @param lon
     * @param alt
     * @param vx
     * @param vy
     * @param vz
     */
    void InputState(double time,double lat,double lon,double alt,double vx,double vy,double vz);
    void AddTraffic(int id, double x,double y, double z, double vx,double vy,double vz);
    void GetTraffic(int id,double* x,double* y,double* z,double* vx,double* vy, double* vz);
    void ClearMissionList();
    void ClearResolutionList();
    void InputNextMissionWP(int index);
    void InputNextResolutionWP(int index);
    void InputTakeoffAlt(double alt);
    void InputCruisingAlt(double alt);
    double GetTakeoffAlt();
    double GetCruisingAlt();
    double GetAltitude();
    int GetTotalMissionWP();
    int GetTotalResolutionWP();
    void Reset();
    void InputAck(CmdAck_t* ack);

    bool CheckAck(command_name_t command);

    int8_t GetStartMissionFlag();
    uint16_t GetMissionPlanSize();
    uint16_t GetResolutionPlanSize();
    uint16_t GetNextMissionWP();
    uint16_t GetNextResolutionWP();
    __CWRAP__

    double getFlightPlanSpeed(Plan* fp,int nextWP);
    void GetTraffic(int id,larcfm::Position& pos,larcfm::Velocity& vel);
};



#endif
