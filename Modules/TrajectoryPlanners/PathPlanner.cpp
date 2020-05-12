//
// Created by Swee Balachandran on 12/14/17.
//
#include <cstring>
#include <list>
#include <string>
#include <sstream>
#include <PlanWriter.h>
#include <PlanReader.h>
#include <PlanUtil.h>
#include <sys/time.h>
#include "PathPlanner.h"
#include <TrajGen.h>

using namespace std;
using namespace larcfm;

PathPlanner::PathPlanner(double _obsBuffer,double _maxCeiling) {
    obsbuffer = _obsBuffer;
    maxCeiling = _maxCeiling;
    geoCDIIPolygon = CDIIPolygon(&geoPolyCarp);
    numPlans = 0;
    char            fmt1[64];
    struct timespec  tv;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;
    sprintf(fmt1,"Path-%f.log",localT);
    log.open(fmt1);
}

void PathPlanner::InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, double pos[][2]){


    double ResolBUFF = obsbuffer;
    fence newfence(index,(FENCE_TYPE)type,totalVertices,floor,ceiling);

    for(int i=0;i<totalVertices;++i) {
        newfence.AddVertex(i, pos[i][0], pos[i][1], ResolBUFF);
    }
    if(fenceList.size() > index){
        fenceList.clear();
        for(int i=geoPolyPath.size()-1;i>=0;i--){
            geoPolyPath.remove(i);
        }
    }

    fenceList.push_back(newfence);
    if(newfence.GetType() == KEEP_OUT)
        geoPolyPath.addPolygon(*newfence.GetPolyMod(),Velocity::makeVxyz(0,0,0),0);

    //std::cout <<"Trajectory module: Received fence: "<<index<<std::endl;

}

void PathPlanner::InputGeofenceData(int type,int index, int totalVertices, double floor, double ceiling, std::list<Position> &vertices){

    double ResolBUFF = obsbuffer;
    fence newfence(index,(FENCE_TYPE)type,totalVertices,floor,ceiling);
    
    size_t i = 0;
    for(Position pos: vertices) {
        newfence.AddVertex(i, pos.latitude(), pos.longitude(), ResolBUFF);
        i++;
    }

    if(fenceList.size() > index){
        fenceList.clear();
        for(int i=geoPolyPath.size()-1;i>=0;i--){
            geoPolyPath.remove(i);
        }
    }

    fenceList.push_back(newfence);
    if(newfence.GetType() == KEEP_OUT)
        geoPolyPath.addPolygon(*newfence.GetPolyMod(),Velocity::makeVxyz(0,0,0),0);
}

fence* PathPlanner::GetGeofence(int id) {
    std::list<fence>::iterator fenceListIt;
    for(fenceListIt = fenceList.begin();fenceListIt != fenceList.end();++fenceListIt){
        if (id == fenceListIt->GetID()){
            return &(*(fenceListIt));
        }
    }
    return NULL;
}

void PathPlanner::ClearFences() {
    fenceList.clear();
}

int PathPlanner::InputTraffic(int id, double *position, double *velocity) {

    GenericObject _traffic(0,_TRAFFIC_,id,(char*)"\0",(float)position[0],(float)position[1],(float)position[2],
                                        (float)velocity[0],(float)velocity[1],(float)velocity[2]);
    return GenericObject::AddObject(trafficList,_traffic);
}

int PathPlanner::InputTraffic(int id, Position &position, Velocity &velocity) {

    double positionR[3] = {position.latitude(),position.longitude(),position.alt()};
    double velocityR[3] = {velocity.x, velocity.y, velocity.z};
    
    return InputTraffic(id,positionR,velocityR);
}

void PathPlanner::UpdateDAAParameters(char *parameterString) {
    daaParameters = to_string(parameterString);
    larcfm::ParameterData newParams;
    newParams.parseParameterList(";",daaParameters);
    maxBankAngle = newParams.getValue("bank_angle") * 180/M_PI;
    maxHorAccel = newParams.getValue("horizontal_accel");
    maxVertAccel = newParams.getValue("vertical_accel");
}

