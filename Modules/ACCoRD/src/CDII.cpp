
/* CDII - conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 *
 * NOTE:   The default core detection method is CD3D.  This can be changed through the method "setCoreDetection"
 *
 */


#include "CDII.h"
//#include "UnitSymbols.h"
#include "Util.h"
#include "string_util.h"
#include "CDCylinder.h"
#include "PlanUtil.h"
#include <limits>

namespace larcfm {

using namespace std;

CDII::CDII(double distance, double height): error("CDII") {
  CDCylinder cyl = CDCylinder(distance, height);
  cdii_core = CDIICore(&cyl);
}

CDII::CDII() : error("CDII") {
}

CDII CDII::make(double distance, const std::string& dStr, double height, const std::string& hStr) {
  return CDII(Units::from(dStr, distance), Units::from(hStr,height));
}

CDII CDII::mk(double distance, double height) {
  return CDII(distance, height);
}

CDIICore CDII::getCore() {
  return cdii_core;
}

double CDII::getDistance() const {
  Detection3D* cd = cdii_core.getCoreDetectionPtr();
  if (equals(cd->getCanonicalClassName(), "gov.nasa.larcfm.ACCoRD.CDCylinder")) {
    return ((CDCylinder*)cd)->getHorizontalSeparation();
  }
  error.addError("getDistance: incorrect detection core for this function");
  return std::numeric_limits<double>::quiet_NaN();
}

double CDII::getHeight() const {
  Detection3D* cd = cdii_core.getCoreDetectionPtr();
  if (equals(cd->getCanonicalClassName(), "gov.nasa.larcfm.ACCoRD.CDCylinder")) {
    return ((CDCylinder*)cd)->getVerticalSeparation();
  }
  error.addError("getHeight: incorrect detection core for this function");
  return std::numeric_limits<double>::quiet_NaN();
}

double CDII::getDistance(const std::string& units) const {
  return Units::to(units,getDistance());
}

double CDII::getHeight(const std::string& units) const {
  return Units::to(units,getHeight());
}

void CDII::setDistance(double distance) {
  Detection3D* cd = cdii_core.getCoreDetectionPtr();
  if (equals(cd->getCanonicalClassName(), "gov.nasa.larcfm.ACCoRD.CDCylinder")) {
    ((CDCylinder*)cd)->setHorizontalSeparation(distance);
  } else {
    error.addError("setDistance: incorrect detection core for this function");
  }
}

void CDII::setHeight(double height) {
  Detection3D* cd = cdii_core.getCoreDetectionPtr();
  if (equals(cd->getCanonicalClassName(), "gov.nasa.larcfm.ACCoRD.CDCylinder")) {
    ((CDCylinder*)cd)->setVerticalSeparation(height);
  } else {
    error.addError("setHeight: incorrect detection core for this function");
  }
}

void CDII::setDistance(double distance, const std::string& units) {
  setDistance(Units::from(units, distance));
}

void CDII::setHeight(double height, const std::string& units) {
  setHeight(Units::from(units, height));
}

double CDII::getFilterTime() {
  return cdii_core.getFilterTime();
}

void CDII::setFilterTime(double cdfilter) {
  cdii_core.setFilterTime(cdfilter);
}


int CDII::size() const {
  return cdii_core.size();
}

bool CDII::conflict() const {
  return cdii_core.size() > 0;
}

double CDII::getTimeIn(int i) const {
  if ( ! (0 <= i && i < size())) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeIn");
    return 0.0;
  }
  return cdii_core.getTimeIn(i);
}

double CDII::getTimeOut(int i) const {
  if ( ! (0 <= i && i < size())) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeOut");
    return 0.0;
  }
  return cdii_core.getTimeOut(i);
}

int CDII::getSegmentIn(int i) const {
  if ( ! (0 <= i && i < size())) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getSegmentIn");
    return 0;
  }
  return cdii_core.getSegmentIn(i);
}

int CDII::getSegmentOut(int i) const {
  if ( ! (0 <= i && i < size())) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getSegmentOut");
    return 0;
  }
  return cdii_core.getSegmentOut(i);
}

double CDII::getTimeClosest(int i) const {
  if (i < 0 || i >= size()) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeClosest()");
    return 0.0;
  }
  return cdii_core.getTimeClosest(i);
}

