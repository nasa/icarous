//
// Created by Swee on 12/15/17.
//

#include "TrafficMonitor.h"
#include <sys/time.h>
#include <cstring>

TrafficMonitor::TrafficMonitor(bool reclog,char daaConfig[]) {
    time(&conflictStartTime);
    time(&startTime);
    conflictTrack = false;
    conflictSpeed = false;
    conflictVerticalSpeed = false;
    prevLogTime = 0;
    char            fmt1[64],fmt2[64];
    struct timeval  tv;
    struct tm       *tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    strftime(fmt1, sizeof fmt1, "Icarous-%Y-%m-%d-%H:%M:%S", tm);
    strcat(fmt1,".login");

    log = reclog;

    if(log) {
        logfileIn.open(fmt1);
    }

    std::string filename(daaConfig);
    DAA.parameters.loadFromFile(filename);

    numTrackBands = 0;
    numSpeedBands = 0;
    numVerticalSpeedBands = 0;

    trackIntTypes[0] = BandsRegion::NONE;
    speedIntTypes[0] = BandsRegion::NONE;
    vsIntTypes[0] = BandsRegion::NONE;

}

void TrafficMonitor::UpdateDAAParameters(char daaParameters[]) {

    larcfm::ParameterData params;
    params.parseParameterList(";",to_string(daaParameters));
    DAA.parameters.setParameters(params);
}

int TrafficMonitor::InputTraffic(int id, double *position, double *velocity,double elapsedTime) {

    time_t currentTime = time(&currentTime);
    //double elapsedTime = difftime(currentTime, startTime);

    TrafficObject _traffic(elapsedTime,_TRAFFIC_,id,(float)position[0],(float)position[1],(float)position[2],
                                        (float)velocity[0],(float)velocity[1],(float)velocity[2]);
    return TrafficObject::AddObject(trafficList,_traffic);
}

void TrafficMonitor::MonitorTraffic(double position[],double velocity[],double elapsedTime) {

    int numTraffic = trafficList.size();
    if(numTraffic == 0){
        conflictTrack = false;
        conflictSpeed = false;
        conflictVerticalSpeed = false;
        return;
    }

    double holdConflictTime = 3;
    time_t currentTime = time(&currentTime);
    //double elapsedTime = difftime(currentTime, startTime);

    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity vo = Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");

    DAA.setOwnshipState("Ownship", so, vo, elapsedTime);
    double dist2traffic = MAXDOUBLE;
    int count = 0;
    bool conflict = false;
    for (TrafficObject _traffic:trafficList){
        count++;
        Position si = _traffic.pos;
        Velocity vi = _traffic.vel;

        char name[10];
        sprintf(name, "Traffic%d", count);
        DAA.addTrafficState(name, si, vi, _traffic.time);
        double dist = so.distanceH(si);
        if (dist < dist2traffic) {
            dist2traffic = dist;
        }

        if(DAA.alerting(count)) {
            conflict = true;
            conflictStartTime = time(&currentTime);
        }
    }


    double daaTimeElapsed = difftime(currentTime,conflictStartTime);

    if(daaTimeElapsed > holdConflictTime && conflict != true){
       numTrackBands = 0;
       numSpeedBands = 0;
       numVerticalSpeedBands = 0;
       numAltitudeBands = 0;
    }



    if(conflict) {
        DAA.kinematicMultiBands(KMB);
        daaViolationTrack = BandsRegion::isConflictBand(KMB.regionOfTrack(DAA.getOwnshipState().track()));
        daaViolationSpeed = BandsRegion::isConflictBand(KMB.regionOfGroundSpeed(DAA.getOwnshipState().groundSpeed()));
        daaViolationVS = BandsRegion::isConflictBand(KMB.regionOfVerticalSpeed(DAA.getOwnshipState().verticalSpeed()));
        daaViolationAlt = BandsRegion::isConflictBand(KMB.regionOfAltitude(DAA.getOwnshipState().altitude()));

        numTrackBands = KMB.trackLength();
        for (int i = 0; i < numTrackBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = KMB.track(i, "deg");
            trackIntTypes[i] = (int) KMB.trackRegion(i);
            trackIntervals[i][0] = iv.low;
            trackIntervals[i][1] = iv.up;
        }

        numSpeedBands = KMB.groundSpeedLength();
        for (int i = 0; i < numSpeedBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = KMB.groundSpeed(i, "m/s");
            speedIntTypes[i] = (int) KMB.groundSpeedRegion(i);
            speedIntervals[i][0] = iv.low;
            speedIntervals[i][1] = iv.up;
        }

        numVerticalSpeedBands = KMB.verticalSpeedLength();
        for (int i = 0; i < numVerticalSpeedBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = KMB.verticalSpeed(i, "m/s");
            vsIntTypes[i] = (int) KMB.verticalSpeedRegion(i);
            vsIntervals[i][0] = iv.low;
            vsIntervals[i][1] = iv.up;
        }

        numAltitudeBands = KMB.altitudeLength();
        for(int i =0;i< numAltitudeBands; ++i){
            if(i> 19)
                break; // Current hardcoded to handle only 20 tracks
            Interval iv = KMB.altitude(i,"m");
            altIntTypes[i] = (int) KMB.altitudeRegion(i);
            altIntervals[i][0] = iv.low;
            altIntervals[i][1] = iv.up;
        }
    }


    if(log && elapsedTime > prevLogTime){
        logfileIn << "**************** Current Time:"+std::to_string(elapsedTime)+" *******************\n";
        logfileIn << DAA.toString()+"\n";
        prevLogTime = elapsedTime;
    }
}

