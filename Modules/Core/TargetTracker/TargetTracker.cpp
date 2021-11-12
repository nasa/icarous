#include <Eigen/Dense>
#include <TargetTracker.hpp>
#include <TargetTracker.h>
#include <StateReader.h>
#include <ParameterData.h>

TargetTracker::TargetTracker(std::string name,std::string configFile){
    callsign = name;
    timeout = 5;
    totalTracks = 0;
    prevLogTime = -1;
    ReadParamFromFile(configFile);
}

void TargetTracker::ReadParamFromFile(std::string configFile){
   larcfm::StateReader reader;
   larcfm::ParameterData parameters;
   reader.open(configFile);
   reader.updateParameterData(parameters);
   timeout              = parameters.getValue("track_timeout");
   modelUncertaintyP[0] = parameters.getValue("pos_model_uncertainty_xx");
   modelUncertaintyP[1] = parameters.getValue("pos_model_uncertainty_yy");
   modelUncertaintyP[2] = parameters.getValue("pos_model_uncertainty_zz");
   modelUncertaintyP[3] = parameters.getValue("pos_model_uncertainty_xy");
   modelUncertaintyP[4] = parameters.getValue("pos_model_uncertainty_yz");
   modelUncertaintyP[5] = parameters.getValue("pos_model_uncertainty_xz");
   modelUncertaintyV[0] = parameters.getValue("vel_model_uncertainty_xx");
   modelUncertaintyV[1] = parameters.getValue("vel_model_uncertainty_yy");
   modelUncertaintyV[2] = parameters.getValue("vel_model_uncertainty_zz");
   modelUncertaintyV[3] = parameters.getValue("vel_model_uncertainty_xy");
   modelUncertaintyV[4] = parameters.getValue("vel_model_uncertainty_yz");
   modelUncertaintyV[5] = parameters.getValue("vel_model_uncertainty_xz");
   pThreshold           = parameters.getValue("pos_chi_threshold");
   vThreshold           = parameters.getValue("vel_chi_threshold");
   log                  = parameters.getBool("tracker_log");
   if(log && !logFile.is_open()){
       logFile.open("Tracker.log");

       logFile<<"time,callsign,x,y,z,lat,lon,alt,trk,gs,vs,sxx,syy,szz,sxy,syz,sxz,vxx,vyy,vzz,vxy,vyz,vxz"<<std::endl;
   }
}

void TargetTracker::SetGateThresholds(double p, double v){
    pThreshold = p;
    vThreshold = v;
}

void TargetTracker::SetHomePosition(larcfm::Position& home){
    homePos = home;
    proj = larcfm::Projection::createProjection(homePos);
}

void TargetTracker::SetModelUncertainty(double sigmaP[6],double sigmaV[6]){
    std::memcpy(modelUncertaintyP,sigmaP,sizeof(double)*6);
    std::memcpy(modelUncertaintyV,sigmaV,sizeof(double)*6);
}

void TargetTracker::InputCurrentState(double time,larcfm::Position& pos,larcfm::Velocity& vel,double sigmaP[6],double sigmaV[6]){
    currentState.time = time;
    currentState.position = pos;
    currentState.velocity = vel;
    currentState.callsign = callsign;
    currentState.locPos = proj.project(pos);
    currentState.sigmaP[0] = sigmaP[0];
    currentState.sigmaP[1] = sigmaP[1];
    currentState.sigmaP[2] = sigmaP[2];
    currentState.sigmaP[3] = sigmaP[3];
    currentState.sigmaP[4] = sigmaP[4];
    currentState.sigmaP[5] = sigmaP[5];

    currentState.sigmaV[0] = sigmaV[0];
    currentState.sigmaV[1] = sigmaV[1];
    currentState.sigmaV[2] = sigmaV[2];
    currentState.sigmaV[3] = sigmaV[3];
    currentState.sigmaV[4] = sigmaV[4];
    currentState.sigmaV[5] = sigmaV[5];

    currentState.sigma[0*6+0] = currentState.sigmaP[0]; 
    currentState.sigma[2*6+2] = currentState.sigmaP[1]; 
    currentState.sigma[4*6+4] = currentState.sigmaP[2]; 
    currentState.sigma[0*6+2] = currentState.sigmaP[3]; 
    currentState.sigma[2*6+0] = currentState.sigmaP[3]; 
    currentState.sigma[0*6+4] = currentState.sigmaP[4]; 
    currentState.sigma[4*6+0] = currentState.sigmaP[4]; 
    currentState.sigma[2*6+4] = currentState.sigmaP[5]; 
    currentState.sigma[4*6+2] = currentState.sigmaP[5]; 
    
    currentState.sigma[1*6+1] = currentState.sigmaV[0];
    currentState.sigma[3*6+3] = currentState.sigmaV[1];
    currentState.sigma[5*6+5] = currentState.sigmaV[2];
    currentState.sigma[1*6+3] = currentState.sigmaV[3];
    currentState.sigma[3*6+1] = currentState.sigmaV[3];
    currentState.sigma[1*6+5] = currentState.sigmaV[4];
    currentState.sigma[5*6+1] = currentState.sigmaV[4];
    currentState.sigma[3*6+5] = currentState.sigmaV[5];
    currentState.sigma[5*6+3] = currentState.sigmaV[5];


    std::memset(currentState.sigma,0,sizeof(double)*36);

    if(tracks.size() == 0){
        tracks.push_back(currentState);
    }else{
        tracks[0] = currentState;
    }
}

