//
// Created by Swee on 12/15/17.
//

#include "DaidalusMonitor.hpp"
#include "StateReader.h"
#include "ParameterData.h"
#include <sys/time.h>
#include <list>
#include <cstring>
#include <Units.h>

DaidalusMonitor::DaidalusMonitor(std::string callsgn,std::string daaConfig) {
    conflictStartTime = 0;
    startTime = 0;
    conflictTrack = false;
    conflictSpeed = false;
    conflictVerticalSpeed = false;
    callsign = callsgn;
    prevLogTime = 0;
    
    numTrackBands = 0;
    numSpeedBands = 0;
    numVerticalSpeedBands = 0;
    numAltitudeBands = 0;

    trackIntTypes.push_back(larcfm::BandsRegion::NONE);
    speedIntTypes.push_back(larcfm::BandsRegion::NONE);
    vsIntTypes.push_back(larcfm::BandsRegion::NONE);
    altIntTypes.push_back(larcfm::BandsRegion::NONE);

    UpdateParameters(daaConfig); 

}

void DaidalusMonitor::UpdateParameters(std::string daaParameters) {

    larcfm::StateReader reader;
    larcfm::ParameterData parameters;
    reader.open(daaParameters);
    reader.updateParameterData(parameters);

    char fmt1[64];
    struct timespec  tv;
    clock_gettime(CLOCK_REALTIME,&tv);
    double localT = tv.tv_sec + static_cast<float>(tv.tv_nsec)/1E9;
    sprintf(fmt1,"log/Daidalus-%s-%f.log",callsign.c_str(),localT);

    if(parameters.getBool("record_daa_logs")) {
        if(!logfileIn.is_open()){
           logfileIn.open(fmt1);
        }
    }

    DAA1.setParameterData(parameters);
    DAA2.setParameterData(parameters);

    alertingTime = DAA1.getAlerterAt(1).getParameters().getValue("alert_1_alerting_time");
}

std::string DaidalusMonitor::GetAlerter(object& intruder){
   if(intruder.source == _TRAFFIC_FLARM_){
       return "flarm";
   }else{
       return "default";
   }
}

