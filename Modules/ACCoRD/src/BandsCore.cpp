/* 
 * Conflict Prevention Bands
 *
 * Contact: Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "BandsCore.h"
#include "Util.h"
#include "Units.h"
#include "CD3D.h"
#include "CriticalVectors.h"
#include "Velocity.h"
#include <list>
#include <vector>
#include <cmath>
#include <sstream>
#include <sstream>


using namespace larcfm;
using namespace std;

BandsCore::BandsCore() {
  init(Units::from("nmi", 5),
      Units::from("ft", 1000),
      Units::from("min", 0),
      Units::from("min", 3),
      Units::from("kn", 1000),
      Units::from("fpm", 5000));
}

BandsCore::BandsCore(double D, double H, double T, double max_gs, double max_vs) {
  init(D, H, 0, T, max_gs, max_vs);
}

BandsCore::BandsCore(double D, double H, double B, double T, double max_gs, double max_vs) {
  init(D, H, B, T, max_gs, max_vs);
}

void BandsCore::init(double D, double H, double B, double T, double max_gs, double max_vs) {

  // trk_regions = IntervalSet(); // Default value
  // gs_regions = IntervalSet(); // Default value
  // vs_regions = IntervalSet(); // Default value
  // regions = IntervalSet(); // Default value

  setDiameter(D);
  setHeight(H);
  setTimeRange(B,T);
  setMaxGroundSpeed(max_gs);
  setMaxVerticalSpeed(max_vs);

  trkTol = Units::from("deg", 0.0);
  gsTol = Units::from("knot", 0.0);
  vsTol = Units::from("fpm", 0.0);
}

void BandsCore::setTime(double t) {
  B = 0; 
  T = t;
  clear();
}

double BandsCore::getTime() const {
  return T;
}

void BandsCore::setStartTime(double t) {
  B = t;
  clear();
}

double BandsCore::getStartTime() const {
  return B;
}

void BandsCore::setTimeRange(double b, double t) {
  B = b; 
  T = t;
  clear();
}

void BandsCore::setDiameter(double d) {
  D = std::abs(d);
  clear();
}

double BandsCore::getDiameter() const {
  return D;
}

void BandsCore::setHeight(double h) {
  H = std::abs(h);
  clear();
}

double BandsCore::getHeight() const {
  return H;
}

void BandsCore::setMaxGroundSpeed(double gs) {
  max_gs = std::abs(gs);
  clearGroundSpeedRegions();
}

double BandsCore::getMaxGroundSpeed() const {
  return max_gs;
}

void BandsCore::setMaxVerticalSpeed(double vs) {
  max_vs = std::abs(vs);
  clearVerticalSpeedRegions();
}

double BandsCore::getMaxVerticalSpeed() const {
  return max_vs;
}

void BandsCore::setTrackTolerance(double trk) {
  if (trk >= 0) {
    trkTol = trk;
    clearTrackRegions();
  }
}
double BandsCore::getTrackTolerance() const {
  return trkTol;
}

void BandsCore::setGroundSpeedTolerance(double gs) {
  if (gs >= 0) {
    gsTol = gs;
    clearGroundSpeedRegions();
  }
}
double BandsCore::getGroundSpeedTolerance() const {
  return gsTol;
}

void BandsCore::setVerticalSpeedTolerance(double vs) {
  if (vs >= 0) {
    vsTol = vs;
    clearVerticalSpeedRegions();
  }
}
double BandsCore::getVerticalSpeedTolerance() const {
  return vsTol;
}

// This is a different implementation than the Java, for performance
void BandsCore::addTraffic(const Vect3& s, const Velocity& vo, const Velocity& vi, bool do_trk, bool do_gs, bool do_vs) {
  if (do_trk) {
    calcTrkBands(s, vo, vi);
    trk_regions.unions(regions);
  }
  if (do_gs) {
    calcGsBands(s, vo, vi);
    gs_regions.unions(regions);
  }
  if (do_vs) {
    calcVsBands(s, vo, vi);
    vs_regions.unions(regions);
  }
}

// This is a different implementation than the Java, for performance
void BandsCore::addTraffic(const Vect3& s, const Velocity& vo, const Velocity& vi) {
  addTraffic(s,vo,vi,true,true,true);
}

/**
 * Clear all bands to "empty" bands.
 */