bool TrafficMonitor::MonitorWPFeasibility(double *position, double *velocity, double *wp) {

    int numTraffic = trafficList.size();
    if(numTraffic == 0){
        conflictTrack = false;
        conflictSpeed = false;
        conflictVerticalSpeed = false;
        return true;
    }

    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Position WP = Position::makeLatLonAlt(wp[0],"degree",wp[1],"degree",wp[2],"m");

    double track = so.track(WP) * 180/M_PI;
    Velocity vo = Velocity::makeTrkGsVs(track,"degree",velocity[1],"m/s",0,"m/s");

    DAA.setOwnshipState("Ownship", so, vo, 0);
    double dist2traffic = MAXDOUBLE;
    int count = 0;
    bool conflict = false;
    for (TrafficObject _traffic:trafficList){
        count++;
        Position si = _traffic.pos;
        Velocity vi = _traffic.vel;

        char name[10];
        sprintf(name, "Traffic%d", count);
        DAA.addTrafficState(name, si, vi);

        if(DAA.alerting(count)) {
            conflict = true;
        }
    }

    return !conflict;
}

bool TrafficMonitor::CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading){
    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity vo = Velocity::makeTrkGsVs(toHeading,"degree",velocity[1],"m/s",velocity[2],"m/s");

    bool conflict = false;

    int numTraffic = trafficList.size();

    if (numTraffic == 0)
        return true;

    int count = 0;
    DAA.setOwnshipState("Ownship", so, vo, 0);
    for (TrafficObject _traffic:trafficList){
        count++;
        Position si = _traffic.pos;
        Velocity vi = _traffic.vel;

        char name[10];
        sprintf(name, "Traffic%d", count);
        DAA.addTrafficState(name, si, vi);
    }

    DAA.kinematicMultiBands(KMB);

    for (int i = 0; i < KMB.trackLength(); ++i){

        Interval iv = KMB.track(i, "deg");
        double low = iv.low;
        double high = iv.up;

        conflict |= CheckTurnConflict(low,high,toHeading,fromHeading);
    }

    return !conflict;
}

