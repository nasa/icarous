//
// Created by swee on 6/24/18.
//

#include "UtilFunctions.h"
#include <list>
#include <Position.h>
#include <Velocity.h>
#include <math.h>
#include <EuclideanProjection.h>
#include <Projection.h>
#include <Plan.h>
#include <TrajGen.h>
#include <Units.h>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <ctime>
#include <StateReader.h>
#include <PlanReader.h>
#include <PlanWriter.h>
#include <TrajGen.h>
#include <iomanip>
#include "WP2Plan.hpp"

using namespace larcfm;

double ComputeDistance(double positionA[],double positionB[]){

    Position A = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m");
    Position B = Position::makeLatLonAlt(positionB[0],"degree",positionB[1],"degree",positionB[2],"m");

    return A.distanceH(B);
}

double ComputeHeading(double positionA[],double positionB[]){

    Position A = Position::makeLatLonAlt(positionA[0],"degree",positionA[1],"degree",positionA[2],"m/s");
    Position B = Position::makeLatLonAlt(positionB[0],"degree",positionB[1],"degree",positionB[2],"m/s");

    return Units::to(Units::deg,A.track(B));
}

void ComputeOffsetPosition(double position[],double track,double dist,double output[]){
    Position A = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m/s");

    track = Units::from(Units::deg,track);
    dist = Units::from(Units::m,dist);

    Position B = A.linearDist2D(track,dist);

    output[0] = B.latitude();
    output[1] = B.longitude();
    output[2] = B.alt();
}

void ConvertLLA2END(double gpsOrigin[],double LLA[],double outputEND[]){
   Position origin = Position::makeLatLonAlt(gpsOrigin[0],"degree",gpsOrigin[1],"degree",gpsOrigin[2],"m");
   Position query = Position::makeLatLonAlt(LLA[0],"degree",LLA[1],"degree",LLA[2],"m");
   EuclideanProjection proj = Projection::createProjection(origin);
   Vect3 output = proj.project(query);
   outputEND[0] = output.x;
   outputEND[1] = output.y;
   outputEND[2] = output.z;
}

void ConvertEND2LLA(double gpsOrigin[],double END[],double outputLLA[]){
    Position origin = Position::makeLatLonAlt(gpsOrigin[0],"degree",gpsOrigin[1],"degree",gpsOrigin[2],"m");
    EuclideanProjection proj = Projection::createProjection(origin);
    Vect3 query = Vect3::makeXYZ(END[0],"m",END[1],"m",END[2],"m");
    LatLonAlt output = proj.inverse(query);
    outputLLA[0] = output.latitude();
    outputLLA[1] = output.longitude();
    outputLLA[2] = gpsOrigin[2] - END[2];
}


void ConvertRAE2LLA(double lat,double lon,double heading,double range,double azimuth,double elevation,double outputLL[]){
    double az_rad   = azimuth * M_PI/180;
    double elev_rad = elevation * M_PI/180;
    double heading_rad = heading * M_PI/180;

    double N,E,U;

    N = range * cos(elev_rad) * cos(heading_rad + az_rad);
    E = range * cos(elev_rad) * sin(heading_rad + az_rad);
    U = range * sin(elev_rad);

    double origin[3] = {lat,lon,0};
    double END[3]    = {E,N,U};
    ConvertEND2LLA(origin,END,outputLL);
}

void ConvertVnedToTrkGsVs(double vn,double ve,double vz,double *Trk,double *Gs,double *Vs){
   double angle = 360 + atan2(ve, vn) * 180 / M_PI;
   *Trk = fmod(angle, 360);
   *Gs = sqrt(pow(vn, 2) + pow(ve, 2));
   *Vs = -vz;
}

void ConvertTrkGsVsToVned(double Trk,double Gs,double Vs,double *vn,double *ve,double *vd){
    Velocity vel = Velocity::makeTrkGsVs(Trk, "degree", Gs, "m/s", Vs, "m/s");
    *vn = vel.y;
    *ve = vel.x;
    *vd = -vel.z;
}

void ComputeLatLngAlt(double origin[],double xyz[],double output[]){
    Position CurrentPos = Position::makeLatLonAlt(origin[0],"degree",origin[1],"degree",origin[2],"m");
    EuclideanProjection proj = Projection::createProjection(CurrentPos);
    const Vect3 cartPos(xyz[0],xyz[1],xyz[2]);
    LatLonAlt transformedPos = proj.inverse(cartPos);
    output[0] = transformedPos.latitude();
    output[1] = transformedPos.longitude();
    output[2] = transformedPos.alt();
}


