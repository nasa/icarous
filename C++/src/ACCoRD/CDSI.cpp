/*
 * CDSI - Conflict detection between an ownship state vector and a
 * traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDSI.h"
//#include "UnitSymbols.h"
#include "Util.h"
#include "Velocity.h"
#include "CDCylinder.h"

namespace larcfm {

  using namespace std;
 
//  CDSI::CDSI(double distance, double height) :
//    core(Units::from("nm", distance), Units::from("ft", height)),
//    error("CDSI") {
//  }

  CDSI::CDSI(double distance, double height) :  error("CDSI") {
    CDCylinder cyl = CDCylinder(distance, "nm", height, "ft");
    core = CDSICore(&cyl);
  }


  CDSI::CDSI() : error("CDSI") {}

  CDSI::CDSI(Detection3D* cd) : core(cd), error("CDSI") {}

  /**
   * Create a new state-based conflict detection object using specified units.
   *
   * @param distance the minimum horizontal separation distance [hStr]
   * @param height the minimum vertical separation height [dStr].
   */
  CDSI CDSI::make(double distance, const std::string& dUnits, double height, const std::string& hUnits) {
    return CDSI(Units::from(dUnits, distance), Units::from(hUnits,height));
 }

  CDSI CDSI::make(Detection3D* cd) {
    return CDSI(cd);
  }

  /**
   * Create a new state-based conflict detection object using internal units.
   *
   * @param distance the minimum horizontal separation distance [m]
   * @param height the minimum vertical separation height [m].
   */
  CDSI CDSI::mk(double distance, double height) {
     return CDSI(distance, height);
  }


//  double CDSI::getDistance() const {
//    return core.getDistance();
//  }
//
//  double CDSI::getHeight() const {
//    return core.getHeight();
//  }
//
//  double CDSI::getDistance(const std::string& units) const {
//    return Units::to(units, core.getDistance());
//  }
//
//  double CDSI::getHeight(const std::string& units) const {
//    return Units::to(units, core.getHeight());
//  }

  double CDSI::getFilterTime() const {
    return core.getFilterTime();
  }

//  void CDSI::setDistance(double distance) {
//    core.setDistance(distance);
//  }
//
//  void CDSI::setHeight(double height) {
//    core.setHeight(height);
//  }
//
//  void CDSI::setDistance(double distance, const std::string& units) {
//    core.setDistance(Units::from(units, distance));
//  }
//
//  void CDSI::setHeight(double height, const std::string& units) {
//    core.setHeight(Units::from(units, height));
//  }

  
  void CDSI::setFilterTime(double cdfilter) {
    core.setFilterTime(cdfilter);
  }

  int CDSI::size() const {
    return core.size();
  }

  bool CDSI::conflict() const {
    return core.size() > 0;
  }

  double CDSI::getTimeIn(int i) const {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeIn()");
      return 0.0;
    }
    return core.getTimeIn(i);
  }

  double CDSI::getTimeOut(int i) const {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeIn()");
      return 0.0;
    }
    return core.getTimeOut(i);
  }

  int CDSI::getSegmentIn(int i) const {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getSegmentIn()");
      return 0;
    }
    return core.getSegmentIn(i);
  }

  int CDSI::getSegmentOut(int i) const {
    if ( ! (0 <= i && i < size())) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getSegmentOut()");
      return 0;
    }
    return core.getSegmentOut(i);
  }

  double CDSI::getTimeClosest(int i) const {
    if (i < 0 || i >= size()) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getTimeClosest()");
      return 0.0;
    }
    return core.getTimeClosest(i);
  }

  double CDSI::getDistanceClosest(int i) const {
    if (i < 0 || i >= size()) {
      error.addError("Out of range error 0 <= "+to_string(i)+" < "+to_string(size())+" in getDistanceClosest()");
      return 0.0;
    }
    return core.getDistanceClosest(i);
  }


