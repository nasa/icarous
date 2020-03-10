/*
 * CDSSCore.cpp
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

#include "CDSSCore.h"
#include "Units.h"
#include "Util.h"
#include "format.h"
#include "Detection3D.h"
#include "WCV_TAUMOD.h"
#include "EuclideanProjection.h"
#include "Projection.h"
#include "string_util.h"


namespace larcfm {

CDSSCore::CDSSCore(Detection3D* c, double cdfilter) {
  filter = cdfilter;
  tca = PINFINITY;
  stca = Vect3();
  dtca = PINFINITY;
  t_in = PINFINITY;
  t_out = NINFINITY;
  cd = c != NULL ? c->copy() : NULL;
}


CDSSCore::CDSSCore(const CDSSCore& cdss) {
  filter = cdss.filter;
  tca = cdss.tca;
  stca = cdss.stca;
  dtca = PINFINITY;
  t_in = cdss.t_in;
  t_out = cdss.t_out;
  cd = cdss.cd != NULL ? cdss.cd->copy() : NULL;
}

CDSSCore::CDSSCore() {
  filter = 1.0;
  tca = 0.0;
  stca = 0.0;
  dtca = PINFINITY;
  t_in = 0.0;
  t_out = 0.0;
  cd = new CDCylinder();
}

CDSSCore::~CDSSCore() {
  delete cd;
}

CDSSCore CDSSCore::make(Detection3D& c, double cdfilter, const std::string& unit) {
  return CDSSCore(&c, Units::from(unit,cdfilter));
}

CDSSCore CDSSCore::mkCyl(const std::string& id, double D, double H, double filter) {
    CDCylinder cdCyl = CDCylinder::mk(D, H);
    cdCyl.setIdentifier(id);
    CDSSCore cdsscore = CDSSCore();
    cdsscore.setCoreDetectionRef(cdCyl);
    cdsscore.setFilterTime(filter);
    return cdsscore;
}

CDSSCore CDSSCore::mkCyl(const std::string& id, double D, double H) {
  return mkCyl(id,D,H,1.0);
}

CDSSCore CDSSCore::mkTauMOD(const std::string& id, double DTHR, double ZTHR, double TTHR, double TCOA) {
    WCV_TAUMOD* wcv = new WCV_TAUMOD();
    wcv->setDTHR(DTHR);    // width of cylinder around ownship
    wcv->setZTHR(ZTHR);    // height of cylinder around ownship
    wcv->setTTHR(TTHR);    // tau forward stretch
    wcv->setTCOA(TCOA);    // also has to do with time -- leave as zero
    wcv->setIdentifier(id);
    CDSSCore core = CDSSCore(wcv,0.0);
    return core;
}


CDSSCore& CDSSCore::operator=(const CDSSCore& cdss) {
  filter = cdss.filter;
  tca = cdss.tca;
  stca = cdss.stca;
  dtca = PINFINITY;
  t_in = cdss.t_in;
  t_out = cdss.t_out;
  if (cd != NULL) {
    delete cd;
  }
  cd = cdss.cd != NULL ? cdss.cd->copy() : NULL;
  return *this;
}

double CDSSCore::getFilterTime() const {
  return filter;
}

void CDSSCore::setFilterTime(double cdfilter) {
  filter = cdfilter;
}

/**
 * Was there a detected conflict?
 */
bool CDSSCore::conflict() const {
  return t_in < t_out;
}

bool CDSSCore::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return cd->violation(so, vo, si, vi);
}

bool CDSSCore::violation(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip, const EuclideanProjection& proj) const {
  if (sop.isLatLon()) {
    Vect3 so = proj.projectPoint(sop);
    Velocity vo = proj.projectVelocity(sop, vop);
    Vect3 si = proj.projectPoint(sip);
    Velocity vi = proj.projectVelocity(sip, vip);
    return violation(so, vo, si, vi);
  } else {
    return violation(sop.vect3(), vop, sip.vect3(), vip);
  }
}

bool CDSSCore::violation(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip) const {
  EuclideanProjection proj = Projection::createProjection(sop.lla().zeroAlt());
  return violation(sop,vop,sip,vip,proj);
}

bool CDSSCore::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double T) const {
  return cd->conflict(so,vo,si,vi,0.0,T);
}

bool CDSSCore::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return cd->conflict(so,vo,si,vi,B,T);
}

bool CDSSCore::conflict(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip, double B, double T) const {
  EuclideanProjection sp = Projection::createProjection(sop.lla());
   //EuclideanProjection sp = Projection.createProjection(sop.lla().zeroAlt());
   Vect3 so = sp.project(sop);
   Velocity vo = sp.projectVelocity(sop,vop);
   Vect3 si = sp.project(sip);
   Velocity vi = sp.projectVelocity(sip,vip);
   return conflict(so,vo,si,vi,B,T);
}


