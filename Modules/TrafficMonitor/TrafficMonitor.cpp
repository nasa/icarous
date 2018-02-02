//
// Created by research133 on 12/15/17.
//

#include "TrafficMonitor.h"
#include <sys/time.h>
#include <cstring>
#include <Icarous_msg.h>

TrafficMonitor::TrafficMonitor(FlightData *fd) {
    fdata = fd;
    time(&conflictStartTime);
    time(&startTime);
    conflict = false;

    char            fmt1[64],fmt2[64];
    struct timeval  tv;
    struct tm       *tm;
    gettimeofday(&tv, NULL);
    tm = localtime(&tv.tv_sec);
    strftime(fmt1, sizeof fmt1, "Icarous-%Y-%m-%d-%H:%M:%S", tm);
    strftime(fmt2, sizeof fmt2, "Icarous-%Y-%m-%d-%H:%M:%S", tm);
    strcat(fmt1,".login");
    strcat(fmt2,".logout");

    log = fdata->paramData.getBool("LOG_DAA");

    if(log) {
        logfileIn.open(fmt1);
        logfileOut.open(fmt2);
    }
    DAA.parameters.loadFromFile(fdata->paramData.getString("DAA_CONFIG"));

    visBands.numBands = 0;
}

bool TrafficMonitor::MonitorTraffic(bool visualize,double gpsTime,double position[],double velocity[],double resolution[],visbands_t* trkbands) {

    int numTraffic = fdata->GetTotalTraffic();
    if(numTraffic == 0){
        conflict = false;
        return conflict;
    }

    double holdConflictTime = fdata->paramData.getValue("CONFLICT_HOLD");
    time_t currentTime = time(&currentTime);
    double daaTimeElapsed = difftime(currentTime,conflictStartTime);
    double elapsedTime = difftime(currentTime, startTime);

    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity vo = Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");

    DAA.setOwnshipState("Ownship", so, vo, elapsedTime);
    double dist2traffic = MAXDOUBLE;
    for (unsigned int i = 0; i < numTraffic; i++) {
        Position si;
        Velocity vi;
        fdata->GetTraffic(i, si, vi);
        char name[10];
        sprintf(name, "Traffic%d", i);
        DAA.addTrafficState(name, si, vi);
        double dist = so.distanceH(si);
        if (dist < dist2traffic) {
            dist2traffic = dist;
        }
    }

    DAA.kinematicMultiBands(KMB);
    bool daaViolation = BandsRegion::isConflictBand(KMB.regionOfTrack(DAA.getOwnshipState().track()));

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
        resolution[0] = prefHeading * 180 / M_PI;
    }else{
        resolution[0] = 1e5;
    }
    
    if (daaViolation) {
        conflict = true;
        time(&conflictStartTime);
    }


    if (daaTimeElapsed > holdConflictTime) {
        if (!daaViolation) {
            conflict = false;
        }
    }


    // Construct kinematic bands message to send to ground station
    if (dist2traffic < 20 && visualize) {
        trkbands->numBands = KMB.trackLength();
        for (int i = 0; i < KMB.trackLength(); ++i) {
            Interval iv = KMB.track(i, "deg");
            int type = 0;
            if (KMB.trackRegion(i) == BandsRegion::NONE) {
                type = 0;
            } else if (KMB.trackRegion(i) == BandsRegion::NEAR) {
                type = 1;
            }

            if (i == 0) {
                trkbands->type1 = type;
                trkbands->min1 = (float) iv.low;
                trkbands->max1 = (float) iv.up;
            } else if (i == 1) {
                trkbands->type2 = type;
                trkbands->min2 = (float) iv.low;
                trkbands->max2 = (float) iv.up;
            } else if (i == 2) {
                trkbands->type3 = type;
                trkbands->min3 = (float) iv.low;
                trkbands->max3 = (float) iv.up;
            } else if (i == 3) {
                trkbands->type4 = type;
                trkbands->min4 = (float) iv.low;
                trkbands->max4 = (float) iv.up;
            } else {
                trkbands->type5 = type;
                trkbands->min5 = (float) iv.low;
                trkbands->max5 = (float) iv.up;
            }
        }
    }else{
        if(trkbands != NULL)
            trkbands->numBands = 0;
    }

    if(log && visualize){
        logfileIn << "**************** Current Time:"+std::to_string(gpsTime)+" *******************\n";
        logfileIn << DAA.toString()+"\n";
    }

    return conflict;
}

bool TrafficMonitor::CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading){
    Position so = Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    Velocity vo = Velocity::makeTrkGsVs(velocity[0],"degree",velocity[1],"m/s",velocity[2],"m/s");

    bool conflict = false;

    int numTraffic = fdata->GetTotalTraffic();

    if (numTraffic == 0)
        return true;

    DAA.setOwnshipState("Ownship", so, vo, 0);
    double dist2traffic = MAXDOUBLE;
    for (unsigned int i = 0; i < numTraffic; i++) {
        Position si;
        Velocity vi;
        fdata->GetTraffic(i, si, vi);
        char name[10];
        sprintf(name, "Traffic%d", i);
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

void TrafficMonitor::GetVisualizationBands(visbands_t &bands) {

    if(visBands.numBands > 0)
        std::cout<<"Num bands:"<<visBands.numBands<<std::endl;
    // NOTE: Must not use a memcpy here.
    bands.numBands = visBands.numBands;
    bands.max1 = visBands.max1;
    bands.max2 = visBands.max2;
    bands.max3 = visBands.max3;
    bands.max4 = visBands.max4;
    bands.max5 = visBands.max5;

    bands.min1 = visBands.min1;
    bands.min2 = visBands.min2;
    bands.min3 = visBands.min3;
    bands.min4 = visBands.min4;
    bands.min5 = visBands.min5;

}
