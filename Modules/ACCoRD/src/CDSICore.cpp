/*
 * CDSI Core - The core algorithms for conflict detection between an
 * ownship state vector and a traffic aircraft with a flight plan.
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov), Rick Butler
 *
 * Copyright (c) 2011-2019 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDSICore.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "Util.h"
#include "Horizontal.h"
#include "format.h"
#include <vector>



namespace larcfm {

CDSICore CDSICore::CDSICore_def = CDSICore();


//  CDSICore::CDSICore(double distance, double height) : cdss(1.0, 1.0, 0.0),
//						       error("CDSICore") {
//
////    setDistance(distance);
////    setHeight(height);
//    tin_size = 0;
//    //tin = new ArrayList<Double>(10);
//    //tout = new ArrayList<Double>(10);
//    //segin = new ArrayList<Integer>(10);
//    //segout = new ArrayList<Integer>(10);
//
//    //cd = new CDSSCore();
//    //error = new ErrorLog("CDSICore");
//  }

CDSICore::CDSICore() : error("CDSICore") {
  //    tin_size = 0;
}

CDSICore::CDSICore(Detection3D* cd) : cdsscore(cd, 0.0), error("CDSICore") {
  //    tin_size = 0;
}

CDSICore::CDSICore(const CDSICore& cdsicore) : error("CDSICore") {
  //    tin_size  = cdsicore.tin_size;
  for (int i = 0; i < (int)cdsicore.tin.size(); i++) {
    tin.push_back(cdsicore.tin[i]);
    tout.push_back(cdsicore.tout[i]);
    tca.push_back(cdsicore.tca[i]);
    dist_tca.push_back(cdsicore.dist_tca[i]);
    segin.push_back(cdsicore.segin[i]);
    segout.push_back(cdsicore.segout[i]);
  }
  cdsscore = CDSSCore(cdsicore.cdsscore);
  error = ErrorLog(cdsicore.error);
}

CDSICore::~CDSICore() {}

CDSICore& CDSICore::operator= (const CDSICore& cdsicore) {
  //fpln(" $$$$$$$ CDSICore ASSIGN");
  //    tin_size  = cdsicore.tin_size;
  for (int i = 0; i < (int)cdsicore.tin.size(); i++) {
    tin.push_back(cdsicore.tin[i]);
    tout.push_back(cdsicore.tout[i]);
    tca.push_back(cdsicore.tca[i]);
    dist_tca.push_back(cdsicore.dist_tca[i]);
    segin.push_back(cdsicore.segin[i]);
    segout.push_back(cdsicore.segout[i]);
  }
  cdsscore = CDSSCore(cdsicore.cdsscore);
  error = ErrorLog(cdsicore.error);
  return *this;
}


//  double CDSICore::getDistance() const {
//    return cdss.getDistance();
//  }
//
//  double CDSICore::getHeight() const {
//    return cdss.getHeight();
//  }

double CDSICore::getFilterTime() const {
  return cdsscore.getFilterTime();
}

//  void CDSICore::setDistance(double distance) {
//    cdss.setDistance(Util::max(distance, 1e-6));
//  }
//
//  void CDSICore::setHeight(double height) {
//    cdss.setHeight(Util::max(height, 1e-6));
//  }

void CDSICore::setFilterTime(double cdfilter) {
  cdsscore.setFilterTime(cdfilter);
}

int CDSICore::size() const {
  return (int)tin.size();
}

bool CDSICore::conflict() const {
  return (int)tin.size() > 0;
}

double CDSICore::getTimeIn(int i) const {
  if (i < 0 || i >= (int)tin.size()) return 0.0;
  return tin[i];
}

double CDSICore::getTimeOut(int i) const {
  if (i < 0 || i >= (int)tout.size()) return 0.0;
  return tout[i];
}

int CDSICore::getSegmentIn(int i) const {
  if (i < 0 || i >= (int)segin.size()) return 0;
  return segin[i];
}

int CDSICore::getSegmentOut(int i) const {
  if (i < 0 || i >= (int)segout.size()) return 0;
  return segout[i];
}

double CDSICore::getCriticalTime(int i) const {
  if (i < 0 || i >= (int)tca.size()) return 0.0;
  return tca[i];
}

double CDSICore::getDistanceAtCriticalTime(int i) const {
  if (i < 0 || i >= (int)dist_tca.size()) return 0.0;
  return dist_tca[i];
}


//   // returns the time from beginning of segment to the end of the time range of interest
//   // the (relative) segment lookahead end time
//   // a negative result indicates the segment starts after the lookahead time
//   double CDSICore::seg_lh_top(const Plan& flp, double t0, int j, double B, double T) const {
//     if (j == flp.size()-1) {
//       if (flp.getExtend()) {
//         return t0 + T;
//       } else {
//         return -1.0; // this will ensure that seg_lh_top < seg_lh_bot
//       }
//     } else {
//       return Util::min(flp.getTime(j+1),t0+T)-flp.getTime(j);
//     }
//   }

//   // returns the time from beginning of segment to the start of the time range of interest
//   // the (relative) segment lookahead start time
//   // a time > flp(j+1)`time indicates the segment ends before the lookahead range starts
//   double CDSICore::seg_lh_bottom(const Plan& flp, double t0, int j, double B, double T) const {
//     return Util::max(flp.getTime(j),t0+B)-flp.getTime(j);
//   }


bool CDSICore::cdsicore_xyz(const Vect3& so, const Velocity& vo, Detection3D* cd,double t0, double state_horizon, const Plan& intent,
    double B, double T) {
  CDSICore_def.setCoreDetectionPtr(cd);
  return CDSICore_def.detectionXYZ(so, vo, t0, state_horizon, intent, B, T);
}

bool CDSICore::cdsicore_ll(const LatLonAlt& state, Velocity vo, Detection3D* cd, double t0, double state_horizon,
    const Plan& intent, double B, double T) {
  CDSICore_def.setCoreDetectionPtr(cd);
  return CDSICore_def.detectionLL(state, vo, t0, state_horizon, intent, B, T);
}


bool CDSICore::cdsicore(const Position& so, const Velocity& vo, Detection3D* cd, double t0, double state_horizon,
    const Plan& intent, double B, double T) {
  CDSICore_def.setCoreDetectionPtr(cd);
  return CDSICore_def.detection(so, vo, t0, state_horizon, intent, B, T);
}


bool CDSICore::detection(const Position& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent, double B, double T) {
  if (so.isLatLon()) {
    return detectionLL(so.lla(), vo, t0, state_horizon, intent, B, T);
  } else {
    return detectionXYZ(so.vect3(), vo, t0, state_horizon, intent, B, T);
  }
}

bool CDSICore::detectionXYZ(const Vect3& so, const Velocity& vo, double t0, double state_horizon,
    const Plan& intent, double B, double T) {
  //    tin_size = 0;
  tin.clear();
  tout.clear();
  tca.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();

  bool linear = true;

  double t_base;
  int start_seg;
  bool cont = false;
  double BT = 0.0;
  double NT = 0.0;
  double HT = 0.0;

  if (t0 < intent.time(0)) {
    t_base = intent.time(0);
    start_seg = 0;
  } else {
    t_base = t0;
    start_seg = intent.getSegment(t0);
    if (start_seg < 0) {
      return false;  // t0 past end of Plan
    }
  }

  //    double d = getDistance();
  //    double h = getHeight();

  //std::cout << "$$CDSICoreXYZ1 intent size: " << intent.size() << " t_base: " << t_base << std::endl;
  for (int j = start_seg; j < intent.size(); j++){
    // invariant: t0 <= t_base

    double dt = t_base - intent.time(j);
    if (checkSmallTimes && dt > 0.0 && dt < 0.000001) {
      error.addWarning("Attempting detectionXYZ on segment "+Fm0(j)+" of "+intent.getID()+" with very small offset: "+Fm12(dt));
    }


    Vect3 sop = so.AddScal((t_base - t0),vo);
    Vect3 sip = intent.position(t_base,linear).vect3(); // intent.positionXYZ(j).AddScal((t_base - intent.getTime(j)),vi);
    //Vect3 s = sop.Sub(sip);

    //      if (allowVariableDistanceBuffer) {
    //        cdss.setDistance(d+intent.getSegmentDistanceBuffer(j));
    //        cdss.setHeight(h+intent.getSegmentHeightBuffer(j));
    //      }

    if (j == intent.size() - 1) { // extend
      continue; // leave loop
    } else { // normal case
      HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
      BT = Util::max(0.0, B + t0 - t_base);
      NT = cont ? HT : T + t0 - t_base;

      //std::cout << "$$CDSICoreXYZ2 positions: "<<sop.toString()<<" "<<sip.toString()<< std::endl;
      //std::cout << "$$CDSICoreXYZ3 rel pos, vo, vi: "<<s.toString()<<" "<<vo.toString() << " " << intent.velocity(j).toString() << std::endl;
      //Vect3 sp = s.AddScal(1.0,vo.Sub(intent.velocity(j)));
      //Vect3 spp = sp.AddScal(1.0, vo.Sub(intent.velocity(j+1)));
      //std::cout << "$$CDSICoreXYZ3B distance: "<<sp.vect2().norm()<<" "<<spp.vect2().norm());
      //std::cout << "$$CDSICoreXYZ4 times: j="<<j<<" t_base="<<t_base<<" BT="<<BT<<" NT="<<NT<<" HT="<<HT<<" "<<cont << std::endl;
      //std::cout << "$$CDSICoreXYZ5 times: D="<<cd.getDistance()<<" H="<<cd.getHeight()<< std::endl;
      //        std::cout << "CDSICore.detectionXYZ: vo=" << vo.toString() << " vi=" << intent.initialVelocity(j).toString() << std::endl;
      if (NT >= 0.0) {
        //cd.setTimeHorizon(HT);

        double dt = NT-BT;
        if (checkSmallTimes && dt > 0) {
          if (dt < cdsscore.getFilterTime()) {
            error.addWarning("Attempting detectionXYZ on segment "+Fm0(j)+" of "+intent.getID()+" with duration smaller than the filter time");
          } else if (dt < 0.000001) {
            error.addWarning("Attempting detectionXYZ on segment "+Fm0(j)+" at time "+Fm12(BT+t_base)+" of "+intent.getID()+" with very small duration: "+Fm12(dt));
          }
        }


        if ( cdsscore.detectionBetween(sop, vo, sip, intent.initialVelocity(j, linear), BT, NT, HT) ) {
          if (std::abs((t0+B) - (cdsscore.getTimeOut()+t_base)) > 0.0000001) {
            captureOutput(t_base, j);
          }
        }
      }
      cont = size() > 0 ? tout[(int)tout.size()-1] == HT + t_base : false;
      t_base = intent.time(j+1);
    }
  }

  //    if (allowVariableDistanceBuffer) {
  //      cdss.setDistance(d);
  //      cdss.setHeight(h);
  //    }

  merge();

  return conflict();
}


bool CDSICore::detectionLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon,
    const Plan& intent, double B, double T) {
  //    tin_size = 0;
  tin.clear();
  tout.clear();
  tca.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();

  bool linear = true;

  double t_base;
  int start_seg;
  bool cont = false;
  double BT = 0.0;
  double NT = 0.0;
  double HT = 0.0;

  if (t0 < intent.time(0)) {
    t_base = intent.time(0);
    start_seg = 0;
  } else {
    t_base = t0;
    start_seg = intent.getSegment(t0);
    if (start_seg < 0) {
      return false;  // t0 past end of Plan
    }
  }

  //    double d = getDistance();
  //    double h = getHeight();

  //std::cout << "$$CDSICoreLL1: intent size: "+intent.size()+" t_base: "+t_base);
  for (int j = start_seg; j < intent.size(); j++){
    // invariant: t0 <= t_base

    double dt = t_base - intent.time(j);
    if (checkSmallTimes && dt > 0.0 && dt < 0.000001) {
      error.addWarning("Attempting detectionLL on segment "+Fm0(j)+" of "+intent.getID()+" with very small offset: "+Fm12(dt));
    }

    LatLonAlt so2p = GreatCircle::linear_initial(so, vo, t_base-t0);  //CHANGED!!!
    LatLonAlt sip = intent.position(t_base,linear).lla();
    EuclideanProjection proj = Projection::createProjection(so.zeroAlt());   // CHECK THIS!!!  Should it be so2p?
    //      Vect3 s = proj.project(so2p).Sub(proj.project(sip));
//    Vect3 so3 = proj.project(so2p);
//    Vect3 si3 = proj.project(sip);


    //      if (allowVariableDistanceBuffer) {
    //        cdss.setDistance(d+intent.getSegmentDistanceBuffer(j));
    //        cdss.setHeight(h+intent.getSegmentHeightBuffer(j));
    //      }


    if (j == intent.size() - 1) {
      continue; // leave loop
    } else { // normal case
      HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
      BT = Util::max(0.0, B + t0 - t_base);
      NT = cont ? HT : T + t0 - t_base;

      //        Velocity vop = vo;
      //        Velocity vip = intent.initialVelocity(t_base);
//      Velocity vop = proj.projectVelocity(so2p, vo); //CHANGED!!!
//      Velocity vip = proj.projectVelocity(sip, intent.velocity(t_base, linear));

        std::pair<Vect3,Velocity> prj_o = proj.project(so2p, vo);
        std::pair<Vect3,Velocity> prj_i = proj.project(sip, intent.velocity(t_base, linear));
        Vect3 so3 = prj_o.first;
        Vect3 si3 = prj_i.first;
        Velocity vop = prj_o.second;
        Velocity vip = prj_i.second;


      //        std::cout << "$$CDSICoreLL2 positions: "<< so2p.toString() <<" "<<sip.toString() << std::endl;
      //        std::cout << "$$CDSICoreLL3 rel pos and vo: "<<s.toString() <<" "<<vop.toString() << std::endl;
      //        std::cout << "$$CDSICoreLL4 vi "<<intent.initialVelocity(t_base).toString()<<" v="<<Velocity.newVect3(vo.Sub(intent.initialVelocity(t_base))).toString() << std::endl;
      //        std::cout << "$$CDSICoreLL5 times: BT="<<BT<<" NT="<<NT<<" HT"<<HT<<" cont="<<cont <<std::endl;
      //Vect3 r = s.AddScal(1317.0 - t_base, vo.Sub(intent.initialVelocity(t_base)));
      //Vect3 r = s.AddScal(1295.0 - t_base, vo.Sub(intent.velocity(0)));
      //std::cout << "$$CDSICoreLL6" << r << " " << r.vect2().norm()<< " "<<Units.str("ft", r.z));
      if (NT >= 0) {
        //cd.setTimeHorizon(HT);

        double dt = NT-BT;
        if (checkSmallTimes && dt > 0) {
          if (dt < cdsscore.getFilterTime()) {
            error.addWarning("Attempting detectionLL on segment "+Fm0(j)+" of "+intent.getID()+" with duration smaller than the filter time");
          } else if (dt < 0.000001) {
            error.addWarning("Attempting detectionLL on segment "+Fm0(j)+" at time "+Fm12(BT+t_base)+" of "+intent.getID()+" with very small duration: "+Fm12(dt));
          }
        }


        if ( cdsscore.detectionBetween(so3, vop, si3, vip, BT, NT, HT) ) {  //CHANGED!!!
          if (std::abs((t0+B) - (cdsscore.getTimeOut()+t_base)) > 0.0000001) {
            captureOutput(t_base, j);
            //	    	  if ( ! captureOutput(t_base, j, cdsscore)) {
            //				return true;
          }
        }
      }
    }
    cont = size() > 0 ? tout[(int)tout.size()-1] == HT + t_base : false;
    t_base = intent.time(j+1);
  }


  //    if (allowVariableDistanceBuffer) {
  //      cdss.setDistance(d);
  //      cdss.setHeight(h);
  //    }

  merge();

  return conflict();
}


void CDSICore::captureOutput(double t_base, int seg) {
  //  std::cout << "$$CDSICore output times "<<t_base<<" "<<(cdsscore.getTimeIn()+t_base)<<" "<<(cdsscore.getTimeOut()+t_base)<<" seg:"<<seg<<" tcpa: "<<cdsscore.timeOfClosestApproach()<<" filter="<< cdsscore.getFilterTime()<<std::endl;

  segin.push_back(seg);
  segout.push_back(seg);
  tin.push_back(cdsscore.getTimeIn()+t_base);
  if (cdsscore.getTimeOut() < 0) {
    tout.push_back(cdsscore.getTimeOut());
  } else {
    tout.push_back(cdsscore.getTimeOut()+t_base);
  }
  tca.push_back(cdsscore.getCriticalTime() + t_base);
  dist_tca.push_back(cdsscore.distanceAtCriticalTime());
  //    tin_size++;
  //    if (tin_size >= SIZE) {
  //      merge();
  //      if (tin_size >= SIZE) {
  //		error.addError("there are more conflicts than internal array size allows, increase SIZE constant");
  //		return false;
  //      }
  //      return true;
  //    }
  //	return true;
}


void CDSICore::merge() {
  int i = 0;
  while ( i < (int)tin.size() - 1){
    if ( ! Util::almost_less(tout[i], tin[i+1], PRECISION7)) {
      tin.erase(tin.begin()+i+1);
      segin.erase(segin.begin()+i+1);
      tout.erase(tout.begin()+i);
      segout.erase(segout.begin()+i);
      if (dist_tca[i] < dist_tca[i+1]) {
        tca.erase(tca.begin()+i+1);
        dist_tca.erase(dist_tca.begin()+i+1);
      } else {
        tca.erase(tca.begin()+i);
        dist_tca.erase(dist_tca.begin()+i);
      }
      //      remove(tin, i+1, SIZE);
      //      remove(segin, i+1, SIZE);
      //      remove(tout, i, SIZE);
      //      remove(segout, i, SIZE);
      //      if (dist_tca[i] < dist_tca[i+1]) {
      //        remove(tca,i+1,SIZE);
      //        remove(dist_tca,i+1,SIZE);
      //      } else {
      //        remove(tca,i,SIZE);
      //        remove(dist_tca,i,SIZE);
      //      }
      //      tin_size--;
    } else {
      i++;
    }
  }
}

bool CDSICore::violation(const Position& so, const Velocity& vo, const Plan& intent, double tm) const {
  if (tm < intent.getFirstTime() || tm > intent.getLastTime()) {
    return false;
  }
  return cdsscore.violation(so, vo, intent.position(tm), intent.velocity(tm));
}



/**
 * EXPERIMENTAL
 * @param so
 * @param vo
 * @param t0
 * @param state_horizon
 * @param intent
 * @param B
 * @param T
 * @return true if conflict
 */