int TargetTracker::CheckValidationGate(measurement& value){
    int n = tracks.size();

    for(int i=0;i<n;++i){
        measurement &candidate = tracks[i];
        Eigen::Matrix3f posSigma;
        Eigen::Matrix2f velSigma;
        Eigen::Vector3f errorP;
        Eigen::Vector2f errorV;

        errorP(0) = value.locPos.x - candidate.locPos.x;
        errorP(1) = value.locPos.y - candidate.locPos.y;
        errorP(2) = value.locPos.z - candidate.locPos.z;
        errorV(0) = value.velocity.x - candidate.velocity.x;
        errorV(1) = value.velocity.y - candidate.velocity.y;

        posSigma.setZero();
        posSigma(0,0) =value.sigmaP[0] + candidate.sigma[0*6+0];
        posSigma(1,1) =value.sigmaP[1] + candidate.sigma[2*6+2];
        posSigma(2,2) =value.sigmaP[2] + candidate.sigma[4*6+4];
        posSigma(0,1) =value.sigmaP[3] + candidate.sigma[0*6+2];
        posSigma(1,0) =value.sigmaP[3] + candidate.sigma[2*6+0];
        posSigma(0,2) =value.sigmaP[4] + candidate.sigma[0*6+4];
        posSigma(2,0) =value.sigmaP[4] + candidate.sigma[4*6+0];
        posSigma(1,2) =value.sigmaP[5] + candidate.sigma[2*6+4];
        posSigma(2,1) =value.sigmaP[5] + candidate.sigma[4*6+2];

        velSigma.setZero(); 
        velSigma(0,0) =value.sigmaV[0] + candidate.sigma[1*6+1];
        velSigma(1,1) =value.sigmaV[1] + candidate.sigma[3*6+3];
        velSigma(0,1) =value.sigmaV[3] + candidate.sigma[1*6+3];
        velSigma(1,0) =value.sigmaV[3] + candidate.sigma[3*6+1];

        double mahalanobisDistanceP = errorP.transpose()*posSigma.inverse()*errorP;

        // gate 
        // chi2 distribution probablity  of 0.95
        // p(x<=X) = 0.90
        // using inverse chi2 cdf (quantile function), X = 6.2513886311 for degree of freedom 3
        // X = 7.81472 for p = 0.95
        if(mahalanobisDistanceP <= pThreshold){
            // Check validation gate around velocity estimate
            double mahalanobisDistanceV = errorV.transpose()*velSigma.inverse()*errorV;

            if(mahalanobisDistanceV <= vThreshold){
                  return i;
            }
        }
    }

    return -1;
}