void ComputeTrackingResolution(double targetPos[],double currentPos[],double currentVel[],double heading,double distH,double distV,
                                      double PropGains[],double outputVel[],double* outputHeading){

    // Get coordinates of object to track
    Position CurrentPos = Position::makeLatLonAlt(currentPos[0],"degree",currentPos[1],"degree",currentPos[2],"m");
    Position Target     = Position::makeLatLonAlt(targetPos[0],"degree",targetPos[1],"degree",targetPos[2],"m");
    double distHx  = distH*sin(heading *M_PI/180); // Heading is measured from North
    double distHy  = distH*cos(heading *M_PI/180);

    double Kx_trk = PropGains[0];
    double Ky_trk = PropGains[1];
    double Kz_trk = PropGains[2];

    Vect3 delPos(distHx,distHy,distV);

    // Project from LatLonAlt to cartesian coordinates

    EuclideanProjection proj = Projection::createProjection(CurrentPos.mkAlt(0));
    Vect3 vecCP  =  proj.project(CurrentPos);
    Vect3 vecTP  =  proj.project(Target);

    Vect3 vecTPf =  vecTP.Add(delPos);

    // Relative vector to object
    Vect3 Rel    = vecTPf.Sub(vecCP);

    // Compute velocity commands that will enure smooth tracking of object
    double dx = Rel.x;
    double dy = Rel.y;
    double dz = Rel.z;

    // Velocity is proportional to distance from object.
    double Vx = SaturateVelocity(Kx_trk * dx,2.0);
    double Vy = SaturateVelocity(Ky_trk * dy,2.0);
    double Vz = SaturateVelocity(Kz_trk * dz,2.0);

    //printf("vecCP:%f,%f,%f\n",vecCP.x,vecCP.y,vecCP.z);
    //printf("vecTPf:%f,%f,%f\n",vecTPf.x,vecTPf.y,vecTPf.z);
    //printf("dx,dy,dz = %f,%f,%f\n",dx,dy,dz);

    double RefHeading = atan2(Vx,Vy) * 180/M_PI;

    if(RefHeading < 0){
        RefHeading = 360 + RefHeading;
    }

    outputVel[0] = Vy;
    outputVel[1] = Vx;
    outputVel[2] = -Vz;

    *outputHeading = RefHeading;

    //printf("Heading = %f, Velocities %1.3f, %1.3f, %1.3f\n",heading,Vx,Vy,Vz);
}

double SaturateVelocity(double V, double Vsat){
    if(abs(V) > Vsat){
        return sign(V)*Vsat;
    }
    else{
        return V;
    }
}

double ComputeXtrackDistance(double wpA[],double wpB[],double position[],double offset[]){

    Position pos        = Position::makeLatLonAlt(position[0],"degree",
                                                  position[1],"degree",
                                                  position[2],"m");

    Position PrevWP     = Position::makeLatLonAlt(wpA[0],"degree",
                                                  wpA[1],"degree",
                                                  wpA[2],"m");

    Position NextWP     = Position::makeLatLonAlt(wpB[0],"degree",
                                                  wpB[1],"degree",
                                                  wpB[2],"m");

    double psi1         = PrevWP.track(NextWP) * 180/M_PI;
    double psi2         = PrevWP.track(pos) * 180/M_PI;
    double sgn          = 0;

    if( (psi1 - psi2) >= 0){
        sgn = 1;              // Vehicle left of the path
    }
    else if( (psi1 - psi2) <= 180){
        sgn = -1;             // Vehicle right of the path
    }
    else if( (psi1 - psi2) < 0 ){
        sgn = -1;             // Vehicle right of path
    }
    else if ( (psi1 - psi2) >= -180  ){
        sgn = 1;              // Vehicle left of path
    }

    double bearing = std::abs(psi1 - psi2);
    double dist = PrevWP.distanceH(pos);
    double crossTrackDeviation = sgn*dist*sin(bearing * M_PI/180);
    double crossTrackOffset    = dist*cos(bearing * M_PI/180);

    if(offset != NULL){
        offset[0] = crossTrackDeviation;
        offset[1] = crossTrackOffset;
    }

    return crossTrackDeviation;
}

