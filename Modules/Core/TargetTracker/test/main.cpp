#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <memory>
#include "TargetTracker.hpp"


#define POSN 2
#define POSE 3
#define POSZ 4
#define VELT 8
#define VELG 9
#define VELZ 10
#define SIGP0 11
#define SIGP1 12
#define SIGP2 13 
#define SIGP3 14
#define SIGP4 15
#define SIGP5 16
#define SIGV0 17
#define SIGV1 18
#define SIGV2 19
#define SIGV3 20
#define SIGV4 21
#define SIGV5 22
#define TOTALENTIRES 23

measurement GetState(larcfm::ENUProjection* proj,std::vector<std::string> data){

    measurement output;
    output.time = std::stof(data[0]);
    output.callsign = data[1];
    output.locPos = larcfm::Vect3::makeXYZ(std::stof(data[POSE]),"m",std::stof(data[POSN]),"m",std::stof(data[POSZ]),"m");
    output.position = larcfm::Position(proj->inverse(output.locPos));
    output.velocity = larcfm::Velocity::makeTrkGsVs(std::stof(data[VELT]),"degree",std::stof(data[VELG]),"m/s",std::stof(data[VELZ]),"m/s");
    output.sigmaP[0] = std::stof(data[SIGP0]);
    output.sigmaP[1] = std::stof(data[SIGP1]);
    output.sigmaP[2] = std::stof(data[SIGP2]);
    output.sigmaP[3] = std::stof(data[SIGP3]);
    output.sigmaP[4] = std::stof(data[SIGP4]);
    output.sigmaP[5] = std::stof(data[SIGP5]);

    output.sigmaV[0] = std::stof(data[SIGV0]);
    output.sigmaV[1] = std::stof(data[SIGV1]);
    output.sigmaV[2] = std::stof(data[SIGV2]);
    output.sigmaV[3] = std::stof(data[SIGV3]);
    output.sigmaV[4] = std::stof(data[SIGV4]);
    output.sigmaV[5] = std::stof(data[SIGV5]);

    return output; 
}

double AddMeasurement(larcfm::ENUProjection* proj,TargetTracker* obj,std::vector<std::string>& data,bool ownship){
    measurement meas = GetState(proj,data);
    if (ownship) {
        obj->InputCurrentState(meas.time, meas.position, meas.velocity, meas.sigmaP, meas.sigmaV);
    } else {
        obj->InputMeasurement(meas);
    }
    return meas.time;
}

void ReadMeasurements(TargetTracker *tracker, larcfm::ENUProjection *proj, int &openFiles, std::vector<std::shared_ptr<std::ifstream>> files, double &time) {
    for (int fp = 0; fp < files.size(); ++fp) {
        auto &inputFile = files[fp];
        std::string line = "";
        getline(*inputFile, line);
        if (line != "") {
            std::stringstream filestream(line);
            std::vector<std::string> dict(TOTALENTIRES);
            for (int i = 0; i < TOTALENTIRES; ++i) {
                std::string value;
                getline(filestream, value, ',');
                dict[i] = value;
            }
            bool ownship = (fp == 0) ? true : false;
            time = AddMeasurement(proj, tracker, dict, ownship);
        } else {
            inputFile->close();
            openFiles--;
        }
        line = "";
    }
}

int main(int argc,char** argv){
    std::vector<std::shared_ptr<std::ifstream>> inputFiles;
    
    for(int i=2;i<argc;++i){
         std::shared_ptr<std::ifstream> inputFile = std::make_shared<std::ifstream>(std::ifstream((const std::string) std::string(argv[i])));
         inputFiles.push_back(inputFile);
    }

    larcfm::Position homePos = larcfm::Position::makeLatLonAlt(0,0,0);
    larcfm::EuclideanProjection proj = larcfm::Projection::createProjection(homePos);

    TargetTracker tracker("ownship",argv[1]);
    tracker.SetHomePosition(homePos);
    int openFiles = inputFiles.size();
    double time;
    while(openFiles > 0){
        ReadMeasurements(&tracker,&proj,openFiles,inputFiles,time); 
        tracker.UpdatePredictions(time);
    }
    
    return 0;
}