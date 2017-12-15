/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "CDPolyIter.h"
#include "Vect2.h"
#include "Vertical.h"
#include "MovingPolygon2D.h"
#include "CDPolyIter2D.h"
#include "Util.h"
#include "format.h"
#include <vector>
#include <string>

namespace larcfm {

  
  const std::pair<double,double> CDPolyIter::noDetection = std::pair<double,double>(0.0,-1.0);


  CDPolyIter::CDPolyIter() {
    id = "";
    timeStep = 1.0;
  }

//  CDPolyIter::~CDPolyIter() {}
//  CDPolyIter::CDPolyIter(const CDPolyIter& d){
//    timesin = d.timesin;
//    timesout = d.timesout;
//    timestca = d.timestca;
//    diststca = d.diststca;
//  }
//  CDPolyIter& CDPolyIter::operator= (const CDPolyIter& d) {
//    timesin = d.timesin;
//    timesout = d.timesout;
//    timestca = d.timestca;
//    diststca = d.diststca;
//    return *this;
//  }

  double CDPolyIter::getTimeStep() const {
    return timeStep;
  }

  void CDPolyIter::setTimeStep(double val) {
    if (val > 0.0) {
      timeStep = val;
    }
  }

  std::vector<Vect2> CDPolyIter::polygon_2D_at(const MovingPolygon2D& mp, double tr) {
    std::vector<Vect2> p;
    std::vector<Vect2> polystart = mp.getPolystart();
    std::vector<Vect2> polyvel = mp.getPolyvel();
    for (int ii = 0; ii < (int)polystart.size(); ii++) {
      p.push_back(polystart[ii].Add(polyvel[ii].Scal(tr)));
    }
    return p;
  }




  
  
  std::pair<double,double> CDPolyIter::polygon_alt_inside_time(double B, double T, double vspeed, double minalt, double maxalt, double sz, double vz) {
     if (B > T) {
      return noDetection; 
    }
    if (minalt>=maxalt) {
      return noDetection;
    } else if (Util::almost_equals(vz-vspeed, 0.0) || Util::almost_equals(B, T)) {
      if (within_polygon_altitude(minalt+B*vspeed,maxalt+B*vspeed,sz+B*vz)) {
        return std::pair<double,double>(B,T);
      } else {
        return noDetection;
      }
    } else {
      double thin  = Vertical::Theta_H(sz-(minalt+maxalt)/2,vz-vspeed,-1,(maxalt-minalt)/2);
      double thout = Vertical::Theta_H(sz-(minalt+maxalt)/2,vz-vspeed,1,(maxalt-minalt)/2);
      if (thout < B || thin > T) {
        return noDetection;
      } else {
        return  std::pair<double,double>(Util::max(B,Util::min(T,thin)), Util::max(B,Util::min(T,thout)));
      }
    }
  }
  
  bool CDPolyIter::within_polygon_altitude(double minalt, double maxalt, double sz) {
    return minalt <= sz && sz <= maxalt;
  }

  bool CDPolyIter::contains(Poly3D& p3d, const Vect3& s) {
    Poly2D ap = p3d.poly2D();
   return ap.contains(s.vect2()) && within_polygon_altitude(p3d.getBottom(), p3d.getTop(), s.z);
  }


//  ParameterData getParameterData() const;
//
//  ParameterData getParameterData(ParameterData& p) const;
//
//  void setParameterData(const ParameterData& p);


  ParameterData CDPolyIter::getParameters() const {
    ParameterData p;
    updateParameterData(p);
    return p;
  }

    void CDPolyIter::updateParameterData(ParameterData& p) const {
      p.set("id", id);
  }

    void CDPolyIter::setParameters(const ParameterData& p) {
      if (p.contains("id")) {
        id =  p.getString("id");
      }
     }


  DetectionPolygon* CDPolyIter::make() const {
    return new CDPolyIter();
  }

  DetectionPolygon* CDPolyIter::copy() const {
    CDPolyIter* ret = new CDPolyIter();
    ret->timesin = timesin;
    ret->timesout = timesout;
    ret->timestca = timestca;
    ret->diststca = diststca;
    ret->id = id;
    return ret; 
  }
  
