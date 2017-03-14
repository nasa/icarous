/* 
 * Conflict Prevention Bands (two-region-type version)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Vect2.h"
#include "Vect3.h"
#include "Position.h"
#include "Bands.h"
#include "GreatCircle.h"
#include "Interval.h"
#include "Projection.h"
#include "EuclideanProjection.h"
#include "ErrorLog.h"
#include "format.h"
#include <cmath>
#include <cstdlib>
#include <memory>

using namespace std;

namespace larcfm {

Bands::Bands() : error("Bands") {
  init(5, "nmi", 1000, "ft", 180, "s", 700, "kn", 5000, "fpm");
}

Bands::Bands(double D, const std::string& dunit, double H, const std::string& hunit, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) : error("Bands"){
  init(D, dunit, H, hunit, T, tunit, max_gs, gsunit, max_vs, vsunit);
}

Bands::Bands(const Bands& b) : error("Bands") {
  red = BandsCore(b.red);
  doTrk = b.doTrk;
  doGs = b.doGs;
  doVs = b.doVs;

  computeTrk = b.computeTrk;
  computeGs = b.computeGs;
  computeVs = b.computeVs;

  trackArray = std::vector<Interval>();
  trackArray.insert(trackArray.end(),b.trackArray.begin(),b.trackArray.end());
  trackRegionArray = std::vector<BandsRegion::Region>();
  trackRegionArray.insert(trackRegionArray.end(),b.trackRegionArray.begin(),b.trackRegionArray.end());

  groundArray = std::vector<Interval>();
  groundArray.insert(groundArray.end(),b.groundArray.begin(),b.groundArray.end());
  groundRegionArray = std::vector<BandsRegion::Region>();
  groundRegionArray.insert(groundRegionArray.end(),b.groundRegionArray.begin(),b.groundRegionArray.end());

  verticalArray = std::vector<Interval>();
  verticalArray.insert(verticalArray.end(),b.verticalArray.begin(),b.verticalArray.end());
  verticalRegionArray = std::vector<BandsRegion::Region>();
  verticalRegionArray.insert(verticalRegionArray.end(),b.verticalRegionArray.begin(),b.verticalRegionArray.end());

  conflictBands = b.conflictBands;
  ownship = b.ownship;
  traffic = b.traffic;
}

void Bands::init(double D, const std::string& dunit, double H, const std::string& hunit, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) {
  red = BandsCore(Units::from(dunit,D),Units::from(hunit,H),Units::from(tunit,T),Units::from(gsunit,max_gs),Units::from(vsunit,max_vs));
  doGs = true;
  doTrk = true;
  doVs = true;
  computeTrk = true;
  computeGs = true;
  computeVs = true;
  trackArray = std::vector<Interval>();
  groundArray = std::vector<Interval>();
  verticalArray = std::vector<Interval>();
  trackRegionArray = std::vector<BandsRegion::Region>();
  groundRegionArray = std::vector<BandsRegion::Region>();
  verticalRegionArray = std::vector<BandsRegion::Region>();
  conflictBands = false;
  ownship = TrafficState::INVALID;
  traffic = 0;
}

void Bands::needComputeBands() {
  computeTrk = doTrk;
  computeGs = doGs;
  computeVs = doVs;
}

/* Enable or disable the computation of conflict bands only. Should be called before adding traffic */
void Bands::setConflictBands(bool cb) {
  conflictBands = cb;
  resetRegions();
}

void Bands::doConflictBands(){
  setConflictBands(true);
}

bool Bands::areConflictBands() const {
  return conflictBands;
}

/* Enable or disable the computation of track bands. Should be called before adding traffic */
void Bands::setTrackBands(bool dotrk) {
  doTrk = dotrk;
  computeTrk = dotrk;
  red.clearTrackRegions();
}

void Bands::enableTrackBands() {
  setTrackBands(true);
}

