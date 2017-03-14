/*
 * CDSI Core - The core algorithms for conflict detection between an
 * ownship state vector and a traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDPolyIter.h"
#include "CDSIPolygon.h"
#include "DetectionPolygon.h"
#include "DetectionPolygonAcceptor.h"
#include "EuclideanProjection.h"
#include "PolyPath.h"
#include "Projection.h"
#include "Position.h"
#include "SimplePoly.h"
#include "Vect3.h"
#include "Velocity.h"
#include "LatLonAlt.h"
#include "Util.h"
#include "GreatCircle.h"
#include "Plan.h"
#include "ErrorLog.h"
#include "ErrorReporter.h"
#include "format.h"
#include <cfloat>

namespace larcfm {

/** 
 * This class implements the core algorithms for conflict detection
 * between an ownship (modeled with a state vector) and a traffic
 * aircraft trajectory (modeled with a intent).  This class assumes
 * all inputs are in internal units.  As such, this class is most
 * appropriate to be part of a larger system, not as a stand-alone
 * conflict detection algorithm.  For that functionality, see the class
 * CDSI.<p>
 *
 * This class can be used two ways: through an object or through a
 * static method.  The static method approach has the advantage that
 * an object does not need to be created.  The object approach has the
 * advantage that more information is available.  Specifically, the
 * start and end times for every conflict are available in the object
 * approach. <p>
 *
 * In addition, for each of these types of use, the aircraft can be
 * specified in two different coordinate systems: a Euclidean space
 * and a latitude and longitude reference frame. <p>
 */
  CDSIPolygon CDSIPolygon::def = CDSIPolygon(new CDPolyIter());

  CDSIPolygon::CDSIPolygon(DetectionPolygon* d) : error("CDSIPolygon") {
    cdss = d->copy();
//    tin = new ArrayList<Double>(10);
//    tout = new ArrayList<Double>(10);
//    tca = new ArrayList<Double>(10);
//    dist_tca = new ArrayList<Double>(10);
  }

  CDSIPolygon::CDSIPolygon() : error("CDSIPolygon") {
    cdss = new CDPolyIter();
  }


  /** Returns the number of conflicts */
  int CDSIPolygon::size() const {
    return tin.size();
  }

  /** Returns if there were any conflicts */
  bool CDSIPolygon::conflict() const {
    return tin.size() > 0;
  }

  /**
   * Returns the start time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDSIPolygon::getTimeIn(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return tin[i];
  }

  /**
   * Returns the end time of the conflict.  This value is in absolute time.
   * If there was not a conflict, then this value is meaningless.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDSIPolygon::getTimeOut(int i) const {
    if (i < 0 || i >= (int) tout.size()) return 0.0;
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
  double CDSIPolygon::getTimeClosest(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return tca[i];
  }

  /**
   * Returns the cylindrical distance at the time of closest approach.
   * @param i the i-th conflict, must be between 0..size()-1
   */
  double CDSIPolygon::getDistanceClosest(int i) const {
    if (i < 0 || i >= (int) tin.size()) return 0.0;
    return dist_tca[i];
  }


  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan. <p>
   *
   * @param so the Position of the state aircraft
   * @param vo the velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
   * @param state_horizon the largest time where the state aircraft's position can be predicted.
   * @param intent the flight plan of the intent aircraft
   * @param D the minimum horizontal separation distance
   * @param H the minimum vertical separation distance
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool CDSIPolygon::cdsicore(const Position& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T) {
     return def.detection(so, vo, t0, state_horizon, intent, B, T);
  }

  /**
   * Return true if the given position and velocity are in violation with the intent aircraft at time tm.
   * If tm is outisde the intent information, return false;
   * @param so
   * @param vo
   * @param intent
   * @param tm
   * @return
   */
  bool CDSIPolygon::violation(const Position& so, const Velocity& vo, const PolyPath& intent, double tm) const {
    if (tm < intent.getFirstTime() || tm > intent.getLastTime()) {
      return false;
    }

    EuclideanProjection proj = Projection::createProjection(so);
    if (so.isLatLon()) {
      Vect3 so3 = proj.project(so);
      const Velocity& vo3 = proj.projectVelocity(so, vo);
      return cdss->violation(so3, vo3, intent.position(tm).poly3D(proj));
    } else {
      return cdss->violation(so.point(), vo, intent.position(tm).poly3D(proj));
    }
  }

  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan. <p>
   *
   * @param so the position of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at so with respect to the traffic intent.  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool CDSIPolygon::detection(const Position& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T) {
    if (so.isLatLon()) {
      return detectionLL(so.lla(), vo, t0, state_horizon, intent, B, T);
    } else {
      return detectionXYZ(so.point(), vo, t0, state_horizon, intent, B, T);
    }
  }


  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a Euclidean reference
   * frame at the time t0. <p>
   *
   * @param so the position of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (so.x,so.y,so.z).  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool CDSIPolygon::detectionXYZ(Vect3 so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T) {
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();

    EuclideanProjection proj = Projection::createProjection(0,0,0);

    double t_base;
    int start_seg;
    bool cont = false;
    double BT = 0.0;
    double NT = 0.0;
    double HT = 0.0;
    if (t0 < intent.getTime(0)) {
      t_base = intent.getTime(0);
      start_seg = 0;
    } else {
      t_base = t0;
      start_seg = intent.getSegment(t0);
      if (start_seg < 0) {
        return false;  // t0 past end of Flight Plan
      }
    }
    for (int j = start_seg; j < intent.size(); j++){
      Vect3 sop = so.AddScal((t_base - t0),vo);
      if (j == intent.size() - 1 && !intent.isContinuing()) {
    	  continue;
      } else { // normal case
        double tend = DBL_MAX;
        if (j < intent.size()-1) {
            tend = intent.getTime(j+1) - t_base;
        }
        HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), tend));
        BT = Util::max(0.0, B + t0 - t_base);
        NT = cont ? HT : T + t0 - t_base;