void GetPositionOnPlan(double wpA[],double wpB[],double currentPos[],double position[]){

    double offsets[2];
    ComputeXtrackDistance(wpA,wpB,currentPos,offsets);

    Position prevWP     = Position::makeLatLonAlt(wpA[0],"degree",
                                                  wpA[1],"degree",
                                                  wpA[2],"m");

    Position nextWP     = Position::makeLatLonAlt(wpB[0],"degree",
                                                  wpB[1],"degree",
                                                  wpB[2],"m");

    double headingNextWP  = prevWP.track(nextWP);;
    double dn             = offsets[1]*cos(headingNextWP);
    double de             = offsets[1]*sin(headingNextWP);
    Position cp           = prevWP.linearEst(dn, de);

    if(cp.alt() <= 0){
        cp = cp.mkAlt(nextWP.alt());
    }

    position[0] = cp.latitude();
    position[1] = cp.longitude();
    position[2] = cp.altitude();
}

void ManueverToIntercept(double wpA[],double wpB[],double currPosition[],double velocity[],
                        double xtrkDevGain,double resolutionSpeed,double allowedDev){

    double Vs,Vf,V,sgn;
    double Trk;
    Position currentPos,cp;

    Position prevWP     = Position::makeLatLonAlt(wpA[0],"degree",
                                                  wpA[1],"degree",
                                                  wpA[2],"m");

    Position nextWP     = Position::makeLatLonAlt(wpB[0],"degree",
                                                  wpB[1],"degree",
                                                  wpB[2],"m");

    double offsets[2];
    ComputeXtrackDistance(wpA,wpB,currPosition,offsets);
    double crossTrackDeviation = offsets[0];

    if(xtrkDevGain < 0){
        xtrkDevGain = -xtrkDevGain;
    }

    Vs = xtrkDevGain*crossTrackDeviation;
    V  = resolutionSpeed;

    if(Vs >= 0){
        sgn = 1;
    }
    else{
        sgn = -1;
    }

    if(pow(std::abs(Vs),2) >= pow(V,2)){
        Vs = V*sgn;
    }

    Vf = sqrt(pow(V,2) - pow(Vs,2));

    Trk = prevWP.track(nextWP);
    double Vn = Vf*cos(Trk) - Vs*sin(Trk);
    double Ve = Vf*sin(Trk) + Vs*cos(Trk);
    double Vu = 0;

    double track = atan2(Ve,Vn)*180/M_PI;
    if(track < 0){
        track = 360 + track;
    }

    velocity[0] = track;
    velocity[1] = resolutionSpeed;
    velocity[2] = 0;
}

double GetInterceptHeadingToPlan(double wpA[],double wpB[],double currentPos[]){

    Position prevWP     = Position::makeLatLonAlt(wpA[0],"degree",
                                                  wpA[1],"degree",
                                                  wpA[2],"m");

    Position nextWP     = Position::makeLatLonAlt(wpB[0],"degree",
                                                  wpB[1],"degree",
                                                  wpB[2],"m");

    Position pos = Position::makeLatLonAlt(currentPos[0],"degree",currentPos[1],"degree",currentPos[2],"m");

    double _positiontOnPlan[3];
    GetPositionOnPlan(wpA,wpB,currentPos,_positiontOnPlan);
    Position goal = Position::makeLatLonAlt(_positiontOnPlan[0],"degree",_positiontOnPlan[1],"degree",_positiontOnPlan[2],"m");
    return pos.track(goal)*180/M_PI;
}

void ComputeWaypointsETA(double scenarioTime,int numWP, double wpSpeed[], waypoint_t wpts[]){
    // Compute time of arrival at each waypoint based on speed
    // NOTE: This assumes there are no overlapping types ot TCPs
    double prevRadius = 0.0;
    for (int i = 0; i < numWP; ++i) {
        if (i == 0) {
            wpts[i].time = scenarioTime;
        }
        else {
            waypoint_t &prev = wpts[i - 1];
            waypoint_t &next = wpts[i];
            Position prevWP = Position::makeLatLonAlt(prev.latitude, "degree", prev.longitude, "degree", prev.altitude, "m");
            Position nextWP = Position::makeLatLonAlt(next.latitude, "degree", next.longitude, "degree", next.altitude, "m");
            double distH = prevWP.distanceH(nextWP);
            double dt = distH / wpSpeed[i - 1];

            if(prev.tcp[0] == TCP_BOT){
                 prevRadius = std::fabs(prev.tcpValue[0]);
            }

            if(next.tcp[0] == TCP_EOTBOT || next.tcp[0] == TCP_EOT || next.tcp[0] == TCP_MOT){
                double angle = 2*std::asin(distH/(2*prevRadius));
                double turnRate = wpSpeed[i]/prevRadius;
                double dt = angle/turnRate;
                next.time = prev.time + dt;
            }else{
                next.time = prev.time + dt;
            }
        }
    }
}

