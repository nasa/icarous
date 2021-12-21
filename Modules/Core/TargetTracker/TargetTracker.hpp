/**
 * @file TargetTracker.hpp
 * @brief Sensor fusion and tracking implementation
 * 
 */

#ifndef SENSOR_FUSION_HPP
#define SENSOR_FUSION_HPP

#include <string>
#include <cstring>
#include <vector>
#include <array>
#include <fstream>
#include <Projection.h>
#include <EuclideanProjection.h>
#include <Position.h>
#include <Core/Interfaces/Interfaces.h>


typedef std::array<std::array<double,3>,3> matrix3x3;

/**
 * @struct measurement
 * @brief datastructure to hold measurement data
 */
typedef struct{
    std::string callsign;      ///< callsign of traffic 
    int id;                    ///< numerical id
    double time;               ///< measurement timestamp
    double lastUpdate;         ///< last updated timestamp
    larcfm::Position position; ///< position 
    larcfm::Velocity velocity; ///< velocity
    larcfm::Vect3 locPos;      ///< position in local coordinate
    double sigmaP[6];          ///< positional covariance values xx,yy,zz,xy,xz,yz
    double sigmaV[6];          ///< velocity covariance values xx,yy,zz,xy,xz,yz
    double sigma[36];          ///< covariance for combined state [x,Vx,y,Vy,z,Vz]
}measurement;


/**
 * @brief TargetTracker
 * 
 */
class TargetTracker{

    private:
       std::string callsign;             ///< ownship callsign
       std::ofstream logFile;            ///< tracker log file
       bool log;                         ///< log enable status
       double timeout;                   ///< track timeout
       double modelUncertaintyP[6];      ///< position uncertainty associated with prediction model
       double modelUncertaintyV[6];      ///< velocity uncertainty associated with velocity model
       double pThreshold;                ///< chi2 threshold for mahalanobis distance - position error
       double vThreshold;                ///< chi2 threshold for mahalanobis distance - velocity error
       larcfm::Position homePos;         ///< initial position - use to produce NED projections
       measurement currentState;         ///< current state of ownship
       std::vector<measurement> tracks;  ///< tracked measurements
       larcfm::EuclideanProjection proj; ///< projection
       int totalTracks;                  ///< total tracks
       double prevLogTime;               ///< previous log time

       /**
        * @brief Check if mahalanobis distance criteria is satisfied
        * 
        * @param data input measurement
        * @return int index of track the input data is associated with. -1 if no associations are found
        */
       int CheckValidationGate(measurement& data); 

       /**
        * @brief Perform filter updates
        * 
        * @param prediction previous know measurement
        * @param value new measurement to fuse into update
        * @param onlyPrediction perform only prediction
        * @param time time to be used for updatess if not available in the measurement
        */
       void UpdateEstimate(measurement& prediction,measurement& value,bool onlyPrediction=false,double time=0);
       
    public:
       /**
        * @brief Construct a new Target Tracker object
        * 
        * @param callsign 
        * @param configFile 
        */
       TargetTracker(std::string callsign,std::string configFile);

       /**
        * @brief Read parameters from file
        * 
        * @param configFile 
        */
       void ReadParamFromFile(std::string configFile);

       /**
        * @brief Set the Home Position object
        * 
        * @param home 
        */
       void SetHomePosition(larcfm::Position& home);

       /**
        * @brief Set the chi2 gating thresholds
        * 
        * @param p position error threshold
        * @param v velocity error threshold
        */
       void SetGateThresholds(double p, double v);

       /**
        * @brief Update track predictions for given time
        * 
        * @param time 
        */
       void UpdatePredictions(double time);

       /**
        * @brief Set the Model Uncertainty 
        * 
        * @param sigmaP xx,yy,zz,xy,xz,yz
        * @param sigmaV xx,yy,zz,xy,xz,yz
        */
       void SetModelUncertainty(double sigmaP[6],double sigmaV[6]);

       /**
        * @brief Input ownship information
        * 
        * @param time 
        * @param pos 
        * @param vel 
        * @param sigmaP covariances for xx,yy,zz,xy,xz,yz
        * @param sigmaV covariances for xx,yy,zz,xy,xz,yz
        */
       void InputCurrentState(double time,larcfm::Position& pos,larcfm::Velocity& vel,double sigmaP[6],double sigmaV[6]);

       /**
        * @brief Input measurement data
        * 
        * @param traffic 
        */
       void InputMeasurement(measurement& traffic);

       /**
        * @brief Get the total traffic 
        * 
        * @return int 
        */
       int GetTotalTraffic();

       /**
        * @brief Get intruder 
        * 
        * @param i starts with 0. 0th index is first traffic track
        * @return measurement 
        */
       measurement GetIntruderData(int i);

       /**
        * @brief Get track data at given index
        * 
        * @param i starts with 0. 0th index is ownship
        * @return measurement 
        */
       measurement GetData(int i);

};


#endif