void TargetTracker::InputMeasurement(measurement& value){

    value.locPos = proj.project(value.position);
    // Iterate through available measurements
    bool associated = false;
    int n =CheckValidationGate(value); 
    if(n >= 0){
       // Update estimate with sensor measurement
       // Skip ownship
       if(n > 0){
           measurement &prediction = tracks[n]; 
           UpdateEstimate(prediction,value);
       }
       associated = true;
    }

    if(!associated){

        totalTracks++;
        //std::cout<<"new association:"<<value.callsign<<" at "<<value.time;
        value.callsign = "kf"+std::to_string(totalTracks);
        

        // Initial covariance matrix with process and measurement noise
        for(int i=0;i<6;++i){
            for(int j=0;j<6;++j){
                value.sigma[i*6 + j] = 0;
            }
        }
        value.sigma[0*6+0] = value.sigmaP[0];
        value.sigma[1*6+1] = value.sigmaV[0];
        value.sigma[2*6+2] = value.sigmaP[1];
        value.sigma[3*6+3] = value.sigmaV[1];
        value.sigma[4*6+4] = value.sigmaP[2];
        value.sigma[5*6+5] = value.sigmaV[2];
        value.sigma[0*6+2] = value.sigmaP[3];
        value.sigma[2*6+0] = value.sigmaP[3];
        value.sigma[0*6+4] = value.sigmaP[4];
        value.sigma[4*6+0] = value.sigmaP[4];
        value.sigma[2*6+4] = value.sigmaP[5];
        value.sigma[4*6+2] = value.sigmaP[5];
        value.sigma[1*6+3] = value.sigmaV[3];
        value.sigma[3*6+1] = value.sigmaV[3];
        value.sigma[1*6+5] = value.sigmaV[4];
        value.sigma[5*6+1] = value.sigmaV[4];
        value.sigma[3*6+5] = value.sigmaV[5];
        value.sigma[5*6+3] = value.sigmaV[5];

        tracks.push_back(value);
    }

    if(value.time > prevLogTime && log){
         prevLogTime = value.time;
         for(auto trk: tracks){
             logFile<<trk.time<<","<<trk.callsign<<","<<trk.locPos.x<<","<<trk.locPos.y<<","<<trk.locPos.z<<","
                    <<trk.position.latitude()<<","<<trk.position.longitude()<<","<<trk.position.alt()<<","
                    <<trk.velocity.track("degree")<<","<<trk.velocity.gs()<<","<<trk.velocity.vs()<<","
                    <<trk.sigmaP[0]<<","<<trk.sigmaP[1]<<","<<trk.sigmaP[2]<<","
                    <<trk.sigmaP[3]<<","<<trk.sigmaP[4]<<","<<trk.sigmaP[5]<<","
                    <<trk.sigmaV[0]<<","<<trk.sigmaV[1]<<","<<trk.sigmaV[2]<<","
                    <<trk.sigmaV[3]<<","<<trk.sigmaV[4]<<","<<trk.sigmaV[5]<<std::endl;

         }
    }

}

