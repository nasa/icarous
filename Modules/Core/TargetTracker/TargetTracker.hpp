#ifndef SENSOR_FUSION_HPP
#define SENSOR_FUSION_HPP

#include <string>
#include <cstring>
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
       double modelUncertaintyP[6];
       double modelUncertaintyV[6];
       double pThreshold;
       double vThreshold;
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
       void SetGateThresholds(double p, double v);
       void UpdatePredictions(double time);
       void SetModelUncertainty(double sigmaP[6],double sigmaV[6]);
       void InputCurrentState(double time,larcfm::Position& pos,larcfm::Velocity& vel,double sigmaP[6],double sigmaV[6]);
       void InputMeasurement(measurement& traffic);
       int GetTotalTraffic();
       measurement GetIntruderData(int i);

};


#endif