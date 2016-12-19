/*
 * CDIICore - the core algorithm for conflict detection between two aircraft with intent information for each.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */


#include "CDIICore.h"
#include "Util.h"
#include "Plan.h"
#include "format.h"
#include <vector>

namespace larcfm {

CDIICore CDIICore::CDIICore_def = CDIICore();


CDIICore::CDIICore() : error("CDIICore") {
  //
  //  for (int i = 0; i < SIZE; i++) {
  //    tin[i] = 0;
  //    tout[i] = 0;
  //    tcpa[i] = 0;
  //    dist_tca[i] = 0;
  //    segin[i] = 0;
  //    segout[i] = 0;
  //  }
  //  tin_size = 0;
}


CDIICore::CDIICore(Detection3D* cd):
                    cdsicore(cd),
                    error("CDIICore") {
  //  for (int i = 0; i < SIZE; i++) {
  //    tin[i] = 0;
  //    tout[i] = 0;
  //    tcpa[i] = 0;
  //    dist_tca[i] = 0;
  //    segin[i] = 0;
  //    segout[i] = 0;
  //  }
  //  tin_size = 0;
}

//CDIICore::CDIICore(double distance, double height):
//                cdsi(distance, height),
//                error("CDIICore") {
//  for (int i = 0; i < SIZE; i++) {
//    tin[i] = 0;
//    tout[i] = 0;
//    tcpa[i] = 0;
//    dist_tca[i] = 0;
//    segin[i] = 0;
//    segout[i] = 0;
//  }
//  tin_size = 0;
//}

CDIICore::CDIICore(const CDIICore& cdiicore) : error("CDIICore") {
  //fpln(" $$$$$$$ CDIICore COPY");
  //  tin_size  = cdiicore.tin_size;
  for (int i = 0; i < (int)cdiicore.tin.size(); i++) {
    tin.push_back(cdiicore.tin[i]);
    tout.push_back(cdiicore.tout[i]);
    tcpa.push_back(cdiicore.tcpa[i]);
    dist_tca.push_back(cdiicore.dist_tca[i]);
    segin.push_back(cdiicore.segin[i]);
    segout.push_back(cdiicore.segout[i]);
  }
  cdsicore = cdiicore.cdsicore;
  error = cdiicore.error;
}

CDIICore& CDIICore::operator= (const CDIICore& cdiicore) {
  //fpln(" $$$$$$$ CDIICore ASSIGN");
  //  tin_size  = cdiicore.tin_size;
  for (int i = 0; i < (int)cdiicore.tin.size(); i++) {
    tin.push_back(cdiicore.tin[i]);
    tout.push_back(cdiicore.tout[i]);
    tcpa.push_back(cdiicore.tcpa[i]);
    dist_tca.push_back(cdiicore.dist_tca[i]);
    segin.push_back(cdiicore.segin[i]);
    segout.push_back(cdiicore.segout[i]);
  }
  cdsicore = cdiicore.cdsicore;
  error = cdiicore.error;
  return *this;
}

CDIICore::~CDIICore() { }


//double CDIICore::getDistance() const {
//  return cdsi.getDistance();
//}
//
//double CDIICore::getHeight() const {
//  return cdsi.getHeight();
//}
//
//void CDIICore::setDistance(double distance) {
//  cdsi.setDistance(distance);
//}
//
//void CDIICore::setHeight(double height) {
//  cdsi.setHeight(height);
//}

int CDIICore::size() const {
  return (int)tin.size();
}

bool CDIICore::conflict() const {
  return (int)tin.size() > 0;
}

double CDIICore::getFilterTime() {
    return cdsicore.getFilterTime();
}

void CDIICore::setFilterTime(double cdfilter) {
    cdsicore.setFilterTime(cdfilter);
}

double CDIICore::getTimeIn(int i) const {
  if (i < 0 || i >= (int)tin.size()) return 0.0;
  return tin[i];
}

double CDIICore::getTimeOut(int i) const {
  if (i < 0 || i >= (int)tout.size()) return 0.0;
  return tout[i];
}

int CDIICore::getSegmentIn(int i) const {
  if (i < 0 || i >= (int)segin.size()) return 0;
  return segin[i];
}

int CDIICore::getSegmentOut(int i) const {
  if (i < 0 || i >= (int)segout.size()) return 0;
  return segout[i];
}

double CDIICore::getTimeClosest(int i) const {
  if (i < 0 || i >= (int)tcpa.size()) return 0.0;
  return tcpa[i];
}

double CDIICore::getDistanceClosest(int i) const {
  if (i < 0 || i >= (int)dist_tca.size()) return 0.0;
  return dist_tca[i];
}

bool CDIICore::conflictBetween(double start, double end) const {
  bool rtn = false;
  for (int k = 0; k < size(); k++) {
    double tmIn  = getTimeIn(k);
    double tmOut = getTimeOut(k);
    if (start <= tmIn && tmIn < end) {
      rtn = true;
      break;
    }
    if (start < tmOut && tmOut <= end) {
      rtn = true;
      break;
    }
    if (tmIn <= start && end <= tmOut) {
      rtn = true;
      break;
    }
  }
  return rtn;
}

bool CDIICore::cdiicore(const Plan& ownship, const Plan& traffic, Detection3D* cd, double B, double T) {
  CDIICore_def.setCoreDetectionPtr(cd);
  return CDIICore_def.detection(ownship, traffic, B, T);
}

bool CDIICore::detection(const Plan& ownship, const Plan& traffic, double B, double T) {
  //  tin_size = 0;
  tin.clear();
  tout.clear();
  tcpa.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();

  if (ownship.isLatLon() != traffic.isLatLon()) {
    return false;
  }

  if (ownship.isLatLon()) {
    return detectionLL(ownship, traffic, B, T);
  } else {
    return detectionXYZ(ownship, traffic, B, T);
  }
}

bool CDIICore::detectionExtended(const Plan& ownship, const Plan& traffic, double B, double T) {
  if (!detection(ownship, traffic, Util::max(ownship.getFirstTime(), traffic.getFirstTime()), Util::min(ownship.getLastTime(), traffic.getLastTime()))) {
    return false;
  }
  int i = 0;
  while (i < (int)tin.size()) {
    if (getTimeIn(i) > T || getTimeOut(i) < B) {
      //      tin_size--;
      if (i < (int)tin.size()-1) {
        tin.erase(tin.begin()+i);
        tout.erase(tout.begin()+i);
        segin.erase(segin.begin()+i);
        segout.erase(segout.begin()+i);
        tcpa.erase(tcpa.begin()+i);
        dist_tca.erase(dist_tca.begin()+i);
        //        tin[i] = tin[i+1];
        //        tout[i] = tout[i+1];
        //        segin[i] = segin[i+1];
        //        segout[i] = segout[i+1];
        //        tcpa[i] = tcpa[i+1];
        //        dist_tca[i] = dist_tca[i+1];
      }
    } else {
      i++;
    }
  }
  return size() > 0;
}


bool CDIICore::detectionXYZ(const Plan& ownship, const Plan& traffic, double B, double T) {
  bool cont = false;
  bool linear = true;

  //  double d = getDistance();
  //  double h = getHeight();

  tin.clear();
  tout.clear();
  tcpa.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();


  int start = B > ownship.getLastTime() ? ownship.size()-1 : Util::max(0,ownship.getSegment(B));
  // calculated end segment.  Only continue search if we are currently in a conflict
  int end = T > ownship.getLastTime() ? ownship.size()-1 : Util::max(0,ownship.size()-1);

  for (int i = start; i < ownship.size(); i++){
    if (i <= end || cont) {
      Vect3 so = ownship.point(i).point();
      Velocity vo = ownship.initialVelocity(i, linear);
      double t_base = ownship.getTime(i);
      // GEH : special case!!!
      if (t_base < B) {
          so = so.AddScal(B-t_base, vo);
          t_base = B;
      }
      double nextTime;
      if (i == ownship.size() - 1) {
        nextTime = 0.0;
      } else {
        nextTime = ownship.getTime(i + 1);
      }
      double HT = nextTime - t_base;
      double BT = Util::max(0.0,B - t_base); // Math.max(0,B-(t_base - t0));
      double NT = cont ? HT : T - t_base; // - (t_base - t0);

      if (NT < 0.0) {
        continue;
      }

      //      if (cdsi.allowVariableDistance()) {
      //        cdsi.setDistance(d+ownship.getSegmentDistanceBuffer(i));
      //        cdsi.setHeight(h+ownship.getSegmentHeightBuffer(i));
      //      }

      //System.out.println("$$CDIICore1 xyz: t_base = "+t_base+" HT = "+HT+" BT = "+BT+"  NT = "+NT);
      //System.out.println("$$CDIICore2 xyz: "+so+" "+vo+" "+t_base+" "+getTimeOut(tin.size()-1)+" "+cont);
      //Vect3 r = so.AddScal(225.4 - to, vo);
      //System.out.println("$$CDIICore3 xyz: "+r.cyl_norm(cdsi.getDistance(), cdsi.getHeight()));
      //r = so.AddScal(213.1 - to, vo);
      //System.out.println("$$CDIICore4 xyz: "+r.cyl_norm(cdsi.getDistance(), cdsi.getHeight()));
      //System.out.println("$$CDIICore5 xyz: "+cdsi.size());
      cdsicore.detectionXYZ(so, vo, t_base, HT, traffic, BT, NT);
      captureOutput(cdsicore,i);
      //      if ( ! captureOutput(cdsicore, i) ){
      //        return true;
      //      }
      cont = size() > 0.0 ? tout[(int)tout.size()-1] == HT + t_base : false;
    }
  }

  //  if (cdsi.allowVariableDistance()) {
  //    cdsi.setDistance(d);
  //    cdsi.setHeight(h);
  //  }


  merge();

  return conflict();
}




bool CDIICore::detectionLL(const Plan& ownship, const Plan& traffic, double B, double T) {
  bool cont = false;
  bool linear = true;

  //  double d = getDistance();
  //  double h = getHeight();

  tin.clear();
  tout.clear();
  tcpa.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();


  int start = B > ownship.getLastTime() ? ownship.size()-1 : Util::max(0,ownship.getSegment(B));
  // calculated end segment.  Only continue search if we are currently in a conflict
  int end = T > ownship.getLastTime() ? ownship.size()-1 : Util::max(0,ownship.size()-1);

  for (int i = start; i < ownship.size(); i++){
    if (i <= end || cont) {
      LatLonAlt llo = ownship.point(i).lla();
      //double lato = lla.latitudeInternal(); //Units.from(Units.deg, ownship.getLatitude(i));
      //double lono = lla.longitudeInternal(); //Units.from(Units.deg, ownship.getLongitude(i));
      //double alto = lla.altitudeInternal(); //Units.from(Units.ft, ownship.getAltitude(i));
      double t_base = ownship.getTime(i);
      // GEH : special case!!!
      if (t_base < B) {
          llo = ownship.position(B,linear).lla();
          t_base = B;
      }

      double nextTime;
      if (i == ownship.size() - 1) {
        nextTime = 0.0;
      } else {
        nextTime = ownship.getTime(i + 1);  // ownship.getTime(ownship.nextReal(i));  // stop on virtual pts?
      }
      double HT = nextTime - t_base;
      double BT = Util::max(0.0,B - t_base); // Math.max(0,B-(t_base - t0));
      double NT = cont ? HT : T - t_base; // - (t_base - t0);

      if (NT < 0.0) {
        continue;
      }
      Velocity vo = ownship.velocity(t_base, linear); //CHANGED!!!

      //      if (cdsi.allowVariableDistance()) {
      //        cdsi.setDistance(d+ownship.getSegmentDistanceBuffer(i));
      //        cdsi.setHeight(h+ownship.getSegmentHeightBuffer(i));
      //      }

      //System.out.println("$$CDIICore1 LL: BT = "+BT+"  NT = "+NT+" HT = "+HT);
      //System.out.println("$$CDIICore2 LL: "+llo+" "+vo+" "+t_base);
      cdsicore.detectionLL(llo, vo, t_base, HT, traffic, BT, NT);
      captureOutput(cdsicore,i);
      //      if ( ! captureOutput(cdsicore, i) ){
      //        return true;
      //      }
      cont = size() > 0 ? tout[(int)tout.size()-1] == HT + t_base : false;
    }
  }

  //  if (cdsi.allowVariableDistance()) {
  //    cdsi.setDistance(d);
  //    cdsi.setHeight(h);
  //  }

  merge();

  return conflict();
}


void CDIICore::captureOutput(const CDSICore& cdsi, int seg) {
  for (int index = 0; index < cdsi.size(); index++) {
    double vtin = cdsi.getTimeIn(index);
    double vtout = cdsi.getTimeOut(index);
    double vtcpa = cdsi.getTimeClosest(index);
    double vd = cdsi.getDistanceClosest(index);
    //double vdv = cdsi.getClosestVert(index);
    segin.push_back(seg);
    segout.push_back(seg);
    tin.push_back(vtin);
    tout.push_back(vtout);
    tcpa.push_back(vtcpa);
    dist_tca.push_back(vd);
    //dv[tin_size] = vdv;
    //    tin_size++;
    //    if (tin_size >= SIZE) {
    //      merge();
    //      if (tin_size >= SIZE) {
    //        error.addError("there are more conflicts than internal array size allows, increase SIZE constant");
    //        return false;
    //      }
    //    }
  }
  //
  //  return true;
}

void CDIICore::merge() {
  int i = 0;
  while ( i < (int)tin.size() - 1){
    if ( ! Util::almost_less(tout[i], tin[i+1], PRECISION7)) {
      tin.erase(tin.begin()+i+1);
      segin.erase(segin.begin()+i+1);
      tout.erase(tout.begin()+i);
      segout.erase(segout.begin()+i);
      if (dist_tca[i] < dist_tca[i+1]) {
        tcpa.erase(tcpa.begin()+i+1);
        dist_tca.erase(dist_tca.begin()+i+1);
      } else {
        tcpa.erase(tcpa.begin()+i);
        dist_tca.erase(dist_tca.begin()+i);
      }
      //      tin_size--;
    } else {
      i++;
    }
  }
}

bool CDIICore::violation(const Plan& ownship, const Plan& traffic, double tm) const {
  if (tm < ownship.getFirstTime() || tm > ownship.getLastTime()) {
    return false;
  }
  return cdsicore.violation(ownship.position(tm), ownship.velocity(tm), traffic, tm);
}


void CDIICore::setCoreDetectionPtr(const Detection3D* d) {
  cdsicore.setCoreDetectionPtr(d);
  tin.clear();
  tout.clear();
  tcpa.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();
  //  tin_size = 0;
}

Detection3D* CDIICore::getCoreDetectionPtr() const {
  return cdsicore.getCoreDetectionPtr();
}


void CDIICore::setCoreDetectionRef(const Detection3D& d) {
  cdsicore.setCoreDetectionRef(d);
  tin.clear();
  tout.clear();
  tcpa.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();
  //  tin_size = 0;
}

Detection3D& CDIICore::getCoreDetectionRef() const {
  return cdsicore.getCoreDetectionRef();
}


std::string CDIICore::toString() const {
  std::string str = "CDII: ";
  if (size() > 0) {
    str = str + " TimeIn(k)  Timeout(k) SegmentIn(k)  SegmentOut(k) \n";
  } else {
    str = str + "no conflicts\n";
  }
  for (int k = 0; k < size(); k++) {
    str = str + "      "+Fm4(getTimeIn(k))+"    "+Fm4(getTimeOut(k))
                        +"       "+Fmi(getSegmentIn(k))+"              "+Fmi(getSegmentOut(k))+" \n";
  }
  str = str + "CDSI = "+cdsicore.toString();
  return str;
}


}

