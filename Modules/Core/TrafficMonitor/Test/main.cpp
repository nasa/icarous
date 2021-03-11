#include <iostream>
#include "SequenceReader.h"
#include "TrafficMonitor.hpp"
#include "TrafficMonitor.h"
#include "DaidalusMonitor.hpp"
#include <vector>
#include <time.h>

int main(int argc,char** argv) {


    
    larcfm::SequenceReader sr = larcfm::SequenceReader(std::string("do365b_default.daa"));
    
    //TrafficMonitor *tfMonitor = new DaidalusMonitor("Ownship","do365b_default.conf",false);
    void *tfMonitor = newDaidalusTrafficMonitor((char*)"Ownship",(char*)"do365b_default.conf",false);
    
    std::vector<double> keys = sr.sequenceKeys();

    double sigmaPos[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    double sigmaVel[6] = {0.0,0.0,0.0,0.0,0.0,0.0};
    larcfm::Velocity winds;
    object intruder;
    for(int i=0;i<keys.size();++i){
        larcfm::Position ownpos = sr.getSequencePosition("Ownship",keys[i]);
        larcfm::Velocity ownvel = sr.getSequenceVelocity("Ownship",keys[i]);
        larcfm::Position tfpos = sr.getSequencePosition("AC1",keys[i]);
        larcfm::Velocity tfvel = sr.getSequenceVelocity("AC1",keys[i]);

        /*
        intruder.callsign = "AC1";
        intruder.id = 1;
        intruder.position = tfpos;
        intruder.velocity = tfvel;
        intruder.time = keys[i];
        std::memcpy(intruder.posSigma,sigmaPos,sizeof(double)*6);
        std::memcpy(intruder.velSigma,sigmaVel,sizeof(double)*6);*/

        struct timespec ts1,ts2;
        clock_gettime(CLOCK_REALTIME,&ts1);
        double time1 = ts1.tv_sec + static_cast<float>(ts1.tv_nsec)/1E9;

        double opos[3] = {ownpos.latitude(),ownpos.longitude(),ownpos.alt()};
        double ovel[3] = {ownvel.track("deg"),ownvel.gs(),ownvel.vs()};

        double tpos[3] = {tfpos.latitude(),tfpos.longitude(),tfpos.alt()};
        double tvel[3] = {tfvel.track("deg"),tfvel.gs(),tfvel.vs()};
        TrafficMonitor_InputIntruderData(tfMonitor,1,(char *)"AC1",tpos,tvel,keys[i],sigmaPos,sigmaVel);
        TrafficMonitor_InputOwnshipData(tfMonitor,opos,ovel,keys[i],sigmaPos,sigmaVel);
        double wind[2] = {0.0,0.0};
        TrafficMonitor_MonitorTraffic(tfMonitor,wind);

        clock_gettime(CLOCK_REALTIME,&ts2);
        double time2 = ts2.tv_sec + static_cast<float>(ts2.tv_nsec)/1E9;
        std::cout<<time2-time1<<std::endl;

        //tfMonitor->InputIntruderData(intruder);
        //tfMonitor->InputOwnshipData(ownpos,ownvel,keys[i],sigmaPos,sigmaVel);
        //tfMonitor->MonitorTraffic(winds);
    }    
}