void ConvertWPList2Plan(larcfm::Plan* fp,const std::string &plan_id, const std::list<waypoint_t> &waypoints, const double initHeading,bool repair,double turnRate){
   int count = 0;
   for(auto waypt: waypoints){
       double eta = waypt.time;
       larcfm::Position pos = larcfm::Position::makeLatLonAlt(waypt.latitude,"degree",
                                                              waypt.longitude,"degree",
                                                              waypt.altitude,"m");
       fp->add(pos,eta);
       bool tcp1Available = true;
       bool tcp2Available = true;
       bool tcp3Available = true;
       if(waypt.tcp[0] == TCP_BOT){
           double startHeading = initHeading;
           if(count > 0){
               startHeading = larcfm::Units::to(larcfm::Units::deg,fp->trkIn(count));
           }   
           double turn = waypt.tcpValue[0] > 0?90:-90;
           larcfm::Position center = pos.linearDist2D((startHeading+turn)*M_PI/180,fabs(waypt.tcpValue[0]));
           fp->addBOT(count,waypt.tcpValue[0],center);
       }else if(waypt.tcp[0] == TCP_MOT){
           fp->getTcpDataRef(count).setMOT(true);
       }else if(waypt.tcp[0] == TCP_EOT){
           fp->addEOT(count); 
       }else if(waypt.tcp[0] == TCP_EOTBOT){
            double startHeading = larcfm::Units::to(larcfm::Units::deg,fp->trkIn(count));
            double turn = waypt.tcpValue[0] > 0?90:-90;
            larcfm::Position center = pos.linearDist2D((startHeading+turn)*M_PI/180,fabs(waypt.tcpValue[0]));
            fp->addEOT(count);
            fp->addBOT(count,waypt.tcpValue[0],center);
       }else{
           tcp1Available = false;
       }

       if(waypt.tcp[1] ==  TCP_BGS){
            fp->setBGS(count,waypt.tcpValue[1]); 
       }else if(waypt.tcp[1] == TCP_EGS){
            fp->setEGS(count); 
       }else{
           tcp2Available = false;
       }

       if(waypt.tcp[2] == TCP_BVS){
            fp->setBVS(count,waypt.tcpValue[2]); 
       }else if(waypt.tcp[2] == TCP_EVS){
            fp->setEVS(count);
       }else{
           tcp3Available = false;
       }

       if(tcp1Available || tcp2Available || tcp3Available){
           fp->getTcpDataRef(count).setInformation(std::string(waypt.info));
       }

       count++;
   }

   if(repair){
       double speed = fp->gsOut(1);
       double bankAngle = larcfm::Kinematics::bankAngle(speed,turnRate*M_PI/180);
       double turnRadius= larcfm::Kinematics::turnRadius(speed,bankAngle);
       *fp = larcfm::TrajGen::makeKinematicPlan(*fp,bankAngle,2,1.47,true,true,true);
   }
}

void GetWaypointFromPlan(const larcfm::Plan* fp,const int id,waypoint_t &wp){
       larcfm::Position pos = fp->getPos(id);
       double time = fp->time(id);
       wp.time = time; 
       wp.latitude = pos.latitude();
       wp.longitude = pos.longitude();
       wp.altitude = pos.alt();
       if(fp->isBOT(id) && fp->isEOT(id)){
           wp.tcp[0] = TCP_EOTBOT;
           wp.tcpValue[0] = fp->getTcpData(id).getRadiusSigned();
       }else if(fp->isBOT(id)){
           wp.tcp[0] = TCP_BOT;
           wp.tcpValue[0] = fp->getTcpData(id).getRadiusSigned();
       }else if(fp->isEOT(id)){
           wp.tcp[0] = TCP_EOT;
           wp.tcpValue[0] = fp->getTcpData(fp->prevTRK(id)).getRadiusSigned();
       }else if(fp->isMOT(id)){
           wp.tcp[0] = TCP_MOT;
           int previd = fp->prevTrkTCP(id);
           wp.tcpValue[0] = fp->getTcpData(previd).getRadiusSigned();
       }else{
           wp.tcp[0] = TCP_NONE;
       }

       if(fp->isBGS(id)){
           wp.tcp[1] = TCP_BGS;
           wp.tcpValue[1] = fp->getTcpData(id).getGsAccel();
       }else if(fp->isEGS(id)){
           wp.tcp[1] = TCP_EGS;
       }else{
           wp.tcp[1] = TCP_NONEg;
       }

       if(fp->isBVS(id)){
           wp.tcp[2] = TCP_BVS;
           wp.tcpValue[2] = fp->getTcpData(id).getVsAccel();
       }else if(fp->isEVS(id)){
           wp.tcp[2] = TCP_EVS;
       }else{
           wp.tcp[2] = TCP_NONEv;
       }
       strcpy(wp.info,fp->getTcpData(id).getInformation().c_str());
}

