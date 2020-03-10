/*
 * CDSS.cpp
 * Release: ACCoRD++-2.b (08/22/10)
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 * 
 * Conflict detection between an ownship and traffic aircraft using state information.
 *   
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <cmath>
#include <iostream>
#include "Units.h"
#include "format.h"
#include "Util.h"
#include "CDCylinder.h"
#include "CDSS.h"
#include "ConflictData.h"


namespace larcfm {


CDSS::CDSS() {
  Detection3D* cd = new CDCylinder();
  cdsscore = CDSSCore(cd, 0.0);;

}

CDSS::CDSS(CDSSCore& core) {
  cdsscore = core;
}

CDSS::CDSS(CDCylinder& cdCyl) {
    cdsscore.setCoreDetectionRef(cdCyl);
}

CDSS::CDSS(CDCylinder& cdCyl, double filterTm, const std::string& ut) {
    cdsscore.setCoreDetectionRef(cdCyl);
    cdsscore.setFilterTime(Units::from(ut,filterTm));
}

CDSS::CDSS(double D, const std::string& ud, double H, const std::string& uh) {
    Detection3D* cd = new CDCylinder(D,ud,H,uh);
    cdsscore = CDSSCore(cd, 0.0);
}

CDSS::CDSS(WCV_TAUMOD& wcv_TAUMOD) {
    cdsscore.setCoreDetectionRef(wcv_TAUMOD);
}

CDSS CDSS::makeCyl(const std::string& cyid, double D, const std::string& ud, double H, const std::string& uh) {
    Detection3D* cd = new CDCylinder(D,ud,H,uh);
    cd->setIdentifier(cyid);
    CDSSCore core = CDSSCore(cd, 0.0);
    return CDSS(core);
}


CDSS CDSS::mkCyl(const std::string& cyid, double D, double H, double filter) {
//    Detection3D* cd = new CDCylinder(D,H);
//    cd->setIdentifier(cyid);
//    CDSSCore core = CDSSCore(cd, 0.0);
    CDSSCore core = CDSSCore::mkCyl(cyid,D,H,filter);
    return CDSS(core);
}



CDSS CDSS::mkTauMOD(const std::string& id, double DTHR, double ZTHR, double TTHR, double TCOA) {
    CDSSCore cdss_cc = CDSSCore::mkTauMOD(id, DTHR, ZTHR, TTHR, TCOA);
    return CDSS(cdss_cc);

}

void CDSS::setFilterTime(double filter) {
    cdsscore.setFilterTime(filter);
}



CDSS::~CDSS() {
}

CDSS& CDSS::operator=(const CDSS& cdss) {
  //fpln(" $$$$$$$ CDSS ASSIGN D="+Fm1(cdss.D)+" H="+Fm1(cdss.H));
  cdsscore = cdss.cdsscore;
  return *this;
}


bool CDSS::lossOfSeparation(const Position& sop, const Velocity&vo, const Position& sip, const Velocity& vi) const {
  bool rtn = cdsscore.violation(sop, vo, sip, vi);
  return rtn;
}

/**
 * Detects a conflict that lasts more than filter time within a given lookahed time
 * and computes the time interval of conflict (internal units). If timeIn() == 0, after this function is called then aircraft
 * is in loss of separation.
 *
 * @param so the position of the ownship
 * @param vo the velocity of the ownship
 * @param si the position of the intruder
 * @param vi the velocity of the intruder
 * @param T  the lookahead time in internal units ut (T > 0)
 *
 * @return true, if there is a conflict that last more than the filter time
 * in the interval [0,T].
 */
bool CDSS::detection(const Position& sop, const Velocity&vo, const Position& sip, const Velocity& vi, double T, const std::string& ut) {
    return cdsscore.detection(sop,vo,sip,vi,Units::from(ut,T));
}


/**
 * Determines if two aircraft are in conflict within a given lookahed time (internal
 * units).
 * This function DOES NOT compute time interval of conflict.
 *
 * @param s the relative position of the aircraft
 * @param vo the velocity of the ownship
 * @param vi the velocity of the intruder
 * @param T  lookahead time in internal units [s] (T > 0)
 *
 * @return true, if the aircraft are in conflict in the interval [0,T].
 */
bool CDSS::conflict(const Position& so, const Velocity& vo, const Position& si, const Velocity& vi, double B, double T) const {
  return cdsscore.conflict(so,vo,si,vi,B,T);
}


/**
 * Time to loss of separation in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the time to loss of separation in explicit units [ut].
 * If time is negative then there is no conflict.
 */
double CDSS::getTimeIn(const std::string& ut) const {
  return Units::to(ut,cdsscore.getTimeIn());
}


/**
 * Time to recovery of loss of separation in explicit units.
 *
 * @param ut the explicit units of time
 *
 * @return the time to recovery of loss of separation in explicit units [ut].
 * If timeOut is zero then there is no conflict. If timeOut is negative then,
 * timeOut is infinite.
 */
double CDSS::getTimeOut(const std::string& ut) const {
  return Units::to(ut,cdsscore.getTimeOut());
}

/**
 * Return the critical time for the conflict, as determined by the core detection algorithm.  This is generally not the traditional time of closest approach.
 */
double CDSS::getCriticalTime(const std::string& ut) const {
    return Units::to(ut,cdsscore.getCriticalTime());
}



CDSSCore* CDSS::getCorePtr() {
      return &cdsscore;
  }


std::string CDSS::toString() {
  return "CDSS [cdsscore=" + cdsscore.toString() + "]";
}

}