void BandsCore::clearTrackRegions() {
  trk_regions.clear();
}

void BandsCore::clearGroundSpeedRegions() {
  gs_regions.clear();
}

void BandsCore::clearVerticalSpeedRegions() {
  vs_regions.clear();
}

void BandsCore::clear(bool do_trk, bool do_gs, bool do_vs) {
  if (do_trk) {
    clearTrackRegions();
  }
  if (do_gs) {
    clearGroundSpeedRegions();
  }
  if (do_vs) {
    clearVerticalSpeedRegions();
  }
}

void BandsCore::clear() {
  clear(true,true,true);
}

const IntervalSet& BandsCore::trackBands() const {
  return trk_regions;
}

bool BandsCore::trackBands(double trk) const {
  return trk_regions.in(trk);
}

int BandsCore::trackSize() const {
  return trk_regions.size();
}

const IntervalSet& BandsCore::groundSpeedBands() const {
  return gs_regions;
}

bool BandsCore::groundSpeedBands(double gs) const {
  return gs_regions.in(gs);
}

int BandsCore::groundSpeedSize() const {
  return gs_regions.size();
}

const IntervalSet& BandsCore::verticalSpeedBands() const {
  return vs_regions;
}
bool BandsCore::verticalSpeedBands(double vs) const {
  return vs_regions.in(vs);
}

int BandsCore::verticalSpeedSize() const {
  return vs_regions.size();
}


std::string BandsCore::toString() const {
  std::ostringstream st;
  st.precision(2);
  st << std::fixed;

  st << "Distance: " << Units::str("nmi", D) << " ";
  st << "Height " << Units::str("ft", H) << " ";
  st << "Start Time: " << Units::str("s", B) << " ";
  st << "End Time: " << Units::str("s", T) << " ";
  st << "Max GS: " << Units::str("knot", max_gs) << " ";
  st << "Max VS: " << Units::str("ft/min", max_vs) << " ";

  return st.str();
}

//
// *************** TRACK BANDS ********************
//

void BandsCore::calcTrkBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const {
  double end_pts[20];
  int end_len = 0;
  list<Vect2> l = CriticalVectors::tracks(s3, vo3, vi3, D, H, B, T);
  list<Vect2>::iterator i = l.begin();

  while (i != l.end()) {
    end_pts[end_len++] = i->compassAngle();
    ++i;
  }
  end_pts[end_len++] = 2*M_PI;
  sort(end_pts, end_pts+end_len);
  regions.clear();
  double lang = 0.0;
  double speed = vo3.norm2D();
  for (int i = 0; i < end_len; i++) {
    double nang = end_pts[i];
    //pln(" >> Angles "+toString(lang, "deg")+"  "+toString(nang, "deg"));
    //pln(" >> T = "+toString(T, "s"));
    //Vect2 v_mid = v_from_trk((lang + nang)/2.0, speed);
    double mid = (lang + nang)/2.0;
    Velocity v_mid = Velocity::mkTrkGsVs(mid, speed, vo3.z);
    //    Vect3 nvo3(trkgs2vx(trk, speed), trkgs2vy(trk, speed), vo3.z);
    if (CD3D::cd3d(s3,v_mid,vi3,D,H,B,T)){
      Interval ntr(lang, nang);
      regions.unions(ntr);
    }
    //pln("-> "+s3+" "+Vv3+" "+ct+"  "+Tamber);
    lang = nang;
  }//for
} // trk_bands



//
// *************** GROUND SPEED BANDS ********************
//