bool CDSICore::conflictXYZ(const Vect3& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent, double B, double T) const {
  bool linear = true;     // was Plan.PlanType.LINEAR);
  double t_base;
  int start_seg;
  double BT = 0.0;
  double NT = 0.0;
  double HT = 0.0;

  if (t0 < intent.time(0)) {
    t_base = intent.time(0);
    start_seg = 0;
  } else {
    t_base = t0;
    start_seg = intent.getSegment(t0);
    if (start_seg < 0) {
      return false;  // t0 past end of Flight Plan
    }
  }
  for (int j = start_seg; j < intent.size(); j++){

    double dt = t_base - intent.time(j);
    if (checkSmallTimes && dt > 0.0 && dt < 0.000001) {
      error.addWarning("Attempting conflictXYZ on segment "+Fm0(j)+" of "+intent.getID()+" with very small offset: "+Fm12(dt));
    }


    Vect3 sop = so.AddScal((t_base - t0),vo);
    Vect3 sip = intent.position(t_base,linear).vect3(); // intent.positionXYZ(j).AddScal((t_base - intent.getTime(j)),vi);
    if (j == intent.size() - 1) {
      continue;
    } else { // normal case
      HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
      BT = Util::max(0.0, B + t0 - t_base);
      NT = T + t0 - t_base;
      if (NT >= 0) {

        double dt = NT-BT;
        if (checkSmallTimes && dt > 0) {
          if (dt < cdsscore.getFilterTime()) {
            error.addWarning("Attempting conflictXYZ on segment "+Fm0(j)+" of "+intent.getID()+" with duration smaller than the filter time");
          } else if (dt < 0.000001) {
            error.addWarning("Attempting conflictXYZ on segment "+Fm0(j)+" at time "+Fm12(BT+t_base)+" of "+intent.getID()+" with very small duration: "+Fm12(dt));
          }
        }


        if (cdsscore.conflict(sop, vo, sip, intent.initialVelocity(j, linear), BT, Util::min(NT, HT)) ) return true;
      }
      t_base = intent.time(j+1);
    }
  }
  return false;
}



