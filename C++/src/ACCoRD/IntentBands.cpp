/* 
 * Conflict Prevention IntentBands (two-region-type version)
 *
 * Contact: Jeff Maddalon
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "Units.h"
//#include "UnitSymbols.h"
#include "Velocity.h"
#include "Vect2.h"
#include "Vect3.h"
#include "IntentBands.h"
#include "GreatCircle.h"
#include "Plan.h"
#include "ErrorLog.h"
#include "Projection.h"
#include <cmath>
#include <cstdlib>

using namespace std;

namespace larcfm {
  
  IntentBands::IntentBands() : error("IntentBands") {
    init(5.0, "nmi", 1000.0, "ft", 0, 3*60.0, "s", 1000.0, "kn", 5000.0, "fpm");
  }
  
  IntentBands::IntentBands(double D, const std::string& dunit, double H, const std::string& hunit, double B, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) : error("IntentBands") {
    init(D, dunit, H, hunit, B, T, tunit, max_gs, gsunit, max_vs, vsunit);
  }
  
  void IntentBands::init(double D, const std::string& dunit, double H, const std::string& hunit, double B, double T, const std::string& tunit, double max_gs, const std::string& gsunit, double max_vs, const std::string& vsunit) {
    setDiameter(D, dunit);
    setHeight(H, hunit);
    setTimeRange(B, T, tunit);
    setMaxGroundSpeed(max_gs, gsunit);
    setMaxVerticalSpeed(max_vs, vsunit);
    
    ownship = false;
    
    needCompute = true;
  }
  
  
  
  bool IntentBands::isLatLon() const {
		if (ownship) return so.isLatLon();
		return true;  // need some default
  }
  

  
  
  void IntentBands::setLookaheadTime(double t, const std::string& unit) {
    red.setTime(Units::from(unit,t));
    clear();
  }
  
  void IntentBands::setTimeRange(double b, double t, const std::string& tunit) {
    red.setTimeRange(Units::from(tunit,b),Units::from(tunit,t));
    clear();
  }
  
  double IntentBands::getLookaheadTime(const std::string& unit) const {
    return Units::to(unit,red.getTime());
  }
  double IntentBands::getStartTime(const std::string& unit) const {
    return Units::to(unit,red.getStartTime());
  }
  
  
  void IntentBands::setDiameter(double d, const std::string& unit) {
    double D = std::abs(Units::from(unit,d));
    red.setDiameter(D);
    clear();
  }
  
  double IntentBands::getDiameter(const std::string& unit) const {
    return Units::to(unit,red.getDiameter());
  }
  
  void IntentBands::setHeight(double h, const std::string& unit) {
    double H = std::abs(Units::from(unit,h));
    red.setHeight(H);
    clear();
  }
  
  double IntentBands::getHeight(const std::string& unit) const {
    return Units::to(unit,red.getHeight());
  }
  
  void IntentBands::setMaxGroundSpeed(double gs, const std::string& unit) {
    double max_gs = std::abs(Units::from(unit,gs));
    red.setMaxGroundSpeed(max_gs);
    clear();
  }
  
  double IntentBands::getMaxGroundSpeed(const std::string& unit) {
    return Units::to(unit, red.getMaxGroundSpeed());
  }
  
  void IntentBands::setMaxVerticalSpeed(double vs, const std::string& unit) {
    double max_vs = std::abs(Units::from(unit,vs));
    red.setMaxVerticalSpeed(max_vs);
    clear();
  }
  
  double IntentBands::getMaxVerticalSpeed(const std::string& unit) {
    return Units::to(unit, red.getMaxVerticalSpeed());
  }
  
  void IntentBands::setTrackTolerance(double trk, const std::string& unit) {
    if (trk >= 0) {
      red.setTrackTolerance(Units::from(unit, trk));
      needCompute = true;
      red.clear();
    }
  }
  double IntentBands::getTrackTolerance(const std::string& unit) const {
    return Units::to(unit, red.getTrackTolerance());
  }
  
  void IntentBands::setGroundSpeedTolerance(double gs, const std::string& unit) {
    if (gs >= 0) {
      red.setGroundSpeedTolerance(Units::from(unit, gs));
      needCompute = true;
      red.clear();
    }
  }
  double IntentBands::getGroundSpeedTolerance(const std::string& unit) const {
    return Units::to(unit, red.getGroundSpeedTolerance());
  }
  
  void IntentBands::setVerticalSpeedTolerance(double vs, const std::string& unit) {
    if (vs >= 0) {
      red.setVerticalSpeedTolerance(Units::from(unit, vs));
      needCompute = true;
      red.clear();
    }
  }
  double IntentBands::getVerticalSpeedTolerance(const std::string& unit) const {
    return Units::to(unit, red.getVerticalSpeedTolerance());
  }
  
 
  
  
  //  SPECIAL: in internal units
  void IntentBands::setOwnship(const Position& s, const Velocity& v, double time, const std::string& tunit) {
    clear();
    so = s;
    vo = v;
    to = Units::from(tunit,time);
    ownship = true;
  }
  
  
  void IntentBands::setOwnshipLL(double lat, double lon, const std::string& hunit, double alt, const std::string& vunit, double trk, const std::string& trkunit, double gs, const std::string& gsunit, double vs, const std::string& vsunit, double time, const std::string& tunit) {
    setOwnship(Position(LatLonAlt::make(lat, hunit, lon, hunit, alt, vunit)),
        Velocity::makeTrkGsVs(trk, trkunit, gs, gsunit, vs, vsunit), time, tunit);
  }
  
  void IntentBands::setOwnshipXYZ(double sx, double sy, const std::string& hpunit, double sz, const std::string& vpunit, double vx, double vy, const std::string& hvunit, double vz, const std::string& vvunit, double time, const std::string& tunit) {
    setOwnship(Position(Vect3(Units::from(hpunit, sx), Units::from(hpunit, sy), Units::from(vpunit, sz))),
        Velocity::makeVxyz(vx, vy, hvunit, vz, vvunit), time, tunit);
  }
  
  
  
  bool IntentBands::addTraffic(const Plan& fp) {
    Vect3 rel;
    if ( ! ownship) {
	   error.addError("SetOwnship must be called before addTraffic!");
      return false;
    }
    
    // latlong mismatch
    if ( isLatLon() != fp.isLatLon()) {
	  error.addError("Some data is Euclidean and some data is Lat/Lon");
      return false;
    }
    // if in latlong, treat so as to 0-vector (relative)
    if (isLatLon()) {
      // error checking
      double T = red.getTime();
      for (int i = 0; i < fp.size()-1 && fp.getTime(i) <= to+T; i++) {
        if (fp.getTime(i+1) >= to) {
          double dt = fp.getTime(i) - to;
          LatLonAlt so2 = GreatCircle::linear_initial(so.lla(), vo, dt);
          if (GreatCircle::distance(so2, fp.point(i).lla()) > Projection::projectionMaxRange()) {
            error.addError("Distances are too great for this projection");
          }
        }
      }
      red.addTrafficLL(so.lat(), so.lon(), so.alt(), vo, to, fp); 
    } else {
      Vect3 sr = so.point();
      red.addTraffic(sr,vo,to,fp); 
    }
    needCompute = true;
    return true;
  }
  
  
  void IntentBands::clear() {
    red.clear();
    
    trackSize = 0;
    groundSize = 0;
    verticalSize = 0;
    
    needCompute = true;
  }
  
  int IntentBands::trackLength() {
    if (needCompute) {
      recompute();
    }
    
    return trackSize;
  }
  
  Interval IntentBands::track(int i, const std::string& unit) {
    return track(i, Units::getFactor(unit));
  }
  
//  Interval IntentBands::track(int i) {
//    return track(i, "deg");
//  }
  
  Interval IntentBands::track(int i, double unit) {
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
  
  BandsRegion::Region IntentBands::trackRegion(int i) {
    if (needCompute) {
      recompute();
    }
    if ( i >= trackSize) {
      i = trackSize;
    }
    
    return trackRegionArray[i];
  }
  
  BandsRegion::Region IntentBands::regionOfTrack(double trk, const std::string& unit) {
    double trk2 = Units::from(unit,trk);
    if (isLatLon()) {
      EuclideanProjection sp = Projection::createProjection(so.lla());
      Velocity v1 = Velocity::mkTrkGsVs(trk2, vo.gs(), vo.vs());
      Velocity v2 = sp.projectVelocity(so.lla(), v1);
      trk2 = v2.compassAngle();
    }  
    if (red.trackBands().in(trk2)) {
      return BandsRegion::NEAR;
    }
    return BandsRegion::NONE;
  }
  
  
  int IntentBands::groundSpeedLength() {
    if (needCompute) {
      recompute();
    }
    
    return groundSize;
  }
  
  Interval IntentBands::groundSpeed(int i, const std::string& unit) {
    return groundSpeed(i, Units::getFactor(unit));
  }
  
//  Interval IntentBands::groundSpeed(int i) {
//    return groundSpeed(i, "kn");
//  }
  
  Interval IntentBands::groundSpeed(int i, double unit) {
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
  
  BandsRegion::Region IntentBands::groundSpeedRegion(int i) {
    if (needCompute) {
      recompute();
    }
    if ( i >= groundSize) {
      i = groundSize;
    }
    
    return groundRegionArray[i];
  }
  
  BandsRegion::Region IntentBands::regionOfGroundSpeed(double gs, const std::string& unit) {
    double gs2 = Units::from(unit,gs);
    if (isLatLon()) {
      EuclideanProjection sp = Projection::createProjection(so.lla());
      Velocity v1 = Velocity::mkTrkGsVs(vo.trk(), gs, vo.vs());
      Velocity v2 = sp.projectVelocity(so.lla(), v1);
      gs = v2.gs();
    }  
    if (red.groundSpeedBands().in(gs2)) {
      return BandsRegion::NEAR;
    }
    return BandsRegion::NONE;
  }
  
  
  int IntentBands::verticalSpeedLength() {
    if (needCompute) {
      recompute();
    }
    
    return verticalSize;
  }
  
  Interval IntentBands::verticalSpeed(int i, const std::string& unit) {
    return verticalSpeed(i, Units::getFactor(unit));
  }
  
//  Interval IntentBands::verticalSpeed(int i) {
//    return verticalSpeed(i, "fpm");
//  }
  
  Interval IntentBands::verticalSpeed(int i, double unit) {
    if (needCompute) {
      recompute();
    }
    if ( i >= verticalSize) {
      i = verticalSize;
    }
    
    return Interval(Units::to(unit, verticalArray[i].low), Units::to(unit, verticalArray[i].up));
  }
  
  BandsRegion::Region IntentBands::verticalSpeedRegion(int i) {
    if (needCompute) {
      recompute();
    }
    if ( i >= verticalSize) {
      i = verticalSize;
    }
    
    return verticalRegionArray[i];
  }
  
  BandsRegion::Region IntentBands::regionOfVerticalSpeed(double vs, const std::string& unit) {
    if (red.verticalSpeedBands().in(Units::from(unit,vs))) {
      return BandsRegion::NEAR;
    }
    return BandsRegion::NONE;
  }
  
  
  
  void IntentBands::recompute() {
    trackCompute();
    groundCompute();
    verticalCompute();
    red.clearBreaks();
    
    needCompute = false;
  }
  
  
  void IntentBands::trackCompute() {
    IntervalSet trk_red = red.trackBands();
    IntervalSet trk_green;
    
    
    trk_green.unions(Interval(0.0, 2*M_PI));
    
    trk_green.diff(trk_red);
    trk_green.sweepSingle(); //only eliminate actual singles
	
    int len = trk_green.size()+trk_red.size();
    if (len > NUM_REGIONS) {
      cout << "Too few NUM_REGIONS, fix and recompile IntentBands" << endl;
      exit(1);
    }
    
    toArrays(trackArray, trackRegionArray, trackSize, trk_red, trk_green);
    trackSize = len;
  }
  
  void IntentBands::groundCompute() {
    IntervalSet gs_red = red.groundSpeedBands(); 
    IntervalSet gs_green;
    
    gs_green.unions(Interval(0.0, red.getMaxGroundSpeed()));
    
    gs_green.diff(gs_red); 
    gs_green.sweepSingle(); //only eliminate actual singles
	
    int len = gs_green.size()+gs_red.size();
    if (len > NUM_REGIONS) {
      cout << "Too few NUM_REGIONS, fix and recompile IntentBands" << endl;
      exit(1);
    }
    
    toArrays(groundArray, groundRegionArray, groundSize, gs_red, gs_green);
    groundSize = len;
  }
  
  void IntentBands::verticalCompute() {
    IntervalSet vs_red = red.verticalSpeedBands(); 
    IntervalSet vs_green;
    
    vs_green.unions(Interval(-red.getMaxVerticalSpeed(), red.getMaxVerticalSpeed()));
    
    vs_green.diff(vs_red);
    vs_green.sweepSingle(); //only eliminate actual singles
	
    int len = vs_green.size()+vs_red.size();
    if (len > NUM_REGIONS) {
      cout << "Too few NUM_REGIONS, fix and recompile IntentBands" << endl;
      exit(1);
    }
    
    toArrays(verticalArray, verticalRegionArray, verticalSize, vs_red, vs_green);
    verticalSize = len;
  }
  
  void IntentBands::toArrays(Interval interval[], BandsRegion::Region intRegion[], int& size,
                             const IntervalSet& red, const IntervalSet& green) {
    int i, j;
    Interval n;
	
    for (j = 0; j < green.size(); j++) {
      n = green.getInterval(j);
      i = order(interval, size, n);
      add(interval, i, n);
      add(intRegion, i, BandsRegion::NONE);
      size++;
    }
    for (j = 0; j < red.size(); j++) {
      n = red.getInterval(j);
      i = order(interval, size, n);
      add(interval, i, n);
      add(intRegion, i, BandsRegion::NEAR);
      size++;
    }
  }
  
  
  void IntentBands::add(Interval arr[], int i, const Interval& r) {
    for (int j = NUM_REGIONS - 2; j >= i; j--) {
      arr[j + 1] = arr[j];
    }
    
    arr[i] = r;
  }
  
  void IntentBands::add(BandsRegion::Region arr[], int i, BandsRegion::Region rt) {
    for (int j = NUM_REGIONS - 2; j >= i; j--) {
      arr[j + 1] = arr[j];
    }
    
    arr[i] = rt;
  }
  
  int IntentBands::order(Interval arr[], int size, const Interval& n) {
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
  
  std::string IntentBands::toString() const {
    return red.toString();
  }

}