//fpln(" $$>> CDSIPolygon.detectionXYZ: bleh");
        if (NT >= 0) {
          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, proj);
//         fpln(" $$>> CDSIPolygon.detectionXYZ: mpg = "+mpg.toString()+" BT = "+Fm2(BT)+" NT = "+Fm2(NT)+" HT = "+Fm2(HT));
          bool cdssRtn = cdss->conflictDetection(sop, vo, mpg, BT, Util::min(NT, HT));
//fpln(" $$>> CDSIPolygon.detectionXYZ: j = "+Fmi(j)+" cdssRtn = "+Fmb(cdssRtn));
          if ( cdssRtn ) {
//for (int i = 0; i < intent.getPoly(0).size(); i++) fpln("ui_reference_point_"+Util::nextCount()+" = "+intent.getPoly(0).getPoint(i).toString8());
//for (int i = 0; i < mpg.horizpoly.size(); i++) fpln("ui_reference_point_"+Util::nextCount()+" = "+Position(Vect3(mpg.horizpoly.polystart.get(i),0)).toString8());

            for(int i = 0; i < (int) cdss->getTimesOut().size(); i++) {
              if (std::abs((t0+B) - (cdss->getTimesOut()[i]+t_base)) > 0.0000001) {
//fpln("$$$$>> CDSIPolygon.detectionXYZ: in0="+Fm4(cdss->getTimesIn()[0]+t0)+" out0="+Fm4(cdss->getTimesOut()[0]+t0));
                captureOutput(t_base, i, cdss);
              }
            }
          }
        }
        cont = size() > 0 ? tout[tout.size()-1] == HT + t_base : false;
        t_base = intent.getTime(j+1);
      }
    }
    merge();
    return conflict();
  }



  /**
   * Returns if there is a conflict between two aircraft: the state
   * aircraft and the intent aircraft.  The state aircraft is
   * assumed to move linearly from it position.  The intent aircraft
   * is assumed to move according to the given flight plan.  Both
   * aircraft are assumed to be represented in a latitude/longitude reference
   * frame. <p>
   *
   * @param so the latitude/longitude/altitude of the state aircraft
   * @param vo velocity of the state aircraft
   * @param t0 the time of the state aircraft when located at (lat, lon, alt).  This can be 0.0 to represent "now"
   * @param state_horizon the maximum amount of time after t0 that the position of the state aircraft's position can be predicted
   * @param intent the flight plan of the intent aircraft
   * @param B the time to start looking for conflicts relative to t0. This can be 0.0.
   * @param T the time to end looking for conflicts relative to t0
   * @return true if there is a conflict
   */
  bool CDSIPolygon::detectionLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon, const PolyPath& intent, double B, double T) {
//fpln("======== $$$$$$ CDSIPolygon.detectionLL so="+so.toString()+" vo="+vo.toString()+" to="+Fm1(t0)+" state_horizon="+Fm1(state_horizon)+" intent="+intent.toString()+" B (relto t0) = "+Fm1(B)+" T (rel to t0) = "+Fm1(T));
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();
    double t_base;
    int start_seg;
    bool cont = false;
    double BT = 0.0;
    double NT = 0.0;
    double HT = 0.0;
    if (t0 < intent.getTime(0)) {
      t_base = intent.getTime(0);
      start_seg = 0;
    } else {
      t_base = t0;
      start_seg = intent.getSegment(t0);
//fpln(" $$ CDSIPolygon.detectionLL start_seg="+Fmi(start_seg)+" intent start="+Fm1(intent.getFirstTime())+" intent end="+Fm1(intent.getLastTime()));
      if (start_seg < 0) {
//fpln("CDSIPolygon: start_seg <0!");
        return false;  // t0 past end of Flight Plan
      }
    }
    //fpln(" $$ CDSIPolygon.detectionLL start_seg="+start_seg+" t_base = "+t_base+" intent ="+intent);
    for (int j = start_seg; j < intent.size(); j++){
      LatLonAlt so2p = GreatCircle::linear_initial(so, vo, t_base-t0);  //CHANGED!!!
      //SimplePoly sip = intent.position(t_base);
      EuclideanProjection proj = Projection::createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
      Vect3 so3 = proj.project(so2p);
//      fpln(" $$>> ^^^^^^^^^^^^^^^^ CDSIPolygon.detectionLL: so3 = "+so3);
      //fpln(" $$>> CDSIPolygon.detectionLL so2p = "+so2p+" proj = "+proj+" sip = "+sip);
      if (j == intent.size() - 1 && !intent.isContinuing()) {
//fpln("CDSIPolygon: j == intent.size() - 1!");
          continue; // leave loop
      } else { // normal case
        double tend = DBL_MAX;
        if (j < intent.size()-1) {
            tend = intent.getTime(j+1) - t_base;
        }
        HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), tend));
        BT = Util::max(0.0, B + t0 - t_base);
        NT = cont ? HT : T + t0 - t_base;
        Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
        //Velocity vip = proj.projectVelocity(sip.centroid(), intent.velocity(t_base));