void TargetTracker::UpdateEstimate(measurement& prev,measurement& update,double time){
   /* 
    * dt: prediction time step
    * States      (X)  = [x,vx,y,vy,z,vz]
    * Motion model (A) = [1 dt 0 0  0 0
    *                     0 1  0 0  0 0
    *                     0 0  1 dt 0 0
    *                     0 0  0 1  0 0
    *                     0 0  0 0  1 dt
    *                     0 0  0 0  0 1] 
    * Sensor Model (H) = I6x6 
    * Model uncertainty: P
    * Sensor noise: R 
    * 
    * Prediction step:
    * Xp(k+1) = A*X(k)
    * Sx(k+1) = A*S(k)*At + P
    * 
    * Correction step:
    * yp     = H*Xp(k+1)
    * Sy     = H*Sx*Ht + R
    * G      = Sx*Ht*inv(Sy)
    * X(k+1) = Xp(k+1) + G*(y - yp)
    * S(k+1) = (I-GH)*Sx
    */

   // Define motion model matrix
   double dt;
   if(time < 1e-3){
       dt = update.time - prev.time;
       prev.lastUpdate = update.time;
   }else{
       dt = time - prev.time;
       update.time = time;
   }
   
   Eigen::MatrixXf I6(6,6);
   Eigen::MatrixXf modelP(6,6);
   Eigen::MatrixXf covPred(6,6); 
   Eigen::MatrixXf covCorr(6,6); 
   Eigen::MatrixXf covInnov(6,6); 
   Eigen::MatrixXf measurementP(6,6); 
   Eigen::MatrixXf motionModel(6,6);
   Eigen::MatrixXf kalmanGain(6,6);
   Eigen::VectorXf xprev(6);
   Eigen::VectorXf xpred(6);
   Eigen::VectorXf xcorr(6);
   Eigen::VectorXf innovation(6);

   I6.setIdentity();
   motionModel.setZero();
   motionModel(0,0) = 1.0;
   motionModel(0,1) = dt;
   motionModel(1,1) = 1.0;
   motionModel(2,2) = 1.0;
   motionModel(2,3) = dt;
   motionModel(3,3) = 1.0;
   motionModel(4,4) = 1.0;
   motionModel(4,5) = dt;
   motionModel(5,5) = 1.0;

   xprev(0,0) = prev.locPos.x;
   xprev(1,0) = prev.velocity.x;
   xprev(2,0) = prev.locPos.y;   
   xprev(3,0) = prev.velocity.y;   
   xprev(4,0) = prev.locPos.z;
   xprev(5,0) = prev.velocity.z;   

   modelP.setZero();
   modelP(0,0) = modelUncertaintyP[0];
   modelP(1,1) = modelUncertaintyV[0];
   modelP(2,2) = modelUncertaintyP[1];
   modelP(3,3) = modelUncertaintyV[1];
   modelP(4,4) = modelUncertaintyP[2];
   modelP(5,5) = modelUncertaintyV[2];

   modelP(0,2) = modelUncertaintyP[3];
   modelP(2,0) = modelUncertaintyP[3];
   modelP(1,3) = modelUncertaintyV[3];
   modelP(3,1) = modelUncertaintyV[3];
   
   modelP(0,4) = modelUncertaintyP[4];
   modelP(4,0) = modelUncertaintyP[4];
   modelP(1,5) = modelUncertaintyV[4];
   modelP(5,1) = modelUncertaintyV[4];
   
   modelP(2,4) = modelUncertaintyP[5];
   modelP(4,2) = modelUncertaintyP[5];
   modelP(3,5) = modelUncertaintyV[5];
   modelP(5,3) = modelUncertaintyV[5];
   
   measurementP.setZero();
   measurementP(0,0) = update.sigmaP[0];
   measurementP(1,1) = update.sigmaV[0];
   measurementP(2,2) = update.sigmaP[1];
   measurementP(3,3) = update.sigmaV[1];
   measurementP(4,4) = update.sigmaP[2];
   measurementP(5,5) = update.sigmaV[2];
   measurementP(0,2) = update.sigmaP[3];
   measurementP(2,0) = update.sigmaP[3];
   measurementP(0,4) = update.sigmaP[4];
   measurementP(4,0) = update.sigmaP[4];
   measurementP(2,4) = update.sigmaP[5];
   measurementP(4,2) = update.sigmaP[5];
   measurementP(1,3) = update.sigmaV[3];
   measurementP(3,1) = update.sigmaV[3];
   measurementP(1,5) = update.sigmaV[4];
   measurementP(5,1) = update.sigmaV[4];
   measurementP(3,5) = update.sigmaV[5];
   measurementP(5,3) = update.sigmaV[5];

   
   xpred = motionModel*xprev;
   for(int i=0;i<6;++i){
       for(int j=0;j<6;++j){
          covPred(i,j) = prev.sigma[i*6+j];
       }
   }
   if(dt > 1e-3){
       covPred = motionModel*covPred*motionModel.transpose() + modelP;
   }

   innovation(0,0) = update.locPos.x - xpred(0);
   innovation(1,0) = update.velocity.x - xpred(1);
   innovation(2,0) = update.locPos.y - xpred(2);
   innovation(3,0) = update.velocity.y - xpred(3);
   innovation(4,0) = update.locPos.z - xpred(4);
   innovation(5,0) = update.velocity.z - xpred(5);

   covInnov = covPred + measurementP;
   if(innovation.norm() > 1e-5){
       kalmanGain = covPred*covInnov.inverse();
   }else{
       kalmanGain.setZero();
   }
   xcorr = xpred + kalmanGain*innovation;
   covCorr = (I6 - kalmanGain)*covPred;

   larcfm::Vect3 locPos(xcorr(0),
                        xcorr(2),
                        xcorr(4));

   larcfm::Vect3 locVel(xcorr(1),
                        xcorr(3),
                        xcorr(5));

   for(int i=0;i<6;++i){
       for(int j=0;j<6;++j){
           prev.sigma[i*6+j] = covCorr(i,j);
       }
   }

   prev.time = update.time;
   prev.locPos = locPos;
   prev.position = larcfm::Position(proj.inverse(locPos));
   prev.velocity = larcfm::Velocity(locVel);
   prev.sigmaP[0] = prev.sigma[0*6 + 0];
   prev.sigmaP[1] = prev.sigma[2*6 + 2];
   prev.sigmaP[2] = prev.sigma[4*6 + 4];
   prev.sigmaP[3] = prev.sigma[0*6 + 2];
   prev.sigmaP[4] = prev.sigma[0*6 + 4];
   prev.sigmaP[5] = prev.sigma[2*6 + 4];
   prev.sigmaV[0] = prev.sigma[1*6 + 1];
   prev.sigmaV[1] = prev.sigma[3*6 + 3];
   prev.sigmaV[2] = prev.sigma[5*6 + 5];
   prev.sigmaV[3] = prev.sigma[1*6 + 3];
   prev.sigmaV[4] = prev.sigma[1*6 + 5];
   prev.sigmaV[5] = prev.sigma[3*6 + 5];

}

