/* 
 * Conflict Prevention Bands (two time version)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Units.h"
#include "Velocity.h"
//#include "UnitSymbols.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "ConflictBandsTwoTimes.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "ErrorLog.h"
#include <cmath>
#include <cstdlib>
#include <memory>

using namespace std;

namespace larcfm {

ConflictBandsTwoTimes::ConflictBandsTwoTimes() : error("ConflictBandsTimesTwo") {
  init(5, "nmi", 1000, "ft", 3*60, 5*60, "s", 1000, "kn", 5000, "fpm");
}

ConflictBandsTwoTimes::ConflictBandsTwoTimes(double D, const std::string& dunit, double H, const std::string& hunit, double Tnear, double Tmid, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) : error("ConflictBandsTimesTwo") {
  init(D, dunit, H, hunit, Tnear, Tmid, tunit, max_gs, gsunit, max_vs, vsunit);
}

void ConflictBandsTwoTimes::init(double D, const std::string& dunit, double H, const std::string& hunit, double Tred, double Tamber, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) {
  setDistance(D, dunit);
  setHeight(H, hunit);
  setTimeNear(Tred, tunit);
  setTimeMid(Tamber, tunit);
  setMaxGroundSpeed(max_gs, gsunit);
  setMaxVerticalSpeed(max_vs, vsunit);
  setMaxGroundSpeed(max_gs, "kn");
  setMaxVerticalSpeed(max_vs, "fpm");

  ownship = false;

  needCompute = true;
}

bool ConflictBandsTwoTimes::isLatLon() const {
  if (ownship) return so.isLatLon();
  return true;  // need some default
}

void ConflictBandsTwoTimes::setTimeNear(double t, const std::string& unit) {
  red.setTime(Units::from(unit,t));
  clear();
}

double ConflictBandsTwoTimes::getTimeNear(const std::string& unit) const {
  return Units::to(unit,red.getTime());
}

void ConflictBandsTwoTimes::setLookaheadTime(double t, const std::string& unit) {
  setTimeNear(t, unit);
}

double ConflictBandsTwoTimes::getLookaheadTime(const std::string& unit) const {
  return getTimeNear(unit);
}


void ConflictBandsTwoTimes::setTimeRange(double b, double t, const std::string& tunit) {
  red.setTimeRange(larcfm::Units::from(tunit,b), larcfm::Units::from(tunit,t));
  clear();
}

double ConflictBandsTwoTimes::getStartTime(const std::string& unit) const {
  return larcfm::Units::to(unit, red.getStartTime());
}


void ConflictBandsTwoTimes::setTimeMid(double t, const std::string& unit) {
  amber.setTime(Units::from(unit,t));
  clear();
}

double ConflictBandsTwoTimes::getTimeMid(const std::string& unit) const {
  return Units::to(unit,amber.getTime());
}

void ConflictBandsTwoTimes::setDistance(double d, const std::string& unit) {
  double D = std::abs(Units::from(unit,d));
  red.setDiameter(D);
  amber.setDiameter(D);
  clear();
}

double ConflictBandsTwoTimes::getDistance(const std::string& unit) const {
  return Units::to(unit,red.getDiameter());
}

void ConflictBandsTwoTimes::setHeight(double h, const std::string& unit) {
  double H = std::abs(Units::from(unit,h));
  red.setHeight(H);
  amber.setHeight(H);
  clear();
}

double ConflictBandsTwoTimes::getHeight(const std::string& unit) const {
  return Units::to(unit,red.getHeight());
}

void ConflictBandsTwoTimes::setMaxGroundSpeed(double gs, const std::string& unit) {
  double max_gs = std::abs(Units::from(unit,gs));
  red.setMaxGroundSpeed(max_gs);
  amber.setMaxGroundSpeed(max_gs);
  clear();
}

double ConflictBandsTwoTimes::getMaxGroundSpeed(const std::string& unit) {
  return Units::to(unit, red.getMaxGroundSpeed());
}

void ConflictBandsTwoTimes::setMaxVerticalSpeed(double vs, const std::string& unit) {
  double max_vs = std::abs(Units::from(unit,vs));
  red.setMaxVerticalSpeed(max_vs);
  amber.setMaxVerticalSpeed(max_vs);
  clear();
}

double ConflictBandsTwoTimes::getMaxVerticalSpeed(const std::string& unit) {
  return Units::to(unit, red.getMaxVerticalSpeed());
}

void ConflictBandsTwoTimes::setTrackTolerance(double trk, const std::string& unit) {
  if (trk >= 0) {
    red.setTrackTolerance(Units::from(unit, trk));
    amber.setTrackTolerance(Units::from(unit, trk));
    needCompute = true;
    red.clear();
    amber.clear();
  }
}
double ConflictBandsTwoTimes::getTrackTolerance(const std::string& unit) const {
  return Units::to(unit, red.getTrackTolerance());
}

void ConflictBandsTwoTimes::setGroundSpeedTolerance(double gs, const std::string& unit) {
  if (gs >= 0) {
    red.setGroundSpeedTolerance(Units::from(unit, gs));
    amber.setGroundSpeedTolerance(Units::from(unit, gs));
    needCompute = true;
    red.clear();
    amber.clear();
  }
}
double ConflictBandsTwoTimes::getGroundSpeedTolerance(const std::string& unit) const {
  return Units::to(unit, red.getGroundSpeedTolerance());
}

void ConflictBandsTwoTimes::setVerticalSpeedTolerance(double vs, const std::string& unit) {
  if (vs >= 0) {
    red.setVerticalSpeedTolerance(Units::from(unit, vs));
    amber.setVerticalSpeedTolerance(Units::from(unit, vs));
    needCompute = true;
    red.clear();
    amber.clear();
  }
}
double ConflictBandsTwoTimes::getVerticalSpeedTolerance(const std::string& unit) const {
  return Units::to(unit, red.getVerticalSpeedTolerance());
}

//  SPECIAL: in internal units
void ConflictBandsTwoTimes::setOwnship(const std::string& id, const Position& s, const Velocity& v) {
  clear();
  so = s;
  vo = v;
  ownship = true;
}


void ConflictBandsTwoTimes::setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit) {
  setOwnship("",Position(LatLonAlt::make(lat, hunit, lon, hunit, alt, vunit)),
      Velocity::makeTrkGsVs(trk, trkunit, gs, gsunit, vs, vsunit));
}

void ConflictBandsTwoTimes::setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit) {
  setOwnship("",Position(Vect3(Units::from(hpunit, sx), Units::from(hpunit, sy), Units::from(vpunit, sz))),
      Velocity::makeVxyz(vx, vy, hvunit, vz, vvunit));
}


//  SPECIAL: in internal units
void ConflictBandsTwoTimes::addTraffic(const std::string& id, const Position& p, const Velocity& v) {

  Vect3 rel;

  Velocity vi = v;

  if ( ! ownship) {
    error.addError("AddTraffic: setOwnship must be called first.");
  } else {
    if (p.isLatLon() != isLatLon()) {
      error.addError("AddTraffic: inconsistent use of lat/lon and Euclidean data.");
    }

    if (isLatLon()) {
      LatLonAlt si = p.lla();
      if (GreatCircle::distance(si, so.lla()) > Projection::projectionMaxRange()) {
        error.addError("Distances are too great for this projection");
      }
      EuclideanProjection sp = Projection::createProjection(so.lla());
      rel = sp.project(so).Sub(sp.project(si));
      vi = sp.projectVelocity(si, vi);
      vo = sp.projectVelocity(so, vo);
    } else {
      Point si = p.point();
      rel = so.point().Sub(si);
    }
    red.addTraffic(rel,vo,vi); 
    amber.addTraffic(rel,vo,vi); 
    needCompute = true;
  }
}

void ConflictBandsTwoTimes::addTrafficLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit) {
  addTraffic("",Position(LatLonAlt::make(lat, hunit, lon, hunit, alt, vunit)),
      Velocity::makeTrkGsVs(trk, trkunit, gs, gsunit, vs, vsunit));
}

void ConflictBandsTwoTimes::addTrafficXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit) {
  addTraffic("",Position(Vect3(Units::from(hpunit, sx), Units::from(hpunit, sy), Units::from(vpunit, sz))),
      Velocity::makeVxyz(vx, vy, hvunit, vz, vvunit));
}


void ConflictBandsTwoTimes::clear() {
  red.clear();
  amber.clear();

  trackSize = 0;
  groundSize = 0;
  verticalSize = 0;

  needCompute = true;
}

int ConflictBandsTwoTimes::trackLength() {
  if (needCompute) {
    recompute();
  }

  return trackSize;
}

//  Interval ConflictBandsTwoTimes::track(int i) {
//    return track(i, "deg");
//  }

Interval ConflictBandsTwoTimes::track(int i, const std::string& unit) {
  return track(i, Units::getFactor(unit));
}

Interval ConflictBandsTwoTimes::track(int i, double unit) {
  if (needCompute) {
    recompute();
  }
  if ( i >= trackSize) {
    i = trackSize;
  }

  double loval = trackArray[i].low;
  double upval = trackArray[i].up;

  if (isLatLon()) {
    Velocity lo1 = Velocity::mkTrkGsVs(trackArray[i].low, vo.gs(), vo.vs());
    Velocity hi1 = Velocity::mkTrkGsVs(trackArray[i].up, vo.gs(), vo.vs());
    EuclideanProjection sp = Projection::createProjection(so.lla());
    Vect3 so3 = sp.project(so);
    Velocity lo2 = sp.inverseVelocity(so3,lo1,true);
    Velocity hi2 = sp.inverseVelocity(so3,hi1,true);
    // deal with special cases around 0, 2pi
    loval = lo2.compassAngle();
    upval = hi2.compassAngle();
    if (trackArray[i].low == 0.0 || trackArray[i].low == 2*M_PI) {
      loval = trackArray[i].low;
    }
    if (trackArray[i].up == 0.0 || trackArray[i].up == 2*M_PI) {
      upval = trackArray[i].up;
    }
    // There is the potential for a problem if the unprojected bands are right near the 0/2pi boundary and the
    // projection causes one (but not both) to cross -- in this special case we have to return return a band that
    // is outside of the 0..2pi range (this will be converted to a -pi..pi range instead).
    if (trackArray[i].low < trackArray[i].up && upval < loval) {
      loval = Util::to_pi(loval);
      upval = Util::to_pi(upval);
    }
  }
  return Interval(larcfm::Units::to(unit, loval), larcfm::Units::to(unit, upval));

}

BandsRegion::Region ConflictBandsTwoTimes::trackRegion(int i) {
  if (needCompute) {
    recompute();
  }
  if ( i >= trackSize) {
    i = trackSize;
  }

  return trackRegionArray[i];
}

BandsRegion::Region ConflictBandsTwoTimes::regionOfTrack(double trk, const std::string& unit) {
  double trk2 = Units::from("deg",trk);
  if (isLatLon()) {
    EuclideanProjection sp = Projection::createProjection(so.lla());
    Velocity v1 = Velocity::mkTrkGsVs(trk2, vo.gs(), vo.vs());
    Velocity v2 = sp.projectVelocity(so.lla(), v1);
    trk2 = v2.compassAngle();
  }
  if (red.trackBands().in(trk2)) {
    return BandsRegion::NEAR;
  }
  if (amber.trackBands().in(trk2)) {
    return BandsRegion::MID;
  }
  return BandsRegion::NONE;
}


int ConflictBandsTwoTimes::groundSpeedLength() {
  if (needCompute) {
    recompute();
  }

  return groundSize;
}

//  Interval ConflictBandsTwoTimes::groundSpeed(int i) {
//    return groundSpeed(i, "kn");
//  }

Interval ConflictBandsTwoTimes::groundSpeed(int i, const std::string& unit) {
  return groundSpeed(i, Units::getFactor(unit));
}

Interval ConflictBandsTwoTimes::groundSpeed(int i, double unit) {
  if (needCompute) {
    recompute();
  }
  if ( i >= groundSize) {
    i = groundSize;
  }

  if (isLatLon()) {
    Velocity lo1 = Velocity::mkTrkGsVs(vo.trk(), groundArray[i].low, vo.vs());
    Velocity hi1 = Velocity::mkTrkGsVs(vo.trk(), groundArray[i].up, vo.vs());
    EuclideanProjection sp = Projection::createProjection(so.lla());
    Vect3 so3 = sp.project(so);
    Velocity lo2 = sp.inverseVelocity(so3,lo1,true);
    Velocity hi2 = sp.inverseVelocity(so3,hi1,true);
    return Interval(Units::to(unit, lo2.gs()), Units::to(unit, hi2.gs()));
  } else {
    return Interval(Units::to(unit, groundArray[i].low), Units::to(unit, groundArray[i].up));
  }
}

BandsRegion::Region ConflictBandsTwoTimes::groundSpeedRegion(int i) {
  if (needCompute) {
    recompute();
  }
  if ( i >= groundSize) {
    i = groundSize;
  }

  return groundRegionArray[i];
}

BandsRegion::Region ConflictBandsTwoTimes::regionOfGroundSpeed(double gs, const std::string& unit) {
  double gs2 = Units::from("kn",gs);
  if (isLatLon()) {
    EuclideanProjection sp = Projection::createProjection(so.lla());
    Velocity v1 = Velocity::mkTrkGsVs(vo.trk(), gs, vo.vs());
    Velocity v2 = sp.projectVelocity(so.lla(), v1);
    gs = v2.gs();
  }
  if (red.groundSpeedBands().in(gs2)) {
    return BandsRegion::NEAR;
  }
  if (amber.groundSpeedBands().in(gs2)) {
    return BandsRegion::MID;
  }
  return BandsRegion::NONE;
}


int ConflictBandsTwoTimes::verticalSpeedLength() {
  if (needCompute) {
    recompute();
  }

  return verticalSize;
}

//  Interval ConflictBandsTwoTimes::verticalSpeed(int i) {
//    return verticalSpeed(i, "fpm");
//  }

Interval ConflictBandsTwoTimes::verticalSpeed(int i, const std::string& unit) {
  return verticalSpeed(i, Units::getFactor(unit));
}

Interval ConflictBandsTwoTimes::verticalSpeed(int i, double unit) {
  if (needCompute) {
    recompute();
  }
  if ( i >= verticalSize) {
    i = verticalSize;
  }

  return Interval(Units::to(unit, verticalArray[i].low), Units::to(unit, verticalArray[i].up));
}

BandsRegion::Region ConflictBandsTwoTimes::verticalSpeedRegion(int i) {
  if (needCompute) {
    recompute();
  }
  if ( i >= verticalSize) {
    i = verticalSize;
  }

  return verticalRegionArray[i];
}

BandsRegion::Region ConflictBandsTwoTimes::regionOfVerticalSpeed(double trk, const std::string& unit) {
  if (red.verticalSpeedBands().in(Units::from(unit,trk))) {
    return BandsRegion::NEAR;
  }
  if (amber.verticalSpeedBands().in(Units::from(unit,trk))) {
    return BandsRegion::MID;
  }
  return BandsRegion::NONE;
}


std::string ConflictBandsTwoTimes::toString() const {
  std::ostringstream st;
  st << " Red Bands: " << red.toString();
  st << " Amber Bands: " << amber.toString();
  return st.str();
}

//
// Internal methods...
//

void ConflictBandsTwoTimes::recompute() {
  trackCompute();
  groundCompute();
  verticalCompute();
  red.clearBreaks();
  amber.clearBreaks();

  needCompute = false;
}


void ConflictBandsTwoTimes::trackCompute() {
  const IntervalSet& trk_red = red.trackBands(); // no copy
  tmp_amber = amber.trackBands(); // implicit copy

  tmp_amber.diff(trk_red);
  tmp_amber.sweepSingle(); //only eliminate actual singles

  int len = tmp_amber.size()+trk_red.size();
  if (len > NUM_REGIONS) {
    cout << "Too few NUM_REGIONS, fix and recompile ConflictBandsTwoTimes" << endl;
    exit(1);
  }

  trackSize = 0;
  toArrays(trackArray, trackRegionArray, trackSize, trk_red, tmp_amber);
  trackSize = len;
}

void ConflictBandsTwoTimes::groundCompute() {
  const IntervalSet& gs_red = red.groundSpeedBands(); // no copy
  tmp_amber = amber.groundSpeedBands(); // implicit copy

  tmp_amber.diff(gs_red);
  tmp_amber.sweepSingle(); //only eliminate actual singles

  int len = tmp_amber.size()+gs_red.size();
  if (len > NUM_REGIONS) {
    cout << "Too few NUM_REGIONS, fix and recompile ConflictBandsTwoTimes" << endl;
    exit(1);
  }

  groundSize = 0;
  toArrays(groundArray, groundRegionArray, groundSize, gs_red, tmp_amber);
  groundSize = len;
}

void ConflictBandsTwoTimes::verticalCompute() {
  const IntervalSet& vs_red = red.verticalSpeedBands();  // no copy
  tmp_amber = amber.verticalSpeedBands();  // implicit copy

  tmp_amber.diff(vs_red);
  tmp_amber.sweepSingle(); //only eliminate actual singles

  int len = tmp_amber.size()+vs_red.size();
  if (len > NUM_REGIONS) {
    cout << "Too few NUM_REGIONS, fix and recompile ConflictBandsTwoTimes" << endl;
    exit(1);
  }

  verticalSize = 0;
  toArrays(verticalArray, verticalRegionArray, verticalSize, vs_red, tmp_amber);
  verticalSize = len;
}

void ConflictBandsTwoTimes::toArrays(Interval interval[], BandsRegion::Region intRegion[], int& size,
    const IntervalSet& red, const IntervalSet& amber) {
  int i, j;
  //Interval n;

  for (j = 0; j < amber.size(); j++) {
    const Interval& n = amber.getInterval(j);
    i = order(interval, size, n);
    add(interval, size, i, n);
    add(intRegion, size, i, BandsRegion::MID);
    size++;
  }
  for (j = 0; j < red.size(); j++) {
    const Interval& n = red.getInterval(j);
    i = order(interval, size, n);
    add(interval, size, i, n);
    add(intRegion, size, i, BandsRegion::NEAR);
    size++;
  }
}


void ConflictBandsTwoTimes::add(Interval arr[], int size, int i, const Interval& r) {
  for (int j = size - 1; j >= i; j--) {
    arr[j + 1] = arr[j];
  }

  arr[i] = r;
}

void ConflictBandsTwoTimes::add(BandsRegion::Region arr[], int size, int i, BandsRegion::Region rt) {
  for (int j = size - 1; j >= i; j--) {
    arr[j + 1] = arr[j];
  }

  arr[i] = rt;
}

int ConflictBandsTwoTimes::order(Interval arr[], int size, const Interval& n) {
  if (size == 0) {
    return 0; // add to beginning
  }

  for (int i = 0; i < size; i++) {
    if (n.low < arr[i].low || (n.low == arr[i].low && n.up < arr[i].up)) {
      return i;
    }
  }

  return size;
}

}