void DaidalusMonitor::MonitorTraffic(larcfm::Velocity windfrom) {
    DAA1.setWindVelocityFrom(windfrom);
    int numTraffic = trafficList.size();
    if(numTraffic == 0){
        conflictTrack = false;
        conflictSpeed = false;
        conflictVerticalSpeed = false;
        return;
    }

    // Set sensor uncertainty mitigation parameters
    auto SetSUM = [&] (int id,double sumPos[6],double sumVel[6]) {
        double pstdN = sumPos[0]; double pstdE = sumPos[1];
        double pstdZ = sumPos[2]; double pstdNE = sumPos[3];
        double vstdN = sumVel[0]; double vstdE = sumVel[1];
        double vstdZ = sumVel[2]; double vstdNE = sumVel[3];
        DAA1.setHorizontalPositionUncertainty(0,pstdE,pstdN,pstdNE);
        DAA1.setVerticalPositionUncertainty(0,pstdZ);
        DAA1.setHorizontalVelocityUncertainty(0,vstdE,vstdN,vstdNE);
        DAA1.setVerticalSpeedUncertainty(0,vstdZ);
    };


    DAA1.setOwnshipState("Ownship", position, velocity, elapsedTime);
    SetSUM(0,posSigma,velSigma); 
    int count = 0;
    bool conflict = false;
    std::list<object> staleData;
    for (auto elem:trafficList){
        count++;
        // Use traffic only if its data has been updated within the last 10s.
        if(elapsedTime - elem.second.time < 120 && elem.second.position.alt() > 1){
            DAA1.addTrafficState(elem.second.callsign, elem.second.position, elem.second.velocity, elem.second.time);
            SetSUM(count,elem.second.posSigma,elem.second.velSigma);
            std::string alerter = GetAlerter(elem.second);
            DAA1.setAlerter(count,alerter);
        }else{
            staleData.push_back(elem.second);
        }

        int alert = DAA1.alerting(count);
        if(alert > 0) {
            conflict = true;
            conflictStartTime = elapsedTime;
        }
        
        trafficAlerts[elem.first] = alert;
    }

    // Remove all the stale data
    for(auto elem:staleData){
        trafficList.erase(elem.callsign);
    }


    double daaTimeElapsed = elapsedTime - conflictStartTime;
    const double holdConflictTime = 0; 
    if(daaTimeElapsed > holdConflictTime && conflict != true){
       numTrackBands = 0;
       numSpeedBands = 0;
       numVerticalSpeedBands = 0;
       numAltitudeBands = 0;
    }



    if(true) {
        daaViolationTrack = larcfm::BandsRegion::isConflictBand(DAA1.regionOfHorizontalDirection(DAA1.getOwnshipState().horizontalDirection()));
        daaViolationSpeed = larcfm::BandsRegion::isConflictBand(DAA1.regionOfHorizontalSpeed(DAA1.getOwnshipState().horizontalSpeed()));
        daaViolationVS = larcfm::BandsRegion::isConflictBand(DAA1.regionOfVerticalSpeed(DAA1.getOwnshipState().verticalSpeed()));
        daaViolationAlt = larcfm::BandsRegion::isConflictBand(DAA1.regionOfAltitude(DAA1.getOwnshipState().altitude()));
        numTrackBands = DAA1.horizontalDirectionBandsLength();
        larcfm::Interval timeIntervalOfConflict = DAA1.timeIntervalOfConflict(larcfm::BandsRegion::NEAR);
        timeIntervalOfConflictLow =  timeIntervalOfConflict.low;
        timeIntervalOfConflictHigh =  timeIntervalOfConflict.up;
        trackIntTypes.clear();
        trackInterval.clear();
        for (int i = 0; i < numTrackBands; ++i) {
            larcfm::Interval iv = DAA1.horizontalDirectionIntervalAt(i, "deg");
            trackIntTypes.push_back((int) DAA1.horizontalDirectionRegionAt(i));
            std::vector<double> trkband(2,0);
            trkband[0] = iv.low;
            trkband[1] = iv.up;
            trackInterval.push_back(trkband);
        }

        numSpeedBands = DAA1.horizontalSpeedBandsLength();
        speedIntTypes.clear();
        speedInterval.clear();
        for (int i = 0; i < numSpeedBands; ++i) {
            larcfm::Interval iv = DAA1.horizontalSpeedIntervalAt(i, "m/s");
            speedIntTypes.push_back((int) DAA1.horizontalSpeedRegionAt(i));
            std::vector<double> speedband(2,0);
            speedband[0] = iv.low;
            speedband[1] = iv.up;
            speedInterval.push_back(speedband);
        }

        numVerticalSpeedBands = DAA1.verticalSpeedBandsLength();
        vsIntTypes.clear();
        vsInterval.clear();
        for (int i = 0; i < numVerticalSpeedBands; ++i) {
            larcfm::Interval iv = DAA1.verticalSpeedIntervalAt(i, "m/s");
            vsIntTypes.push_back((int) DAA1.verticalSpeedRegionAt(i));
            std::vector<double> vsband(2,0);
            vsband[0] = iv.low;
            vsband[1] = iv.up;
            vsInterval.push_back(vsband);
        }

        numAltitudeBands = DAA1.altitudeBandsLength();
        altIntTypes.clear();
        altInterval.clear();
        for(int i =0;i< numAltitudeBands; ++i){
            larcfm::Interval iv = DAA1.altitudeIntervalAt(i,"m");
            altIntTypes.push_back((int) DAA1.altitudeRegionAt(i));
            std::vector<double> altband(2,0);
            altband[0] = iv.low;
            altband[1] = iv.up;
            altInterval.push_back(altband);
        }

    }


    if(logfileIn.is_open() && elapsedTime > prevLogTime+0.5){
        logfileIn << "**************** Current Time:"+std::to_string(elapsedTime)+" *******************\n";
        logfileIn << DAA1.toString()+"\n";
        prevLogTime = elapsedTime;
    }
}


bool DaidalusMonitor::CheckPositionFeasibility(const larcfm::Position wp,double const speed){
    int numTraffic = trafficList.size();
    if(numTraffic == 0){
        conflictTrack = false;
        conflictSpeed = false;
        conflictVerticalSpeed = false;
        return true;
    }

    double track = larcfm::Units::to(larcfm::Units::deg,position.track(wp));
    double dh = position.alt() - wp.alt();
    larcfm::Velocity vo;
    double verticalSpeed;
    double gs;
    if (speed < 0){
        gs = velocity.groundSpeed("m/s");
    }else{
        gs = speed;
    }

    if (fabs(dh) < 1){
        verticalSpeed = 0;
    }else if(dh > 0){
        verticalSpeed = minVS;
    }else{
        verticalSpeed = maxVS;
    }


    vo = larcfm::Velocity::makeTrkGsVs(track,"degree",gs,"m/s",verticalSpeed,"m/s");
    DAA2.setOwnshipState("Ownship", position, vo, elapsedTime);
    double dist2traffic = MAXDOUBLE;
    int count = 0;
    bool conflict = false;
    for (auto elem:trafficList){
        count++;
        DAA2.addTrafficState(elem.second.callsign, elem.second.position, elem.second.velocity, elem.second.time);
        std::string alerter = GetAlerter(elem.second);
        DAA2.setAlerter(count,alerter);
        if(DAA2.alerting(count) > 0) {
            conflict = true;
        }
    }

    double timeToTrafficViolation = DAA2.timeIntervalOfConflict(larcfm::BandsRegion::NEAR).low;
    if(!std::isinf(timeToTrafficViolation) && !std::isnan(timeToTrafficViolation)){
         double projAlt = position.alt() + verticalSpeed * timeToTrafficViolation;
         double targetAlt = wp.alt();
         
         if (fabs(projAlt - targetAlt) > ZTHR){
             conflict = false;
         }else{
             conflict = true;
         }
    }

    return !conflict;
}