void TargetTracker::UpdatePredictions(double time){
    std::vector<int> oldTracks;
    for(int i=0;i<tracks.size();++i){
        if(i==0){
            continue;
        }else{
            // Save indices of tracks that haven't received 
            // updates for past N seconds (N is defined by timeout)
            if( (time - tracks[i].lastUpdate) > timeout){
                oldTracks.push_back(i);
            }
        }
    }

    // Remove stale tracks
    while(oldTracks.size() > 0){
        int index = oldTracks.back();
        tracks.erase(tracks.begin() + index);
        oldTracks.pop_back();
        //std::cout<<"Removing stale track"<<std::endl;
    }
    for(int i=0;i<tracks.size();++i){
        UpdateEstimate(tracks[i],tracks[i],time);
    }
}

int TargetTracker::GetTotalTraffic(){
    // tracks.size()-1 because the first track is for the ownship
    return tracks.size()-1;
}

measurement TargetTracker::GetIntruderData(int i){
    // i is 0 index. 0th intruder is the 1st track.
    // i+1 because the 0th track is for the ownship
    return tracks[i+1];
}

measurement TargetTracker::GetData(int i){
    return tracks[i];
}

void* new_TargetTracker(char* callsign,char* configFile){
    return new TargetTracker(std::string(callsign),std::string(configFile));
}

void TargetTracker_ReadParamsFromFile(void* obj,char* configFile){
    ((TargetTracker*)obj)->ReadParamFromFile(std::string(configFile));
}

void TargetTracker_SetHomePosition(void* obj,double position[3]){
    larcfm::Position home = larcfm::Position::makeLatLonAlt(position[0],"deg",position[1],"deg",position[2],"m");
    ((TargetTracker*)obj)->SetHomePosition(home);
}

void TargetTracker_SetGateThresholds(void* obj,double p,double v){
    ((TargetTracker*)obj)->SetGateThresholds(p,v);
}

void TargetTracker_SetModelUncertainty(void* obj,double sigmaP[6],double sigmaV[6]){
    ((TargetTracker*)obj)->SetModelUncertainty(sigmaP,sigmaV);
}

void TargetTracker_InputMeasurement(void* obj,char* callsign,double time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]){
    measurement input;
    input.callsign = std::string(callsign);
    input.time = time;
    input.position = larcfm::Position::makeLatLonAlt(position[0],"deg",position[1],"deg",position[2],"m");
    input.velocity = larcfm::Velocity::makeVxyz(velocity[0],velocity[1],"m/s",velocity[2],"m/s");
    std::memcpy(input.sigmaP,sigmaPos,sizeof(double)*6);
    std::memcpy(input.sigmaV,sigmaVel,sizeof(double)*6);
    ((TargetTracker*)obj)->InputMeasurement(input);
}

int TargetTracker_GetTotalIntruders(void* obj){
    return ((TargetTracker*)obj)->GetTotalTraffic();
}

void TargetTracker_GetIntruderData(void* obj,int i,char* callsign,double* time,double position[3],double velocity[3],double sigmaPos[6],double sigmaVel[6]){
    measurement output = ((TargetTracker*)obj)->GetIntruderData(i);
    std::strcpy(callsign,output.callsign.c_str());
    position[0] = output.position.latitude();
    position[1] = output.position.longitude();
    position[2] = output.position.alt();
    velocity[0] = output.velocity.x;
    velocity[1] = output.velocity.y;
    velocity[2] = output.velocity.z;
    *time       = output.time;
    std::memcpy(sigmaPos,output.sigmaP,sizeof(double)*6);
    std::memcpy(sigmaVel,output.sigmaV,sizeof(double)*6);
}

void TargetTracker_InputCurrentState(void* obj,double time,double position[3],double velocity[3],double sigmaP[6],double sigmaV[6]){
    larcfm::Position currentPos = larcfm::Position::makeLatLonAlt(position[0],"deg",position[1],"deg",position[2],"m");
    larcfm::Velocity currentVel = larcfm::Velocity::makeTrkGsVs(velocity[0],"deg",velocity[1],"m/s",velocity[2],"m/s");
    ((TargetTracker*)obj)->InputCurrentState(time,currentPos,currentVel,sigmaP,sigmaV);
}

void TargetTracker_UpdatePredictions(void* obj,double time){
    ((TargetTracker*)obj)->UpdatePredictions(time);
}
