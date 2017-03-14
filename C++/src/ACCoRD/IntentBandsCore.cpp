/* 
 * Conflict Prevention Bands
 *
 * Contact: George Hagen, Jeff Maddalon, Rick Butler, Cesar Munoz
 * Organization: NASA/Langley Research Center
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "IntentBandsCore.h"
#include "Util.h"
#include "Units.h"
#include "CDSI.h"
#include "CriticalVectorsSI.h"
#include "CDCylinder.h"
#include "Plan.h"
#include "Velocity.h"
#include <list>
#include <vector>
#include <cmath>
#include <sstream>

using namespace larcfm;
using namespace std;

bool IntentBandsCore::allowVariableProtectionZones = true;


IntentBandsCore::IntentBandsCore() {
  init(Units::from("nmi", 5),
       Units::from("ft", 1000),
       Units::from("min", 0),
       Units::from("min", 3),
       Units::from("kn", 1000),
       Units::from("fpm", 5000));
}

IntentBandsCore::IntentBandsCore(double D, double H, double B, double T, double max_gs, double max_vs) {
  init(D, H, B, T, max_gs, max_vs);
}

void IntentBandsCore::init(double D, double H, double B, double T, double max_gs, double max_vs) {

  setDiameter(D);
  setHeight(H);
  setTimeRange(B,T);
  setMaxGroundSpeed(max_gs);
  setMaxVerticalSpeed(max_vs);

	trkTol = Units::from("deg", 0.0);
	gsTol = Units::from("knot", 0.0);
	vsTol = Units::from("fpm", 0.0);
}

void IntentBandsCore::setTime(double t) {
  B = 0; 
  T = t;
  clear();
}

double IntentBandsCore::getTime() const {
  return T;
}

double IntentBandsCore::getStartTime() const {
  return B;
}

void IntentBandsCore::setTimeRange(double b, double t) {
  B = b; 
  T = t;
  clear();
}

    
void IntentBandsCore::setDiameter(double d) {
  D = std::abs(d);
  clear();
}

double IntentBandsCore::getDiameter() const {
  return D;
}

void IntentBandsCore::setHeight(double h) {
  H = std::abs(h);
  clear();
}

double IntentBandsCore::getHeight() const {
  return H;
}

void IntentBandsCore::setMaxGroundSpeed(double gs) {
  max_gs = std::abs(gs);
  clear();
}

double IntentBandsCore::getMaxGroundSpeed() const {
  return max_gs;
}

void IntentBandsCore::setMaxVerticalSpeed(double vs) {
  max_vs = std::abs(vs);
  clear();
}

double IntentBandsCore::getMaxVerticalSpeed() const {
  return max_vs;
}

void IntentBandsCore::setTrackTolerance(double trk) {
  if (trk >= 0) {
    trkTol = trk;
  }
}
double IntentBandsCore::getTrackTolerance() const {
  return trkTol;
}

void IntentBandsCore::setGroundSpeedTolerance(double gs) {
  if (gs >= 0) {
    gsTol = gs;
  }
}
double IntentBandsCore::getGroundSpeedTolerance() const {
  return gsTol;
}

void IntentBandsCore::setVerticalSpeedTolerance(double vs) {
  if (vs >= 0) {
    vsTol = vs;
  }
}
double IntentBandsCore::getVerticalSpeedTolerance() const {
  return vsTol;
}


void IntentBandsCore::addTraffic(const Vect3& so, const Velocity& vo, double to, const Plan& fp) {
  trk_regions.unions(calcTrkBands(so, vo, to, fp));
  gs_regions.unions(calcGsBands(so, vo, to, fp));
  vs_regions.unions(calcVsBands(so, vo, to, fp));
}

void IntentBandsCore::addTrafficLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) {
  trk_regions.unions(calcTrkBandsLL(lat, lon, alt, vo, to, fp));
  gs_regions.unions(calcGsBandsLL(lat, lon, alt, vo, to, fp));
  vs_regions.unions(calcVsBandsLL(lat, lon, alt, vo, to, fp));
}

void IntentBandsCore::clear() {
  trk_regions.clear();
  gs_regions.clear();
  vs_regions.clear();
}

const IntervalSet& IntentBandsCore::trackBands() const {
  return trk_regions;
}

const IntervalSet& IntentBandsCore::groundSpeedBands() const {
  return gs_regions;
}

const IntervalSet& IntentBandsCore::verticalSpeedBands() const {
  return vs_regions;
}


//
// *************** TRACK BANDS ********************
//

IntervalSet IntentBandsCore::calcTrkBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const {

  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
//

  list<Vect2> l = CriticalVectorsSI::tracks(so3, vo3, to, fp, d, h, B, T);
  list<Vect2>::iterator i = l.begin();

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;

  while (i != l.end()) {
    end_pts[end_len++] = i->compassAngle();
    ++i;
  }

  end_pts[end_len++] = 2*M_PI;
  sort(end_pts.begin(), end_pts.end());
  
      
  IntervalSet regions;
  double lang = 0.0;
  double speed = vo3.gs();
  Detection3D* cd = new CDCylinder(d, "m", h, "m");

  for (int i = 0; i < end_len; i++) {
    double nang = end_pts[i];
    //pln(" >> Angles "+toString(lang, "deg")+"  "+toString(nang, "deg"));
    //pln(" >> T = "+toString(T, "s"));
    Velocity v_mid = Velocity::mkTrkGsVs((lang + nang)/2.0, speed, vo3.z);

    if (CDSICore::cdsicore_xyz(so3, v_mid, cd, to, 10.0e+300, fp, B,T)){
      Interval ntr(lang, nang);
      regions.unions(ntr);
    }

    //pln("-> "+s3+" "+Vv3+" "+ct+"  "+Tamber);
    lang = nang;
  }//for
  return regions;
} // trk_bands
    

IntervalSet IntentBandsCore::calcTrkBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const {

  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
  
  list<Vect2> l = CriticalVectorsSI::tracksLL(lat, lon, alt, vo, to, fp, d, h, B, T);
  list<Vect2>::iterator i = l.begin();
  LatLonAlt so = LatLonAlt::mk(lat, lon, alt);

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;
 
  while (i != l.end()) {
    end_pts[end_len++] = i->compassAngle();
    ++i;
  }
  
  end_pts[end_len++] = 2*M_PI;
  sort(end_pts.begin(), end_pts.end());
  
  IntervalSet regions;
  double lang = 0.0;
  double speed = vo.gs();
  Detection3D* cd = new CDCylinder(d, "m", h, "m");
  
  for (int i = 0; i < end_len; i++) {
    double nang = end_pts[i];
    //pln(" >> Angles "+toString(lang, "deg")+"  "+toString(nang, "deg"));
    //pln(" >> T = "+toString(T, "s"));
    Velocity v_mid = Velocity::mkTrkGsVs((lang + nang)/2.0, speed, vo.z);
    if (CDSICore::cdsicore_ll(so, v_mid, cd, to, 10.0e+300, fp,B,T)){
      Interval ntr(lang, nang);
      regions.unions(ntr);
    }
    
    //pln("-> "+s3+" "+Vv3+" "+ct+"  "+Tamber);
    lang = nang;
  }//for
  
  return regions;
} // trk_bandsLL

  

//
// *************** GROUND SPEED BANDS ********************
//

IntervalSet IntentBandsCore::calcGsBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const {

  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
  

  list<Vect2> l = CriticalVectorsSI::groundSpeeds(so3, vo3, to, fp, d, h, B, T);
  list<Vect2>::iterator i = l.begin();

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;

  while (i != l.end()) {
    end_pts[end_len++] = i->norm();
    ++i;
  }

  end_pts[end_len++] = max_gs;
  sort(end_pts.begin(), end_pts.end());

        
  IntervalSet regions;
  double lastpt = 0.0;
  double trk = vo3.trk();
  Detection3D* cd = new CDCylinder(d, "m", h, "m");

  for (int i = 0; i < end_len; i++) {
    double pt = end_pts[i];
    if (max_gs < pt) pt = max_gs;
    Velocity v_mid = Velocity::mkTrkGsVs(trk, (lastpt+pt)/2.0, vo3.z);
    if (CDSICore::cdsicore_xyz(so3, v_mid, cd, to, 10.0e+300, fp, B,T)){
      regions.unions(Interval(lastpt, pt));
    }
    lastpt = pt;
  }

  return regions;
}//gs_bands

IntervalSet IntentBandsCore::calcGsBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const {
 
  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
  
 
  LatLonAlt so = LatLonAlt::mk(lat, lon, alt);
  list<Vect2> l = CriticalVectorsSI::groundSpeedsLL(lat, lon, alt, vo, to, fp, d, h, B, T);
  list<Vect2>::iterator i = l.begin();

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;
  
  while (i != l.end()) {
    end_pts[end_len++] = i->norm();
    ++i;
  }
  
  end_pts[end_len++] = max_gs;
  sort(end_pts.begin(), end_pts.end());
  
  
  IntervalSet regions;
  double lastpt = 0.0;  
  double trk = vo.trk();
  Detection3D* cd = new CDCylinder(d, "m", h, "m");

  for (int i = 0; i < end_len; i++) {
    double pt = end_pts[i];
    if (max_gs < pt) pt = max_gs;
    Velocity v_mid = Velocity::mkTrkGsVs(trk, (lastpt+pt)/2.0, vo.z);
    if (CDSICore::cdsicore_ll(so, v_mid, cd, to, 10.0e+300, fp,B,T)){
      regions.unions(Interval(lastpt, pt));
    }
    lastpt = pt;
  }
  
  return regions;
}//gs_bandsLL    
    
//
// *************** VERTICAL SPEED BANDS ********************
//
    
IntervalSet IntentBandsCore::calcVsBands(const Vect3& so3, const Velocity& vo3, double to, const Plan& fp) const {

  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
  

  list<double> l = CriticalVectorsSI::verticalSpeeds(so3, vo3, to, fp, d, h, B, T);
  list<double>::iterator i = l.begin();

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;

  while (i != l.end()) {
    end_pts[end_len++] = *i;
    ++i;
  }

  end_pts[end_len++] = max_vs;
  sort(end_pts.begin(), end_pts.end());

  IntervalSet regions;
  double lastvs = -max_vs;
  Velocity nvo3;
  Detection3D* cd = new CDCylinder(d, "m", h, "m");

  for (int j = 0; j < end_len; j++) {
    double nextvs = end_pts[j];
    if (nextvs < -max_vs) nextvs = -max_vs; 
    if (nextvs > max_vs)  nextvs =  max_vs; 

    double mid_pt = (lastvs+nextvs)/2.0;
    nvo3 = Velocity::mkVxyz(vo3.x,vo3.y,mid_pt);
    
    if (CDSICore::cdsicore_xyz(so3, nvo3, cd, to, 10.0e+300, fp, B, T)) {
      regions.unions(Interval(lastvs,nextvs));
    } 

    lastvs = nextvs;
  }

  return regions;

} // vs_bands


IntervalSet IntentBandsCore::calcVsBandsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp) const {

  // allow for variable protection zone sizes
  double d = D;
  double h = H;
//  if (allowVariableProtectionZones) {
//    if (fp.getProtectionDistance() > d) {
//      d = fp.getProtectionDistance();
//    }
//    if (fp.getProtectionHeight() > h) {
//      h = fp.getProtectionHeight();
//    }
//  }
  
  LatLonAlt so = LatLonAlt::mk(lat, lon, alt);
  list<double> l = CriticalVectorsSI::verticalSpeedsLL(lat, lon, alt, vo, to, fp, d, h, B, T);
  list<double>::iterator i = l.begin();

  vector<double> end_pts(l.size()+1,0);
  int end_len = 0;

  while (i != l.end()) {
    end_pts[end_len++] = *i;
    ++i;
  }
  
  end_pts[end_len++] = max_vs;
  sort(end_pts.begin(), end_pts.end());
  
  IntervalSet regions;
  double lastvs = -max_vs;
  Velocity nvo3;
  Detection3D* cd = new CDCylinder(d, "m", h, "m");

  for (int j = 0; j < end_len; j++) {
    double nextvs = end_pts[j];
    if (nextvs < -max_vs) nextvs = -max_vs; 
    if (nextvs > max_vs)  nextvs =  max_vs; 
    
    double mid_pt = (lastvs+nextvs)/2.0;
    nvo3 = Velocity::mkVxyz(vo.x, vo.y, mid_pt);
    
    if (CDSICore::cdsicore_ll(so, nvo3, cd, to, 10.0E+300, fp,B,T)){
      regions.unions(Interval(lastvs,nextvs));
    } 
    
    lastvs = nextvs;
  }
  
  return regions;
  
} // vs_bandsLL


//
// *************** Refine Bands Information ********************
//

void IntentBandsCore::clearNarrowBands() {
	trk_regions.sweepSingle(trkTol);
	gs_regions.sweepSingle(gsTol);
	vs_regions.sweepSingle(vsTol);
}

void IntentBandsCore::clearBreaks() {
  trk_regions.sweepBreaks(trkTol);
  if (trk_regions.getInterval(0).low <= trkTol &&
      trk_regions.getInterval(trk_regions.size()-1).up+trkTol >= M_PI*2 &&
      trk_regions.getInterval(0).low <= larcfm::to_2pi(trk_regions.getInterval(trk_regions.size()-1).up+trkTol)) {
    trk_regions.unions(Interval(0.0,trk_regions.getInterval(0).up));
    trk_regions.unions(Interval(trk_regions.getInterval(trk_regions.size()-1).low,M_PI*2));
  }
  gs_regions.sweepBreaks(gsTol);
  vs_regions.sweepBreaks(vsTol);
}


//
// -------------------- Output operations ---------------------
//
    
void IntentBandsCore::pln(std::string str) const {
  std::cout << str << std::endl;
}
    
void IntentBandsCore::p(std::string str) const {
  std::cout << str;
}

std::string IntentBandsCore::toString() const {
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





