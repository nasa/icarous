/*
 * SimpleNoPolarProjection.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */

#include "SimpleNoPolarProjection.h"
#include "SimpleProjection.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
//#include "UnitSymbols.h"
#include "Util.h"
#include "Constants.h"
#include "format.h"
#include <stdexcept>
#include <limits>


namespace larcfm {
    using std::string;
    using std::cout;
    using std::endl;
    using std::runtime_error;
  
    SimpleNoPolarProjection::SimpleNoPolarProjection() {
      projLat = projLon = projAlt = 0.0;
    }
    
    SimpleNoPolarProjection::SimpleNoPolarProjection(const LatLonAlt& lla) {
        projLat = lla.lat();
        projLon = lla.lon();
        projAlt = lla.alt();
    }
    
    SimpleNoPolarProjection::SimpleNoPolarProjection(double lat, double lon, double alt) {
        projLat = lat;
        projLon = lon;
        projAlt = alt;
    }
    
    SimpleNoPolarProjection SimpleNoPolarProjection::makeNew(const LatLonAlt& lla) const {
      return SimpleNoPolarProjection(lla);
    }

    SimpleNoPolarProjection SimpleNoPolarProjection::makeNew(double lat, double lon, double alt) const {
      return SimpleNoPolarProjection(lat, lon, alt);
    }

//    SimpleNoPolarProjection SimpleNoPolarProjection::makeNewCopy() const {
//      return SimpleNoPolarProjection(projLat, projLon, projAlt);
//    }

    double SimpleNoPolarProjection::conflictRange(double lat, double accuracy) const {
      double degs = Units::to("deg",lat);
      if (accuracy < Units::from("nm", 0.1)) { //0.01 -- 35 nm - 3 nm
        if (degs < 30) {
          return Units::from("nm",10);
        } else {
          return Units::from("nm",5);
        }
      } else if (accuracy < Units::from("nm", 0.5)) {  // 0.1 nm -- 185 nm - 4 nm
        if (degs < 20) {
          return Units::from("nm", 40);
        } else if (degs < 50) {
          return Units::from("nm", 25);
        } else if (degs < 70) {
          return Units::from("nm", 15);
        } else if (degs < 80) {
          return Units::from("nm", 10);
        } else {
          return Units::from("nm", 5);
        }    		
      } else { // 0.5 nm -- 330 nm - 13 nm
        if (degs < 20) {
          return Units::from("nm", 95);
        } else if (degs < 50) {
          return Units::from("nm", 50);
        } else if (degs < 70) {
          return Units::from("nm", 20);
        } else if (degs < 80) {
          return Units::from("nm", 10);
        } else {
          return Units::from("nm", 5);
        }
      }
    }
    
    double SimpleNoPolarProjection::maxRange() const{
      return std::numeric_limits<double>::max();
    }
    
    LatLonAlt SimpleNoPolarProjection::getProjectionPoint() const {
      return LatLonAlt::mk(projLat, projLon, projAlt);
    }
    	
    Vect2 SimpleNoPolarProjection::project2(const LatLonAlt& lla) const {
           return SimpleProjection::projectXY(projLat,projLon,lla.lat(),lla.lon());
           
    }

    Vect3 SimpleNoPolarProjection::project(const LatLonAlt& lla) const {
       return Vect3(project2(lla),lla.alt() - projAlt);
    }      
    
    Vect3 SimpleNoPolarProjection::project(const Position& sip) const {
    	Vect3 si;
    	if (sip.isLatLon()) {
    		si = project(sip.lla());
    	} else {
    		si = sip.point();
    	}
    	return si;
    }

    Point SimpleNoPolarProjection::projectPoint(const Position& sip) const {
    	return Point::mk(project(sip));
    }

    LatLonAlt SimpleNoPolarProjection::inverse(const Vect2& xy, double alt) const {
           double a = xy.compassAngle();
           double d = xy.norm();
    	   LatLonAlt origin = LatLonAlt::mk(projLat,projLon,alt + projAlt);
           return GreatCircle::linear_initial(origin,a,d);
    }

    LatLonAlt SimpleNoPolarProjection::inverse(const Vect3& xyz) const {  
    	return inverse(xyz.vect2(), xyz.z);
    }
    
  Velocity SimpleNoPolarProjection::projectVelocity(const LatLonAlt& lla, const Velocity& v) const {
//    double timeStep = 10.0;
//    LatLonAlt ll2 = GreatCircle::linear_initial(lla,v,timeStep);
//    Vect3 se = project(lla);
//    Vect3 s2 = project(ll2);
//    Vect3 vn = s2.Sub(se).Scal(1/timeStep);
//    return Velocity::make(vn);
	  return v;
  }
  
  Velocity SimpleNoPolarProjection::projectVelocity(const Position& ss, const Velocity& v) const {
//	    Velocity vn;
//	    if (ss.isLatLon()) {
//	      vn = projectVelocity(ss.lla(),v);
//	    } else {
//	      vn = v;
//	    }
//	    return vn;
	  return v;
  }

  // The user needs to keep track of whether to translate back (i.e. whether original was LatLon())
  Velocity SimpleNoPolarProjection::inverseVelocity(const Vect3& s, const Velocity& v, bool toLatLon) const {
//    if (toLatLon) {
//      double timeStep = 10.0;
//      Vect3 s2 = s.linear(v,timeStep);
//      LatLonAlt lla1 = inverse(s);
//      LatLonAlt lla2 = inverse(s2);
//      Velocity nv = GreatCircle::velocity_initial(lla1,lla2,timeStep);
//      return nv;
//    } else {
//      return v;
//    }
	  return v;
  }

  std::pair<Vect3,Velocity> SimpleNoPolarProjection::project(const Position& p, const Velocity& v) const {
   	return std::pair<Vect3,Velocity>(project(p),projectVelocity(p,v));
   }


  std::pair<Position,Velocity> SimpleNoPolarProjection::inverse(const Vect3& p, const Velocity& v, bool toLatLon) const {
    if (toLatLon) {
      return std::pair<Position,Velocity>(Position(inverse(p)),inverseVelocity(p,v,true));
    } else {
      return std::pair<Position,Velocity>(Position(p),v);
    }
   }


}

