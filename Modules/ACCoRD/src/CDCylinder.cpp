/*
 * CD3D.h 
 * Release: ACCoRD++-2.b (08/22/10) 
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDCylinder.h"
#include "LossData.h"
#include "CD3DTable.h"
#include "LossData.h"
#include "ConflictData.h"
#include <cmath>

#include "CD3D.h"
#include "format.h"

namespace larcfm {

/**
 * Instantiates a new CD3D object.
 */
CDCylinder::CDCylinder() {
  id = "";
}


CDCylinder::CDCylinder(const CD3DTable& tab) {
  table = tab;
  id = "";
}

CDCylinder::CDCylinder(double d, double h) {
  table.setHorizontalSeparation(d);
  table.setVerticalSeparation(h);
  id = "";
}

CDCylinder::CDCylinder(double d, const std::string& dunit, double h, const std::string& hunit) {
  table.setHorizontalSeparation(d,dunit);
  table.setVerticalSeparation(h,hunit);
  id = "";
}

CDCylinder CDCylinder::make(double distance, const std::string& dUnits, double height, const std::string& hUnits) {
  return CDCylinder(distance,dUnits,height,hUnits);
}

CDCylinder CDCylinder::mk(double distance, double height) {
  return CDCylinder(distance, "m", height, "m");
}

CD3DTable CDCylinder::getCD3DTable() {
  return table;
}

void CDCylinder::setCD3DTable(const CD3DTable& tab) {
  table = tab;
}

double CDCylinder::getHorizontalSeparation() const {
  return table.getHorizontalSeparation();
}

double CDCylinder::getVerticalSeparation() const {
  return table.getVerticalSeparation();
}

void CDCylinder::setHorizontalSeparation(double d) {
  table.setHorizontalSeparation(d);
}

void CDCylinder::setVerticalSeparation(double h) {
  table.setVerticalSeparation(h);
}

double CDCylinder::getHorizontalSeparation(const std::string& unit) const {
  return table.getHorizontalSeparation(unit);
}

double CDCylinder::getVerticalSeparation(const std::string& unit) const {
  return table.getVerticalSeparation(unit);
}

void CDCylinder::setHorizontalSeparation(double d, const std::string& unit) {
  // we need to ensure that this is a new table unique to this instance
  table.setHorizontalSeparation(d,unit);
}

void CDCylinder::setVerticalSeparation(double h, const std::string& unit) {
  // we need to ensure that this is a new table unique to this instance
  table.setVerticalSeparation(h,unit);
}

LossData CDCylinder::detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double B, const double T) const {
  return CD3D::detection(s,vo,vi,D,H,B,T);
}

LossData CDCylinder::detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H, const double T) const {
  return detection(s,vo,vi,D,H,0.0,T);
}

LossData CDCylinder::detection(const Vect3& s, const Vect3& vo, const Vect3& vi, const double D, const double H) const {
  return detection(s,vo,vi,D,H,0.0,PINFINITY);
}

bool CDCylinder::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H) const {
  return CD3D::lossOfSep(so,si,D,H);
}

bool CDCylinder::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double B, double T) const {
  return CD3D::cd3d(so.Sub(si), vo, vi, D, H, B, T);
}

ConflictData CDCylinder::conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double B, double T) const {
  Vect3 s = so.Sub(si);
  Vect3 v = vo.Sub(vi);
  double t_tca = CD3D::tccpa(s, vo, vi, D, H, B, T);
  double dist_tca = s.linear(v,t_tca).cyl_norm(D, H);
  LossData ld = CD3D::detection(s,vo,vi,D,H,B,T);
  return ConflictData(ld,t_tca,dist_tca,s,v);
}

double CDCylinder::timeOfClosestApproach(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double D, double H, double B, double T) {
  return CD3D::tccpa(so.Sub(si), vo, vi, D, H, B, T);
}

bool CDCylinder::violation(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi) const {
  return violation(so,vo,si,vi,table.D,table.H);
}

bool CDCylinder::conflict(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return conflict(so, vo, si, vi, table.D, table.H, B, T);
}

ConflictData CDCylinder::conflictDetection(const Vect3& so, const Velocity& vo, const Vect3& si, const Velocity& vi, double B, double T) const {
  return conflictDetection(so,vo,si,vi,table.D, table.H, B, T);
}

CDCylinder* CDCylinder::make() const {
  return new CDCylinder();
}

CDCylinder* CDCylinder::copy() const {
  CDCylinder* cd = new CDCylinder();
  cd->table = table;
  cd->id = id;
  return cd;
}

ParameterData CDCylinder::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

void CDCylinder::updateParameterData(ParameterData& p) const {
  table.updateParameterData(p);
  p.set("id",id);
}

void CDCylinder::setParameters(const ParameterData& p) {
  table.setParameters(p);
  if (p.contains("id")) {
    id = p.getString("id");
  }
}

std::string CDCylinder::getSimpleClassName() const {
  return "CDCylinder";
}

std::string CDCylinder::toString() const {
  return (id == "" ? "" : id+" : ")+getSimpleClassName()+" = {"+table.toString()+"}";
}

std::string CDCylinder::toPVS() const {
  return getSimpleClassName()+"("+table.toPVS()+")";
}

std::string CDCylinder::getIdentifier() const {
  return id;
}

void CDCylinder::setIdentifier(const std::string& s) {
  id = s;
}

bool CDCylinder::equals(Detection3D* d) const {
  if (!larcfm::equals(getCanonicalClassName(), d->getCanonicalClassName())) return false;
  if (!larcfm::equals(id, d->getIdentifier())) return false;
  if (!table.equals(((CDCylinder*)d)->table)) return false;
  return true;
}

bool CDCylinder::contains(const Detection3D* cd) const {
  if (larcfm::equals(getCanonicalClassName(), cd->getCanonicalClassName())) {
    CDCylinder* d = (CDCylinder*)cd;
    return table.D >= d->table.D && table.H >= d->table.H;
  }
  return false;
}

}