int ParseParameterFile(char filename[],ParsedParam_t params[]){
    std::ifstream in;
    in.open(filename);
    if (!in.is_open()){
        return -1;
    }

    larcfm::StateReader reader;
    reader.open(&in);
    larcfm::ParameterData parameters;
    reader.updateParameterData(parameters);

    int i = 0;
    for(auto &key: parameters.getKeyList()){
        std::strcpy(params[i].key,key.c_str());
        std::strcpy(params[i].valueString,parameters.getString(key).c_str());
        std::strcpy(params[i].unitString,parameters.getUnit(key).c_str());
        params[i].value = parameters.getValue(key);
        i++;
    }

    return i;
}

void IsolateEUTLPlans(char filename[],char prefix[],bool zeroTimeStart,bool randomizeStart){
    larcfm::PlanReader planReader;
    larcfm::PlanWriter planWriter;
    planReader.open(std::string(filename));
    srand(time(NULL)); 
    int n = planReader.size();
    for(int i=0;i<n;++i){
        larcfm::Plan plan = planReader.getPlan(i);
        if(zeroTimeStart){
            double time0 = plan.time(0);
            plan.timeShiftPlan(0,-time0);
        }
        
        double randStart = 0;
        if(randomizeStart){
            randStart = rand() % 50 - 50; 
        }
        plan.timeShiftPlan(0,randStart);
        std::string filename = std::string(prefix) + to_string(i) + ".eutl";
        std::vector<larcfm::Plan> planlist;
        std::vector<larcfm::PolyPath> polyPath;
        planlist.push_back(plan);
        planWriter.write(filename,planlist,polyPath,true);
    }    
}

int GetEUTLPlanFromFile(char filename[],int id,waypoint_t waypoints[],bool linearize,double timeshift){
    larcfm::PlanReader planReader;
    planReader.open(std::string(filename));
    if(id < planReader.size()){
        larcfm::Plan plan = planReader.getPlan(id);
        plan.timeShiftPlan(0,timeshift);
        larcfm::Plan modPlan;
        if(linearize){
             modPlan = larcfm::TrajGen::makeLinearPlan(plan);
        }else{
            modPlan = plan; 
        }
        int n = modPlan.size();
        for(int i=0;i<n;++i){

            GetWaypointFromPlan(&modPlan,i,waypoints[i]);
        }
        return n;
    }else{
        return 0;
    }
}

void* GetPlanPosition(void* planIn,waypoint_t wpts[],int planlen,double t,double position[]){

   if((larcfm::Plan*)planIn  == nullptr){
   larcfm::Plan *plan = new Plan();
   
   for(int i=0;i<planlen;++i){
       double lat = wpts[i].latitude;
       double lon = wpts[i].longitude;
       double alt = wpts[i].altitude;
       larcfm::Position wp = larcfm::Position::makeLatLonAlt(lat,"degree",lon,"degree",alt,"m");
       larcfm::NavPoint np = larcfm::NavPoint(wp,wpts[i].time);
       larcfm::Position center;
       std::string trackTCP,gsTCP,vsTCP; 
       switch(wpts[i].tcp[0]){
            case TCP_BOT:
            case TCP_EOTBOT:{
                if(wpts[i].tcp[0] == TCP_BOT)
                    trackTCP = "BOT";
                else
                    trackTCP = "EOTBOT";
                double sign;
                if (wpts[i].tcpValue[0] > 0)
                    sign = 1;
                else
                    sign = -1;
                double hdg;
                hdg = plan->getLastPoint().position().track(wp) + sign * M_PI_2;
                /*
                if (plan->size() > 1){
                    hdg = plan->trkFinal(i - 2, false) + sign * M_PI_2;
                }else{
                    hdg = plan->getLastPoint().position().track(wp) + sign * M_PI_2;
                }*/
                center = wp.linearDist2D(hdg, fabs(wpts[i].tcpValue[0]));
                break;
            }
            case TCP_MOT:
                 trackTCP = "MOT";
                 break;
            case TCP_EOT:
                 trackTCP = "EOT";
                 break;
            default:
                 trackTCP = "NONE";
       }

       switch(wpts[i].tcp[1]){
            case TCP_BGS:
                 gsTCP = "BGS";
                 break;
            case TCP_EGS:
                 gsTCP = "EGS";
                 break;
            case TCP_EGSBGS:
                 gsTCP = "EGSBGS";
                 break;
            default:
                 gsTCP = "NONE";
       }

        switch(wpts[i].tcp[2]){
            case TCP_BVS:
                 vsTCP = "BVS";
                 break;
            case TCP_EVS:
                 vsTCP = "EVS";
                 break;
            case TCP_EVSBVS:
                 vsTCP = "EVSBVS";
                 break;
            default:
                 vsTCP = "NONE";
       }

       larcfm::TcpData tcpData = larcfm::TcpData::makeFull("Orig",trackTCP,gsTCP,vsTCP,wpts[i].tcpValue[0],center,wpts[i].tcpValue[1],wpts[i].tcpValue[2]);
       plan->add(np,tcpData);
       
   }

       planIn = plan;
   }
   larcfm::Position output = static_cast<larcfm::Plan*>(planIn)->position(t);
   position[0] = output.latitude();
   position[1] = output.longitude();
   position[2] = output.alt();
   return planIn;
}