bool TrafficMonitor::CheckTurnConflict(double low, double high, double newHeading, double oldHeading) {
    if(newHeading < 0){
        newHeading = 360 + newHeading;
    }

    if(oldHeading < 0){
        oldHeading = 360 + oldHeading;
    }

    // Get direction of turn
    double psi   = newHeading - oldHeading;
    double psi_c = 360 - std::abs(psi);
    bool rightTurn = false;
    if(psi > 0){
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = false;
        }
        else{
            rightTurn = true;
        }
    }else{
        if(std::abs(psi) > std::abs(psi_c)){
            rightTurn = true;
        }
        else{
            rightTurn = false;
        }
    }

    double A,B,X,Y,diff;
    if(rightTurn){
        diff = oldHeading;
        A = oldHeading - diff;
        B = newHeading - diff;
        X = low - diff;
        Y = high - diff;

        if(B < 0){
            B = 360 + B;
        }

        if(X < 0){
            X = 360 + X;
        }

        if(Y < 0){
            Y = 360 + Y;
        }

        if(A < X && B > Y){
            return true;
        }
    }else{
        diff = 360 - oldHeading;
        A    = oldHeading + diff;
        B    = newHeading + diff;
        X = low + diff;
        Y = high + diff;

        if(B > 360){
            B = B - 360;
        }

        if(X > 360){
            X = X - 360;
        }

        if(Y > 360){
            Y = Y - 360;
        }

        if(A > Y && B < X){
            return true;
        }
    }

    return false;
}

void TrafficMonitor::GetTrackBands(int& numBands,int* bandTypes,double* low,double *high,
                                   int& recovery,
                                   int& currentConflict,
                                   double& tviolation,
                                   double& trecovery,
                                   double& minhdist,
                                   double& minvdist,
                                   double& resup,
                                   double& resdown,
                                   double& respref) {

    recovery = 0;
    numBands = numTrackBands;
    currentConflict = 0;
    if(numBands > 0)
        currentConflict = (int)daaViolationTrack;
    for(int i=0;i<numBands;++i){
       bandTypes[i] = trackIntTypes[i];
       low[i] = trackIntervals[i][0];
       high[i] = trackIntervals[i][1];
       if(bandTypes[i] == BandsRegion::RECOVERY) {
           recovery = 1;
           trecovery = KMB.timeToTrackRecovery();
           minhdist = KMB.getMinHorizontalRecovery("m");
           minvdist = KMB.getMinVerticalRecovery("m");
       }
    }

    if(numBands > 0) {
        resup = KMB.trackResolution(true) * 180/M_PI;
        resdown = KMB.trackResolution(false) * 180/M_PI;
        bool prefDirection = KMB.preferredTrackDirection();
        double prefHeading = KMB.trackResolution(prefDirection);
        if(!ISNAN(prefHeading)) {
            if (prefDirection) {
                prefHeading = prefHeading + 5 * M_PI / 180;
                if (prefHeading > M_PI) {
                    prefHeading = prefHeading - 2 * M_PI;
                }
            } else {
                prefHeading = prefHeading - 5 * M_PI / 180;
                if (prefHeading < -M_PI) {
                    prefHeading = 2 * M_PI + prefHeading;
                }
            }
            respref = prefHeading * 180 / M_PI;
        }else{
            respref = -1;
        }
    }else{
        respref = -1;
        resup = -1;
        resdown = -1;
    }

}