/* Enable or disable the computation of ground speed bands. Should be called before adding traffic */
void Bands::setGroundSpeedBands(bool dogs) {
  doGs = dogs;
  computeGs = dogs;
  red.clearGroundSpeedRegions();
}

void Bands::enableGroundSpeedBands() {
  setGroundSpeedBands(true);
}

/* Enable or disable the computation of vertical speed bands. Should be called before adding traffic */
void Bands::setVerticalSpeedBands(bool dovs) {
  doVs = dovs;
  computeVs = dovs;
  red.clearVerticalSpeedRegions();
}

void Bands::enableVerticalSpeedBands() {
  setVerticalSpeedBands(true);
}

/* Enable the computation of track bands only. Should be called before adding traffic */
void Bands::onlyTrackBands() {
  setTrackBands(true);
  setGroundSpeedBands(false);
  setVerticalSpeedBands(false);
}

/* Enable the computation of ground speed bands only. Should be called before adding traffic */
void Bands::onlyGroundSpeedBands(){
  setTrackBands(false);
  setGroundSpeedBands(true);
  setVerticalSpeedBands(false);
}

/* Enable the computation of vertical speed bands only. Should be called before adding traffic */
void Bands::onlyVerticalSpeedBands(){
  setTrackBands(false);
  setGroundSpeedBands(false);
  setVerticalSpeedBands(true);
}

void Bands::allBands() {
  setTrackBands(true);
  setGroundSpeedBands(true);
  setVerticalSpeedBands(true);
}

void Bands::setLookaheadTime(double t, const std::string& unit) {
  red.setTime(Units::from(unit,t));
  resetRegions();
}

void Bands::setTime(double t) {
  red.setTime(t);
  resetRegions();
}
double Bands::getLookaheadTime(const std::string& unit) const {
  return Units::to(unit,red.getTime());
}

/** Returns the time of the lookahead time in seconds */
double Bands::getTime() const {
  return red.getTime();
}

void Bands::setTimeRange(double b, double t) {
  red.setTimeRange(b,t);
  resetRegions();
}

void Bands::setTimeRange(double b, double t, const std::string& tunit) {
  red.setTimeRange(Units::from(tunit,b), Units::from(tunit,t));
  resetRegions();
}

double Bands::getStartTime() const {
  return red.getStartTime();
}

double Bands::getStartTime(const std::string& unit) const {
  return Units::to(unit, red.getStartTime());
}

bool Bands::isLatLon() const {
  if (ownship.isValid()) return ownship.isLatLon();
  return false;  // need some default
}

void Bands::setDistance(double d, const std::string& unit) {
  double D = std::abs(Units::from(unit,d));
  red.setDiameter(D);
  resetRegions();
}

double Bands::getDistance(const std::string& unit) const {
  return Units::to(unit,red.getDiameter());
}

void Bands::setHeight(double h,const std::string& unit) {
  double H = std::abs(Units::from(unit,h));
  red.setHeight(H);
  resetRegions();
}

double Bands::getHeight(const std::string& unit) const {
  return Units::to(unit,red.getHeight());
}

void Bands::setMaxGroundSpeed(double gs,const std::string& unit) {
  double max_gs = std::abs(Units::from(unit,gs));
  red.setMaxGroundSpeed(max_gs);
  resetRegions();
}

double Bands::getMaxGroundSpeed() const {
  return red.getMaxGroundSpeed();
}

double Bands::getMaxGroundSpeed(const std::string& unit) {
  return Units::to(unit, red.getMaxGroundSpeed());
}

void Bands::setMaxVerticalSpeed(double vs, const std::string& unit) {
  double max_vs = std::abs(Units::from(unit,vs));
  red.setMaxVerticalSpeed(max_vs);
  resetRegions();
}

double Bands::getMaxVerticalSpeed() const {
  return red.getMaxVerticalSpeed();
}

double Bands::getMaxVerticalSpeed(const std::string& unit) {
  return Units::to(unit, red.getMaxVerticalSpeed());
}