bool DaidalusMonitor::CheckSafeToTurn(double position[],double velocity[],double fromHeading,double toHeading){
    larcfm::Position so = larcfm::Position::makeLatLonAlt(position[0],"degree",position[1],"degree",position[2],"m");
    larcfm::Velocity vo = larcfm::Velocity::makeTrkGsVs(toHeading,"degree",velocity[1],"m/s",velocity[2],"m/s");

    bool conflict = false;

    int numTraffic = trafficList.size();

    if (numTraffic == 0)
        return true;

    int count = 0;
    DAA2.setOwnshipState("Ownship", so, vo, 0);
    for (auto elem:trafficList){
        DAA2.addTrafficState(elem.second.callsign, elem.second.position, elem.second.velocity);
    }

    for (int i = 0; i < DAA2.horizontalDirectionBandsLength(); ++i){

        larcfm::Interval iv = DAA2.track(i, "deg");
        double low = iv.low;
        double high = iv.up;

        conflict |= CheckTurnConflict(low,high,toHeading,fromHeading);
    }

    return !conflict;
}

bool DaidalusMonitor::CheckTurnConflict(double low, double high, double newHeading, double oldHeading) {
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

bands_t DaidalusMonitor::GetTrackBands(void) {
    bands_t trkband;
    trkband.recovery = 0;
    trkband.numBands = numTrackBands;
    trkband.currentConflictBand = 0;
    trkband.timeToRecovery = -1;
    trkband.recovery = -1;
    if(numTrackBands > 0)
        trkband.currentConflictBand = (int)daaViolationTrack;
    for(int i=0;i<numTrackBands;++i){
       trkband.type[i] = trackIntTypes[i];
       trkband.min[i] = trackInterval[i][0];
       trkband.max[i] = trackInterval[i][1];
       if(trkband.type[i] == larcfm::BandsRegion::RECOVERY) {
           larcfm::RecoveryInformation rec = DAA1.horizontalDirectionRecoveryInformation();
           trkband.recovery = rec.nFactor();
           trkband.timeToRecovery = rec.timeToRecovery();
           trkband.minHDist = rec.recoveryHorizontalDistance("m");
           trkband.minVDist = rec.recoveryVerticalDistance("m");
       }
       
    }
    trkband.timeToViolation[0] = timeIntervalOfConflictLow;
    trkband.timeToViolation[1] = timeIntervalOfConflictHigh;
    if(numTrackBands > 0) {
        trkband.resUp = DAA1.horizontalDirectionResolution(true) * 180/M_PI;
        trkband.resDown = DAA1.horizontalDirectionResolution(false) * 180/M_PI;
        bool prefDirection = DAA1.preferredHorizontalDirectionRightOrLeft();
        double prefHeading = DAA1.horizontalDirectionResolution(prefDirection);
        trkband.resPreferred = prefHeading;
        if(!ISNAN(prefHeading)) {
            if (prefDirection) {
                prefHeading = prefHeading + 5 * M_PI / 180;
            } else {
                prefHeading = prefHeading - 5 * M_PI / 180;
            }
            trkband.resPreferred = prefHeading * 180 / M_PI;
        }
    }else{
        trkband.resPreferred = -1;
        trkband.resUp = -1;
        trkband.resDown = -1;
    }

    return trkband;
}

bands_t DaidalusMonitor::GetSpeedBands(void) {
    bands_t band;
    band.recovery = 0;
    band.numBands = numSpeedBands;
    band.currentConflictBand = 0;
    band.recovery = -1;
    band.timeToRecovery = -1;
    if(numSpeedBands > 0)
        band.currentConflictBand = (int)daaViolationSpeed;
    for(int i=0;i<numSpeedBands;++i){
       band.type[i] = speedIntTypes[i];
       band.min[i] = speedInterval[i][0];
       band.max[i] = speedInterval[i][1];
       if(band.type[i] == larcfm::BandsRegion::RECOVERY) {
           larcfm::RecoveryInformation rec = DAA1.horizontalSpeedRecoveryInformation();
           band.recovery = rec.nFactor();
           band.timeToRecovery = rec.timeToRecovery();
           band.minHDist = rec.recoveryHorizontalDistance("m");
           band.minVDist = rec.recoveryVerticalDistance("m");
       }

    }
    band.timeToViolation[0] = timeIntervalOfConflictLow;
    band.timeToViolation[1] = timeIntervalOfConflictHigh;

    double speedMax = DAA1.getMaxHorizontalSpeed("m/s");
    double speedMin = DAA1.getMinHorizontalSpeed("m/s");

    band.resUp = DAA1.horizontalSpeedResolution(true);
    band.resDown = DAA1.horizontalSpeedResolution(false);

    if(band.resUp > speedMax)
        band.resUp =  DAA1.horizontalSpeedResolution(true);

    if(band.resDown < speedMin)
        band.resDown = DAA1.horizontalSpeedResolution(false);

    if(DAA1.preferredHorizontalSpeedUpOrDown())
        band.resPreferred = band.resUp;
    else
        band.resPreferred = band.resDown;

    return band;
}

bands_t DaidalusMonitor::GetVerticalSpeedBands(void){
    bands_t band;
    band.recovery = 0;
    band.numBands = numVerticalSpeedBands;
    band.currentConflictBand = 0;
    band.recovery = -1;
    band.timeToRecovery = -1;
    if(numVerticalSpeedBands > 0)
        band.currentConflictBand = (int)daaViolationVS;
    for(int i=0;i<numVerticalSpeedBands;++i){
       band.type[i] = vsIntTypes[i];
       band.min[i] = vsInterval[i][0];
       band.max[i] = vsInterval[i][1];
       if(band.type[i] == larcfm::BandsRegion::RECOVERY) {
	       larcfm::RecoveryInformation rec = DAA1.verticalSpeedRecoveryInformation();
           band.recovery = rec.nFactor();
           band.timeToRecovery = rec.timeToRecovery();
           band.minHDist = rec.recoveryHorizontalDistance("m");
           band.minVDist = rec.recoveryVerticalDistance("m");
       }

    }
    band.timeToViolation[0] = timeIntervalOfConflictLow;
    band.timeToViolation[1] = timeIntervalOfConflictHigh;

    band.resUp = DAA1.verticalSpeedResolution(true);
    band.resDown = DAA1.verticalSpeedResolution(false);
    band.resPreferred = DAA1.verticalSpeedResolution(DAA1.preferredVerticalSpeedUpOrDown());

    // Revise down vertical speed resolution if projected altitude is not safe
    if(!DAA1.preferredVerticalSpeedUpOrDown() && !(std::isnan(band.resDown) || std::isinf(band.resDown))){
        double projAlt = position.alt() + band.resDown*alertingTime;
        if(DAA1.regionOfAltitude(projAlt) != larcfm::BandsRegion::NONE){
            band.resPreferred = band.resUp;
        }
    }

    
    return band;
}


bands_t DaidalusMonitor::GetAltBands(void){
    bands_t band;
    band.recovery = 0;
    band.numBands = numAltitudeBands;
    band.currentConflictBand = 0;
    band.recovery = -1;
    band.timeToRecovery = -1;
    if(numAltitudeBands > 0)
        band.currentConflictBand = (int)daaViolationAlt;
    for(int i=0;i<numAltitudeBands;++i){
        band.type[i] = altIntTypes[i];
        band.min[i] = altInterval[i][0];
        band.max[i] = altInterval[i][1];
        if(band.type[i] == larcfm::BandsRegion::RECOVERY) {
            larcfm::RecoveryInformation rec = DAA1.altitudeRecoveryInformation();
            band.recovery = rec.nFactor();
            band.timeToRecovery = rec.timeToRecovery();
            band.minHDist = rec.recoveryHorizontalDistance("m");
            band.minVDist = rec.recoveryVerticalDistance("m");
        }
    }

    double altMax = DAA1.getMaxAltitude("m");
    double altMin = DAA1.getMinAltitude("m");

    band.timeToViolation[0] = timeIntervalOfConflictLow;
    band.timeToViolation[1] = timeIntervalOfConflictHigh;

    band.resUp = DAA1.altitudeResolution(true);
    band.resDown = DAA1.altitudeResolution(false);

    if(band.resUp > altMax)
        band.resUp =  DAA1.altitudeResolution(true);

    if(band.resDown < altMin)
        band.resDown = DAA1.altitudeResolution(false);

    if(DAA1.preferredAltitudeUpOrDown())
        band.resPreferred = band.resUp;
    else
        band.resPreferred = band.resDown;

    return band;
}

int DaidalusMonitor::GetTrafficAlerts(int index,std::string& trafficID,int& alertLevel){
    int sizetf = trafficAlerts.size();
    if (sizetf > index){
        auto it = trafficAlerts.begin();
        for(int i=0;i<index;++i) ++it;
        trafficID = it->first;
        alertLevel = it->second;
    }
    return sizetf;
}