bool CDSSCore::detectionEver(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) {
  ConflictData det = cd->conflictDetection(so, vo, si, vi, 0.0, PINFINITY);
  tca = det.getCriticalTimeOfConflict();
  Vect3 v = vo-vi;
  stca = v.ScalAdd(tca,so.Sub(si));
  dtca = det.getDistanceAtCriticalTime();
  t_in = det.getTimeIn();
  t_out = det.getTimeOut();
  return det.conflict(filter);
}

bool CDSSCore::detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) {
  bool conflict = detectionEver(so,vo,si,vi);
  tca = Util::max(B,tca);
  Vect3 v = vo-vi;
  stca = v.ScalAdd(tca,so.Sub(si));
  return conflict && t_in < T && t_out >= B;

}

bool CDSSCore::detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, double timeHorizon) {
  ConflictData det = cd->conflictDetection(so, vo, si, vi, 0.0, timeHorizon);
  tca = det.getCriticalTimeOfConflict();
  t_in = det.getTimeIn();
  t_out = det.getTimeOut();
  bool conflict = det.conflict(filter);
  tca = timeHorizon < 0 ? Util::max(B,tca) : Util::min(Util::max(B,tca),timeHorizon);
  Vect3 v = vo.Sub(vi);
  stca = v.ScalAdd(tca,so.Sub(si));
  dtca = det.getDistanceAtCriticalTime();
  return conflict && t_in < T && t_out >= B;
}

bool CDSSCore::detectionBetween(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T, const std::string& ut) {
  return detectionBetween(so,vo,si,vi,Units::from(ut,B),Units::from(ut,T));
}

bool CDSSCore::detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double T) {
  return detectionBetween(so,vo,si,vi,0.0,T);
}

bool CDSSCore::detection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double T, const std::string& ut) {
  return detectionBetween(so,vo,si,vi,0.0,Units::from(ut,T));
}

bool CDSSCore::detection(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip, double T) {
     EuclideanProjection proj = Projection::createProjection(sop.lla().zeroAlt());
    return detection(sop,vop,sip,vip,T,proj);
}

bool CDSSCore::detection(const Position& sop, const Velocity& vop, const Position& sip, const Velocity& vip, double T, EuclideanProjection proj) {
    if (sop.isLatLon()) {
        Vect3 so = proj.projectPoint(sop);
        Velocity vo = proj.projectVelocity(sop, vop);
        Vect3 si = proj.projectPoint(sip);
        Velocity vi = proj.projectVelocity(sip, vip);
        return detection(so, vo, si, vi, T);
    } else {
        return detection(sop.vect3(), vop, sip.vect3(), vip, T);
    }
}



double CDSSCore::conflictDuration() const {
  if (!conflict()) return 0;
  return t_out - t_in;
}

double CDSSCore::conflictDuration(const std::string& ut) const {
  return Units::to(ut,conflictDuration());
}

double CDSSCore::getTimeIn() const {
  if (conflict())
    return t_in;
  return PINFINITY;
}

double CDSSCore::getTimeIn(const std::string& ut) const {
  return Units::to(ut,getTimeIn());
}

double CDSSCore::getTimeOut() const {
  if (conflict())
    return t_out;
  return NINFINITY;
}

double CDSSCore::getTimeOut(const std::string& ut) const {
  return Units::to(ut,getTimeOut());
}

double CDSSCore::getCriticalTime() const {
  return tca;
}

double CDSSCore::getCriticalTime(const std::string& ut) const {
  return Units::to(ut,getCriticalTime());
}

double CDSSCore::distanceAtCriticalTime() const {
  return dtca;
}

Vect3 CDSSCore::relativePositionAtCriticalTime() const {
  return stca;
}

std::string CDSSCore::getIdentifier() const {
    return cd->getIdentifier();
}


std::string CDSSCore::toString() const {
  std::string rtn = "CDSSCore: "+ cd->toString()+"\n";
//  if (equals(cd->getCanonicalClassName(),"gov.nasa.larcfm.ACCoRD.CDCylinder")) {
//    double d = ((CDCylinder*)cd)->getHorizontalSeparation("nmi");
//    double h = ((CDCylinder*)cd)->getVerticalSeparation("ft");
//    rtn+= " D="+Fm4(d)+" H="+Fm4(h);
//  }
  rtn = rtn + " t_in = "+Fm4(t_in)+" t_out =  "+Fm4(t_out)+" tca = "+Fm4(tca)+"\n";
  if (t_in == t_out) rtn = rtn + "--------- No conflict ---------";
  rtn = rtn+"\nUsing "+cd->getSimpleClassName();
  return rtn;
}

void CDSSCore::setCoreDetectionPtr(const Detection3D* c) {
  if (cd != NULL) {
    delete cd;
  }
  cd = c != NULL ? c->copy() : NULL;
  tca = 0;
  stca = Vect3::ZERO();
}

void CDSSCore::setCoreDetectionRef(const Detection3D& c) {
  setCoreDetectionPtr(&c);
}

Detection3D* CDSSCore::getCoreDetectionPtr() const {
  return cd;
}

Detection3D& CDSSCore::getCoreDetectionRef() const {
  return *cd;
}

}