int PathPlanner::FindPath(algorithm searchType, char *planID, double *fromPosition, double *toPosition,
                              double velocity[]) {

    search = searchType; 
    startPos = Position::makeLatLonAlt(fromPosition[0], "degree",fromPosition[1], "degree", fromPosition[2], "m");
    endPos   = Position::makeLatLonAlt(toPosition[0], "degree", toPosition[1], "degree", toPosition[2], "m");
    startVel = Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");

    LogInput();

    int64_t retval = -1;
    std::list<Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++ it){
        if (abs(strcmp(it->getID().c_str(),planID))){
            continue;
        }
        flightPlans.erase(it);
        break;
    }

    switch(search){

        case _GRID_:
            retval = FindPathGridAstar(planID);
            break;
        case _ASTAR_:
            retval = FindPathAstar(planID);
            break;
        case _RRT_:
            retval = FindPathRRT(planID);
            break;
#ifdef SPLINES
        case _SPLINES_:
            retval = FindPathBSplines(planID);
            break;
#endif
        default:
            break;
    }

    // Writing to file for debugging
    Position pos = Position::makeLatLonAlt(fromPosition[0],"degree",fromPosition[1],"degree",fromPosition[2],"m");
    EuclideanProjection projection =  Projection::createProjection(pos);
    char fenceFile[20];
    char wpFile[20];
    sprintf(fenceFile,"gf_%s.log",planID);
    sprintf(wpFile,"wp_%s.log",planID);
    OutputFlightPlan(&projection,planID,fenceFile,wpFile);

    if(retval > 0){
        numPlans++;
    }
    return retval;

}

Plan PathPlanner::ComputeGoAbovePlan(Position start,Position goal,double altFence,double rSpeed){
    // Compute go above plan
    Plan ResolutionPlan2;
    double ETA = 0;
    double distH,distV;

    NavPoint nvpt1(start,ETA);
    ResolutionPlan2.addNavPoint(nvpt1);

    // Second waypoint directly above WP1
    Position wp2 = start.mkAlt(altFence+1);
    distV = wp2.distanceV(start);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt2(wp2,ETA);
    ResolutionPlan2.addNavPoint(nvpt2);

    // Third waypoint directly above exit point
    Position wp3 = goal.mkAlt(altFence+1);
    distH = wp3.distanceH(wp2);
    ETA = ETA + distH/rSpeed;
    NavPoint nvpt3(wp3,ETA);
    ResolutionPlan2.addNavPoint(nvpt3);

    // Final waypoint
    distV = goal.distanceH(wp3);
    ETA = ETA + distV/rSpeed;
    NavPoint nvpt4(goal,ETA);
    ResolutionPlan2.addNavPoint(nvpt4);

    return ResolutionPlan2;
}

void PathPlanner::GetWaypoint(char *planID, int wpID, double *waypoint) {
    Plan* fp = GetPlan(planID);
    if(fp != NULL){
        Position pos = fp->getPos(wpID);
        waypoint[0] = pos.latitude();
        waypoint[1] = pos.longitude();
        waypoint[2] = pos.alt();
        waypoint[3] = fp->time(wpID);
    }else{
        waypoint[0] = 0;
        waypoint[1] = 0;
        waypoint[2] = 0;
        waypoint[3] = 0;
    }

}

Plan* PathPlanner::GetPlan(char planID[]){
    std::list<Plan>::iterator it;
    Plan *fp;
    for(it=flightPlans.begin();it != flightPlans.end(); ++ it){
        if (strcmp(it->getID().c_str(),planID)){
            continue;
        }
        fp = &(*it);
        return fp;
    };

    return NULL;
}

void PathPlanner::OutputFlightPlan(ENUProjection* proj,char* planID,char* fenceFile,char* waypointFile){
    struct timeval  tv;
    gettimeofday(&tv, NULL);
    char buffer[100];
    sprintf(buffer,"%f",(double)tv.tv_sec + (double)(tv.tv_usec)/1E6);
    log<<"Time after computation:"<<std::string(buffer)<<std::endl; 
    log<<"Resolution plan:"<<std::endl;


    char outputPlan[10000];
    PlanToString(planID,outputPlan,true,0);
    std::string resPlan(outputPlan);
    log<<resPlan<<std::endl;
    for(Plan pl: flightPlans){
        if(strcmp(pl.getID().c_str(),planID)){
            continue;
        }else {
            std::ofstream fp1;
            std::ofstream fp2;

            if (pl.size() > 0)
                fp1.open(waypointFile);
            else
                break;

            int fenceSize = fenceList.size();
            if ( fenceSize > 0) {
                fp2.open(fenceFile);
                for(int i=0;i<fenceSize;i++){
                    fence *gf = GetGeofence(i);
                    for(int j=0;j<gf->GetSize();j++){
                        Vect3 loc = proj->project(gf->GetPolyMod()->getVertex(j));
                        fp2 << gf->GetID() <<" "<<loc.x<<" "<<
                                                  loc.y<<std::endl;
                    }

                }
                fp2.close();
            }

            for(int i=0;i<pl.size();i++){
                Position pos = pl.getPos(i);
                Vect3 X = proj->project(pos);
                fp1 << X.x <<" "<<X.y<<" "<<X.z<<std::endl;
            }

            fp1.close();

            break;
        }
    }
}

