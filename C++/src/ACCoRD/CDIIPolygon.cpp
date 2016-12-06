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


#include "ErrorReporter.h"
#include "PolyPath.h"
#include "Vect3.h"
#include "Util.h"
#include "Velocity.h"
#include "Plan.h"
#include "LatLonAlt.h"
#include "CDSIPolygon.h"
#include "CDIIPolygon.h"
#include "CDPolyIter.h"
#include "DetectionPolygon.h"
#include "DetectionPolygonAcceptor.h"
#include "format.h"

namespace larcfm {


  CDIIPolygon CDIIPolygon::def = CDIIPolygon(new CDPolyIter());


  CDIIPolygon::CDIIPolygon(DetectionPolygon* cd) {
    cdsi = CDSIPolygon(cd);
  }

  CDIIPolygon::CDIIPolygon() {
    cdsi = CDSIPolygon(new CDPolyIter());
  }



  /** Returns the number of conflicts */
  int CDIIPolygon::size() const {
    return tin.size();
  }

  /** Returns if there were any conflicts.  */
  bool CDIIPolygon::conflict() const {
    return tin.size() > 0;
  }

  /** 
   * Returns the start time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDIIPolygon::getTimeIn(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return tin[i];
  }

  /** 
   * Returns the end time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDIIPolygon::getTimeOut(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return tout[i];
  }


  /** 
   * Returns an estimate of the time of closest approach.  This value is in absolute time 
   * (not relative from a waypoint).  This point approximates the point where the two aircraft
   * are closest.  The definition of closest is not simple.  Specifically, space in the vertical
   * dimension counts more than space in the horizontal dimension: encroaching in the protected
   * zone 100 vertically is much more serious than encroaching 100 ft. horizontally. 
   * 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDIIPolygon::getTimeClosest(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return tcpa[i];
  }

  /** 
   * Returns the distance index at the time of closest approach. 
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDIIPolygon::getDistanceClosest(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return dist_tca[i];
  }


  /**
   * Is there a conflict at any time in the interval from start to
   * end (inclusive). This method assumes that the detection()
   * method has been called first.
   *
   * @param start the time to begin looking for conflicts
   * @param end the time to end looking for conflicts
   * @return true if there is a conflict
   */
  bool CDIIPolygon::conflictBetween(double start, double end) const {
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

  /**
   * Returns if there is a conflict between two aircraft: the ownship and
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the absolute time to start looking for conflicts
   * @param T the absolute time to end looking for conflicts
   * @return true if there is a conflict
   */
  bool CDIIPolygon::cdiicore(const Plan& ownship, const PolyPath& traffic, double B, double T) {
    return def.detection(ownship, traffic, B, T);
  }

  /**
   * Return true if there is a violation between two aircraft at time tm.
   * If tm is outside either of the plans' times, this will return false.
   * @param ownship
   * @param traffic
   * @param tm
   * @return
   */
  bool CDIIPolygon::violation(const Plan& ownship, const PolyPath& traffic, double tm) const {
    if (tm < ownship.getFirstTime() || tm > ownship.getLastTime()) {
      return false;
    }
    return cdsi.violation(ownship.position(tm), ownship.velocity(tm), traffic, tm);
  }

  
  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts (no time information will be returned prior to the start of the ownship segment containing B)
   * @param T the absolute time to end looking for conflicts
   * @return true if there is a conflict
   */
  bool CDIIPolygon::detection(const Plan& ownship, const PolyPath& traffic, double B, double T) {
    tin.clear();
    tout.clear();
    tcpa.clear();
    dist_tca.clear();
    if (ownship.isLatLon() != traffic.isLatLon()) {
      return false;
    }
    if (ownship.isLatLon()) {
      return detectionLL(ownship, traffic, B, T);
    } else {
      return detectionXYZ(ownship, traffic, B, T);
    }
  }

  /**
   * This version calculates the "true" time in and time out for all conflicts that overlap with [B,T].
   * It is less efficient than the normal detection() algorithm and should only be called if accurate time in information is necessary 
   * when B might be within a loss of separation region.
   */
  bool CDIIPolygon::detectionExtended(const Plan& ownship, const PolyPath& traffic, double B, double T) {
    if (!detection(ownship, traffic, std::max(ownship.getFirstTime(), traffic.getFirstTime()), std::min(ownship.getLastTime(), traffic.getLastTime()))) {
      return false;
    }
    int i = 0;
    while (i < size()) {
      if (getTimeIn(i) > T || getTimeOut(i) < B) {
        tin.erase(tin.begin()+i);
        tout.erase(tout.begin()+i);
        tcpa.erase(tcpa.begin()+i);
        dist_tca.erase(dist_tca.begin()+i);
      } else {
        i++;
      }
    }
    return size() > 0;
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   * 
   * This will not return any timing information prior to the owhship segment containing B.
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts.
   * @param T the absolute time to end looking for conflicts.
   * @return true if there is a conflict
   */
  bool CDIIPolygon::detectionXYZ(const Plan& ownship, const PolyPath& traffic, double B, double T) {
    bool cont = false;                     // what is this?
    int start = B > ownship.getLastTime() ? ownship.size()-1 : std::max(0,ownship.getSegment(B));
    // calculated end segment.  Only continue search if we are currently in a conflict
    int end = T > ownship.getLastTime() ? ownship.size()-1 : std::max(0,ownship.size()-1);
    for (int i = start; i < ownship.size(); i++){
      // truncate the search at Tend if not in a conflict
      if (i <= end || cont) {
        Vect3 so = ownship.point(i).point();
        bool linear = true;
        Velocity vo = ownship.initialVelocity(i, linear);
        double t_base = ownship.getTime(i);   // ownship start of leg
        double nextTime;                      // ownship end of range
        if (i == ownship.size() - 1) {
          nextTime = 0.0;                   // set to 0
        } else {
          nextTime = ownship.getTime(i + 1);  // if in the plan, set to the end of leg
        }
        double HT = nextTime - t_base;        
        double BT = std::max(0.0,B - t_base); // std::max(0,B-(t_base - t0));
        double NT = cont ? HT : T - t_base; // - (t_base - t0); 

        if (NT < 0.0) {
          continue;
        }
//fpln(" $$>> CDIIPolygon: detectionXYZ: call cdsi.detectionXYZ! for i = "+Fmi(i));
        cdsi.detectionXYZ(so, vo, t_base, HT, traffic, BT, NT);
        captureOutput(cdsi, i);
        cont = size() > 0 ? tout[tout.size()-1] == HT + t_base : false;

      }
    }
    merge();
    return conflict();
  }

  /**
   * Returns if there is a conflict between two aircraft: the ownship and 
   * the traffic aircraft.  <p>
   * 
   * This will not return any timing information prior to the ownship segment containing B.
   *
   * @param ownship the trajectory intent of the ownship
   * @param traffic the trajectory intent of the traffic
   * @param B the absolute time to start looking for conflicts.
   * @param T the absolute time to end looking for conflicts.
   * @return true if there is a conflict
   */
  bool CDIIPolygon::detectionLL(const Plan& ownship, const PolyPath& traffic, double B, double T) {
    bool cont = false;
//fpln(" $$ CDIIPolygon.detectionLL ownship="+ownship.toString()+" traffic="+traffic.toString()+" B = "+Fm1(B)+" T= "+Fm1(T));
    int start = B > ownship.getLastTime() ? ownship.size()-1 : std::max(0,ownship.getSegment(B));
    // calculated end segment.  Only continue search if we are currently in a conflict
    int end = T > ownship.getLastTime() ? ownship.size()-1 : std::max(0,ownship.size()-1);
    for (int i = start; i < ownship.size(); i++){ 
      // truncate the search at Tend if not in a conflict
      if (i <= end || cont) {
        LatLonAlt llo = ownship.point(i).lla();
        double t_base = ownship.getTime(i);
        double nextTime;
        if (i == ownship.size() - 1) {
          nextTime = 0.0;
        } else {
          nextTime = ownship.getTime(i + 1);  
        }
        double HT = nextTime - t_base;      // state-based time horizon (relative time)
        double BT = std::max(0.0,B - t_base); // begin time to look for conflicts (relative time)
        double NT = cont ? HT : T - t_base; // end time to look for conflicts (relative time)
        if (NT < 0.0) {
          continue;
        }
        bool linear = true;
        Velocity vo = ownship.velocity(t_base, linear);           // CHANGED!!!
//std::cout << "$$CDIIPolygon times: i="<<i<<" BT = "<<BT<<"  NT = "<<NT<<" HT = "<<HT<<" B = "<<B<<std::endl;
        //        System.out.println("$$CDIICore LL: "+llo+" "+vo+" "+t_base);       
        cdsi.detectionLL(llo, vo, t_base, HT, traffic, BT, NT);
//        fpln(" $$## CDIIPolygon.detectionLL: cdsi = "+cdsi);
        captureOutput(cdsi, i);
        cont = size() > 0 ? tout[tout.size()-1] == HT + t_base : false;
      }
    }

    merge();

    return conflict();
  }

  void CDIIPolygon::captureOutput(CDSIPolygon cdsi, int seg) {
    for (int index = 0; index < cdsi.size(); index++) {
      double vtin = cdsi.getTimeIn(index);
      double vtout = cdsi.getTimeOut(index);
      double vtcpa = cdsi.getTimeClosest(index);
      double vd = cdsi.getDistanceClosest(index);
      tin.push_back(vtin);
      tout.push_back(vtout);
      tcpa.push_back(vtcpa);
      dist_tca.push_back(vd);
    }
  }

  void CDIIPolygon::merge() {
    int i = 0;
    while ( i < (int) tin.size() - 1){
      if ( ! Util::almost_less(tout[i], tin[i+1], PRECISION7)) {
        tin.erase(tin.begin()+i+1);
        tout.erase(tout.begin()+i);
        if (dist_tca[i] < dist_tca[i+1]) {
          tcpa.erase(tcpa.begin()+i+1);
          dist_tca.erase(dist_tca.begin()+i+1);
        } else {
          tcpa.erase(tcpa.begin()+i);
          dist_tca.erase(dist_tca.begin()+i);
        }
      } else {
        i++;
      }
    }
  }

  std::string CDIIPolygon::toString() const {
    std::string str = "CDIIPolygon: size() = "+Fmi(size());
    if (size() > 0) {
      str = str + "\n TimeIn(k)  Timeout(k) \n";
    } else {
      str = str + " (no conflicts)\n";
    }
    for (int k = 0; k < size(); k++) {
      str = str + "      "+Fm4(getTimeIn(k))+"    "+Fm4(getTimeOut(k))+"\n";
    }
    str = str + "CDSIPolygon = "+cdsi.toString();
    return str;  
  }

  // ErrorReporter Interface Methods

  bool CDIIPolygon::hasError() const {
    return cdsi.hasError();
  }
  bool CDIIPolygon::hasMessage() const {
    return cdsi.hasMessage();
  }
  std::string CDIIPolygon::getMessage() {
    return cdsi.getMessage();
  }
  std::string CDIIPolygon::getMessageNoClear() const {
    return cdsi.getMessageNoClear();
  }

  void CDIIPolygon::setCorePolygonDetectionPtr(const DetectionPolygon* d) {
    cdsi.setCorePolygonDetectionPtr(d);
    tin.clear();
    tout.clear();
    tcpa.clear();
    dist_tca.clear();
  }

  void CDIIPolygon::setCorePolygonDetectionRef(const DetectionPolygon& d) {
    cdsi.setCorePolygonDetectionRef(d);
    tin.clear();
    tout.clear();
    tcpa.clear();
    dist_tca.clear();
  }

  DetectionPolygon* CDIIPolygon::getCorePolygonDetectionPtr() const {
    return cdsi.getCorePolygonDetectionPtr();
  }

  DetectionPolygon& CDIIPolygon::getCorePolygonDetectionRef() const {
    return cdsi.getCorePolygonDetectionRef();
  }

}

