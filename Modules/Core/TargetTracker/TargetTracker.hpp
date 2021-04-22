#ifndef SENSOR_FUSION_HPP
#define SENSOR_FUSION_HPP

#include <string>
#include <vector>
#include <array>
#include <Projection.h>
#include <EuclideanProjection.h>
#include <Position.h>
#include <Core/Interfaces/Interfaces.h>


typedef std::array<std::array<double,3>,3> matrix3x3;

typedef struct{
    std::string callsign;
    int id;
    double time;
    double lastUpdate;
    larcfm::Position position;
    larcfm::Velocity velocity;
    larcfm::Vect3 locPos;
    double sigmaP[6];
    double sigmaV[6];
    double sigma[36];
}measurement;


class TargetTracker{

    private:
       std::string callsign;
       double timeout;
       double modelUncertainty[6];
       larcfm::Position homePos;
       measurement currentState;
       std::vector<measurement> tracks;        
       larcfm::EuclideanProjection proj;
       int CheckValidationGate(measurement& data);
       void UpdateEstimate(measurement& prediction,measurement& value,double time=0);
       int totalTracks;
    public:
        
       TargetTracker(std::string callsign);
       void SetHomePosition(larcfm::Position& home);
       void SetModelUncertainty(double sigma[6]);
       void InputCurrentState(larcfm::Position& pos,larcfm::Velocity& vel);
       void InputMeasurement(measurement& traffic);
       int GetTotalTraffic(double time);
       measurement GetIntruderData(double time,int i);

};


#endif