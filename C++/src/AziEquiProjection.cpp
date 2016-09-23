/*
 * AziEquiProjection.cpp
 *
 * Contact: Jeff Maddalon (j.m.maddalon@nasa.gov)
 * NASA LaRC
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "AziEquiProjection.h"
#include "EuclideanProjection.h"
#include "LatLonAlt.h"
#include "GreatCircle.h"
#include "Util.h"
#include "Units.h"
//#include "UnitSymbols.h"
#include "Constants.h"
#include <stdexcept>

namespace larcfm {
    using std::string;
    using std::cout;
    using std::endl;
    using std::runtime_error;
  
  // Static functions
  
  static Vect3 spherical2xyz(double lat, double lon) {
    double r = GreatCircle::spherical_earth_radius;
    // convert latitude to 0-PI
    double theta = Pi/2 - lat;
    double phi = Pi - lon;
    double x = r*std::sin(theta)*std::cos(phi);
    double y = r*std::sin(theta)*std::sin(phi);
    double z = r*std::cos(theta);
    return Vect3(x,y,z);
  }
  
  static LatLonAlt xyz2spherical(const Vect3& v, double alt) {
    double r = GreatCircle::spherical_earth_radius;
    double theta = acos_safe(v.z/r);
    double phi = atan2_safe(v.y, v.x);
    double lat = Pi/2 - theta;
    double lon = to_pi(Pi - phi);
    return LatLonAlt::mk(lat, lon, alt);
  }
  
  static Vect3 vect3_orthog_toy(const Vect3& v) {
    if (!Util::within_epsilon(v.x, Constants::GPS_LIMIT_HORIZONTAL) || !Util::within_epsilon(v.y, Constants::GPS_LIMIT_HORIZONTAL)) {
      return Vect3(v.y, -v.x, 0);
    } else {
      return Vect3(1,0,0);
    }
  }
  
  static Vect3 equator_map(const Vect3& ref, const Vect3& p) {
    Vect3 xmult = ref.Hat();
    Vect3 ymult = vect3_orthog_toy(ref).Hat();
    Vect3 zmult = ref.cross(vect3_orthog_toy(ref)).Hat();
    return Vect3(xmult.dot(p), ymult.dot(p), zmult.dot(p));
  }
  
  static Vect3 equator_map_inv(const Vect3& ref, const Vect3& p) {
    Vect3 xmult = ref.Hat();
    Vect3 ymult = vect3_orthog_toy(ref).Hat();
    Vect3 zmult = ref.cross(vect3_orthog_toy(ref)).Hat();
    Vect3 xmultInv = Vect3(xmult.x, ymult.x, zmult.x);
    Vect3 ymultInv = Vect3(xmult.y, ymult.y, zmult.y);
    Vect3 zmultInv = Vect3(xmult.z, ymult.z, zmult.z);
    return  Vect3(xmultInv.dot(p), ymultInv.dot(p), zmultInv.dot(p));
  }
  
  static Vect2 sphere_to_plane(const Vect3& ref, const Vect3& p) {
    Vect3 v = equator_map(ref,p);
    return Vect2(v.y, -v.z);
  }
  
  static Vect3 plane_to_sphere(const Vect2& v) {
    double r = GreatCircle::spherical_earth_radius;
    double x = std::sqrt(r*r - v.x*v.x - v.y*v.y);
    return Vect3(x, v.x, -v.y);
  }
  
  
    // AziEquiProjection
    
    AziEquiProjection::AziEquiProjection() {
      projAlt = 0;
      ref = Vect3();
      llaRef = LatLonAlt::ZERO();
    }
    
    AziEquiProjection::AziEquiProjection(const LatLonAlt& lla) {
        projAlt = lla.alt();
        ref = spherical2xyz(lla.lat(),lla.lon());
        llaRef = lla;
    }
    
    AziEquiProjection::AziEquiProjection(double lat, double lon, double alt) {
        projAlt = alt;
        ref = spherical2xyz(lat,lon);
        llaRef = LatLonAlt::mk(lat, lon, alt);
    }
    
    AziEquiProjection AziEquiProjection::makeNew(const LatLonAlt& lla) const {
      return AziEquiProjection(lla);
    }

    AziEquiProjection AziEquiProjection::makeNew(double lat, double lon, double alt) const {
      return AziEquiProjection(lat, lon, alt);
    }

    double AziEquiProjection::conflictRange(double lat, double accuracy) const {
//      if (accuracy < Units::from(_NM, 0.01)) { //~0.001 nm accuracy
//        return Units::from(_NM, 21);
//      } else if (accuracy < Units::from(_NM, 0.1)) {	//0.01 nm accuracy
//        return Units::from(_NM, 65);
//      } else if (accuracy < Units::from(_NM, 0.5)) { //0.1 nm accuracy
//        return Units::from(_NM, 150);
//      } else {  //0.5 nm accuracy
//        return Units::from(_NM, 260);
//      }
    	return Units::from("NM", std::floor(329.2*std::pow(Units::to("NM",accuracy),1.0/3.0)));
    }
    
    double AziEquiProjection::maxRange() const{
      return GreatCircle::spherical_earth_radius*Pi/2;
    }
    
	LatLonAlt AziEquiProjection::getProjectionPoint() const {
	  return llaRef;
	}

    Vect2 AziEquiProjection::project2(const LatLonAlt& lla) const {
      Vect2 p =  sphere_to_plane(ref, spherical2xyz(lla.lat(),lla.lon()));
      if (p.norm() <= 0.0) {
    	  return Vect2::ZERO();
      } else {
          return p.Scal(GreatCircle::distance(lla, llaRef)/p.norm());
      }
    }

    Vect3 AziEquiProjection::project(const LatLonAlt& lla) const {
       return Vect3(project2(lla),lla.alt() - projAlt);
    }      
    
    Vect3 AziEquiProjection::project(const Position& sip) const {
    	Vect3 si;
    	if (sip.isLatLon()) {
    		si = project(sip.lla());
    	} else {
    		si = sip.point();
    	}
    	return si;
    }

    Point AziEquiProjection::projectPoint(const Position& sip) const {
    	return Point::mk(project(sip));
    }


    LatLonAlt AziEquiProjection::inverse(const Vect2& xy, double alt) const {
      double d = std::sin(GreatCircle::angle_from_distance(xy.norm(),0.0))*GreatCircle::spherical_earth_radius;
    	return xyz2spherical(equator_map_inv(ref, plane_to_sphere(xy.Hat().Scal(d))), alt + projAlt);
    }

    LatLonAlt AziEquiProjection::inverse(const Vect3& xyz) const {
    	return inverse(xyz.vect2(), xyz.z);
    }
    
  Velocity AziEquiProjection::projectVelocity(const LatLonAlt& lla, const Velocity& v) const {
    double timeStep = 10.0;
    LatLonAlt ll2 = GreatCircle::linear_initial(lla,v,timeStep);
    Vect3 se = project(lla);
    Vect3 s2 = project(ll2);
    Vect3 vn = s2.Sub(se).Scal(1/timeStep);
    return Velocity::make(vn);
  }
  
  Velocity AziEquiProjection::projectVelocity(const Position& ss, const Velocity& v) const {
	  if (ss.isLatLon()) {
        return projectVelocity(ss.lla(),v);
	  } else {
		  return v;
	  }
  }

  // The user needs to keep track of whether to translate back (i.e. whether original was LatLon())
  Velocity AziEquiProjection::inverseVelocity(const Vect3& s, const Velocity& v, bool toLatLon) const {
    if (toLatLon) {
      double timeStep = 10.0;
      Vect3 s2 = s.linear(v,timeStep);
      LatLonAlt lla1 = inverse(s);
      LatLonAlt lla2 = inverse(s2);
      Velocity nv = GreatCircle::velocity_initial(lla1,lla2,timeStep);
      return nv;
    } else {
      return v;
    }
  }

  std::pair<Vect3,Velocity> AziEquiProjection::project(const Position& p, const Velocity& v) const {
   	return std::pair<Vect3,Velocity>(project(p),projectVelocity(p,v));
   }

  std::pair<Position,Velocity> AziEquiProjection::inverse(const Vect3& p, const Velocity& v, bool toLatLon) const {
    if (toLatLon) {
      return std::pair<Position,Velocity>(Position(inverse(p)),inverseVelocity(p,v,true));
    } else {
      return std::pair<Position,Velocity>(Position(p),v);
    }
   }


}