int PathPlanner::GetTotalWaypoints(char planID[]){
    Plan* fp = GetPlan(planID);
    if (fp != NULL)
        return fp->size();
    else
        return 0;
}

void PathPlanner::InputFlightPlan(char planID[],int wpID,double waypoint[],double time){
    Position pos = Position::makeLatLonAlt(waypoint[0],"degree",waypoint[1],"degree",waypoint[2],"m");
    std::list<Plan>::iterator it;
    Plan* fp = GetPlan(planID);

    if(fp != NULL) {
        if(wpID == 0){
            fp->clear();
        }
        int n = fp->add(pos, (double) time);
        n = n + 1;
        return;
    }
    else{
        string planName = planID;
        Plan newPlan(planName);
        newPlan.add(pos,(double)time);
        flightPlans.push_back(newPlan);
        return;
    }
}

void PathPlanner::PlanToString(char planID[],char outputString[],bool tcpColumnsLocal,long int timeshift){
    Plan *fp = GetPlan(planID);
    Plan outputFp = fp->copy();
    outputFp.timeShiftPlan(0,(double)timeshift);
    Plan repairedPlan;
    if(timeshift >= 0){
        repairedPlan = TrajGen::makeKinematicPlan(outputFp,maxBankAngle*M_PI/180,maxHorAccel,maxVertAccel,true,true,true);
    }else{
        repairedPlan = *fp;
    }
    if(fp){
       PlanWriter planWriter; 
       std::ostringstream planString;
       planWriter.open(&planString);
       planWriter.writePlan(repairedPlan, tcpColumnsLocal);
       strcpy(outputString,planString.str().c_str());
    }
}

void PathPlanner::StringToPlan(char planID[],char inputString[]){
    Plan output;
    std::string inputPlan(inputString);
    PlanReader planReader;
    std::istringstream planString;
    planString.str(inputPlan);
    planReader.open(&planString);
    output = planReader.getPlan(0);

    // Check if a plan already exists with the given ID and replace it if 
    // it does exist
    std::list<Plan>::iterator it;
    for(it=flightPlans.begin();it != flightPlans.end(); ++it){
        if (!strcmp(it->getID().c_str(),planID)){
           flightPlans.erase(it);
           break;
        }
    }

    string planName(planID);
    output.setID(planName);
    flightPlans.push_back(output);
}

void PathPlanner::CombinePlan(char planID_A[],char planID_B[],int index){
    Plan *fp1 = GetPlan(planID_A);
    Plan *fp2 = GetPlan(planID_B);
    Plan fp = fp1->copy(); 
    fp.setID("Plan+");
    int fp1len = fp.size();
    bool TODAvail = false;
    for(int i=index;i<fp2->size();++i){
        double delta = fp2->time(i) - fp2->time(i-1);
        std::string info = fp2->getInfo(i);
        std::string name = fp2->getName(i);
        double time = fp.getLastTime() + delta;
        NavPoint oldPoint = fp2->point(i);
        fp.add(oldPoint.position(),time);
        fp.setInfo(fp1len + i-index,info);
        fp.setName(fp1len + i-index,name);

        if (info == "<TOD>"){
            TODAvail = true;
        }
    }

    if(!TODAvail){
       fp.setInfo(fp1len,std::string("<TOD>"));
    }

    flightPlans.push_back(fp);
}

double PathPlanner::Dist2Waypoint(double currPosition[],double nextPosition[]){
    Position A = Position::makeLatLonAlt(currPosition[0],"degree",currPosition[1],"degree",currPosition[2],"m");
    Position B = Position::makeLatLonAlt(nextPosition[0],"degree",nextPosition[1],"degree",nextPosition[2],"m");
    return A.distanceH(B);
}

double PathPlanner::GetApproxElapsedPlanTime(Plan* fp,double currentPos[],int nextWP){
    Position pos = Position::makeLatLonAlt(currentPos[0],"degree",currentPos[1],"degree",currentPos[2],"m");
    double legDistance    = fp->pathDistance(nextWP - 1);
    double legTime        = fp->time(nextWP) - fp->time(nextWP-1);
    double lastWPDistance = fp->point(nextWP-1).position().distanceH(pos);
    double currentTime    = fp->time(nextWP-1) + legTime/legDistance * lastWPDistance;

    return currentTime;
}