void Bands::setTrackTolerance(double trk, const std::string& unit) {
  if (trk >= 0) {
    red.setTrackTolerance(Units::from(unit, trk));
    resetRegions();
  }
}

double Bands::getTrackTolerance(const std::string& unit) const {
  return Units::to(unit, red.getTrackTolerance());
}

void Bands::setGroundSpeedTolerance(double gs, const std::string& unit) {
  if (gs >= 0) {
    red.setGroundSpeedTolerance(Units::from(unit, gs));
    resetRegions();
  }
}

double Bands::getGroundSpeedTolerance(const std::string& unit) const {
  return Units::to(unit, red.getGroundSpeedTolerance());
}

void Bands::setVerticalSpeedTolerance(double vs, const std::string& unit) {
  if (vs >= 0) {
    red.setVerticalSpeedTolerance(Units::from(unit, vs));
    resetRegions();
  }
}

double Bands::getVerticalSpeedTolerance(const std::string& unit) const {
  return Units::to(unit, red.getVerticalSpeedTolerance());
}

//  SPECIAL: in internal units
void Bands::setOwnship(const TrafficState& o) {
  needComputeBands();
  ownship = o;
}

void Bands::setOwnship(const std::string& id, const Position& p, const Velocity& v) {
  setOwnship(TrafficState::makeOwnship(id,p,v));
}

void Bands::setOwnship(const Position& p, const Velocity& v) {
  setOwnship("Ownship",p,v);
}

void Bands::addTraffic(const TrafficState& ac) {
  if (!ac.isValid()) {
    error.addError("addTraffic: invalid aircraft.");
    return;
  }
  if (!ownship.isValid()) {
    error.addError("addTraffic: setOwnship must be called first.");
    return;
  }
  if (ac.isLatLon() != isLatLon()) {
    error.addError("addTraffic: inconsistent use of lat/lon and Euclidean data.");
    return;
  }
  traffic++;
  Vect3 si0 = ac.get_s();
  Vect3 s0 = ownship.get_s().Sub(si0);
  Velocity vi0 = ac.get_v();
  red.addTraffic(s0,ownship.get_v(),vi0,doTrk,doGs,doVs);
  needComputeBands();
}

void Bands::addTraffic(const std::string& id, const Position& pi, const Velocity& vi) {
  if (!ownship.isValid()) {
    error.addError("addTraffic: setOwnship must be called first.");
    return;
  }
  addTraffic(ownship.makeIntruder(id,pi,vi));
}

void Bands::addTraffic(const Position& pi, const Velocity& vi) {
  addTraffic("AC_"+Fmi((traffic+1)),pi,vi);
}

void Bands::clear() {
  resetRegions();
  ownship = TrafficState::INVALID;
}

void Bands::resetRegions() {
  red.clear(doTrk,doGs,doVs);
  needComputeBands();
}

// Return false if track bands cannot turned on
bool Bands::turnTrackBandsOn(){
  if (!ownship.isValid()) {
    return false;
  }
  if (!doTrk) {
    enableTrackBands();
  }
  if (computeTrk) {
    trackCompute();
    computeTrk = false;
    red.clearTrackBreaks();
  }
  return true;
}

bool Bands::turnGroundSpeedBandsOn() {
  if (!ownship.isValid()) {
    return false;
  }
  if (!doGs) {
    enableGroundSpeedBands();
  }
  if (computeGs) {
    groundCompute();
    computeGs = false;
    red.clearGroundSpeedBreaks();
  }
  return true;
}

bool Bands::turnVerticalSpeedBandsOn() {
  if (!ownship.isValid()) {
    return false;
  }
  if (!doVs) {
    enableVerticalSpeedBands();
  } if (computeVs) {
    verticalCompute();
    computeVs = false;
    red.clearVerticalSpeedBreaks();
  }
  return true;
}

int Bands::trackLength() {
  if (!turnTrackBandsOn()) {
    return -1;
  }
  return trackArray.size();
}

