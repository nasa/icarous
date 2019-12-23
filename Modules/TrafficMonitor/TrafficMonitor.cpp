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
    DAA.loadFromFile(filename);
    DAA2.loadFromFile(filename);

    numTrackBands = 0;
    numSpeedBands = 0;
    numVerticalSpeedBands = 0;
    numAltitudeBands = 0;

    trackIntTypes[0] = BandsRegion::NONE;
    speedIntTypes[0] = BandsRegion::NONE;
    vsIntTypes[0] = BandsRegion::NONE;
    altIntTypes[0] = BandsRegion::NONE;

}

void TrafficMonitor::UpdateDAAParameters(char daaParameters[],bool reclog) {

    larcfm::ParameterData params;
    params.parseParameterList(";",to_string(daaParameters));
    DAA.setParameterData(params);
    DAA2.setParameterData(params);
    if(reclog && !log) {
        log = reclog;
        char            fmt1[64],fmt2[64];
        struct timeval  tv;
        struct tm       *tm;
        gettimeofday(&tv, NULL);
        tm = localtime(&tv.tv_sec);
        strftime(fmt1, sizeof fmt1, "Icarous-%Y-%m-%d-%H:%M:%S", tm);
        strcat(fmt1,".login");
        logfileIn.open(fmt1);
    }
}

int TrafficMonitor::InputTraffic(int id, double *position, double *velocity,double elapsedTime) {

    time_t currentTime = time(&currentTime);
    //double elapsedTime = difftime(currentTime, startTime);

    TrafficObject _traffic(elapsedTime,_TRAFFIC_,id,(float)position[0],(float)position[1],(float)position[2],
                                        (float)velocity[0],(float)velocity[1],(float)velocity[2]);
    return TrafficObject::AddObject(trafficList,_traffic);
}