/**
 * EXPERIMENTAL
 */
bool CDSICore::conflictLL(const LatLonAlt& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent, double B, double T) const {
  bool linear = true;     // was Plan.PlanType.LINEAR);

  double t_base;
  int start_seg;
  double BT = 0.0;
  double NT = 0.0;
  double HT = 0.0;

  if (t0 < intent.time(0)) {
    t_base = intent.time(0);
    start_seg = 0;
  } else {
    t_base = t0;
    start_seg = intent.getSegment(t0);
    if (start_seg < 0) {
      return false;  // t0 past end of Flight Plan
    }
  }
  for (int j = start_seg; j < intent.size(); j++){

    double dt = t_base - intent.time(j);
    if (checkSmallTimes && dt > 0.0 && dt < 0.000001) {
      error.addWarning("Attempting conflictLL on segment "+Fm0(j)+" of "+intent.getID()+" with very small offset: "+Fm12(dt));
    }


    LatLonAlt so2p = GreatCircle::linear_initial(so, vo, t_base-t0);  //CHANGED!!!
    LatLonAlt sip = intent.position(t_base,linear).lla();
    EuclideanProjection proj = Projection::createProjection(so.zeroAlt()); // CHECK THIS!!!  Should it be so2p?
    Vect3 so3 = proj.project(so2p);
    Vect3 si3 = proj.project(sip);
    if (j == intent.size() - 1) {
      continue; // leave loop
    } else { // normal case
      HT = Util::max(0.0, Util::min(state_horizon - (t_base - t0), intent.time(j+1) - t_base));
      BT = Util::max(0.0, B + t0 - t_base);
      NT = T + t0 - t_base;
      Velocity vop = proj.projectVelocity(so2p, vo);  //CHANGED!!!
      Velocity vip = proj.projectVelocity(sip, intent.velocity(t_base, linear));
      if (NT >= 0) {

        dt = NT-BT;
        if (checkSmallTimes && dt > 0) {
          if (dt < cdsscore.getFilterTime()) {
            error.addWarning("Attempting conflictLL on segment "+Fm0(j)+" of "+intent.getID()+" with duration smaller than the filter time");
          } else if (dt < 0.000001) {
            error.addWarning("Attempting conflictLL on segment "+Fm0(j)+" at time "+Fm12(BT+t_base)+" of "+intent.getID()+" with very small duration: "+Fm12(dt));
          }
        }


        if (cdsscore.conflict(so3, vop, si3, vip, BT, Util::min(NT, HT)) ) return true;
      }
      t_base = intent.time(j+1);
    }
  }
  return false;
}