void PathPlanner::GetExitPoint(char *planID,double currentPoisition[],int nextWP,double exitPosition[]) {
    Plan* fp;
    fp = GetPlan(planID);

    if (fp == NULL){
        //std::cout<<"Couldn't find plan ID:"<<planID<<std::endl;
        return;
    }

    double elapsedTime = GetApproxElapsedPlanTime(fp,currentPoisition,nextWP);

    bool val = geoCDIIPolygon.detection(*fp,geoPolyPath,elapsedTime,fp->getLastTime());

    Position lastPos;
    if(val) {
        double entryTime = geoCDIIPolygon.getTimeIn(0);
        double exitTime = geoCDIIPolygon.getTimeOut(0);

        Plan cutPlan = PlanUtil::cutDownLinear(*fp, entryTime, exitTime);

        lastPos = cutPlan.getLastPoint().position();
    }else{
        lastPos = fp->point(nextWP).position();
    }

    exitPosition[0] = lastPos.latitude();
    exitPosition[1] = lastPos.longitude();
    exitPosition[2] = lastPos.alt();
}

double PathPlanner:: GetInterceptHeadingToPoint(double positionA[],double positionB[]){
    Position posA = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m");
    Position posB = Position::makeLatLonAlt(positionB[0],"degree",positionB[1],"degree",positionB[2],"m");

    return posA.track(posB)*180/M_PI;
}

void PathPlanner::ClearAllPlans() {
    flightPlans.clear();
}

void PathPlanner::LogInput(){
    log<<"Search type:"<<search<<std::endl; 
    if(search == _ASTAR_ || search == _GRID_){
        log<<"- Enable 3D:"<<_astar_enable3D<<std::endl
           <<"- Grid size:"<<_astar_gridSize<<std::endl
           <<"- Lookahead:"<<_astar_lookahead<<std::endl
           <<"- Resspeed :"<<_astar_resSpeed<<std::endl;
    }else if(search == _RRT_){
        log<<"- Resspeed:"<<_rrt_resSpeed<<std::endl
           <<"- DAA     :"<<_rrt_daaConfig<<std::endl
           <<"- Max itrn:"<<_rrt_maxIterations<<std::endl
           <<"- Mic step:"<<_rrt_dt<<","<<std::endl
           <<"- Mac step:"<<_rrt_macroSteps<<std::endl
           <<"- Cap rad :"<<_rrt_goalCaptureRadius<<std::endl;
    }
    log<<"Start Pos:"<<startPos.toString(8)<<std::endl;
    log<<"Goal Pos:"<<endPos.toString(8)<<std::endl;
    log<<"Start vel:"<<startVel.toString(8)<<std::endl;
    log<<"Num traffic:"<<trafficList.size()<<std::endl;
    for(GenericObject traffic: trafficList){
        Velocity Vel = traffic.vel;
        Position Pos = traffic.pos;
        log<<"- pos: "<<Pos.toString(5)<<std::endl;
        log<<"- vel: "<<Vel.toString(5)<<std::endl;
    }
    log<<"Num Geofence:"<<fenceList.size()<<std::endl;
    for(fence gf: fenceList){
        log<<"- id   : "<<gf.GetID()<<std::endl;
        log<<"- type : "<<gf.GetType()<<std::endl;
        log<<"- floor: "<<gf.GetPoly()->getBottom()<<std::endl;
        log<<"- roof : "<<gf.GetPoly()->getTop()<<std::endl;
        log<<"- numV : "<<gf.GetSize()<<std::endl;
        for(int i = 0; i < gf.GetSize(); ++i){
            log<<"-- vertex: " << gf.GetPoly()->getVertex(i).toString(5)<<std::endl;
        }
    }
    Plan *fp = GetPlan((char*)"Plan0");
    if(fp != NULL){
        char outputPlan[10000];
        log<<"Nominal plan:"<<fp->size()<<std::endl;
        PlanToString((char*)"Plan0",outputPlan,true,-1);
        log<<std::string(outputPlan)<<std::endl;
    }else{
        log<<"Nominal plan: 0"<<std::endl;
    }
    


    struct timeval  tv;
    gettimeofday(&tv, NULL);
    char buffer[100];
    sprintf(buffer,"%f",(double)tv.tv_sec + (double)(tv.tv_usec)/1E6);
    log<<"Time before computation:"<<std::string(buffer)<<std::endl;

    return;
}

