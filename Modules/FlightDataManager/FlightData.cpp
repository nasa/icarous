/**
 * Aircraft data
 *
 * Shared data structure containing all flight relevant data and functions
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
 *   RESULTS IN ANY LIABILITIES, DEMANDS, DAMAGES,
 *   EXPENSES OR LOSSES ARISING FROM SUCH USE, INCLUDING ANY DAMAGES FROM PRODUCTS BASED ON, OR RESULTING FROM,
 *   RECIPIENT'S USE OF THE SUBJECT SOFTWARE, RECIPIENT SHALL INDEMNIFY AND HOLD HARMLESS THE UNITED STATES GOVERNMENT,
 *   ITS CONTRACTORS AND SUBCONTRACTORS, AS WELL AS ANY PRIOR RECIPIENT, TO THE EXTENT PERMITTED BY LAW.
 *   RECIPIENT'S SOLE REMEDY FOR ANY SUCH MATTER SHALL BE THE IMMEDIATE, UNILATERAL TERMINATION OF THIS AGREEMENT.
 */

#include "Icarous_msg.h"
#include "FlightData.h"
#include "SeparatedInput.h"

FlightData::FlightData(char configfile[]){
    pthread_mutex_init(&lock, NULL);
    startMission = -1;
    nextMissionWP = 0;
    nextResolutionWP = 0;
    heading = 0;
    roll = 0;
    pitch = 0;
    yaw = 0;
    visBands.numBands = 0;

    ifstream ConfigFile;
    SeparatedInput sepInputReader(&ConfigFile);

    ConfigFile.open(configfile);
    sepInputReader.readLine();
    paramData = sepInputReader.getParameters();
}

void FlightData::InputState(double time,double lat, double lon, double alt, double vx, double vy, double vz) {
    larcfm::Position currentPos = Position::makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");
    larcfm::Velocity currentVel = Velocity::makeVxyz(vy,vx,"m/s",vz,"m/s");

    acState.add(currentPos,currentVel,time);
    acTime = time;
}

void FlightData::InputNextMissionWP(int index){
  pthread_mutex_lock(&lock);
  nextMissionWP = index;
  pthread_mutex_unlock(&lock);
}

uint16_t FlightData::GetNextMissionWP(){
  int nextWP;
  pthread_mutex_lock(&lock);
  nextWP = nextMissionWP;
  pthread_mutex_unlock(&lock);
  return nextWP;
}

void FlightData::InputNextResolutionWP(int index){
  pthread_mutex_lock(&lock);
  nextResolutionWP = index;
  pthread_mutex_unlock(&lock);
}

uint16_t FlightData::GetNextResolutionWP(){
  int nextWP;
  pthread_mutex_lock(&lock);
  nextWP = nextResolutionWP;
  pthread_mutex_unlock(&lock);
  return nextWP;
}

void FlightData::AddMissionItem(waypoint_t* msg){
    pthread_mutex_lock(&lock);
    listMissionItem.push_back(*msg);
    pthread_mutex_unlock(&lock);
}

void FlightData::AddResolutionItem(waypoint_t* msg){

    pthread_mutex_lock(&lock);
    listResolutionItem.push_back(*msg);
    pthread_mutex_unlock(&lock);
}

int8_t FlightData::GetStartMissionFlag(){
    int var;
    pthread_mutex_lock(&lock);
    var = startMission;
    startMission = -1;
    pthread_mutex_unlock(&lock);
    return (int8_t)var;
}

void FlightData::SetStartMissionFlag(uint8_t flag){
    pthread_mutex_lock(&lock);
    startMission = flag;
    pthread_mutex_unlock(&lock);
}

uint16_t FlightData::GetMissionPlanSize(){
    unsigned long size;
    pthread_mutex_lock(&lock);
    size = listMissionItem.size();
    pthread_mutex_unlock(&lock);
    return (uint16_t)size;
}

uint16_t FlightData::GetResolutionPlanSize(){
    unsigned long size;
    pthread_mutex_lock(&lock);
    size = listResolutionItem.size();
    pthread_mutex_unlock(&lock);
    return (uint16_t)size;
}

void FlightData::ClearMissionList(){
    listMissionItem.clear();
}

void FlightData::ClearResolutionList(){
    listResolutionItem.clear();
}

void FlightData::ConstructPlan(Plan* pl, std::list<waypoint_t> *listWaypoints){
    // Create a Plan object with the available mission items
    pl->clear();
    std::list<waypoint_t>::iterator it;
    int ic;
    for(it = listWaypoints->begin(),ic = 0;
        it != listWaypoints->end(); ++it,++ic){
        larcfm::Position WP = Position::makeLatLonAlt(it->latitude,"degree",it->longitude,"degree",it->altitude,"m");
        double wptime = 0;
        if(ic > 0){

            double vel = it->speed;

            if(vel < 0.5){
                vel = 2;
            }

            double distance = pl->point(ic - 1).position().distanceH(WP);
            wptime   = pl->time(ic-1) + distance/vel;
        }
        NavPoint navPoint(WP,wptime);
        pl->addNavPoint(navPoint);
    }
    printf("Constructed Flight plan with %d waypoints\n",pl->size());
}

void FlightData::ConstructMissionPlan() {
    ConstructPlan(&missionPlan,&listMissionItem);
}

void FlightData::ConstructResolutionPlan() {
    ConstructPlan(&resolutionPlan,&listResolutionItem);
}

double FlightData::getFlightPlanSpeed(Plan* fp,int nextWP){
    double speed = fp->pathDistance(nextWP-1,true)/(fp->time(nextWP) - fp->time(nextWP-1));
    return speed;
}