//fpln(" $$ CDSIPolygon.detectionLL BT="+Fm1(BT)+" HT="+Fm1(HT)+" NT="+Fm1(NT)+" cont="+Fmb(cont));
        if (NT >= 0) {
          MovingPolygon3D mpg = intent.getMovingPolygon(t_base, proj);
//fpln(" $$>> CDSIPolygon.detectionLL: mpg = "+mpg.toString()+" BT = "+Fm1(BT)+" NT = "+Fm1(NT)+" HT = "+Fm1(HT));
          bool cdssRtn = cdss->conflictDetection(so3, vop, mpg, BT, Util::min(NT, HT));
//          fpln(" $$>> CDSIPolygon.detectionLL: j = "+j+" cdssRtn = "+cdssRtn);
          if (cdssRtn ) {  // CHANGED!!!
            // filter out when the start time for searching is the the end time of the conflict
            for (int i = 0; i < (int) cdss->getTimesIn().size(); i++) {
              if (std::abs((t0+B) - (cdss->getTimesOut()[i]+t_base)) > 0.0000001) {
                captureOutput(t_base, i, cdss);
//fpln("CDSIPoly: i="+i+" cdss so3="+so3+" vop="+vop+" mpg= ---"+" B="+BT+" T="+(Util::min(NT,HT)));
//fpln("CDSIPoly: i="+i+" tin="+(cdss.getTimesIn().get(i))+" tout="+(cdss.getTimesOut().get(i))+" end="+(T)+" horiz="+(state_horizon));
              }
            }
          }
        }
        cont = size() > 0 ? tout[tout.size()-1] == HT + t_base : false;
        t_base = intent.getTime(j+1);
      }
    }
    merge();
    bool rtn = conflict();
//    fpln(" $$>> CDSIPolygon.detectionLL: rtn = "+rtn);
    return rtn;
  }


  void CDSIPolygon::captureOutput(double t_base, int conf, DetectionPolygon* cd) {
    tin.push_back(cd->getTimesIn()[conf]+t_base);
    tout.push_back(cd->getTimesOut()[conf]+t_base);
    tca.push_back(cd->getCriticalTimesOfConflict()[conf] + t_base);
    dist_tca.push_back(cd->getDistancesAtCriticalTimes()[conf]);
  }


  void CDSIPolygon::merge() {
    int i = 0;
    while ( i < (int) tin.size() - 1){
      if ( ! Util::almost_less(tout[i], tin[i+1], PRECISION7)) {

        tin.erase(tin.begin()+i+1);
        tout.erase(tout.begin()+i);
        if (dist_tca[i] < dist_tca[i+1]) {
          tca.erase(tca.begin()+i+1);
          dist_tca.erase(dist_tca.begin()+i+1);
          //dv.remove(i+1);
        } else {
          tca.erase(tca.begin()+i);
          dist_tca.erase(dist_tca.begin()+i);
          //dv.remove(i);
        }
      } else {
        i++;
      }
    }
  }

  std::string CDSIPolygon::toString() const {
       return "CDSIPolygon: cd = "+cdss->toString();
  }

  // ErrorReporter Interface Methods

  bool CDSIPolygon::hasError() const {
    return error.hasError();
  }
  bool CDSIPolygon::hasMessage() const {
    return error.hasMessage();
  }
  std::string CDSIPolygon::getMessage() {
    return error.getMessage();
  }
  std::string CDSIPolygon::getMessageNoClear() const {
    return error.getMessageNoClear();
  }

  void CDSIPolygon::setCorePolygonDetectionPtr(const DetectionPolygon* d) {
    cdss = d->copy();
    tin.clear();
    tout.clear();
    tca.clear();
    dist_tca.clear();
  }

  void CDSIPolygon::setCorePolygonDetectionRef(const DetectionPolygon& d) {
    setCorePolygonDetectionPtr(&d);
  }


  DetectionPolygon* CDSIPolygon::getCorePolygonDetectionPtr() const {
    return cdss;
  }

  DetectionPolygon& CDSIPolygon::getCorePolygonDetectionRef() const {
    return *cdss;
  }

}