  bool CDPolyIter::polyIter_detection(double B, double T, const MovingPolygon3D& mp3D, const Vect3& s, const Velocity& v) {
    timesin = std::vector<double>();
    timesout = std::vector<double>();
    timestca = std::vector<double>();
    diststca = std::vector<double>();
    if (T < 0.0) {
      T = 36000.0;
    }
    if (B > T || v.vect2().isZero()) {
      return false;
    } else {
//fpln(" $$ poly3D_detection: B= "+Fm4(B)+" T ="+Fm4(T)+" (mp3D = "+mp3D.toString()+")    s = "+s.toString()+" v = "+v.toString());
      // shortcut
      std::pair<double,double> tp = polygon_alt_inside_time(B,T,mp3D.getVspeed(),mp3D.getMinalt(),mp3D.getMaxalt(),s.z,v.z);
      if (tp == noDetection) {
//fpln("polyIter_detection 2");
        return false;
      }
      double tin = tp.first;
      double tout = tp.second;
      MovingPolygon2D start2dpoly = MovingPolygon2D();
      start2dpoly.setPolystart(polygon_2D_at(mp3D.getHorizpoly(),tin));
      start2dpoly.setPolyvel(mp3D.getHorizpoly().getPolyvel());
      //fpln(" $$$$ polyIter_detection:  start2dpoly.polyvel = "+strVelocities(mp3D.getHorizpoly().getPolyvel()));

      start2dpoly.setTend(mp3D.getHorizpoly().getTend() - tin);
      //bool poly2D_detection(double T, Poly2D pi, Vect2 vi, Vect2 so, Vect2 vo) {    
      CDPolyIter2D cdp2 = CDPolyIter2D();
      bool ret = cdp2.polyIter2D_detection(tout-tin,start2dpoly,s.vect2().Add(v.vect2().Scal(tin)),v.vect2(), timeStep);
      if (!ret) {
//fpln("polyIter_detection 3");
        return false;
      }
      for(int i = 0; i < cdp2.getConflicts(); i++) {
        double mytin = cdp2.getTimesIn()[i]+tin;
        double mytout = cdp2.getTimesOut()[i]+tin;
        tp = polygon_alt_inside_time(mytin,mytout,mp3D.getVspeed(),mp3D.getMinalt(),mp3D.getMaxalt(),s.z,v.z);
//fpln("mytin="+Fm4(mytin)+" mytout="+Fm4(mytout)+" tp="+Fm4(tp.first)+","+Fm4(tp.second));
        if (tp != noDetection) {
          timesin.push_back(tp.first);
          timesout.push_back(tp.second);
          double tca = (tp.first+tp.second)/2.0;
          timestca.push_back(tca);  //TODO FIX ME!!! NOT CORRECT!!!
          Poly3D pos = mp3D.position(tca); 
          double dist = pos.centroid().distanceH(s.AddScal(tca, v));
          diststca.push_back(dist);
         }
      }
//fpln("polyIter_detection 4 " + Fmi((int)timesin.size()));
      return timesin.size() > 0;
    }
  }



  bool CDPolyIter::violation(const Vect3& so, const Velocity& vo, const Poly3D& si) const {

    Poly3D asi = si;
    CDPolyIter det;

    //fpln("CDPolyIter.violation: so="+so+" vo="+vo+" si=("+si+")");
      return det.contains(asi, so);
  }



  bool CDPolyIter::conflict(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) const {
    //fpln("CDPolyIter.conflict: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
    CDPolyIter det;
    return det.polyIter_detection(B,T,si,so,vo);
  }



  bool CDPolyIter::conflictDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& si, double B, double T) {
    //fpln("CDPolyIter.conflictDetection: so="+so+" vo="+vo+" si=("+si+") B="+B+" T="+T);
    return polyIter_detection(B,T,si,so,vo);
  }



  std::vector<double> CDPolyIter::getTimesIn() const {
    return timesin;
  }



  std::vector<double> CDPolyIter::getTimesOut() const {
    return timesout;
  }

  double CDPolyIter::getTimeIn(int i) const {
    if (timesin.size() == 0) return -1;
    if (i > (int)timesin.size()) {
      fpln(" $$ getTimeIn error, index out of range");
      i = timesin.size()-1;
    }
    return timesin[i];
  }

  /**
   * Time to exit from loss of separation in internal units.
   * 
   * @return the time to exit out loss of separation. If timeOut is zero then 
   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
   * Note that this is a relative time.
   */
  double CDPolyIter::getTimeOut(int i) const {
    if (timesin.size() == 0) return 0;
    if (i > (int)timesin.size()) {
      fpln(" $$ getTimeIn error, index out of range");
      i = timesin.size()-1;
    }
    return timesout[i];
  }

  double CDPolyIter::getTimeIn() const {
    if (timesin.size() == 0) return -1;
    return timesin[0];
  }

  /**
   * Time to exit from loss of separation in internal units.
   * 
   * @return the time to exit out loss of separation. If timeOut is zero then 
   * there is no conflict. If timeOut is negative then, timeOut is infinite.  
   * Note that this is a relative time.
   */
  double CDPolyIter::getTimeOut() const {
    if (timesin.size() == 0) return 0;
    return timesout[timesout.size()-1];
  }



  std::vector<double> CDPolyIter::getCriticalTimesOfConflict() const {
    return timestca;
  }



  std::vector<double> CDPolyIter::getDistancesAtCriticalTimes() const {
    return diststca;
  }

  std::string CDPolyIter::strVelocities(const std::vector<Vect2>& polyvel) const {
    std::string rtn = "";
    for (int j = 0; j < (int) polyvel.size(); j++) {
        Vect2 v = polyvel[j];
         rtn = rtn + v.toString();
    }
    return rtn;
  }

  
  std::string CDPolyIter::toString() const {
    return "CDPolyIter "+id+" timeStep="+Fm2(timeStep);
  }

  std::string CDPolyIter::getClassName() const {
    return "CDPolyIter";
  }

  std::string CDPolyIter::getIdentifier() const {
    return id;
  }

  void CDPolyIter::setIdentifier(const std::string& s) {
    id = s;
  }

  bool CDPolyIter::equals(DetectionPolygon* d) const {
    if (!larcfm::equals(getClassName(), d->getClassName())) return false;
    if (!larcfm::equals(id, d->getIdentifier())) return false;
    if (timeStep != ((CDPolyIter*)d)->getTimeStep()) return false;
    return true;
  }

}