void FlightData::AddTraffic(int id,double lat,double lon,double alt,double vx,double vy,double vz){
    pthread_mutex_lock(&lock);
    GenericObject newTraffic(1,id,(float)lat,(float)lon,(float)alt,(float)vx,(float)vy,(float)vz);
    GenericObject::AddObject(trafficList,newTraffic);
    pthread_mutex_unlock(&lock);
}

void FlightData::GetTraffic(int id,larcfm::Position& pos,larcfm::Velocity& vel){
    pthread_mutex_lock(&lock);
    std::list<GenericObject>::iterator it;
    for(it = trafficList.begin(); it != trafficList.end(); ++it){
        if(it->id == id){
            pos = it->pos;
            vel = it->vel;
            break;
        }
    }
    pthread_mutex_unlock(&lock);
}

void FlightData::GetTraffic(int id,double* x,double*y,double *z,double* vx,double *vy,double *vz){
    larcfm::Position pos;
    larcfm::Velocity vel;
    GetTraffic(id,pos,vel);
    *x = pos.latitude();
    *y = pos.longitude();
    *z = pos.alt();
    *vx = vel.x;
    *vy = vel.y;
    *vz = vel.z;
}

void FlightData::Reset(){
    pthread_mutex_lock(&lock);
    startMission = -1;
    nextMissionWP = 0;
    nextResolutionWP = 0;
    heading = 0;
    roll = 0;
    pitch = 0;
    yaw = 0;
    ClearMissionList();
    ClearResolutionList();
    pthread_mutex_unlock(&lock);
}

bool FlightData::CheckAck(command_name_t command){
    bool status = false;
    pthread_mutex_lock(&lock);
    while(commandAckList.size() > 0){
        CmdAck_t ack = commandAckList.front();
        commandAckList.pop_front();
        if(ack.name == command && ack.result == 0){
            status = true;
            pthread_mutex_unlock(&lock);
            return status;
        }
    }
    pthread_mutex_unlock(&lock);
    return status;
}

void FlightData::InputAck(CmdAck_t *ack) {
    pthread_mutex_lock(&lock);
    commandAckList.push_back(*ack);
    pthread_mutex_unlock(&lock);
}

void FlightData::InputTakeoffAlt(double alt) {
    pthread_mutex_lock(&lock);
    takeoffAlt = alt;
    pthread_mutex_unlock(&lock);
}

void FlightData::InputCruisingAlt(double alt) {
    pthread_mutex_lock(&lock);
    cruisingAlt = alt;
    pthread_mutex_unlock(&lock);
}

double FlightData::GetTakeoffAlt(){
    double alt;
    pthread_mutex_lock(&lock);
    alt = takeoffAlt;
    pthread_mutex_unlock(&lock);
    return alt;
}

double FlightData::GetCruisingAlt(){
    double alt;
    pthread_mutex_lock(&lock);
    alt = cruisingAlt;
    pthread_mutex_unlock(&lock);
    return alt;
}

int FlightData::GetTotalMissionWP() {
    int n;
    pthread_mutex_lock(&lock);
    n = missionPlan.size();
    pthread_mutex_unlock(&lock);
    return n;
}

int FlightData::GetTotalResolutionWP() {
    int n;
    pthread_mutex_lock(&lock);
    n = resolutionPlan.size();
    pthread_mutex_unlock(&lock);
    return n;
}

int FlightData::GetTotalTraffic(){
    int n;
    pthread_mutex_lock(&lock);
    n = trafficList.size();
    pthread_mutex_unlock(&lock);
    return n;
}

double FlightData::GetAltitude(){
    double val;
    pthread_mutex_lock(&lock);
    val = acState.positionLast().alt();
    pthread_mutex_unlock(&lock);
    return val;
}

double FlightData::GetAllowedXtracDeviation() {
    double val;
    val = paramData.getValue("XTRK_DEV");
    return val;
}

int FlightData::GetTrafficResolutionType(){
    int val;
    val = paramData.getInt("CHEAP_DAA");
    return val;
}

void FlightData::InputGeofenceData(geofence_t* gf){

    if(gf->vertexIndex == 0){
        tempVertices.clear();
    }
    tempVertices.push_back(*gf);
    double ResolBUFF = paramData.getValue("HTHRESHOLD");
    if(gf->vertexIndex+1 == gf->totalvertices){
        fence newfence((int)gf->index,(FENCE_TYPE)gf->type,(int)gf->totalvertices,gf->floor,gf->ceiling);
        for(geofence_t sgf: tempVertices){
            newfence.AddVertex(sgf.vertexIndex,sgf.latitude,sgf.longitude,ResolBUFF);
        }

        if(fenceList.size() > gf->index){
            fenceList.clear();
            for(int i=geoPolyPath.size()-1;i>=0;i--){
                geoPolyPath.remove(i);
            }
        }

        fenceList.push_back(newfence);
        if(newfence.GetType() == KEEP_OUT)
            geoPolyPath.addPolygon(*newfence.GetPoly(),Velocity::makeVxyz(0,0,0),0);
        std::cout << "Received fence: "<<gf->index <<std::endl;

    }
}

fence* FlightData::GetGeofence(int id) {
    for(fenceListIt = fenceList.begin();fenceListIt != fenceList.end();++fenceListIt){
        if (id == fenceListIt->GetID()){
            return &(*(fenceListIt));
        }
    }
}

int FlightData::GetTotalFences(){
    return fenceList.size();
}

PolyPath* FlightData::GetPolyPath(){
    return &geoPolyPath;
}

double FlightData::GetResolutionSpeed() {
    return paramData.getValue("RES_SPEED");
}