void BandsCore::calcGsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const {
  double end_pts[20];
  int end_len = 0;
  Vect2 vo = vo3.vect2();
  list<Vect2> l = CriticalVectors::groundSpeeds(s3, vo3, vi3, D, H, B, T);
  list<Vect2>::iterator i = l.begin();
  while (i != l.end()) {
    end_pts[end_len++] = i->norm();
    ++i;
  }
  end_pts[end_len++] = max_gs;
  sort(end_pts, end_pts+end_len);
  regions.clear();
  double lastpt = 0.0;
  //double speed = vo3.vect2().norm();
  //Vect2 bvo(vo3.x/speed, vo3.y/speed);         // HERE IS THE DIVIDE BY ZERO
  for (int i = 0; i < end_len; i++) {
    double pt = end_pts[i];
    if (max_gs < pt) pt = max_gs;
    //double scale = (lastpt + pt)/2.0;
    //Vect2 nvo2 =  (bvo * ( (lastpt + pt)/2.0 ));
    //Vect3 nvo3(nvo2, vo3.z);
    //Vect3 nvo3(bvo.x*scale, bvo.y*scale, vo3.z);
    Velocity v_mid = Velocity::mkTrkGsVs(vo.trk(), (lastpt+pt)/2.0, vo3.z);
    if (CD3D::cd3d(s3,v_mid,vi3,D,H,B,T)){
      regions.unions(Interval(lastpt, pt));
    }
    lastpt = pt;
  }
}//gs_bands


//
// *************** VERTICAL SPEED BANDS ********************
//

void BandsCore::calcVsBands(Vect3 s3, Vect3 vo3, Vect3 vi3) const {
  double end_pts[20];
  int end_len = 0;
  list<double> l = CriticalVectors::verticalSpeeds(s3, vo3, vi3, D, H, B, T);
  list<double>::iterator i = l.begin();
  while (i != l.end()) {
    end_pts[end_len++] = *i;
    ++i;
  }
  end_pts[end_len++] = max_vs;
  sort(end_pts, end_pts + end_len);
  //IntervalSet regions;
  regions.clear();
  double lastvs = -max_vs;
  for (int j = 0; j < end_len; j++) {
    double nextvs = end_pts[j];
    if (nextvs < -max_vs) nextvs = -max_vs; 
    if (nextvs > max_vs)  nextvs =  max_vs; 
    double mid_pt = (lastvs+nextvs)/2.0;
    Vect3 nvo3 = Vect3(vo3.x,vo3.y,mid_pt);   
    if (CD3D::cd3d(s3,nvo3,vi3,D,H,B,T)) {
      regions.unions(Interval(lastvs,nextvs));
    } 
    lastvs = nextvs;
  }
} // vs_bands

//
// *************** Refine Bands Information ********************
//

void BandsCore::clearNarrowBands(bool do_trk, bool do_gs, bool do_vs) {
  if (do_trk)
    trk_regions.sweepSingle(trkTol);
  if (do_gs)
    gs_regions.sweepSingle(gsTol);
  if (do_vs)
    vs_regions.sweepSingle(vsTol);
}

void BandsCore::clearTrackBreaks() {
  trk_regions.sweepBreaks(trkTol);
  if (trk_regions.getInterval(0).low <= trkTol &&
      trk_regions.getInterval(trk_regions.size()-1).up+trkTol >= M_PI*2 &&
      trk_regions.getInterval(0).low <= larcfm::to_2pi(trk_regions.getInterval(trk_regions.size()-1).up+trkTol)) {
    trk_regions.unions(Interval(0.0,trk_regions.getInterval(0).up));
    trk_regions.unions(Interval(trk_regions.getInterval(trk_regions.size()-1).low,M_PI*2));
  }
}

void BandsCore::clearGroundSpeedBreaks() {
  gs_regions.sweepBreaks(gsTol);
}

void BandsCore::clearVerticalSpeedBreaks() {
  vs_regions.sweepBreaks(vsTol);
}

void BandsCore::clearBreaks(bool do_trk, bool do_gs, bool do_vs) {
  if (do_trk)
    clearTrackBreaks();
  if (do_gs)
    clearGroundSpeedBreaks();
  if (do_vs)
    clearVerticalSpeedBreaks();
}

void BandsCore::clearBreaks() {
  clearBreaks(true,true,true);
}