Interval Bands::track(int i, const std::string& unit) {
  return track(i, Units::getFactor(unit));
}

Interval Bands::track(int i, double unit) {
  if (!turnTrackBandsOn() || i >= (int)trackArray.size()) {
    return Interval::EMPTY;
  }
  double loval = trackArray[i].low;
  double upval = trackArray[i].up;
  if (isLatLon()) {
    Velocity lo1 = Velocity::mkTrkGsVs(trackArray[i].low, ownship.get_v().gs(), ownship.get_v().vs());
    Velocity hi1 = Velocity::mkTrkGsVs(trackArray[i].up, ownship.get_v().gs(), ownship.get_v().vs());
    Velocity lo2 = ownship.inverseVelocity(lo1);
    Velocity hi2 = ownship.inverseVelocity(hi1);
    // deal with special cases around 0, 2pi
    loval = lo2.compassAngle();
    upval = hi2.compassAngle();
    if (trackArray[i].low == 0.0 || trackArray[i].low == 2*Pi) {
      loval = trackArray[i].low;
    }
    if (trackArray[i].up == 0.0 || trackArray[i].up == 2*Pi) {
      upval = trackArray[i].up;
    }
    // There is the potential for a problem if the unprojected bands are right near the 0/2pi boundary and the
    // projection causes one (but not both) bounds to cross -- in this special case we have to return return a band that
    // is outside of the 0..2pi range (this will be converted to a -pi..pi range instead).
    if (trackArray[i].low < trackArray[i].up && upval < loval) {
      loval = Util::to_pi(loval);
      upval = Util::to_pi(upval);
    }
  }
  return Interval(Units::to(unit, loval), Units::to(unit, upval));
}

BandsRegion::Region Bands::trackRegion(int i) {
  if (!turnTrackBandsOn() || i >= (int)trackArray.size()) {
    return BandsRegion::UNKNOWN;
  }
  return trackRegionArray[i];
}

BandsRegion::Region Bands::regionOfTrack(double trk, const std::string& unit) {
  trk = Util::to_2pi(Units::from(unit, trk));
  if (!turnTrackBandsOn()) {
    return BandsRegion::UNKNOWN;
  }
  for (Interval::nat i = 0; i < trackArray.size(); i++) {
    if (trackArray[i].inCC(trk)) {
      return trackRegionArray[i];
    }
  }
  if (conflictBands) {
    return BandsRegion::NONE;
  } else {
    return BandsRegion::UNKNOWN;
  }
}

int Bands::groundSpeedLength() {
  if (!turnGroundSpeedBandsOn()) {
    return -1;
  }
  return groundArray.size();
}

Interval Bands::groundSpeed(int i, const std::string& unit) {
  return groundSpeed(i, Units::getFactor(unit));
}

Interval Bands::groundSpeed(int i, double unit) {
  if (!turnGroundSpeedBandsOn() || i >= (int)groundArray.size()) {
    return Interval::EMPTY;
  }
  if (isLatLon()) {
    Velocity lo1 = Velocity::mkTrkGsVs(ownship.get_v().trk(), groundArray[i].low, ownship.get_v().vs());
    Velocity hi1 = Velocity::mkTrkGsVs(ownship.get_v().trk(), groundArray[i].up, ownship.get_v().vs());
    Velocity lo2 = ownship.inverseVelocity(lo1);
    Velocity hi2 = ownship.inverseVelocity(hi1);
    return Interval(Units::to(unit, lo2.gs()), Units::to(unit, hi2.gs()));
  } else {
    return Interval(Units::to(unit, groundArray[i].low), Units::to(unit, groundArray[i].up));
  }
}

BandsRegion::Region Bands::groundSpeedRegion(int i) {
  if (!turnGroundSpeedBandsOn() || i >= (int)groundArray.size()) {
    return BandsRegion::UNKNOWN;
  }
  return groundRegionArray[i];
}