void TrafficMonitor::MonitorTraffic(double position[],double velocity[],double _elapsedTime) {

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
    elapsedTime = _elapsedTime;

    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity vo = Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");

    DAA.setOwnshipState("Ownship", so, vo, elapsedTime);
    double dist2traffic = MAXDOUBLE;
    int count = 0;
    bool conflict = false;
    list<TrafficObject> staleData;
    for (TrafficObject _traffic:trafficList){
        count++;
        Position si = _traffic.pos;
        Velocity vi = _traffic.vel;

        char name[50];
        sprintf(name, "Traffic%d", _traffic.id);

        // Use traffic only if its data has been updated within the last 10s.
        if(elapsedTime - _traffic.time < 10){
            DAA.addTrafficState(name, si, vi, _traffic.time);
        }else{
            staleData.push_back(_traffic);
        }

        double dist = so.distanceH(si);
        if (dist < dist2traffic) {
            dist2traffic = dist;
        }

        if(DAA.alerting(count)) {
            conflict = true;
            conflictStartTime = time(&currentTime);
        }
    }

    // Remove all the stale data
    for(TrafficObject _traffic:staleData){
        trafficList.remove(_traffic);
    }


    double daaTimeElapsed = difftime(currentTime,conflictStartTime);

    if(daaTimeElapsed > holdConflictTime && conflict != true){
       numTrackBands = 0;
       numSpeedBands = 0;
       numVerticalSpeedBands = 0;
       numAltitudeBands = 0;
    }



    if(conflict) {
        daaViolationTrack = BandsRegion::isConflictBand(DAA.regionOfHorizontalDirection(DAA.getOwnshipState().horizontalDirection()));
        daaViolationSpeed = BandsRegion::isConflictBand(DAA.regionOfHorizontalSpeed(DAA.getOwnshipState().horizontalSpeed()));
        daaViolationVS = BandsRegion::isConflictBand(DAA.regionOfVerticalSpeed(DAA.getOwnshipState().verticalSpeed()));
        daaViolationAlt = BandsRegion::isConflictBand(DAA.regionOfAltitude(DAA.getOwnshipState().altitude()));

        numTrackBands = DAA.horizontalDirectionBandsLength();
        for (int i = 0; i < numTrackBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = DAA.horizontalDirectionIntervalAt(i, "deg");
            trackIntTypes[i] = (int) DAA.horizontalDirectionRegionAt(i);
            trackIntervals[i][0] = iv.low;
            trackIntervals[i][1] = iv.up;
        }

        numSpeedBands = DAA.horizontalSpeedBandsLength();
        for (int i = 0; i < numSpeedBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = DAA.horizontalSpeedIntervalAt(i, "m/s");
            speedIntTypes[i] = (int) DAA.horizontalSpeedRegionAt(i);
            speedIntervals[i][0] = iv.low;
            speedIntervals[i][1] = iv.up;
        }

        numVerticalSpeedBands = DAA.verticalSpeedBandsLength();
        for (int i = 0; i < numVerticalSpeedBands; ++i) {
            if (i > 19)
                break; // Currently hardcoded to handle only 20 tracks
            Interval iv = DAA.verticalSpeedIntervalAt(i, "m/s");
            vsIntTypes[i] = (int) DAA.verticalSpeedRegionAt(i);
            vsIntervals[i][0] = iv.low;
            vsIntervals[i][1] = iv.up;
        }

        numAltitudeBands = DAA.altitudeBandsLength();
        for(int i =0;i< numAltitudeBands; ++i){
            if(i> 19)
                break; // Current hardcoded to handle only 20 tracks
            Interval iv = DAA.altitudeIntervalAt(i,"m");
            altIntTypes[i] = (int) DAA.altitudeRegionAt(i);
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
    double cmd = -(position[2] - wp[2])/5;
    Velocity vo = Velocity::makeTrkGsVs(track,"degree",velocity[1],"m/s",cmd,"m/s");

    DAA2.setOwnshipState("Ownship", so, vo, elapsedTime);
    double dist2traffic = MAXDOUBLE;
    int count = 0;
    bool conflict = false;
    for (TrafficObject _traffic:trafficList){
        count++;
        Position si = _traffic.pos;
        Velocity vi = _traffic.vel;

        char name[50];
        sprintf(name, "Traffic%d", _traffic.id);
        DAA2.addTrafficState(name, si, vi, _traffic.time);

        if(DAA2.alerting(count)) {
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

    for (int i = 0; i < DAA.horizontalDirectionBandsLength(); ++i){

        Interval iv = DAA.track(i, "deg");
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
           RecoveryInformation rec = DAA.horizontalDirectionRecoveryInformation();
           trecovery = rec.timeToRecovery();
           minhdist = rec.recoveryHorizontalDistance("m");
           minvdist = rec.recoveryVerticalDistance("m");
       }
    }

    if(numBands > 0) {
        resup = DAA.horizontalDirectionResolution(true) * 180/M_PI;
        resdown = DAA.horizontalDirectionResolution(false) * 180/M_PI;
        bool prefDirection = DAA.preferredHorizontalDirectionRightOrLeft();
        double prefHeading = DAA.horizontalDirectionResolution(prefDirection);
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
	   RecoveryInformation rec = DAA.horizontalSpeedRecoveryInformation();
           trecovery = rec.timeToRecovery();
           minhdist = rec.recoveryHorizontalDistance("m");
           minvdist = rec.recoveryVerticalDistance("m");
       }

    }

    double speedMax = DAA.getMaxHorizontalSpeed("m/s");
    double speedMin = DAA.getMinHorizontalSpeed("m/s");
    double diff = speedMax - speedMin;
    double percentChange = 0.05 * diff;

    resup = DAA.horizontalSpeedResolution(true) + percentChange;
    resdown = DAA.horizontalSpeedResolution(false) - percentChange;

    if(resup > speedMax)
        resup =  DAA.horizontalSpeedResolution(true);

    if(resdown < speedMin)
        resdown = DAA.horizontalSpeedResolution(false);

    if(DAA.preferredHorizontalSpeedUpOrDown())
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
	   RecoveryInformation rec = DAA.verticalSpeedRecoveryInformation();
           trecovery = rec.timeToRecovery();
           minhdist = rec.recoveryHorizontalDistance("m");
           minvdist = rec.recoveryVerticalDistance("m");
       }

    }

    //if(numBands > 1) {
        resup = DAA.verticalSpeedResolution(true);
        resdown = DAA.verticalSpeedResolution(false);
        respref = DAA.verticalSpeedResolution(DAA.preferredVerticalSpeedUpOrDown());
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
	   RecoveryInformation rec = DAA.altitudeRecoveryInformation();
           trecovery = rec.timeToRecovery();
           minhdist = rec.recoveryHorizontalDistance("m");
           minvdist = rec.recoveryVerticalDistance("m");
        }

    }

    double altMax = DAA.getMaxAltitude("m");
    double altMin = DAA.getMinAltitude("m");
    double diff = altMax - altMin;
    double percentChange = 0.05 * diff;

    resup = DAA.altitudeResolution(true) + percentChange;
    resdown = DAA.altitudeResolution(false) - percentChange;

    if(resup > altMax)
        resup =  DAA.altitudeResolution(true);

    if(resdown < altMin)
        resdown = DAA.altitudeResolution(false);

    if(DAA.preferredAltitudeUpOrDown())
        respref = resup;
    else
        respref = resdown;

    //printf("num bands :%d\n",numBands);
    //printf("current conflict: %d\n",currentConflict);
    //printf("pref resolution:%f\n",respref);
}
