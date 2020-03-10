/*
 * CriticalVectors.cpp
 *
 * Contact: Cesar Munoz (cesar.a.munoz@nasa.gov)
 * NASA LaRC
 * http://shemesh.larc.nasa.gov/people/cam/ACCoRD
 *
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include <list>
#include "Plan.h"
#include "CriticalVectors.h"
#include "CriticalVectorsSI.h"
#include "Velocity.h"
#include "GreatCircle.h"
#include "Projection.h"
#include "Units.h"
#include "Vect2.h"
#include "Vect3.h"
#include "Util.h"
#include "NavPoint.h"

namespace larcfm {
  
  std::list<Vect2> CriticalVectorsSI::tracks(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
			  const double D, const double H, const double B, const double T) {

    std::list<Vect2> lst;

    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Vect3 vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);
        Vect3 si = fp.point(i).vect3();
        Vect3 si_o = si -(vi * (fi_st-to));
        Vect3 s = so - (si_o);
        double rel_st = st-to;
        double rel_et = et-to;

        std::list<Vect2> r = CriticalVectors::tracks(s,vo,vi,D,H,rel_st,rel_et);
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }
  

  std::list<Vect2> CriticalVectorsSI::groundSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
				const double D, const double H, const double B, const double T) {
    std::list<Vect2> lst;
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Vect3 vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);
        Vect3 si = fp.point(i).vect3();
        Vect3 si_o = si -(vi * (fi_st-to));
        Vect3 s = so - (si_o);
        double rel_st = st-to;
        double rel_et = et-to;

        std::list<Vect2> r = CriticalVectors::groundSpeeds(s,vo,vi,D,H,rel_st,rel_et);
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }

  std::list<double> CriticalVectorsSI::verticalSpeeds(const Vect3& so, const Vect3& vo, double to, const Plan& fp,
				   const double D, const double H, const double B, const double T) {
    std::list<double> lst;
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Vect3 vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);
        Vect3 si = fp.point(i).vect3();
        Vect3 si_o = si -(vi * (fi_st-to));
        Vect3 s = so - (si_o);
        double rel_st = st-to;
        double rel_et = et-to;
        
        std::list<double> r = CriticalVectors::verticalSpeeds(s,vo,vi,D,H,rel_st,rel_et);
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }


  std::list<Vect2> CriticalVectorsSI::tracksLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
      const double D, const double H, const double B, const double T) {

    std::list<Vect2> lst;

    LatLonAlt so = LatLonAlt::mk(lat, lon, alt);
    EuclideanProjection proj = Projection::createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space
    Velocity vop = proj.projectVelocity(so,vo);

    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Velocity vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);


        NavPoint si = fp.point(i);

        LatLonAlt siLL = si.lla();
        double dt = fi_st - to;

//        LatLonAlt si_o = GreatCircle::linear_initial(siLL, vi, -dt); // CHANGED!!!
//        Vect3 s = proj.project(so).Sub(proj.project(si_o));
//        Velocity vop = proj.projectVelocity(so, vo);
//        Velocity vip = proj.projectVelocity(si_o, vi);

        Velocity vip = proj.projectVelocity(siLL,vi);
        Vect3 s = proj.project(so).Sub(proj.project(siLL).AddScal(-dt,vip));
        
        double rel_st = st-to;
        double rel_et = et-to;

        std::list<Vect2> r = CriticalVectors::tracks(s,vop,vip,D,H,rel_st,rel_et); //CHANGED!!!
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }


  std::list<Vect2> CriticalVectorsSI::groundSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
      const double D, const double H, const double B, const double T) {
    std::list<Vect2> lst;

    LatLonAlt so = LatLonAlt::mk(lat, lon, alt);
    EuclideanProjection proj = Projection::createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space
    Velocity vop = proj.projectVelocity(so,vo);
  
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Velocity vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);

        NavPoint si = fp.point(i);

        LatLonAlt siLL = si.lla();
        double dt = fi_st - to;

//        LatLonAlt si_o = GreatCircle::linear_initial(siLL, vi, -dt); // CHANGED!!!
//        Vect3 s = proj.project(so).Sub(proj.project(si_o));
//        Velocity vop = proj.projectVelocity(so, vo);
//        Velocity vip = proj.projectVelocity(si_o, vi);

        Velocity vip = proj.projectVelocity(siLL,vi);
        Vect3 s = proj.project(so).Sub(proj.project(siLL).AddScal(-dt,vip));

        double rel_st = st-to;
        double rel_et = et-to;

        std::list<Vect2> r = CriticalVectors::groundSpeeds(s,vop,vip,D,H,rel_st,rel_et); //CHANGED!!!
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }

  std::list<double> CriticalVectorsSI::verticalSpeedsLL(double lat, double lon, double alt, const Velocity& vo, double to, const Plan& fp,
      const double D, const double H, const double B, const double T) {
    std::list<double> lst;

    LatLonAlt so = LatLonAlt::mk(lat, lon, alt);
    EuclideanProjection proj = Projection::createProjection(so.zeroAlt()); // this will preserve altitudes in Vect3 space
    Velocity vop = proj.projectVelocity(so,vo);
    
    for(int i = 0; i < fp.size()-1; i++) {
      double fi_st = fp.time(i);
      double fi_et = fp.time(i+1);
      if(fi_et > B+to && fi_st < T+to) {
        bool linear = true;
        Velocity vi = fp.initialVelocity(i, linear);
        double st = Util::max(fi_st,B+to);
        double et = Util::min(fi_et,T+to);

        NavPoint si = fp.point(i);

        LatLonAlt siLL = si.lla();
        double dt = fi_st - to;

//        LatLonAlt si_o = GreatCircle::linear_initial(siLL, vi, -dt); // CHANGED!!!
//        Vect3 s = proj.project(so).Sub(proj.project(si_o));
//        Velocity vop = proj.projectVelocity(so, vo);
//        Velocity vip = proj.projectVelocity(si_o, vi);
        
        Velocity vip = proj.projectVelocity(siLL,vi);
        Vect3 s = proj.project(so).Sub(proj.project(siLL).AddScal(-dt,vip));

        double rel_st = st-to;
        double rel_et = et-to;

        std::list<double> r = CriticalVectors::verticalSpeeds(s,vop,vip,D,H,rel_st,rel_et); //CHANGED!!!
        lst.insert(lst.end(), r.begin(), r.end());
      }
    }
    return lst;
  }


}
