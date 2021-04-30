#include <gsl/gsl_matrix.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_linalg.h>
#include "TargetTracker.hpp"
#include "TargetTracker.h"

TargetTracker::TargetTracker(std::string name){
    callsign = name;
    timeout = 5;
    totalTracks = 0;
    modelUncertaintyP[0] = 1.0;
    modelUncertaintyP[1] = 1.0;
    modelUncertaintyP[2] = 1.0;
    modelUncertaintyP[3] = 0.0;
    modelUncertaintyP[4] = 0.0;
    modelUncertaintyP[5] = 0.0;
    modelUncertaintyV[0] = 1.0;
    modelUncertaintyV[1] = 1.0;
    modelUncertaintyV[2] = 1.0;
    modelUncertaintyV[3] = 0.0;
    modelUncertaintyV[4] = 0.0;
    modelUncertaintyV[5] = 0.0;
    pThreshold = 16.27;
    vThreshold = 16.27;
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
        gsl_matrix *posSigma = gsl_matrix_alloc(3,3);
        gsl_matrix *velSigma = gsl_matrix_alloc(2,2);
        gsl_matrix *errorP = gsl_matrix_alloc(3,1);
        gsl_matrix *errorV = gsl_matrix_alloc(2,1);

        gsl_matrix_set(errorP,0,0,value.locPos.x - candidate.locPos.x);
        gsl_matrix_set(errorP,1,0,value.locPos.y - candidate.locPos.y);
        gsl_matrix_set(errorP,2,0,value.locPos.z - candidate.locPos.z);

        gsl_matrix_set(errorV,0,0,value.velocity.x - candidate.velocity.x);
        gsl_matrix_set(errorV,1,0,value.velocity.y - candidate.velocity.y);
        //gsl_matrix_set(errorV,2,0,value.velocity.z - candidate.velocity.z);

        gsl_matrix_set_zero(posSigma);
        gsl_matrix_set_zero(velSigma);
        gsl_matrix_set(posSigma,0,0,value.sigmaP[0] + candidate.sigma[0*6+0]);
        gsl_matrix_set(posSigma,1,1,value.sigmaP[1] + candidate.sigma[2*6+2]);
        gsl_matrix_set(posSigma,2,2,value.sigmaP[2] + candidate.sigma[4*6+4]);
        gsl_matrix_set(posSigma,0,1,value.sigmaP[3] + candidate.sigma[0*6+2]);
        gsl_matrix_set(posSigma,1,0,value.sigmaP[3] + candidate.sigma[2*6+0]);
        gsl_matrix_set(posSigma,0,2,value.sigmaP[4] + candidate.sigma[0*6+4]);
        gsl_matrix_set(posSigma,2,0,value.sigmaP[4] + candidate.sigma[4*6+0]);
        gsl_matrix_set(posSigma,1,2,value.sigmaP[5] + candidate.sigma[2*6+4]);
        gsl_matrix_set(posSigma,2,1,value.sigmaP[5] + candidate.sigma[4*6+2]);

        gsl_matrix_set(velSigma,0,0,value.sigmaV[0] + candidate.sigma[1*6+1]);
        gsl_matrix_set(velSigma,1,1,value.sigmaV[1] + candidate.sigma[3*6+3]);
        //gsl_matrix_set(velSigma,2,2,value.sigmaV[2] + candidate.sigma[5*6+5]);
        gsl_matrix_set(velSigma,0,1,value.sigmaV[3] + candidate.sigma[1*6+3]);
        gsl_matrix_set(velSigma,1,0,value.sigmaV[3] + candidate.sigma[3*6+1]);
        //gsl_matrix_set(velSigma,0,2,value.sigmaV[4] + candidate.sigma[1*6+5]);
        //gsl_matrix_set(velSigma,2,0,value.sigmaV[4] + candidate.sigma[5*6+1]);
        //gsl_matrix_set(velSigma,1,2,value.sigmaV[5] + candidate.sigma[3*6+5]);
        //gsl_matrix_set(velSigma,2,1,value.sigmaV[5] + candidate.sigma[5*6+3]);


        gsl_linalg_cholesky_decomp1(posSigma);
        gsl_linalg_cholesky_decomp1(velSigma);
        gsl_linalg_cholesky_invert(posSigma);
        gsl_linalg_cholesky_invert(velSigma);

        gsl_matrix *tempP = gsl_matrix_alloc(1,3);
        gsl_matrix *tempV = gsl_matrix_alloc(1,2);

        gsl_matrix *valP = gsl_matrix_alloc(1,1);
        gsl_matrix *valV = gsl_matrix_alloc(1,1);

        // Check validation gate around position estimate
        gsl_blas_dgemm(CblasTrans,CblasNoTrans,1,(const gsl_matrix *)errorP,(const gsl_matrix *)posSigma,0,tempP);
        gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)tempP,(const gsl_matrix *)errorP,0,valP);

        // gate 
        // chi2 distribution probablity  of 0.95
        // p(x<=X) = 0.90
        // using inverse chi2 cdf (quantile function), X = 6.2513886311 for degree of freedom 3
        // X = 7.81472 for p = 0.95
        if(gsl_matrix_get(valP,0,0) <= pThreshold){
            // Check validation gate around velocity estimate
            gsl_blas_dgemm(CblasTrans,CblasNoTrans,1,(const gsl_matrix *)errorV,(const gsl_matrix *)velSigma,0,tempV);
            gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)tempV,(const gsl_matrix *)errorV,0,valV);

            if(gsl_matrix_get(valV,0,0) <= vThreshold){
                  gsl_matrix_free(posSigma);
                  gsl_matrix_free(velSigma);
                  gsl_matrix_free(errorP);
                  gsl_matrix_free(errorV);
                  gsl_matrix_free(tempP);
                  gsl_matrix_free(tempV);
                  gsl_matrix_free(valP);
                  gsl_matrix_free(valV);
                  return i;
            }
        }

        gsl_matrix_free(posSigma);
        gsl_matrix_free(velSigma);
        gsl_matrix_free(errorP);
        gsl_matrix_free(errorV);
        gsl_matrix_free(tempP);
        gsl_matrix_free(tempV);
        gsl_matrix_free(valP);
        gsl_matrix_free(valV);

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

        CheckValidationGate(value); 
        tracks.push_back(value);
        //std::cout<<",Total:"<<tracks.size()-1<<std::endl;
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
    * Sensor Mode (H) = I6x6 
    * Model uncertainty: P
    * Sensor noise: R 
    * 
    * Prediction step:
    * Xp(k+1) = A*X(k)
    * Sx(k+1) = A*S(k)*At + P
    * 
    * Correction step:
    * yp     = H*Xp(k)
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

   gsl_matrix *id = gsl_matrix_alloc(6,6);
   gsl_matrix_set_identity(id);

   gsl_matrix * motionModel = gsl_matrix_alloc(6,6);
   gsl_matrix_set_zero(motionModel);
   gsl_matrix_set(motionModel,0,0,1.0);
   gsl_matrix_set(motionModel,0,1,dt);
   gsl_matrix_set(motionModel,1,1,1.0);
   gsl_matrix_set(motionModel,2,2,1.0);
   gsl_matrix_set(motionModel,2,3,dt);
   gsl_matrix_set(motionModel,3,3,1.0);
   gsl_matrix_set(motionModel,4,4,1.0);
   gsl_matrix_set(motionModel,4,5,dt);
   gsl_matrix_set(motionModel,5,5,1.0);


   // Define state vector
   gsl_matrix * xprev = gsl_matrix_alloc(6,1);
   gsl_matrix_set(xprev,0,0,prev.locPos.x);
   gsl_matrix_set(xprev,1,0,prev.velocity.x);
   gsl_matrix_set(xprev,2,0,prev.locPos.y);
   gsl_matrix_set(xprev,3,0,prev.velocity.y);
   gsl_matrix_set(xprev,4,0,prev.locPos.z);
   gsl_matrix_set(xprev,5,0,prev.velocity.z);

   // Motion model uncertainty
   gsl_matrix * modelP = gsl_matrix_alloc(6,6);
   gsl_matrix_set_zero(modelP);
   gsl_matrix_set(modelP,0,0,modelUncertaintyP[0]);
   gsl_matrix_set(modelP,1,1,modelUncertaintyV[0]);
   gsl_matrix_set(modelP,2,2,modelUncertaintyP[1]);
   gsl_matrix_set(modelP,3,3,modelUncertaintyV[1]);
   gsl_matrix_set(modelP,4,4,modelUncertaintyP[2]);
   gsl_matrix_set(modelP,5,5,modelUncertaintyV[2]);

   gsl_matrix_set(modelP,0,2,modelUncertaintyP[3]);
   gsl_matrix_set(modelP,2,0,modelUncertaintyP[3]);
   gsl_matrix_set(modelP,1,3,modelUncertaintyV[3]);
   gsl_matrix_set(modelP,3,1,modelUncertaintyV[3]);

   gsl_matrix_set(modelP,0,4,modelUncertaintyP[4]);
   gsl_matrix_set(modelP,4,0,modelUncertaintyP[4]);
   gsl_matrix_set(modelP,1,5,modelUncertaintyV[4]);
   gsl_matrix_set(modelP,5,1,modelUncertaintyV[4]);

   gsl_matrix_set(modelP,2,4,modelUncertaintyP[5]);
   gsl_matrix_set(modelP,4,2,modelUncertaintyP[5]);
   gsl_matrix_set(modelP,3,5,modelUncertaintyV[5]);
   gsl_matrix_set(modelP,5,3,modelUncertaintyV[5]);

   // Prediction step
   // Xpred = A*xprev
   gsl_matrix *xpred = gsl_matrix_alloc(6,1);
   gsl_matrix_set_zero(xpred);
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)motionModel,(const gsl_matrix *)xprev,0,xpred);

   // Prediction covariance
   gsl_matrix *covPred = gsl_matrix_alloc(6,6);
   gsl_matrix* AcovPred = gsl_matrix_alloc(6,6);
   gsl_matrix_set_zero(covPred);
   for(int i=0;i<6;++i){
       for(int j=0;j<6;++j){
          gsl_matrix_set(covPred,i,j,prev.sigma[i*6+j]);
       }
   }
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)motionModel,(const gsl_matrix *)covPred,0,AcovPred);
   gsl_blas_dgemm(CblasNoTrans,CblasTrans,1,(const gsl_matrix *)AcovPred,(const gsl_matrix *)motionModel,0,covPred);
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)modelP,(const gsl_matrix *)id,1,covPred);

   // Innovation
   gsl_matrix *innovation = gsl_matrix_alloc(6,1);
   gsl_matrix_set(innovation,0,0,update.locPos.x - prev.locPos.x);
   gsl_matrix_set(innovation,1,0,update.velocity.x - prev.velocity.x);
   gsl_matrix_set(innovation,2,0,update.locPos.y - prev.locPos.y);
   gsl_matrix_set(innovation,3,0,update.velocity.y - prev.velocity.y);
   gsl_matrix_set(innovation,4,0,update.locPos.z - prev.locPos.z);
   gsl_matrix_set(innovation,5,0,update.velocity.z - prev.velocity.z);

   // Measurement noise
   gsl_matrix *innovCov = gsl_matrix_alloc(6,6);
   gsl_matrix_set_zero(innovCov);
   gsl_matrix_set(innovCov,0,0,update.sigmaP[0]);
   gsl_matrix_set(innovCov,1,1,update.sigmaV[0]);
   gsl_matrix_set(innovCov,2,2,update.sigmaP[1]);
   gsl_matrix_set(innovCov,3,3,update.sigmaV[1]);
   gsl_matrix_set(innovCov,4,4,update.sigmaP[2]);
   gsl_matrix_set(innovCov,5,5,update.sigmaV[2]);
   gsl_matrix_set(innovCov,0,2,update.sigmaP[3]);
   gsl_matrix_set(innovCov,2,0,update.sigmaP[3]);
   gsl_matrix_set(innovCov,0,4,update.sigmaP[4]);
   gsl_matrix_set(innovCov,4,0,update.sigmaP[4]);
   gsl_matrix_set(innovCov,2,4,update.sigmaP[5]);
   gsl_matrix_set(innovCov,4,2,update.sigmaP[5]);
   gsl_matrix_set(innovCov,1,3,update.sigmaV[3]);
   gsl_matrix_set(innovCov,3,1,update.sigmaV[3]);
   gsl_matrix_set(innovCov,1,5,update.sigmaV[4]);
   gsl_matrix_set(innovCov,5,1,update.sigmaV[4]);
   gsl_matrix_set(innovCov,3,5,update.sigmaV[5]);
   gsl_matrix_set(innovCov,5,3,update.sigmaV[5]);

   
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)covPred,(const gsl_matrix *)id,1,innovCov);

   // Kalman gain
   gsl_matrix *gain = gsl_matrix_alloc(6,6);
   gsl_matrix_set_zero(gain);
   if(time < 1e-3){
       gsl_linalg_cholesky_decomp1(innovCov);
       gsl_linalg_cholesky_invert(innovCov);
       gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)covPred,(const gsl_matrix *)innovCov,0,gain);
   }

   // Updated state
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,1,(const gsl_matrix *)gain,(const gsl_matrix *)innovation,1,xpred);

   // Updated covariance
   gsl_matrix* covUpdate = gsl_matrix_alloc(6,6);
   gsl_matrix_memcpy(covUpdate,covPred);
   gsl_blas_dgemm(CblasNoTrans,CblasNoTrans,-1,(const gsl_matrix *)gain,(const gsl_matrix *)covPred,1,covUpdate);

   larcfm::Vect3 locPos(gsl_matrix_get(xpred,0,0),
                        gsl_matrix_get(xpred,2,0),
                        gsl_matrix_get(xpred,4,0));

   larcfm::Vect3 locVel(gsl_matrix_get(xpred,1,0),
                        gsl_matrix_get(xpred,3,0),
                        gsl_matrix_get(xpred,5,0));


   prev.time = update.time;
   prev.locPos = locPos;
   prev.position = larcfm::Position(proj.inverse(locPos));
   prev.velocity = larcfm::Velocity(locVel);
   std::memcpy(prev.sigma,covUpdate->data,sizeof(double)*36);
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

   gsl_matrix_free(covUpdate);
   gsl_matrix_free(xpred);
   gsl_matrix_free(gain);
   gsl_matrix_free(innovCov);
   gsl_matrix_free(covPred);
   gsl_matrix_free(AcovPred);
   gsl_matrix_free(motionModel);
   gsl_matrix_free(xprev);
   gsl_matrix_free(innovation);
   gsl_matrix_free(id);
  
}

void TargetTracker::UpdatePredictions(double time){
    std::vector<int> oldTracks;
    for(int i=0;i<tracks.size();++i){
        if(i==0){
            continue;
        }else{
            // Save indices of tracks that haven't received 
            // updates for past 5 seconds
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
    return tracks.size()-1;
}

measurement TargetTracker::GetIntruderData(int i){
    return tracks[i+1];
}

void* new_TargetTracker(char* callsign){
    return new TargetTracker(std::string(callsign));
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
