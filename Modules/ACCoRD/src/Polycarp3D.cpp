/*
 * Polycarp3D - containment and conflict detection for 3D polygons
 *
 * Contact: Anthony Narkawicz (anthony.narkawicz@nasa.gov), George Hagen (george.hagen@nasa.gov)
 *
 * Copyright (c) 2015-2018 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
/*
 * Polycarp3D.cpp
 *
 *  Created on: Nov 23, 2015
 *      Author: ghagen
 */

#include "Polycarp3D.h"
#include "Vect3.h"
#include "Poly3D.h"
#include "Velocity.h"
#include "MovingPolygon3D.h"
#include "PolycarpContain.h"
#include "PolycarpDetection.h"
#include "PolycarpAcceptablePolygon.h"
#include "Vertical.h"
#include "IntervalSet.h"
#include "Interval.h"
#include "Units.h"
#include "Util.h"
#include "format.h"

#include <vector>

namespace larcfm {


bool Polycarp3D::nearEdge(const Vect3& so, const Poly3D& p, double h, double v, bool checkNice) {
    // well above or below polygon
    if (so.z < p.getBottom()-v || so.z > p.getTop()+v) {
        return false;
    }
    std::vector<Vect2> ps = std::vector<Vect2>();
    ps.reserve(p.size());
    for (int i = 0; i < p.size(); i++) {
      ps.push_back(p.poly2D().get(i));
    }
    if (checkNice && !PolycarpContain::nice_polygon_2D(ps, h)) {
      ps.clear();
      for (int i = p.size()-1; i >= 0; i--) {
        ps.push_back(p.poly2D().get(i));
      }
      if (!PolycarpContain::nice_polygon_2D(ps, h)) {
        fpln("WARNING: Polycarp3D.violation: NOT A NICE POLYGON!");
        return false;
      }
    }
    // near a 2D edge
    if (PolycarpContain::near_any_edge(ps,so.vect2(),h)) {
        return true;
    }
    // within polygon and near top or bottom
    if (PolycarpContain::definitely_inside(ps, so.vect2(), h) && (so.z < p.getBottom()+v || so.z > p.getTop()-v)) {
        return true;
    }
    // not near any edge
    return false;
}


bool Polycarp3D::definitely_inside(const Vect3& so, const Poly3D& p, double buff, bool checkNice) {
    if (so.z < p.getBottom()+buff || so.z > p.getTop()-buff) {
        return false;
    }
    std::vector<Vect2> ps = std::vector<Vect2>();
    ps.reserve(p.size());
    for (int i = 0; i < p.size(); i++) {
      ps.push_back(p.poly2D().get(i));
    }
    if (checkNice && !PolycarpContain::nice_polygon_2D(ps, buff)) {
      ps.clear();
      for (int i = p.size()-1; i >= 0; i--) {
        ps.push_back(p.poly2D().get(i));
      }
      if (!PolycarpContain::nice_polygon_2D(ps, buff)) {
        fpln("WARNING: Polycarp3D.definitely_inside: NOT A NICE POLYGON!");
        return false;
      }
    }
    return PolycarpContain::definitely_inside(ps, so.vect2(), buff);
}

bool Polycarp3D::definitely_outside(const Vect3& so, const Poly3D& p, double buff, bool checkNice) {
    if (so.z < p.getBottom()-buff || so.z > p.getTop()+buff) {
        return true;
    }
    std::vector<Vect2> ps = std::vector<Vect2>();
    ps.reserve(p.size());
    for (int i = 0; i < p.size(); i++) {
      ps.push_back(p.poly2D().get(i));
    }
    if (checkNice && !PolycarpContain::nice_polygon_2D(ps, buff)) {
      ps.clear();
      for (int i = p.size()-1; i >= 0; i--) {
        ps.push_back(p.poly2D().get(i));
      }
      if (!PolycarpContain::nice_polygon_2D(ps, buff)) {
        fpln("WARNING: Polycarp3D.definitely_outside: NOT A NICE POLYGON!");
        return false;
      }
    }
    return PolycarpContain::definitely_outside(ps, so.vect2(), buff);
}



bool Polycarp3D::violation(const Vect3& so, const Poly3D& p, double buff, bool checkNice) {
  if (so.z < p.getBottom() || so.z > p.getTop()) {
    return false;
  }
  std::vector<Vect2> ps = std::vector<Vect2>();
  ps.reserve(p.size());
  for (int i = 0; i < p.size(); i++) {
    ps.push_back(p.poly2D().get(i));
  }
//  if (!PolycarpAcceptablePolygon::counterclockwise_edges(ps)) {
  if (checkNice && !PolycarpContain::nice_polygon_2D(ps, buff)) {
    ps.clear();
    for (int i = p.size()-1; i >= 0; i--) {
      ps.push_back(p.poly2D().get(i));
    }
    if (!PolycarpContain::nice_polygon_2D(ps, buff)) {
      fpln("WARNING: Polycarp3D.violation: NOT A NICE POLYGON!");
      return false;
    }
  }
  return PolycarpContain::definitely_inside(ps, so.vect2(), buff);
}

bool Polycarp3D::entranceDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac,  bool checkNice) {
  double sz = so.z - (mp.maxalt+mp.minalt)/2.0;
  double vz = vo.z - mp.vspeed;
  // times cannot be too big!  limit to 100 days for now
  double b = Util::max(0.0, Util::min(B, Units::from("day",100.0)));
  double t = Util::max(0.0, Util::min(T, Units::from("day",100.0)));
  if (vz != 0.0) {
    double h = (mp.maxalt-mp.minalt)/2.0;
    double vb = Vertical::Theta_H(sz, vz, -1, h);
    double vt = Vertical::Theta_H(sz, vz, +1, h);
    b = Util::max(B, vb);
    t = Util::min(T, vt);
  } else if (so.z < mp.minalt || so.z > mp.maxalt) {
    return false;
  }
  if (t < b) {
    return false;
  }

  MovingPolygon3D mp2 = mp;
//  if (!PolycarpAcceptablePolygon::counterclockwise_edges(mp.horizpoly.polystart)) {
  if (checkNice && !PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
    mp2 = mp.reverseOrder();
    if (!PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
      fpln("WARNING: Polycarp3D.detection: NOT A NICE POLYGON!");
      return false;
    }
    if (!mp2.isStable() && !PolycarpDetection::nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
        fpln("WARNING: Polycarp3D.detection: NOT A NICE MOVING POLYGON!");
        return false;
    }
  }

  if (mp2.isStable()) {
    Vect2 pv =  mp2.velocity(0).vect2();
    return PolycarpDetection::Static_Collision_Detector(b, t, mp2.horizpoly.polystart, pv, so.vect2(), vo.vect2(), buff, true);
  } else {
    return PolycarpDetection::Collision_Detector(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac, true);
  }
}