BandsRegion::Region Bands::regionOfGroundSpeed(double gs, const std::string& unit) {
  gs = Units::from(unit, gs);
  if (gs < 0.0 || gs > red.getMaxGroundSpeed() || !turnGroundSpeedBandsOn()) {
    return BandsRegion::UNKNOWN;
  }
  for (Interval::nat i = 0; i < groundArray.size(); i++) {
    if (groundArray[i].inCC(gs)) {
      return groundRegionArray[i];
    }
  }
  if (conflictBands) {
    return BandsRegion::NONE;
  } else {
    return BandsRegion::UNKNOWN;
  }
}

int Bands::verticalSpeedLength() {
  if (!turnVerticalSpeedBandsOn()) {
    return -1;
  }
  return verticalArray.size();
}

Interval Bands::verticalSpeed(int i, const std::string& unit) {
  return verticalSpeed(i, Units::getFactor(unit));
}

Interval Bands::verticalSpeed(int i, double unit) {
  if (!turnVerticalSpeedBandsOn() || i >= (int)verticalArray.size()) {
    return Interval::EMPTY;
  }
  return Interval(Units::to(unit, verticalArray[i].low), Units::to(unit, verticalArray[i].up));
}

BandsRegion::Region Bands::verticalSpeedRegion(int i) {
  if (!turnVerticalSpeedBandsOn() || i >= (int)verticalArray.size()) {
    return BandsRegion::UNKNOWN;
  }
  return verticalRegionArray[i];
}

BandsRegion::Region Bands::regionOfVerticalSpeed(double vs, const std::string& unit) {
  vs = Units::from(unit, vs);
  if (vs < -red.getMaxVerticalSpeed() || vs > red.getMaxVerticalSpeed() || !turnVerticalSpeedBandsOn()) {
    return BandsRegion::UNKNOWN;
  }
  for (Interval::nat i = 0; i < verticalArray.size(); i++) {
    if (verticalArray[i].inCC(vs)) {
      return verticalRegionArray[i];
    }
  }
  if (conflictBands) {
    return BandsRegion::NONE;
  } else {
    return BandsRegion::UNKNOWN;
  }
}

/**
 * Provide a copy of the track angle bands.  The angles are in
 * 'compass' angles: 0 to 360 degrees, counter-clockwise from true
 * north.
 */
void Bands::trackCompute() {
  IntervalSet trk_red = red.trackBands();
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,trk_red);
  } else {
    toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,trk_red,0.0,2*Pi);
  }
}

/** Provide a copy of the ground speed bands [knots].  The range of
  ground speed bands is from 0 to max_gs. */
void Bands::groundCompute() {
  IntervalSet gs_red = red.groundSpeedBands();
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,gs_red);
  } else {
    toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,gs_red,0.0,red.getMaxGroundSpeed());
  }
}

/** Provide a copy of the vertical speed bands [feet/min].  The
  range of vertical speeds is -max_vs to max_vs. */
void Bands::verticalCompute() {
  IntervalSet vs_red = red.verticalSpeedBands();
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,vs_red);
  } else {
    toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,vs_red,
        -red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
  }
}

int Bands::order(const std::vector<Interval>& arr, const Interval& n) {
  if (arr.size() == 0) {
    return 0; // add to empty List
  }
  for (Interval::nat i = 0; i < arr.size(); i++) {
    if (n.low < arr[i].low || (n.low == arr[i].low && n.up < arr[i].up)) {
      return i;
    }
  }
  return arr.size();
}

/** Return true if track bands consist of a solid region of the given color  */
bool Bands::solidTrackBand(BandsRegion::Region br) {
  if (trackLength() < 0) {
    return false;
  } else if (conflictBands && br == BandsRegion::NONE) {
    return trackLength() == 0;
  } else if (trackLength() > 0){
    return false;
  } else {
    return
        trackRegion(0) == br &&
        Util::almost_equals(track(0,1).low,0,PRECISION5) &&
        Util::almost_equals(track(0,1).up,2*Pi,PRECISION5);
  }
}

