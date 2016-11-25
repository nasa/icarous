/*
 * Copyright (c) 2015-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "TrafficState.h"
#include "Position.h"
#include "Velocity.h"
#include "Projection.h"
#include "format.h"

namespace larcfm {

TrafficState::TrafficState() {
  s_ = Vect3::INVALID();
  v_ = Velocity::INVALIDV();
  eprj_ = Projection::createProjection(Position::ZERO_LL());
}

const TrafficState TrafficState::INVALID = TrafficState();

const std::vector<TrafficState> TrafficState::INVALIDL = std::vector<TrafficState>();

TrafficState::TrafficState(const std::string& id, const Position& pos, const Velocity& vel, const EuclideanProjection& eprj) : TrafficCoreState(id,pos,vel) {
  if (isLatLon()) {
    s_ = eprj.project(pos);
    v_ = eprj.projectVelocity(pos,vel);
  } else {
    s_ = pos.point();
    v_ = vel;
  }
  eprj_ = eprj;
}

TrafficState TrafficState::makeOwnship(const std::string& id, const Position& pos, const Velocity& vel) {
  return TrafficState(id,pos,vel,pos.isLatLon()?Projection::createProjection(pos.lla().zeroAlt()):
      Projection::createProjection(Position::ZERO_LL()));
}

TrafficState TrafficState::makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const {
  if (isLatLon() != pos.isLatLon()) { // ||
    //        GreatCircle.distance(pos.lla(),getPosition().lla()) > eprj_.maxRange()) {
    //      Thread.dumpStack();
    return INVALID;
  }
  return TrafficState(id,pos,vel,get_eprj());
}

Vect3 const & TrafficState::get_s() const {
  return s_;
}

Velocity const & TrafficState::get_v() const {
  return v_;
}

EuclideanProjection const & TrafficState::get_eprj() const {
  return eprj_;
}

Vect3 TrafficState::pos_to_s(const Position& p) const {
  if (p.isLatLon()) {
    if (!getPosition().isLatLon()) {
      return Vect3::INVALID();
    }
    return eprj_.project(p);
  }
  return p.point();
}


Velocity TrafficState::vel_to_v(const Position& p, const Velocity& v) const {
  if (p.isLatLon()) {
    if (!getPosition().isLatLon()) {
      return Velocity::INVALIDV();
    }
    return eprj_.projectVelocity(p,v);
  }
  return v;
}

Velocity TrafficState::inverseVelocity(const Velocity& v) const {
  return eprj_.inverseVelocity(s_,v,true);
}

TrafficState TrafficState::linearProjection(double offset) const {
  return TrafficState(getId(),getPosition().linear(getVelocity(),offset),getVelocity(),eprj_);
}

TrafficState TrafficState::findAircraft(const std::vector<TrafficState>& traffic, const std::string& id) {
  if (id != TrafficState::INVALID.getId()) {
    for (nat i=0; i < traffic.size(); ++i) {
      TrafficState ac = traffic[i];
      if (id == ac.getId())
        return ac;
    }
  }
  return TrafficState::INVALID;
}

std::string TrafficState::listToString(const std::vector<TrafficState>& traffic) {
  std::string s = "{";
  bool comma = false;
  for (nat i=0; i < traffic.size(); ++i) {
    if (comma) {
      s += ", ";
    } else {
      comma = true;
    }
    s += traffic[i].getId();
  }
  return s+"}";
}

std::string TrafficState::formattedTraffic(const std::vector<TrafficState>& traffic, double time) const {
  std::string s="";
  if (isLatLon()) {
    s += "NAME lat lon alt trk gs vs time\n";
    s += "[none] [deg] [deg] [ft] [deg] [knot] [fpm] [s]\n";
  } else {
    s += "NAME sx sy sz trk gs vs time\n";
    s += "[none] [NM] [NM] [ft] [deg] [knot] [fpm] [s]\n";
  }
  s += getId()+", "+getPosition().toStringNP()+", "+getVelocity().toStringNP()+", "+
      Fm1(time)+"\n";

  for (nat i = 0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    s += ac.getId()+", "+ac.getPosition().toStringNP()+", "+ac.getVelocity().toStringNP()+", "+
        Fm1(time)+"\n";
  }
  return s;
}

std::string TrafficState::toPVS(int prec) const {
  return "(# id := \"" + getId() + "\", s := "+s_.toPVS(prec)+", v := "+v_.toPVS(prec)+" #)";
}

std::string TrafficState::listToPVSAircraftList(const std::vector<TrafficState>& traffic, int prec) const {
  std::string s="";
  s += "(: ";
  s += toPVS(prec);
  for (nat i = 0; i < traffic.size(); ++i) {
    TrafficState ac = traffic[i];
    s += ", ";
    s += ac.toPVS(prec);
  }
  return s+" :)";
}

std::string TrafficState::listToPVSStringList(const std::vector<TrafficState>& traffic, int prec) {
  if (traffic.empty()) {
    return "null[string]";
  } else {
    std::string s = "(:";
    bool comma = false;
    for (nat i = 0; i < traffic.size(); ++i) {
      TrafficState ac = traffic[i];
      if (comma) {
        s += ", ";
      } else {
        s += " ";
        comma = true;
      }
      s += "\"" + ac.getId() + "\"";
    }
    return s+" :)";
  }
}

}