Position PathPlanner::GetPositionFromLog(std::ifstream& fp){
       std::string line, data;
       std::getline(fp,line);
       data = split(line,":")[1];
       return Position::parse(data); 
}

Velocity PathPlanner::GetVelocityFromLog(std::ifstream& fp){
       std::string line, data;
       std::getline(fp,line);
       data = split(line,":")[1];
       return Velocity::parse(data); 
}

int PathPlanner::GetIntFromLog(std::ifstream& fp){
       std::string line,data;
       std::getline(fp,line);
       data = split(line,":")[1];
       return stoi(data);
}

double PathPlanner::GetDoubleFromLog(std::ifstream& fp){
       std::string line,data;
       std::getline(fp,line);
       data = split(line,":")[1];
       return stod(data);
}

string PathPlanner::GetStringFromLog(std::ifstream& fp){
       std::string line;
       std::getline(fp,line);
       return split(line,":")[1];
}

string PathPlanner::GetPlanFromLog(std::ifstream& fp){
       std::string line("plan");
       std::string plan;
       while(line != ""){
           line = "";
           std::getline(fp,line);
           plan += line + "\r\n";
       }
       return plan;
}

void PathPlanner::InputDataFromLog(string filename){
    std::ifstream fp;
    fp.open(filename,std::ifstream::in);
    
    if(fp.is_open()){

       search = (algorithm)GetIntFromLog(fp);
       if(search == _ASTAR_ || search == _GRID_){
           bool enable3D = (bool) GetIntFromLog(fp);
           double grid = GetDoubleFromLog(fp);
           double lookahead = GetDoubleFromLog(fp);
           double resSpeed = GetDoubleFromLog(fp);
           UpdateAstarParameters(enable3D,grid,resSpeed,lookahead,(char*)"");
       }else if(search == _RRT_){
           double resSpeed = GetDoubleFromLog(fp);
           string filenm = GetStringFromLog(fp); 
           int maxItn = GetIntFromLog(fp);
           double micStep = GetDoubleFromLog(fp);
           double macStep = GetDoubleFromLog(fp);
           double capR = GetDoubleFromLog(fp);
           UpdateRRTParameters(resSpeed,maxItn,micStep,macStep,capR,(char*)filenm.c_str());
       }

       startPos = GetPositionFromLog(fp); 
       endPos = GetPositionFromLog(fp); 
       startVel = GetVelocityFromLog(fp); 

       std::cout<<startPos.toString()<<std::endl;
       std::cout<<endPos.toString()<<std::endl;
       std::cout<<startVel.toString()<<std::endl;

       int numTraffic = GetIntFromLog(fp); 
        std::cout<<"numTraffic:"<<numTraffic<<std::endl;
       for(int i=0;i<numTraffic;++i){
           Position trafficPos = GetPositionFromLog(fp);
           Velocity trafficVel = GetVelocityFromLog(fp);
           InputTraffic(i,trafficPos,trafficVel);
       }

       int numFences = GetIntFromLog(fp);
       std::cout<<"numFences:"<<numFences<<std::endl;

       for(int i=0; i< numFences; ++i){
            size_t id = GetIntFromLog(fp);
            size_t fenceType = GetIntFromLog(fp); 
            double floor = GetDoubleFromLog(fp);
            double roof = GetDoubleFromLog(fp);
            size_t numV = GetIntFromLog(fp);
            std::list<Position> vertices;

            cout<<id<<","<<fenceType<<","<<floor<<","<<roof<<","<<numV<<std::endl;
            for(int j=0;j<numV;++j){
                Position vertex = GetPositionFromLog(fp);
                vertices.push_back(vertex);
                cout<<vertex.toString()<<std::endl;
            }
            InputGeofenceData(fenceType,id,numV,floor,roof,vertices);
       }

       int nominalPlanSize = GetIntFromLog(fp);
       if (nominalPlanSize > 0){
           string plan0 = GetPlanFromLog(fp);
           StringToPlan((char*)"Plan0",(char*) plan0.c_str());
       }
       double timeA = GetDoubleFromLog(fp);
       double timeB = GetDoubleFromLog(fp);
       cout<<"Computation time:"<<timeA<<","<<timeB<<std::endl;

       GetPlanFromLog(fp);
       fp.close();
    }
    else{
        std::cout<<"Error opening log file:"<<filename<<std::endl;
    }
}