/** Return true if ground speed bands consist of a region band of the given color  */
bool Bands::solidGroundSpeedBand(BandsRegion::Region br) {
  if (groundSpeedLength() < 0) {
    return false;
  } else if (conflictBands && br == BandsRegion::NONE) {
    return groundSpeedLength() == 0;
  } else if (groundSpeedLength() > 0){
    return false;
  } else {
    return
        groundSpeedRegion(0) == br &&
        Util::almost_equals(groundSpeed(0,1).low,0,PRECISION5) &&
        Util::almost_equals(groundSpeed(0,1).up,red.getMaxGroundSpeed(),PRECISION5);
  }
}

/** Return true if vertical speed bands consist of a region band of the given color  */
bool Bands::solidVerticalSpeedBand(BandsRegion::Region br) {
  if (verticalSpeedLength() < 0) {
    return false;
  } else if (conflictBands && br == BandsRegion::NONE) {
    return verticalSpeedLength() == 0;
  } else if (verticalSpeedLength() > 0){
    return false;
  } else {
    return
        verticalSpeedRegion(0) == br &&
        Util::almost_equals(verticalSpeed(0,1).low,-red.getMaxVerticalSpeed(),PRECISION5) &&
        Util::almost_equals(verticalSpeed(0,1).up,red.getMaxVerticalSpeed(),PRECISION5);
  }
}

/*
 * Find first band that is equal/not equal to a given bands region and
 * whose size is greater than or equal to a given tolerance. It returns -1
 * if no such band exists.
 */
int Bands::find_first_explicit_band(bool eq,
    BandsRegion::Region br, double tolerance,
    const std::vector<Interval>& arraylist,
    const std::vector<BandsRegion::Region>& regions) {
  for (Interval::nat i = 0; i < arraylist.size(); ++i) {
    if ((eq ? regions[i] == br : regions[i] != br) &&
        arraylist[i].up - arraylist[i].low >= tolerance) {
      return i;
    }
  }
  return -1;
}
/*
 * Find first band that is equal/not equal to an implicit NONE and whose size is
 * greater than or equal to a given tolerance. Returned index i is such that
 * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
 * It returns -1 if no such band exists.
 * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
 * band is NONE.
 *
 */
int Bands::find_first_implicit_none(double tolerance,
    const std::vector<Interval>& arraylist, const std::vector<BandsRegion::Region>& regions,
    double lb, double ub) {
  if (arraylist.size() == 0) {
    return 0;
  }
  for (Interval::nat i = 0; i <= arraylist.size(); ++i) {
    if (i==0 ? arraylist[i].low - lb > tolerance :
        i == arraylist.size() ? ub - arraylist[i-1].up > tolerance :
            arraylist[i].low -arraylist[i-1].up > tolerance) {
      return i;
    }
  }
  return -1;
}

/*
 * Find first band that is equal/not equal to a given bands region and whose size is
 * greater than or equal to a given tolerance. It returns -1 if no such band exists.
 * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
 * set through setConflictBands(), the returned value corresponds to index i such that
 * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
 * The parameters lb and up are upper and lower bound for the whole band.
 * CAVEAT: This function returns 0, if arraylist is empty. In this case the whole
 * band is NONE.
 */
int Bands::find_first_band(bool eq, BandsRegion::Region br, double tolerance,
    const std::vector<Interval>& arraylist, const std::vector<BandsRegion::Region>& regions,
    double lb, double ub) {
  if (conflictBands && br == BandsRegion::NONE && eq) {
    return find_first_implicit_none(tolerance,arraylist,regions,lb,ub);
  } else {
    return find_first_explicit_band(eq,br,tolerance,arraylist,regions);
  }
}