void TrafficMonitor::GetGSBands(int& numBands,int* bandTypes,double* low,double *high,
                                   int& recovery,
                                   int& currentConflict,
                                   double& tviolation,
                                   double& trecovery,
                                   double& minhdist,
                                   double& minvdist,
                                   double& resup,
                                   double& resdown,
                                   double& respref) {

    recovery = 0;
    numBands = numSpeedBands;
    currentConflict = 0;
    if(numBands > 0)
        currentConflict = (int)daaViolationSpeed;
    for(int i=0;i<numBands;++i){
       bandTypes[i] = speedIntTypes[i];
       low[i] = speedIntervals[i][0];
       high[i] = speedIntervals[i][1];
       if(bandTypes[i] == BandsRegion::RECOVERY) {
           recovery = 1;
           trecovery = KMB.timeToGroundSpeedRecovery();
           minhdist = KMB.getMinHorizontalRecovery("m");
           minvdist = KMB.getMinVerticalRecovery("m");
       }

    }

    double speedMax = DAA.parameters.getMaxGroundSpeed("m/s");
    double speedMin = DAA.parameters.getMinGroundSpeed("m/s");
    double diff = speedMax - speedMin;
    double percentChange = 0.05 * diff;

    resup = KMB.groundSpeedResolution(true) + percentChange;
    resdown = KMB.groundSpeedResolution(false) - percentChange;

    if(resup > speedMax)
        resup =  KMB.groundSpeedResolution(true);

    if(resdown < speedMin)
        resdown = KMB.groundSpeedResolution(false);

    if(KMB.preferredGroundSpeedDirection())
        respref = resup;
    else
        respref = resdown;
}

void TrafficMonitor::GetVSBands(int& numBands,int* bandTypes,double* low,double *high,
                                   int& recovery,
                                   int& currentConflict,
                                   double& tviolation,
                                   double& trecovery,
                                   double& minhdist,
                                   double& minvdist,
                                   double& resup,
                                   double& resdown,
                                   double& respref){
    recovery = 0;
    numBands = numVerticalSpeedBands;
    currentConflict = 0;
    if(numBands > 0)
        currentConflict = (int)daaViolationVS;
    for(int i=0;i<numBands;++i){
       bandTypes[i] = vsIntTypes[i];
       low[i] = vsIntervals[i][0];
       high[i] = vsIntervals[i][1];
       if(bandTypes[i] == BandsRegion::RECOVERY) {
           recovery = 1;
           trecovery = KMB.timeToVerticalSpeedRecovery();
           minhdist = KMB.getMinHorizontalRecovery("m");
           minvdist = KMB.getMinVerticalRecovery("m");
       }

    }

    //if(numBands > 1) {
        resup = KMB.verticalSpeedResolution(true);
        resdown = KMB.verticalSpeedResolution(false);
        respref = KMB.verticalSpeedResolution(KMB.preferredVerticalSpeedDirection());
    //}
}


void TrafficMonitor::GetAltBands(int& numBands,int* bandTypes,double* low,double *high,
                                int& recovery,
                                int& currentConflict,
                                double& tviolation,
                                double& trecovery,
                                double& minhdist,
                                double& minvdist,
                                double& resup,
                                double& resdown,
                                double& respref){
    recovery = 0;
    numBands = numAltitudeBands;
    currentConflict = 0;
    if(numBands > 0)
        currentConflict = (int)daaViolationAlt;
    for(int i=0;i<numBands;++i){
        bandTypes[i] = altIntTypes[i];
        low[i] = altIntervals[i][0];
        high[i] = altIntervals[i][1];
        if(bandTypes[i] == BandsRegion::RECOVERY) {
            recovery = 1;
            trecovery = KMB.timeToAltitudeRecovery();
            minhdist = KMB.getMinHorizontalRecovery("m");
            minvdist = KMB.getMinVerticalRecovery("m");
        }

    }

    double altMax = DAA.parameters.getMaxAltitude("m");
    double altMin = DAA.parameters.getMinAltitude("m");
    double diff = altMax - altMin;
    double percentChange = 0.05 * diff;

    resup = KMB.altitudeResolution(true) + percentChange;
    resdown = KMB.altitudeResolution(false) - percentChange;

    if(resup > altMax)
        resup =  KMB.altitudeResolution(true);

    if(resdown < altMin)
        resdown = KMB.altitudeResolution(false);

    if(KMB.preferredAltitudeDirection())
        respref = resup;
    else
        respref = resdown;

    //printf("num bands :%d\n",numBands);
    //printf("current conflict: %d\n",currentConflict);
    //printf("pref resolution:%f\n",respref);
}
