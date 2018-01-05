#ifndef FLIGHTDATAMANAGER_H
#define FLIGHTDATAMANAGER_H

#include <list>
#include <cstdint>
#include <pthread.h>
#include "AircraftState.h"
#include "Position.h"
#include "Velocity.h"
#include "fence.h"
#include "GenericObject.h"
#include "Icarous_msg.h"
#include "Plan.h"


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
    larcfm::PolyPath geoPolyPath;
    std::list<fence> fenceList;
    std::list<fence>::iterator fenceListIt;
    std::list<GenericObject> trafficList;

    std::list<ArgsCmd_t> outputList;
    std::list<CmdAck_t> commandAckList;
    std::list<waypoint_t> listMissionItem;
    std::list<waypoint_t> listResolutionItem;
    std::list <geofence_t>tempVertices;
    visbands_t visBands;

    void ConstructPlan(Plan* pl, std::list<waypoint_t> *listWaypoints);

public:

    ParameterData paramData;

    FlightData(char configfile[]);
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
    void InputGeofenceData(geofence_t* gf);
    double GetTakeoffAlt();
    double GetCruisingAlt();
    double GetAltitude();
    double GetAllowedXtracDeviation();
    int GetTotalMissionWP();
    int GetTotalResolutionWP();
    int GetTotalTraffic();
    void Reset();
    void InputAck(CmdAck_t* ack);
    bool CheckAck(command_name_t command);
    int8_t GetStartMissionFlag();
    uint16_t GetMissionPlanSize();
    uint16_t GetResolutionPlanSize();
    uint16_t GetNextMissionWP();
    uint16_t GetNextResolutionWP();
    int GetTotalFences();

    double getFlightPlanSpeed(Plan* fp,int nextWP);
    void GetTraffic(int id,larcfm::Position& pos,larcfm::Velocity& vel);
    fence* GetGeofence(int id);
    PolyPath* GetPolyPath();
};



#endif