/*
 * Find first track band that is equal/not equal to a given bands region and whose size is
 * greater than or equal to a given tolerance [rad]. It returns -1 if no such band exists.
 * If finding a band equal to NONE and the bands is a conflict bands, i.e., it has been
 * set through setConflictBands(), the returned value corresponds to index i such that
 * (i-1).up and i.low is an implicit NONE that satisfies the tolerance condition.
 * CAVEAT: This function returns 0 when when the band is empty. In this case the whole
 * band is NONE.
 */
int Bands::firstTrackBand(bool eq, BandsRegion::Region br, double trk) {
  if (!turnTrackBandsOn()) {
    return -1;
  }
  return find_first_band(eq,br,trk,trackArray,trackRegionArray,0,2*Pi);
}

/*
 * Find first ground speed band that is equal/not equal to a given bands region and
 * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if
 * no such band exists. If the bands region is NONE and the bands is a conflict bands,
 * i.e., it has been set through setConflictBands(), the returned value corresponds to
 * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance
 * condition.
 * CAVEAT: This function returns 0 when the band is empty. In this case the whole
 * band is NONE.
 */
int Bands::firstGroundSpeedBand(bool eq, BandsRegion::Region br, double gs) {
  if (!turnGroundSpeedBandsOn()) {
    return -1;
  }
  return find_first_band(eq,br,gs,groundArray,groundRegionArray,0,red.getMaxGroundSpeed());
}

/*
 * Find first vertical speed band that is equal/not equal to a given bands region and
 * whose size is greater than or equal to a given tolerance [m/s]. It returns -1 if
 * no such band exists. If the bands region is NONE and the bands is a conflict bands,
 * i.e., it has been set through setConflictBands(), the returned value corresponds to
 * index i such that (i-1).up and i.low is an implicit NONE that satisfies the tolerance
 * condition.
 * CAVEAT: This function returns 0 when the band is empty. In this case the whole
 * band is NONE.
 */
int Bands::firstVerticalSpeedBand(bool eq, BandsRegion::Region br, double vs) {
  if (!turnVerticalSpeedBandsOn()) {
    return -1;
  }
  return find_first_band(eq,br,vs,verticalArray,verticalRegionArray,
      -red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
}
void Bands::toIntervalSet_fromIntervalArray(IntervalSet& intervalset,
    const std::vector<Interval>& intervalarray, const std::vector<BandsRegion::Region>& regions,
    BandsRegion::Region br) {
  for (Interval::nat i=0; i < intervalarray.size(); ++i) {
    if (regions[i] == br)
      intervalset.unions(intervalarray[i]);
  }
}

void Bands::toArrays(std::vector<Interval>& intervalarray, std::vector<BandsRegion::Region>& regions,
    const IntervalSet& red, const IntervalSet& green){
  int i;

  intervalarray.clear();
  regions.clear();

  for (int j=0; j < green.size(); ++j) {
    Interval n = green.getInterval(j);
    i = order(intervalarray, n);
    intervalarray.insert(intervalarray.begin()+i,n);
    regions.insert(regions.begin()+i,BandsRegion::NONE);

  }
  for (int j=0; j < red.size(); ++j) {
    Interval n = red.getInterval(j);
    i = order(intervalarray, n);
    intervalarray.insert(intervalarray.begin()+i,n);
    regions.insert(regions.begin()+i,BandsRegion::NEAR);
  }
}

void Bands::toIntervalArray_fromIntervalSet(std::vector<Interval>& intervalarray,
    std::vector<BandsRegion::Region>& regions, const IntervalSet& intervalset) {
  intervalarray.clear();
  regions.clear();
  for (int j=0; j < intervalset.size(); ++j) {
    Interval n = intervalset.getInterval(j);
    intervalarray.push_back(n);
    regions.push_back(BandsRegion::NEAR);
  }
}

void Bands::toIntervalArray_fromIntervalSet(std::vector<Interval>& intervalarray,
    std::vector<BandsRegion::Region>& regions, const IntervalSet& intervalset, double lowBound, double upBound) {
  IntervalSet green_bands = IntervalSet();

  green_bands.unions(Interval(lowBound, upBound));
  green_bands.diff(intervalset);

  green_bands.sweepSingle(); //only eliminate actual singles

  toArrays(intervalarray, regions, intervalset, green_bands);
}

void Bands::mergeTrackBands(Bands& bands) {
  if (!turnTrackBandsOn() || !bands.turnTrackBandsOn() ||
      bands.solidTrackBand(BandsRegion::NONE)) {
    return;
  }
  IntervalSet red_bands = IntervalSet();
  toIntervalSet_fromIntervalArray(red_bands,trackArray,trackRegionArray,BandsRegion::NEAR);
  toIntervalSet_fromIntervalArray(red_bands,bands.trackArray,bands.trackRegionArray,BandsRegion::NEAR);
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,red_bands);
  } else {
    toIntervalArray_fromIntervalSet(trackArray,trackRegionArray,red_bands,0.0,2*Pi);
  }
  computeTrk = false;
}

