#include <iostream>
#include "TrafficMonitor.h"

int main() {

    FlightData fData("../Test/icarous.txt");

    Position so = Position::makeLatLonAlt(37.1021913,"deg", -76.3869528,"deg", 5.0,"m");
    Velocity vo = Velocity::makeTrkGsVs(90.0,"deg",  0.94,"m/s", 0.0,"fpm");

    Position si = Position::makeLatLonAlt(37.10217667,"deg", -76.38679504,"deg", 5.0,"m");
    Velocity vi = Velocity::makeTrkGsVs(270.023339,"deg", 0.5,"m/s", 0.0,"fpm");
    fData.AddTraffic(0,si.latitude(),si.longitude(),si.alt(),vi.y,vi.x,vi.z);

    TrafficMonitor tfMonitor(&fData);

    double position[3] = {so.latitude(),so.longitude(),so.alt()};
    double velocity[3] = {vo.track("degree"),vo.gs(),vo.vs()};
    double resolution[3];
    bool conflict = tfMonitor.MonitorTraffic(false,0,position,velocity,resolution);


    std::cout<<"CONFLICT STATUS:"<<conflict<<std::endl;
    std::cout<<" **** DAA **** "<<std::endl;
    std::cout<<tfMonitor.DAA.toString()<<std::endl;
    std::cout<<" **** KMB **** "<<std::endl;
    std::cout<<tfMonitor.KMB.outputString()<<std::endl;
}