bool Polycarp3D::exitDetection(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac, bool checkNice) {
  double sz = so.z - (mp.maxalt+mp.minalt)/2.0;
  double vz = vo.z - mp.vspeed;
  double b = Util::max(0.0, Util::min(B, Units::from("day",100.0)));
  double t = Util::max(0.0, Util::min(T, Units::from("day",100.0)));
  if (vz != 0.0) {
    double h = (mp.maxalt-mp.minalt)/2.0;
    double vb = Vertical::Theta_H(sz, vz, -1, h);
    double vt = Vertical::Theta_H(sz, vz, +1, h);
    b = Util::max(B, vb);
    t = Util::min(T, vt);
  } else if (so.z < mp.minalt || so.z > mp.maxalt) {
    return true;
  }
  if (t < b) {
    return true;
  }

  MovingPolygon3D mp2 = mp;
//  if (!PolycarpAcceptablePolygon::counterclockwise_edges(mp.horizpoly.polystart)) {
  if (checkNice && !PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
    mp2 = mp.reverseOrder();
    if (!PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
      fpln("WARNING: Polycarp3D.detection: NOT A NICE POLYGON!");
      return false;
    }
    if (!mp2.isStable() && !PolycarpDetection::nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
        fpln("WARNING: Polycarp3D.detection: NOT A NICE MOVING POLYGON!");
        return false;
    }
  }

  if (mp2.isStable()) {
    Vect2 pv =  mp2.velocity(0).vect2();
    return PolycarpDetection::Static_Collision_Detector(b, t, mp2.horizpoly.polystart, pv, so.vect2(), vo.vect2(), buff, false);
  } else {
    return PolycarpDetection::Collision_Detector(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac, false);
  }
}

IntervalSet Polycarp3D::conflictTimes(const Vect3& so, const Velocity& vo, const MovingPolygon3D& mp, double B, double T, double buff, double fac, bool avoid, bool checkNice) {
  double sz = so.z - (mp.maxalt+mp.minalt)/2.0;
  double vz = vo.z - mp.vspeed;
  double b = Util::max(0.0, Util::min(B, Units::from("day",100.0)));
  double t = Util::max(0.0, Util::min(T, Units::from("day",100.0)));
  IntervalSet ret;
  if (vz != 0.0) {
    double h = (mp.maxalt-mp.minalt)/2.0;
    double vb = Vertical::Theta_H(sz, vz, -1, h);
    double vt = Vertical::Theta_H(sz, vz, +1, h);
    b = Util::max(B, vb);
    t = Util::min(T, vt);
  } else if (so.z < mp.minalt || so.z > mp.maxalt) {
    return ret;
  }

  if (t < b) {
    return ret; // no vertical conflict possible
  }
  MovingPolygon3D mp2 = mp;
//  if (!PolycarpAcceptablePolygon::counterclockwise_edges(mp.horizpoly.polystart)) {
  if (checkNice && !PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
    mp2 = mp.reverseOrder();
    if (!PolycarpContain::nice_polygon_2D(mp2.horizpoly.polystart, buff)) {
      fpln("WARNING: Polycarp3D.conflictTimes: NOT A NICE POLYGON!");
      return ret;
    }
    if (!mp2.isStable() && !PolycarpDetection::nice_moving_polygon_2D(b, t, mp2.horizpoly, buff, fac)) {
        fpln("WARNING: Polycarp3D.detection: NOT A NICE MOVING POLYGON!");
        return ret;
    }
  }
  std::vector<double> times = PolycarpDetection::collisionTimesInOut(b, t, mp2.horizpoly, so.vect2(), vo.vect2(), buff, fac);
  for (int i = 0; i < (int) times.size()-1; i++) {
    double t1 = times[i];
    double t2 = times[i+1];
    double mid = (t1+t2)/2;
    bool addInterval;
    if (avoid) {
        addInterval = !definitely_outside(so.AddScal(mid, vo), mp2.position(mid), buff, false); // was already checked nice above
    } else {
        addInterval = !definitely_inside(so.AddScal(mid, vo), mp2.position(mid), buff, false); // was already checked nice above
    }
    if (addInterval) {
        ret.unions(Interval(t1, t2));
    }
  }
  return ret;

}


}
