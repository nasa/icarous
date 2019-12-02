/*
 * Copyright (c) 2016-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * CDPolycarp.cpp
 *
 *  Created on: Dec 23, 2015
 *      Author: ghagen
 */

#include "CDPolycarp.h"
#include "Polycarp3D.h"
#include "Constants.h"
#include "PolycarpContain.h"

namespace larcfm {

CDPolycarp::CDPolycarp() {
  buff = 0.1;
  fac = 0.001;
  id = "";
  checkNice = true;
}

CDPolycarp::CDPolycarp(double b, double f, bool check) {
  buff = b;
  fac = f;
  id = "";
  checkNice = check;
}

void CDPolycarp::setCheckNice(bool b) {
  checkNice = b;
}

bool CDPolycarp::isCheckNice() {
  return checkNice;
}


double CDPolycarp::getBuff() const {
  return buff;
}

void CDPolycarp::setBuff(double x) {
  buff = x;
}

double CDPolycarp::getFac() const {
  return fac;
}

void CDPolycarp::setFac(double x) {
  fac = x;
}

bool CDPolycarp::isNicePolygon(const Poly3D& mp) const {
  return mp.getBottom() <= mp.getTop() && PolycarpContain::nice_polygon_2D(mp.poly2D().getVerticesRef(), buff);
}

bool CDPolycarp::isNicePolygon(const MovingPolygon3D& mp) const {
  return mp.minalt <= mp.maxalt && PolycarpContain::nice_polygon_2D(mp.horizpoly.polystart, buff);
}

bool CDPolycarp::definitelyInside(const Vect3& so, const Poly3D& si) const {
  return Polycarp3D::definitely_inside(so, si, buff, checkNice);
}

bool CDPolycarp::definitelyOutside(const Vect3& so, const Poly3D& si) const {
  return Polycarp3D::definitely_outside(so, si, buff, checkNice);
}

bool CDPolycarp::nearEdge(const Vect3& so, const Poly3D& si, double h, double v) const {
  return Polycarp3D::nearEdge(so, si, h, v, checkNice);
}


bool CDPolycarp::violation(const Vect3& so, const Velocity& vo, const Poly3D& si) const {
  return Polycarp3D::violation(so, si, buff, checkNice);
}

bool CDPolycarp::conflict(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) const {
  return Polycarp3D::entranceDetection(so, vo, si, B, T, buff, fac, checkNice);
}


bool CDPolycarp::conflictDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) {
  tin.clear();
  tout.clear();
  tca.clear();
  tdist.clear();
  IntervalSet times = Polycarp3D::conflictTimes(so, vo, si, B, T, buff, fac, true, checkNice);
  for (int i = 0; i < times.size(); i++) {
      tin.push_back(times.getInterval(i).low);
      tout.push_back(times.getInterval(i).up);
      double mid = (times.getInterval(i).low + times.getInterval(i).up)/2.0;
      tca.push_back(mid);
      Vect3 so3 = so.linear(vo, mid);
      Vect3 cent = si.position(mid).centroid();
      tdist.push_back(so3.distanceH(cent));
  }
  return times.size() > 0;
}


ParameterData CDPolycarp::getParameters() const {
  ParameterData p;
  updateParameterData(p);
  return p;
}

  void CDPolycarp::updateParameterData(ParameterData& p) const {
    p.set("id", id);
    p.setInternal("buff", buff, "m");
    p.setInternal("fac",fac, "unitless");
    p.setBool("checkNice", checkNice);
}

  void CDPolycarp::setParameters(const ParameterData& p) {
    if (p.contains("id")) {
        id = p.getString("id");
    }
    if (p.contains("buff")) {
        buff = p.getValue("buff");
    }
    if (p.contains("fac")) {
        fac = p.getValue("fac");
    }
    if (p.contains("checkNice")) {
        checkNice = p.getBool("checkNice");
    }
  }



   DetectionPolygon* CDPolycarp::make() const {
     return new CDPolycarp(buff,fac,checkNice);
   }

   DetectionPolygon* CDPolycarp::copy() const {
     CDPolycarp* ret = new CDPolycarp();
     ret->tin = tin;
     ret->tout = tout;
     ret->tca = tca;
     ret->tdist = tdist;
     ret->id = id;
     ret->checkNice = checkNice;
     return ret;
   }



std::vector<double> CDPolycarp::getTimesIn() const {
  return tin;
}

std::vector<double> CDPolycarp::getTimesOut() const {
  return tout;
}

std::vector<double> CDPolycarp::getCriticalTimesOfConflict() const {
  return tca;
}

std::vector<double> CDPolycarp::getDistancesAtCriticalTimes() const {
  return tdist;
}


std::string CDPolycarp::toString() const {
  return "CDPolycarp "+id+" buff="+Fm2(buff)+" fac="+Fm2(fac);
}

std::string CDPolycarp::getClassName() const {
  return "CDPolycarp";
}

std::string CDPolycarp::getIdentifier() const {
  return id;
}

void CDPolycarp::setIdentifier(const std::string& s) {
  id = s;
}

bool CDPolycarp::equals(DetectionPolygon* d) const {
  if (!larcfm::equals(getClassName(), d->getClassName())) return false;
  if (!larcfm::equals(id, d->getIdentifier())) return false;
  if (buff != ((CDPolycarp*)d)->buff) return false;
  if (fac != ((CDPolycarp*)d)->fac) return false;
  if (checkNice != ((CDPolycarp*)d)->checkNice) return false;
  return true;
}


} /* namespace larcfm */