void CDSICore::setCoreDetectionPtr(const Detection3D* d) {
  cdsscore.setCoreDetectionPtr(d);
  tin.clear();
  tout.clear();
  tca.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();
}

void CDSICore::setCoreDetectionRef(const Detection3D& d) {
  cdsscore.setCoreDetectionRef(d);
  tin.clear();
  tout.clear();
  tca.clear();
  dist_tca.clear();
  segin.clear();
  segout.clear();
}


Detection3D* CDSICore::getCoreDetectionPtr() const {
  return cdsscore.getCoreDetectionPtr();
}

Detection3D& CDSICore::getCoreDetectionRef() const {
  return cdsscore.getCoreDetectionRef();
}

//  //deprecated functions:
//  bool cdsicore_xyz(const Vect3& so, const Velocity& vo, double t0, double state_horizon, const Plan& intent,
//            double D, double H, double B, double T) {
//    return CDSICore::cdsicore_xyz(so,vo,t0,state_horizon,intent,D,H,B,T);
////    CDSICore_def.setDistance(D);
////    CDSICore_def.setHeight(H);
////    return CDSICore_def.detectionXYZ(so, vo, t0, 10.0e+100, intent, B, T);
//  }
//
//  bool cdsicore_ll(const LatLonAlt& state, Velocity vo, double t0, double state_horizon,
//           const Plan& intent, double D, double H, double B, double T) {
//    return CDSICore::cdsicore_ll(state,vo,t0,state_horizon,intent,D,H,B,T);
////    CDSICore_def.setDistance(D);
////    CDSICore_def.setHeight(H);
////    return CDSICore_def.detectionLL(state, vo, t0, 10.0e+100, intent, B, T);
//  }

std::string CDSICore::toString() const {
  return "CDSICore: cd = "+cdsscore.toString();
}


}