trajTimeFunction ConvertEUTL2TimeFunction(const larcfm::Plan* fp){

    const int n = fp->size();
    larcfm::EuclideanProjection projection = larcfm::Projection::createProjection(fp->getPos(0));
    double accel = 0;
    double R = 0;
    larcfm::Vect3 center;
    trajTimeFunction trajmap;
    std::function<double(double)> psi, omega, fX, fY, fVx, fVy, fV, fA, falpha;
    bool turn = false;
    for(int i=1;i<n;++i){
       larcfm::TcpData tcpA = fp->getTcpData(i-1); 
       larcfm::TcpData tcpB = fp->getTcpData(i); 
       larcfm::Vect3 start = projection.project(fp->getPos(i-1));
       double t0 = fp->time(i-1);
       double trk = fp->trkOut(i-1);
       double gs = fp->gsOut(i-1);
       
       if(tcpA.isBGS()){
         accel  = tcpA.getGsAccel(); 
       }else if(tcpA.isEGS()){
         accel = 0.0;
       }

       if(tcpA.isBOT()){
          turn = true;
          center = projection.project(tcpA.turnCenter());
          R = tcpA.getRadiusSigned();
       }else if(tcpA.isEOT()){
          turn = false;
       }

       if(!turn){
           fX     = [=] (double t) { t = t - t0; return start.x + gs*sin(trk) * t + accel*sin(trk)*t*t/2;};
           fY     = [=] (double t) { t = t - t0; return start.y + gs*cos(trk) * t + accel*cos(trk)*t*t/2;};
           //fVx    = [=] (double t) {return gs*sin(trk) + accel*sin(trk)*(t-t0);};
           //fVy    = [=] (double t) {return gs*cos(trk) + accel*cos(trk)*(t-t0);};
           fV     = [=] (double t) {return gs + accel*(t-t0);};
           psi    = [=] (double t) {return trk;};
           omega  = [=] (double t) {return 0;};
           fA     = [=] (double t) {return accel;};
           falpha = [=] (double t) {return 0;};
       }else{
           psi      = [=] (double t) { t = t - t0;  return trk + gs*t/R + accel*t*t/(2*R);};
           omega    = [=] (double t) { return (accel*(t-t0) + gs)/R;};
           fX       = [=] (double t) { return -R*cos(psi(t)) + center.x; };
           fY       = [=] (double t) { return R*sin(psi(t)) + center.y; };
           //fVx      = [=] (double t) { return accel*(t-t0)*sin(psi(t)) + gs*sin(psi(t));};
           //fVy      = [=] (double t) { return accel*(t-t0)*cos(psi(t)) + gs*cos(psi(t));};
           fV       = [=] (double t) {return gs + accel*(t-t0);};
           fA       = [=] (double t) {return accel;};
           falpha   = [=] (double t) {return 0;};
       }
       
       std::vector<std::function<double(double)>> trajelem = {fX,fY,fV,psi,omega,fA,falpha};
       trajmap.push_back(trajelem);
    }

    return trajmap;
}