//  bool CDSI::cdsi_xyz(double sx, double sy, double sz,
//		       double vx, double vy, double vz, double t0,
//		       const Plan& intent, double distance, double height, double startT, double endT) {
//    if (startT > endT) {
//      return false;
//    }
//    Vect3 so(Units::from("nm", sx),
//	     Units::from("nm", sy),
//	     Units::from("ft", sz));
//    Velocity vo = Velocity::makeVxyz(vx, vy, "kn", vz, "fpm");
//    t0 = Units::from("s", t0);
//    distance = Units::from("nm", distance);
//    height = Units::from("ft", height);
//    startT = Units::from("s", startT);
//    endT = Units::from("s", endT);
//    return CDSICore::cdsicore_xyz(so, vo, t0, 10.0e+300, intent, distance, height, startT, endT);
//  }
//
//  bool CDSI::cdsi_ll(double lat, double lon, double alt,
//		      double trk, double gs, double vs, double t0,
//		      const Plan& intent, double distance, double height, double startT, double endT) {
//    if (startT > endT) {
//      return false;
//    }
//    //lat = Units::from("deg", lat);
//    //lon = Units::from("deg", lon);
//    //alt = Units::from("ft", alt);
//    Velocity vo = Velocity::makeTrkGsVs(trk, "deg", gs, "kn", vs, "fpm");
//    t0 = Units::from("s", t0);
//    distance = Units::from("nm", distance);
//    height = Units::from("ft", height);
//    startT = Units::from("s", startT);
//    endT = Units::from("s", endT);
//    return CDSICore::cdsicore_ll(LatLonAlt::make(lat, lon, alt), vo, t0, 10.0e+300, intent, distance, height, startT, endT);
//  }

  bool CDSI::detectionXYZ(double sx, double sy, double sz, 
			  double vx, double vy, double vz, double t0, double horizon, 
			  const Plan& intent, double startT, double endT) {
    if (startT > endT) {
      error.addWarning("startT > endT in detectionXYZ");
      return false;
    }
    Vect3 so(Units::from("nm", sx),
	     Units::from("nm", sy),
	     Units::from("ft", sz));
    Velocity vo = Velocity::makeVxyz(vx, vy, "kn", vz, "fpm");
    t0 = Units::from("s", t0);
    startT = Units::from("s", startT);
    endT = Units::from("s", endT);
    bool rtn = core.detectionXYZ(so, vo, t0, horizon, intent, startT, endT);
    error.addReporter(core);
    return rtn;
  }

  bool CDSI::detectionLL(double lat, double lon, double alt, 
			 double trk, double gs, double vs, double t0, double horizon,
			 const Plan& intent, double startT, double endT) {
    if (startT > endT) {
      error.addWarning("startT > endT in detectionLL");
      return false;
    }
    //lat = Units::from("deg", lat);
    //lon = Units::from("deg", lon);
    //alt = Units::from("ft", alt);
    Velocity vo = Velocity::makeTrkGsVs(trk, "deg", gs, "kn", vs, "fpm");
    t0 = Units::from("s", t0);
    startT = Units::from("s", startT);
    endT = Units::from("s", endT);
    bool rtn = core.detectionLL(LatLonAlt::make(lat, lon, alt), vo, t0, horizon, intent, startT, endT);
    error.addReporter(core);
    return rtn;
  }

  void CDSI::setCoreDetectionPtr(const Detection3D* d) {
      core.setCoreDetectionPtr(d);
  }

  void CDSI::setCoreDetectionRef(const Detection3D& d) {
      core.setCoreDetectionRef(d);
  }

  Detection3D* CDSI::getCoreDetectionPtr() const {
    return core.getCoreDetectionPtr();
  }

  Detection3D& CDSI::getCoreDetectionRef() const {
    return core.getCoreDetectionRef();
  }


//  // deprecated functions:
//  bool cdsi_xyz(double sx, double sy, double sz,
//               double vx, double vy, double vz, double t0,
//               const Plan& intent, double distance, double height, double startT, double endT) {
//    return CDSI::cdsi_xyz(sx, sy, sz, vx, vy, vz, t0, intent, distance, height, startT, endT);
//  }
//
//  bool cdsi_ll(double lat, double lon, double alt,
//              double trk, double gs, double vs, double t0,
//              const Plan& intent, double distance, double height, double startT, double endT) {
//    return CDSI::cdsi_ll(lat,lon,alt,trk,gs,vs,t0,intent, distance,height,startT,endT);
//  }



}