double CDII::getDistanceClosest(int i) const {
  if (i < 0 || i >= size()) {
    error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getDistanceClosest()");
    return 0.0;
  }
  return cdii_core.getDistanceClosest(i);
}

bool CDII::conflictBetween(double start, double end) const {
  return cdii_core.conflictBetween(Units::from("s", start), Units::from("s", end));
}

//  bool CDII::cdii(const Plan& ownship, const Plan& traffic, double distance, double height, double startT, double endT) {
//    if (ownship.isLatLon() != traffic.isLatLon()) {
//      return false;
//    }
//    if (startT > endT) {
//      return false;
//    }
//    distance = Units::from("nm", distance);
//    height = Units::from("ft", height);
//    startT = Units::from("s", startT);
//    endT = Units::from("s", endT);
//    return CDIICore::cdiicore(ownship, traffic, distance, height, startT, endT);
//  }


bool CDII::detection(const Plan& ownship, const Plan& traffic, double startT, double endT, bool interpolateVirtuals) {
  if (ownship.isLatLon() != traffic.isLatLon()) {
    error.addError("Ownship and traffic flight plans are not both Euclidean or Lat/Lon");
    return false;
  }
  if (startT > endT) {
    error.addWarning("startT > endT in detection()");
    return false;
  }
  startT = Units::from("s", startT);
  endT = Units::from("s", endT);
  bool rtn;
  if (interpolateVirtuals) {
    Plan ownshipCopy = ownship; //new copy
    PlanUtil::interpolateVirtuals(ownshipCopy,Units::from("NM",0.1) , startT, endT);
    rtn = cdii_core.detection(ownshipCopy, traffic, startT, endT);
  } else {
    rtn = cdii_core.detection(ownship, traffic, startT, endT);
  }
  error.addReporter(cdii_core);
  return rtn;
}

bool CDII::detection(const Plan& ownship, const Plan& traffic, double startT, double endT) {
  return detection(ownship,traffic,startT,endT,true);
}

bool CDII::detectionExtended(const Plan& ownship, const Plan& traffic, double startT, double endT, bool interpolateVirtuals) {
  if (ownship.isLatLon() != traffic.isLatLon()) {
    error.addError("Ownship and traffic flight plans are not both Euclidean or Lat/Lon");
    return false;
  }
  if (startT > endT) {
    error.addWarning("startT > endT in detection()");
    return false;
  }
  startT = Units::from("s", startT);
  endT = Units::from("s", endT);
  bool rtn;
  if (interpolateVirtuals) {
    Plan ownshipCopy = ownship; //new copy
    PlanUtil::interpolateVirtuals(ownshipCopy,Units::from("NM",0.1) , startT, endT);
    rtn = cdii_core.detectionExtended(ownshipCopy, traffic, startT, endT);
  } else {
    rtn = cdii_core.detectionExtended(ownship, traffic, startT, endT);
  }
  error.addReporter(cdii_core);
  return rtn;
}

bool CDII::detectionExtended(const Plan& ownship, const Plan& traffic, double startT, double endT) {
  return detectionExtended(ownship,traffic,startT,endT,true);
}

void CDII::setCoreDetectionPtr(const Detection3D* d) {
  cdii_core.setCoreDetectionPtr(d);
}

void CDII::setCoreDetectionRef(const Detection3D& d) {
  cdii_core.setCoreDetectionRef(d);
}

Detection3D* CDII::getCoreDetectionPtr() const {
  return cdii_core.getCoreDetectionPtr();
}

Detection3D& CDII::getCoreDetectionRef() const {
  return cdii_core.getCoreDetectionRef();
}



//  // deprecated functions:
//  bool cdii(const Plan& ownship, const Plan& traffic, double distance, double height, double startT, double endT) {
//    return CDII::cdii(ownship, traffic, distance, height, startT, endT);
//    if (ownship.isLatLon() != traffic.isLatLon()) {
//      return false;
//    }
//    if (startT > endT) {
//      return false;
//    }
//    distance = Units::from("nm", distance);
//    height = Units::from("ft", height);
//    startT = Units::from("s", startT);
//    endT = Units::from("s", endT);
//    return cdiicore(ownship, traffic, distance, height, startT, endT);
//  }

/** Experimental.  You are responsible for deleting c after this call. */
void setCoreDetection(Detection3D* c);


}