void Bands::mergeGroundSpeedBands(Bands& bands) {
  if (!turnGroundSpeedBandsOn() || !bands.turnGroundSpeedBandsOn() ||
      bands.solidGroundSpeedBand(BandsRegion::NONE)) {
    return;
  }
  IntervalSet red_bands = IntervalSet();
  toIntervalSet_fromIntervalArray(red_bands,groundArray,groundRegionArray,BandsRegion::NEAR);
  toIntervalSet_fromIntervalArray(red_bands,bands.groundArray,bands.groundRegionArray,BandsRegion::NEAR);
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,red_bands);
  } else {
    toIntervalArray_fromIntervalSet(groundArray,groundRegionArray,red_bands,0.0,red.getMaxGroundSpeed());
  }
  computeGs = false;
}

void Bands::mergeVerticalSpeedBands(Bands& bands) {
  if (!turnVerticalSpeedBandsOn() || !bands.turnVerticalSpeedBandsOn() ||
      bands.solidVerticalSpeedBand(BandsRegion::NONE)) {
    return;
  }
  IntervalSet red_bands = IntervalSet();
  toIntervalSet_fromIntervalArray(red_bands,verticalArray,verticalRegionArray,BandsRegion::NEAR);
  toIntervalSet_fromIntervalArray(red_bands,bands.verticalArray,bands.verticalRegionArray,BandsRegion::NEAR);
  if (conflictBands) {
    toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,red_bands);
  } else {
    toIntervalArray_fromIntervalSet(verticalArray,verticalRegionArray,red_bands,-red.getMaxVerticalSpeed(),red.getMaxVerticalSpeed());
  }
  computeVs = false;
}

void Bands::mergeBands(Bands& bands) {
  mergeTrackBands(bands);
  mergeGroundSpeedBands(bands);
  mergeVerticalSpeedBands(bands);
}

std::string Bands::toString() const {
  return red.toString();
}

std::string Bands::strBands() {
  std::string rtn = "";
  rtn = rtn +("\nTrack Bands [deg,deg]:");
  for (int i=0; i < trackLength(); ++i) {
    rtn = rtn +("  "+track(i,"deg").toString()+" "+BandsRegion::to_string(trackRegion(i)));
  }
  rtn = rtn +("\nGround Speed Bands [knot,knot]:");
  for (int i=0; i < groundSpeedLength(); ++i) {
    rtn = rtn +("  "+groundSpeed(i,"kn").toString()+" "+BandsRegion::to_string(groundSpeedRegion(i)));
  }
  rtn = rtn +("\nVertical Speed Band [fpm,fpm]:");
  for (int i=0; i < verticalSpeedLength(); ++i) {
    rtn = rtn +("  "+verticalSpeed(i,"fpm").toString()+" "+BandsRegion::to_string(verticalSpeedRegion(i)));
  }
  return rtn;
}


}
