/*
 * Copyright (c) 2015-2017 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
#include "TrafficState.h"
#include "Position.h"
#include "Velocity.h"
#include "Projection.h"
#include "Constants.h"
#include "format.h"

namespace larcfm {

TrafficState::TrafficState() {
  id_ = "_NoAc_";
  pos_ = Position::INVALID();
  vel_ = Velocity::INVALIDV();
  posxyz_ = Position::INVALID();
  velxyz_ = Velocity::INVALIDV();
  eprj_ = Projection::createProjection(Position::ZERO_LL());
  time_ = 0.0;
}

const TrafficState TrafficState::INVALID = TrafficState();

const std::vector<TrafficState> TrafficState::INVALIDL = std::vector<TrafficState>();

TrafficState::TrafficState(const std::string& id, const Position& pos, const Velocity& vel,
    double time, const EuclideanProjection& eprj) {
  id_ = id;
  pos_ = pos;
  vel_ = vel;
  if (pos.isLatLon()) {
    Vect3 s = eprj.project(pos);
    Velocity v = eprj.projectVelocity(pos,vel);
    posxyz_ = Position(s);
    velxyz_ = Velocity::make(v);
  } else {
    posxyz_ = pos;
    velxyz_ = vel;
  }
  time_ = time;
  eprj_ = eprj;
}

TrafficState TrafficState::makeOwnship(const TrafficState& ac) {
  return makeOwnship(ac.id_,ac.pos_,ac.vel_,ac.time_);
}

TrafficState TrafficState::makeOwnship(const std::string& id, const Position& pos, const Velocity& vel,
    double time) {
  return TrafficState(id,pos,vel,time,pos.isLatLon()?Projection::createProjection(pos.lla().zeroAlt()):
      Projection::createProjection(Position::ZERO_LL()));
}

TrafficState TrafficState::makeIntruder(const TrafficState& ac) {
  return makeIntruder(ac.id_,ac.pos_,ac.vel_);
}

TrafficState TrafficState::makeIntruder(const std::string& id, const Position& pos, const Velocity& vel) const {
  if (pos_.isLatLon() != pos.isLatLon()) {
    return INVALID;
  }
  return TrafficState(id,pos,vel,time_,eprj_);
}

double TrafficState::getTime() const {
  return time_;
}

Vect3 const & TrafficState::get_s() const {
  return posxyz_.point();
}

Velocity const & TrafficState::get_v() const {
  return velxyz_;
}

Vect3 TrafficState::pos_to_s(const Position& p) const {
  if (p.isLatLon()) {
    if (!pos_.isLatLon()) {
      return Vect3::INVALID();
    }
    return eprj_.project(p);
  }
  return p.point();
}


Velocity TrafficState::vel_to_v(const Position& p, const Velocity& v) const {
  if (p.isLatLon()) {
    if (!pos_.isLatLon()) {
      return Velocity::INVALIDV();
    }
    return eprj_.projectVelocity(p,v);
  }
  return v;
}

Velocity TrafficState::inverseVelocity(const Velocity& v) const {
  return eprj_.inverseVelocity(get_s(),v,true);
}

TrafficState TrafficState::linearProjection(double offset) const {
  return TrafficState(getId(),pos_.linear(vel_,offset),vel_,time_+offset,eprj_);
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

std::string TrafficState::formattedHeader(const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const {
  std::string s = "";
  if (pos_.isLatLon()) {
      s += "NAME lat lon alt trk gs vs time\n";
      s += "[none] [deg] [deg] ["+ualt+"] [deg] ["+ugs+"] ["+uvs+"] [s]\n";
  } else {
      s += "NAME sx sy sz trk gs vs time\n";
      s += "[none] ["+uxy+"] ["+uxy+"] ["+ualt+"] [deg] ["+ugs+"] ["+uvs+"] [s]\n";
  }
  return s;
}

std::string TrafficState::formattedTrafficState(const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const {
  std::string s = "";
  if (pos_.isLatLon()) {
      s += getId()+", "+pos_.lla().toStringNP(ualt,Constants::get_output_precision());
  } else {
      s += getId()+", "+pos_.point().toStringNP(Constants::get_output_precision(),uxy,uxy,ualt);
  }
  s += ", "+vel_.toStringNP("deg",ugs,uvs,Constants::get_output_precision())+", "+
          FmPrecision(time_,Constants::get_output_precision())+"\n";
  return s;
}

std::string TrafficState::formattedTrafficList(const std::vector<TrafficState>& traffic,
    const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) {
  std::string s = "";
  for (nat i=0; i < traffic.size(); ++i) {
      TrafficState ac = traffic[i];
      s += ac.formattedTrafficState(uxy, ualt, ugs, uvs);
  }
  return s;
}

std::string TrafficState::formattedTraffic(const std::vector<TrafficState>& traffic,
    const std::string& uxy, const std::string& ualt, const std::string&  ugs, const std::string& uvs) const {
  std::string s = "";
  s += formattedHeader(uxy,ualt,ugs,uvs);
  s += formattedTrafficState(uxy,ualt,ugs,uvs);
  s += formattedTrafficList(traffic,uxy,ualt,ugs,uvs);
  return s;
}

std::string TrafficState::toPVS(int prec) const {
  return "(# id := \"" + getId() + "\", s := "+get_s().toPVS(prec)+", v := "+get_v().toPVS(prec)+" #)";
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

bool TrafficState::isValid() const {
  return !pos_.isInvalid() && !vel_.isInvalid();
}

bool TrafficState::isLatLon() const {
  return pos_.isLatLon();
}

std::string TrafficState::getId() const {
  return id_;
}

Position const & TrafficState::getPosition() const {
  return pos_;
}

Velocity const & TrafficState::getVelocity() const {
  return vel_;
}

Position const & TrafficState::getPositionXYZ() const {
  return posxyz_;
}

Velocity const & TrafficState::getVelocityXYZ() const {
  return velxyz_;
}


/**
 *  Returns current track in internal units [0 - 2pi] [rad] (clock wise with respect to North)
 */
double TrafficState::track() const {
  return vel_.compassAngle();
}

/**
 *  Returns current track in given units [0 - 2pi] [u] (clock wise with respect to North)
 */
double TrafficState::track(const std::string& utrk) const {
  return vel_.compassAngle(utrk);
}

/**
 * Returns current ground speed in internal units
 */
double TrafficState::groundSpeed() const {
  return vel_.gs();
}

/**
 * Returns current ground speed in given units
 */
double TrafficState::groundSpeed(const std::string& ugs) const {
  return vel_.groundSpeed(ugs);
}

/**
 * Returns current vertical speed in internal units
 */
double TrafficState::verticalSpeed() const {
  return vel_.vs();
}

/**
 * Returns current vertical speed in given units
 */
double TrafficState::verticalSpeed(const std::string& uvs) const {
  return vel_.verticalSpeed(uvs);
}

/**
 * Returns current altitude in internal units
 */
double TrafficState::altitude() const{
  return pos_.alt();
}

/**
 * Returns current altitude in given units
 */
double TrafficState::altitude(const std::string& ualt) const {
  return Units::to(ualt,pos_.alt());
}

bool TrafficState::sameId(const TrafficState& ac) const {
  return isValid() && ac.isValid() && id_ == ac.id_;
}

std::string TrafficState::toString() const {
  return "("+id_+", "+pos_.toString()+", "+vel_.toString()+")";
}

}
